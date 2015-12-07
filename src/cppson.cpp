#include "src/cppson.h"

namespace cppson
{

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

}