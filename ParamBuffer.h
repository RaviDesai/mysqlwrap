#pragma once

#include <string>
#include <typeinfo>
#include "mysql.h"
#include "Binary.h"
#include "Julian.h"

namespace MySQLWrap {

	class ParamBuffer {
	public:
		ParamBuffer(const enum_field_types type, my_bool isUnsigned);
		ParamBuffer(const std::type_info &info);
		ParamBuffer(const std::string &str, size_t maxSize);
		ParamBuffer(const std::string &str);
		ParamBuffer(const short int i);
		ParamBuffer(const unsigned short int i);
		ParamBuffer(const int i);
		ParamBuffer(const unsigned int i);
		ParamBuffer(const Julian &t);
		ParamBuffer(const Binary &data);
		ParamBuffer(const char ch);
		ParamBuffer(const unsigned char ch);
		ParamBuffer(const float f);
		ParamBuffer(const double d);

		ParamBuffer(const ParamBuffer &copy);
		~ParamBuffer();

		void ResizeBlob(size_t newSize);

		void *Buffer() const;
		enum_field_types BufferType() const;
		size_t BufferSize() const;
		unsigned long *BufferLength();
		my_bool *IsNull();
		my_bool IsUnsigned();
		my_bool *IsTruncated();
		my_bool *Error();

	private:
		void *_buffer;
		size_t _bufferSize;
		unsigned long _bufferLength;
		enum_field_types _type;
		my_bool _isNull;
		my_bool _isUnsigned;
		my_bool _isTruncated;
		my_bool _error;
	};

}
