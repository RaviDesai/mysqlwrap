#pragma once

#include <vector>

#include "Database.h"
#include "Statement.h"
#include "ParamBuffer.h"
#include "Nullable.h"
#include "AdhocParameter.h"
#include "Julian.h"

#include <mysql.h>
#include <mysql_time.h>

namespace MySQLWrap {

	class AdhocStatement : public IStatement {
	public:
		AdhocStatement(Database &db, const std::string &sqlStatement); 
		AdhocStatement(const AdhocStatement &copy);
		virtual ~AdhocStatement();

		virtual unsigned long ParameterCount();
		virtual unsigned long RemainingParameters();
		virtual void Execute();
		virtual void ResetParameters();
		virtual bool FetchNextRow();
		virtual bool Eof();
		virtual unsigned long long NumberOfAffectedRows();
		virtual operator bool();
		virtual unsigned long long NumberOfReturnedRows();

		Nullable<char> GetTinyDataInRow(unsigned int column);
		Nullable<unsigned char> GetUTinyDataInRow(unsigned int column);
		Nullable<short int> GetShortDataInRow(unsigned int column);
		Nullable<unsigned short int> GetUShortDataInRow(unsigned int column);
		Nullable<int> GetLongDataInRow(unsigned int column);
		Nullable<unsigned int> GetULongDataInRow(unsigned int column);
		Nullable<Julian> GetTimeDataInRow(unsigned int column);
		Nullable<std::string> GetStringDataInRow(unsigned int column);
		Nullable<Binary> GetBinaryDataInRow(unsigned int column);
		Nullable<float> GetFloatDataInRow(unsigned int column);
		Nullable<double> GetDoubleDataInRow(unsigned int column);

		virtual void AssignNextParameter(const Nullable<std::string> &data);
		virtual void AssignNextParameter(const Nullable<char> &data);
		virtual void AssignNextParameter(const Nullable<unsigned char> &data);
		virtual void AssignNextParameter(const Nullable<short int> &data);
		virtual void AssignNextParameter(const Nullable<unsigned short int> &data);
		virtual void AssignNextParameter(const Nullable<int> &data);
		virtual void AssignNextParameter(const Nullable<unsigned int> &data);
		virtual void AssignNextParameter(const Nullable<Julian> &data);
		virtual void AssignNextParameter(const Nullable<Binary> &data);
		virtual void AssignNextParameter(const Nullable<float> &data);
		virtual void AssignNextParameter(const Nullable<double> &data);

		virtual void GetDataInRow(unsigned int column, Nullable<std::string> &data);
		virtual void GetDataInRow(unsigned int column, Nullable<char> &data);
		virtual void GetDataInRow(unsigned int column, Nullable<unsigned char> &data);
		virtual void GetDataInRow(unsigned int column, Nullable<short int> &data);
		virtual void GetDataInRow(unsigned int column, Nullable<unsigned short int> &data);
		virtual void GetDataInRow(unsigned int column, Nullable<int> &data);
		virtual void GetDataInRow(unsigned int column, Nullable<unsigned int> &data);
		virtual void GetDataInRow(unsigned int column, Nullable<Julian> &data);
		virtual void GetDataInRow(unsigned int column, Nullable<Binary> &data);
		virtual void GetDataInRow(unsigned int column, Nullable<float> &data);
		virtual void GetDataInRow(unsigned int column, Nullable<double> &data);

	protected:
		virtual unsigned int GetNextDataColumn();
		void StoreSqlStatement(const std::string &sqlStatement);

	private:
		void Prepare();
		std::string ReplaceInsertions();
		void ScanForInsertions();
		Nullable<std::string> GetStringDataInRowInternal(unsigned int column);
		void AssignNextParameter(AdhocParameter *value);

		unsigned int _numberResultColumns;
		unsigned int _currentColumn;
		my_ulonglong _numberAffectedRows;
		my_ulonglong _numberResultRows;

		Database &_db;
		MYSQL_RES *_result;
		MYSQL_ROW _currentRow;
		MYSQL_FIELD *_fields;
		unsigned long* _currentRowLengths;

		std::wstring _sqlStatement;
		bool _resultWasStored;
		bool _eof;
		unsigned int _numberParams;
		std::vector<AdhocParameter*> _params;
	};
}
