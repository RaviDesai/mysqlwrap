#pragma once

#include <iostream>
#include "Nullable.h"
#include "UTFail.h"

using namespace MySQLWrap;

class TestAdhocParameter;
typedef void (TestAdhocParameter::*AdhocParameterMemberPointer)();

class TestAdhocParameter {
public:
	TestAdhocParameter();
	int RunTests();
private:
	void Test1();
	void Test2();
	void Test3();
	void Test4();
	void Test5();
	void Test6();
	void Test7();

	int RunSpecificTest(AdhocParameterMemberPointer test);
};
