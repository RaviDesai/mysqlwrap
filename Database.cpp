#include "Database.h"
#include "DatabaseException.h"

Database::Database(const std::string &host, const std::string &user, const std::string &password, const std::string &database, unsigned int port, const char *unixSocket, unsigned long clientFlag) {
	_host = host;
	_user = user;
	_password = password;
	_database = database;
	_port = port;
	_unixSocket = unixSocket;
	_clientFlag = clientFlag;
	_initialCommand = "";
	_isConnected = false;
	_autocommit = true;

	if ((_db = mysql_init(NULL)) == NULL) {
		throw DatabaseException("Error tryng to initialize MYSQL database", 0, "-----", "insufficient memory");
	}
}

Database::Database(const Database &db) {
	_host = db._host;
	_user = db._user;
	_password = db._password;
	_database = db._database;
	_port = db._port;
	_unixSocket = db._unixSocket;
	_clientFlag = db._clientFlag;
	_initialCommand = db._initialCommand;

	if ((_db = mysql_init(NULL)) == NULL) {
		throw DatabaseException("Error trying to initialize MYSQL database", 0, "----", "insufficient memory");
	}

	if (db._isConnected) {
		try {
			Connect();
		} catch (DatabaseException &) {
			mysql_close(_db);
			throw;
		}
	}

	if (db._autocommit) {
		try {
			Autocommit(db._autocommit);
		} catch (DatabaseException &) {
			mysql_close(_db);
			throw;
		}
	}
}

Database::~Database() {
	mysql_close(_db);
}

void Database::Connect() {
	if (mysql_real_connect(_db, _host.c_str(), _user.c_str(), _password.c_str(), _database.c_str(), 
	     _port, _unixSocket, _clientFlag) == NULL) {
		throw DatabaseException(_db, "Error in Database::Connect");
	}
	_isConnected = true;
}

void Database::SetInitialCommand(const std::string &initialCommand) {
	_initialCommand = initialCommand;
}

bool Database::Autocommit() const {
	return _autocommit;	
}

void Database::Autocommit(bool setvalue) {
	int ac = (setvalue) ? 1 : 0;

	if (mysql_autocommit(_db, ac) != 0) {
		throw DatabaseException(_db, "Error in Database::Autocommit");
	}
}

void Database::StartTransaction() {
	if (mysql_query(_db, "START TRANSACTION") != 0) {
		throw DatabaseException(_db, "Error in Database::StartTransaction");
	}
}

void Database::Commit() {
	if (mysql_query(_db, "COMMIT") != 0) {
		throw DatabaseException(_db, "Error in Database::Commit");
	}
}

void Database::Rollback() {
	if (mysql_query(_db, "ROLLBACK") != 0) {
		throw DatabaseException(_db, "Error in Database::Rollback");
	}
}

bool Database::IsConnected() {
	return _isConnected;
}
