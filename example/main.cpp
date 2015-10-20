#include <iostream>
#include "src/cppson.hpp"

JSON_CLASS(Test)
{
private:
	FIELD(std::vector<int>, a);
	FIELD(int, b);
};

JSON_CLASS(Test2)
{
private:
	FIELD(std::vector<Test>, tests);
	FIELD(Test, test2);
};

int main()
{
	Test2 t;

	if (t.loadFile("test.json"))
	{
		printf("parse succed.\n");
	}
	else
	{
		printf("parse failed.\n");
	}

	return 0;
}