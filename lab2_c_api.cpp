
#include <stdio.h>
#include <WinSock.h>  //һ��Ҫ�������������winsock2.h
#include "mysql.h"    //����mysqlͷ�ļ�(һ�ַ�ʽ����vcĿ¼�������ã�һ�����ļ��п�������Ŀ¼��Ȼ����������)
#include <Windows.h>
 
//�������������Ҳ�����ڹ���--������������
#pragma comment(lib,"wsock32.lib")
#pragma comment(lib,"libmysql.lib")
MYSQL mysql; //mysql����
MYSQL_FIELD *fd;  //�ֶ�������
char field[32][32];  //���ֶ�����ά����
MYSQL_RES *res; //����ṹ�������е�һ����ѯ�����
MYSQL_ROW column; //һ�������ݵ����Ͱ�ȫ(type-safe)�ı�ʾ����ʾ�����е���
char query[150]; //��ѯ���
 
bool ConnectDatabase();     //��������
void FreeConnect();
bool QueryDatabase1();  //��ѯ1
bool QueryDatabase2();  //��ѯ2
bool QueryDatabase3();  //��ѯ2
bool InsertData();
bool ModifyData();
bool DeleteData();
void f();
#define STRING_SIZE 100
#define update_SAMPLE "update employee set salary = salary + 200 where department = ?"

int main(int argc,char **argv)
{
	ConnectDatabase();
	//QueryDatabase1();
	//InsertData();
	//QueryDatabase2();
	//ModifyData();
	//QueryDatabase2();
	//DeleteData();
	//QueryDatabase2();
	//f();
	QueryDatabase3();
	FreeConnect();
	system("pause");
	return 0;
}


