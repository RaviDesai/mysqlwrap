#include <iostream>
#include <sstream>
#include <string.h>

#include "UTFail.h"
#include "TestDatabase.h"
#include "Database.h"
#include "DatabaseException.h"
#include "Statement.h"

using namespace std;

TestDatabase::TestDatabase() {}

void TestDatabase::Test1() {
	cout << "in test1" << endl;
	
	char line[512];
	bool wasCaught = false;
	try {
		Database db("localhost", "baduser", "badpass", "sakila", 0, NULL, 0);
		db.Connect();
	} catch (const DatabaseException &de) {
		std::stringstream ss;
		ss << de << endl;
		ss.getline(line, 512);
		wasCaught = true;
	}

	UTASSERT(wasCaught);
	UTASSERT(strcmp(line, "Error in Database::Connect ERROR 1045(28000) Access denied for user 'baduser'@'localhost' (using password: YES)") == 0);
}

void TestDatabase::Test2() {
	cout << "in test2" << endl;

	bool wasCaught = false;
	try {
		Database db("localhost", "root", "", "sakila", 0, NULL, 0);
		db.Connect();

		UTASSERT(db.IsConnected());	

		Statement stmt(db, "SELECT * from COUNTRY");
		stmt.Execute();

		int testId = 0;
		std::string lastCountry;
		
		while (stmt.FetchNextRow()) {
			testId++;
			Nullable<short int> countryId = stmt.GetShortDataInRow(0);
			Nullable<std::string> countryName = stmt.GetStringDataInRow(1);
			Nullable<MYSQL_TIME> lastUpdate = stmt.GetTimeDataInRow(2);

			UTASSERT(testId == (*countryId));	
			UTASSERT(lastUpdate->year == 2006);
			UTASSERT(lastUpdate->month == 2);
			UTASSERT(lastUpdate->day == 15);
			
			if (testId > 1) {
				UTASSERT(countryName.deref() > lastCountry);
			}
			lastCountry = countryName.deref();
		}

		UTASSERT(testId == 109);

	} catch (const DatabaseException &de) {
		cout << de << endl;
		wasCaught = true;
	}

	UTASSERT(! wasCaught);	
}

void TestDatabase::Test3() {
	cout << "in test3" << endl;
}

void TestDatabase::Test4() {
	cout << "in test4" << endl;
}

void TestDatabase::Test5() {
	cout << "in test5" << endl;
}


void TestDatabase::Test6() {
	cout << "in test6" << endl;
}

void TestDatabase::Test7() {
	cout << "in test7" << endl;
}

int TestDatabase::RunSpecificTest(DatabaseMemberPointer test) {
	int failures = 0;
	try {
		(this->*test)();
	} catch (UTFail &fail) {
		failures++;
		cout << fail << endl;
	}
	return failures;
}

int TestDatabase::RunTests() {
	int failures = 0;
	failures += RunSpecificTest(&TestDatabase::Test1);
	failures += RunSpecificTest(&TestDatabase::Test2);
	failures += RunSpecificTest(&TestDatabase::Test3);
	failures += RunSpecificTest(&TestDatabase::Test4);
	failures += RunSpecificTest(&TestDatabase::Test5);
	failures += RunSpecificTest(&TestDatabase::Test6);
	failures += RunSpecificTest(&TestDatabase::Test7);
	return failures;
}
