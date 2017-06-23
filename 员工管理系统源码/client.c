#include "head.h"
struct worker_info_t worker;//员工信息结构体
struct login_info_t login;//登录信息结构体
int len_worker = sizeof(worker);
int len_login = sizeof(login);
struct select select_t;
int sockfd;
char buf[N] = {};
char func[N];
/*******************************************************
 *登录函数
 *函数功能：从客户端获取用户名和密码，将其封装进login结构体
 发送到服务器进行验证，并判断用户权限：
 root用户：login.flags = 1;
 普通用户：login.flags = 0;
 登录失败：login.flags = -1;
 *参数：sockfd；套接字描述符
 *返回值：0 登录成功  
 *        -1 登录失败； 
 * *****************************************************/
int do_login(int sockfd)
{
    char name_input[N] = {};
    char password_input[N] = {};
    printf("please input your name:");
    scanf("%s",name_input);
    printf("please input your password:");
    scanf("%s",password_input);
    strcpy(login.name,name_input);
    strcpy(login.password,password_input);
    if(send(sockfd,(void *)&login, len_login, 0) <= 0)
    {
        perror("fail to send login information\n");
        return -1;
    }
    if(recv(sockfd,(void *)&login, len_login, 0) <= 0)
    {
        perror("fail to recv login_flags\n");
        return -1;
    }
    return 0;
}
int do_insert(void)
{
    strcpy(func, "root_insert");
    send(sockfd,func, N, 0);

    printf("input name : ");
    scanf("%s", worker.name);
    printf("input sex : ");
    scanf("%s", worker.sex);
    printf("input age : ");
    scanf("%d", &(worker.age));
    printf("input id : ");
    scanf("%d", &(worker.id));
    printf("input section : ");
    scanf("%s", worker.section);
    printf("input salary : ");
    scanf("%d", &(worker.salary));
    printf("input sign_in : ");
    scanf("%d", &(worker.sign_in));
    printf("input performance : ");
    scanf("%d", &(worker.performance));
    if(send(sockfd, (void *)&worker, len_worker, 0) <= 0)
    {
        perror("fail to send worker message!\n");
        return -1;
    }

    printf("input password : ");
    scanf("%s", login.password);
    strcpy(login.name, worker.name);
    if(send(sockfd, (void *)&login, len_login, 0) <= 0)
    {
        perror("fail to send login message!\n");
        return -1;
    }

    bzero(buf, N);
    if(recv(sockfd, buf, N, 0) <= 0)
    {
        perror("fail to recv insert result!\n");
        return -1;
    }
    printf("%s\n",buf);
    return 0;
}
int do_delete(void)
{
    strcpy(func, "root_delete");
    send(sockfd,func, N, 0);

    printf("input the name you want delete: ");
    scanf("%s", buf);
    if(send(sockfd, buf, N, 0) <= 0)
    {
        perror("fail to send delete message!\n");
        return -1;
    }
    bzero(buf, N);
    if(recv(sockfd, buf, N, 0) <= 0)
    {
        perror("fail to recv delete result!\n");
        return -1;
    }
    printf("%s\n",buf);

    return 0;
}

