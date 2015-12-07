#pragma once
#include <string>
#include <map>
#include <sstream>
#include <fstream>
#include <functional>
#include <vector>
#include "json.h"
#include <list>
#ifndef OUT
#define OUT
#endif
#define JSON_CLASS(name) class name final : public cppson::Parsable<name>

#define FIELD(type, name) \
struct Init_ ## name \
{\
	Init_ ## name ## () \
	{ \
		readMeta[ #name ] = [](Type* t, cppson::JsonValue& value) -> bool \
		{ \
			cppson::JsonValue& v = (cppson::JsonValue)value;\
			return v.parse(t->name); \
		}; \
		writeMeta.push_back([](Type* t) -> std::string \
		{ \
			return std::string("\"") + std::string( #name ) + std::string("\"") + std::string(":") + cppson::toJson(t->name);\
		}); \
	} \
};\
template<typename T>\
class Field_ ## name : public cppson::JsonField<T>\
{\
public:\
	Field_ ## name ## ()\
	{\
		static Init_ ## name init;\
	}\
};\
Field_ ## name ## <type> name;

#define FIELD_WITH_KEY(type, key, name) \
struct Init_ ## name \
{\
	Init_ ## name ## () \
	{ \
		readMeta[ #key ] = [](Type* t, cppson::JsonValue& value) -> bool \
		{ \
			return value.parse(t->name); \
		}; \
		writeMeta.push_back([](Type* t) -> std::string \
		{ \
			return std::string("\"") + std::string( #key ) + std::string("\"") + std::string(":") + cppson::toJson(t->name);\
		});\
	} \
}; \
template<typename T>\
class Field_ ## name : public cppson::JsonField<T>\
{\
public:\
	Field_ ## name ## ()\
	{\
		static Init_ ## name init;\
	}\
};\
Field_ ## name ## <type> name;


namespace cppson
{


class JsonValue : public Json::Value
{
public:

	JsonValue() = default;
	JsonValue(const Json::Value& value) : Json::Value(value) {}
	
	//if failed, return false
	bool init(const std::string& str)
	{
		Json::Reader reader;
		return reader.parse(str.c_str(), *this);
	}

	template<typename T,
		     typename = std::enable_if_t<std::is_enum<T>::value> >
	bool parse(T& val)
	{
		if (!isIntegral())
		{
			return false;
		}

		val = static_cast<T>(asInt());

		return true;
	}

	template<typename T,
		     typename = std::enable_if_t<std::is_base_of<Parsable<T>, T>::value>,
		     typename = void>
	bool parse(T& val)
	{
		if (!isObject())
		{
			return false;
		}

		auto members = getMemberNames();

		for (auto m : members)
		{
			if (!T::getReadMeta().at(m)(&val, JsonValue(get(m, Json::nullValue))))
			{
				return false;
			}
		}
		return true;

	}

	template<template<typename> typename T, typename U,
		typename = std::enable_if_t<std::is_base_of<JsonField<U>, T<U>>::value >>
	bool parse(T<U>& val)
	{
		if (!isNull())
		{
			val.null = false;
			return parse(val.get());
		}
		return true;
	}

	template<typename T>
	bool parse(std::list<T>& val)
	{
		static_assert(std::is_base_of<Parsable<T>, T>::value ||
			std::is_same<int, T>::value ||
			std::is_same<float, T>::value ||
			std::is_same<double, T>::value ||
			std::is_same<std::string, T>::value ||
			std::is_same<bool, T>::value, "T must be derived from Parsable<T>, or T must be int or float or double or std::string or bool.");
	
		if (!isArray())
			return false;

		for (auto& v : arr)
		{
			T p;
			JsonValue& jsonVal = (JsonValue)v;

			if (v.parse(p))
				val.push_back(std::move(p));
			else
				return false;
		}

		return true;
	}

	template<typename T>
	bool parse(std::vector<T>& val)
	{
		static_assert(std::is_base_of<Parsable<T>, T>::value ||
					std::is_same<int, T>::value ||
					std::is_same<float, T>::value ||
					std::is_same<double, T>::value ||
					std::is_same<std::string, T>::value ||
					std::is_same<bool, T>::value, "T must be derived from Parsable<T>, or T must be int or float or double or std::string or bool.");

		if (!isArray())
			return false;

		for (auto v : *this)
		{
			T p;
			JsonValue& jsonVal = (JsonValue)v;

			if (jsonVal.parse(p))
				val.push_back(std::move(p));
			else
				return false;
		}

		return true;
	}

	bool parse(int& val)
	{
		if (!isInt())
			return false;

		val = asInt();

		return true;
	}

	bool parse(double& val)
	{
		if (!isDouble())
			return false;

		val = asDouble();

		return true;
	}

	bool parse(float& val)
	{
		if (!isDouble())
			return false;

		val = asFloat();

		return true;
	}

	bool parse(bool& val)
	{
		if (isBool())
		{
			val = asBool();
			return true;
		}
		else if (isString())
		{
			std::string str = asString();
			if (str == "true")
			{
				val = true;
				return true;
			}
			else if (str == "false")
			{
				val = false;
				return true;
			}
		}
		else
		{
			return false;
		}

		return false;
	}

	bool parse(std::string& val)
	{
		if (!isString())
			return false;

		val = asString();

		return true;
	}
};

template<typename T>
class JsonField
{
public:
	operator T()
	{
		return value;
	}

	T& get()
	{
		return value;
	}

	T& operator*()
	{
		return value;
	}

	T* operator->()
	{
		return &value;
	}

	T& operator=(T& rhs)
	{
		null = false;
		value = rhs;
	}

	bool operator==(T& rhs)
	{
		return value == rhs;
	}

	bool isNull()
	{
		return null;
	}

protected:
	T value;
	bool null = true;

	friend class JsonValue;
};

template<typename T>
class Parsable
{
public:
	using Type = T;

	bool loadFile(const std::string& fileName)
	{
		Type& t = static_cast<Type&>(*this);
		return cppson::loadFile(t, fileName);
	}

	bool toJson(const std::string& fileName)
	{
		Type& t = static_cast<Type&>(*this);
		return cppson::toJson(t, fileName);
	}

	using ReadMeta = std::map<std::string, std::function<bool(Type*, JsonValue&)> >;
	using WriteMeta = std::vector<std::function<std::string(Type*)> >;

	static const ReadMeta& getReadMeta()
	{
		return readMeta;
	}

	static const WriteMeta& getWriteMeta()
	{
		return writeMeta;
	}

protected:
	static WriteMeta writeMeta;
	static ReadMeta readMeta;
};


template<typename T>
bool loadFile(T& value, const std::string& fileName)
{
	std::ifstream file(fileName);

	if (!file.is_open())
		return false;

	std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	
	return loadString(value, str);
}

template<typename T>
bool loadString(T& value, const std::string& str)
{
	JsonValue json;

	if (!json.init(str))
		return false;

	json.parse(value);
	return true;
}

template<typename T>
typename Parsable<T>::WriteMeta Parsable<T>::writeMeta;

template<typename T>
typename Parsable<T>::ReadMeta Parsable<T>::readMeta;


template<typename T,
		 typename = std::enable_if_t<std::is_enum<T>::value> >
std::string toJson(T& value)
{
	int v = static_cast<int>(value);
	return toJson(v);
}

template<typename T,
		 typename = std::enable_if_t<std::is_base_of<Parsable<T>, T>::value>,
		 typename = void>
std::string toJson(T& value)
{
	std::string res = "{";

	auto& meta = T::getWriteMeta();

	if (meta.size() == 0)
	{
		return res + "}";
	}

	res += (meta[0])(&value);

	for (int i = 1; i < meta.size(); i++)
	{
		res += "," + (meta[i])(&value);
	}

	return res + "}";
}

template<template<typename> typename T, typename U,
		 typename = std::enable_if_t<std::is_base_of<JsonField<U>, T<U>>::value>>
std::string toJson(T<U>& val)
{
	if (val.isNull())
		return "null";

	return toJson(val.get());
}

template<typename T>
std::string toJson(std::list<T>& val)
{
	static_assert(std::is_base_of<Parsable<T>, T>::value ||
		std::is_same<int, T>::value ||
		std::is_same<float, T>::value ||
		std::is_same<double, T>::value ||
		std::is_same<std::string, T>::value ||
		std::is_same<bool, T>::value, "T must be derived from Parsable<T>, or T must be int or float or double or std::string or bool.");

	if (val.size() == 0)
		return "[]";

	std::string res = "[" + toJson(*val.begin());

	for (auto& it = val.begin() + 1; it != val.end(); ++it)
	{
		res += "," + toJson(*it);
	}

	return res + "]";
}

template<typename T>
std::string toJson(std::vector<T>& val)
{
	static_assert(std::is_base_of<Parsable<T>, T>::value ||
		std::is_same<int, T>::value ||
		std::is_same<float, T>::value ||
		std::is_same<double, T>::value ||
		std::is_same<std::string, T>::value ||
		std::is_same<bool, T>::value, "T must be derived from Parsable<T>, or T must be int or float or double or std::string or bool.");

	if (val.size() == 0)
		return "[]";

	std::string res = "[" + toJson(val[0]);

	for (int i = 1; i < val.size(); i++)
	{
		res += "," + toJson(val[i]);
	}

	return res + "]";
}

std::string toJson(int& val)
{
	return std::to_string(val);
}

std::string toJson(float& val)
{
	return std::to_string(val);
}

std::string toJson(double& val)
{
	return std::to_string(val);
}

std::string toJson(bool& val)
{
	return val ? "true" : "false";
}

std::string toJson(std::string& val)
{
	return "\"" + val + "\"";
}

template<typename T>
bool toJson(T& value, const std::string& fileName)
{
	std::ofstream file(fileName);

	if (!file.is_open())
		return false;

	file << toJson(value);

	return true;
}

}