#include <iostream>
#include "src/cppson.hpp"

JSON_CLASS(Test)
{
public:
	FIELD(int, a);
	FIELD(int, b);
	FIELD(double, c);
};

int main()
{
	return 0;
}