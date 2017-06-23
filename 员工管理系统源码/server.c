#include "head.h"

struct worker_info_t worker;
struct login_info_t login;//从客户端接收的用户名和密码
struct login_info_t login_root = {//自定义的root用户的用户名和密码
    .name = "zhangsan",
    .password = "123"
};

int sockfd, acceptfd;
int len_login = sizeof(login);
int len_worker = sizeof(worker);
struct select select_t;
char buf[N] = {};
char op[N] = {};
sqlite3 *db;

/*登录验证函数
 *函数功能：检查来自客户端的用户名及密码是否与数据库中的匹配
 *          匹配之后确定其权限。
 *          login.flags = 1 root用户
 *          login.flags = 0 普通用户
 *          login.flags = -1 用户名或密码错误
 *函数参数：无
 *函数返回值：0 
 * */
int login_check(void)
{
    /*验证是否为root用户*/
    if((strcmp(login.name,login_root.name)==0) && (strcmp(login.password,login_root.password)==0))
    {
        login.flags = 1;
        if(send(acceptfd, (void *)&login, len_login, 0) <= 0)
        {
            perror("fail to send login_flags\n");
            exit(-1);
        }
        return 0;
    }

    /*验证是否为普通用户，用户名和密码需要从数据库中读取*/
    char *errmsg;
    char **result;
    int row, column;
    char sqlstr[128];
    sprintf(sqlstr,"select password from login where name='%s'",login.name);

    if(sqlite3_get_table(db, sqlstr, &result, &row, &column, &errmsg) != SQLITE_OK)
    {
        printf("%s\n", errmsg);
    }

    if(strcmp(login.password,result[1]) == 0)
    {
        login.flags = 0;
        if(send(acceptfd, (void *)&login, len_login, 0) <= 0)
        {
            perror("fail to send login_flags\n");
            exit(-1);
        }
    }

    /*用户名及密码与数据库不匹配，重新登录*/
    else 
    {
        login.flags = -1;
        if(send(acceptfd, (void *)&login, len_login, 0) <= 0)
        {
            perror("fail to send login_flags\n");
            exit(-1);
        }
    }
    
    return 0;
}

int do_insert(sqlite3 *db)
{

    if(recv(acceptfd, (void *)&worker, len_worker, 0) <= 0)
    {	
		perror("fail to receive worker information\n");
    	exit(-1);
	}

    if(recv(acceptfd, (void *)&login, len_login, 0) <= 0)
    {	
		perror("fail to receive login information\n");
    	exit(-1);
	}

    char sqlstr1[128],sqlstr2[128], *errmsg;
    bzero(buf,N);
    sprintf(sqlstr1, "insert into empoloyee values ('%s', '%s', %d, %d, '%s', %d, %d, %d)", worker.name, worker.sex, worker.age, worker.id, worker.section, worker.salary, worker.sign_in, worker.performance);
    sprintf(sqlstr2, "insert into login values ('%s', '%s')",login.name,login.password); 
    if (sqlite3_exec(db, sqlstr1, NULL, NULL, &errmsg) != 0)
    {
        strcpy(buf, "insert failed!");
        send(acceptfd, buf, N, 0);
        printf("error : %s\n", sqlite3_errmsg(db));
    }
    if (sqlite3_exec(db, sqlstr2, NULL, NULL, &errmsg) != 0)
    {
        strcpy(buf, "insert failed!");
        send(acceptfd, buf, N, 0);
        printf("error : %s\n", sqlite3_errmsg(db));
    }
    else
    {
        strcpy(buf, "insert ok!\n");
        send(acceptfd, buf, N, 0);
        printf("%s\n", buf);
    }
    return 0;
}
int do_delete(sqlite3 *db)
{
    bzero(buf,N);
    if(recv(acceptfd, buf, N, 0) <= 0)
    {	
		perror("fail to receive delete information\n");
    	exit(-1);
	}

    char *errmsg;
    char sqlstr[128];
    sprintf(sqlstr, "delete from empoloyee where name='%s'", buf);
    if (sqlite3_exec(db, sqlstr, NULL, NULL, &errmsg) != 0)
    {
        bzero(buf,N);
        strcpy(buf,"delete failed!\n");
        send(acceptfd,buf, N, 0);
        printf("error : %s\n", sqlite3_errmsg(db));
    }
    else 
    {
        bzero(buf,N);
        strcpy(buf,"delete ok!\n");
        send(acceptfd, buf, N, 0);
        printf("%s\n", buf);
    }

    return 0;
}

