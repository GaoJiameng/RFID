#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "rfid_lib.h"
#include "sqlite3.h"
#include <pthread.h>
//串口号
#if 0
    #define UART_DEV   "/dev/ttyUSB0"
#else
    #define UART_DEV   "/dev/ttySAC2"
#endif

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

    //创建一张考勤表KQ id time rfid
    char *cmd = "create table KQ(id int, \
    time text, rfid text);";
    sqlite3_exec(db,cmd,NULL, NULL, NULL);
    
    //创建一张员工表person id name rfid
    cmd = "create table person(id int, \
    name text, rfid text);";
    sqlite3_exec(db,cmd,NULL, NULL, NULL);

    //创建一张管理员表amdin usr psw
    cmd = "create table admin(usr text, \
    psw text);";
    sqlite3_exec(db,cmd,NULL, NULL, NULL);

    return db;
}
void* usr_input_fun(void *arg)
{
    sqlite3 *db = (sqlite3 *)arg;
    printf("注册：register\n");
    printf("查询：search\n");
    while(1)
    {
        char cmd[32]="";
        fgets(cmd,sizeof(cmd),stdin);
        cmd[strlen(cmd)-1] = 0;//去掉回车

        if(strncmp(cmd,"register", strlen("register")) == 0)
        {
            //注册信息
            int id=0;
            char name[32]="";
            char rfid[64]="";
            printf("请输入需要注册的id name:");
            scanf("%d %s", &id, name);

            flag = 1;//注册
            
            while(flag);//等待线程2获取rfid卡号
            strcpy(rfid, rfid_tmp);
            memset(rfid_tmp, 0, sizeof(rfid_tmp));

            char sql[128]="";
            sprintf(sql,"insert into person values(%d, \'%s\', \'%s\');"\
            , id, name, rfid);

            sqlite3_exec(db,sql,NULL, NULL,NULL);

            printf("成功注册");

        }
        else if(strncmp(cmd,"search", strlen("search")) == 0)
        {
            //查询
            printf("请输入要查询的姓名：");
            char name[32]="";
            fgets(name, sizeof(name), stdin);
            name[strlen(name)-1] = 0;

            char sql[128]="";
            sprintf(sql,"select name,time from KQ,person \
            where person.id=KQ.id and person.name=\'%s\';",name);
            char **result=NULL;
            int nRow = 0;
            int nCol = 0;
            sqlite3_get_table(db,sql, &result, &nRow, &nCol, NULL);
            
            int i=0,j=0;
            for (i = 0; i < nRow+1; i++)
            {
                for (j = 0; j < nCol; j++)
                {
                    printf("%s ", result[j+i*nCol]);
                }
                printf("\n");
                
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
                sprintf(rfid+strlen(rfid),"%02x-", id[i]);
            }
            rfid[strlen(rfid)-1]=0;//去掉最后一个‘-’

            if(flag == 1)//注册
            {
                flag =0;
                //遍历数据库 如果不存在此卡 才允许注册
                strcpy(rfid_tmp,rfid);

            }
            else if(flag == 0)//考勤
            {
                //在person通过rfid的id
                char sql[128]="";
                sprintf(sql,"select id from person where rfid=\'%s\';", rfid);

                char **result=NULL;
                int nRow = 0;
                int nCol = 0;
                sqlite3_get_table(db,sql, &result, &nRow, &nCol, NULL);
                if(nRow == 0)
                {
                    printf("无效的卡\n");
                }
                else
                {
                    //id存放在resualt[1]中
                    memset(sql,0,sizeof(sql));
                    sprintf(sql,"insert into KQ values(%s, datetime(\'now\'), \'%s\');",\
                    result[1], rfid);
                    sqlite3_exec(db,sql,NULL,NULL,NULL);
                    printf("KQ ok\n");

                }
                
                
            }
            

        }
    }

    return 0;
}
