#pragma once

#include <vector>
#include "Database.h"
#include "ParamBuffer.h"
#include "Nullable.h"
#include "IStatement.h"
#include <mysql_time.h>

namespace MySQLWrap {

	class Statement : public IStatement {
	public:
		Statement(Database &db, const std::string &sqlStatement); 
		Statement(const Statement &);
		virtual ~Statement();

		virtual unsigned long ParameterCount();
		virtual unsigned long RemainingParameters();
		virtual void ResetParameters();
		virtual void Execute();
		virtual bool FetchNextRow();
		virtual bool Eof();
		virtual unsigned long long NumberOfAffectedRows();
		virtual operator bool();
		virtual unsigned long long NumberOfReturnedRows();

		virtual Nullable<char> GetTinyDataInRow(unsigned int column);
		virtual Nullable<unsigned char> GetUTinyDataInRow(unsigned int column);
		virtual Nullable<short int> GetShortDataInRow(unsigned int column);
		virtual Nullable<unsigned short int> GetUShortDataInRow(unsigned int column);
		virtual Nullable<int> GetLongDataInRow(unsigned int column);
		virtual Nullable<unsigned int> GetULongDataInRow(unsigned int column);
		virtual Nullable<Julian> GetTimeDataInRow(unsigned int column);
		virtual Nullable<std::string> GetStringDataInRow(unsigned int column);
		virtual Nullable<Binary> GetBinaryDataInRow(unsigned int column);
		virtual Nullable<float> GetFloatDataInRow(unsigned int column);
		virtual Nullable<double> GetDoubleDataInRow(unsigned int column);

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

	private:
		void AssignNextParameter(ParamBuffer *buffer);
		void Prepare();
		void ClearParameters();
		void ClearResults();

		unsigned int _numberResultColumns;
		unsigned int _currentColumn;
		bool _hasBlobField;
		my_ulonglong _numberAffectedRows;

		Database &_db;
		MYSQL_STMT *_stmt;
		MYSQL_BIND *_bind;
		MYSQL_BIND *_resultBind;

		std::string _sqlStatement;
		bool _resultWasStored;
		bool _eof;

		std::vector<ParamBuffer*> _params;
		std::vector<ParamBuffer*> _resultParams;
	};

}
