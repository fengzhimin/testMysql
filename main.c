/******************************************************
* Author       : fengzhimin
* Create       : 2017-08-12 15:34
* Last modified: 2017-08-12 15:34
* Email        : 374648064@qq.com
* Filename     : main.c
* Description  : 
******************************************************/
#include <stdio.h>
#include <mysql.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include "strOper.h"
#include "confOper.h"

char config_value[CONFIG_VALUE_MAX_NUM];
char bind_address[CONFIG_VALUE_MAX_NUM];
int port = 3306;
char user[CONFIG_VALUE_MAX_NUM];
char pass[CONFIG_VALUE_MAX_NUM];
char database[CONFIG_VALUE_MAX_NUM];
int insertNum = 100, selectNum = 100, updateNum = 100, deleteNum = 100;
int insertThreadNum = 100, updateThreadNum = 100, selectThreadNum= 100, deleteThreadNum = 100;
char createTableStr1[] = "create table stu1(name varchar(30), stunum int)";
char createTableStr2[] = "create table stu2(name varchar(30), stunum int)";
char createTableStr3[] = "create table stu3(name varchar(30), stunum int)";
char deleteTableStr1[] = "drop table stu1";
char deleteTableStr2[] = "drop table stu2";
char deleteTableStr3[] = "drop table stu3";
char selectStr[] = "select * from stu1 where stunum=";
char insertStr1[] = "insert into stu1 value('insert', ";
char insertStr2[] = "insert into stu2 value('insert', ";
char insertStr3[] = "insert into stu3 value('insert', ";
char updateStr[] = "update stu2 set name='update' where stunum=";
char deleteStr[] = "delete from stu3 where stunum=";

//创建一个临时表
void createTable();
//删除创建的临时表
void deleteTable();
//处理插入数据的线程
void* threadFuncInsert(void *arg);
//处理更新数据的线程
void* threadFuncUpdate(void *arg);
//处理查询数据的线程
void* threadFuncSelect(void *arg);
//处理删除数据的线程
void* threadFuncDelete(void *arg);

int main(int argc, char *argv[])
{
	//读取配置项
 	if(getConfValueByLabelAndKey("mysqlInfo", "insertNum", config_value))
		insertNum = StrToInt(config_value);
 	if(getConfValueByLabelAndKey("mysqlInfo", "updateNum", config_value))
		updateNum = StrToInt(config_value);
 	if(getConfValueByLabelAndKey("mysqlInfo", "selectNum", config_value))
		selectNum = StrToInt(config_value);
 	if(getConfValueByLabelAndKey("mysqlInfo", "deleteNum", config_value))
		deleteNum = StrToInt(config_value);
	if(getConfValueByLabelAndKey("threadInfo", "insertThread", config_value))
		insertThreadNum = StrToInt(config_value);
	if(getConfValueByLabelAndKey("threadInfo", "updateThread", config_value))
		updateThreadNum = StrToInt(config_value);
	if(getConfValueByLabelAndKey("threadInfo", "selectThread", config_value))
		selectThreadNum = StrToInt(config_value);
	if(getConfValueByLabelAndKey("threadInfo", "deleteThread", config_value))
		deleteThreadNum = StrToInt(config_value);
	if(getConfValueByLabelAndKey("mysqlInfo", "bind_address", config_value))
		strcpy(bind_address, config_value);
	if(getConfValueByLabelAndKey("mysqlInfo", "port", config_value))
		port = StrToInt(config_value);
	if(getConfValueByLabelAndKey("mysqlInfo", "user", config_value))
		strcpy(user, config_value);
	if(getConfValueByLabelAndKey("mysqlInfo", "pass", config_value))
		strcpy(pass, config_value);
	if(getConfValueByLabelAndKey("mysqlInfo", "database", config_value))
		strcpy(database, config_value);
	srand(time(0));

	mysql_library_init(0, NULL, NULL);
	createTable();

	pthread_t *insertThreads = (pthread_t *)malloc(sizeof(pthread_t)*insertThreadNum);
	pthread_t *updateThreads = (pthread_t *)malloc(sizeof(pthread_t)*updateThreadNum);
	pthread_t *selectThreads = (pthread_t *)malloc(sizeof(pthread_t)*selectThreadNum);
	pthread_t *deleteThreads = (pthread_t *)malloc(sizeof(pthread_t)*deleteThreadNum);
	int i, err;
	for(i = 0; i < insertThreadNum; i++)
	{
		err = pthread_create(&insertThreads[i], NULL, threadFuncInsert, NULL);
		if(err != 0)
			printf("create insertThread failed\n");
	}
	for(i = 0; i < updateThreadNum; i++)
	{
		err = pthread_create(&updateThreads[i], NULL, threadFuncUpdate, NULL);
		if(err != 0)
			printf("create updateThread failed\n");
	}
	for(i = 0; i < selectThreadNum; i++)
	{
		err = pthread_create(&selectThreads[i], NULL, threadFuncSelect, NULL);
		if(err != 0)
			printf("create selectThread failed\n");
	}
	for(i = 0; i < deleteThreadNum; i++)
	{
		err = pthread_create(&deleteThreads[i], NULL, threadFuncDelete, NULL);
		if(err != 0)
			printf("create deleteThread failed\n");
	}

	for(i = 0; i < insertThreadNum; i++)
		pthread_join(insertThreads[i], NULL);
	for(i = 0; i < updateThreadNum; i++)
		pthread_join(updateThreads[i], NULL);
	for(i = 0; i < selectThreadNum; i++)
		pthread_join(selectThreads[i], NULL);
	for(i = 0; i < deleteThreadNum; i++)
		pthread_join(deleteThreads[i], NULL);

	deleteTable();
	mysql_library_end();
	free(insertThreads);
	free(updateThreads);
	free(selectThreads);
	free(deleteThreads);

	return 0;
}

