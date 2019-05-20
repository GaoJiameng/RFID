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
    #define UART_DEV   "/dev/ttyUSB0"
#else
    #define UART_DEV   "/dev/ttySAC2"
#endif

typedef struct 
{
	long type;
	char text[128];
}MSG;

int flag = 0;//0表示考勤 1表示注册
char rfid_tmp[32]="";//存放rfid


sqlite3 * sqlite3_init(char *db_name)
{
    //打开数据库
    sqlite3 *db = NULL;
    int ret = sqlite3_open(db_name, &db);
    if(ret != SQLITE_OK)
    {
        perror("sqlite3_open");
        return NULL;
    }

    //创建一张考勤表checkin id name time rfid
    char *cmd = "create table checkin(id int, \
    name text , time text , rfid text);";
    sqlite3_exec(db,cmd,NULL, NULL, NULL);
    
    //创建一张员工表person id name sex rfid
    cmd = "create table staff(id int, \
    name text , sex text , rfid text);";
    sqlite3_exec(db,cmd,NULL, NULL, NULL);

    return db;
}


void* usr_input_fun(void *arg)
{
    sqlite3 *db = (sqlite3 *)arg;

    while(1)
    {
        //创建消息队列
        key_t key = ftok("/",2019);
        int msgid = msgget(key,IPC_CREAT|0666);
        if(msgid < 0)  
        {  
            perror("msgget");  
            return 0;  
        }  

        //接收消息
        MSG msg;
        // puts("----------1-----------");
        msgrcv(msgid,&msg,sizeof(MSG)-sizeof(long),-4,0);//0表示正常模式阻塞 
        flag = 1;   //标记位改变
        // puts("----------2-----------");
        printf("msg.text = %s\n",msg.text);
        printf("msg.type = %ld\n",msg.type);
        
        if(msg.type == 1)//注册 回复标记9
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
            sprintf(cmd , "select * from staff where rfid = \'%s\';" , rfid_tmp);
            printf("rfid_tmp = %s\n" , rfid_tmp);
            char **result = NULL;
            int nRow = 0;
            int nCol = 0;
            sqlite3_get_table(db,cmd,&result,&nRow,&nCol,NULL);


            printf("nRow = %d\n" , nRow);
            printf("nCol = %d\n" , nCol);

            if(nRow > 0 && strcmp(rfid_tmp , result[7]) == 0)  //如果匹配则说明已经注册
            {
                MSG msg1;
                msg1.type = 9; //回复标记
                strcpy(msg1.text,"failed");
                puts("注册失败");
                msgsnd(msgid,&msg1,sizeof(MSG)-sizeof(long),0); //0表示正常模式阻塞  
            }
            else if(nRow == 0)   //不匹配就进行数据库更新
            {
                char sql[128]="";
                sprintf(sql,"insert into staff values(%d, \'%s\',\'%s\', \'%s\');"\
                , id,usr,sex,rfid_tmp);
                // printf("id = %ld , usr = %s , sex = %s , rfid = %s",id , usr , sex , rfid);
                sqlite3_exec(db,sql,NULL, NULL,NULL);
                MSG msg1;
                msg1.type = 9; //回复标记
                puts("注册成功");
                strcpy(msg1.text,"succeed");
                msgsnd(msgid,&msg1,sizeof(MSG)-sizeof(long),0);
            }
            
            if(result != NULL)
            {
                sqlite3_free_table(result);  
                result = NULL;  
            }
        }
        else if(msg.type == 2)//注销  回复标记7
        {
            //将拿来的消息分解
            char rfid[32] = "";
            if(strcmp(msg.text , "$$$") == 0)  //刷卡获取的消息
            {
                printf("刷卡获取的消息");
            }
            else   //手动输入的消息
            {
                strcpy(rfid , msg.text);
                printf("手动输入的消息");
            }
            memset(&msg,0,sizeof(MSG));//清空结构体

            //在数据库中查找
            //遍历数据库 如果不存在此卡 才允许注册
            char cmd[64]="";
            char cmd_acllback[64]="";
            sprintf(cmd , "select * from staff where rfid = \'%s\';" , rfid_tmp);
            printf("delete_rfid_tmp = %s\n" , rfid_tmp);
            char **result = NULL;
            int nRow = 0;
            int nCol = 0;
            sqlite3_get_table(db,cmd,&result,&nRow,&nCol,NULL);

            if(nRow > 0 && strcmp(rfid_tmp , result[7]) == 0)  //如果匹配则说明已经注册 删除卡号
            {
                sprintf(cmd , "delete from staff where rfid = \'%s\';" , rfid_tmp);  
                sqlite3_exec(db,cmd,NULL, NULL, NULL);
                sprintf(cmd , "delete from checkin where rfid = \'%s\';" , rfid_tmp);  
                sqlite3_exec(db,cmd,NULL, NULL, NULL);

                sprintf(cmd , "select * from staff where rfid = \'%s\';" , rfid_tmp);
                printf("delete_rfid_tmp = %s\n" , rfid_tmp);
                char **result = NULL;
                int nRow = 0;
                int nCol = 0;
                sqlite3_get_table(db,cmd,&result,&nRow,&nCol,NULL);

                if(nRow == 0)   //不匹配
                {
                    sprintf(cmd_acllback , "1-%s" , rfid_tmp);
                    MSG msg2;
                    msg2.type = 7; //回复标记
                    strcpy(msg2.text,cmd_acllback);
                    msgsnd(msgid,&msg2,sizeof(MSG)-sizeof(long),0);
                    puts("删除成功");
                    //清空结构体
		            memset(&msg2,0,sizeof(MSG));
                }
            }
            else if(nRow == 0)   //不匹配
            {
                sprintf(cmd_acllback , "0-%s" , rfid_tmp);
                MSG msg2;
                msg2.type = 7; //回复标记
                strcpy(msg2.text,cmd_acllback);
                msgsnd(msgid,&msg2,sizeof(MSG)-sizeof(long),0);
                puts("删除失败");
                //清空结构体
		        memset(&msg2,0,sizeof(MSG));
            }

            
        }
        else if(msg.type == 3)//查询   回复标记6(回复数据库数据) 和 8(回复列数和行数)
        {
            //查询
            //将拿来的消息分解
            char method[32] = "";
            char text[32] = "";
            char cmd_acllback[64]="";
            sscanf(msg.text,"%[^:]:%s",method,text);
            printf("method = %s , text = %s \n",method,text);
            memset(&msg,0,sizeof(MSG));//清空结构体

            if(strcmp(method , "id") == 0)   //id方式查询
            {
                char sql[128]="";
                sprintf(sql,"select * from checkin where id = %d;",atoi(text));
                char **result=NULL;
                int nRow = 0;
                int nCol = 0;
                sqlite3_get_table(db,sql, &result, &nRow, &nCol, NULL);
                // printf("query_nRow = %d\n" , nRow);
                // printf("query_nCol = %d\n" , nCol);

                MSG msg3;
                msg3.type = 8; //回复标记
                sprintf(cmd_acllback , "%d$%d" , nRow , nCol);
                strcpy(msg3.text,cmd_acllback);
                msgsnd(msgid,&msg3,sizeof(MSG)-sizeof(long),0);
                memset(&msg3,0,sizeof(MSG));//清空结构体

                int i=0,j=0;
                for (i = 0; i < nRow+1; i++)
                {
                    for (j = 0; j < nCol; j++)
                    {
                        msg3.type = 6; //回复标记
                        strcpy(msg3.text,result[j+i*nCol]);
                        msgsnd(msgid,&msg3,sizeof(MSG)-sizeof(long),0);
                        memset(&msg3,0,sizeof(MSG));//清空结构体
                    }
                    printf("\n");
                    
                }
            }
            else if(strcmp(method , "name") == 0)   //name方式查询
            {
                char sql[128]="";
                sprintf(sql,"select * from checkin where name = \'%s\';",text);
                char **result=NULL;
                int nRow = 0;
                int nCol = 0;
                sqlite3_get_table(db,sql, &result, &nRow, &nCol, NULL);
                // printf("query_nRow = %d\n" , nRow);
                // printf("query_nCol = %d\n" , nCol);

                MSG msg3;
                msg3.type = 8; //回复标记
                sprintf(cmd_acllback , "%d$%d" , nRow , nCol);
                strcpy(msg3.text,cmd_acllback);
                msgsnd(msgid,&msg3,sizeof(MSG)-sizeof(long),0);
                memset(&msg3,0,sizeof(MSG));//清空结构体

                int i=0,j=0;
                for (i = 0; i < nRow+1; i++)
                {
                    for (j = 0; j < nCol; j++)
                    {
                        msg3.type = 6; //回复标记
                        strcpy(msg3.text,result[j+i*nCol]);
                        msgsnd(msgid,&msg3,sizeof(MSG)-sizeof(long),0);
                        memset(&msg3,0,sizeof(MSG));//清空结构体
                    }
                    printf("\n");
                    
                }
            }
            else if(strcmp(method , "rfid") == 0)   //rfid方式查询
            {
                char sql[128]="";
                sprintf(sql,"select * from checkin where rfid = \'%s\';",text);
                char **result=NULL;
                int nRow = 0;
                int nCol = 0;
                sqlite3_get_table(db,sql, &result, &nRow, &nCol, NULL);
                // printf("rfid_nRow = %d\n" , nRow);
                // printf("rfid_nCol = %d\n" , nCol);

                MSG msg3;
                msg3.type = 8; //回复标记
                sprintf(cmd_acllback , "%d$%d" , nRow , nCol);
                strcpy(msg3.text,cmd_acllback);
                msgsnd(msgid,&msg3,sizeof(MSG)-sizeof(long),0);
                memset(&msg3,0,sizeof(MSG));//清空结构体

                int i=0,j=0;
                for (i = 0; i < nRow+1; i++)
                {
                    for (j = 0; j < nCol; j++)
                    {
                        msg3.type = 6; //回复标记
                        strcpy(msg3.text,result[j+i*nCol]);
                        msgsnd(msgid,&msg3,sizeof(MSG)-sizeof(long),0);
                        memset(&msg3,0,sizeof(MSG));//清空结构体
                    }
                    printf("\n");
                    
                }
            }

        }    

    }

}


