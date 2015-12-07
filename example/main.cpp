#include <iostream>
#include "src/cppson.h"

JSON_CLASS(Test)
{
public:
	FIELD(std::vector<int>, a);
	FIELD(int, b);
};

JSON_CLASS(Test2)
{
public:
	FIELD(std::vector<Test>, tests);
	FIELD(Test, test2);
};

int main()
{
	Test2 t;

	if (t.loadFile("test.json"))
	{
		printf("parse succed.\n");

		printf("%d", t.test2->b);
	}
	else
	{
		printf("parse failed.\n");
	}

	printf("%s\n", cppson::toJson(t).c_str());

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