#include "UTFail.h"

UTFail::UTFail(const string &func, int line, const string &msg) {
	function = func;
	message = msg;
	lineNumber = line;
}

ostream& operator<<(ostream &out, const UTFail &fail) {
	out << "UT Failure in " << fail.function << " line: " << fail.lineNumber << " (" << fail.message << ").";
	return out;
}