void createTable()
{
	MYSQL db;
	MYSQL *mysqlConnect;
	mysqlConnect = mysql_init(&db);
	if(mysqlConnect == NULL)
		printf("init failed\n");
	if(NULL == mysql_real_connect((MYSQL *)mysqlConnect, bind_address, user, pass, database, port, NULL, 0))
	{
		printf("connect failed: %s\n", mysql_error(mysqlConnect));
		return ;
	}

	if(mysql_real_query(mysqlConnect, createTableStr1, strlen(createTableStr1)) != 0)
	{
		printf("create table1 failed: %s\n", mysql_error(mysqlConnect));
	}
	if(mysql_real_query(mysqlConnect, createTableStr2, strlen(createTableStr2)) != 0)
	{
		printf("create table2 failed: %s\n", mysql_error(mysqlConnect));
	}
	if(mysql_real_query(mysqlConnect, createTableStr3, strlen(createTableStr3)) != 0)
	{
		printf("create table3 failed: %s\n", mysql_error(mysqlConnect));
	}
		
	mysql_close(mysqlConnect);
}

void deleteTable()
{
	MYSQL db;
	MYSQL *mysqlConnect;
	mysqlConnect = mysql_init(&db);
	if(mysqlConnect == NULL)
		printf("init failed\n");
	if(NULL == mysql_real_connect((MYSQL *)mysqlConnect, bind_address, user, pass, database, port, NULL, 0))
	{
		printf("connect failed: %s\n", mysql_error(mysqlConnect));
		return ;
	}

	if(mysql_real_query(mysqlConnect, deleteTableStr1, strlen(deleteTableStr1)) != 0)
	{
		printf("delete table1 failed: %s\n", mysql_error(mysqlConnect));
	}
	if(mysql_real_query(mysqlConnect, deleteTableStr2, strlen(deleteTableStr2)) != 0)
	{
		printf("delete table2 failed: %s\n", mysql_error(mysqlConnect));
	}
	if(mysql_real_query(mysqlConnect, deleteTableStr3, strlen(deleteTableStr3)) != 0)
	{
		printf("delete table3 failed: %s\n", mysql_error(mysqlConnect));
	}
		
	mysql_close(mysqlConnect);
}

