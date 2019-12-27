#include "test/yats.h"
#include "yasl.h"
#include "IO.h"
#include "yasl_state.h"

SETUP_YATS();

#define STR(x) #x

#define ASSERT_VALUE_ERR(code, expected) {\
	struct YASL_State *S = YASL_newstate_bb(code, strlen(code));\
	S->vm.err.print = io_print_string;\
	S->vm.out.print = io_print_string;\
	ASSERT_SUCCESS(YASL_compile(S));\
	int result = YASL_execute(S);\
	ASSERT_EQ(result, 7);\
	const char *exp_err = "ValueError: " expected ".\n";\
	ASSERT_EQ(S->vm.out.len, 0);\
	ASSERT_EQ(strlen(exp_err), S->vm.err.len);\
	ASSERT_EQ(memcmp(exp_err, S->vm.err.string, S->vm.err.len), 0);\
	YASL_delstate(S);\
}

#define ASSERT_DIV_BY_ZERO_ERR(code) {\
	struct YASL_State *S = YASL_newstate_bb(code, strlen(code));\
	S->vm.err.print = io_print_string;\
	S->vm.out.print = io_print_string;\
	ASSERT_SUCCESS(YASL_compile(S));\
	int result = YASL_execute(S);\
	ASSERT_EQ(result, 6);\
	const char *exp_err = "DivisionByZeroError\n";\
	ASSERT_EQ(S->vm.out.len, 0);\
	ASSERT_EQ(strlen(exp_err), S->vm.err.len);\
	ASSERT_EQ(memcmp(exp_err, S->vm.err.string, S->vm.err.len), 0);\
	YASL_delstate(S);\
}

#define ASSERT_TYPE_ERR(code, expected) {\
	struct YASL_State *S = YASL_newstate_bb(code, strlen(code));\
	S->vm.err.print = io_print_string;\
	S->vm.out.print = io_print_string;\
	ASSERT_SUCCESS(YASL_compile(S));\
	int result = YASL_execute(S);\
	ASSERT_EQ(result, 5);\
	const char *exp_err = "TypeError: " expected ".\n";\
	ASSERT_EQ(S->vm.out.len, 0);\
	ASSERT_EQ(strlen(exp_err), S->vm.err.len);\
	ASSERT_EQ(memcmp(exp_err, S->vm.err.string, S->vm.err.len), 0);\
	YASL_delstate(S);\
}

#define ASSERT_UNOP_TYPE_ERR(code, op, expr) ASSERT_TYPE_ERR(code, op " not supported for operand of type " expr)
#define ASSERT_BINOP_TYPE_ERR(code, op, left, right) \
	ASSERT_TYPE_ERR(code, op " not supported for operands of types " left " and " right)
#define ASSERT_ARG_TYPE_ERR(code, method, exp, actual, arg) \
	ASSERT_TYPE_ERR(code, method " expected arg in position " STR(arg) \
	" to be of type " exp ", got arg of type " actual)

////////////////////////////////////////////////////////////////////////////////

