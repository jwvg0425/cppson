#pragma once
#include "parsable.h"
#include "jsonField.h"

#define FIELD(T, N) \
struct key_ ## N : cppson::JsonKey \
{\
	virtual const char* key()\
	{\
		return #N ;\
	}\
};\
cppson::JsonField<T, key_ ## N> N