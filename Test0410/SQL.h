#pragma once
#include "/usr/include/mysql/mysql.h"

#define SQL_ADDRESS "localhost"
#define SQL_ID		"root"
#define SQL_PW		"password"

//지금 연결된 SQL입니다!
MYSQL* SQLConnection;

//SQL한테 "질문"을 합니다! SQL은 저희한테 "대답"해주죠!
MYSQL_RES* SQLResponse;

//검색을 해서 결과로 나온 줄을 여기에다가 둡시다!
MYSQL_ROW resultRow;

//MYSQL에 실제로 연결하는 함수가 필요할 거에요!
int SQLConnect()
{
	//제일 먼저 저희가 해야하는 것은 mySQL에 연결하기 전에 "초기화"를 시도합니다!
	if (!(SQLConnection = mysql_init((MYSQL*)NULL)))
	{
		cout << "MYSQL Initiate Failed" << endl;
		return -1;
	};

	//초기화를 했으니까 그 위치에다가 "실제 연결"을 시키는 겁니다!
	if (!(mysql_real_connect(SQLConnection, SQL_ADDRESS,  SQL_ID, SQL_PW, NULL, 3306, NULL, 0)))
	{                    //   대상 포인터   주소(내 컴퓨터) 아이디   비번          포트
		cout << "MYSQL Connection Failed" << endl;
		return -1;
	};

	cout << "MYSQL Connection Succeed" << endl;

	//어..? login_info가 없네요?
	if (mysql_query(SQLConnection, "USE Project9") != 0)
	{
		//없으면 만들면 되지!
		mysql_query(SQLConnection, "CREATE DATABASE Project9");
		mysql_query(SQLConnection, "USE Project9");
		mysql_query(SQLConnection, "CREATE TABLE UserData(ID VARCHAR(24) PRIMARY KEY, PW VARCHAR(24), NAME VARCHAR(24))");

		cout << "Table Created" << endl;
	};

	return 0;
}