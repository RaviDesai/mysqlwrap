#pragma once

#include <iostream>

using namespace std;

#define UTASSERT(EXP) if (!(EXP)) throw UTFail(__PRETTY_FUNCTION__, __LINE__, #EXP);
#define UTEQUALS(LHS, RHS) if (!((LHS) == (RHS))) { cerr << "Failure: " << (LHS) << " == " << (RHS) << endl; throw UTFail(__PRETTY_FUNCTION__, __LINE__, #LHS + std::string(" != ") + #RHS); }

class UTFail {
public:
	UTFail(const string &func, int line, const string &msg);

	friend ostream& operator<<(ostream &out, const UTFail& fail);
private:
	string function;
	string message;
	int lineNumber;
};

