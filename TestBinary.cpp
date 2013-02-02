#include <iostream>
#include "UTFail.h"
#include "TestBinary.h"

using namespace std;

TestBinary::TestBinary() {}

void TestBinary::Test1() {
	cout << "in test1" << endl;
	Binary b1;
	UTASSERT(b1.BufferLength() == 0);
	UTASSERT(b1.BufferSize() == 0);
	UTASSERT(b1.Buffer() == NULL);

	unsigned char *buffer = (unsigned char *)malloc(10);
	for (unsigned int i = 0; i < 10; i++) {
		buffer[i] = i + 1;
	}
	
	Binary b2;
	b2.SubsumeBuffer(buffer, 10, 10);
	UTASSERT(b2.BufferLength() == 10);
	UTASSERT(b2.BufferSize() == 10);
	UTASSERT(b2.Buffer() == buffer);

	b1.AssignDataToBuffer(buffer, 10);
	UTASSERT(b1.BufferLength() == 10);
	UTASSERT(b1.BufferSize() == 10);
	UTASSERT(b1.Buffer() != buffer);
}

void TestBinary::Test2() {
	cout << "in test2" << endl;

	unsigned char *buffer = (unsigned char *)malloc(10);
	for (unsigned int i = 0; i < 10; i++) {
		buffer[i] = i + 1;
	}

	Binary b1(buffer, 10, 10);

	Binary b2(b1);	
	UTASSERT(b2.BufferLength() == 10);
	UTASSERT(b2.BufferSize() == 10);
	UTASSERT(b2.Buffer() == buffer);
	UTASSERT(b1.BufferLength() == 0);
	UTASSERT(b1.BufferSize() == 0);
	UTASSERT(b1.Buffer() == NULL);
}

void TestBinary::Test3() {
	cout << "in test3" << endl;
	
	unsigned char *buffer = (unsigned char *)malloc(10);
	for (unsigned int i = 0; i < 10; i++) {
		buffer[i] = i + 1;
	}

	Binary b1(buffer, 10, 10);
	Binary b2;
	b2.AssignDataToBuffer(buffer, 10);

	UTASSERT(b1.Buffer() != b2.Buffer());
	UTASSERT(b1.BufferLength() == 10);
	UTASSERT(b2.BufferLength() == 10);

	b2.ResizeBuffer(100);
	UTASSERT(b2.BufferLength() == 10);
	UTASSERT(b2.BufferSize() == 100);

	for (int i = 0; i < 10; i++) {
		UTASSERT(b1.Buffer()[i] == b2.Buffer()[i]);
	}

	UTASSERT(b1 == b2);
}

void TestBinary::Test4() {
	cout << "in test4" << endl;

	Binary b1(128);
	UTASSERT(b1.Buffer() != NULL);
	UTASSERT(b1.BufferLength() == 0);
	UTASSERT(b1.BufferSize() == 128);

	unsigned char *buffer = (unsigned char *)malloc(10);
	for (unsigned int i = 0; i < 10; i++) {
		buffer[i] = i + 1;
	}

	Binary b2(buffer, 10, 10);	

	b1.AssignDataToBuffer(buffer, 10);
	UTASSERT(b1.BufferLength() == 10);
	UTASSERT(b1.BufferSize() == 128);

	//Sizing too small should have no effect
	b1.ResizeBuffer(5);
	UTASSERT(b1.BufferLength() == 10);
	UTASSERT(b1.BufferSize() == 128);
	UTASSERT(b1 == b2);

	b1.ResizeBuffer(256);
	UTASSERT(b1.BufferSize() == 256)
	UTASSERT(b1 == b2);
}

void TestBinary::Test5() {
	cout << "in test5" << endl;

	unsigned char *buffer = (unsigned char *)malloc(10);
	for (unsigned int i = 0; i < 10; i++) {
		buffer[i] = i + 1;
	}

	Binary b1(buffer, 10, 10);
	b1.ClearBuffer();

	UTASSERT(b1.Buffer() == NULL);
	UTASSERT(b1.BufferLength() == 0);
	UTASSERT(b1.BufferSize() == 0);
}


void TestBinary::Test6() {
	cout << "in test6" << endl;
}

void TestBinary::Test7() {
	cout << "in test7" << endl;
}

int TestBinary::RunSpecificTest(BinaryMemberPointer test) {
	int failures = 0;
	try {
		(this->*test)();
	} catch (UTFail &fail) {
		failures++;
		cout << fail << endl;
	}
	return failures;
}

int TestBinary::RunTests() {
	int failures = 0;
	failures += RunSpecificTest(&TestBinary::Test1);
	failures += RunSpecificTest(&TestBinary::Test2);
	failures += RunSpecificTest(&TestBinary::Test3);
	failures += RunSpecificTest(&TestBinary::Test4);
	failures += RunSpecificTest(&TestBinary::Test5);
	failures += RunSpecificTest(&TestBinary::Test6);
	failures += RunSpecificTest(&TestBinary::Test7);
	return failures;
}
