#include "sxml.h"

oansistringstream& operator << (oansistringstream& _os, sxml_node* _node) {
	_node->xml (_os, 0);
	return _os;
}

oansistringstream& operator << (oansistringstream& __oss, sxml& __sxml) {
	__sxml.dump_stream (__oss);
	return __oss;
}

sxml_property::sxml_property() : 
		_count(0), _max_count(SXML_PROPERTY_COUNT) {
	_name_array = new ansistring[_max_count];
	_value_array = new ansistring[_max_count];
}

sxml_property::~sxml_property() {
	delete [] _name_array;
	delete [] _value_array;
}

void sxml_property::append (const char *_name, const char *_value) {
	if (_count >= _max_count) {
		expand_size();
	}
	_name_array[_count] = _name;
	_value_array[_count] = _value;
	++_count;
	return;
}

void sxml_property::expand_size () {
	_max_count += SXML_PROPERTY_COUNT;
	ansistring* _name_array_new = new ansistring [_max_count];
	ansistring* _value_array_new = new ansistring [_max_count];
	for (int i = 0; i < _count; i++) {
		_name_array_new[i] = _name_array[i];
		_value_array_new[i] = _value_array[i];
	}
	delete[] _name_array;
	delete[] _value_array;
	_name_array = _name_array_new;
	_value_array = _value_array_new;
}

void sxml_node::step_all_nodes (sxml_node_vector& _node_vec) {
	if (_child)
		_child->step_all_nodes (_node_vec);
	if (_sibling)
		_sibling->step_all_nodes (_node_vec);
	
	/*sxml_node *sibling = _sibling;
	
	while(sibling)
	{
		if(sibling->_child)
			sibling->_child->step_all_nodes(_node_vec);
		_node_vec.push_back(sibling);
		sibling = sibling->_sibling;
	}  */
		
	_node_vec.push_back (this);
}

void sxml_node::delete_all_nodes () {
	sxml_node_vector* __node_vec = new sxml_node_vector;
	step_all_nodes (*__node_vec);
	int __size = __node_vec->size();
	for (int i = 0; i < __size; i ++) {
		delete (sxml_node*)(*__node_vec)[i];
	}
	delete __node_vec;
}

void sxml_node::release_unused () {
	ansistring nil_s("");

	if (_sibling && (_sibling->_name.is_null() ||
		_sibling->_name == nil_s)) {
		delete _sibling;
		_sibling = NULL;
	}
	if (_child)
		_child->release_unused ();
	if (_sibling)
		_sibling->release_unused ();
}

void sxml_node::show_all_nodes (oansistringstream& os, int jump_tab) {
	ansistring nil_s("");

	for (int i = 0; i < jump_tab; i++)
		os << "     ";
	if (_value.as_string() == nil_s)
		os << "{" << _name.as_char() << "}" << endl;
	else 
		os << "{" << _name.as_char() << "} = {" << 
				_value.as_string().as_char() << "}" << endl;
	if (_child)
		_child->show_all_nodes (os, jump_tab + 1);
	if (_sibling)
		_sibling->show_all_nodes (os, jump_tab);
}

void sxml_node::xml (oansistringstream& __oss, int jump_tab) {
	ansistring nil_s("");

	for (int i = 0; i < jump_tab; i++)
		__oss << "    ";
	__oss << "<" << _name.as_char();
	for (int i = 0; i < _property._count; i++) {
		__oss << " " << _property._name_array[i].as_char() <<
			"=\"" << _property._value_array[i].as_char() << "\"";
	}
	__oss << ">";

	if (_value.as_string() != nil_s) {
		__oss << "" << _value.as_string().as_char() << "";
	}

	if (_child) {
		__oss << endl;
		_child->xml (__oss, jump_tab + 1);
		for (int i = 0; i < jump_tab; i++)
			__oss << "    ";
	}

	__oss << "</" << _name.as_char() << ">" << endl;
	if (_sibling)
		_sibling->xml (__oss, jump_tab);

}

