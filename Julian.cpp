#include "Julian.h"
#include <iomanip>

using namespace std;

namespace MySQLWrap {

GregorianBreakdown::GregorianBreakdown() { 
	year = month = day = hour = minute = second = millisecond = 0;
	minutes_west_utc = 0;
	time_type = TimeType::DateTime;
}

GregorianBreakdown::GregorianBreakdown(const MYSQL_TIME &time, int minutes_west) {
	year = time.year;
	month = time.month;
	day = time.day;
	hour = time.hour;
	minute = time.minute;
	second = time.second;
	millisecond = (int)(time.second_part / 1000);
	minutes_west_utc = minutes_west;

	if (time.time_type == MYSQL_TIMESTAMP_DATE) {
		time_type = TimeType::Date;
	} else if (time.time_type == MYSQL_TIMESTAMP_TIME) { 
		time_type = TimeType::Time;
	} else {
		time_type = TimeType::DateTime;
	}
}

MYSQL_TIME GregorianBreakdown::to_mysql_time() const {
	MYSQL_TIME result;

	result.year = year;
	result.month = month;
	result.day = day;
	result.hour = hour;
	result.minute = minute;
	result.second = second;
	result.second_part = millisecond * 1000;

	if (time_type == TimeType::Date) {
		result.time_type = MYSQL_TIMESTAMP_DATE;
	} else if (time_type == TimeType::Time) {
		result.time_type = MYSQL_TIMESTAMP_TIME;
	} else {
		result.time_type = MYSQL_TIMESTAMP_DATETIME;
	}

	return result;
}

std::ostream &operator<<(std::ostream &out, const GregorianBreakdown &gb) {
	if (gb.time_type != TimeType::Time) {
		out << setfill('0') << setw(4) << gb.year << "-"  << setw(2) << gb.month << "-" << setw(2) << gb.day << " ";
	}

	out << setfill('0') << setw(2) << gb.hour << ":" << setw(2) << gb.minute << ":" << setw(2) << gb.second;
	if (gb.millisecond > 0) {
		out << "." << setw(3) << gb.millisecond;
	}

	if (gb.time_type != TimeType::Time) { 
		char plusOrMinus = (gb.minutes_west_utc > 0) ? '-' : '+';
		out << " UTC " << plusOrMinus << setprecision(2) << ((float) abs(gb.minutes_west_utc) / 60);
	}
	return out;
}

Julian::Julian() {
	_julian = 0;
}

Julian::Julian(const GregorianBreakdown &gb) {
	_julian = calculate_utc(gb.year, gb.month, gb.day, gb.hour, gb.minute, gb.second, gb.millisecond);
	_julian += ((double) gb.minutes_west_utc) / 1440;
	_time_type = gb.time_type;
}

Julian::Julian(unsigned int year, unsigned int month, unsigned int day, unsigned int hour, unsigned int minute, unsigned int second, unsigned int ms) {
	_julian = calculate_utc(year, month, day, hour, minute, second, ms);
	_time_type = TimeType::DateTime;
}

Julian::Julian(unsigned int year, unsigned int month, unsigned int day) {
	_julian = calculate_utc(year, month, day, 0, 0, 0, 0);
	_time_type = TimeType::Date;
}

Julian::Julian(unsigned int hour, unsigned int minute, unsigned int second, unsigned int ms) {
	_julian = calculate_time(hour, minute, second, ms);
	_time_type = TimeType::Time;
}

TimeType Julian::Type() const {
	return _time_type;
}

double Julian::Value() const { 
	return _julian;
}

double Julian::calculate_utc(unsigned int year, unsigned int month, unsigned int day, unsigned int hour, unsigned int minute, unsigned int second, unsigned int ms) {
	return calculate_jdn(year, month, day) + calculate_time(hour, minute, second, ms);
}

double Julian::calculate_jdn(unsigned int year, unsigned int month, unsigned int day) {
	int a = (14 - month) / 12;
	int y = year + 4800 - a;
	int m = month + (12 * a) - 3;

	int jdn = day + (int)(((153 * m) + 2) / 5) + (365*y) +
		 (int)(y/4) - (int)(y / 100) + (int)(y/400) - 32045;

	return ((double) jdn) - 0.5;
}

double Julian::calculate_time(unsigned int hour, unsigned int minute, unsigned int second, unsigned int ms) {
	double result = (((double) hour)  / 24) + (((double) minute) / 1440) +
			(((double) second) / 86400) + (((double) ms) / 86400000);
	return result;
}

const int y = 4716;
const int v = 3;
const int j = 1401;
const int u = 5;
const int m = 2;
const int s = 153;
const int n = 12;
const int w = 2;
const int r = 4;
const int B = 274277;
const int p = 1461;
const int C = -38;

GregorianBreakdown Julian::to_gregorian(int minutes_west_utc) const {
	GregorianBreakdown result;

	double time;
	if (_time_type != TimeType::Time) {
		double adjJulian = _julian - (((double) minutes_west_utc) / 1440);
		int J = (int) (adjJulian + 0.5);
		int f = J + j;
		f = f + ((int) ((((int) (4 * J + B) / 146097) * 3) / 4)) + C;
		int e = r * f + v;
		int g = (int) ((e % p) / r);
		int h = u * g + w;
		result.day = ((int)((h % s) / u)) + 1;
		result.month = ((((int) h / s) + m) % n) + 1;
		result.year = ((int) e/p) - y + ((int)((n + m - result.month) / n));

		time = adjJulian + .5;
	} else {
		time = _julian - (((double) minutes_west_utc) / 1440);
	}

	time -= (int)time;
	double subms = (0.5 / 86400000);

	time = time * 24;
	subms = subms * 24;
	result.hour = (int) (time + subms);
	if (((int) time) != ((int) (time + subms))) {
		time -= (int)(time + subms);
		subms = 0;
	} else {
		time -= (int)(time + subms);
	}

	time = time * 60;
	subms = subms * 60;
	result.minute = (int) (time + subms);
	if (((int) time) != ((int) (time + subms))) {
		time -= (int)(time + subms);
		subms = 0;
	} else {
		time -= (int)(time + subms);
	}

	time = time * 60;
	subms = subms * 60;
	result.second = (int) (time + subms);
	if (((int) time) != ((int) (time + subms))) {
		time -= (int)(time + subms);
		subms = 0;
	} else {
		time -= (int)(time + subms);
	}

	time = time * 1000;
	subms = subms * 1000;
	result.millisecond = (int) (time + subms);

	result.minutes_west_utc = minutes_west_utc;
	result.time_type = _time_type;
	return result;
}

}

