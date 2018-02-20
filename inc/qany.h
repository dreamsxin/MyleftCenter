//
// QANY is designed as a package for swap common type
// support UNIX/WINDOWS platform
//
// (@) qany.h
//

#ifndef __QANY_H_
#define __QANY_H_

#include "ansistring.h"

void Qany_Exception (const char*);

enum QanyType {
	qt_null, qt_string, 
	qt_int, qt_float
};

class Qany {
protected:
	QanyType _qtype;
	void * _data;
public:
	template<class __T> __T& var() {
		return *(static_cast<__T*> (_data));
	}
	template<class __T> __T value() const {
		return *(__T*) (_data);
	}
	template<class __T> __T* pointer() {
		return static_cast<__T*> (_data);
	}
	template<class __T> const __T* const_pointer() const {
		return (const __T*) (_data);
	}
private:
	void __release_mem ();

	template<class __T>
	void __dup_rock_type (__T __source, QanyType __qt);

	void __dup_string_type (const char* __str);

	void __dup_any (const Qany& __any);

	template <class __T> __T __rock_to_rock ();

	template<class __T> __T __string_to_rock ();

	template<class __T> ansistring __rock_to_string (const __T& __v);

public:
	inline QanyType type() const {
		return _qtype;
	}
	inline const char* type_name() const {
		switch (_qtype) {
		case qt_null:
			return "null";
		case qt_int:
			return "int";
		case qt_float:
			return "float";
		case qt_string:
			return "string";
		}
		return "unknown";
	}
	inline bool is_null() const {
		return _qtype == qt_null;
	}
	inline void set_null() {
		__release_mem ();
		_data = NULL;
		// _data_size = 0;
		_qtype = qt_null;
	}
	inline bool is_rocktype () const {
		return (_qtype == qt_int) || (_qtype == qt_float);
	}
	inline bool is_float_or_double() const {
		return (_qtype == qt_float);
	}
	inline bool is_string () const {
		return (_qtype == qt_string);
	}

	Qany () : _qtype(qt_null), _data(NULL)
		/* , _data_size(0) */ {}
	~Qany() { __release_mem (); }

	Qany (int __v) : _data(NULL) { __dup_rock_type (__v, qt_int); }
	Qany (float __v) : _data(NULL) { __dup_rock_type (__v, qt_float); }

	Qany (const ansistring& __v) : _data(NULL) { __dup_string_type (__v.as_char()); }
	Qany (const char* __v) : _data(NULL) { __dup_string_type (__v); }
	Qany (const Qany& __q) : _data(NULL) { __dup_any (__q); }

	inline Qany& operator = (int __v) {
		__dup_rock_type (__v, qt_int); return *this;
	}
	inline Qany& operator = (float __v) {
		__dup_rock_type (__v, qt_float); return *this;
	}
	inline Qany& operator = (const ansistring& __v) {
		__dup_string_type (__v.as_char()); return *this;
	}
	inline Qany& operator = (const char* __v) {
		__dup_string_type (__v); return *this;
	}
	inline Qany& operator = (const Qany& __v) {
		if (this == &__v)
			return *this;
		__dup_any (__v); return *this;
	}

	template <class __T> __T as_rock ();

	int as_int () { return as_rock<int> (); }
	float as_float () { return as_rock<float> (); }

	operator int() { return as_int(); }
	operator float() { return as_float(); }
	operator ansistring() {return as_string(); }

	ansistring as_string ();

	void expand_string (int __newsize);

	inline void pack_string () {
		ansistring __str (const_pointer<char>());
		__dup_string_type (__str.as_char());
	}
};

oansistringstream& operator << (oansistringstream& os, Qany& __q);

typedef Qany QAny;

#endif


