#include "DatabaseException.h"

namespace MySQLWrap {

DatabaseException::DatabaseException(const std::string &initialMessage, const int errno, const char *sqlState, const char *errorMessage) {
	_initialMessage = initialMessage;
	_errno = errno;
	_sqlState = sqlState;
	_errorMessage = errorMessage;
}

DatabaseException::DatabaseException(MYSQL *db, const std::string &initialMessage) {
	_errno = mysql_errno(db);
	_errorMessage = mysql_error(db);
	_initialMessage = initialMessage;
	_sqlState = mysql_sqlstate(db);
}

DatabaseException::DatabaseException(MYSQL_STMT *stmt, const std::string &initialMessage) {
	_errno = mysql_stmt_errno(stmt);
	_errorMessage = mysql_stmt_error(stmt);
	_sqlState = mysql_stmt_sqlstate(stmt);
	_initialMessage = initialMessage;
}

std::ostream &operator<<(std::ostream &out, const DatabaseException &exp) {
	out << exp._initialMessage << " ERROR " << exp._errno << "(" << exp._sqlState << ") " << exp._errorMessage;
	return out;
}

}

