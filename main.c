#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "src/phreadTest.h"

#define N 1
#define PIPE_PATH "/root/work/demoA/"

pthread_t tid[3];
int sum = 0;
pthread_mutex_t sum_lock = PTHREAD_MUTEX_INITIALIZER; /* 静态初始化互斥量 */
pthread_cond_t cond_sum_ready = PTHREAD_COND_INITIALIZER; /* 静态初始化条件变量 */

typedef struct edge_node_t
{
    const char * name;
    const char * value;
} edge_node;

int generate_vrf_json(int count,edge_node *edge_nodes){
    for (int m = 0; m < count; m++) {
        printf("\n name: %s ",(edge_nodes+m)->name);
        printf("\n value: %s ",(edge_nodes+m)->value);
    }
    return 0;
}

void calloc_test()
{
    int element_count = 10;
    edge_node *edge_nodes = (edge_node *) calloc(element_count, sizeof *edge_nodes);
    if (NULL == edge_nodes) {
        printf("\n 错误：edge_node内存申请失败===1！");
    }
    for (int m = 0; m < element_count; m++) {
        (edge_nodes+m)->name = "elemeKab-a";
        (edge_nodes+m)->value = "elementVal";
    }
    int rc = generate_vrf_json(element_count,edge_nodes);
    free(edge_nodes);
}

int split_str(char *text,char * sepChar,char ** res){
    char *tmp = strtok(text,sepChar);
    int x = 0;
    while (tmp != NULL){
//        res[x] = malloc(1024);
        strcpy(*res+x*6, tmp);
        printf("\n =**= %s",tmp);
        x++;
//        *res = realloc(*res, sizeof(char*)*(x+1));
        tmp = strtok(NULL,sepChar);
    }

    return x;
}

void malloc_test(){
    char text[] = "Hello world this is elliot";
    char * ptr = (char*)malloc(6 * sizeof(char*) * 6);
    int x = split_str(text, " ", &ptr);

    for (int i =0; i< x; i++){
        printf("\n %s", ptr+i*6);
    }
    free(ptr);
}

//==============================================================
int getPipePath(int number, char **path)
{
    if (asprintf(path, PIPE_PATH"sr_evpipe_%d", number) == -1) {
        return -1;
    }
    return 0;
}

void write_notify(int number) {
    char *path = NULL, buf[1] = {0};
    int fd  = -1, ret;

    if (getPipePath(number, &path) < 0) {
        printf("error get pipe path");
        return;
    }

    if ((fd = open(path, O_WRONLY|O_NONBLOCK)) == -1)
    {
        free(path);
        return;
    }
    free(path);

    do {
        ret = write(fd, buf, 1);
    } while (!ret);
    if (ret == -1) {
        printf("error write");
        return;
    }

    printf("write a notify\r\n");
    if (fd > -1) {
        close(fd);
    }
}

void do_read(int fd)
{
    int ret = 0;
    char buf[1];
    do {
        ret = read(fd, buf, 1);
    } while (ret == 1);
    if ((ret == -1) && (errno != EAGAIN)) {
        printf("read error\r\n");
        return;
    }
    printf("read ok , do something!\r\n");
}

void *pipe_listen_thread(void *arg)
{
    fd_set rfds;
    struct timeval tv;
    time_t stop_time_in = 0;
    // pipe read

    int read = *(int *)arg;
    while (1) {
        tv.tv_sec = 10;
        tv.tv_usec = 0;
        FD_ZERO(&rfds);
        FD_SET(read, &rfds);
        int ret = select(read + 1, &rfds, NULL, NULL, &tv);
        if ((ret == -1) && (errno != EINTR)) {
            break;
        } else if ((!ret || ((ret == -1)&&(errno == EINTR)))) {
            printf("time out");
            continue;
        }
        // do read
        do_read(read);
    }
    return((void *)0);
}

void pthread_test(){
    int number = 2;
    int ret = 0;
    char *path = NULL;
    if (getPipePath(number, &path) < 0) {
        printf("asprintf error");
        return;
    }
    printf("path:%s \n",path);
    unlink(path);
    mode_t um = umask(0);
    int rest = mkfifo(path, 00622);
    umask(um);
    if (rest == -1) {
        if(errno == EACCES){
            printf("没有权限 \n");
        }else if (errno != EEXIST) {
            printf("文件已存在，或者创建路径完全不对 \n");
        }
        printf("mkfifo error");
        free(path);
        return;
    }
    int readFd = open(path, O_RDWR | O_NONBLOCK);
    if (readFd == -1) {
        printf("open error");
        free(path);
        return;
    }
    free(path);

    pthread_t tid;
    if (ret == pthread_create(&tid, NULL, pipe_listen_thread, &readFd))
    {
        close(readFd);
        return;
    }

    int counter = 0;
    while (counter < 5) {
        sleep(5);
        write_notify(number);
        counter++;
    }

    pthread_join(tid, NULL);
    close(readFd);
}

