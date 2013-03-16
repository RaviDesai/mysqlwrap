#pragma once

#include <iostream>
#include <mysql.h>

namespace MySQLWrap {
	class DatabaseException {
	public:
		DatabaseException(const std::string &initialMessage, const int errno, const char *sqlState, const char *errorMessage);
		DatabaseException(MYSQL *db, const std::string &initialMessage);
		DatabaseException(MYSQL_STMT *stmt, const std::string &initialMessage);

		friend std::ostream &operator<<(std::ostream &out, const DatabaseException &exp);

	private:
		int _errno;
		std::string _sqlState;
		std::string _initialMessage;
		std::string _errorMessage;
	};
}