int do_root_update(void)
{
    strcpy(func, "root_update");
    send(sockfd,func, N, 0); 

	printf("**************** please select *****************\n");
	printf(" 1.update id. 2.update name. 3.update age.\n");
	printf("************************************************\n");
	printf("please input num ；");
	scanf("%d",&select_t.num);
	switch (select_t.num)
	{
	case 1 :
		printf("input new_id and old_name : ");
		scanf ("%d %s", &select_t.id, select_t.o_name);
		break;
	case 2 :
		printf("input new_name and old_name : ");
		scanf ("%s %s", select_t.n_name, select_t.o_name);
		break;			
	case 3 :
		printf("input new_age and old_name : ");
		scanf ("%d %s", &select_t.age, select_t.o_name);
		break;
	}
	if(send(sockfd,(void *)&select_t, sizeof(select_t), 0) <= 0)
    {
        perror("fail to send num information\n");
        return -1;
    }
    bzero(buf, N);
    if(recv(sockfd, buf, N, 0) <= 0)
    {
        perror("fail to recv root_update result!\n");
        return -1;
    }
    printf("%s\n",buf);

    return 0;
}
int do_root_find(void)
{
    int num;
    strcpy(func, "root_find");
    send(sockfd,func, N, 0); 
#if 1
    bzero(buf, N);
    printf("input the name you want to find:\n");
    scanf("%s",buf);
    send(sockfd,buf, N, 0); 
#endif
    if(recv(sockfd, (void *)&worker, len_worker, 0) <= 0)
    {
        perror("fail to recv root_find information!\n");
        return -1;
    }
    printf("name sex age id section salary signin performance\n");
    printf("%-5s",worker.name);
    printf("%-5s",worker.sex);
    printf("%-5d",worker.age);
    printf("%-5d",worker.id);
    printf("%-5s",worker.section);
    printf("%-5d",worker.salary);
    printf("%-5d",worker.sign_in);
    printf("%-5d\n",worker.performance);

    return 0;
}
/*
 * 查看日志
 */
int do_root_history(void)
{
    strcpy(func, "root_history");
    send(sockfd,func, N, 0); 
	
    char buf[32] = {0};
	int ret;
	strcpy(func, "history");
	send(sockfd,func, N, 0);
	if(send(sockfd, (void *)&worker, len_worker, 0) < 0)
	{
		perror("fail to send show_history worker");
		return -1;
	}

	while((ret = recv(sockfd, buf, 31, 0)) > 0)
	{
		if(strncmp(buf,"end",3) == 0)
		{
			break;
		}
		printf("%s",buf);
	}
    return 0;
}

int do_user_update(void)
{
    strcpy(func, "user_update");
    send(sockfd,func, N, 0); 

	strcpy(select_t.o_name, login.name);
	printf("**************** please select *****************\n");
	printf(" 1.update id. 2.update password. 3.update age.\n");
	printf("************************************************\n");
	printf("please input num ；");
	scanf("%d",&select_t.num);
	switch (select_t.num)
	{
	case 1 :
		printf("input new_id : ");
		scanf ("%d", &select_t.id);
		break;
	case 2 :
		printf("input new_password : ");
		scanf ("%s", select_t.n_name);
		break;			
	case 3 :
		printf("input new_age : ");
		scanf ("%d", &select_t.age);
		break;
	}
	if(send(sockfd,(void *)&select_t, sizeof(select_t), 0) <= 0)
    {
        perror("fail to send num information\n");
        return -1;
    }
    bzero(buf, N);
    if(recv(sockfd, buf, N, 0) <= 0)
    {
        perror("fail to recv user_update result!\n");
        return -1;
    }
    printf("%s\n",buf);
    return 0;
}

int do_user_find(void)
{
    strcpy(func, "user_find");
    send(sockfd,func, N, 0); 

    if(recv(sockfd, (void *)&worker, len_worker, 0) <= 0)
    {
        perror("fail to recv user_find result!\n");
        return -1;
    }
    printf("name sex age id section salary signin performance\n");
    printf("%-5s",worker.name);
    printf("%-5s",worker.sex);
    printf("%-5d",worker.age);
    printf("%-5d",worker.id);
    printf("%-5s",worker.section);
    printf("%-5d",worker.salary);
    printf("%-5d",worker.sign_in);
    printf("%-5d\n",worker.performance);

    return 0;
}

int do_user_sign_in(void)
{
    strcpy(func, "user_sign_in");
    send(sockfd,func, N, 0); 
    
	time_t tm;
	struct tm *mytm;
	time(&tm);  //获取秒数
	mytm = localtime(&tm);//将描述转化为年月日十分秒
	
    /**将早上8:30-8:59规定为签到时间**/
    if((mytm->tm_hour == 8) && ((mytm->tm_min >=30) && (mytm->tm_min <= 59)))
    {
        worker.sign_in = 1;
    }
    else
    {
        worker.sign_in = 0;
    }
	if(send(sockfd,(void *)&worker, len_worker, 0) <= 0)
    {
        perror("fail to send signin information\n");
        return -1;
    }
    bzero(buf, N);
    if(recv(sockfd, buf, N, 0) <= 0)
    {
        perror("fail to recv signin result!\n");
        return -1;
    }
    printf("%s\n",buf);
    
    return 0;
}

