#include <iostream>

#include "UTFail.h"

#include "TestNullable.h"
#include "TestBinary.h"
#include "TestDatabaseException.h"
#include "TestDatabase.h"
#include "TestImport.h"

using namespace std;
using namespace MySQLWrap;

int main(int argc, char** argv)
{
	cout << "Begin Test Suite" << endl;
	bool embedded = ((argc > 1) && (strcmp(argv[1], "embedded") == 0));
	cout << "embedded: " << embedded << endl;

	LibraryInitializer di(embedded);
	ThreadInitializer ti;

	int failures = 0;
	TestImport ts0;
	failures += ts0.RunTests(embedded);

	TestNullable ts1;
	failures += ts1.RunTests();

	TestBinary ts2;
	failures += ts2.RunTests();

	TestDatabaseException ts3;
	failures += ts3.RunTests(embedded);

	TestDatabase ts4;
	failures += ts4.RunTests(embedded);

	cout << "End Test Suite" << endl;

	return failures;
}
