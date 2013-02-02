#include <iostream>
#include <sstream>
#include <string.h>

#include "UTFail.h"
#include "TestDatabaseException.h"

#include <mysql.h>

using namespace std;

TestDatabaseException::TestDatabaseException() {}

void TestDatabaseException::Test1() {
	cout << "in test1" << endl;
	
	DatabaseException de("initial", 1, "9988", "end");
	stringstream ss;
	ss << de << endl;
	char line[512];
	ss.getline(line, 512);

	UTASSERT(strcmp(line, "initial ERROR 1(9988) end") == 0);
}

void TestDatabaseException::Test2() {
	cout << "in test2" << endl;

	MYSQL *db = mysql_init(NULL);
	UTASSERT(mysql_real_connect(db, "localhost", "baduser", "badpass", "sakila", 0, NULL, 0) == NULL)
	DatabaseException de(db, "initial");

	stringstream ss;
	ss << de;
	char line[512];
	ss.getline(line, 512);

	UTASSERT(strcmp(line, "initial ERROR 1045(28000) Access denied for user 'baduser'@'localhost' (using password: YES)") == 0);
	mysql_close(db);
}

void TestDatabaseException::Test3() {
	cout << "in test3" << endl;

	MYSQL *db = mysql_init(NULL);
	UTASSERT(mysql_real_connect(db, "localhost", "root", "", "sakila", 0, NULL, 0) != NULL)
	MYSQL_STMT *stmt = mysql_stmt_init(db);
	
	const char *stmt_str = "SELECT * FROM BADTABLE";
	UTASSERT(mysql_stmt_prepare(stmt, stmt_str, strlen(stmt_str)) != 0);
	DatabaseException de(stmt, "initial");

	stringstream ss;
	ss << de;
	char line[512];
	ss.getline(line, 512);

	UTASSERT(strcmp(line, "initial ERROR 1146(42S02) Table 'sakila.badtable' doesn't exist") == 0);
	mysql_stmt_close(stmt);
	mysql_close(db);
}

void TestDatabaseException::Test4() {
	cout << "in test4" << endl;
}

void TestDatabaseException::Test5() {
	cout << "in test5" << endl;
}


void TestDatabaseException::Test6() {
	cout << "in test6" << endl;
}

void TestDatabaseException::Test7() {
	cout << "in test7" << endl;
}

int TestDatabaseException::RunSpecificTest(DatabaseExceptionMemberPointer test) {
	int failures = 0;
	try {
		(this->*test)();
	} catch (UTFail &fail) {
		failures++;
		cout << fail << endl;
	}
	return failures;
}

int TestDatabaseException::RunTests() {
	int failures = 0;
	failures += RunSpecificTest(&TestDatabaseException::Test1);
	failures += RunSpecificTest(&TestDatabaseException::Test2);
	failures += RunSpecificTest(&TestDatabaseException::Test3);
	failures += RunSpecificTest(&TestDatabaseException::Test4);
	failures += RunSpecificTest(&TestDatabaseException::Test5);
	failures += RunSpecificTest(&TestDatabaseException::Test6);
	failures += RunSpecificTest(&TestDatabaseException::Test7);
	return failures;
}
