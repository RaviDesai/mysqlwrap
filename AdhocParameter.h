#pragma once

#include <string>
#include <sstream>

#include "Binary.h"
#include "Julian.h"

#include <mysql.h>
#include <mysql_time.h>

namespace MySQLWrap {

	class AdhocParameter {
	public:
		AdhocParameter();
		void SetData(const unsigned char data);
		void SetData(const char data);
		void SetData(const unsigned short int data);
		void SetData(const short int data);
		void SetData(const unsigned long data);
		void SetData(const long data);
		void SetData(const std::string &data);
		void SetData(const Julian &data);
		void SetData(const Binary &data);
		void SetData(const float data);
		void SetData(const double data);

		std::string Get() const;
	private:
		bool _isNull;
		std::stringstream _data;
	};

}

