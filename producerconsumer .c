/*
* －－ 生产者/消费者问题
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#define BUFFER "./buffer"   //生产者 消费者的一个非共享式资源


//在许多Solaris环境中没有这一说明，它应当在<sys/sem.h>文件中，但几个经常不是这样！
//如果你收到一个多semun的双重定义错误，则注释掉union定义
union semun                 //是一个sem联合结构
{
    int             val;
    struct semid_ds *buf;
    ushort          *array;
};

main (int argc,char *argv[])
{
    FILE                    *fptr;
    static struct sembuf    acquire = { 0,-1, SEM_UNDO },   //两个信号量模板
                            release = { 0, 1, SEM_UNDO };   //SEM_UNDO表示可以撤销
    pid_t                   c_pid;
    key_t                   ipc_key;
    static ushort           start_val[2] = {1,0};
    int                     semid,producer = 0,i,n,p_sleep,c_sleep,j=9;
    union semun             arg;
    enum {READ,MADE};
    if (argc != 2)              //检查是否有一个参数：进程在生命周期中的slepp秒数
    {
        fprintf (stderr,"%s sleep_time\n",argv[0]);
        exit (-1);
    }
    ipc_key = ftok (".",'S');   //获取信号量标识
    if ((semid = semget(ipc_key,2,IPC_CREAT | IPC_EXCL | 0666)) != -1)  //创建一个信号量集合或者获得
    {
        producer = 1;           //开始是1，表示最开始为生产者生产
        arg.array = start_val;
        if(semctl(semid,0,SETALL,arg) == -1)    //初始化信号量集合
        {
            perror("semctl -- producer -- initialization");
            exit (1);
        }
    }
    else if ((semid = semget(ipc_key,2,0)) == -1)
    {
        exit(2);
    }
/*
    *生产者/消费者问题的第二部分
*/  
  d{
    switch (producer)
    {
        case 1:                     //生产者
            p_sleep = atoi(argv[1]);    //睡眠时间
            srand ((unsigned) getpid());
            for (i = 0; i < 10; i++)
            {
                sleep (p_sleep);
                n = rand()%99 + 1;
                printf("A. The number [%2d] enerated by producer\n",n);
                acquire.sem_num = READ; //
                if (semop(semid,&acquire,1) == -1)
                {
                    perror ("semop -- producer -- waiting for cnsumer to read number");
                    exit(3);
                }
                if ((fptr = fopen(BUFFER,"w")) == NULL)
                {
                    perror(BUFFER);
                    exit(4);
                }
                fprintf(fptr,"%d\n",n); //写入
                fclose(fptr);
                release.sem_num = MADE; //类似于full
                printf("B.The number [%2d] deposited by producer\n",n);
                if (semop(semid,&release,1) == -1)
                {
                    perror ("semop -- producer -- indicating new number has been made");
                    exit(5);
                }
            }
            sleep(5);
            if (semctl(semid,0,IPC_RMID,0) == -1)
            {
                perror("semctl -- producer");
                exit(6);
            }
            printf("Semaphore removed\n");
            producer=0;
            break;
        case 0:                           //消费者
            c_sleep = atoi(argv[1]);
            c_pid = getpid();
            while(1)
            {
                sleep(c_sleep);
                acquire.sem_num = MADE;
                if (semop(semid,&acquire,1) == -1)
                {
                    perror ("semop -- consumer -- waiting for new number to be made");
                    exit(7);
                }
                if ((fptr = fopen(BUFFER,"r")) == NULL)
                {
                    perror(BUFFER);
                    exit(8);
                }
                fptr = fopen(BUFFER,"r");
                fscanf(fptr,"%d",&n);
                fclose(fptr);
                release.sem_num = READ;
                if(semop(semid,&release,1) == -1)
                {
                    perror ("semop -- consumer -- indicatingnumberhas been read");
                    exit(9);
                }
                printf("C.The number [%2d] obtained by consumer %6d\n",n,c_pid);
            } 
          producer=1;
           break;
    }while(j--);
  semctl(semid,0,IPC_RMDI,0);  
}


///////////////////////////////////////////

   

















