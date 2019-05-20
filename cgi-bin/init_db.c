#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"


int main(int argc, char const *argv[])
{
    //打开数据库
	sqlite3 * db = NULL;
	int ret = sqlite3_open("rfid.db" , &db);
	if(ret != SQLITE_OK)
	{
		perror("Open sqlite3 datebase failed!");
		return 0;
	}
	//创建一张考勤表 id name time rfid
	char * cmd = "create table checkin(id int , name text , time text , rfid text);"; 
	sqlite3_exec(db , cmd , NULL , NULL , NULL);
	//创建一张员工表 id name rfid
	cmd = "create table staff(name text , id int , sex text , rfid text);"; 
	sqlite3_exec(db , cmd , NULL , NULL , NULL);

    sqlite3_close(db);




    //打开数据库
	sqlite3 * db1 = NULL;
	int ret1 = sqlite3_open("admin.db", &db1);
	if(ret != SQLITE_OK)
	{
		perror("Open sqlite3 datebase failed!");
		return 0;
	}
	//创建一张管理员表
	cmd = "create table admin(user text , psw text);"; 
	sqlite3_exec(db1 , cmd , NULL , NULL , NULL);

    cmd = "insert into admin values(\'admin\' , \'123456\')";
    sqlite3_exec(db1 , cmd , NULL , NULL , NULL);
    sqlite3_close(db1);


    return 0;
}
