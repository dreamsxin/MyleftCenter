#ifndef _ANSI_STRING_H_
#define _ANSI_STRING_H_

#include <e32std.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FxAssert(x)		__ASSERT_ALWAYS((x),User::Panic(_L("ASSERT"), 0))

char* _itoa(int value, char* string, int radix);

class ansistring {
protected:
	char *_data;
public:
	~ansistring () {
		release ();
	}
	inline void release() {
		if (_data) {
			delete[] _data;
			_data = NULL;
		}
	}

	ansistring () : _data(NULL) { }

	inline int is_null () const {
		return (_data == NULL);
	}

	int length () const {
		FxAssert (_data);
		return strlen (_data);
	}

	inline void set_null () {
		release ();
	}

	ansistring (const char *_astr) : _data(NULL) {
		FxAssert (_astr);
		_dup (_astr);
	}
	ansistring (const ansistring& _astr) : _data(NULL) {
		if (!_astr.is_null ())
			_dup (_astr._data);
		else
			_data = NULL;
	}
	ansistring (int _val) : _data(NULL) {
		char _buf[32];
		//sprintf (_buf, "%d", _val);
		_itoa(_val, _buf, 10);
		_dup (_buf);
	}
	ansistring (float _val) : _data(NULL) {
		char _buf[32];
		sprintf (_buf, "%f", _val);
		_dup (_buf);
	}

	ansistring& operator = (const char *_astr) {
		FxAssert (_astr);
		set_null ();
		_dup (_astr);
		return *this;
	}

	char& operator [] (unsigned int _index) const {
		FxAssert (!is_null() && _index < strlen(_data) && _index > 0);
		return _data[_index];
	}

	int find_char (char _c) {
		FxAssert (!is_null());
		const char *_ptr = _data;
		while (*_ptr && *_ptr != _c)
			++_ptr;
		if (!*_ptr)
			return -1;
		return _ptr - _data;
	}

	void set_string (const char *_astr, int start, int end_no_include) {
		/* end_no_include as -1 indicate copy string to end */
		if (end_no_include == -1)
			end_no_include = strlen (_astr);

		FxAssert (_astr && end_no_include >= start && start >= 0 && start <= (int)strlen(_astr));
		set_null ();
		int _size = end_no_include - start;
		_data = new char [_size + 1];
		FxAssert (_data);
		strncpy (_data, _astr + start, _size);
		_data[_size] = 0;
	}

	void set_string (const char *_start, const char *_end_no_include) {
		FxAssert (_start && _end_no_include && _end_no_include >= _start);
		set_null ();
		int _size = _end_no_include - _start;
		_data = new char [_size + 1];
		FxAssert (_data);
		strncpy (_data, _start, _size);
		_data[_size] = 0;
	}

	ansistring& operator = (const ansistring& _astr) {
		if (this != &_astr) {
			set_null ();
			if (!_astr.is_null())
				_dup (_astr._data);
		}
		return *this;
	}

	int operator == (const ansistring& _astr) {
		if (&_astr == this)
			return 1;
		if (_astr.is_null() && is_null())
			return 1;
		if (_astr.is_null() || is_null())
			return 0;
		return (strcmp (_astr._data, _data) == 0);
	}

	int operator != (const ansistring& _astr) {
		return !(*this == _astr);
	}

	ansistring& operator += (const char* _astr) {
		FxAssert (!is_null() && _astr);
		char* _ptr = new char [strlen (_data) + strlen (_astr) + 1];
		FxAssert (_ptr);
		strcpy (_ptr, _data);
		strcat (_ptr, _astr);
		delete[] _data;
		_data = _ptr;
		return *this;
	}

	ansistring& operator += (const ansistring& _astr) {
		FxAssert (!is_null() && !_astr.is_null());
		char* _ptr = new char [strlen (_data) + strlen (_astr._data) + 1];
		FxAssert (_ptr);
		strcpy (_ptr, _data);
		strcat (_ptr, _astr._data);
		delete[] _data;
		_data = _ptr;
		return *this;
	}

	const char * as_char () const {
		FxAssert (_data);
		return _data;
	}

	operator const char* () const {
		FxAssert (_data);
		return _data;
	}

	int as_int () const {
		FxAssert (_data);
		return atoi (_data);
	}

	float as_float () const {
		FxAssert (_data);
		return atof (_data);
	}

	void as_type (int& _int_type) {
		_int_type = as_int();
	}

	void as_type (float& _float_type) {
		_float_type = as_float ();
	}

	void rtrim () {
		int i;
		FxAssert (_data);
		for (i = strlen (_data) - 1; i >= 0; i--) {
			if (_data[i] != ' ' && _data[i] != '\t' &&
				_data[i] != 0x0d && _data[i] != 0x0a)
				break;
		}
		if (i == -1) {
			*_data = 0;
		} else {
			_data[i + 1] = 0;
		}
	}

	void ltrim () {
		int i;
		FxAssert (_data);
		int _len = strlen(_data);
		for (i = 0; i < _len; i++) {
			if (_data[i] != ' ' && _data[i] != '\t' &&
				_data[i] != 0x0d && _data[i] != 0x0a)
				break;
		}
		if (i == 0)
			return;
		char *_newptr = new char [_len - i + 1];
		FxAssert (_newptr);
		strcpy (_newptr, _data + i);
		delete[] _data;
		_data = _newptr;
	}

	inline void alltrim () {
		rtrim ();
		ltrim ();
	}

protected:
	void _dup (const char *_astr) {	
		FxAssert (_astr);
		_data = new char[strlen (_astr) + 1];
		FxAssert (_data);
		strcpy (_data, _astr);
	}
};

class _oxstream {
public:
	virtual _oxstream& operator << (ansistring& _astr) = 0;
	virtual _oxstream& operator << (const char* _astr) = 0;
	virtual _oxstream& operator << (int _x) = 0;
	virtual _oxstream& operator << (float _x) = 0;
};

class oansistringstream : public _oxstream {
protected:
	ansistring* _body_string;
public:
	oansistringstream (ansistring& _astr) {
		FxAssert (!_astr.is_null());
		_body_string = &_astr;
	}
	oansistringstream& operator << (ansistring& _astr) {
		*_body_string += _astr;
		return *this;
	}
	oansistringstream& operator << (const char* _astr) {
		*_body_string += _astr;
		return *this;
	}
	oansistringstream& operator << (int _x) {
		ansistring _v (_x);
		*_body_string += _v;
		return *this;
	}
	oansistringstream& operator << (float _x) {
		ansistring _v (_x);
		*_body_string += _v;
		return *this;
	}
};

#endif


