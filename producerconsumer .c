/*
* ���� ������/����������
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#define BUFFER "./buffer"   //������ �����ߵ�һ���ǹ���ʽ��Դ


//�����Solaris������û����һ˵������Ӧ����<sys/sem.h>�ļ��У���������������������
//������յ�һ����semun��˫�ض��������ע�͵�union����
union semun                 //��һ��sem���Ͻṹ
{
    int             val;
    struct semid_ds *buf;
    ushort          *array;
};

main (int argc,char *argv[])
{
    FILE                    *fptr;
    static struct sembuf    acquire = { 0,-1, SEM_UNDO },   //�����ź���ģ��
                            release = { 0, 1, SEM_UNDO };   //SEM_UNDO��ʾ���Գ���
    pid_t                   c_pid;
    key_t                   ipc_key;
    static ushort           start_val[2] = {1,0};
    int                     semid,producer = 0,i,n,p_sleep,c_sleep,j=9;
    union semun             arg;
    enum {READ,MADE};
    if (argc != 2)              //����Ƿ���һ�����������������������е�slepp����
    {
        fprintf (stderr,"%s sleep_time\n",argv[0]);
        exit (-1);
    }
    ipc_key = ftok (".",'S');   //��ȡ�ź�����ʶ
    if ((semid = semget(ipc_key,2,IPC_CREAT | IPC_EXCL | 0666)) != -1)  //����һ���ź������ϻ��߻��
    {
        producer = 1;           //��ʼ��1����ʾ�ʼΪ����������
        arg.array = start_val;
        if(semctl(semid,0,SETALL,arg) == -1)    //��ʼ���ź�������
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
    *������/����������ĵڶ�����
*/  
  d{
    switch (producer)
    {
        case 1:                     //������
            p_sleep = atoi(argv[1]);    //˯��ʱ��
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
                fprintf(fptr,"%d\n",n); //д��
                fclose(fptr);
                release.sem_num = MADE; //������full
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
        case 0:                           //������
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

   

















