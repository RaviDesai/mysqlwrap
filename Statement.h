#pragma once

#include <vector>
#include "Database.h"
#include "ParamBuffer.h"
#include "Nullable.h"
#include <mysql_time.h>

class ExecuteSentinel {
public:
	ExecuteSentinel();
};
const ExecuteSentinel execute;

class FetchSentinel {
public:
	FetchSentinel();
};
const FetchSentinel fetch;

class Statement {
public:
	Statement(Database &db, const std::string &sqlStatement); 
	Statement(const Statement &);
	virtual ~Statement();

	unsigned long ParameterCount();
	void AssignNextParameter(const Nullable<std::string> &param);
	void AssignNextParameter(const Nullable<char> &param);
	void AssignNextParameter(const Nullable<short int> &param);
	void AssignNextParameter(const Nullable<unsigned long> &param);
	void AssignNextParameter(const Nullable<MYSQL_TIME> &param);
	void AssignNextParameter(const Nullable<Binary> &data);

	void ResetParameters();
	void Execute();
	bool FetchNextRow();
	bool Eof();
	unsigned long long NumberOfAffectedRows();
	bool operator!();

	Nullable<char> GetTinyDataInRow(unsigned int column);
	Nullable<short int> GetShortDataInRow(unsigned int column);
	Nullable<MYSQL_TIME> GetTimeDataInRow(unsigned int column);
	Nullable<std::string> GetStringDataInRow(unsigned int column);
	Nullable<Binary> GetBinaryDataInRow(unsigned int column);

protected:
	void GetDataInRow(unsigned int column, Nullable<std::string> &data);
	void GetDataInRow(unsigned int column, Nullable<char> &data);
	void GetDataInRow(unsigned int column, Nullable<short int> &data);
	void GetDataInRow(unsigned int column, Nullable<unsigned long> &data);
	void GetDataInRow(unsigned int column, Nullable<MYSQL_TIME> &data);
	void GetDataInRow(unsigned int column, Nullable<Binary> &data);

private:
	void AssignNextParameter(ParamBuffer *buffer);
	void Prepare();
	void ClearParameters();
	void ClearResults();

	unsigned int _numberResultColumns;
	bool _hasBlobField;
	my_ulonglong _numberAffectedRows;

	MYSQL_STMT *_stmt;
	MYSQL *_dbcopy;
	MYSQL_BIND *_bind;
	MYSQL_BIND *_resultBind;

	std::string _sqlStatement;
	bool _resultWasStored;
	bool _eof;

	std::vector<ParamBuffer*> _params;
	std::vector<ParamBuffer*> _resultParams;
};

Statement &operator<<(Statement &stmt, const ExecuteSentinel&);
Statement &operator<<(Statement &stmt, const FetchSentinel&);

template <class X> Statement &operator<<(Statement &stmt, const Nullable<X> &param) {
	stmt.AssignNextParameter(param);
	return stmt;
}

//template <class X> Statement &operator>>(Statement &stmt, Nullable<X> &data) {
//	stmt.GetNextDataInRow(data);
//	return stmt;
//}
