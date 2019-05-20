#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
typedef struct 
{
	long type;
	char text[128];	
}MSG;

int main(int argc, char const *argv[])
{
	
	printf("content-type:text/html\n\n");

	char *dataLen = NULL;
	dataLen = getenv("CONTENT_LENGTH");
	if(dataLen != NULL)
	{
		//获取服务器的输出
		char msg1[64] = "";
		fgets(msg1,sizeof(msg1),stdin);

		//创建消息队列
		key_t key = ftok("/",2019);
		int msgid = msgget(key,IPC_CREAT|0666);
		if(msgid < 0)  
		{  
			perror("msgget");  
			return 0;  
		}  

		//发送消息
		MSG msg;
		//设置消息类型
		msg.type = 3;
		//将得到的数据传输给消息队列
		strcpy(msg.text,msg1);

		msgsnd(msgid,&msg,sizeof(MSG)-sizeof(long),0); //0表示正常模式阻塞  

		//清空结构体
		memset(&msg,0,sizeof(MSG));
	
		//得到反馈信息
		msgrcv(msgid,&msg,sizeof(MSG)-sizeof(long),8,0);

		int nRow = 0;
        int nCol = 0;
		char char_nRow[8] = "";
		char char_nCol[8] = "";
		sscanf(msg.text,"%[^$]$%s",char_nRow , char_nCol);
		nRow = atoi(char_nRow);
		nCol = atoi(char_nCol);
		memset(&msg,0,sizeof(MSG));//清空结构体


		//传送给前端
		int i=0,j=0;
		for (i = 0; i < nRow+1; i++)
		{
			for (j = 0; j < nCol; j++)
			{
				//得到反馈信息
				msgrcv(msgid,&msg,sizeof(MSG)-sizeof(long),6,0);
				printf("%s     ",msg.text);
				memset(&msg,0,sizeof(MSG));//清空结构
			}		
			printf("$");
		}
		
	}

	return 0;
}