#pragma once
#include <string>
#include <map>
#include <sstream>
#include <fstream>
#include <functional>
#include <vector>
#include "json.h"
#ifndef OUT
#define OUT
#endif
#define JSON_CLASS(name) class name final : public cppson::Parsable<name>
#define FIELD(type, name) \
struct Init_ ## name \
{\
	Init_ ## name ## () \
	{ \
		meta[ #name ] = [](Type* t, Json::Value value) -> bool \
		{ \
			cppson::JsonValue& v = (cppson::JsonValue)value;\
			return v.parse(t->name); \
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

	template<typename T>
	bool parse(T& val)
	{
		static_assert(std::is_base_of<Parsable<T>, T>::value, "T must be derived from Parsable<T>");

		if (!isObject())
		{
			return false;
		}

		auto members = getMemberNames();

		for (auto m : members)
		{
			if (!T::getMeta().at(m)(&val, get(m, Json::nullValue)))
			{
				return false;
			}
		}

		return true;
	}

	template<typename T>
	bool parse(std::vector<T>& val)
	{
		static_assert(std::is_base_of<Parsable<T>, T>::value ||
					std::is_same<int, T>::value ||
					std::is_same<double, T>::value ||
					std::is_same<std::string, T>::value ||
					std::is_same<bool, T>::value, "T must be derived from Parsable<T>, or T must be int or double or std::string or bool.");

		if (!isArray())
			return false;

		for (auto v : *this)
		{
			T p;
			JsonValue& jsonVal = (JsonValue)v;

			if (jsonVal.parse(p))
				val.push_back(p);
			else
				return false;
		}

		return true;
	}

	template<>
	bool parse<int>(int& val)
	{
		if (!isInt())
			return false;

		val = asInt();

		return true;
	}

	template<>
	bool parse<double>(double& val)
	{
		if (!isDouble())
			return false;

		val = asDouble();

		return true;
	}

	template<>
	bool parse<float>(float& val)
	{
		if (!isDouble())
			return false;

		val = asFloat();

		return true;
	}

	template<>
	bool parse<bool>(bool& val)
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
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	template<>
	bool parse<std::string>(std::string& val)
	{
		if (!isString())
			return false;

		val = asString();

		return true;
	}
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

	using Meta = std::map<std::string, std::function<bool(Type*, Json::Value)> >;

	static const Meta& getMeta()
	{
		return meta;
	}

protected:

	static Meta meta;
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
typename Parsable<T>::Meta Parsable<T>::meta;

}