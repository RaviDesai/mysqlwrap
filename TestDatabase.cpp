#include <iostream>
#include <sstream>
#include <string.h>

#include "UTFail.h"
#include "TestDatabase.h"
#include "Database.h"
#include "DatabaseException.h"
#include "Statement.h"
#include "AdhocStatement.h"

#include <fstream>
#include <sys/stat.h>

using namespace std;

TestDatabase::TestDatabase() {}

void TestDatabase::Test1() {
	cout << __PRETTY_FUNCTION__ << endl;
	
	char line[512];
	bool wasCaught = false;
	try {
		Database db("localhost", "root", "", "baddb", 0, NULL, 0);
		db.Connect();
	} catch (const DatabaseException &de) {
		std::stringstream ss;
		ss << de << endl;
		ss.getline(line, 512);
		wasCaught = true;
	}

	UTASSERT(wasCaught);
	UTASSERT(strcmp(line, "Error in Database::Connect ERROR 1049(42000) Unknown database 'baddb'") == 0);
}

void TestDatabase::Test2AdHoc() {
	cout << __PRETTY_FUNCTION__ << endl;
	
	bool wasCaught = false;
	try {
		Database db("localhost", "root", "", "sakila", 0, NULL, 0);
		db.Connect();

		UTASSERT(db.IsConnected());	

		AdhocStatement stmt(db, "SELECT * from COUNTRY");
		stmt.Execute();

		int testId = 0;
		std::string lastCountry;
		
		while (stmt.FetchNextRow()) {
			testId++;
			Nullable<unsigned short int> countryId = stmt.GetUShortDataInRow(0);
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
	} catch (const UTFail &fail) {
		cout << fail << endl;
		wasCaught = true;
	} catch (...) { 
		cout << "random exception caught" << endl;
		wasCaught = true;
	}

	UTASSERT(! wasCaught);	
}

void TestDatabase::Test2() {
	cout << __PRETTY_FUNCTION__ << endl;

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
			Nullable<unsigned short int> countryId = stmt.GetUShortDataInRow(0);
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
	} catch (const UTFail &fail) {
		cout << fail << endl;
		wasCaught = true;
	} catch (...) { 
		cout << "random exception caught" << endl;
		wasCaught = true;
	}

	UTASSERT(! wasCaught);	
}

void TestDatabase::Test3() {
	cout << __PRETTY_FUNCTION__ << endl;

	bool wasCaught = false;
	try {
		Database db("localhost", "root", "", "sakila", 0, NULL, 0);
		db.Connect();

		UTASSERT(db.IsConnected());	

		Statement stmt(db, "SELECT * from COUNTRY where country_id = ?");
		stmt << Nullable<short int>(7) << execute;

		short int testId = 6;
		std::string lastCountry;
		
		while (stmt << fetch) {
			testId++;
			Nullable<unsigned short int> countryId;
			Nullable<std::string> countryName;
			Nullable<MYSQL_TIME> lastUpdate;

			stmt >> countryId >> countryName >> lastUpdate;

			UTASSERT(testId == (*countryId));	
			UTASSERT(lastUpdate->year == 2006);
			UTASSERT(lastUpdate->month == 2);
			UTASSERT(lastUpdate->day == 15);
			
			if (testId > 1) {
				UTASSERT(countryName.deref() > lastCountry);
			}
			lastCountry = countryName.deref();
		}

		UTASSERT(testId == 7);

	} catch (const DatabaseException &de) {
		cout << de << endl;
		wasCaught = true;
	}

	UTASSERT(! wasCaught);	
}

void TestDatabase::Test4() {
	cout << __PRETTY_FUNCTION__ << endl;

	bool caught = false;

	struct stat filestat;
	UTASSERT(stat("mike.jpg", &filestat) == 0);

	ifstream myFile("pic.jpg", ios::in | ios::binary);
	myFile.exceptions(std::ios::failbit);
	unsigned char *picBuf = (unsigned char *) malloc(filestat.st_size);
	myFile.read((char *)picBuf, filestat.st_size);
	myFile.close();

	Binary picFromDisk(picBuf, filestat.st_size, filestat.st_size);

	try {
		Database db("localhost", "root", "", "sakila", 0, NULL, 0);
		db.Connect();
		Statement stmt(db, "select picture from staff where first_name = ?");
		stmt << Nullable<std::string>("Mike") << execute;

		UTASSERT(stmt << fetch);
	
		Nullable<Binary> mikepic;
		stmt >> mikepic;
	
		UTASSERT(mikepic.HasValue());
		UTASSERT(mikepic->BufferLength() == 36365);
		UTASSERT((mikepic.const_deref()) == picFromDisk);
	} catch (const DatabaseException &de) {
		cout << de << endl;
		caught = true;
	} catch (const NullableException &ne) {
		cout << ne << endl;
		caught = true;
	}

	UTASSERT(! caught);
}

