#pragma once
#include <string>
#include <map>
#include <sstream>
#include <fstream>
#include <functional>
#include <vector>
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
			return value.parse(t->name); \
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

class Single
{
public:
	template<typename R>
	R getValue()
	{
		static_assert(false, "invalid call.");
	}

	template<>
	int getValue<int>()
	{
		return atoi(str.c_str());
	}

	template<>
	bool getValue<bool>()
	{
		if (str == "true")
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	template<>
	double getValue<double>()
	{
		return atof(str.c_str());
	}

	template<>
	std::string getValue<std::string>()
	{
		return str;
	}

	std::string& getStr()
	{
		return str;
	}

private:
	std::string str;
};

class JsonValue
{
public:
	
	//if failed, return -1
	int init(std::vector<std::string>& tokens, int offset)
	{
		//object
		if (tokens[offset] == "{")
		{
			type = OBJECT;
			offset++;
			
			while (offset < tokens.size())
			{
				if (tokens[offset] == "\"")
				{
					std::string& key = tokens[offset + 1];

					if (tokens[offset + 2] != "\"" || tokens[offset + 3] != ":")
						return -1;

					offset = obj[key].init(tokens, offset + 4);

					if (offset == -1)
						return -1;
				}
				else if (tokens[offset] == ",")
				{
					offset++;
				}
				else if (tokens[offset] == "}")
				{
					return offset + 1;
				}
				else
				{
					return -1;
				}
			}
			return -1;
		}
		else if (tokens[offset] == "[")
		{
			type = ARRAY;
			offset++;
			while (offset < tokens.size())
			{
				if (tokens[offset] == ",")
				{
					offset++;
				}
				else if (tokens[offset] == "]")
				{
					return offset + 1;
				}
				else
				{
					JsonValue value;
					offset = value.init(tokens, offset);
					if (offset == -1)
						return -1;

					arr.push_back(value);
				}
			}
			return -1;
		}
		else
		{
			type = SINGLE;
			if (tokens[offset] == "\"")
			{
				single.getStr() = tokens[offset + 1];
				return offset + 3;
			}
			else
			{
				single.getStr() = tokens[offset];
				if (tokens[offset] == "null")
				{
					type = NONE;
				}
				return offset + 1;
			}
		}
	}

	template<typename T>
	bool parse(T& val)
	{
		static_assert(std::is_base_of<Parsable<T>, T>::value, "T must be derived from Parsable<T>");

		if (type != OBJECT)
			return false;

		for (auto& m : obj)
		{
			if (!T::getReadMeta().at(m.first)(&val, m.second))
			{
				return false;
			}
		}
		return true;
	}

	template<template<typename> typename T, typename U>
	bool parse(T<U>& val)
	{
		static_assert(std::is_base_of<JsonField<U>, T<U>>::value, "T<U> must be derived from JsonField<U>");
		
		if (type != NONE)
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
	
		if (type != ARRAY)
			return false;

		for (auto& v : arr)
		{
			T p;

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

		if (type != ARRAY)
			return false;

		for (auto v : arr)
		{
			T p;

			if (v.parse(p))
				val.push_back(std::move(p));
			else
				return false;
		}

		return true;
	}

	template<>
	bool parse<int>(int& val)
	{
		if (type != SINGLE)
			return false;

		val = single.getValue<int>();

		return true;
	}

	template<>
	bool parse<double>(double& val)
	{
		if (type != SINGLE)
			return false;

		val = single.getValue<double>();

		return true;
	}

	template<>
	bool parse<float>(float& val)
	{
		if (type != SINGLE)
			return false;

		val = single.getValue<double>();

		return true;
	}

	template<>
	bool parse<bool>(bool& val)
	{
		if (type != SINGLE)
			return false;

		val = single.getValue<bool>();

		return true;
	}

	template<>
	bool parse<std::string>(std::string& val)
	{
		if (type != SINGLE)
			return false;

		val = single.getValue<std::string>();

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

bool tokenize(const std::string& str, OUT std::vector<std::string>& tokens)
{
	std::string token;
	std::string space = " \t\r\n";
	std::string delim = ":{}[],";
	std::string number = "0123456789.";
	int state = 0;

	for (size_t i = 0; i < str.size(); i++)
	{
		switch (state)
		{
		case 0:
			//ignore space
			if (space.find(str[i]) != std::string::npos)
			{
				if (token.size() != 0)
				{
					tokens.push_back(token);
					token.clear();
				}

				continue;
			}
			//go string
			else if (str[i] == '"')
			{
				if (token.size() != 0)
					return false;

				tokens.push_back({ str[i] });
				state = 1;
			}
			//one char token
			else if (delim.find(str[i]) != std::string::npos)
			{
				if (token.size() != 0)
				{
					tokens.push_back(token);
					token.clear();
				}
				tokens.push_back({ str[i] });
			}
			//number
			else if (number.find(str[i]) != std::string::npos)
			{
				token.push_back({ str[i] });
			}
			//true or false
			else if (str.substr(i, 4) == "true")
			{
				if (token.size() != 0)
					return false;

				tokens.push_back(str.substr(i, 4));
				i += 3;
			}
			else if (str.substr(i, 5) == "false")
			{
				if (token.size() != 0)
					return false;

				tokens.push_back(str.substr(i, 5));
				i += 4;
			}
			else if (str.substr(i, 4) == "null")
			{
				if (token.size() != 0)
					return false;

				tokens.push_back(str.substr(i, 4));
				i += 3;
			}
			else
			{
				return false;
			}
			break;
		case 1:
			if (str[i] == '"')
			{
				tokens.push_back(token);
				tokens.push_back({ str[i] });
				token.clear();
				state = 0;
			}
			else
			{
				token.push_back({ str[i] });
			}
			break;
		}
	}

	return true;
}

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
	std::vector<std::string> tokens;

	if (!tokenize(str, tokens))
	{
		return false;
	}

	JsonValue json;

	if (json.init(tokens, 0) == -1)
		return false;

	json.parse(value);
	return true;
}

template<typename T>
typename Parsable<T>::WriteMeta Parsable<T>::writeMeta;

template<typename T>
typename Parsable<T>::ReadMeta Parsable<T>::readMeta;


template<typename T>
std::string toJson(T& value)
{
	static_assert(std::is_base_of<Parsable<T>, T>::value, "T must be derived from Parsable<T>");

	std::string res = "{";

	auto& meta = T::getWriteMeta();

	if (meta.size() == 0)
	{
		return res + "}";
	}

	res += (meta[0])(&value);

	for (int i = 1; i < meta.size(); i++)
	{
		res += "," + (meta[1])(&value);
	}

	return res + "}";
}

template<template<typename> typename T, typename U>
std::string toJson(T<U>& val)
{
	static_assert(std::is_base_of<JsonField<U>, T<U>>::value, "T<U> must be derived from JsonField<U>");

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

template<>
std::string toJson<int>(int& val)
{
	return std::to_string(val);
}

template<>
std::string toJson<double>(double& val)
{
	return std::to_string(val);
}

template<>
std::string toJson<float>(float& val)
{
	return std::to_string(val);
}

template<>
std::string toJson<bool>(bool& val)
{
	return val ? "true" : "false";
}

template<>
std::string toJson<std::string>(std::string& val)
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
}

}