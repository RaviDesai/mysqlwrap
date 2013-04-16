#pragma once

#include <stdlib.h>
#include <iostream>
#include <utility>

using namespace std;

namespace MySQLWrap {

class NullableException {
public:
	NullableException() {}
	friend ostream &operator<<(ostream &out, const NullableException &ne) {
		out << "Error dereferencing a Nullable type that does not have a value"; 
		return out;
	}
};


template <class T>
class Nullable {
public:
	Nullable() {
		_hasValue = false;
	}
	Nullable(T &value) {
		_storage = value;
		_hasValue = true;
	}
	Nullable(T const &value) {
		_storage = value;
		_hasValue = true;
	}
		
	Nullable(const Nullable &copy) {
		_hasValue = copy._hasValue;	
		if (_hasValue) {
			_storage = copy._storage;
		}
	}
	~Nullable() {
		_hasValue = false;
	}

	Nullable &operator=(const T& value) {
		_storage = value;
		_hasValue = true;
		return *this;
	}

	Nullable &operator=(T& value) {
		_storage = value;
		_hasValue = true;
		return *this;
	}

	Nullable &operator=(const Nullable &equal) {
		_hasValue = equal._hasValue;
		if (_hasValue) { 
			_storage = equal._storage;
		}
		return *this;
	}

	void ClearValue() { 
		_hasValue = false;
	}
	
	bool HasValue() const {
		return _hasValue;
	}

	T const *operator->() const {
		if (! _hasValue) {
			throw NullableException();
		}
		return &_storage;
	}

	const T& const_deref() const {
		if (! _hasValue) {
			throw NullableException();
		}
		return _storage;
	}

	T& deref() {
		if (! _hasValue) {
			throw NullableException();
		}
		return _storage;
	}

	T const operator *() const {
		if (! _hasValue) {
			throw NullableException();
		}
		return _storage;
	}

	operator bool() const {
		return _hasValue;
	}

	bool operator ! () const { 
		return (! _hasValue);
	}

	friend ostream &operator<<(ostream &out, const Nullable &value) {
		if (! value) {
			out << "<null>";
		} else {
			out << *value;
		}
		return out;
	}

private:
	T _storage;
	bool _hasValue;
};

template<class T> inline bool operator == (const Nullable<T> &x, const Nullable<T> &y) {
	if ((!x) && (!y)) return true;
	if ((!x) || (!y)) return false;
	return (*x == *y);
}

template<class T> inline bool operator != (const Nullable<T> &x, const Nullable<T> &y) {
	if ((!x) && (!y)) return false;
	if ((!x) || (!y)) return true;
	return (*x != *y);
}

template<class T> inline bool operator < (const Nullable<T> &x, const Nullable<T> &y) { 
	if ((!x) && (!y)) return false;
	if (!x) return true;
	if (!y) return false;
	return (*x < *y);
}

template<class T> inline bool operator > (const Nullable<T> &x, const Nullable<T> &y) {
	if ((!x) && (!y)) return false;
	if (!x) return false;
	if (!y) return true;
	return (*x > *y);
}

template<class T> inline bool operator <= (const Nullable<T> &x, const Nullable<T> &y) {
	if ((!x) && (!y)) return true;
	if (!x) return true;
	if (!y) return false;
	return (*x <= *y);
}

template<class T> inline bool operator >= (const Nullable<T> &x, const Nullable<T> &y) {
	if ((!x) && (!y)) return true;
	if (!x) return false;
	if (!y) return true;
	return (*x >= *y);
}

}
