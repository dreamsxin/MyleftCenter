#include "loop_share.h"
#include "symbian_def.h"
#include "portab_symbian.h"
#include "CenterCore.h"
#include <MyleftCenter.rsg>
/* 
 * return _size mean read ok, 
 * otherwise the value of the real cap. can read out
 * but still not read out NOW! 
*/
int loop_share_buffer::alloc () {
	_buffer = new unsigned char [_fix_cap];
	if (_buffer == NULL) {
		_fix_cap = 50 * 1024;
		_write_cap = _fix_cap;
		_buffer = new unsigned char [_fix_cap];
		if (_buffer == NULL) {

			CenterCore::ShowMessage (
				CenterCore::GetInstance()->TextResource (R_TEXT_NO_MEMORY));
			CenterCore::GetInstance()->m_appUI->HandleCommandL (EAknCmdExit);
	
			return 0;
		}
		return 1;
	}
	return 1;
}

int loop_share_buffer::Read (void* _data, int _size) {
	/* if no buffer alloc, alloc it now */
	if (_buffer == NULL) {
		alloc ();
	}

	if (_size == 0)
		return 0;
	if (_size > _read_cap) {
		return _read_cap;
	}
	if (_read_loc + _size <= _fix_cap) {
		memcpy (_data, _buffer + _read_loc, _size);
	} else {
		int _part_size = _fix_cap - _read_loc;
		memcpy (_data, _buffer + _read_loc, _part_size);
		memcpy ((char*)_data + _part_size, _buffer, _size - _part_size);
	}
	_read_loc += _size;
	if (_read_loc >= _fix_cap) 
		_read_loc -= _fix_cap;
	_read_cap -= _size;
	_write_cap += _size;

	return _size;
}

/* 
 * return _size mean read ok, 
 * otherwise the value of the real cap. can read out
 * skip the data
*/
int loop_share_buffer::SkipRead (int _size) {
	/* if no buffer alloc, alloc it now */
	if (_buffer == NULL) {
		alloc ();
	}

	if (_size == 0)
		return 0;
	if (_size > _read_cap) {
		return _read_cap;
	}
	_read_loc += _size;
	if (_read_loc >= _fix_cap)
		_read_loc -= _fix_cap;
	_read_cap -= _size;
	_write_cap += _size;

	return _size;
}

/*
 * Peek is like Read, but not change cache pointer
*/
int loop_share_buffer::Peek (void* _data, int _size) {
	/* if no buffer alloc, alloc it now */
	if (_buffer == NULL) {
		alloc ();
	}

	if (_size == 0)
		return 0;
	if (_size > _read_cap) {
		return _read_cap;
	}
	if (_read_loc + _size <= _fix_cap) {
		memcpy (_data, _buffer + _read_loc, _size);
	} else {
		int _part_size = _fix_cap - _read_loc;
		memcpy (_data, _buffer + _read_loc, _part_size);
		memcpy ((char*)_data + _part_size, _buffer, _size - _part_size);
	}
	return _size;
}

/*
 * return _size mean ok
 * otherwise the value of the loop_share could be written in 
 * but still not write out NOW! 
*/
int loop_share_buffer::Write (const void* _data, int _size) {
	if (_size == 0)
		return 0;

	/* if no buffer alloc, alloc it now */
	if (_buffer == NULL) {
		alloc ();
	}

	if (_size > _write_cap) {
		return _write_cap;
	}
	if (_write_loc + _size <= _fix_cap) {
		memcpy (_buffer + _write_loc, _data, _size);
	} else {
		int _part_size = _fix_cap - _write_loc;
		memcpy (_buffer + _write_loc, _data, _part_size);
		memcpy (_buffer, (char*)_data + _part_size, _size - _part_size);
	}
	_write_loc += _size;
	if (_write_loc >= _fix_cap)
		_write_loc -= _fix_cap;
	_write_cap -= _size;
	_read_cap += _size;

	return _size;
}

/*
 * make cache empty
*/
void loop_share_buffer::Clear () {
	release ();
	_write_cap = _fix_cap;
	_read_cap = 0;
	_read_loc = 0;
	_write_loc = 0;
	_previous_write_size = 0;
}

int loop_share_buffer::UndoWrite() 
{
	if( _previous_write_size )
	{
		if( _previous_write_size>_read_cap )
			return 0;

		_read_cap -= _previous_write_size;
		_write_cap += _previous_write_size;
		_write_loc -= _previous_write_size;
		if( _write_loc<0 )
			_write_loc +=_fix_cap;

		_previous_write_size = 0;
		return 1;
	}

	return 0;
}
