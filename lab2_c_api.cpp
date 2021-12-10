
#include <stdio.h>
#include <WinSock.h>  //一定要包含这个，或者winsock2.h
#include "mysql.h"    //引入mysql头文件(一种方式是在vc目录里面设置，一种是文件夹拷到工程目录，然后这样包含)
#include <Windows.h>
 
//包含附加依赖项，也可以在工程--属性里面设置
#pragma comment(lib,"wsock32.lib")
#pragma comment(lib,"libmysql.lib")
MYSQL mysql; //mysql连接
MYSQL_FIELD *fd;  //字段列数组
char field[32][32];  //存字段名二维数组
MYSQL_RES *res; //这个结构代表返回行的一个查询结果集
MYSQL_ROW column; //一个行数据的类型安全(type-safe)的表示，表示数据行的列
char query[150]; //查询语句
 
bool ConnectDatabase();     //函数声明
void FreeConnect();
bool QueryDatabase1();  //查询1
bool QueryDatabase2();  //查询2
bool QueryDatabase3();  //查询2
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
	strncpy(str_data, "业务科", STRING_SIZE); /* string  */
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
//连接数据库
bool ConnectDatabase()
{
	//初始化mysql
	mysql_init(&mysql);  //连接mysql，数据库
 
	//返回false则连接失败，返回true则连接成功
	if (!(mysql_real_connect(&mysql,"localhost", "root", "123456", "OrderDB",3306,NULL,0))) //中间分别是主机，用户名，密码，数据库名，端口号（可以写默认0或者3306等），可以先写成参数再传进去
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
//释放资源
void FreeConnect()
{
	//释放资源
	mysql_free_result(res);
	mysql_close(&mysql);
}
/***************************数据库操作***********************************/
//其实所有的数据库操作都是先写个sql语句，然后用mysql_query(&mysql,query)来完成，包括创建数据库或表，增删改查
//查询数据
bool QueryDatabase1()
{
	sprintf(query, "select employeeNo,employeeName,salary from employee order by salary desc"); //执行查询语句，这里是查询所有，user是表名，不用加引号，用strcpy也可以
	mysql_query(&mysql,"set names gbk"); //设置编码格式（SET NAMES GBK也行），否则cmd下中文乱码
	//返回0 查询成功，返回1查询失败
	if(mysql_query(&mysql, query))        //执行SQL语句
	{
		printf("Query failed (%s)\n",mysql_error(&mysql));
		return false;
	}
	else
	{
		printf("query success\n");
	}
	//获取结果集
	if (!(res=mysql_store_result(&mysql)))    //获得sql语句结束后返回的结果集
	{
		printf("Couldn't get result from %s\n", mysql_error(&mysql));
		return false;
	}
 
	//打印数据行数
	printf("number of dataline returned: %d\n",mysql_affected_rows(&mysql));
 
	//获取字段的信息
	char *str_field[32];  //定义一个字符串数组存储字段信息
	for(int i=0;i<3;i++)   //在已知字段数量的情况下获取字段名
	{
		str_field[i]=mysql_fetch_field(res)->name;
	}
	for(int i=0;i<4;i++)   //打印字段
		printf("%10s\t",str_field[i]);
	printf("\n");
	//打印获取的数据
	while (column = mysql_fetch_row(res))   //在已知字段数量情况下，获取并打印下一行
	{
		printf("%10s\t%10s\t%10s\t%10s\n", column[0], column[1], column[2],column[3]);  //column是列数组
	}
	return true;
}
bool QueryDatabase2()
{
	mysql_query(&mysql,"set names gbk"); //返回0 查询成功，返回1查询失败
	if(mysql_query(&mysql, "select employeeNo,employeeName,salary from employee order by salary desc"))        //执行SQL语句
	{
		printf("Query failed (%s)\n",mysql_error(&mysql));
		return false;
	}
	else
	{
		printf("query success\n");
	}
	res=mysql_store_result(&mysql);
	//打印数据行数
	printf("number of dataline returned: %d\n",mysql_affected_rows(&mysql));
	for(int i=0;fd=mysql_fetch_field(res);i++)  //获取字段名
		strcpy(field[i],fd->name);
	int j=mysql_num_fields(res);  // 获取列数
	for(int i=0;i<j;i++)  //打印字段
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
	mysql_query(&mysql,"set names gbk"); //返回0 查询成功，返回1查询失败
	if(mysql_query(&mysql, "select customerName,telephone,address from customer"))        //执行SQL语句
	{
		printf("Query failed (%s)\n",mysql_error(&mysql));
		return false;
	}
	else
	{
		printf("query success\n");
	}
	res=mysql_store_result(&mysql);
	//打印数据行数
	printf("number of dataline returned: %d\n",mysql_affected_rows(&mysql));
	for(int i=0;fd=mysql_fetch_field(res);i++)  //获取字段名
		strcpy(field[i],fd->name);
	int j=mysql_num_fields(res);  // 获取列数
	for(int i=0;i<j;i++)  //打印字段
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
//插入数据
bool InsertData()
{
	mysql_query(&mysql,"set names gbk");
	sprintf(query, "insert into customer values ('C20080002', '泰康股份有限公司', '010-5422685','天津市','220501');");  //可以想办法实现手动在控制台手动输入指令
	if(mysql_query(&mysql, query))        //执行SQL语句
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
//修改数据
bool ModifyData()
{

	sprintf(query, "update product set productPrice = 0.5*productPrice  where productPrice>1000");
	if(mysql_query(&mysql, query))        //执行SQL语句
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
//删除数据
bool DeleteData()
{
	/*sprintf(query, "delete from user where id=6");*/
	char query[100];
	sprintf(query, "delete from employee where salary>5000");	
	if(mysql_query(&mysql, query))        //执行SQL语句
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

