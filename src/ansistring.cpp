#include "ansistring.h"

ansistring operator + (ansistring& _first, ansistring& _second) {
	FxAssert (!_first.is_null() && !_second.is_null());
	const char *_first_ptr = _first.as_char();
	const char *_second_ptr = _second.as_char();

	char* _ptr = new char [strlen (_first_ptr) + strlen (_second_ptr) + 1];
	FxAssert (_ptr);
	strcpy (_ptr, _first_ptr);
	strcat (_ptr, _second_ptr);
	ansistring _val(_ptr);
	delete[] _ptr;
	return _val;
}

ansistring operator + (ansistring& _first, const char *_second) {
	FxAssert (!_first.is_null() && _second);
	const char *_first_ptr = _first.as_char();

	char* _ptr = new char [strlen (_first_ptr) + strlen (_second) + 1];
	strcpy (_ptr, _first_ptr);
	strcat (_ptr, _second);
	ansistring _val(_ptr);
	delete[] _ptr;
	return _val;
}

char* _itoa(int value, char* string, int radix)
{
	char tmp[33];
	char* tp = tmp;
	int i;
	unsigned v;
	int sign;
	char* sp;

	if (radix > 36 || radix <= 1)
	{
		FxAssert(0);
		return 0;
	}

	sign = (radix == 10 && value < 0);
	if (sign)
		v = -value;
	else
		v = (unsigned)value;
	while (v || tp == tmp)
	{
		i = v % radix;
		v = v / radix;
		if (i < 10)
			*tp++ = i+'0';
		else
			*tp++ = i + 'a' - 10;
	}

	if (string == 0)
		string = (char*)malloc((tp-tmp)+sign+1);
	sp = string;

	if (sign)
		*sp++ = '-';
	while (tp > tmp)
		*sp++ = *--tp;
	*sp = 0;
	return string;
}
