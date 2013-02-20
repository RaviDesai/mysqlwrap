#include <iostream>
#include <sstream>
#include <string.h>

#include "UTFail.h"
#include "TestDatabaseException.h"

#include <mysql.h>

using namespace std;

TestDatabaseException::TestDatabaseException() {}

void TestDatabaseException::Test1() {
	cout << __PRETTY_FUNCTION__ << endl;
	
	DatabaseException de("initial", 1, "9988", "end");
	stringstream ss;
	ss << de << endl;
	char line[512];
	ss.getline(line, 512);

	UTASSERT(strcmp(line, "initial ERROR 1(9988) end") == 0);
}

void TestDatabaseException::Test2() {
	cout << __PRETTY_FUNCTION__ << endl;

	MYSQL *db = mysql_init(NULL);
	UTASSERT(mysql_real_connect(db, "localhost", "root", "", "baddb", 0, NULL, 0) == NULL)
	DatabaseException de(db, "initial");

	stringstream ss;
	ss << de;
	char line[512];
	ss.getline(line, 512);

	UTASSERT(strcmp(line, "initial ERROR 1049(42000) Unknown database 'baddb'") == 0);
	mysql_close(db);
}

void TestDatabaseException::Test3() {
	cout << __PRETTY_FUNCTION__ << endl;

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
	cout << __PRETTY_FUNCTION__ << endl;
}

void TestDatabaseException::Test5() {
	cout << __PRETTY_FUNCTION__ << endl;
}


void TestDatabaseException::Test6() {
	cout << __PRETTY_FUNCTION__ << endl;
}

void TestDatabaseException::Test7() {
	cout << __PRETTY_FUNCTION__ << endl;
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
