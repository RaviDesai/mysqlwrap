#pragma once

#include <iostream>
#include "UTFail.h"

using namespace std;

class TestImport;
typedef void (TestImport::*ImportMemberPointer)();

class TestImport {
public:
	TestImport();
	int RunTests(bool embedded);
private:
	void Test1();
	void Test2();
	void Test3();
	void Test4();
	void Test5();
	void Test6();
	void Test7();

	int RunSpecificTest(ImportMemberPointer test);
};
