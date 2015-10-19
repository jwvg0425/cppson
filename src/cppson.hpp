#pragma once
#include <string>
#include <map>
#include <sstream>
#include <functional>
#include <vector>
#define JSON_CLASS(name) class name : public cppson::Parsable<name>
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

class Single
{
public:
	template<typename R>
	R GetValue()
	{
		static_assert(false, "invalid call.");
	}

	template<>
	int GetValue<int>()
	{
		return atoi(str.c_str());
	}

	template<>
	bool GetValue<bool>()
	{
		if (str == "true" || str == "TRUE")
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	template<>
	double GetValue<double>()
	{
		return atof(str.c_str());
	}

	template<>
	std::string GetValue<std::string>()
	{
		return str;
	}

private:
	std::string str;
};

class JsonValue
{
public:
	JsonValue(std::string str)
	{
	}

	template<typename T>
	bool parse(T& val)
	{
		static_assert(std::is_base_of<Parsable<T>, T>::value, "T must be derived from Parsable<T>");

		if (type != OBJECT)
			return false;

		for (auto m : obj)
		{
			if (!T::GetMeta()[m.first](&val, m.second))
			{
				return false;
			}
		}

		return true;
	}

	template<>
	bool parse<int>(int& val)
	{
		if (type != SINGLE)
			return false;

		val = single.GetValue<int>();

		return true;
	}

	template<>
	bool parse<double>(double& val)
	{
		if (type != SINGLE)
			return false;

		val = single.GetValue<double>();

		return true;
	}

	template<>
	bool parse<bool>(bool& val)
	{
		if (type != SINGLE)
			return false;

		val = single.GetValue<bool>();

		return true;
	}

	template<>
	bool parse<std::string>(std::string& val)
	{
		if (type != SINGLE)
			return false;

		val = single.GetValue<std::string>();

		return true;
	}

private:
	enum Type
	{
		NONE,
		SINGLE,
		OBJECT,
		ARRAY
	};

	Type type;
	Single single;
	std::map<std::string, JsonValue> obj;
	std::vector<JsonValue> arr;
};


template<typename T>
class Parsable
{
public:
	using Type = T;

	bool ParseFromFile(const std::string& fileName)
	{
		return false;
	}

	bool ParseFromString(const std::string& str)
	{
		JsonValue value(str);

		return false;
	}
	using Meta = std::map<std::string, std::function<bool(Type*, JsonValue)> >;

	static const Meta& GetMeta()
	{
		return meta;
	}

protected:

	static Meta meta;
};

template<typename T>
typename Parsable<T>::Meta Parsable<T>::meta;

}