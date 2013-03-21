#pragma once

#include <iostream>
#include <mysql.h>
#include <mysql_time.h>

namespace MySQLWrap {
	enum class TimeType { Date, Time, DateTime };
	struct GregorianBreakdown {
		unsigned int year; unsigned int month; unsigned int day;
		unsigned int hour; unsigned int minute; unsigned int second; unsigned int millisecond;
		int minutes_west_utc;
		TimeType time_type;

		GregorianBreakdown();
		GregorianBreakdown(const MYSQL_TIME &time, int minutes_west_utc);
		MYSQL_TIME to_mysql_time() const;
	};

	std::ostream &operator<<(std::ostream &out, const GregorianBreakdown &gb);

	class Julian {
	public:
		Julian();
		Julian(unsigned int year, unsigned int month, unsigned int day, unsigned int hour, unsigned int minute, unsigned int second, unsigned int ms);
		Julian(unsigned int year, unsigned int month, unsigned int day);
		Julian(unsigned int hour, unsigned int minute, unsigned int second, unsigned int ms);
		Julian(const GregorianBreakdown &gb);
		Julian(const Julian &) = default;
		Julian &operator=(const Julian &) = default;

		double Value() const;
		TimeType Type() const;
		GregorianBreakdown to_gregorian(int minutes_west_utc) const;
	
	protected:
		static double calculate_utc(unsigned int year, unsigned int month, unsigned int day, 
					    unsigned int hour, unsigned int minute, unsigned int second, unsigned int ms);

		static double calculate_jdn(unsigned int year, unsigned int month, unsigned int day);
		static double calculate_time(unsigned int hour, unsigned int minute, unsigned int second, unsigned int ms);
	private:
		double _julian;
		TimeType _time_type;
	};

}
