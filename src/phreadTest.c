/*================================================================
 *   Copyright (C) 2019 Ltd. All rights reserved.
 *
 *   File Name ：robust_mutex.c
 *   Author    ：Hamilton
 *   Date      ：2019-07-30
 *   Descriptor：
 *
 ================================================================*/
#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#define handle_error_en(en, msg) \
    do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

static pthread_mutex_t mtx;

static void *original_owner_thread(void *ptr){
    printf("\n[original owner] Setting lock...\n");
    pthread_mutex_lock(&mtx);
    printf("[original owner] Locked. Now exiting without unlocking.\n");
    pthread_exit(NULL);
}

static void *bad_thread(void *ptr)
{
    printf("\n======再次中断，未释放。。。\n");
    printf("\n[bad owner] Setting lock...\n");
    pthread_mutex_lock(&mtx);
    printf("[bad owner] Locked. Now exiting without unlocking.\n");
    pthread_exit(NULL);
}

static void *second_thread(void *ptr)
{
    int i = 5;
    while (i--)
    {
        int s = pthread_mutex_lock(&mtx);
        if (s == EOWNERDEAD)
        {
            printf("\n[second thread] pthread_mutex_lock() returned EOWNERDEAD\n");
            printf("[second thread] Now make the mutex consistent\n");
            s = pthread_mutex_consistent(&mtx);
            if (s != 0)
                handle_error_en(s, "pthread_mutex_consistent");
            printf("[second thread] Mutex is now consistent; unlocking\n");
            s = pthread_mutex_unlock(&mtx);
            if (s != 0)
                handle_error_en(s, "pthread_mutex_unlock");
        }
        else if (s < 0)
        {
            printf("\n[second thread] pthread_mutex_lock() unexpectedly failed\n");
            handle_error_en(s, "pthread_mutex_lock");
        }
        else
        {
            printf("\n[second thread] pthread_mutex_lock success.\n");
            printf("do somthing.... \n");
            s = pthread_mutex_unlock(&mtx);
            if (s != 0)
                handle_error_en(s, "pthread_mutex_unlock");
        }
        sleep(1);
    }

    pthread_exit(NULL);
}

int phread_deadlock_test()
{
    pthread_t thr;
    pthread_mutexattr_t attr;
    int s;

    pthread_mutexattr_init(&attr);
    /* initialize the attributes object */
    pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST);
    /* set robustness */

    pthread_mutex_init(&mtx, &attr);   /* initialize the mutex */
    pthread_create(&thr, NULL, original_owner_thread, NULL);

    sleep(1);
    pthread_create(&thr, NULL, second_thread, NULL);
    sleep(1);
    pthread_create(&thr, NULL, bad_thread, NULL);

    /* "original_owner_thread" should have exited by now */

    int i = 5;
    while(i--)
    {
        s = pthread_mutex_lock(&mtx);

        if (s == EOWNERDEAD)
        {
            printf("\n[main thread] pthread_mutex_lock() returned EOWNERDEAD\n");

            printf("[main thread] Now make the mutex consistent\n");
            s = pthread_mutex_consistent(&mtx);
            if (s != 0)
                handle_error_en(s, "pthread_mutex_consistent");

            printf("[main thread] Mutex is now consistent; unlocking\n");
            s = pthread_mutex_unlock(&mtx);
            if (s != 0)
                handle_error_en(s, "pthread_mutex_unlock");

        }
        else if (s < 0)
        {
            printf("\n[main thread] pthread_mutex_lock() unexpectedly failed\n");
            handle_error_en(s, "pthread_mutex_lock");
        }
        else
        {
            printf("\n[main thread] pthread_mutex_lock success.\n");
            printf("do somthing.... \n");
            s = pthread_mutex_unlock(&mtx);
            if (s != 0)
                handle_error_en(s, "pthread_mutex_unlock");
        }

        sleep(1);
    }
    exit(EXIT_SUCCESS);
}