int sxml_node::child_count () {
	if (!_child)
		return 0;
	int __count = 1;
	sxml_node* _trace = _child;
	while (_trace->_sibling) {
		__count ++;
		_trace = _trace->_sibling;
	}
	return __count;
}

sxml_node* sxml_node::childs (int index) {
	if (index >= child_count())
		return NULL;
	if (index == 0) 
		return _child;
	sxml_node* _trace = _child;
	for (int i = 0; i < index; i++) {
		_trace = _trace->_sibling;
	}
	return _trace;
}

sxml_node* sxml_node::child_find (const char* __child_name) {
	ansistring child_name(__child_name);

	if (!_child)
		return NULL;
	sxml_node* _trace = _child;
	while (_trace->_name != child_name) {
		if (_trace->_sibling)
			_trace = _trace->_sibling;
		else
			return NULL;
	}
	return _trace;
}

sxml_node* sxml_node::add_child (const ansistring& __n, const Qany& __v) {
	if (!_child) {
		_child = new sxml_node;
		_child->_name = __n;
		_child->_value = __v;
		return _child;
	} else {
		//sxml_node *_trace = _child;
		return _child->add_sibling (__n, __v);
	}
}

sxml_node* sxml_node::add_sibling (const ansistring& __n, const Qany& __v) {
	sxml_node *_trace = this;
	while (_trace->_sibling != NULL)
		_trace = _trace->_sibling;
	_trace->_sibling = new sxml_node;
	_trace->_sibling->_name = __n;
	_trace->_sibling->_value = __v;
	return _trace->_sibling;
}

sxml_node* sxml_node::find_in_child (ansistring __n) {
	if (!_child)
		return NULL;
	sxml_node *trace = _child;
	return trace->find_in_sibling (__n);
}

sxml_node* sxml_node::find_in_sibling (ansistring __n) {
	sxml_node *_trace = this;
	while (_trace) {
		if (_trace->_name == __n)
			return _trace;
		_trace = _trace->_sibling;
	}
	return NULL;
}

bool sxml_node::_take_one (const char* __s, 
				int &__from, ansistring& __desc) {
	int i;
	char _buf[2048];
	for (i = __from; __s[i] != '/' && __s[i] != 0; i++);
	if (__s[i] == 0) {
		/* to end */
		__desc = &__s[__from];
		return false;
	}
	memset (_buf, 0, 2048);
	strncpy (_buf, __s + __from, i - __from);
	__desc = _buf;
	__from = i + 1;
	return true;
}

Qany& sxml_node::operator [] (const char* __path) {
	sxml_node * __node;
	__node = find_path (__path);
	if (__node == NULL) {
		Sxml_Exception ("operator [] no found");
	}
	return __node->_value;
}

sxml_node* sxml_node::find_path (const char* __path) {
	bool _ret;
	int _itr = 0;
	ansistring _name;
	sxml_node *_trace;

	if (__path == NULL || __path[0] == 0)
		return NULL;
	if (__path[0] == '/')
		_itr = 1;
	_ret = _take_one (__path, _itr, _name);
	_trace = find_in_sibling (_name);
	if (_trace == NULL)
		return NULL;
	if (_ret == false)
		return _trace;
	while (1) {
		_ret = _take_one (__path, _itr, _name);
		_trace = _trace->find_in_child (_name);
		if (_trace == NULL) 
			return NULL;
		if (_ret == false) 
			return _trace;
	}
}

int sxml_node::get_property (const ansistring& _pro_name, 
				ansistring& _pro_value) {
	for (int i = 0; i < _property._count; i++) {
		if (!strcmp (_pro_name.as_char(), 
					_property._name_array[i].as_char())) {
			_pro_value = _property._value_array[i];
			return 1;
		}
	}
	return 0;
}

