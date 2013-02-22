#pragma once

#include <vector>
#include "Database.h"
#include "Statement.h"
#include "ParamBuffer.h"
#include "Nullable.h"
#include <mysql_time.h>
#include <mysql.h>

namespace MySQLWrap {

	class AdhocStatement {
	public:
		AdhocStatement(Database &db, const std::string &sqlStatement); 
		AdhocStatement(const AdhocStatement &copy);
		virtual ~AdhocStatement();

		void Execute();
		bool FetchNextRow();
		bool Eof();
		unsigned long long NumberOfAffectedRows();
		operator bool();
		unsigned long long NumberOfReturnedRows();

		Nullable<char> GetTinyDataInRow(unsigned int column);
		Nullable<unsigned char> GetUTinyDataInRow(unsigned int column);
		Nullable<short int> GetShortDataInRow(unsigned int column);
		Nullable<unsigned short int> GetUShortDataInRow(unsigned int column);
		Nullable<int> GetLongDataInRow(unsigned int column);
		Nullable<unsigned int> GetULongDataInRow(unsigned int column);
		Nullable<MYSQL_TIME> GetTimeDataInRow(unsigned int column);
		Nullable<std::string> GetStringDataInRow(unsigned int column);
		Nullable<Binary> GetBinaryDataInRow(unsigned int column);
		Nullable<float> GetFloatDataInRow(unsigned int column);
		Nullable<double> GetDoubleDataInRow(unsigned int column);

		template <class X> AdhocStatement &operator>>(Nullable<X> &data) {
			GetDataInRow(GetNextDataColumn(), data);
			return *this;
		}
	protected:
		void GetDataInRow(unsigned int column, Nullable<std::string> &data);
		void GetDataInRow(unsigned int column, Nullable<char> &data);
		void GetDataInRow(unsigned int column, Nullable<unsigned char> &data);
		void GetDataInRow(unsigned int column, Nullable<short int> &data);
		void GetDataInRow(unsigned int column, Nullable<unsigned short int> &data);
		void GetDataInRow(unsigned int column, Nullable<int> &data);
		void GetDataInRow(unsigned int column, Nullable<unsigned int> &data);
		void GetDataInRow(unsigned int column, Nullable<MYSQL_TIME> &data);
		void GetDataInRow(unsigned int column, Nullable<Binary> &data);
		void GetDataInRow(unsigned int column, Nullable<float> &data);
		void GetDataInRow(unsigned int column, Nullable<double> &data);

		unsigned int GetNextDataColumn();

	private:
		void Prepare();

		unsigned int _numberResultColumns;
		unsigned int _currentColumn;
		my_ulonglong _numberAffectedRows;
		my_ulonglong _numberResultRows;

		MYSQL_RES *_result;
		MYSQL *_dbcopy;
		MYSQL_ROW _currentRow;
		MYSQL_FIELD *_fields;
		unsigned long* _currentRowLengths;

		std::string _sqlStatement;
		bool _resultWasStored;
		bool _eof;
	};

	AdhocStatement &operator<<(AdhocStatement &stmt, const ExecuteSentinel&);
	AdhocStatement &operator<<(AdhocStatement &stmt, const FetchSentinel&);
}
