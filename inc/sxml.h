#ifndef _SXML_H_
#define _SXML_H_

#include "ansistring.h"
#include "qany.h"
//#include "FondoGlobal.h"

#define endl	"\n"

const char sxml_escape = '^';
const char sxml_lp = '<';
const char sxml_rp = '>';
const char sxml_nl = '\n';
const int sxml_end_reach = -1;

const int sxml_name_length = 512;
const int sxml_value_length = 8*1024;

enum _ele_t {
	ELEMENT = 0,
	ATTR
};

void Sxml_Exception (const char*);

template <class __T>
class vector {
protected:
	__T _array[1024];
	int _count;
public:
	vector() : _count(0) {}
	void push_back (const __T& _v) {
		if(_count>=1022)
			return;
		_array[_count++] = _v;
	}
	int size () const {
		return _count;
	}
	__T& operator[] (int _index) {
		return _array[_index];
	}
};

#define SXML_PROPERTY_COUNT	3
class sxml_property {
public:
	ansistring *_name_array;
	ansistring *_value_array;
	int _count;
	int _max_count;
public:
	sxml_property ();
	~sxml_property ();
	void append (const char* _name, const char *_value);
	void expand_size ();
	int count () {return _count;}
};

class sxml_node {
public:
	sxml_node() : _child(NULL), _sibling(NULL),
		_name(""), _value(""), _ele_type(ELEMENT) {
	}

	typedef vector<sxml_node*>	sxml_node_vector;

	void step_all_nodes (sxml_node_vector& _node_vec);

	void delete_all_nodes ();

	void release_unused ();

	void show_all_nodes (oansistringstream& os, int jump_tab = 0);

	void xml (oansistringstream& __oss, int jump_tab = 0);

	int child_count ();

	int property_count () {
		return _property._count;
	}

	/* return 0 if fail */
	int get_property (const ansistring& _pro_name,
					ansistring& _pro_value);
	int get_property (int _index,
		ansistring& _pro_name, ansistring& _pro_value);
	void add_property (
		const ansistring& _pro_name, 
		const ansistring& _pro_value);
	inline Qany& get_value() {return _value;}

	sxml_node* childs (int index);

	sxml_node* child_find (const char* __child_name);

	sxml_node* add_child (const ansistring& __n, const Qany& __v);
	sxml_node* add_sibling (const ansistring& __n, const Qany& __v);

	sxml_node* find_in_child (ansistring __n);
	sxml_node* find_in_sibling (ansistring __n);

	sxml_node* add_child (const char* __n, const char* __v = "") {
		ansistring n(__n);
		ansistring v(__v);
		return add_child (n, v);
	}
	sxml_node* add_sibling (const char* __n, const char* __v = "") {
		ansistring n(__n);
		ansistring v(__v);
		return add_sibling (n, v);
	}
	sxml_node* find_in_child (const char* __n) {
		return find_in_child (ansistring (__n));
	}
	sxml_node* find_in_sibling (const char* __n) {
		return find_in_sibling (ansistring (__n));
	}
	static bool _take_one (const char* __s, int &__from, ansistring& __desc);

	Qany& operator [] (const char* __path);
	sxml_node* find_path (const char* __path);
	sxml_node* create_path (const char* __path);

public:
	sxml_node* _child;
	sxml_node* _sibling;
	ansistring _name;
	Qany _value;
	sxml_property _property;
	_ele_t _ele_type;
};

oansistringstream& operator << (oansistringstream& _os, sxml_node* _node);

class sxml_parser {
public:
	const char* _source;
	int _source_len;
	int _cur_line;
	int _cur_col;
	int _cur_off;
	bool _end_of_file;
	sxml_node* _root_node;
public:
	void __init () {
		_cur_line = 0;
		_cur_col = 0;
		_cur_off = 0;
		_end_of_file = true;
		_root_node = NULL;
	}
	sxml_parser () {
		__init ();
	}
	~sxml_parser () {
		reset ();
	}
	inline sxml_node* root () {
		return _root_node;
	}
	inline int line () {
		return _cur_line;
	}
	inline int col () {
		return _cur_col;
	}
	void delete_root () {
		if (_root_node == NULL)
			return;
		_root_node->delete_all_nodes ();
	}
	inline void reset () {
		delete_root ();
		__init();
	}
	inline void show_nodes (oansistringstream& _os) {
		_root_node->show_all_nodes (_os);
	}
	inline bool _is_in_bound (int __of) {
		return (__of < _source_len);
	}
	inline char look_ahead (int i = 0) {
		return (_cur_off + i >= _source_len) ? 0 : _source[_cur_off + i];
	}
	int found_match_char (char __v, int& __l, int& __c);
	bool skip_to_char (char __v);

	static void deal_property (sxml_node* _p_node, 
					const char * _proper_string);

	static void ltrim (const char * __s, const char* &__v);

	inline static bool is_seem_as_space (char __c) {
		return (__c == ' ' || __c < 0x1A);
	}

	static void rtrim (char * __s);

	static void all_trim (char* __s);
	bool skip_space ();

	bool copy_to_char (char __v, char *__p);

	char move_next ();

	/* parser entry */
	void parse_string (const char *__s);
	bool parse_file (const char * __fname);

	/* real parser, and _p_node as parser_root */
	void parse_node (sxml_node* _p_node, int __lev = 0);
};

class sxml {
protected:
	sxml_parser _parser;
public:
	sxml () { __init (); }
	~sxml () { _parser.reset(); }
	void __init () { }
	inline sxml_node* root() { 
		return _parser._root_node;
	};
	inline sxml_parser& parser() { 
		return _parser;
	}
	inline void reset () {
		_parser.reset ();
		__init ();
	}

	inline void load_string (const ansistring& __str) {
		_parser.parse_string (__str.as_char());
	}

	inline void load_string (const char*__str) {
		_parser.parse_string (__str);
	}

	inline void dump_string (ansistring& __str) {
		oansistringstream __oss(__str);
		dump_stream (__oss);
	}
	
	inline bool load_file (const char* __file) {
		return _parser.parse_file (__file);
	}

	void dump_stream (oansistringstream& __os);

	inline void dump_tree (oansistringstream& __os) {
		if (_parser._root_node)
			_parser._root_node->show_all_nodes (__os);
	}

	Qany& operator[] (const char * __path);

	sxml_node* find_path (const char* __path);
	sxml_node* create_path (const char* __path);

	sxml_node* add (const char* __path, const Qany& __v);
	bool get_value (const char* __path, Qany& __v);
};

oansistringstream& operator << (oansistringstream& __oss, sxml& __sxml);

#endif