int vmtest(void) {
	// binary operator type errors
	ASSERT_BINOP_TYPE_ERR(".true | false;", "|", "str", "bool");
	ASSERT_BINOP_TYPE_ERR(".true ^ false;", "^", "str", "bool");
	ASSERT_BINOP_TYPE_ERR(".true & false;", "&", "str", "bool");
	ASSERT_BINOP_TYPE_ERR(".true &^ false;", "&^", "str", "bool");
	ASSERT_BINOP_TYPE_ERR(".true << false;", "<<", "str", "bool");
	ASSERT_BINOP_TYPE_ERR(".true >> false;", ">>", "str", "bool");
	ASSERT_BINOP_TYPE_ERR(".true + false;", "+", "str", "bool");
	ASSERT_BINOP_TYPE_ERR(".true - false;", "-", "str", "bool");
	ASSERT_BINOP_TYPE_ERR(".true * false;", "*", "str", "bool");
	ASSERT_BINOP_TYPE_ERR(".true / false;", "/", "str", "bool");
	ASSERT_BINOP_TYPE_ERR(".true // false;", "//", "str", "bool");
	ASSERT_BINOP_TYPE_ERR(".true % false;", "%", "str", "bool");
	ASSERT_BINOP_TYPE_ERR(".true ** false;", "**", "str", "bool");

	// unary operator type errors
	ASSERT_UNOP_TYPE_ERR("+true;", "+", "bool");
	ASSERT_UNOP_TYPE_ERR("-true;", "-", "bool");
	ASSERT_UNOP_TYPE_ERR("^true;", "^", "bool");
	ASSERT_UNOP_TYPE_ERR("len true;", "len", "bool");

	// bool method type errors
	ASSERT_ARG_TYPE_ERR("true.tostr(1);", "bool.tostr", "bool", "int", 0);
	ASSERT_ARG_TYPE_ERR("true.tobool(1);", "bool.tobool", "bool", "int", 0);

	// float method type errors
	ASSERT_ARG_TYPE_ERR("0.0.tostr(1);", "float.tostr", "float", "int", 0);
	ASSERT_ARG_TYPE_ERR("0.0.toint(1);", "float.toint", "float", "int", 0);
	ASSERT_ARG_TYPE_ERR("0.0.tofloat(1);", "float.tofloat", "float", "int", 0);
	ASSERT_ARG_TYPE_ERR("0.0.tobool(1);", "float.tobool", "float", "int", 0);

	// int method type errors
	ASSERT_ARG_TYPE_ERR("0.tostr(1.0);", "int.tostr", "int", "float", 0);
	ASSERT_ARG_TYPE_ERR("0.toint(1.0);", "int.toint", "int", "float", 0);
	ASSERT_ARG_TYPE_ERR("0.tofloat(1.0);", "int.tofloat", "int", "float", 0);
	ASSERT_ARG_TYPE_ERR("0.tobool(1.0);", "int.tobool", "int", "float", 0);

	// list method type errors
	ASSERT_ARG_TYPE_ERR("[].push(1, true);", "list.push", "list", "int", 0);
	ASSERT_ARG_TYPE_ERR("[].copy(true);", "list.copy", "list", "bool", 0);
	ASSERT_ARG_TYPE_ERR("[].__add(true, []);", "list.__add", "list", "bool", 0);
	ASSERT_ARG_TYPE_ERR("[].__add([], true);", "list.__add", "list", "bool", 1);
	ASSERT_ARG_TYPE_ERR("[].__add(true, 1);", "list.__add", "list", "int", 1);
	ASSERT_ARG_TYPE_ERR("[] + true;", "list.__add", "list", "bool", 1);
	// ASSERT_ARG_TYPE_ERR("true + [];", "list.__add", "list", "bool", 0);
	ASSERT_ARG_TYPE_ERR("[].extend(1, []);", "list.extend", "list", "int", 0);
	ASSERT_ARG_TYPE_ERR("[].extend([], 1);", "list.extend", "list", "int", 1);
	ASSERT_ARG_TYPE_ERR("[].extend(1, true);", "list.extend", "list", "bool", 1);
	// TODO: __get, __set
	ASSERT_ARG_TYPE_ERR("[].tostr(1);", "list.tostr", "list", "int", 0);
	ASSERT_ARG_TYPE_ERR("[].search(1);", "list.search", "list", "int", 0);
	ASSERT_ARG_TYPE_ERR("[].reverse(1);", "list.reverse", "list", "int", 0);
	ASSERT_ARG_TYPE_ERR("[].clear(1);", "list.clear", "list", "int", 0);
	ASSERT_ARG_TYPE_ERR("[].join(1, .str);", "list.join", "list", "int", 0);
	ASSERT_ARG_TYPE_ERR("[].join(1, true);", "list.join", "str", "bool", 1);
	ASSERT_ARG_TYPE_ERR("[].join([], true);", "list.join", "str", "bool", 1);
	ASSERT_ARG_TYPE_ERR("[].sort(1);", "list.sort", "list", "int", 0);

	// str method type errors
	ASSERT_ARG_TYPE_ERR("''.tofloat(1);", "str.tofloat", "str", "int", 0);
	ASSERT_ARG_TYPE_ERR("''.toint(1);", "str.toint", "str", "int", 0);
	ASSERT_ARG_TYPE_ERR("''.isalnum(1);", "str.isalnum", "str", "int", 0);
	ASSERT_ARG_TYPE_ERR("''.isal(1);", "str.isal", "str", "int", 0);
	ASSERT_ARG_TYPE_ERR("''.isnum(1);", "str.isnum", "str", "int", 0);
	ASSERT_ARG_TYPE_ERR("''.isspace(1);", "str.isspace", "str", "int", 0);
	ASSERT_ARG_TYPE_ERR("''.tobool(1);", "str.tobool", "str", "int", 0);
	ASSERT_ARG_TYPE_ERR("''.tostr(1);", "str.tostr", "str", "int", 0);
	ASSERT_ARG_TYPE_ERR("''.toupper(1);", "str.toupper", "str", "int", 0);
	ASSERT_ARG_TYPE_ERR("''.tolower(1);", "str.tolower", "str", "int", 0);
	ASSERT_ARG_TYPE_ERR("''.startswith(1, true);", "str.startswith", "str", "bool", 1);
	ASSERT_ARG_TYPE_ERR("''.startswith(1, .true);", "str.startswith", "str", "int", 0);
	ASSERT_ARG_TYPE_ERR("''.startswith(.str, true);", "str.startswith", "str", "bool", 1);
	ASSERT_ARG_TYPE_ERR("''->startswith(1);", "str.startswith", "str", "int", 1);
	ASSERT_ARG_TYPE_ERR("''.endswith(1, true);", "str.endswith", "str", "bool", 1);
	ASSERT_ARG_TYPE_ERR("''.endswith(1, .true);", "str.endswith", "str", "int", 0);
	ASSERT_ARG_TYPE_ERR("''.endswith(.str, true);", "str.endswith", "str", "bool", 1);
	ASSERT_ARG_TYPE_ERR("''->endswith(1);", "str.endswith", "str", "int", 1);
	ASSERT_ARG_TYPE_ERR("''.replace(1, true, 1.0);", "str.replace", "str", "float", 2);
	ASSERT_ARG_TYPE_ERR("''.replace(1, true, .str);", "str.replace", "str", "bool", 1);
	ASSERT_ARG_TYPE_ERR("''.replace(1, .true, .str);", "str.replace", "str", "int", 0);
	ASSERT_ARG_TYPE_ERR("''.replace(1, .true, 1.0);", "str.replace", "str", "float", 2);
	ASSERT_ARG_TYPE_ERR("''.replace(.tr, true, .str);", "str.replace", "str", "bool", 1);
	ASSERT_ARG_TYPE_ERR("''.replace(.tr, .true, 1);", "str.replace", "str", "int", 2);
	ASSERT_ARG_TYPE_ERR("''.replace(.tr, true, 1);", "str.replace", "str", "int", 2);
	ASSERT_ARG_TYPE_ERR("''.search(1, true);", "str.search", "str", "bool", 1);
	ASSERT_ARG_TYPE_ERR("''.search(1, .true);", "str.search", "str", "int", 0);
	ASSERT_ARG_TYPE_ERR("''.search(.str, true);", "str.search", "str", "bool", 1);
	ASSERT_ARG_TYPE_ERR("''->search(1);", "str.search", "str", "int", 1);
	ASSERT_ARG_TYPE_ERR("''.count(1, true);", "str.count", "str", "bool", 1);
	ASSERT_ARG_TYPE_ERR("''.count(1, .true);", "str.count", "str", "int", 0);
	ASSERT_ARG_TYPE_ERR("''.count(.str, true);", "str.count", "str", "bool", 1);
	ASSERT_ARG_TYPE_ERR("''->count(1);", "str.count", "str", "int", 1);
	ASSERT_ARG_TYPE_ERR("''.split(1, true);", "str.split", "str", "bool", 1);
	ASSERT_ARG_TYPE_ERR("''.split(1, .true);", "str.split", "str", "int", 0);
	ASSERT_ARG_TYPE_ERR("''.split(.str, true);", "str.split", "str", "bool", 1);
	ASSERT_ARG_TYPE_ERR("''->split(1);", "str.split", "str", "int", 1);
	ASSERT_ARG_TYPE_ERR("''.ltrim(1, true);", "str.ltrim", "str", "bool", 1);
	ASSERT_ARG_TYPE_ERR("''.ltrim(1, .true);", "str.ltrim", "str", "int", 0);
	ASSERT_ARG_TYPE_ERR("''.ltrim(.str, true);", "str.ltrim", "str", "bool", 1);
	ASSERT_ARG_TYPE_ERR("''->ltrim(1);", "str.ltrim", "str", "int", 1);
	ASSERT_ARG_TYPE_ERR("''.rtrim(1, true);", "str.rtrim", "str", "bool", 1);
	ASSERT_ARG_TYPE_ERR("''.rtrim(1, .true);", "str.rtrim", "str", "int", 0);
	ASSERT_ARG_TYPE_ERR("''.rtrim(.str, true);", "str.rtrim", "str", "bool", 1);
	ASSERT_ARG_TYPE_ERR("''->rtrim(1);", "str.rtrim", "str", "int", 1);
	ASSERT_ARG_TYPE_ERR("''.trim(1, true);", "str.trim", "str", "bool", 1);
	ASSERT_ARG_TYPE_ERR("''.trim(1, .true);", "str.trim", "str", "int", 0);
	ASSERT_ARG_TYPE_ERR("''.trim(.str, true);", "str.trim", "str", "bool", 1);
	ASSERT_ARG_TYPE_ERR("''->trim(1);", "str.trim", "str", "int", 1);
	ASSERT_ARG_TYPE_ERR("''.rep(1, true);", "str.rep", "int", "bool", 1);
	ASSERT_ARG_TYPE_ERR("''.rep(1, 1);", "str.rep", "str", "int", 0);
	ASSERT_ARG_TYPE_ERR("''.rep(.str, true);", "str.rep", "int", "bool", 1);
	ASSERT_ARG_TYPE_ERR("''->rep(true);", "str.rep", "int", "bool", 1);

	// table method type errors
	ASSERT_ARG_TYPE_ERR("{}.remove(1, 2.0);", "table.remove", "table", "int", 0);
	ASSERT_ARG_TYPE_ERR("{}.keys(1);", "table.keys", "table", "int", 0);
	ASSERT_ARG_TYPE_ERR("{}.values(1);", "table.values", "table", "int", 0);
	ASSERT_ARG_TYPE_ERR("{}.copy(1);", "table.copy", "table", "int", 0);
	ASSERT_ARG_TYPE_ERR("{}.tostr(1);", "table.tostr", "table", "int", 0);
	// TODO: __get, __set
	ASSERT_ARG_TYPE_ERR("{}.clear(1);", "table.clear", "table", "int", 0);

	// undef method type errors
	ASSERT_ARG_TYPE_ERR("undef.tostr(1);", "undef.tostr", "undef", "int", 0);

	// value errors
	ASSERT_VALUE_ERR("echo []->pop();", "list.pop expected nonempty list as arg 0");
	ASSERT_VALUE_ERR("echo [1, .a]->sort();", "list.sort expected a list of all numbers or all strings");
	ASSERT_VALUE_ERR("echo ''.replace(.tr, '', .sad);", "str.replace expected a nonempty str as arg 1");
	ASSERT_VALUE_ERR("echo 'wasd'->split('');", "str.split expected a nonempty str as arg 1");
	ASSERT_VALUE_ERR("echo 'as'->rep(-1);", "str.rep expected non-negative int as arg 1");

	// division by zero errors
	ASSERT_DIV_BY_ZERO_ERR("echo 1 // 0;");
	ASSERT_DIV_BY_ZERO_ERR("echo 1 % 0;");

	// math type errors
	// ASSERT_ARG_TYPE_ERR("math.max(1, 2, .a);", "math.max", "float", "str", 2);
	// ASSERT_ARG_TYPE_ERR("math.min(1, 2, .a);", "math.max", "float", "str", 2);

	return __YASL_TESTS_FAILED__;
}