int do_quit(void)
{
    strcpy(func, "quit");
    send(sockfd,func, N, 0); 
#if 0
    bzero(buf, N);
    if(recv(sockfd, buf, N, 0) <= 0)
    {
        perror("fail to recv quit result!\n");
        return -1;
    }
    printf("%s\n",buf);
#endif
    exit(0);
}

/******************************************
 *用户界面选择函数
 *函数功能：根据不同的权限选择不同的用户界面
 *参数：无
 *返回值：无
 * *****************************************/
int choose_interface(void)
{
    char op;
    
    while(1)
    {
        if(login.flags == 1)
        {
            printf("login success! you are a root user!\n");
            printf("**************************************\n");
            printf("*       1 ---> insert a user         *\n");
            printf("*       2 ---> delete a user         *\n");
            printf("*       3 ---> update a user         *\n");
            printf("*       4 ---> find  a  user         *\n");
            printf("*       5 ---> history               *\n");
            printf("*       6 ---> quit                  *\n");
            printf("**************************************\n");
            printf("choose your operation:\n");
            getchar();
            op = getchar();
            switch(op)
            {
                case'1':
                    printf("client: excute root_insert function!\n");
                    do_insert();
                    break;
                case'2':
                    printf("client: excute root_delete function!\n");
                    do_delete();
                    break;
                case'3':
                    printf("client: excute root_update function!\n");
                    do_root_update();
                    break;
                case'4':
                    printf("client: excute root_find function!\n");
                    do_root_find();
                    break;
                case'5':
                    printf("client: excute root_history function!\n");
                    do_root_history();
                    break;
                case'6':
                    printf("client: excute root_quit function!\n");
                    do_quit();
                    break;
                default:
                    return -1;
            }
        }
        else if(login.flags ==0)
        {
            printf("login success! you are a normal user!\n");
            printf("**************************************\n");
            printf("*       1 ---> find my information   *\n");
            printf("*       2 ---> update my information *\n");
            printf("*       3 ---> sign in               *\n");
            printf("*       4 ---> quit                  *\n");
            printf("**************************************\n");
            printf("choose your operation:\n");
            getchar();
            op = getchar();
            switch(op)
            {
                case'1':
                    printf("client: excute user_find function!\n");
                    do_user_find();
                    break;
                case'2':
                    printf("client: excute user_update function!\n");
                    do_user_update();
                    break;
                case'3':
                    printf("client: excute user_sign_in function!\n");
                    do_user_sign_in();
                    break;
                case'4':
                    printf("client: excute user_quit function!\n");
                    do_quit();
                    break;
                default:
                    return -1;
            }
        }
        else if(login.flags == -1)
        {
            printf("this user doesn't exist,please input again!\n");
            return -1;
        }
        else
        {
            printf("invalid login flags\n");
            return -1;
        }
    }
    return 0;
}
/********************************
 *主函数
 *函数功能：初始化客户端的网络连接
 * *******************************/
int main(int argc, const char *argv[])
{
    struct sockaddr_in serveraddr;
    int len = sizeof(serveraddr);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("fail to socket");
        exit(1);
    }
    bzero(&serveraddr,len);
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(atoi(argv[2]));
    serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
    //向服务器发送链接请求
    if (connect(sockfd,(struct sockaddr*)&serveraddr, len) < 0)	
    {
        perror("fail to connect");
        exit(1);
    }
    //用户登录
login:
    if(do_login(sockfd) < 0)
    {
        printf("fail to login\n");
        goto login;
    }
    //选择用户界面
    choose_interface();
    while(1);
    return 0;
}