void* threadFuncInsert(void *arg)
{
	MYSQL db;
	MYSQL *mysqlConnect;
	mysqlConnect = mysql_init(&db);
	if(mysqlConnect == NULL)
		printf("init failed\n");
	if(NULL == mysql_real_connect((MYSQL *)mysqlConnect, bind_address, user, pass, database, port, NULL, 0))
	{
		printf("insert Operation connect failed: %s\n", mysql_error(mysqlConnect));
		return 0;
	}

	char insert[100];
	char randNum[20];
	int i;
	for(i = 0; i < insertNum; i++)
	{
		int num = rand()%1000000000;
		memset(randNum, 0, 20);
		IntToStr(randNum, num);
		sprintf(insert, "%s %s)", insertStr1, randNum);
		if(mysql_real_query(mysqlConnect, insert, strlen(insert)) != 0)
		{
			printf("%s\n", insert);
			printf("insert1 failed: %s\n", mysql_error(mysqlConnect));
		}
		num = rand()%1000000000;
		memset(randNum, 0, 20);
		IntToStr(randNum, num);
		sprintf(insert, "%s %s)", insertStr2, randNum);
		if(mysql_real_query(mysqlConnect, insert, strlen(insert)) != 0)
		{
			printf("insert2 failed: %s\n", mysql_error(mysqlConnect));
		} 
		num = rand()%1000000000;
		memset(randNum, 0, 20);
		IntToStr(randNum, num);
		sprintf(insert, "%s %s)", insertStr3, randNum);
		if(mysql_real_query(mysqlConnect, insert, strlen(insert)) != 0)
		{
			printf("insert3 failed: %s\n", mysql_error(mysqlConnect));
		} 
	}
		
	mysql_close(mysqlConnect);

	pthread_exit(0);
}

void* threadFuncUpdate(void *arg)
{
	MYSQL db;
	MYSQL *mysqlConnect;
	mysqlConnect = mysql_init(&db);
	if(mysqlConnect == NULL)
		printf("init failed\n");
	if(NULL == mysql_real_connect((MYSQL *)mysqlConnect, bind_address, user, pass, database, port, NULL, 0))
	{
		printf("update Operation connect failed: %s\n", mysql_error(mysqlConnect));
		return 0;
	}

	char update[100];
	char randNum[20];
	int i;
	for(i = 0; i < updateNum; i++)
	{
		int num = rand()%1000000000;
		memset(randNum, 0, 20);
		IntToStr(randNum, num);
		sprintf(update, "%s%s", updateStr, randNum);
		if(mysql_real_query(mysqlConnect, update, strlen(update)) != 0)
		{
			printf("update failed: %s\n", mysql_error(mysqlConnect));
		}
	}
	mysql_close(mysqlConnect);

	pthread_exit(0);
}

void* threadFuncSelect(void *arg)
{
	MYSQL_RES *res_ptr;
	MYSQL_ROW sqlrow;
	MYSQL db;
	MYSQL *mysqlConnect;
	mysqlConnect = mysql_init(&db);
	if(mysqlConnect == NULL)
		printf("init failed\n");
	if(NULL == mysql_real_connect((MYSQL *)mysqlConnect, bind_address, user, pass, database, port, NULL, 0))
	{
		printf("select Operation connect failed: %s\n", mysql_error(mysqlConnect));
		return 0;
	}

	char select[100];
	char randNum[20];
	int i;
	for(i = 0; i < selectNum; i++)
	{
		int num = rand()%1000000000;
		memset(randNum, 0, 20);
		IntToStr(randNum, num);
		sprintf(select, "%s%s", selectStr, randNum);
		if(mysql_real_query(mysqlConnect, select, strlen(select)) != 0)
		{
			printf("%s\n", select);
			printf("select failed: %s\n", mysql_error(mysqlConnect));
		}
		else
		{
			res_ptr = mysql_store_result(mysqlConnect);
			int rownum = mysql_num_rows(res_ptr);
			if(rownum != 0)
				printf("select count = %d\n", rownum);
			if(res_ptr)
			{
				while((sqlrow = mysql_fetch_row(res_ptr)));
				mysql_free_result(res_ptr);
			}
		}
	}
	mysql_close(mysqlConnect);

	pthread_exit(0);
}

void* threadFuncDelete(void *arg)
{
	MYSQL db;
	MYSQL *mysqlConnect;
	mysqlConnect = mysql_init(&db);
	if(mysqlConnect == NULL)
		printf("init failed\n");
	if(NULL == mysql_real_connect((MYSQL *)mysqlConnect, bind_address, user, pass, database, port, NULL, 0))
	{
		printf("delete Operation connect failed: %s\n", mysql_error(mysqlConnect));
		return 0;
	}

	char delete[100];
	char randNum[20];
	int i;
	for(i = 0; i < deleteNum; i++)
	{
		int num = rand()%1000000000;
		memset(randNum, 0, 20);
		IntToStr(randNum, num);
		sprintf(delete, "%s%s", deleteStr, randNum);
		if(mysql_real_query(mysqlConnect, delete, strlen(delete)) != 0)
		{
			printf("delete failed: %s\n", mysql_error(mysqlConnect));
		}
	}
	mysql_close(mysqlConnect);

	pthread_exit(0);
}
