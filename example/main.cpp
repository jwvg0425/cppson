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

	std::vector<int> test2;

	if (cppson::loadFile(test2, "test2.json"))
	{
		for (auto t : test2)
		{
			printf("%d ", t);
		}
	}

	return 0;
}