int sxml_node::get_property (
			int _index, 
			ansistring& _pro_name, ansistring& _pro_value) {
	if (_index >= _property._count)
		return 0;
	_pro_name = _property._name_array[_index];
	_pro_value = _property._value_array[_index];
	return 1;
}

void sxml_node::add_property (
		const ansistring& _pro_name, const ansistring& _pro_value) {
	_property.append (_pro_name.as_char(), _pro_value.as_char());
}

sxml_node* sxml_node::create_path (const char* __path) {
	bool _ret;
	int _itr = 0;
	ansistring _name;
	sxml_node * _trace;

	if (__path == NULL || __path[0] == 0)
		return NULL;
	if (__path[0] == '/')
		_itr = 1;
	_ret = _take_one (__path, _itr, _name);
	_trace = find_in_sibling (_name);
	if (_trace == NULL) 
		_trace = add_sibling (_name.as_char(), "");
	if (_ret == false)
		return _trace;
	while (1) {
		_ret = _take_one (__path, _itr, _name);
		sxml_node* _tmp_trace = _trace->find_in_child (_name);
		if (_tmp_trace == NULL) 
			_trace = _trace->add_child (_name.as_char(), "");
		else
			_trace = _tmp_trace;
		if (_ret == false)
			return _trace;
	}
}

int sxml_parser::found_match_char (char __v, int& __l, int& __c) {
	for (int i = _cur_off; i < _source_len; i++) {
		if (_source[i] == __v) 
			return i;
		else if (_source[i] == sxml_escape) {
			i ++;
		} else if (_source[i] == sxml_nl) {
			__l ++;
			__c = 0;
		} 
	}
	return sxml_end_reach;
}

bool sxml_parser::skip_to_char (char __v) {
	int __x = found_match_char (__v,_cur_line, _cur_col);
	if (__x == sxml_end_reach)
		return false;
	_cur_off = __x;
	return true;
}

void sxml_parser::deal_property (sxml_node* _p_node, 
				const char * _proper_string) {
	char __temp_name[128];
	char __temp_value[512];
	_p_node->_name = _proper_string;

	/* this function deal like <name pro1=val pro2="val2"> */
	const char* __itr = _proper_string;

	do {
		++__itr;
	} while (*__itr && *__itr != ' ' && *__itr != '\n' && *__itr != '\t');
	if (! *__itr) {
		/* only have a name */
		_p_node->_name = _proper_string;
		return;
	}
	memset (__temp_name, 0, 128);
	memcpy (__temp_name, _proper_string, __itr - _proper_string);
	_p_node->_name = __temp_name;

	do {
		++__itr;
	} while (*__itr == ' ' || *__itr == '\n' || *__itr == '\t');

	const char *__head;
	while (1) {
		memset (__temp_name, 0, 128);
		memset (__temp_value, 0, 512);
		/* head point to the prop-name start */
		__head = __itr;

		/* find the = */
		do {
			++__itr;
		} while (*__itr != '=' && *__itr != 0);
		if (!*__itr) {
			Sxml_Exception ("Parser: property lost =");
		}
		memcpy (__temp_name, __head, __itr - __head);
		all_trim (__temp_name);

		do {
			++__itr;
		} while (*__itr == ' ' || *__itr == '\t' || *__itr == '\n');
		if (!*__itr) {
			Sxml_Exception (
				"Parser: property lost value after =");
		}

		if (*__itr == '\"') {
			__itr++;
			__head = __itr;
		
			while (*__itr != '\"' && *__itr != 0) {
				++__itr;
			}
			if (!*__itr) {
				Sxml_Exception ("Parser: property lost right \"");
			}
			if (__itr - __head != 0) {
				memcpy (__temp_value, __head, __itr - __head);
			} else {
				__temp_value[0] = '\0';
			}
		} else {
			__head = __itr;
			do {
				++__itr;
			} while (*__itr && *__itr != ' ' && *__itr != '\t');
			memcpy (__temp_value, __head, __itr - __head);
		}

		_p_node->_property.append (__temp_name, __temp_value);

		if (!*__itr)
			return;

		do {
			++__itr;
		} while (*__itr && 
				(*__itr == ' ' || *__itr == '\t' || *__itr == '\n'));
		if (!*__itr) {
			/* mean to the end of properties */
			return;
		}
	}
}