void TestDatabase::Test5() {
	cout << __PRETTY_FUNCTION__ << endl;

	bool wasCaught = false;
	try {
		Database db("localhost", "root", "", "sakila", 0, NULL, 0);
		db.Connect();

		UTASSERT(db.IsConnected());	

		Statement stmt(db, "SELECT * from COUNTRY where country_id = ?");
		stmt << Nullable<short int>(7) << execute;

		UTASSERT(stmt << fetch);
		UTASSERT(! stmt.Eof());

		Nullable<unsigned short int> countryId;
		Nullable<std::string> countryName;
		Nullable<MYSQL_TIME> lastUpdate;

		stmt >> countryId >> countryName >> lastUpdate;

		UTASSERT(7 == (*countryId));	
		UTASSERT(strcmp(countryName->c_str(), "Armenia") == 0);
		UTASSERT(lastUpdate->year == 2006);
		UTASSERT(lastUpdate->month == 2);
		UTASSERT(lastUpdate->day == 15);

		stmt << reset << Nullable<short int>(8) << execute;
		UTASSERT(stmt << fetch);
		UTASSERT(! stmt.Eof());

		stmt >> countryId >> countryName >> lastUpdate;

		UTASSERT(8 == (*countryId));	
		UTASSERT(strcmp(countryName->c_str(), "Australia") == 0);
		UTASSERT(lastUpdate->year == 2006);
		UTASSERT(lastUpdate->month == 2);
		UTASSERT(lastUpdate->day == 15);

	} catch (const DatabaseException &de) {
		cout << de << endl;
		wasCaught = true;
	}

	UTASSERT(! wasCaught);	
}


void TestDatabase::Test6() {
	cout << __PRETTY_FUNCTION__ << endl;

	bool wasCaught = false;
	try {
		Database db("localhost", "root", "", "sakila", 0, NULL, 0);
		db.Connect();

		UTASSERT(db.IsConnected());	

		Statement stmt(db, "SELECT film_id, release_year from FILM where film_id = ?");
		stmt << Nullable<unsigned short int>(100) << execute;

		UTASSERT(stmt << fetch);
		UTASSERT(! stmt.Eof());

		Nullable<unsigned short int> filmId;
		Nullable<unsigned short int> releaseYear;

		stmt >> filmId >> releaseYear;
		UTASSERT(*filmId == (unsigned short) 100);
		UTASSERT(*releaseYear == (unsigned short) 2006);

		stmt << reset << Nullable<unsigned short int>(101) << execute;

		UTASSERT(stmt << fetch);
		UTASSERT(! stmt.Eof());

		stmt >> filmId >> releaseYear;
		UTASSERT(*filmId == (unsigned short) 101);
		UTASSERT(*releaseYear == (unsigned short) 2006);

	} catch (const DatabaseException &de) {
		cout << de << endl;
		wasCaught = true;
	}

	UTASSERT(! wasCaught);	
}

