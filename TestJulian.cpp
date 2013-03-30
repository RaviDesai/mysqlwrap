#include <iostream>
#include <iomanip>
#include <sstream>

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

	Julian sixHours(0, 6,0,0,0);
	GregorianBreakdown gb = sixHours.to_gregorian(0);
	UTASSERT(gb.time_type == TimeType::Time);
	UTASSERT((gb.year == 0) && (gb.month == 0) && (gb.day == 0));
	UTASSERT(gb.hour == 6);
	UTASSERT((gb.minute == 0) && (gb.second == 0) && (gb.millisecond == 0));
}

void TestJulian::Test3() {
	cout << __PRETTY_FUNCTION__ << endl;
	for (int i = 0; i < 1000; i++) {
		Julian j(0, 0,0,0,i);
		GregorianBreakdown gb = j.to_gregorian(0);
		UTASSERT(gb.millisecond == i);
	}
}

void TestJulian::Test4() {
	cout << __PRETTY_FUNCTION__ << endl;
	GregorianBreakdown gb(2004, 03, 01, 0, 0, 0, 0, 0);
	Julian j(gb);
	GregorianBreakdown newgb = j.to_gregorian(300);

	UTASSERT(newgb.year == 2004);
	UTASSERT(newgb.month == 02);
	UTASSERT(newgb.day == 29);
	UTASSERT(newgb.hour == 19);
	UTASSERT(newgb.minute == 00);
	UTASSERT(newgb.second == 00);
	UTASSERT(newgb.millisecond == 00);
	UTASSERT(newgb.minutes_west_utc == 300);

	gb.year = 2003;
	Julian j2(gb);
	newgb = j2.to_gregorian(300);	

	UTASSERT(newgb.year == 2003);
	UTASSERT(newgb.month == 02);
	UTASSERT(newgb.day == 28);
	UTASSERT(newgb.hour == 19);
	UTASSERT(newgb.minute == 00);
	UTASSERT(newgb.second == 00);
	UTASSERT(newgb.millisecond == 00);
	UTASSERT(newgb.minutes_west_utc == 300);
}

void TestJulian::Test5() {
	cout << __PRETTY_FUNCTION__ << endl;

	Julian jd(2010, 10, 13);
	Julian jt(0, 10, 50, 34, 352);
	
	Julian jj = jd + jt;
	GregorianBreakdown gb = jj.to_gregorian(0);

	UTASSERT(gb.year == 2010);
	UTASSERT(gb.month == 10);
	UTASSERT(gb.day == 13);
	UTASSERT(gb.hour == 10);
	UTASSERT(gb.minute == 50);
	UTASSERT(gb.second == 34);
	UTASSERT(gb.millisecond == 352);
	UTASSERT(gb.time_type == TimeType::DateTime);
	UTASSERT(gb.minutes_west_utc == 0);

	Julian jdt(2013, 3, 20, 19, 49, 27, 810);
	Julian jt2(0, 1, 0, 0, 200);

	jj = jdt + jt2;
	gb = jj.to_gregorian(0);
	
	UTASSERT(gb.year == 2013);
	UTASSERT(gb.month == 03);
	UTASSERT(gb.day == 20);
	UTASSERT(gb.hour == 20);
	UTASSERT(gb.minute == 49);
	UTASSERT(gb.second == 28);
	UTASSERT(gb.millisecond == 10);
	UTASSERT(gb.time_type == TimeType::DateTime);
	UTASSERT(gb.minutes_west_utc == 0);

	Julian jt3(0, 3, 4, 2, 400);
	Julian jt4(0, 1, 55, 57, 600);

	Julian jt5 = jt3 + jt4;
	gb = jt5.to_gregorian(0);
	
	UTASSERT(gb.hour == 5);
	UTASSERT(gb.minute == 0);
	UTASSERT(gb.second == 0);
	UTASSERT(gb.millisecond == 0);
	UTASSERT(gb.time_type == TimeType::Time);
	UTASSERT(gb.minutes_west_utc == 0);
}


void TestJulian::Test6() {
	cout << __PRETTY_FUNCTION__ << endl;

	Julian j(-43, 1, 15);
	GregorianBreakdown bc = j.to_gregorian(0);

	std::stringstream ss;
	ss << bc << endl;
	char line[512];
	ss.getline(line, 512);
	UTASSERT(strcmp(line, "BCE 44-01-15") == 0);	

	Julian year1(1, 1, 1, 0, 0, 0, 0);
	UTASSERT(1721425.499 < year1.Value() && year1.Value() < 1721425.501);

	Julian j2(1721424.5, TimeType::Date);
	GregorianBreakdown gb = j2.to_gregorian(0);
	std::stringstream s2;
	s2 << gb << endl;
	s2.getline(line, 512);
	UTASSERT(strcmp(line, "BCE 1-12-31") == 0);

	Julian today(2013, 03, 29, 12, 0, 0, 0);
	UTASSERT(today.Weekday(0) == DayOfWeek::Friday);
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
