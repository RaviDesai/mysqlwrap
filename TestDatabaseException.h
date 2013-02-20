#pragma once

#include <iostream>
#include "DatabaseException.h"
#include "UTFail.h"

using namespace std;
using namespace MySQLWrap;

class TestDatabaseException;
typedef void (TestDatabaseException::*DatabaseExceptionMemberPointer)();

class TestDatabaseException {
public:
	TestDatabaseException();
	int RunTests();
private:
	void Test1();
	void Test2();
	void Test3();
	void Test4();
	void Test5();
	void Test6();
	void Test7();

	int RunSpecificTest(DatabaseExceptionMemberPointer test);
};