void TestDatabase::Test7() {
	cout << __PRETTY_FUNCTION__ << endl;

	bool wasCaught = false;
	try {
		Database db("localhost", "root", "", "sakila", 0, NULL, 0);
		db.Connect();

		UTASSERT(db.IsConnected());	

		db.Execute("DROP PROCEDURE IF EXISTS  rewards_report2");

		db.Execute("CREATE PROCEDURE rewards_report2 ( IN min_monthly_purchases TINYINT UNSIGNED , IN min_dollar_amount_purchased DECIMAL(10,2) UNSIGNED , OUT count_rewardees INT ) LANGUAGE SQL NOT DETERMINISTIC READS SQL DATA SQL SECURITY DEFINER COMMENT 'Provides a customizable report on best customers' proc: BEGIN  DECLARE last_month_start DATE; DECLARE last_month_end DATE;   IF min_monthly_purchases = 0 THEN SELECT 'Minimum monthly purchases parameter must be > 0'; LEAVE proc; END IF; IF min_dollar_amount_purchased = 0.00 THEN SELECT 'Minimum monthly dollar amount purchased parameter must be > $0.00'; LEAVE proc; END IF;   SET last_month_start = DATE_SUB(CURRENT_DATE(), INTERVAL 1 MONTH); SET last_month_start = STR_TO_DATE(CONCAT(YEAR(last_month_start),'-',MONTH(last_month_start),'-01'),'%Y-%m-%d'); SET last_month_end = LAST_DAY(last_month_start);   CREATE TEMPORARY TABLE tmpCustomer (customer_id SMALLINT UNSIGNED NOT NULL PRIMARY KEY);   INSERT INTO tmpCustomer (customer_id) SELECT p.customer_id FROM payment AS p WHERE DATE(p.payment_date) BETWEEN last_month_start AND last_month_end GROUP BY customer_id HAVING SUM(p.amount) > min_dollar_amount_purchased AND COUNT(customer_id) > min_monthly_purchases;   SELECT COUNT(*) FROM tmpCustomer INTO count_rewardees;   SELECT c.* FROM tmpCustomer AS t INNER JOIN customer AS c ON t.customer_id = c.customer_id;   DROP TABLE tmpCustomer; END");

	} catch (const DatabaseException &de) {
		cout << de << endl;
		wasCaught = true;
	}

	UTASSERT(! wasCaught);	
}

void TestDatabase::Test8() {
	cout << __PRETTY_FUNCTION__ << endl;

	bool wasCaught = false;
	try {
		Database db_initial("localhost", "root", "", "sakila", 0, NULL, 0);
		Database db(db_initial);
		db.Connect();

		UTASSERT(db.IsConnected());	

		Statement stmt(db, "SELECT * from COUNTRY");
		stmt.Execute();

		int testId = 0;
		std::string lastCountry;
		
		while (stmt.FetchNextRow()) {
			testId++;
			Nullable<unsigned short int> countryId = stmt.GetUShortDataInRow(0);
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
	} catch (const UTFail &fail) {
		cout << fail << endl;
		wasCaught = true;
	} catch (...) { 
		cout << "random exception caught" << endl;
		wasCaught = true;
	}

	UTASSERT(! wasCaught);	
}

void TestDatabase::Test9() {
	cout << __PRETTY_FUNCTION__ << endl;
	bool wasCaught = false;
	try {
		Database db_initial("localhost", "root", "", "sakila", 0, NULL, 0);
		db_initial.Connect();
		Database db(db_initial);
		UTASSERT(db.IsConnected());
	} catch (const DatabaseException &de) {
		cout << de << endl;
		wasCaught = true;
	} catch (const UTFail &fail) {
		cout << fail << endl;
		wasCaught = true;
	} catch (...) { 
		cout << "random exception caught" << endl;
		wasCaught = true;
	}

	UTASSERT(! wasCaught);	
}

int TestDatabase::RunSpecificTest(DatabaseMemberPointer test) {
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

int TestDatabase::RunTests(bool embedded) {
	int failures = 0;
	if (embedded) {
		failures += RunSpecificTest(&TestDatabase::Test1);
		failures += RunSpecificTest(&TestDatabase::Test2AdHoc);
	} else {
		failures += RunSpecificTest(&TestDatabase::Test1);
		failures += RunSpecificTest(&TestDatabase::Test2AdHoc);
		failures += RunSpecificTest(&TestDatabase::Test2);
		failures += RunSpecificTest(&TestDatabase::Test3);
		failures += RunSpecificTest(&TestDatabase::Test4);
		failures += RunSpecificTest(&TestDatabase::Test5);
		failures += RunSpecificTest(&TestDatabase::Test6);
		failures += RunSpecificTest(&TestDatabase::Test7);
		failures += RunSpecificTest(&TestDatabase::Test8);
		failures += RunSpecificTest(&TestDatabase::Test9);
	}
	return failures;
}
