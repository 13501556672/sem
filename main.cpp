#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define THREAD_NUMBER 3 /* 进程/线程数 */
#define REPEAT_NUMBER 3 /* 每个进程/线程中的小任务数 */
#define DELAY_TIME_LEVELS 3.0 /*小任务之间的最大时间间隔*/

sem_t sem[THREAD_NUMBER];

void* thread_func(void* arg)
{
    int num = (long)arg;
    int delay_time = 0;
    int count = 0;

    /* 进行 P 操作 */
    sem_wait(&sem[num]);

    printf("Thread %d is starting\n", num);
    for (count = 0; count < REPEAT_NUMBER; count++)
    {
        delay_time = (int)(rand() * DELAY_TIME_LEVELS / (RAND_MAX)) + 1;
        sleep(delay_time);
        printf("\tThread %d: job %d delay = %d\n", num, count, delay_time);
    }

    printf("Thread %d finished\n", num);
    pthread_exit(NULL);
}

int main(void)
{
    pthread_t thread[THREAD_NUMBER];
    int no = 0, res;
    void* thread_ret;
   // srand(time(NULL));

    for (no = 0; no < THREAD_NUMBER; no++)
    {
        sem_init(&sem[no], 0, 0);
        res = pthread_create(&thread[no], NULL, thread_func, (void*)(unsigned long long)no);

        if (res != 0)
        {
            printf("Create thread %d failed\n", no);
            exit(res);
        }
    }

    printf("Create treads success\n Waiting for threads to finish...\n");

    /* 对最后创建的进程/线程的信号量进行 V 操作 */
    sem_post(&sem[THREAD_NUMBER - 1]);
    for (no = THREAD_NUMBER - 1; no >= 0; no--)
    {
        res = pthread_join(thread[no], &thread_ret);
        if (!res)
        {
            printf("Thread %d joined\n", no);
        }
        else
        {
            printf("Thread %d join failed\n", no);
        }

        /* 进行 V 操作 */
        sem_post(&sem[(no + THREAD_NUMBER - 1) % THREAD_NUMBER]);
    }

    for (no = 0; no < THREAD_NUMBER; no++)
    {
        /* 删除信号量 */
        sem_destroy(&sem[no]);
    }

    return 0;
}