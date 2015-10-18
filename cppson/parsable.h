#pragma once
#include <string>

namespace cppson
{

class Parsable
{
public:
	bool ParseFromFile(const std::string& fileName);
	bool ParseFromString(const std::string& str);

private:
};

}