#include <iostream>
#include "src/cppson.hpp"

JSON_CLASS(Test)
{
public:
	FIELD(std::vector<int>, a);
	FIELD(int, b);
	FIELD(double, c);
};

int main()
{
	Test t;

	if (t.ParseFromFile("test.json"))
	{
		printf("%d %d %lf\n", t.a.size(), t.b, t.c);
	}
	else
	{
		printf("parse failed.\n");
	}

	return 0;
}