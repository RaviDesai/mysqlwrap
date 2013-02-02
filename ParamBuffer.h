#pragma once

#include <string>
#include "mysql.h"
#include "Binary.h"

class ParamBuffer {
public:
	ParamBuffer(const enum_field_types type);
	ParamBuffer(const std::string &str, size_t maxSize);
	ParamBuffer(const short int i);
	ParamBuffer(const int i);
	ParamBuffer(const unsigned long l);
	ParamBuffer(const MYSQL_TIME &t);
	ParamBuffer(const Binary &data);
	ParamBuffer(const char ch);

	ParamBuffer(const ParamBuffer &copy);
	~ParamBuffer();

	void ResizeBlob(size_t newSize);

	void *Buffer() const;
	enum_field_types BufferType() const;
	size_t BufferSize() const;
	unsigned long *BufferLength();
	my_bool *IsNull();

private:
	void *_buffer;
	size_t _bufferSize;
	unsigned long _bufferLength;
	enum_field_types _type;
	my_bool _isNull;
};