void sxml_parser::ltrim (const char * __s, const char* &__v) {
	while (*__s == ' ' || *__s == '\t' || *__s == 0x0a || *__s == 0x0d)
		++__s;
	__v = __s;
}

void sxml_parser::rtrim (char * __s) {
	if (*__s == 0)
		return;
	int __off = strlen (__s) - 1;
	while (__off >= 0) {
		if (!is_seem_as_space (__s[__off]))
			break;
		__s[__off --] = 0;
	}
}

void sxml_parser::all_trim (char* __s) {
	char * __buf;
	const char * __pt;
	rtrim (__s);
	ltrim (__s, __pt);
	__buf = new char[strlen (__pt) + 1];
	strcpy (__buf, __pt);
	strcpy (__s, __buf);
	delete[] __buf;
}

bool sxml_parser::skip_space () {
	for (; _cur_off < _source_len; _cur_off++) {
		if (_source[_cur_off] != ' ' && _source[_cur_off] > 0x1A)
			break;
	}
	return _is_in_bound (_cur_off);
}

bool sxml_parser::copy_to_char (char __v, char *__p) {
	int __x = found_match_char (__v, _cur_line, _cur_col);
	if (__x == sxml_end_reach) 
		return false;
	memcpy (__p, &_source[_cur_off], __x - _cur_off);
	__p[__x - _cur_off] = 0;
	_cur_off = __x;
	return true;
}

char sxml_parser::move_next () {
	char __c = _source[_cur_off];
	switch (__c) {
	case sxml_nl:
		_cur_line ++, _cur_col = 0;
		break;
	case sxml_escape:
		__c = _source[++_cur_off];
		break;
	default:
		_cur_col ++;
	}
	_end_of_file = (_cur_off >= _source_len);
	_cur_off ++;

	return __c;
}

/* parser entry */

void sxml_parser::parse_string (const char *__s) {
	_source = __s;
	_source_len = strlen (_source);

	/* first one under root_node is is SXMLVERSION */
	/* if not use SXMLVERSION, use 
		_root_node = new sxml_node;
		parse_node (_root_node);
	   else
		_root_node->_sibling = new sxml_node;
		parse_node (_root_node->_sibling);
	*/
	_root_node = new sxml_node;
	FxAssert (_root_node);
	parse_node (_root_node);
	_root_node->release_unused ();
}

bool sxml_parser::parse_file (const char * __fname) {
	FILE *fp;
	char *__buf;
	fp = fopen (__fname, "r+b");
	if (!fp)
		return false;
	fseek (fp, 0, SEEK_END);
	int __size = ftell (fp);
	if (__size == 0 || __size > 100 * 1024 * 1024) {
		fclose (fp);
		return false;
	}
	fseek (fp, 0, SEEK_SET);
	__buf = new char[__size + 1];
	if (__buf == NULL) {
		fclose (fp);
		return false;
	}
	memset (__buf, 0, __size + 1);
	fread (__buf, __size, 1, fp);
	fclose (fp);

	parse_string (__buf);
	delete[] __buf;
	return true;
}

