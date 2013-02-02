#pragma once

#include <iostream>
#include "Database.h"
#include "UTFail.h"

using namespace std;

class TestDatabase;
typedef void (TestDatabase::*DatabaseMemberPointer)();

class TestDatabase {
public:
	TestDatabase();
	int RunTests();
private:
	void Test1();
	void Test2();
	void Test3();
	void Test4();
	void Test5();
	void Test6();
	void Test7();

	int RunSpecificTest(DatabaseMemberPointer test);
};
