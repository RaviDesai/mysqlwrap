#pragma once

#include <vector>
#include "Database.h"
#include "ParamBuffer.h"
#include "Nullable.h"
#include <mysql_time.h>

namespace MySQLWrap {

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

	class ResetSentinel {
	public:
		ResetSentinel();
	};
	const ResetSentinel reset;

	class IStatement {
	public:
		IStatement() {}
		virtual ~IStatement() {};

		virtual unsigned long ParameterCount() = 0;
		virtual unsigned long RemainingParameters() = 0;
		virtual void ResetParameters() = 0;
		virtual void Execute() = 0;
		virtual bool FetchNextRow() = 0;
		virtual bool Eof() = 0;
		virtual unsigned long long NumberOfAffectedRows() = 0;
		virtual operator bool() = 0;
		virtual unsigned long long NumberOfReturnedRows() = 0;

		// virtual Nullable<char> GetTinyDataInRow(unsigned int column) = 0;
		// virtual Nullable<unsigned char> GetUTinyDataInRow(unsigned int column) = 0;
		// virtual Nullable<short int> GetShortDataInRow(unsigned int column) = 0;
		// virtual Nullable<unsigned short int> GetUShortDataInRow(unsigned int column) = 0;
		// virtual Nullable<int> GetLongDataInRow(unsigned int column) = 0;
		// virtual Nullable<unsigned int> GetULongDataInRow(unsigned int column) = 0;
		// virtual Nullable<Julian> GetTimeDataInRow(unsigned int column) = 0;
		// virtual Nullable<std::string> GetStringDataInRow(unsigned int column) = 0;
		// virtual Nullable<Binary> GetBinaryDataInRow(unsigned int column) = 0;
		// virtual Nullable<float> GetFloatDataInRow(unsigned int column) = 0;
		// virtual Nullable<double> GetDoubleDataInRow(unsigned int column) = 0;

		virtual void AssignNextParameter(const Nullable<std::string> &data) = 0;
		virtual void AssignNextParameter(const Nullable<char> &data) = 0;
		virtual void AssignNextParameter(const Nullable<unsigned char> &data) = 0;
		virtual void AssignNextParameter(const Nullable<short int> &data) = 0;
		virtual void AssignNextParameter(const Nullable<unsigned short int> &data) = 0;
		virtual void AssignNextParameter(const Nullable<int> &data) = 0;
		virtual void AssignNextParameter(const Nullable<unsigned int> &data) = 0;
		virtual void AssignNextParameter(const Nullable<Julian> &data) = 0;
		virtual void AssignNextParameter(const Nullable<Binary> &data) = 0;
		virtual void AssignNextParameter(const Nullable<float> &data) = 0;
		virtual void AssignNextParameter(const Nullable<double> &data) = 0;

		virtual void GetDataInRow(unsigned int column, Nullable<std::string> &data) = 0;
		virtual void GetDataInRow(unsigned int column, Nullable<char> &data) = 0;
		virtual void GetDataInRow(unsigned int column, Nullable<unsigned char> &data) = 0;
		virtual void GetDataInRow(unsigned int column, Nullable<short int> &data) = 0;
		virtual void GetDataInRow(unsigned int column, Nullable<unsigned short int> &data) = 0;
		virtual void GetDataInRow(unsigned int column, Nullable<int> &data) = 0;
		virtual void GetDataInRow(unsigned int column, Nullable<unsigned int> &data) = 0;
		virtual void GetDataInRow(unsigned int column, Nullable<Julian> &data) = 0;
		virtual void GetDataInRow(unsigned int column, Nullable<Binary> &data) = 0;
		virtual void GetDataInRow(unsigned int column, Nullable<float> &data) = 0;
		virtual void GetDataInRow(unsigned int column, Nullable<double> &data) = 0;

		template <class X> IStatement &operator>>(Nullable<X> &data) {
			GetDataInRow(GetNextDataColumn(), data);
			return *this;
		}

	protected:
		virtual unsigned int GetNextDataColumn() = 0;
	};

	IStatement &operator<<(IStatement &stmt, const ExecuteSentinel&);
	IStatement &operator<<(IStatement &stmt, const FetchSentinel&);
	IStatement &operator<<(IStatement &stmt, const ResetSentinel&);

	template <class X> IStatement &operator<<(IStatement &stmt, const Nullable<X> &param) {
		stmt.AssignNextParameter(param);
		return stmt;
	}

}