void f(){
	mysql_query(&mysql,"set names gbk");
	MYSQL_BIND    bind[1];
	MYSQL_STMT    *stmt;
	my_ulonglong  affected_rows;
	unsigned long str_length;
	char          str_data[STRING_SIZE];
	int           param_count;
	//mysql_query(&mysql,"set names gbk");

	stmt = mysql_stmt_init(&mysql);
	if (!stmt)
	{
	  fprintf(stderr, " mysql_stmt_init(), out of memory\n");
	  system("pause");
	  exit(0);
	}
	if (mysql_stmt_prepare(stmt, update_SAMPLE, strlen(update_SAMPLE)))
	{
	  fprintf(stderr, " mysql_stmt_prepare(), INSERT failed\n");
	  fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
	  system("pause");
	  exit(0);
	}
	fprintf(stdout, " prepare, update successful\n");
	
	param_count= mysql_stmt_param_count(stmt);
	fprintf(stdout, " total parameters in INSERT: %d\n", param_count);
 
	if (param_count != 1) // validate parameter count 
	{
	  fprintf(stderr, " invalid parameter count returned by MySQL\n");
	  system("pause");
	  exit(0);
	}
	memset(bind, 0, sizeof(bind));
	bind[0].buffer_type= MYSQL_TYPE_STRING;
	bind[0].buffer= (char *)str_data;
	bind[0].buffer_length= STRING_SIZE;

	bind[0].is_null= 0;	

	bind[0].length= &str_length;
	/* Bind the buffers */

	if (!mysql_stmt_bind_param(stmt, bind))
	{
	  //fprintf(stderr, " %d\n", mysql_stmt_errno(stmt));
	  fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
	  
	  fprintf(stderr, " mysql_stmt_bind_param() failed\n");
	  
	  system("pause");
	  exit(0);
	}
	strncpy(str_data, "ҵ���", STRING_SIZE); /* string  */
	str_length= strlen(str_data);
	if (mysql_stmt_execute(stmt))
	{
	  fprintf(stderr, " mysql_stmt_execute(), 1 failed\n");
	  fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
	  system("pause");
	  exit(0);
	}
	affected_rows= mysql_stmt_affected_rows(stmt);
	fprintf(stdout, " total affected rows(insert 1): %lu\n",
                (unsigned long) affected_rows);
	if (mysql_stmt_close(stmt))
	{
	  fprintf(stderr, " failed while closing the statement\n");
	  fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
	  system("pause");
	  exit(0);
	}


}
//�������ݿ�
bool ConnectDatabase()
{
	//��ʼ��mysql
	mysql_init(&mysql);  //����mysql�����ݿ�
 
	//����false������ʧ�ܣ�����true�����ӳɹ�
	if (!(mysql_real_connect(&mysql,"localhost", "root", "123456", "OrderDB",3306,NULL,0))) //�м�ֱ����������û��������룬���ݿ������˿ںţ�����дĬ��0����3306�ȣ���������д�ɲ����ٴ���ȥ
	{
		printf( "Error connecting to database:%s\n",mysql_error(&mysql));
		return false;
	}
	else
	{
		printf("Connected...\n");
		return true;
	}
}
//�ͷ���Դ
void FreeConnect()
{
	//�ͷ���Դ
	mysql_free_result(res);
	mysql_close(&mysql);
}
/***************************���ݿ����***********************************/
//��ʵ���е����ݿ����������д��sql��䣬Ȼ����mysql_query(&mysql,query)����ɣ������������ݿ�����ɾ�Ĳ�
//��ѯ����
bool QueryDatabase1()
{
	sprintf(query, "select employeeNo,employeeName,salary from employee order by salary desc"); //ִ�в�ѯ��䣬�����ǲ�ѯ���У�user�Ǳ��������ü����ţ���strcpyҲ����
	mysql_query(&mysql,"set names gbk"); //���ñ����ʽ��SET NAMES GBKҲ�У�������cmd����������
	//����0 ��ѯ�ɹ�������1��ѯʧ��
	if(mysql_query(&mysql, query))        //ִ��SQL���
	{
		printf("Query failed (%s)\n",mysql_error(&mysql));
		return false;
	}
	else
	{
		printf("query success\n");
	}
	//��ȡ�����
	if (!(res=mysql_store_result(&mysql)))    //���sql�������󷵻صĽ����
	{
		printf("Couldn't get result from %s\n", mysql_error(&mysql));
		return false;
	}
 
	//��ӡ��������
	printf("number of dataline returned: %d\n",mysql_affected_rows(&mysql));
 
	//��ȡ�ֶε���Ϣ
	char *str_field[32];  //����һ���ַ�������洢�ֶ���Ϣ
	for(int i=0;i<3;i++)   //����֪�ֶ�����������»�ȡ�ֶ���
	{
		str_field[i]=mysql_fetch_field(res)->name;
	}
	for(int i=0;i<4;i++)   //��ӡ�ֶ�
		printf("%10s\t",str_field[i]);
	printf("\n");
	//��ӡ��ȡ������
	while (column = mysql_fetch_row(res))   //����֪�ֶ���������£���ȡ����ӡ��һ��
	{
		printf("%10s\t%10s\t%10s\t%10s\n", column[0], column[1], column[2],column[3]);  //column��������
	}
	return true;
}
bool QueryDatabase2()
{
	mysql_query(&mysql,"set names gbk"); //����0 ��ѯ�ɹ�������1��ѯʧ��
	if(mysql_query(&mysql, "select employeeNo,employeeName,salary from employee order by salary desc"))        //ִ��SQL���
	{
		printf("Query failed (%s)\n",mysql_error(&mysql));
		return false;
	}
	else
	{
		printf("query success\n");
	}
	res=mysql_store_result(&mysql);
	//��ӡ��������
	printf("number of dataline returned: %d\n",mysql_affected_rows(&mysql));
	for(int i=0;fd=mysql_fetch_field(res);i++)  //��ȡ�ֶ���
		strcpy(field[i],fd->name);
	int j=mysql_num_fields(res);  // ��ȡ����
	for(int i=0;i<j;i++)  //��ӡ�ֶ�
		printf("%10s\t",field[i]);
	printf("\n");
	while(column=mysql_fetch_row(res))
	{
		int k = 0;
		for(int i=0;i<j;i++)
			printf("%10s\t",column[i]);
		printf("\n");
		k++;
		if(k==20){
			break;
		}
	}
	return true;
}
bool QueryDatabase3()
{
	mysql_query(&mysql,"set names gbk"); //����0 ��ѯ�ɹ�������1��ѯʧ��
	if(mysql_query(&mysql, "select customerName,telephone,address from customer"))        //ִ��SQL���
	{
		printf("Query failed (%s)\n",mysql_error(&mysql));
		return false;
	}
	else
	{
		printf("query success\n");
	}
	res=mysql_store_result(&mysql);
	//��ӡ��������
	printf("number of dataline returned: %d\n",mysql_affected_rows(&mysql));
	for(int i=0;fd=mysql_fetch_field(res);i++)  //��ȡ�ֶ���
		strcpy(field[i],fd->name);
	int j=mysql_num_fields(res);  // ��ȡ����
	for(int i=0;i<j;i++)  //��ӡ�ֶ�
		printf("%10s\t",field[i]);
	printf("\n");
	while(column=mysql_fetch_row(res))
	{
		int k = 0;
		for(int i=0;i<j;i++)
			printf("%10s\t",column[i]);
		printf("\n");
		k++;
		if(k==20){
			break;
		}
	}
	return true;
}
//��������
bool InsertData()
{
	mysql_query(&mysql,"set names gbk");
	sprintf(query, "insert into customer values ('C20080002', '̩���ɷ����޹�˾', '010-5422685','�����','220501');");  //������취ʵ���ֶ��ڿ���̨�ֶ�����ָ��
	if(mysql_query(&mysql, query))        //ִ��SQL���
	{
		printf("Query failed (%s)\n",mysql_error(&mysql));
		return false;
	}
	else
	{
		printf("Insert success\n");
		return true;
	}
}
//�޸�����
bool ModifyData()
{

	sprintf(query, "update product set productPrice = 0.5*productPrice  where productPrice>1000");
	if(mysql_query(&mysql, query))        //ִ��SQL���
	{
		printf("Query failed (%s)\n",mysql_error(&mysql));
		return false;
	}
	else
	{
		printf("modify success\n");
		return true;
	}
}
//ɾ������
bool DeleteData()
{
	/*sprintf(query, "delete from user where id=6");*/
	char query[100];
	sprintf(query, "delete from employee where salary>5000");	
	if(mysql_query(&mysql, query))        //ִ��SQL���
	{
		printf("Query failed (%s)\n",mysql_error(&mysql));
		return false;
	}
	else
	{
		printf("delete success\n");
		return true;
	}
}

