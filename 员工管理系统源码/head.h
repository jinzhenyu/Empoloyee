#ifndef __HEAD_H__
#define __HEAD_H__

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sqlite3.h>
#include <time.h>

#define N 128

struct worker_info_t {
    char name[N];
    char sex[N];
    int age;
    int id;
    char section[N];
    int salary;
    int sign_in;
    int performance;
    char password;
};

struct login_info_t {
    char name[N];
    char password[N];
    int flags;
};

struct select {
    int id;
    int num;
    int age;
    char o_name[N];
    char n_name[N];
};
#endif
