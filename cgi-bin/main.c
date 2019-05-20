#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include "rfid_lib.h"
#include "sqlite3.h"


//串口号
#if 0
	#define UART_DEV  	"/dev/ttyUSB0"
#else
	#define UART_DEV  	"/dev/ttySAC2"
#endif

int flag = 0;//0表示考勤 1表示注册
char rfid_tmp[]

sqlite3 * sqlite3_init(char *db_name)
{
	//打开数据库
	sqlite3 * db = NULL;
	int ret = sqlite3_open(db_name , &db);
	if(ret != SQLITE_OK)
	{
		perror("Open sqlite3 datebase failed!");
		return NULL;
	}
	//创建一张考勤表 id name time rfid
	char * cmd = "create table checking_in(id int , name text , time text , rfid text);"; 
	sqlite3_exec(db , cmd , NULL , NULL , NULL);
	//创建一张员工表 id name rfid
	cmd = "create table staff(id int , name text , rfid text);"; 
	sqlite3_exec(db , cmd , NULL , NULL , NULL);
	//创建一张管理员表
	cmd = "create table admin(user text , psw text);"; 
	sqlite3_exec(db , cmd , NULL , NULL , NULL);

	return db;
}

void* usr_input_fun(void *arg)
{
	sqlite3 *db = (sqlite3 *)arg;
	printf("register\n");
	printf("search\n");
	while(1)
	{
		char cmd[32] = "";
		fgets(cmd , sizeof(cmd) , stdin);
		cmd[strlen(cmd)-1] = 0;

		if(strncmp(cmd , "register" , strlen("register")) == 0)
		{
			//注册信息
			int id = 0;
			char name[32] = "";
			char rfid[64] = "";
			printf("请输入需要注册的id name");
			scanf("%d %s" , &id , name);

			flag = 1;//注册
			while(flag) //等待线程注册完毕
			{
				strcpy(rfid , rfid_tmp);
				memset(rfid_tmp , 0 , sizeof(rfid_tmp));

				char sql[128];
				sprintf*(sql , "insert into person values(%d , \'%s\' , \'%s\');" , id ,name , rfid);

				sqlite3_exec(db , sql , NULL , NULL , NULL);

				printf("注册成功");

			}
		}
		else if(strncmp(cmd , "search" , strlen("search")) == 0)
		{
			//查询
			printf("请输入要查询的姓名");
			
		}
	}

}

int main(void)
{
	int len,i;
	char type;
	unsigned char id[18] = {0};
	//捕获信号
	//rfid初始化
	uart_rfid_init(UART_DEV);
	//数据库初始化
	sqlite3 *db = sqlite3_init("rfid.db");

	//创建一个用户输入线程(项目中用网页实现)
	pthread_t tid;
	pthread_create(&tid , NULL , usr_input_fun , db);
	pthread_detach(tid);

	while(1)
	{
		if(len = get_rfid_card_id(id,&type)){
			printf("%c类卡卡号:",type);
			for(i=0;i<len;i++)
				printf("%02x ",id[i]);			
			puts("");

			char rfid[128]="";
			size_t i = 0 ;
			for(i = 0 ; i < len ; i++)
			{
				sprintf(rfid+strlen(rfid) , "%02x:" , id[i]);
			}
			rfid[strlen(rfid)-1] = 0; //去掉尊后一个:号
			// printf("rfid = %s\n" , rfid);

			if(flag == 1)//注册
			{
				flag = 0;
				//遍历数据库 如果不存在此卡 才允许注册
				strcpy(rfid_tmp , rfid);
			}
			else if(flag == 0)//考勤
			{

			}
		}
	}

	return 0;
}

