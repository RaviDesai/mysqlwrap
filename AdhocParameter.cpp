#include "AdhocParameter.h"
#include "DatabaseException.h"

#include <iostream>
#include <iomanip>

using namespace std;

namespace MySQLWrap {

AdhocParameter::AdhocParameter() {
	_isNull = true;
}

void AdhocParameter::SetData(const unsigned char data) {
	_data << std::dec << data;
}

void AdhocParameter::SetData(const char data) {
	_data << std::dec << data;
}

void AdhocParameter::SetData(const unsigned short data) {
	_data << std::dec << data;
}

void AdhocParameter::SetData(const short data) {
	_data << std::dec << data;
}

void AdhocParameter::SetData(const unsigned long data) {
	_data << std::dec << data;
}

void AdhocParameter::SetData(const long data) {
	_data << std::dec << data;
}

void AdhocParameter::SetData(const std::string &data) {
	_data << "'" << data << "'";
}

void AdhocParameter::SetData(const Binary &data) {
	char buff[data.BufferLength() * 2 + 1];
	if (mysql_hex_string(buff, (const char *)data.Buffer(), data.BufferLength()) != data.BufferLength() * 2) {
		throw DatabaseException("Error in AdhocParameter::SetData", 0, "----", "mysql_hex_string returned the wrong size string");
	}
	_data << "X'" << buff << "'";
}

void AdhocParameter::SetData(const float data) {
	_data << data;
}

void AdhocParameter::SetData(const double data) {
	_data << data;
}

void AdhocParameter::SetData(const Julian &julian) {
	GregorianBreakdown data = julian.to_gregorian(0);
	
	if (julian.Type() == TimeType::DateTime) {
		_data << setfill('0') << setw(4) << data.year << "-" << setw(2) << data.month << "-" << setw(2) << data.day << " "
		      << setw(2) << data.hour << ":" << setw(2) << data.minute << ":" << setw(2) << data.second << "."
		      << setw(6) << data.millisecond * 1000;
	} else if (julian.Type() == TimeType::Date) {
		_data << setfill('0') << setw(4) << data.year << "-" << setw(2) << data.month << "-" << setw(2) << data.day;
	} else if (julian.Type() == TimeType::Time) {
		unsigned long hours = data.day * 24 + data.hour;
		_data << setfill('0') 
		      << setw(2) << hours << ":" << setw(2) << data.minute << ":" << setw(2) << data.second << "." 
		      << setw(6) << data.millisecond * 1000;
	}
}

std::string AdhocParameter::Get() const {
	return _data.str();
}

}
