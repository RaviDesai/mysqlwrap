#pragma once

#include <iostream>
#include "DatabaseException.h"
#include "UTFail.h"

using namespace std;
using namespace MySQLWrap;

class TestDatabaseException;
typedef void (TestDatabaseException::*DatabaseExceptionMemberPointer)(bool embedded);

class TestDatabaseException {
public:
	TestDatabaseException();
	int RunTests(bool embedded);
private:
	void Test1(bool embedded);
	void Test2(bool embedded);
	void Test3(bool embedded);
	void Test4(bool embedded);
	void Test5(bool embedded);
	void Test6(bool embedded);
	void Test7(bool embedded);

	int RunSpecificTest(DatabaseExceptionMemberPointer test, bool embedded);
};
