#pragma once

#include <iostream>
#include <mysql.h>
#include <string>

using namespace std;

namespace MySQLWrap { 
	class Database {
		friend class Statement;
		friend class AdhocStatement;
	public:
		Database(const std::string &host, const std::string& user, const std::string &password, const std::string &database, unsigned int port, const char *unixSocket, unsigned long clientFlags);
		Database();
		Database(const Database &db);
		virtual ~Database();

		static void LibraryInitialize(bool embedded);
		static void ThreadInitialize();

		static void LibraryFinalize();
		static void ThreadFinalize();

		void SetInitialCommand(const std::string &initialCommand);
		bool Autocommit() const;
		void Autocommit(bool value);
		void Connect();

		void StartTransaction();
		void Commit();
		void Rollback();
		void UseDatabase(const string &dbName);
		int Execute(const string &statement);
		void Source(const string &fileName);

		bool IsConnected();

	private:
		MYSQL *_db;
	
		std::string _host;
		std::string _user;
		std::string _password;
		std::string _database;
		std::string _initialCommand;
		unsigned int _port;
		const char *_unixSocket;
		unsigned long _clientFlag;
	
		bool _isConnected;
		bool _autocommit;
		
		static bool _embedded;
	};

	class LibraryInitializer {
	public:
		LibraryInitializer(bool embedded);
		~LibraryInitializer();
	};

	class ThreadInitializer {
	public:
		ThreadInitializer();
		~ThreadInitializer();
	};
}
