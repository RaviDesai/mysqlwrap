#pragma once

#include <iostream>
#include <mysql.h>

class Database {
	friend class Statement;
public:
	Database(const std::string &host, const std::string& user, const std::string &password, const std::string &database, unsigned int port, const char *unixSocket, unsigned long clientFlags);
	Database(const Database &db);
	virtual ~Database();

	static void Initialize();
	static void Finalize();

	void SetInitialCommand(const std::string &initialCommand);
	bool Autocommit() const;
	void Autocommit(bool value);
	void Connect();

	void StartTransaction();
	void Commit();
	void Rollback();

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
};

class DatabaseInitializer {
public:
	DatabaseInitializer();
	~DatabaseInitializer();
};
