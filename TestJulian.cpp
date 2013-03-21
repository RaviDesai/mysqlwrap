#include <iostream>
#include <iomanip>
#include "UTFail.h"
#include "TestJulian.h"
#include "Julian.h"

using namespace std;

TestJulian::TestJulian() {}

void TestJulian::Test1() {
	cout << __PRETTY_FUNCTION__ << endl;
	
	Julian j(1969, 07, 20, 20, 18, 0, 0);
	UTASSERT(j.Value() > 2440423.345833333 - 0.000001 && j.Value() < 2440423.345833333 + 0.000001);

	GregorianBreakdown gb = j.to_gregorian(0);

	UTASSERT(gb.year == 1969);
	UTASSERT(gb.month == 07);
	UTASSERT(gb.day == 20);
	UTASSERT(gb.hour == 20);
	UTASSERT(gb.minute == 18);
	UTASSERT(gb.second == 0);
	UTASSERT(gb.millisecond == 0);
	UTASSERT(gb.minutes_west_utc == 0);

	Julian one(-4713, 11, 25, 12, 0, 0, 0);
	UTASSERT(one.Value() > 0.999999 && one.Value() < 1.000001);

	gb.year = 2013;
	gb.month = 03;
	gb.day = 20;
	gb.hour = 6;
	gb.minute = 53;
	gb.second = 12;
	gb.millisecond = 540;
	gb.minutes_west_utc = 300;

	Julian mar20(gb);
	gb = mar20.to_gregorian(0);

	UTASSERT(gb.year == 2013);
	UTASSERT(gb.month = 03);
	UTASSERT(gb.day == 20);
	UTASSERT(gb.hour == 11);
	UTASSERT(gb.minute == 53);
	UTASSERT(gb.second == 12);
	UTASSERT(gb.millisecond == 540);
	UTASSERT(gb.minutes_west_utc == 0);

	gb = mar20.to_gregorian(300);

	UTASSERT(gb.year == 2013);
	UTASSERT(gb.month = 03);
	UTASSERT(gb.day == 20);
	UTASSERT(gb.hour == 06);
	UTASSERT(gb.minute == 53);
	UTASSERT(gb.second == 12);
	UTASSERT(gb.millisecond == 540);
	UTASSERT(gb.minutes_west_utc == 300);
}

void TestJulian::Test2() {
	cout << __PRETTY_FUNCTION__ << endl;

}

void TestJulian::Test3() {
	cout << __PRETTY_FUNCTION__ << endl;

}

void TestJulian::Test4() {
	cout << __PRETTY_FUNCTION__ << endl;

}

void TestJulian::Test5() {
	cout << __PRETTY_FUNCTION__ << endl;

}


void TestJulian::Test6() {
	cout << __PRETTY_FUNCTION__ << endl;

}

void TestJulian::Test7() {
	cout << __PRETTY_FUNCTION__ << endl;

}

int TestJulian::RunSpecificTest(JulianMemberPointer test) {
	int failures = 0;
	try {
		(this->*test)();
	} catch (UTFail &fail) {
		failures++;
		cout << fail << endl;
	}
	return failures;
}

int TestJulian::RunTests() {
	int failures = 0;
	failures += RunSpecificTest(&TestJulian::Test1);
	failures += RunSpecificTest(&TestJulian::Test2);
	failures += RunSpecificTest(&TestJulian::Test3);
	failures += RunSpecificTest(&TestJulian::Test4);
	failures += RunSpecificTest(&TestJulian::Test5);
	failures += RunSpecificTest(&TestJulian::Test6);
	failures += RunSpecificTest(&TestJulian::Test7);
	return failures;
}
