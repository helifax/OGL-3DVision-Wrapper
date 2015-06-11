#ifndef __DATA__BASE__
#define __DATA__BASE__

#include <vector>

// Used for storing parameters
typedef void* S32;

// Prototype of the saved function
typedef void(*functionCallBack)(S32 a, S32 b, S32 c, S32 d, S32 e, S32 f, S32 g, S32 h, S32 i, S32 k, S32 l);

// Will contain the value and valueType
typedef struct
{
	std::vector<S32> value;
} DATA_VALUE_T;

// Will hold the stack of the function calls
typedef struct
{
	std::vector<functionCallBack> functionAddess;
	DATA_VALUE_T a;
	DATA_VALUE_T b;
	DATA_VALUE_T c;
	DATA_VALUE_T d;
	DATA_VALUE_T e;
	DATA_VALUE_T f;
	DATA_VALUE_T g;
	DATA_VALUE_T h;
	DATA_VALUE_T i;
	DATA_VALUE_T k;
	DATA_VALUE_T l;
}DB_FUNCTION_CALLBACK_T;

enum
{
	a,
	b,
	c,
	d,
	e,
	f,
	g,
	h,
	i,
	k,
	l,
};

///my database
DB_FUNCTION_CALLBACK_T * g_database;

#endif