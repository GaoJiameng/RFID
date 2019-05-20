#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>
#include <sys/shm.h> 
#include "rfid_lib.h"
#include "sqlite3.h"


//串口号
#if 0
	#define UART_DEV  	"/dev/ttyUSB0"
#else
	#define UART_DEV  	"/dev/ttySAC2"
#endif

int card_type = 0;

typedef struct 
{
	long type;
	char text[128];
}MSG;

void* checking_in(void *arg , char rfid[])
{
    sqlite3 *db = (sqlite3 *)arg;

    //在数据库中查找
    //遍历数据库 如果存在此卡 才允许打卡
    char cmd[64]="";
    sprintf(cmd , "select * from staff where rfid = \'%s\';" , rfid);
    char **result = NULL;
    int nRow = 0;
    int nCol = 0;
    sqlite3_get_table(db,cmd,&result,&nRow,&nCol,NULL);

    if(nRow > 0 && strcmp(rfid , result[7]) == 0)  //如果匹配则说明可以打卡
    {
        //id存放在resualt[1]中
        char sql[128]="";
        memset(sql,0,sizeof(sql));
        sprintf(sql,"insert into checkin values(%s, %s, datetime(\'now\'), \'%s\');",\
                result[4], result[5], rfid);
        sqlite3_exec(db,sql,NULL,NULL,NULL);
        printf("打卡成功！\n");
    }
    else if(nRow == 0)  //不匹配
    {
        printf("无效的卡\n");
    }

    if(result != NULL)
    {
        sqlite3_free_table(result);  
        result = NULL;  
    }
    return NULL;
}


int main(int argc, char const *argv[])
{
	int len;
	char type;
	unsigned char id[18] = {0};
	//捕获信号
	//rfid初始化
	uart_rfid_init(UART_DEV);

	while(1)
	{
		if(len = get_rfid_card_id(id,&type))
        { 
            char rfid[128]="";
			size_t i = 0 ;
			for(i = 0 ; i < len ; i++)
			{
				sprintf(rfid+strlen(rfid) , "%02x:" , id[i]);
			}
			rfid[strlen(rfid)-1] = 0; //去掉最后一个:号
            printf("rfid = %s\n",rfid);
            
            //创建消息队列
            key_t key = ftok("/",2019);
            int msgid = msgget(key,IPC_CREAT|0666);
            if(msgid < 0)  
            {  
                perror("msgget");  
                return 0;  
            }  

            //创建一个唯一的键值  
            key_t key_shm = ftok("/", 2019);  
            //根据key创建一个物理共享内存标识符  
            int shmId = shmget(key_shm, 128, IPC_CREAT|0600);  
            //根据物理内存标识符 映射 虚拟内存  NULL自动分配  0可读可写  
            char *addr =NULL;  
            addr = shmat(shmId, NULL, 0);  
            if(addr == NULL)  
            {  
                perror("shmat");  
                return 0;  
            }  
	
            //借助addr虚拟地址  往物理共享内存写入数据  
            if(addr != NULL)
            {
                card_type = atoi(addr);
                printf("card_type_01 = %d\n",card_type);
                printf("addr = %s\n" , addr);
                addr = NULL;
            }
              
            //解除映射
            shmdt(addr);

            //打开数据库
            sqlite3 *db = NULL;
            int ret = sqlite3_open("/home/www/cgi-bin/rfid.db",&db);
            if(ret < 0)
            {
                perror("sqlite3_open");
            }

            if(card_type == 0)  //考勤
            {
                //考勤函数
                checking_in(db , rfid);
            }
            else if(card_type == 1)  //非考勤
            {
                //接收消息
                MSG msg;
                // puts("----------1-----------");
                msgrcv(msgid,&msg,sizeof(MSG)-sizeof(long),1,0);//0表示正常模式阻塞 
                // puts("----------2-----------");
                printf("msg.text = %s\n",msg.text);
                printf("msg.type = %ld\n",msg.type);

                if(msg.type == 1)//注册
                {
                    //将拿来的消息分解
                    char usr[32] = "";
                    int id = 0;
                    char sex[32] = "";
                    sscanf(msg.text,"%d:%[^:]:%s",&id,usr,sex);
                    printf("id = %d , usr = %s , sex = %s \n",id,usr,sex);
                    memset(&msg,0,sizeof(MSG));//清空结构体

                    //在数据库中查找
                    //遍历数据库 如果不存在此卡 才允许注册
                    char cmd[64]="";
                    sprintf(cmd , "select * from staff where rfid = \'%s\';" , rfid);
                    char **result = NULL;
                    int nRow = 0;
                    int nCol = 0;
                    sqlite3_get_table(db,cmd,&result,&nRow,&nCol,NULL);

                    // puts("-----------3------------");
                    printf("nRow = %d\n" , nRow);
                    printf("nCol = %d\n" , nCol);

                    if(nRow > 0 && strcmp(rfid , result[7]) == 0)  //如果匹配则说明已经注册
                    {
                        msg.type = 5; //回复标记
                        strcpy(msg.text,"failed");
                        puts("注册失败");
                        msgsnd(msgid,&msg,sizeof(MSG)-sizeof(long),0); //0表示正常模式阻塞  
                    }
                    else if(nRow == 0)   //不匹配就进行数据库更新
                    {
                        char sql[128]="";
                        sprintf(sql,"insert into staff values(%d, \'%s\',\'%s\', \'%s\');"\
                        , id,usr,sex,rfid);
                        // printf("id = %ld , usr = %s , sex = %s , rfid = %s",id , usr , sex , rfid);
                        sqlite3_exec(db,sql,NULL, NULL,NULL);
                        msg.type = 5;   //回复标记
                        puts("注册成功");
                        strcpy(msg.text,"succeed");
                        msgsnd(msgid,&msg,sizeof(MSG)-sizeof(long),0);
                    }
                    
                    if(result != NULL)
                    {
                        sqlite3_free_table(result);  
                        result = NULL;  
                    }
                    sqlite3_close(db);

                    
                }
                else if(msg.type == 2)//注销
                {

                    
                }
                else if(msg.type == 3)//查询
                {

                
                }    

            card_type = 0; 
            }
      
            
		}
	}

	return 0;
}

