#pragma once

#include <iostream>
#include "Nullable.h"
#include "UTFail.h"

using namespace std;
using namespace MySQLWrap;

class TestNullable;
typedef void (TestNullable::*NullableMemberPointer)();

class TestNullable {
public:
	TestNullable();
	int RunTests();
private:
	void Test1();
	void Test2();
	void Test3();
	void Test4();
	void Test5();
	void Test6();
	void Test7();

	int RunSpecificTest(NullableMemberPointer test);
};
