#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "src/cppson.h"

enum class TestEnum
{
	ONE = 1,
	TWO = 2,
	THREE = 3,
	FOUR = 4
};

JSON_CLASS(NestedObject)
{
public:
	FIELD(int, _int);
	FIELD(bool, _bool);
};
JSON_CLASS(TestObject)
{
public:
	FIELD(int, _int);
	FIELD(bool, _bool);
	FIELD(float, _float);
	FIELD(double, _double);
	FIELD(std::string, _string);

	FIELD(std::string, _null);

	FIELD(TestEnum, _enum);

	FIELD(std::vector<int>, _vector);

	FIELD(NestedObject, _nested);
};

TEST_CASE("parse from string / save file / load file") 
{
	TestObject saveObj;

	cppson::loadString(saveObj,
		"{"
		/* for 'basic types' section */
		"\"_int\" : 5,"
		"\"_bool\" : \"true\","
		"\"_float\" : 2.13,"
		"\"_double\" : 2.5,"
		"\"_string\" : \"hello\","

		/* for 'null' section */
		"\"_null\" : null,"

		/* for 'enum' section */
		"\"_enum\" : 3,"

		/* for 'std::vector type' section */
		"\"_vector\" : [1, 2, 3 ,4 ,5],"

		/* for 'nested object' section */
		"\"_nested\" : {"
		"\"_int\" : 14,"
		"\"_bool\" : false"
		"}"
		"}");

	saveObj.toJson("test.json");

	TestObject obj;
	obj.loadFile("test.json");

	SECTION("basic types") 
	{
		REQUIRE(obj._int == 5);
		REQUIRE(obj._bool == true);
		REQUIRE(obj._float == 2.13f);
		REQUIRE(obj._double == 2.5);
		REQUIRE(obj._string.get() == "hello");
	}
	SECTION("null") 
	{
		REQUIRE(obj._null.isNull());
	}
	SECTION("enum")
	{
		REQUIRE(obj._enum == TestEnum::THREE);
	}
	SECTION("std::vector type") 
	{
		REQUIRE(obj._vector->size() == 5);
		REQUIRE(obj._vector == (std::vector<int>{1, 2, 3, 4, 5}));
	}
	SECTION("nested object") 
	{
		REQUIRE(obj._nested->_int == 14);
		REQUIRE(obj._nested->_bool == false);
	}
}