//=====================================================
struct course
{
    char subject[11];
    int catalogNum;
    char *name;
};

void realloc_ptr_array_with_param(struct course **d, char *filename){
    struct course *temp;
    char *temp_name = NULL;

    for (int i = 1; i < 4; i++) {
        temp = realloc(*d, i * sizeof(**d));
//        temp = realloc(*d, i * sizeof(struct course));
        if(temp == NULL) {
            printf("unable to reallocate\n");
            exit(1);
        }
        *d = temp;
        asprintf(&temp_name,"zhangsan%d",i);
        (*d)[i-1].name = strdup(temp_name);
        printf("\n == name:%s",(*d)[i-1].name);
    }
}

void realloc_ptr_array_no_param(){
    struct course *temp;
    struct course *dd = NULL;
    char *temp_name = NULL;

    for (int i = 1; i < 4; ++i) {
        temp = realloc(dd, i * sizeof(*dd));
        if(temp == NULL) {
            printf("unable to reallocate\n");
            exit(1);
        }
        dd = temp;
        asprintf(&temp_name,"zhangsan%d",i);
        dd[i-1].name = strdup(temp_name);
        printf("\n == name:%s",dd[i-1].name);
    }
    printf("\n name:%s",dd[0].name);
    printf("\n name:%s",dd[1].name);
    printf("\n name:%s",dd[2].name);

}

void realloc_ptr_array_with_param_test(){
    struct course *d;
    char *filename = "hw06-data.csv";
    d = malloc( 1 * sizeof(struct course));//必须先分配一个地址，否则后面的realloc的第一个参数会无效
    realloc_ptr_array_with_param(&d,filename);
    printf("\n 2 name:%s",d[0].name);
    printf("\n 2 name:%s",d[1].name);
    printf("\n 2 name:%s",d[2].name);
}

//======================================
void * t1t2(void *arg) {
    int i;
    long id = (long)arg;
    for (i = 0; i < 60; i++) {
        pthread_mutex_lock(&sum_lock); //使用互斥量保护临界变量
        sum++;
        printf("t%ld: read sum value = %d\n", id + 1 , sum);
        pthread_mutex_unlock(&sum_lock);
        if (sum >= 100){
            pthread_cond_signal(&cond_sum_ready); //发送条件通知，唤醒等待线程
        }
    }
    return NULL;
}
void * t3(void *arg) {
    pthread_mutex_lock(&sum_lock);
    while(sum < 100 ) // 不满足条件将一直等待
    {
        printf("Waiting the signal...\n");
        pthread_cond_wait(&cond_sum_ready, &sum_lock); //等待条件满足
    }

    sum = 0;
    printf("t3: clear sum value\n");
    pthread_mutex_unlock(&sum_lock);
    return NULL;
}

int pthread_cond_wait_test(){
    int err;
    long i;
    for (i = 0; i < 2; i++) {
        err = pthread_create(&(tid[i]), NULL, &t1t2, (void *)i);//创建线程1、2
        if (err != 0) {
            printf("Can't create thread :[%s]", strerror(err));
        }
    }
    err = pthread_create(&(tid[2]), NULL, &t3, NULL); //创建线程3
    if (err != 0){
        printf("Can't create thread :[%s]", strerror(err));
    }
    for (i = 0; i < 3; i++){
        pthread_join(tid[i], NULL);
    }
    return 0;
}

int main ()
{
//============================= 范例一：malloc、calloc 动态分配内存 ================
    calloc_test();
    malloc_test();
    return 0;

//============================= 范例二：realloc 结构体数组传参 ================
//    realloc_ptr_array_with_param_test();    //函数传参
//    realloc_ptr_array_no_param();   //不传参

//============================= 范例三：通过文件实现多线程通讯，pthread_join()  ==========================
//    pthread_test(); //167虚拟机 编译:gcc -o main main.c -pthread,然后执行./main =====

//============================= 范例四：线程死锁及robust接管 ==========================
//    phread_deadlock_test();

//============================= 范例五：线程死锁及robust接管 pthread_cond_wait() ==========================
//    pthread_cond_wait_test();
}