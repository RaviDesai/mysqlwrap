#pragma once

#include <iostream>
#include <mysql.h>
#include <mysql_time.h>

namespace MySQLWrap {
	class JulianException : std::exception {
	public:
		JulianException(const std::string &msg) : _message(msg) {}
		virtual ~JulianException() throw() {}
		virtual const char *what() const throw() { return _message.c_str(); }
	private:
		std::string _message;
	};

	enum class TimeType { Date, Time, DateTime };
	struct GregorianBreakdown {
		int year; unsigned int month; unsigned int day;
		unsigned int hour; unsigned int minute; unsigned int second; unsigned int millisecond;
		int minutes_west_utc;
		TimeType time_type;

		GregorianBreakdown();
		GregorianBreakdown(int year, unsigned int month, unsigned int day, 
				   unsigned int hour, unsigned int minute, unsigned int second, unsigned int ms, int minutes_west_utc);
		GregorianBreakdown(int year, unsigned int month, unsigned int day);
		GregorianBreakdown(unsigned int day, unsigned int hour, unsigned int minute, unsigned int second, unsigned int ms);
		GregorianBreakdown(const MYSQL_TIME &time, int minutes_west_utc);
		MYSQL_TIME to_mysql_time() const;
	};

	std::ostream &operator<<(std::ostream &out, const GregorianBreakdown &gb);

	class Julian {
	public:
		Julian();
		Julian(double value, TimeType time_type);
		Julian(int year, unsigned int month, unsigned int day, unsigned int hour, unsigned int minute, unsigned int second, unsigned int ms);
		Julian(int year, unsigned int month, unsigned int day);
		Julian(unsigned int day, unsigned int hour, unsigned int minute, unsigned int second, unsigned int ms);
		Julian(const GregorianBreakdown &gb);
		Julian(const Julian &) = default;
		Julian &operator=(const Julian &) = default;

		double Value() const;
		TimeType Type() const;
		GregorianBreakdown to_gregorian(int minutes_west_utc) const;
	
	protected:
		static double calculate_utc(int year, unsigned int month, unsigned int day, 
					    unsigned int hour, unsigned int minute, unsigned int second, unsigned int ms);

		static double calculate_jdn(int year, unsigned int month, unsigned int day);
		static double calculate_time(unsigned int hour, unsigned int minute, unsigned int second, unsigned int ms);
	private:
		double _julian;
		TimeType _time_type;
	};

	std::ostream &operator<<(std::ostream &out, const Julian &julian);
	bool operator<(const Julian &left, const Julian &right);
	Julian operator+(const Julian &left, const Julian &right);
	Julian operator+(const Julian &left, double right);
	Julian operator-(const Julian &left, const Julian &right);
	Julian operator-(const Julian &left, double right);
}
