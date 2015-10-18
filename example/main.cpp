#include <iostream>
#include "cppson.h"

class Test
{
private:
	FIELD(int, abc);
};

int main()
{
	Test a;
	return 0;
}