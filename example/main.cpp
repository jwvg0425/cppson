#include <iostream>
#include "cppson.h"

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