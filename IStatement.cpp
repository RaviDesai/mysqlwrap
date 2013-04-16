#include "IStatement.h"

namespace MySQLWrap {

ExecuteSentinel::ExecuteSentinel() {}

IStatement &operator<<(IStatement &stmt, const ExecuteSentinel&) {
	stmt.Execute();
	return stmt;
}

FetchSentinel::FetchSentinel() {}

IStatement &operator<<(IStatement &stmt, const FetchSentinel&) {
	stmt.FetchNextRow();
	return stmt;
}

ResetSentinel::ResetSentinel() {}

IStatement &operator<<(IStatement &stmt, const ResetSentinel&) {
	stmt.ResetParameters();
	return stmt;
}

}
