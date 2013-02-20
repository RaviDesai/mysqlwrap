#include "Statement.h"
#include "DatabaseException.h"
#include <iostream>
#include <typeinfo>
#include <iomanip>

using namespace std;

namespace MySQLWrap {

Statement::Statement(Database &db, const std::string &sqlStatement) {
	_sqlStatement = sqlStatement;
	_dbcopy = db._db;
	_resultWasStored = false;
	_eof = true;
	_currentColumn = 0;

	Prepare();
}

Statement::Statement(const Statement &copy) {
	_sqlStatement = copy._sqlStatement;
	_dbcopy = copy._dbcopy;
	_resultWasStored = false;
	_eof = true;
	_currentColumn = 0;

	Prepare();
}

void Statement::Prepare() {
	_numberAffectedRows = 0;

	if ((_stmt = mysql_stmt_init(_dbcopy)) == NULL) {
		throw DatabaseException(_dbcopy, "Error in Statement::Prepare during initialize");
	}

	if (mysql_stmt_prepare(_stmt, _sqlStatement.c_str(), _sqlStatement.length()) != 0) {
		mysql_stmt_close(_stmt);
		throw DatabaseException(_stmt, "Error in Statement::Prepare during prepare");
	}

	_bind = NULL;
	unsigned long parameterCount = ParameterCount();
	if (parameterCount > 0) {
		_bind = (MYSQL_BIND *) malloc(sizeof(MYSQL_BIND) * parameterCount);
		memset(_bind, 0, sizeof(MYSQL_BIND) * parameterCount);
	}

	_numberResultColumns = 0;
	_hasBlobField = false;

	MYSQL_RES *metaData;
	if ((metaData = mysql_stmt_result_metadata(_stmt)) == NULL) {
		return;
	}

	_resultBind = NULL;
	_numberResultColumns = mysql_num_fields(metaData);
	if (_numberResultColumns > 0) {
		_resultBind = (MYSQL_BIND *) malloc(sizeof(MYSQL_BIND) * _numberResultColumns);
		memset(_resultBind, 0, sizeof(MYSQL_BIND) * _numberResultColumns);
	}

	int fieldPos = 0;
	MYSQL_FIELD *field;
	while ((field = mysql_fetch_field(metaData)) != NULL) {
		ParamBuffer *buffer = NULL;

		if ((field->type == MYSQL_TYPE_VAR_STRING) || 
		    (field->type == MYSQL_TYPE_STRING) ||
		    (field->type == MYSQL_TYPE_DECIMAL) ||
		    (field->type == MYSQL_TYPE_BIT) ||
		    (field->type == MYSQL_TYPE_VARCHAR)) {
			std::string str;
			buffer = new ParamBuffer(str, field->length);
			_resultBind[fieldPos].buffer_type = field->type;
			_resultBind[fieldPos].buffer = buffer->Buffer();
			_resultBind[fieldPos].buffer_length = buffer->BufferSize();
			_resultBind[fieldPos].length = buffer->BufferLength();
			_resultBind[fieldPos].is_null = buffer->IsNull();
			_resultBind[fieldPos].error = buffer->Error();
		}
		else if (field->type == MYSQL_TYPE_TINY) {
			if ((field->flags & UNSIGNED_FLAG) != 0) {
				buffer = new ParamBuffer((const unsigned char) 0);
			} else {
				buffer = new ParamBuffer((const char) 0);
			}
			_resultBind[fieldPos].buffer_type = MYSQL_TYPE_TINY;
			_resultBind[fieldPos].buffer = buffer->Buffer();
			_resultBind[fieldPos].buffer_length = buffer->BufferSize();
			_resultBind[fieldPos].length = buffer->BufferLength();
			_resultBind[fieldPos].is_null = buffer->IsNull();
			_resultBind[fieldPos].error = buffer->Error();
			_resultBind[fieldPos].is_unsigned = buffer->IsUnsigned();
		}
		else if ((field->type == MYSQL_TYPE_SHORT) ||
			 (field->type == MYSQL_TYPE_YEAR)) {
			if ((field->flags & UNSIGNED_FLAG) != 0) {
				buffer = new ParamBuffer((const unsigned short int) 0);
			} else {
				buffer = new ParamBuffer((const short int) 0);
			}
			_resultBind[fieldPos].buffer_type = MYSQL_TYPE_SHORT;
			_resultBind[fieldPos].buffer = buffer->Buffer();
			_resultBind[fieldPos].buffer_length = buffer->BufferSize();
			_resultBind[fieldPos].length = buffer->BufferLength();
			_resultBind[fieldPos].is_null = buffer->IsNull();
			_resultBind[fieldPos].error = buffer->Error();
			_resultBind[fieldPos].is_unsigned = buffer->IsUnsigned();
		}
		else if ((field->type == MYSQL_TYPE_LONG) ||
			 (field->type == MYSQL_TYPE_INT24)) {
			if ((field->flags & UNSIGNED_FLAG) != 0) {
				buffer = new ParamBuffer((const unsigned int) 0);
			} else {
				buffer = new ParamBuffer((const int) 0);
			}
			_resultBind[fieldPos].buffer_type = field->type;
			_resultBind[fieldPos].buffer = buffer->Buffer();
			_resultBind[fieldPos].buffer_length = buffer->BufferSize();
			_resultBind[fieldPos].length = buffer->BufferLength();
			_resultBind[fieldPos].is_null = buffer->IsNull();
			_resultBind[fieldPos].error = buffer->Error();
			_resultBind[fieldPos].is_unsigned = buffer->IsUnsigned();
		}
		else if ((field->type == MYSQL_TYPE_TIMESTAMP) ||
			 (field->type == MYSQL_TYPE_DATE) ||
			 (field->type == MYSQL_TYPE_TIME) ||
			 (field->type == MYSQL_TYPE_DATETIME)) {
			MYSQL_TIME time;
			buffer = new ParamBuffer(time);
			_resultBind[fieldPos].buffer_type = field->type;
			_resultBind[fieldPos].buffer = buffer->Buffer();
			_resultBind[fieldPos].buffer_length = buffer->BufferSize();
			_resultBind[fieldPos].length = buffer->BufferLength();
			_resultBind[fieldPos].is_null = buffer->IsNull();
			_resultBind[fieldPos].error = buffer->Error();
		}
		else if ((field->type == MYSQL_TYPE_BLOB) ||
			 (field->type == MYSQL_TYPE_TINY_BLOB) ||
			 (field->type == MYSQL_TYPE_MEDIUM_BLOB) ||
			 (field->type == MYSQL_TYPE_LONG_BLOB)) {
			_hasBlobField = true;
			Binary data(field->length);
			buffer = new ParamBuffer(data);
			_resultBind[fieldPos].buffer_type = field->type;
			_resultBind[fieldPos].buffer = buffer->Buffer();
			_resultBind[fieldPos].buffer_length = buffer->BufferSize();
			_resultBind[fieldPos].length = buffer->BufferLength();
			_resultBind[fieldPos].is_null = buffer->IsNull();
			_resultBind[fieldPos].error = buffer->Error();
		}

		if (buffer != NULL) {
			_resultParams.push_back(buffer);	
		}
		fieldPos++;
	}

	if (_resultParams.size() != _numberResultColumns) {
		mysql_free_result(metaData);
		mysql_stmt_close(_stmt);
		throw DatabaseException("Error in Statement::Prepare", 0, "----", "was not able to bind all parameters");
	}

	if (_hasBlobField) {
		my_bool setMax = 1;
		mysql_stmt_attr_set(_stmt, STMT_ATTR_UPDATE_MAX_LENGTH, &setMax);
	}

	if (_numberResultColumns > 0) {
		if (mysql_stmt_bind_result(_stmt, _resultBind) != 0) {
			mysql_free_result(metaData);
			mysql_stmt_close(_stmt);
			throw DatabaseException(_stmt, "Error in Statement::Prepare while binding results");
		}
	}

	mysql_free_result(metaData);
}

Statement::~Statement() {
	if (_resultWasStored) { 
		mysql_stmt_free_result(_stmt);
	}
	mysql_stmt_close(_stmt);
	ClearParameters();
	ClearResults();
}

unsigned long Statement::ParameterCount() {
	return mysql_stmt_param_count(_stmt);
}
	
void Statement::AssignNextParameter(ParamBuffer *buffer) {
	if (buffer == NULL) { 
		throw DatabaseException("Error in Statement::AssignNextParameter", 0, "----", "Buffer cannot be null");
	}

	unsigned int pos = _params.size();
	if (pos >= ParameterCount()) {
		delete buffer;
		throw DatabaseException("Error in Statement::AssignNextParameter", 0, "----", "Have already assigned all possible input parameters");
	}

	_params.push_back(buffer);

	_bind[pos].buffer_type = buffer->BufferType();
        _bind[pos].buffer = buffer->Buffer();
        _bind[pos].buffer_length = buffer->BufferSize();
        _bind[pos].is_null = buffer->IsNull();
        _bind[pos].length = buffer->BufferLength();
	_bind[pos].is_unsigned = buffer->IsUnsigned();
}

void Statement::AssignNextParameter(const Nullable<std::string> &str) { 
	if (! str.HasValue()) {
		AssignNextParameter(new ParamBuffer(MYSQL_TYPE_STRING, false));
	} else {
		AssignNextParameter(new ParamBuffer(*str, str->length()));
	}
}

void Statement::AssignNextParameter(const Nullable<float> &f) {
	if (! f.HasValue()) {
		AssignNextParameter(new ParamBuffer(MYSQL_TYPE_FLOAT, false));
	} else {
		AssignNextParameter(new ParamBuffer(*f));
	}	
}

void Statement::AssignNextParameter(const Nullable<double> &d) {
	if (! d.HasValue()) {
		AssignNextParameter(new ParamBuffer(MYSQL_TYPE_DOUBLE, false));
	} else {
		AssignNextParameter(new ParamBuffer(*d));
	}	
}

void Statement::AssignNextParameter(const Nullable<short int> &i) {
	if (! i.HasValue()) {
		AssignNextParameter(new ParamBuffer(MYSQL_TYPE_SHORT, false));
	} else {
		AssignNextParameter(new ParamBuffer(*i));
	}	
}

void Statement::AssignNextParameter(const Nullable<unsigned short int> &i) {
	if (! i.HasValue()) {
		AssignNextParameter(new ParamBuffer(MYSQL_TYPE_SHORT, true));
	} else {
		AssignNextParameter(new ParamBuffer(*i));
	}	
}

void Statement::AssignNextParameter(const Nullable<int> &i) {
	if (! i.HasValue()) {
		AssignNextParameter(new ParamBuffer(MYSQL_TYPE_LONG, false));
	} else {
		AssignNextParameter(new ParamBuffer(*i));
	}
}

void Statement::AssignNextParameter(const Nullable<unsigned int> &i) {
	if (! i.HasValue()) {
		AssignNextParameter(new ParamBuffer(MYSQL_TYPE_LONG, false));
	} else {
		AssignNextParameter(new ParamBuffer(*i));
	}
}

Nullable<int> Statement::GetLongDataInRow(unsigned int column) {
	Nullable<int> result;
	GetDataInRow(column, result);
	return result;
}

Nullable<unsigned int> Statement::GetULongDataInRow(unsigned int column) {
	Nullable<unsigned int> result;
	GetDataInRow(column, result);
	return result;
}

void Statement::AssignNextParameter(const Nullable<MYSQL_TIME> &tm) {
	if (! tm.HasValue()) {
		AssignNextParameter(new ParamBuffer(MYSQL_TYPE_TIME, false));
	} else {
		AssignNextParameter(new ParamBuffer(*tm));
	}
}

void Statement::AssignNextParameter(const Nullable<Binary> &data) {
	if (! data.HasValue()) {
		enum enum_field_types fieldType = MYSQL_TYPE_TINY_BLOB;
		if (data.HasValue()) {
			if (data->BufferLength() < 256) {
				fieldType = MYSQL_TYPE_TINY_BLOB;
			} else if (data->BufferLength() < 64 * 1024) {
				fieldType = MYSQL_TYPE_BLOB;
			} else if (data -> BufferLength() < 16 * 1024 * 1024) {
				fieldType = MYSQL_TYPE_MEDIUM_BLOB;
			} else {
				fieldType = MYSQL_TYPE_LONG_BLOB;
			}
		}
		AssignNextParameter(new ParamBuffer(fieldType, false));
	} else {
		AssignNextParameter(new ParamBuffer(data.const_deref()));
	}
}

void Statement::AssignNextParameter(const Nullable<char> &data) {
	if (! data.HasValue()) {
		AssignNextParameter(new ParamBuffer(MYSQL_TYPE_TINY, false));
	} else {
		AssignNextParameter(new ParamBuffer(*data));
	}
}

void Statement::AssignNextParameter(const Nullable<unsigned char> &data) {
	if (! data.HasValue()) {
		AssignNextParameter(new ParamBuffer(MYSQL_TYPE_TINY, true));
	} else {
		AssignNextParameter(new ParamBuffer(*data));
	}
}

void Statement::Execute() {
	_numberAffectedRows = 0;
	_eof = true;
	_currentColumn = 0;

	if (ParameterCount() != _params.size()) {
		throw DatabaseException("Error in Statement::Execute", 0, "----", "Have not yet assigned all parameters");
	}

	if (mysql_stmt_bind_param(_stmt, _bind) != 0) {
		throw DatabaseException(_stmt, "Error in Statement::Execute while binding parameters");
	}

	if (mysql_stmt_execute(_stmt) != 0) {
		throw DatabaseException(_stmt, "Error in Statement::Execute while executing statement");
	}

	if (_numberResultColumns > 0) {
		if (mysql_stmt_store_result(_stmt) != 0) { 
			throw DatabaseException(_stmt, "Error in Statement::Execute while storing results");
		}
		_eof = false;
	}
	else {
		_numberAffectedRows = mysql_stmt_affected_rows(_stmt);
	}

	_resultWasStored = true;

	if (_hasBlobField && _resultBind != NULL) {
		MYSQL_RES *res = mysql_stmt_result_metadata(_stmt);
		if (res != NULL) {
			for (unsigned int i = 0; i < _resultParams.size(); i++) {
				MYSQL_FIELD *column = mysql_fetch_field_direct(res, i);
				if (_resultBind[i].buffer_type == MYSQL_TYPE_BLOB) {
					_resultParams[i]->ResizeBlob(column->max_length);
					_resultBind[i].buffer = _resultParams[i]->Buffer();
					_resultBind[i].buffer_length = _resultParams[i]->BufferSize();
					_resultBind[i].length = _resultParams[i]->BufferLength();
				}
			}
			mysql_free_result(res);
		}
	}
}

void Statement::ResetParameters() { 
	while (! _params.empty()) {
		ParamBuffer *buf = _params.back();
		_params.pop_back();
		delete buf;
	}	

	mysql_stmt_reset(_stmt);
	mysql_stmt_free_result(_stmt);
	_eof = true;
}

void Statement::ClearParameters() {
	ResetParameters();
	if (_bind != NULL) {
		free(_bind);
	}
}

void Statement::ClearResults() {
	_eof = true;
	while (! _resultParams.empty()) {
		ParamBuffer *buf = _resultParams.back();
		_resultParams.pop_back();
		delete buf;
	}

	if ((_numberResultColumns > 0) && (_resultBind != NULL)) {
		free(_resultBind);
	}	
}

bool Statement::Eof() {
	return _eof;
}

bool Statement::FetchNextRow() { 
	_currentColumn = 0;
	bool ret = true;
	int result = mysql_stmt_fetch(_stmt);
	if (result == 1) {
		throw DatabaseException(_stmt, "Error in Statement::FetchNewRow while fetching");
	}
	else if (result == MYSQL_NO_DATA) {
		ret = false;
	}
	else if (result == MYSQL_DATA_TRUNCATED) {
		throw DatabaseException("Error in Statement::FetchNewRow while fetching", 0, "----", "Data would have been truncated.");
	}
	_eof = !ret;
	return ret;
}

void Statement::GetDataInRow(unsigned int column, Nullable<char> &result) {
	if (column >= _resultParams.size()) {
		throw DatabaseException("Error in Statement::GetDataInRow", 0, "----", "column out of range");
	}

	if (_resultBind[column].buffer_type != MYSQL_TYPE_TINY) {
		throw DatabaseException("Error in Statement::GetDataInRow", 0, "----", "column not of correct type");
	}

	if (_resultBind[column].is_unsigned) {
		throw DatabaseException("Error in Statement::GetDataInRow", 0, "----", "column is an unsigned data type");
	}

	if (! (*(_resultParams[column]->IsNull()))) {
		result = *((char *) _resultParams[column]->Buffer());
	} else {
		result.ClearValue();
	}
}

void Statement::GetDataInRow(unsigned int column, Nullable<unsigned char> &result) {
	if (column >= _resultParams.size()) {
		throw DatabaseException("Error in Statement::GetDataInRow", 0, "----", "column out of range");
	}

	if (_resultBind[column].buffer_type != MYSQL_TYPE_TINY) {
		throw DatabaseException("Error in Statement::GetDataInRow", 0, "----", "column not of correct type");
	}

	if (! _resultBind[column].is_unsigned) {
		throw DatabaseException("Error in Statement::GetDataInRow", 0, "----", "column is not an unsigned data type");
	}

	if (! (*(_resultParams[column]->IsNull()))) {
		result = *((char *) _resultParams[column]->Buffer());
	} else {
		result.ClearValue();
	}
}

Nullable<char> Statement::GetTinyDataInRow(unsigned int column) {
	Nullable<char> result;
	GetDataInRow(column, result);
	return result;
}

Nullable<unsigned char> Statement::GetUTinyDataInRow(unsigned int column) {
	Nullable<unsigned char> result;
	GetDataInRow(column, result);
	return result;
}

void Statement::GetDataInRow(unsigned int column, Nullable<int> &result) {
	if (column >= _resultParams.size()) {
		throw DatabaseException("Error in Statement::GetDataInRow", 0, "----", "column out of range");
	}

	if (_resultBind[column].buffer_type != MYSQL_TYPE_LONG) {
		throw DatabaseException("Error in Statement::GetDataInRow", 0, "----", "column not of correct type");
	}
	
	if (_resultBind[column].is_unsigned) {
		throw DatabaseException("Error in Statement::GetDataInRow", 0, "----", "column is an unsigned data type");
	}

	if (! (*(_resultParams[column]->IsNull()))) {
		result = *((int *) _resultParams[column]->Buffer());
	} else {
		result.ClearValue();
	}
}

void Statement::GetDataInRow(unsigned int column, Nullable<unsigned int> &result) {
	if (column >= _resultParams.size()) {
		throw DatabaseException("Error in Statement::GetDataInRow", 0, "----", "column out of range");
	}

	if (_resultBind[column].buffer_type != MYSQL_TYPE_LONG) {
		throw DatabaseException("Error in Statement::GetDataInRow", 0, "----", "column not of correct type");
	}
	
	if (! _resultBind[column].is_unsigned) {
		throw DatabaseException("Error in Statement::GetDataInRow", 0, "----", "column is not an unsigned data type");
	}

	if (! (*(_resultParams[column]->IsNull()))) {
		result = *((int *) _resultParams[column]->Buffer());
	} else {
		result.ClearValue();
	}
}

void Statement::GetDataInRow(unsigned int column, Nullable<short int> &result) {
	if (column >= _resultParams.size()) {
		throw DatabaseException("Error in Statement::GetDataInRow", 0, "----", "column out of range");
	}

	if ((_resultBind[column].buffer_type != MYSQL_TYPE_SHORT) &&
	    (_resultBind[column].buffer_type != MYSQL_TYPE_YEAR)) {
		throw DatabaseException("Error in Statement::GetDataInRow", 0, "----", "column not of correct type");
	}
	
	if (_resultBind[column].is_unsigned) {
		throw DatabaseException("Error in Statement::GetDataInRow", 0, "----", "column is an unsigned data type");
	}

	if (! (*(_resultParams[column]->IsNull()))) {
		result = *((short int *) _resultParams[column]->Buffer());
	} else {
		result.ClearValue();
	}
}

void Statement::GetDataInRow(unsigned int column, Nullable<unsigned short int> &result) {
	if (column >= _resultParams.size()) {
		throw DatabaseException("Error in Statement::GetDataInRow", 0, "----", "column out of range");
	}

	if ((_resultBind[column].buffer_type != MYSQL_TYPE_SHORT) &&
	    (_resultBind[column].buffer_type != MYSQL_TYPE_YEAR)) {
		throw DatabaseException("Error in Statement::GetDataInRow", 0, "----", "column not of correct type");
	}
	
	if (! _resultBind[column].is_unsigned) {
		throw DatabaseException("Error in Statement::GetDataInRow", 0, "----", "column is not an unsigned data type");
	}

	if (! (*(_resultParams[column]->IsNull()))) {
		result = *((short int *) _resultParams[column]->Buffer());
	} else {
		result.ClearValue();
	}
}

Nullable<short int> Statement::GetShortDataInRow(unsigned int column) {
	Nullable<short int> result;
	GetDataInRow(column, result);
	return result;
}

Nullable<unsigned short int> Statement::GetUShortDataInRow(unsigned int column) {
	Nullable<unsigned short int> result;
	GetDataInRow(column, result);
	return result;
}

void Statement::GetDataInRow(unsigned int column, Nullable<std::string> &result) {
	if (column >= _resultParams.size()) { 
		throw DatabaseException("Error in Statement::GetDataInRow", 0, "----", "column out of range");
	}

	if ((_resultBind[column].buffer_type != MYSQL_TYPE_VAR_STRING) &&
	    (_resultBind[column].buffer_type != MYSQL_TYPE_STRING) && 
	    (_resultBind[column].buffer_type != MYSQL_TYPE_VARCHAR) &&
	    (_resultBind[column].buffer_type != MYSQL_TYPE_DECIMAL) &&
	    (_resultBind[column].buffer_type != MYSQL_TYPE_BIT)) {
		throw DatabaseException("Error in Statement::GetDataInRow", 0, "----", "column not of correct type");
	}
	
	if (! (*(_resultParams[column]->IsNull()))) {
		result = std::string((const char *) _resultParams[column]->Buffer(), *(_resultParams[column]->BufferLength()));
	} else {
		result.ClearValue();
	}
}

Nullable<std::string> Statement::GetStringDataInRow(unsigned int column) { 
	Nullable<std::string> result;
	GetDataInRow(column, result);
	return result;
}

void Statement::GetDataInRow(unsigned int column, Nullable<double> &result) {
	if (column >= _resultParams.size()) { 
		throw DatabaseException("Error in Statement::GetDataInRow", 0, "----", "column out of range");
	}

	if (_resultBind[column].buffer_type != MYSQL_TYPE_DOUBLE) {
		throw DatabaseException("Error in Statement::GetDataInRow", 0, "----", "column not of correct type");
	}
	
	if (! (*(_resultParams[column]->IsNull()))) {
		result = *((double *) _resultParams[column]->Buffer());
	} else {
		result.ClearValue();
	}
}

Nullable<double> Statement::GetDoubleDataInRow(unsigned int column) {
	Nullable<double> result;
	GetDataInRow(column, result);
	return result;
}

void Statement::GetDataInRow(unsigned int column, Nullable<float> &result) {
	if (column >= _resultParams.size()) { 
		throw DatabaseException("Error in Statement::GetDataInRow", 0, "----", "column out of range");
	}

	if (_resultBind[column].buffer_type != MYSQL_TYPE_FLOAT) {
		throw DatabaseException("Error in Statement::GetDataInRow", 0, "----", "column not of correct type");
	}
	
	if (! (*(_resultParams[column]->IsNull()))) {
		result = *((float *) _resultParams[column]->Buffer());
	} else {
		result.ClearValue();
	}
}

Nullable<float> Statement::GetFloatDataInRow(unsigned int column) {
	Nullable<float> result;
	GetDataInRow(column, result);
	return result;
}

void Statement::GetDataInRow(unsigned int column, Nullable<MYSQL_TIME> &result) {
	if (column >= _resultParams.size()) {
		throw DatabaseException("Error in Statement::GetDataInRow", 0, "----", "column out of range");
	}

	if ((_resultBind[column].buffer_type != MYSQL_TYPE_TIMESTAMP) &&
	    (_resultBind[column].buffer_type != MYSQL_TYPE_DATE) &&
	    (_resultBind[column].buffer_type != MYSQL_TYPE_TIME) && 
	    (_resultBind[column].buffer_type != MYSQL_TYPE_DATETIME))  {
		throw DatabaseException("Error in Statement::GetDataInRow", 0, "----", "column not of correct type");
	}

	if (! (*(_resultParams[column]->IsNull()))) {
		result = *((MYSQL_TIME *) _resultParams[column]->Buffer());
	} else {
		result.ClearValue();
	}
}

Nullable<MYSQL_TIME> Statement::GetTimeDataInRow(unsigned int column) {
	Nullable<MYSQL_TIME> result;
	GetDataInRow(column, result);
	return result;
}

void Statement::GetDataInRow(unsigned int column, Nullable<Binary> &result) {
	if (column >= _resultParams.size()) {
		throw DatabaseException("Error in Statement::GetDataInRow", 0, "----", "column out of range");
	}

	if (! (*(_resultParams[column]->IsNull()))) {
		if (_resultBind[column].buffer_type == MYSQL_TYPE_TINY_BLOB) {
			if (*(_resultParams[column]->BufferLength()) >= 256) {
				throw DatabaseException("Error in Statement::GetDataInRow", 0, "----", "data too large for Blob type");
			}
		} else if (_resultBind[column].buffer_type == MYSQL_TYPE_BLOB) {
			if (*(_resultParams[column]->BufferLength()) >= 64 * 1024) {
				throw DatabaseException("Error in Statement::GetDataInRow", 0, "----", "data too large for Blob type");
			}
		} else if (_resultBind[column].buffer_type == MYSQL_TYPE_MEDIUM_BLOB) {
			if (*(_resultParams[column]->BufferLength()) >= 16 * 1024 * 1024) {
				throw DatabaseException("Error in Statement::GetDataInRow", 0, "----", "data too large for Medium Blob type");
			}
		} else if (_resultBind[column].buffer_type == MYSQL_TYPE_LONG_BLOB) {
			if (*(_resultParams[column]->BufferLength()) >= (size_t) 4 * 1024 * 1024 * 1024) {
				throw DatabaseException("Error in Statement::GetDataInRow", 0, "----", "data too large for Large Blob type");
			}
		} else {
			throw DatabaseException("Error in Statement::GetDataInRow<Binary>", 0, "----", "column not of correct type (should be Blob)");
		}

		if (mysql_stmt_fetch_column(_stmt, &(_resultBind[column]), column, 0) != 0) {
			throw DatabaseException(_stmt, "Error in GetDataInRow<Binary>");
		}	

		Binary fromdb;
		fromdb.AssignDataToBuffer((unsigned char *)_resultParams[column]->Buffer(), *(_resultParams[column]->BufferLength()));
		result = fromdb;
	} else {
		result.ClearValue();
	}
}

Nullable<Binary> Statement::GetBinaryDataInRow(unsigned int column) {
	Nullable<Binary> result;
	GetDataInRow(column, result);
	return result;
}

unsigned long long Statement::NumberOfAffectedRows() {
	return _numberAffectedRows;
}

unsigned long long Statement::NumberOfReturnedRows() { 
	return mysql_stmt_num_rows(_stmt);
}

Statement::operator bool() {
	return ! Eof();
}

unsigned int Statement::GetNextDataColumn() {
	unsigned int result = _currentColumn;
	_currentColumn++;
	return result;
}

ExecuteSentinel::ExecuteSentinel() {}

Statement &operator<<(Statement &stmt, const ExecuteSentinel&) {
	stmt.Execute();
	return stmt;
}

FetchSentinel::FetchSentinel() {}

Statement &operator<<(Statement &stmt, const FetchSentinel&) {
	stmt.FetchNextRow();
	return stmt;
}

ResetSentinel::ResetSentinel() {}

Statement &operator<<(Statement &stmt, const ResetSentinel&) {
	stmt.ResetParameters();
	return stmt;
}

}
