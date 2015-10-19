#pragma once
#include <string>
#include <map>
#include <functional>
#include "json.h"

namespace cppson
{

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
		return false;
	}

protected:

	static std::map<std::string, std::function<void(Type*, JsonValue)> > meta;
};

template<typename T>
std::map<std::string, std::function<void(typename T::Type*, JsonValue)>> Parsable<T>::meta;

}