#pragma once
#include <string>

namespace cppson
{

class JsonKey
{
public:
	JsonKey() = default;
	virtual ~JsonKey() = default;
protected:
	virtual const char* key() = 0;
};

template<typename R, typename K>
class JsonField : K
{
public:

	operator R()
	{
		return value;
	}

private:
	R value;
};

}