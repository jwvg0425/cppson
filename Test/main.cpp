#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "src/cppson.hpp"

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

	FIELD(std::vector<int>, _vector);

	FIELD(NestedObject, _nested);
};

TEST_CASE("parse from string") 
{
	TestObject obj;

	cppson::loadString(obj,
		"{"
		/* for 'basic types' section */
		"\"_int\" : 5,"
		"\"_bool\" : \"true\","
		"\"_float\" : 2.13,"
		"\"_double\" : 2.5,"
		"\"_string\" : \"hello\","

		/* for 'std::vector type' section */
		"\"_vector\" : [1, 2, 3 ,4 ,5],"

		/* for 'nested object' section */
		"\"_nested\" : {"
		"\"_int\" : 14,"
		"\"_bool\" : false"
		"}"
		"}");

	SECTION("basic types") {
		REQUIRE(obj._int == 5);
		REQUIRE(obj._bool == true);
		REQUIRE(obj._float == 2.13f);
		REQUIRE(obj._double == 2.5);
		REQUIRE(obj._string.get() == "hello");
	}
	SECTION("std::vector type") {
		REQUIRE(obj._vector->size() == 5);
		REQUIRE(obj._vector == (std::vector<int>{1, 2, 3, 4, 5}));
	}
	SECTION("nested object") {
		REQUIRE(obj._nested->_int == 14);
		REQUIRE(obj._nested->_bool == false);
	}
}