int do_root_update(sqlite3 *db)
{
	int num  = 0, new_num, new_score;
	if(recv(acceptfd, (void *)&select_t, sizeof(select_t), 0) <= 0)
	{
		perror("fail to receive\n");
		exit(-1);
	}
	char *errmsg;
	char sqlstr[N];
	char update_date[N];
	switch (select_t.num)
	{
	case 1 :
		sprintf(update_date,"id = %d", select_t.id);
		sprintf(sqlstr, "update empoloyee set %s where name = '%s'", update_date, select_t.o_name);
		break;
	case 2 :
		sprintf(update_date,"name = '%s'", select_t.n_name);
		sprintf(sqlstr, "update empoloyee set %s where name = '%s'", update_date, select_t.o_name);
		break;			
	case 3 :
		sprintf(update_date,"age = %d", select_t.age);
		sprintf(sqlstr, "update empoloyee set %s where name = '%s'", update_date, select_t.o_name);
		break;
	}
	if (sqlite3_exec(db, sqlstr, NULL, NULL, &errmsg) != 0)
	{
        bzero(buf, N);
        strcpy(buf, "root_update failed!");
        send(acceptfd, buf, N, 0);
		printf("error : %s\n", sqlite3_errmsg(db));
	}
	else 
    {
        bzero(buf, N);
        strcpy(buf, "root_update ok!");
        send(acceptfd, buf, N, 0);
		printf("root_update is done\n");
    }
	return 0;
    
}

int do_root_find(sqlite3 *db)
{
    #if 1
    bzero(buf, N);
	if(recv(acceptfd, buf, N, 0) <= 0)
	{
		perror("fail to receive root_find information!\n");
		exit(-1);
	}
    printf("%s\n",buf);
    #endif

    char *errmsg;
    char **result;
    char sqlstr[128] = {};
    int row, column, i, j, index;

    sprintf(sqlstr,"select * from empoloyee where name='%s'",buf);
    if(sqlite3_get_table(db, sqlstr, &result, &row, &column, &errmsg) != SQLITE_OK)
    {
        printf("%s\n", errmsg);
    }
    index = column;

    for(i = 0; i < column; i++)
    {
        printf("%-10s", result[i]);
    }
    putchar(10);
    for(j = 0; j < row; j++)
    {
        for(i = 0; i < column; i++)
        {
            printf("%-10s", result[index++]);
        }

        putchar(10);
    }
    strcpy(worker.name, result[column]);
    strcpy(worker.sex, result[column+1]);
    worker.age = *result[column+2];
    worker.id = *result[column+3];
    strcpy(worker.section, result[column+4]);
    worker.salary = *result[column+5];
    worker.sign_in = *result[column+6];
    worker.performance = *result[column+7];

	if(send(acceptfd, (void *)&worker, len_worker, 0) <= 0)
	{
		perror("fail to send root_find information!\n");
		exit(-1);
	}
	printf("root_find ok!\n");
    return 0;
}
/*
 *  发送日志
 */
int do_root_history()
{
	int fd, nbyte;
	char buf[32] = {0};
	if((fd = open("./history.txt", O_RDONLY)) < 0)
	{
		perror("fail to open history.txt");
		return -1;
	}
	while((nbyte = read(fd, buf, 31)) > 0)
	{
		send(acceptfd, buf, 31, 0);
	}
    bzero(buf, N);
	memset(buf,0,sizeof(buf));
	strcpy(buf,"end");
	send(acceptfd, buf, 31, 0);
}

