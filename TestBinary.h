#pragma once

#include <iostream>
#include "Binary.h"
#include "UTFail.h"

using namespace std;
using namespace MySQLWrap;

class TestBinary;
typedef void (TestBinary::*BinaryMemberPointer)();

class TestBinary {
public:
	TestBinary();
	int RunTests();
private:
	void Test1();
	void Test2();
	void Test3();
	void Test4();
	void Test5();
	void Test6();
	void Test7();

	int RunSpecificTest(BinaryMemberPointer test);
};
