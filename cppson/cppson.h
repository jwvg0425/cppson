#pragma once
#include "parsable.h"
#define JSON_CLASS(name) class name : public cppson::Parsable<name>
#define FIELD(type, name) \
struct Init_ ## name \
{\
	Init_ ## name ## () \
	{ \
		meta[ #name ] = [](Type* t, cppson::JsonValue value) \
		{ \
			value.parse(t->name); \
		}; \
	} \
};\
struct Meta_ ## name\
{\
	Meta_ ## name ## ()\
	{\
		static Init_ ## name init;\
	}\
};\
Meta_ ## name meta_ ## name;\
type name;