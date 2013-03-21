#include "ParamBuffer.h"
#include "DatabaseException.h"

#include <iostream>
using namespace std;

namespace MySQLWrap {

ParamBuffer::ParamBuffer(enum_field_types type, my_bool isUnsigned) {
	_type = type;
	_buffer = NULL;
	_bufferSize = 0;
	_bufferLength = 0;
	_isNull = 1;
	_isUnsigned = isUnsigned;
}

ParamBuffer::ParamBuffer(const std::type_info &info) {
	my_bool isUnsigned = false;

	if (info == typeid(std::string)) {
		_type = MYSQL_TYPE_STRING;
	} else if (info == typeid(Binary)) {
		_type = MYSQL_TYPE_BLOB;
	} else if (info == typeid(double)) {
		_type = MYSQL_TYPE_DOUBLE;
	} else if (info == typeid(float)) {
		_type = MYSQL_TYPE_FLOAT;
	} else if (info == typeid(char)) {
		_type = MYSQL_TYPE_TINY;
	} else if (info == typeid(unsigned char)) {
		_type = MYSQL_TYPE_TINY;
		isUnsigned = true;
	} else if (info == typeid(short)) {
		_type = MYSQL_TYPE_SHORT;
	} else if (info == typeid(unsigned short)) {
		_type = MYSQL_TYPE_SHORT;
		isUnsigned = true;
	} else if (info == typeid(int)) {
		_type = MYSQL_TYPE_LONG;
	} else if (info == typeid (unsigned int)) {
		_type = MYSQL_TYPE_LONG;
		isUnsigned = true;
	}

	_buffer = NULL;
	_bufferSize = 0;
	_bufferLength = 0;
	_isNull = 1;
	_isUnsigned = isUnsigned;
}

ParamBuffer::ParamBuffer(const std::string &str) : ParamBuffer(str, str.length()) {
}

ParamBuffer::ParamBuffer(const std::string &str, size_t maxSize) {
	if (str.size() > maxSize) {
		throw DatabaseException("Error in ParamBuffer::ParamBuffer(string)", 0, "----", "length of str parameter is greater than maxSize");
	}

	if (maxSize > 0) {
		_buffer = malloc(maxSize);
		_bufferSize = maxSize;
		_bufferLength = str.length();
		_type = MYSQL_TYPE_STRING;
		_isNull = 0;
	} else {
		_buffer = NULL;
		_bufferSize = 0;
		_bufferLength = 0;
		_type = MYSQL_TYPE_STRING;
		_isNull = 1;
	}

	memcpy(_buffer, str.data(), str.length());
}

void *ParamBuffer::Buffer() const { 
	return _buffer;
}

enum_field_types ParamBuffer::BufferType() const {
	return _type;
}

size_t ParamBuffer::BufferSize() const {
	return _bufferSize;
}	

unsigned long *ParamBuffer::BufferLength() {
	return &_bufferLength;
}

my_bool *ParamBuffer::IsNull() {
	return &_isNull;
}

ParamBuffer::ParamBuffer(const char ch) {
	_bufferSize = sizeof (char);
	_buffer = malloc(_bufferSize);
	_bufferLength = _bufferSize;
	_type = MYSQL_TYPE_TINY;
	_isNull = 0;
	_isTruncated = 0;
	_isUnsigned = 0;

	*((char *)_buffer) = ch;
}

ParamBuffer::ParamBuffer(const unsigned char ch) {
	_bufferSize = sizeof (char);
	_buffer = malloc(_bufferSize);
	_bufferLength = _bufferSize;
	_type = MYSQL_TYPE_TINY;
	_isNull = 0;
	_isTruncated = 0;
	_isUnsigned = 1;

	*((char *)_buffer) = ch;
}

ParamBuffer::ParamBuffer(const short int i) {
	_buffer = malloc(sizeof(short int));
	_bufferSize = sizeof (short int);
	_bufferLength = _bufferSize;
	_type = MYSQL_TYPE_SHORT;
	_isNull = 0;
	_isTruncated = 0;
	_isUnsigned = 0;

	*((short int *)_buffer) = i;
}

ParamBuffer::ParamBuffer(const unsigned short int i) {
	_buffer = malloc(sizeof(short int));
	_bufferSize = sizeof (short int);
	_bufferLength = _bufferSize;
	_type = MYSQL_TYPE_SHORT;
	_isNull = 0;
	_isTruncated = 0;
	_isUnsigned = 1;

	*((short int *)_buffer) = i;
}

ParamBuffer::ParamBuffer(const float f) {
	_buffer = malloc(sizeof(float));
	_bufferSize = sizeof (float);
	_bufferLength = _bufferSize;
	_type = MYSQL_TYPE_FLOAT;
	_isNull = 0;
	_isTruncated = 0;
	_isUnsigned = 1;

	*((float *)_buffer) = f;
}

ParamBuffer::ParamBuffer(const double d) {
	_buffer = malloc(sizeof(double));
	_bufferSize = sizeof (double);
	_bufferLength = _bufferSize;
	_type = MYSQL_TYPE_FLOAT;
	_isNull = 0;
	_isTruncated = 0;
	_isUnsigned = 1;

	*((double *)_buffer) = d;
}

ParamBuffer::ParamBuffer(const int i) {
	_buffer = malloc(sizeof(int));
	_bufferSize = sizeof(int);
	_bufferLength = sizeof(int);
	_type = MYSQL_TYPE_LONG;
	_isNull = 0;
	_isTruncated = 0;
	_isUnsigned = 0;

 	*((int *)_buffer) = i;
}

ParamBuffer::ParamBuffer(const unsigned int i) {
	_buffer = malloc(sizeof(unsigned int));
	_bufferSize = sizeof(unsigned int);
	_bufferLength = _bufferSize;
	_type = MYSQL_TYPE_LONG;
	_isNull = 0;
	_isTruncated = 0;
	_isUnsigned = 1;

	*((unsigned long *)_buffer) = i;
}

ParamBuffer::ParamBuffer(const Julian &julian) {
	_buffer = malloc(sizeof(MYSQL_TIME));
	_bufferSize = sizeof(MYSQL_TIME);
	_bufferLength = _bufferSize;
	_type = MYSQL_TYPE_DATETIME;
	_isNull = 0;
	memset(_buffer, 0, sizeof(MYSQL_TIME));

	if (julian.Value() > 0) {
		GregorianBreakdown gb = julian.to_gregorian(0);
		MYSQL_TIME tm = gb.to_mysql_time();
		*((MYSQL_TIME *)_buffer) = tm;
	}
}

ParamBuffer::ParamBuffer(const Binary &data) {
	_bufferSize = data.BufferSize();
	_buffer = malloc(_bufferSize);
	_bufferLength = data.BufferLength();
	_type = MYSQL_TYPE_BLOB;
	_isNull = ((_buffer == NULL) ? 1 : 0);

	memcpy(_buffer, data.Buffer(), _bufferLength);
}

ParamBuffer::~ParamBuffer() {
	if (_buffer != NULL) {
		free(_buffer);
	}
	_buffer = NULL;
	_bufferSize = 0;
	_bufferLength = 0;
	_isNull = 1;
}

void ParamBuffer::ResizeBlob(size_t newSize) {
	if (_type != MYSQL_TYPE_BLOB) { 
		throw DatabaseException("Error in ParamBuffer::ResizeBlob", 0, "----", "Datatype is not blob");
	}

	if (_bufferSize != newSize) {
		free(_buffer);
		_buffer = malloc(newSize);
		_bufferSize = newSize;
		_bufferLength = 0;
	}
}

my_bool ParamBuffer::IsUnsigned() {
	return _isUnsigned;
}

my_bool *ParamBuffer::IsTruncated() {
	return &_isTruncated;
}

my_bool *ParamBuffer::Error() {
	return &_error;
}

}
