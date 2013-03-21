#pragma once

#include <iostream>
#include "Nullable.h"
#include "UTFail.h"

using namespace MySQLWrap;

class TestJulian;
typedef void (TestJulian::*JulianMemberPointer)();

class TestJulian {
public:
	TestJulian();
	int RunTests();
private:
	void Test1();
	void Test2();
	void Test3();
	void Test4();
	void Test5();
	void Test6();
	void Test7();

	int RunSpecificTest(JulianMemberPointer test);
};