int do_log(void)
{
	time_t tm;
	struct tm *mytm;
	time(&tm);  //获取秒数
	mytm = localtime(&tm);//将描述转化为年月日十分秒
	
	FILE *fd_history;


	fd_history = fopen("./history.txt","a+");
	if(fd_history == NULL)
	{
		perror("fail to open history.txt");
		return -1;
	}
	//打印操作用户及操作方式。
	fprintf(fd_history,"%s,%s,",login.name , op);
	fprintf(fd_history,"%s,%d,%s,%d,%s,%d,%d,%d,",worker.name, worker.age, worker.sex, worker.id, worker.section, worker.salary, worker.sign_in, worker.performance);
	//打印时间
	fprintf(fd_history,"%d-%d-%d  %d:%d:%d\n",mytm->tm_year+1900, mytm->tm_mon+1, mytm->tm_mday,
			mytm->tm_hour, mytm->tm_min, mytm->tm_sec);
	fflush(fd_history);	
	return 0;
}

int do_user_update(sqlite3 *db)
{
	int num  = 0, new_num, new_score;
	if(recv(acceptfd, (void *)&select_t, sizeof(select_t), 0) <= 0)
	{
		perror("fail to receive\n");
		exit(-1);
	}
	char *errmsg;
	char sqlstr[N];
	char update_date[N];
	switch (select_t.num)
	{
	case 1 :
		sprintf(update_date,"id = %d", select_t.id);
		sprintf(sqlstr, "update empoloyee set %s where name = '%s'", update_date, select_t.o_name);
		break;
	case 2 :
		sprintf(update_date,"password = '%s'", select_t.n_name);
		sprintf(sqlstr, "update login set %s where name = '%s'", update_date, select_t.o_name);
		break;			
	case 3 : 
		sprintf(update_date,"age = %d", select_t.age);
		sprintf(sqlstr, "update empoloyee set %s where name = '%s'", update_date, select_t.o_name);
		break;
	}
	if (sqlite3_exec(db, sqlstr, NULL, NULL, &errmsg) != 0)
	{
        bzero(buf, N);
        strcpy(buf, "root_update failed!");
        send(acceptfd, buf, N, 0);
		printf("error : %s\n", sqlite3_errmsg(db));
	}
	else 
    {
        bzero(buf, N);
        strcpy(buf, "root_update ok!");
        send(acceptfd, buf, N, 0);
		printf("root_update is done\n");
	}
    return 0;
}

int do_user_find(sqlite3 *db)
{
    char *errmsg;
    char **result;
    char sqlstr[128] = {};
    int row, column, i, index;

    sprintf(sqlstr,"select * from empoloyee where name='%s'",login.name);
    if(sqlite3_get_table(db, sqlstr, &result, &row, &column, &errmsg) != SQLITE_OK)
    {
        printf("%s\n", errmsg);
    }

    index = column;

    for(i = 0; i < column; i++)
    {
        printf("%-10s", result[i]);
    }
    putchar(10);
    for(i = 0; i < column; i++)
    {
        printf("%-10s", result[index++]);
    }            
    putchar(10);

    strcpy(worker.name, result[column]);
    strcpy(worker.sex, result[column+1]);
    worker.age = *result[column+2];
    worker.id = *result[column+3];
    strcpy(worker.section, result[column+4]);
    worker.salary = *result[column+5];
    worker.sign_in = *result[column+6];
    worker.performance = *result[column+7];
	if(send(acceptfd, (void *)&worker, len_worker, 0) <= 0)
	{
		perror("fail to send user_find information!\n");
		exit(-1);
	}
	printf("user_find ok!\n");
    return 0;

}

int do_user_sign_in(sqlite3 *db)
{
    if(recv(acceptfd, (void *)&worker, len_worker, 0) <= 0)
    {	
		perror("fail to receive worker information\n");
    	exit(-1);
	}

	char *errmsg;
	char sqlstr[128];

	sprintf(sqlstr, "update empoloyee set signin=%d where name = '%s'", worker.sign_in, worker.name);
	if (sqlite3_exec(db, sqlstr, NULL, NULL, &errmsg) != 0)
	{
        bzero(buf, N);
        strcpy(buf, "sign_in failed!");
        send(acceptfd, buf, N, 0);
		printf("error : %s\n", sqlite3_errmsg(db));
	}
	else 
    {
        bzero(buf, N);
        strcpy(buf, "sign_in ok!");
        send(acceptfd, buf, N, 0);
		printf("%s\n",buf);
    }
    return 0;
}

