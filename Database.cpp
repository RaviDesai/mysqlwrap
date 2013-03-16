#include "Database.h"
#include "DatabaseException.h"
#include <iostream>
#include <sys/stat.h>
#include <fstream>
#include <cwchar>
#include <locale>

using namespace std;

namespace MySQLWrap {

Database::Database() {
	_host = "";
	_user = "";
	_password = "";
	_database = "";
	_port = 0;
	_unixSocket = NULL;
	_clientFlag = 0;
	_initialCommand = "";
	_isConnected = false;
	_autocommit = true;
	
	if ((_db = mysql_init(NULL)) == NULL) {
		throw DatabaseException("Error tryng to initialize MYSQL database", 0, "-----", "insufficient memory");
	}

	if (_embedded) {
		mysql_options(_db, MYSQL_READ_DEFAULT_GROUP, "embedded");
		mysql_options(_db, MYSQL_OPT_USE_EMBEDDED_CONNECTION, NULL);
	}
}

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
	_autocommit = db._autocommit;

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

	if (! _autocommit) {
		try {
			Autocommit(_autocommit);
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
	if (_host == "") {
		if (mysql_real_connect(_db, NULL, NULL, NULL, _database.c_str(), 0, NULL, 0) == NULL) {
			throw DatabaseException(_db, "Error in Database::Connect");
		}
	} else {
		if (mysql_real_connect(_db, _host.c_str(), _user.c_str(), _password.c_str(), _database.c_str(), 
		    _port, _unixSocket, _clientFlag) == NULL) {
			throw DatabaseException(_db, "Error in Database::Connect");
		}
	}
	_isConnected = true;

	if (mysql_set_character_set(_db, "utf8") != 0)
	{
		throw DatabaseException(_db, "Error in DatabaseConnect");
	}	
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

void Database::UseDatabase(const string &dbName) {
	if (mysql_select_db(_db, dbName.c_str()) != 0) {
		throw DatabaseException(_db, "Error in Database::UseDatabase");
	}
}


int Database::Execute(const string &statement) {
	if (mysql_query(_db, statement.c_str()) != 0) {
		throw DatabaseException(_db, "Error in Database::Execute - query");
	}

	MYSQL_RES *res = mysql_store_result(_db);
	if (res != NULL) {
		mysql_free_result(res);
		throw DatabaseException("Error in Database::Execute", 0, "----", "statements passed to Execute should never return results.");
	}
	
	return mysql_affected_rows(_db);
}

void Database::LibraryInitialize(bool embedded) {
	_embedded = embedded;
	if (! embedded) {
		if (mysql_library_init(0, NULL, NULL) != 0) {
			throw DatabaseException("Error in Database::Initialize", 0, "----", "Failure to initialize the database library");
		}
	} else {
		static const char *server_args[] = { "this_program", "--basedir=/usr/local/mysql", "--datadir=/Users/ravidesai/mysql/data", 
						     "--plugin-dir=/Users/ravidesai/mysql/plugins", "--log-error=/Users/ravidesai/mysql/tmp/test.err",
						     "--pid-file=/Users/ravidesai/mysql/tmp/test.pid",
						     "--key_buffer_size=32M", "--log-bin=/Users/ravidesai/mysql/log/logbin"
						     "--log-bin-trust-function-creators=1"
						     "--log-bin-trust-routine-creators=1"
						    };
		static const char *server_groups[] = { "embedded", "server", "this_program_SERVER", (char *) NULL };

		if (mysql_library_init(sizeof(server_args) / sizeof(char *), (char**) server_args, (char **)server_groups) != 0) {
			throw DatabaseException("Error in Database::Initialize", 0, "----", "Failure to initialize the database library");
		}
	}

	cout << setlocale(LC_ALL, "") << endl;
}

void Database::ThreadInitialize() { 
	if (mysql_thread_init() != 0) {
		throw DatabaseException("Error in Database::ThreadInitialize", 0, "----", "Failure to initialize database for this thread");
	}
}

void Database::LibraryFinalize() {
	mysql_library_end();
}

void Database::ThreadFinalize() {
	mysql_thread_end();
}

LibraryInitializer::LibraryInitializer(bool embedded) {
	Database::LibraryInitialize(embedded);
}

LibraryInitializer::~LibraryInitializer() {
	Database::LibraryFinalize();
}

ThreadInitializer::ThreadInitializer() {
	Database::ThreadInitialize();
}

ThreadInitializer::~ThreadInitializer() {
	Database::ThreadFinalize();
}

bool Database::_embedded;

static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
}

void Database::Source(const string &fileName) {
	struct stat filestat;
	if (stat(fileName.c_str(), &filestat) != 0) {
		throw DatabaseException("Error in Database::Source", 0, "----", ("could not stat file " + fileName).c_str());
	}

	ifstream commands(fileName.c_str(), ios::in);
	if (!commands.is_open()) {
		throw DatabaseException("Error in Database::Source", 0, "----", ("could not open file: " + fileName).c_str());
	}

	commands.exceptions(std::ios::badbit);

	try {
		std::string cmd;
		std::string collector;
		std::string delimiter = ";";
		size_t delimpos;
		bool insideProcOrFunc = false;
		while (! commands.eof()) {
			getline(commands, cmd);
			cmd = trim(cmd);

			size_t dashDashPos = cmd.find("--");
			if (dashDashPos != std::string::npos && dashDashPos == 0) {
				// comment node - ignore
				continue;
			} 

			delimpos = cmd.find("USE DATABASE ");
			if (delimpos != std::string::npos && delimpos == 0) {
				cmd.erase(0, 13); 
				cmd = trim(cmd);
				UseDatabase(cmd);
				continue;
			}

			delimpos = cmd.find("DELIMITER ");
			if (delimpos != std::string::npos && delimpos == 0) {
				delimiter = cmd.erase(0, 10);
				delimiter = trim(delimiter);
				continue;
			}

			delimpos = cmd.find("CREATE PROC");
			if (delimpos != std::string::npos && delimpos == 0) {
				insideProcOrFunc = true;
			}
			delimpos = cmd.find("CREATE FUNC");
			if (delimpos != std::string::npos && delimpos == 0) {
				insideProcOrFunc = true;
			}

			if (insideProcOrFunc) {
				size_t poundPos = cmd.find("#");
				if (poundPos != std::string::npos) {
					// remove pound comment
					cmd = cmd.erase(poundPos, cmd.length() - poundPos + 1);
				}
			}

			collector.append(" ");
			collector.append(cmd);

			delimpos = collector.rfind(delimiter.c_str());
			if (delimpos != std::string::npos && delimpos + delimiter.length() >= collector.length()) {
				// execute the line
				collector.replace(delimpos, delimiter.length(), "");
			
				if (insideProcOrFunc) {
					size_t commentStart = collector.find("/*");
					while (commentStart != std::string::npos) {
						size_t commentEnd = collector.find("*/");
						if (commentEnd == std::string::npos || commentEnd <= commentStart) {
							throw DatabaseException("Error in Database::Source", 0, "----", "unmatched comment strings");
						}
						collector = collector.replace(commentStart, commentEnd - commentStart + 2, "");
						commentStart = collector.find("/*");
					}
				}

				Execute(collector);
				collector = "";
				insideProcOrFunc = false;
			}
		}
	} catch (ifstream::failure &ex) {
		throw DatabaseException("Error while reading file in Database::Source", 0, "----", ex.what());
	}

	commands.close();
}
}
