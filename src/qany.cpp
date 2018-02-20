#include "qany.h"

oansistringstream& operator << (oansistringstream& os, Qany& __q) {
	os << __q.as_string().as_char();
	return os;
}

void Qany::__release_mem () {
	if (qt_null == _qtype)
		return;
	if (_data == NULL)
		return;
	if (_qtype == qt_int) {
		delete (pointer<int> ());
	} else if (_qtype == qt_float) {
		delete (pointer<float> ());
	} else if (_qtype == qt_string) {
		delete[] (pointer<char> ());
	} else {
		Qany_Exception ("__release_mem: unexcept type");
	}
	_data = NULL;
	// _data_size = 0;
	_qtype = qt_null;
}

template<class __T>
void Qany::__dup_rock_type (__T __source, QanyType __qt) {
	__release_mem ();
	__T* _newone = new __T;
	if (_newone == NULL) {
		Qany_Exception ("__dup_new_type: memory used out");
	}
	*_newone = __source;
	// _data_size = sizeof (__T);
	_data = static_cast<void*>(_newone);
	_qtype = __qt;
}

void Qany::__dup_string_type (const char* __str) {
	if (__str == NULL) {
		Qany_Exception ("__dup_string_type: source NULL");
	}
	int _src_len = strlen (__str);
	char* _newone = new char[_src_len + 1];
	if (_newone == NULL) {
		Qany_Exception ("__dup_string_type: memory used out");
	}
	strcpy (_newone, __str);
	__release_mem ();
	_data = static_cast<void*> (_newone);
	// _data_size = _src_len + 1;
	_qtype = qt_string;
}

void Qany::__dup_any (const Qany& __any) {
	if (__any.is_null()) {
		set_null ();
		return;
	} else if (__any.type() == qt_string) {
		__dup_string_type (__any.const_pointer<char>());
	} else if (__any.type() == qt_int) {
		int __v = __any.value<int> ();
		__dup_rock_type (__v, qt_int);
	} else if (__any.type() == qt_float) {
		float __v = __any.value<float> ();
		__dup_rock_type (__v, qt_float);
	} else {
		Qany_Exception ("__dup_any: unexpect type");
	}
}

template <class __T>
__T Qany::__rock_to_rock () {
	if (_qtype == qt_int) {
		return static_cast<__T> (value<int>());
	} else if (_qtype == qt_float) {
		return static_cast<__T> (value<float>());
	} else {
		Qany_Exception ("__rock_to_rock: unexpection type");
		return __T(0);
	}
}

template<class __T>
__T Qany::__string_to_rock () {
	ansistring __str (const_pointer<char> ());
	__T __v;
	__str.as_type (__v);
	return __v;
}

template<class __T> 
ansistring Qany::__rock_to_string (const __T& __v) {
	ansistring _s("");
	oansistringstream __oss (_s);
	__oss << __v;
	return _s;
}

template <class __T> 
__T Qany::as_rock () {
	if (is_null()) {
		Qany_Exception ("as_rock: null cannot as_rock");
	} else if (is_rocktype()) {
		return __rock_to_rock<__T> ();
	} else if (is_string()) {
		return __string_to_rock<__T> ();
	} else {
		Qany_Exception ("as_rock: unexcept type");
	}
	return __T(0);
}

ansistring Qany::as_string () {
	if (_qtype == qt_string) {
		ansistring __str (pointer<char> ());
		return __str;
	} else if (_qtype == qt_int) {
		return __rock_to_string<int> (value<int> ());
	} else if (_qtype == qt_float) {
		return __rock_to_string<float> (value<float> ());
	} else if (_qtype == qt_null) {
		ansistring __str("_$null_");
		return __str;
	} else {
		Qany_Exception ("as_string: unexcept type");
		return ansistring("");
	}
}

void Qany::expand_string (int __newsize) {
	if (__newsize <= 0)
		Qany_Exception ("expand_string: size under 0");
	char* __str = pointer<char> ();
	int _src_len = strlen (__str) + 1;
	if (__newsize > _src_len)
		_src_len = __newsize;
	char* _newone = new char[_src_len];
	if (_newone == NULL) {
		Qany_Exception ("expand_string: memory used out");
	}
	strcpy (_newone, __str);
	__release_mem ();
	_data = static_cast<void*> (_newone);
	// _data_size = _src_len;
	_qtype = qt_string;
}


