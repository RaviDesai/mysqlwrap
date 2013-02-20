#include <iostream>
#include <sstream>
#include <string.h>

#include "UTFail.h"
#include "TestImport.h"
#include "Database.h"
#include "DatabaseException.h"
#include "Statement.h"

#include <fstream>
#include <sys/stat.h>

using namespace std;
using namespace MySQLWrap;

TestImport::TestImport() {}

void TestImport::Test1() {
	cout << __PRETTY_FUNCTION__ << endl;
	
	char line[512];
	bool wasCaught = false;
	try {
		Database db("localhost", "", "", "", 0, NULL, 0);
		db.Connect();

		db.Execute("drop database if exists sakila");
	} catch (const DatabaseException &de) {
		std::stringstream ss;
		ss << de << endl;
		ss.getline(line, 512);
		wasCaught = true;
		cout << de << endl;
	}

	UTASSERT(! wasCaught);
}

void TestImport::Test2() {
	cout << __PRETTY_FUNCTION__ << endl;
	
	char line[512];
	bool wasCaught = false;
	try {
		Database db("localhost", "root", "", "", 0, NULL, 0);
		db.Connect();

		db.Source("/Users/ravidesai/Dropbox/cpp/mysqlwrap/sakila-db/sakila-schema.sql");

	} catch (const DatabaseException &de) {
		std::stringstream ss;
		ss << de << endl;
		ss.getline(line, 512);
		wasCaught = true;
		cout << de << endl;
	}

	UTASSERT(! wasCaught);
}

void TestImport::Test3() {
	cout << __PRETTY_FUNCTION__ << endl;
	
	char line[512];
	bool wasCaught = false;
	try {
		Database db("localhost", "root", "", "sakila", 0, NULL, 0);
		db.Connect();

		db.Source("/Users/ravidesai/Dropbox/cpp/mysqlwrap/sakila-db/sakila-data.sql");

	} catch (const DatabaseException &de) {
		std::stringstream ss;
		ss << de << endl;
		ss.getline(line, 512);
		wasCaught = true;
		cout << de << endl;
	}

	UTASSERT(! wasCaught);
}

void TestImport::Test4() {
	cout << __PRETTY_FUNCTION__ << endl;

	char line[512];
	bool wasCaught = false;
	try {
		Database db("localhost", "root", "", "sakila", 0, NULL, 0);
		db.Connect();

		db.Execute("select * from country");

	} catch (const DatabaseException &de) {
		std::stringstream ss;
		ss << de << endl;
		ss.getline(line, 512);
		wasCaught = true;
	}

	UTASSERT(wasCaught);
	UTASSERT(strcmp(line, "Error in Database::Execute ERROR 0(----) statements passed to Execute should never return results.") == 0);
}

void TestImport::Test5() {
	cout << __PRETTY_FUNCTION__ << endl;
}


void TestImport::Test6() {
	cout << __PRETTY_FUNCTION__ << endl;
}

void TestImport::Test7() {
	cout << __PRETTY_FUNCTION__ << endl;
}

int TestImport::RunSpecificTest(ImportMemberPointer test) {
	int failures = 0;
	try {
		(this->*test)();
	} catch (const UTFail &fail) {
		failures++;
		cout << fail << endl;
	} catch (...) {
		failures++;
		cout << "Some exception other than UTFAIL escaped from your UT!" << endl;
	}
	return failures;
}

int TestImport::RunTests(bool embedded) {
	int failures = 0;
	if (embedded) {
		failures += RunSpecificTest(&TestImport::Test1);
		failures += RunSpecificTest(&TestImport::Test2);
		failures += RunSpecificTest(&TestImport::Test3);
	}
	failures += RunSpecificTest(&TestImport::Test4);
	failures += RunSpecificTest(&TestImport::Test5);
	failures += RunSpecificTest(&TestImport::Test6);
	failures += RunSpecificTest(&TestImport::Test7);
	return failures;
}
