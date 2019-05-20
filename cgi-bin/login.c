#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"


int main(int argc, char const *argv[])
{
    
    printf("content-type:text/html\n\n");


    char *dataLen = NULL;
    dataLen = getenv("CONTENT_LENGTH");
    if(dataLen != NULL)
    {
        char *dataLen = NULL;
        dataLen = getenv("CONTENT_LENGTH");
        if(dataLen != NULL)
        {
            char msg[32] = "";
            fgets(msg,sizeof(msg),stdin);
            char usr[32]="";
            char pwd[32]="";
            sscanf(msg,"%[^:]:%s",usr,pwd);

            sqlite3 *db = NULL;
            int ret = sqlite3_open("/home/www/cgi-bin/admin.db",&db);
            if(ret != SQLITE_OK)
            {
                perror("Open sqlite3 datebase failed!");
                return 0;
            }
            char cmd[64];
            sprintf(cmd , "select * from admin where user = \'%s\';" , usr);
            char **result = NULL;
            int nRow = 0;
            int nCol = 0;
            sqlite3_get_table(db,cmd,&result,&nRow,&nCol,NULL);
             
            if(strcmp(usr,result[2]) == 0 && strcmp(pwd,result[3]) == 0)
            {
                printf("ok");
            }
            else
            {
                printf("no");
            }
            
        

            if(result != NULL)
            {
                sqlite3_free_table(result);
                result = NULL;
            }
            sqlite3_close(db);
        }
    }

    return 0;
}
