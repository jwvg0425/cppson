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
		meta[ #name ] = [](Type* t, cppson::JsonValue value) -> bool \
		{ \
			return value.parse(t->name); \
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
class JsonValue
{
public:

	JsonValue() = default;
	JsonValue(const Json::Value& value) : json(value) {}
	
	//if failed, return false
	bool init(const std::string& str)
	{
		Json::Reader reader;
		return reader.parse(str.c_str(), json);
	}

	template<typename T>
	bool parse(T& val)
	{
		static_assert(std::is_base_of<Parsable<T>, T>::value, "T must be derived from Parsable<T>");

		if (!json.isObject())
		{
			return false;
		}

		auto members = json.getMemberNames();

		for (auto m : members)
		{
			if (!T::getMeta().at(m)(&val, json.get(m, Json::nullValue)))
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

		if (!json.isArray())
			return false;

		for (auto v : json)
		{
			T p;
			JsonValue jsonVal(v);

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
		if (!json.isInt())
			return false;

		val = json.asInt();

		return true;
	}

	template<>
	bool parse<double>(double& val)
	{
		if (!json.isDouble())
			return false;

		val = json.asDouble();

		return true;
	}

	template<>
	bool parse<float>(float& val)
	{
		if (!json.isDouble())
			return false;

		val = json.asFloat();

		return true;
	}

	template<>
	bool parse<bool>(bool& val)
	{
		if (json.isBool())
		{
			val = json.asBool();
			return true;
		}
		else if (json.isString())
		{
			std::string str = json.asString();
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
		if (!json.isString())
			return false;

		val = json.asString();

		return true;
	}

private:
	Json::Value json;
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

	using Meta = std::map<std::string, std::function<bool(Type*, JsonValue)> >;

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