void sxml_parser::parse_node (sxml_node* _p_node, int __lev) {
	char __name[sxml_name_length + 1];
//	char __value[sxml_value_length + 1];
	char* __value = new char[sxml_value_length + 1];
	bool __suc;
	while (1) {
		if (skip_to_char (sxml_lp) == false) {
			delete []__value;
			return;
		}
		if (look_ahead (1) == '?' || look_ahead (1) == '-' || look_ahead(1) == '!') {
			if (skip_to_char (sxml_rp) == false) {
				delete []__value;
				Sxml_Exception ("Parser: Comment not found end");
			}
			continue;
		}
		else if (look_ahead (1) == '/') {
			delete []__value;
			return;
		}
		move_next ();
		__suc = copy_to_char (sxml_rp, __name);
		if (!__suc) {
			delete []__value;
			Sxml_Exception ("Parser: cannot found > ");
		}
		all_trim (__name);

		if (__name[strlen(__name) - 1] == '/') {
			__name[strlen(__name) - 1] = 0;
			__value[0] = 0;
			all_trim (__name);
			deal_property (_p_node, __name);
			_p_node->_value = __value;
			_p_node->_sibling = new sxml_node;
			FxAssert (_p_node->_sibling);
			_p_node = _p_node->_sibling;
			continue;
		}

		move_next ();
		__suc = copy_to_char (sxml_lp, __value);
		if (!__suc) {
			delete []__value;
			Sxml_Exception ("Parser: found no </>");
		}
		all_trim (__value);

		deal_property (_p_node, __name);
		_p_node->_value = __value;

		if (look_ahead (1) == '/') {
			char __end_name[sxml_name_length];
			move_next ();
			move_next ();
			copy_to_char (sxml_rp, __end_name);
			all_trim (__end_name);
			if (strcmp (
				_p_node->_name.as_char(), __end_name) != 0) {
				delete []__value;
				Sxml_Exception ("Parser: <> not match </>");
			}
			move_next ();

			_p_node->_sibling = new sxml_node;
			FxAssert (_p_node->_sibling);
			_p_node = _p_node->_sibling;
		} else {
			_p_node->_child = new sxml_node;
			FxAssert (_p_node->_child);

			parse_node (_p_node->_child, __lev + 1);

			if (look_ahead(1) != '/') {
				delete []__value;
				Sxml_Exception ("Parser: lost </>");
			}
			move_next ();
			move_next ();
			char __end_name[sxml_name_length];
			copy_to_char (sxml_rp, __end_name);
			all_trim (__end_name);
			if (strcmp (
				_p_node->_name.as_char(), __end_name) != 0) {
				delete []__value;
				Sxml_Exception ("Parser: <> not match </>");
			}
			_p_node->_sibling = new sxml_node;
			FxAssert (_p_node->_sibling);
			_p_node = _p_node->_sibling;
		}
	}
	delete []__value;
}

void sxml::dump_stream (oansistringstream& __os) {
	if (_parser._root_node) {
		__os << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>" << endl;
		_parser._root_node->xml (__os);
	}
}

Qany& sxml::operator[] (const char * __path) {
	if (_parser._root_node)
		return (*(_parser._root_node))[__path];
	Sxml_Exception ("sxml: root node is NULL");
	return (*(_parser._root_node))[__path];	// no use, for ignore compiler
}

sxml_node* sxml::find_path (const char* __path) {
	if (_parser._root_node)
		return _parser._root_node->find_path (__path);
	return NULL;
}

sxml_node* sxml::create_path (const char* __path) {
	if (_parser._root_node)
		return _parser._root_node->create_path (__path);
	else {
		sxml_node* _tmp_node = new sxml_node;
		sxml_node* _new_one =_tmp_node->create_path (__path);
		_parser._root_node = _tmp_node->_sibling;
		delete _tmp_node;
		return _new_one;
	}
}

sxml_node* sxml::add (const char* __path, const Qany& __v) {
	sxml_node *_trace = create_path (__path);
	_trace->_value = __v;
	return _trace;
}

bool sxml::get_value (const char* __path, Qany& __v) {
	sxml_node* _trace;
	_trace = find_path (__path);
	if (_trace == NULL)
		return false;
	__v = _trace->_value;
	return true;
};


