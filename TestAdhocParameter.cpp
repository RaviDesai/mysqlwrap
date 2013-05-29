#include <iostream>
#include <iomanip>
#include <sstream>

#include "UTFail.h"
#include "TestAdhocParameter.h"
#include "AdhocParameter.h"

using namespace std;

TestAdhocParameter::TestAdhocParameter() {}

void TestAdhocParameter::Test1() {
	cout << __PRETTY_FUNCTION__ << endl;
	AdhocParameter p;
	UTASSERT(p.IsNull());
	p.SetData((unsigned char) 220);
	UTASSERT(! p.IsNull());

	std::string result = p.Get();	
	UTASSERT(result.size() == 1);
	unsigned char res0 = (unsigned char) result[0];
	UTASSERT(res0 == (unsigned char) 220);
}

void TestAdhocParameter::Test2() {
	cout << __PRETTY_FUNCTION__ << endl;
	AdhocParameter p;
	UTASSERT(p.IsNull());
	p.SetData((char) 'R');
	UTASSERT(! p.IsNull());

	std::string result = p.Get();
	UTASSERT(result.size() == 1);
	char res0 = (char) result[0];
	UTASSERT(res0 == (char) 'R');
}

void TestAdhocParameter::Test3() {
	cout << __PRETTY_FUNCTION__ << endl;
	AdhocParameter p;
	UTASSERT(p.IsNull());
	p.SetData((unsigned short) 16000);
	UTASSERT(! p.IsNull());

	std::string result = p.Get();
	UTASSERT(result.size() == 5);
	UTASSERT(strcmp(result.c_str(), "16000") == 0);
}

void TestAdhocParameter::Test4() {
	cout << __PRETTY_FUNCTION__ << endl;

	AdhocParameter p;
	UTASSERT(p.IsNull());
	p.SetData((short) 8100);
	UTASSERT(! p.IsNull());

	std::string result = p.Get();
	UTASSERT(result.size() == 4);
	UTASSERT(strcmp(result.c_str(), "8100") == 0);
}

void TestAdhocParameter::Test5() {
	cout << __PRETTY_FUNCTION__ << endl;

	AdhocParameter p;
	UTASSERT(p.IsNull());
	p.SetData((unsigned int) 3000000000);
	UTASSERT(! p.IsNull());

	std::string result = p.Get();
	UTASSERT(result.size() == 10);
	UTASSERT(strcmp(result.c_str(), "3000000000") == 0);
}


void TestAdhocParameter::Test6() {
	cout << __PRETTY_FUNCTION__ << endl;

	AdhocParameter p;
	UTASSERT(p.IsNull());
	p.SetData((int) 1000000000);
	UTASSERT(! p.IsNull());

	std::string result = p.Get();
	UTASSERT(result.size() == 10);
	UTASSERT(strcmp(result.c_str(), "1000000000") == 0);
}

void TestAdhocParameter::Test7() {
	cout << __PRETTY_FUNCTION__ << endl;

	AdhocParameter p;
	UTASSERT(p.IsNull());
	p.SetData((float) 3.14f);
	UTASSERT(! p.IsNull());

	std::string result = p.Get();
	cout << result << endl;
	UTASSERT(result.size() == 4);
	UTASSERT(strcmp(result.c_str(), "3.14") == 0);
}

int TestAdhocParameter::RunSpecificTest(AdhocParameterMemberPointer test) {
	int failures = 0;
	try {
		(this->*test)();
	} catch (UTFail &fail) {
		failures++;
		cout << fail << endl;
	}
	return failures;
}

int TestAdhocParameter::RunTests() {
	int failures = 0;
	failures += RunSpecificTest(&TestAdhocParameter::Test1);
	failures += RunSpecificTest(&TestAdhocParameter::Test2);
	failures += RunSpecificTest(&TestAdhocParameter::Test3);
	failures += RunSpecificTest(&TestAdhocParameter::Test4);
	failures += RunSpecificTest(&TestAdhocParameter::Test5);
	failures += RunSpecificTest(&TestAdhocParameter::Test6);
	failures += RunSpecificTest(&TestAdhocParameter::Test7);
	return failures;
}
