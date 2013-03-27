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

	class AdhocStatement {
	public:
		AdhocStatement(Database &db, const std::string &sqlStatement); 
		AdhocStatement(const AdhocStatement &copy);
		virtual ~AdhocStatement();

		void Execute();
		void ResetParameters();
		bool FetchNextRow();
		bool Eof();
		unsigned long long NumberOfAffectedRows();
		operator bool();
		unsigned long long NumberOfReturnedRows();
		unsigned int RemainingParameters();

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

		template<class X> void AssignNextParameter(const Nullable<X> &param) {
			AdhocParameter *buffer = new AdhocParameter();
			if (param.HasValue()) {
				buffer->SetData(param.const_deref());
			}
			AssignNextParameter(buffer);
		}

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
		void GetDataInRow(unsigned int column, Nullable<Julian> &data);
		void GetDataInRow(unsigned int column, Nullable<Binary> &data);
		void GetDataInRow(unsigned int column, Nullable<float> &data);
		void GetDataInRow(unsigned int column, Nullable<double> &data);

		unsigned int GetNextDataColumn();
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

	AdhocStatement &operator<<(AdhocStatement &stmt, const ExecuteSentinel&);
	AdhocStatement &operator<<(AdhocStatement &stmt, const FetchSentinel&);
	AdhocStatement &operator<<(AdhocStatement &stmt, const ResetSentinel&);

	template <class X> AdhocStatement &operator<<(AdhocStatement &stmt, const Nullable<X> &param) {
		stmt.AssignNextParameter(param);
		return stmt;
	}
}
