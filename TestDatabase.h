#pragma once

#include <iostream>
#include "Database.h"
#include "UTFail.h"

using namespace std;
using namespace MySQLWrap;

class TestDatabase;
typedef void (TestDatabase::*DatabaseMemberPointer)();

class TestDatabase {
public:
	TestDatabase();
	int RunTests(bool embedded);
private:
	void Test1();
	void Test2();
	void Test2AdHoc();
	void Test2AdHoc2();
	void Test3();
	void Test4();
	void Test5();
	void Test6();
	void Test7();
	void Test8();
	void Test9();
	void Test10();
	void Test10AdHoc();
	void Test11();
	void Test11AdHoc();

	int RunSpecificTest(DatabaseMemberPointer test);
};
