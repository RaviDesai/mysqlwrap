#include <iostream>
#include "UTFail.h"
#include "TestNullable.h"

using namespace std;

TestNullable::TestNullable() {}

void TestNullable::Test1() {
	cout << __PRETTY_FUNCTION__ << endl;

	Nullable<int> ni;
	UTASSERT(ni.HasValue() == false);
	UTASSERT(!ni);

	bool caught = false;
	try {
		int i = *ni;
		UTASSERT(i == 0xBAD);  // should never reach this line
	} catch (const NullableException &ne) {
		caught = true;
	}
	UTASSERT(caught == true);
}

void TestNullable::Test2() {
	cout << __PRETTY_FUNCTION__ << endl;

	Nullable<int> ni(7);
	UTASSERT(ni.HasValue() == true);
	UTASSERT((!ni) == false);
}

void TestNullable::Test3() {
	cout << __PRETTY_FUNCTION__ << endl;

	Nullable<int> ni;
	Nullable<int> ni2(14);

	ni = ni2;
	UTASSERT(ni.HasValue());
	UTASSERT(*ni == 14);

	ni.ClearValue();
	UTASSERT(!ni);

	ni = 9;
	UTASSERT(ni.HasValue());
	UTASSERT(*ni == 9);
}

void TestNullable::Test4() {
	cout << __PRETTY_FUNCTION__ << endl;

	Nullable<double> nd1;
	Nullable<double> nd2;

	UTASSERT(! nd1);
	UTASSERT(! nd2);
	UTASSERT(nd1 == nd2);
	UTASSERT(! (nd1 < nd2));
	UTASSERT(nd1 <= nd2);
	UTASSERT(! (nd1 != nd2));
	UTASSERT(nd1 >= nd2);
}

void TestNullable::Test5() {
	cout << __PRETTY_FUNCTION__ << endl;

	Nullable<short int> nsi1;
	Nullable<short int> nsi2(15);

	UTASSERT(! nsi1);
	UTASSERT(nsi2.HasValue());

	UTASSERT(! (nsi1 == nsi2));
	UTASSERT(nsi1 != nsi2);
	UTASSERT(nsi1 < nsi2);
	UTASSERT(nsi1 <= nsi2);
	UTASSERT(! (nsi1 > nsi2));
	UTASSERT(! (nsi1 >= nsi2));

	UTASSERT(! (nsi2 == nsi1));
	UTASSERT(nsi2 != nsi1);
	UTASSERT(! (nsi2 < nsi1));
	UTASSERT(! (nsi2 <= nsi1));
	UTASSERT(nsi2 > nsi1);
	UTASSERT(nsi2 >= nsi1);
}


void TestNullable::Test6() {
	cout << __PRETTY_FUNCTION__ << endl;

	Nullable<std::string> ns1;
	Nullable<std::string> ns2("test");
	ns1 = "test";

	UTASSERT(ns1 == ns2);
	UTASSERT(! (ns1 != ns2));
	UTASSERT(! (ns1 < ns2));
	UTASSERT(ns1 <= ns2);
	UTASSERT(! (ns1 > ns2));
	UTASSERT(ns1 >= ns2);

	UTASSERT(ns2 == ns1);
	UTASSERT(! (ns2 != ns1));
	UTASSERT(! (ns2 < ns1));
	UTASSERT(ns2 <= ns1);
	UTASSERT(! (ns2 > ns1));
	UTASSERT(ns2 >= ns1);
}

void TestNullable::Test7() {
	cout << __PRETTY_FUNCTION__ << endl;

	Nullable<int> ni1(5);
	Nullable<int> ni2(19);

	UTASSERT(! (ni1 == ni2));
	UTASSERT(ni1 != ni2);
	UTASSERT(ni1 < ni2);
	UTASSERT(ni1 <= ni2);
	UTASSERT(! (ni1 > ni2));
	UTASSERT(! (ni1 >= ni2));

	UTASSERT(! (ni2 == ni1));
	UTASSERT(ni2 != ni1);
	UTASSERT(! (ni2 < ni1));
	UTASSERT(! (ni2 <= ni1));
	UTASSERT(ni2 > ni1);
	UTASSERT(ni2 >= ni1);
}

int TestNullable::RunSpecificTest(NullableMemberPointer test) {
	int failures = 0;
	try {
		(this->*test)();
	} catch (UTFail &fail) {
		failures++;
		cout << fail << endl;
	}
	return failures;
}

int TestNullable::RunTests() {
	int failures = 0;
	failures += RunSpecificTest(&TestNullable::Test1);
	failures += RunSpecificTest(&TestNullable::Test2);
	failures += RunSpecificTest(&TestNullable::Test3);
	failures += RunSpecificTest(&TestNullable::Test4);
	failures += RunSpecificTest(&TestNullable::Test5);
	failures += RunSpecificTest(&TestNullable::Test6);
	failures += RunSpecificTest(&TestNullable::Test7);
	return failures;
}
