#ifndef _LOOP_SHARE_H_
#define _LOOP_SHARE_H_

#include <e32std.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

class loop_share_buffer {
protected:
	unsigned char *_buffer;
	int _write_loc;				/* next write start point */
	int _read_loc;				/* next read start point */
	int _read_cap;				/* indicate the bytes could be read out */
	int _write_cap;				/* indicate the bytes could be wrote in */
	int _fix_cap;				/* equal to buffer max. size */

	int _previous_write_size;
public:
	loop_share_buffer (int _bsize) : _buffer(NULL),
		_write_loc(0), _read_loc(0),
		_read_cap(0), _write_cap (_bsize), _fix_cap (_bsize),
		_previous_write_size(0) {
	}

	int alloc ();

	~loop_share_buffer () {
		release ();
	}

	inline void release () {
		if (_buffer) {
			delete[] _buffer;
			_buffer = NULL;
		}
	}

	inline int can_read_size () {
		return _read_cap;
	}

	inline int can_write_size () {
		return _write_cap;
	}

	inline int max_size () {
		return _fix_cap;
	}

	/* 
	 * clear all data
	*/
	void Clear ();
	/* 
	 * return _size mean read ok, 
	 * otherwise the value of the real cap. can read out 
	 * but still not read out NOW! 
	*/
	int Read (void* _data, int _size);
	int Peek (void* _data, int _size);
	int SkipRead (int _size);
	/*
	 * return _size mean ok
	 * otherwise the value of the loop_share could be written in 
	 * but still not write out NOW! 
	*/
	int Write (const void* _data, int _size);

	/*
		Undo previous writing operation
	*/
	int UndoWrite();
};

#endif