int main(void)
{
    int len,i;char type;
    unsigned char id[18] = {0};
    //捕获信号
    //rfid初始化
    uart_rfid_init(UART_DEV);
    //数据库初始化
    sqlite3 *db = sqlite3_init("rfid.db");

    //创建一个用户输入线程（项目中用网页实现）
    pthread_t tid;
    pthread_create(&tid, NULL, usr_input_fun , db);
    pthread_detach(tid);

    while(1)
    {
        if(len = get_rfid_card_id(id,&type)){
            printf("%c类卡卡号:",type);
            for(i=0;i<len;i++)
                printf("%02x ",id[i]);          
            puts("");


            char rfid[128]="";
            for (i = 0; i < len; i++)
            {
                sprintf(rfid+strlen(rfid),"%02x:", id[i]);
            }
            rfid[strlen(rfid)-1]=0;//去掉最后一个‘-’
            strcpy(rfid_tmp,rfid);

            if(flag == 1)//注册
            {
                strcpy(rfid_tmp,rfid);
                flag =0;
                //遍历数据库 如果不存在此卡 才允许注册
                
            }
            else if(flag == 0)//考勤
            {
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
                    // printf("--------1--------\n");
                    // printf("result[4] = %s\n" , result[4]);
                    // printf("result[5] = %s\n" , result[5]);
                    //id存放在resualt[1]中
                    char sql[128]="";
                    memset(sql,0,sizeof(sql));
                    sprintf(sql,"insert into checkin values(%d, \'%s\', datetime(\'now\'), \'%s\');",\
                            atoi(result[4]), result[5], rfid);
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

            }
            

        }
    }

    return 0;
}
