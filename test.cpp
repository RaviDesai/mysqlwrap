#include <iostream>

#include "UTFail.h"

#include "TestNullable.h"
#include "TestBinary.h"
#include "TestDatabaseException.h"
#include "TestDatabase.h"

using namespace std;

int main(int argc, char** argv)
{
	cout << "Begin Test Suite" << endl;

	TestNullable ts1;
	int failures = ts1.RunTests();

	TestBinary ts2;
	failures += ts2.RunTests();

	TestDatabaseException ts3;
	failures += ts3.RunTests();

	TestDatabase ts4;
	failures += ts4.RunTests();

	cout << "End Test Suite" << endl;

	return failures;
}