void do_quit(void)
{
    //pthread_exit("pthread_exit\n");
    //printf("exit succeed!\n");
    return;
}

/*功能选择函数
 *函数功能：接收客户端发来的操作请求，根据不同的操作请求调用相应的函数
 *函数参数：无
 *函数返回值：0
 * */
int function_select(void)
{
    while(1)
    {
        if(recv(acceptfd, op, N, 0) <= 0)
	    {
		    perror("fail to receive op\n");
    		exit(-1);
	    }
        //root用户的功能
        if(strncmp(op,"root_insert",11) == 0)
        {
            printf("server:excute root_insert function\n");
            do_insert(db);
        }
        if(strncmp(op,"root_delete",11) == 0)
        {
            printf("server:excute root_delete function\n");
            do_delete(db);
        }
        if(strncmp(op,"root_update",11) == 0)
        {
            printf("server:excute root_update function\n");
            do_root_update(db);
        }
        if(strncmp(op,"root_find",9) == 0)
        {
            printf("server:excute root_find function\n");
            do_root_find(db);
        }
        if(strncmp(op,"root_history",12) == 0)
        {
            printf("server:excute history function\n");
            do_root_history();
        }
        //普通用户的功能
        if(strncmp(op,"user_update",11) == 0)
        {
            printf("server:excute user_update function\n");
            do_user_update(db);
        }
        if(strncmp(op,"user_find",9) == 0)
        {
            printf("server:excute user_find function\n");
            do_user_find(db);
        }
        if(strncmp(op,"user_sign_in",12) == 0)
        {
            printf("server:excute user_sign_in function\n");
            do_user_sign_in(db);
        }
        if(strncmp(op,"quit",4) == 0)
        {
            printf("server:excute quit function\n");
            do_quit();
            printf("quit ok!\n");
            break;
        }
        do_log();
    }
    return 0;
}

/*线程回调函数
 *函数功能：接收来自客户端的登录信息结构体：login
 *          然后调用登录信息检查函数login_check()对登录信息进行检查
 *          检查无误后调用function_select()函数进行相应功能的选择
 *
 * */
void *pthread_fun(void *arg)
{
	if (sqlite3_open("empoloyee.db", &db) < 0)
	{
		printf("fail to sqlite3_open : %s\n", sqlite3_errmsg(db));
		exit(-1);
	}
    if(recv(acceptfd, (void *)&login, len_login, 0) <= 0)
	{
		perror("fail to receive\n");
		exit(-1);
	}
    printf("server:login_name:%s login_password:%s\n",login.name,login.password);

    login_check();

    function_select();

}

/*服务器端主程序
 * 函数功能：实现网络链接的初始化，客户端每发送一个连接请求
 *           就创建一个线程，由此实现并发；
 * */
int main(int argc, const char *argv[])
{
	struct sockaddr_in serveraddr, clientaddr;
	int len = sizeof(serveraddr);
    int len_worker = sizeof(worker);
    pthread_t tid;//线程标识符

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		perror("fail to socket");
		exit(-1);
	}

	bzero(&serveraddr,len);//将地址结构中的成员置零

	serveraddr.sin_family = AF_INET;//ipv4
	serveraddr.sin_port = htons(atoi(argv[2]));
	serveraddr.sin_addr.s_addr = inet_addr(argv[1]);

	if (bind(sockfd, (struct sockaddr*)&serveraddr, len) < 0)
	{
		perror("fail to bind");
		exit(-1);
	}

	if (listen(sockfd, 5) < 0)
	{
		perror("fail to listen");
		exit(-1);
	}

	while(1)
    {
	    acceptfd = accept(sockfd, (struct sockaddr*)&clientaddr,&len);
	    if (acceptfd < 0)
	    {
		    perror("fail to accept");
		    exit(-1);
	    }

	    printf("addr = %s, port = %d \n", inet_ntoa(clientaddr.sin_addr),ntohs(clientaddr.sin_port));
        
        if(pthread_create(&tid,NULL,pthread_fun,NULL) !=0)
        {
            perror("fail to pthread_create!\n");
            exit(-1);
        }
    }
	return 0;
}
