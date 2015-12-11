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

JSON_CLASS(NestedObjectWithKey)
{
public:
	FIELD_WITH_KEY(int, _int, _intval);
	FIELD_WITH_KEY(bool, _bool, _boolval);
};

JSON_CLASS(TestObjectWithKey)
{
public:
	FIELD_WITH_KEY(int, _int, _intval);
	FIELD_WITH_KEY(bool, _bool, _boolval);
	FIELD_WITH_KEY(float, _float, _floatval);
	FIELD_WITH_KEY(double, _double, _doubleval);
	FIELD_WITH_KEY(std::string, _string, _stringval);

	FIELD_WITH_KEY(std::string, _null, _nullval);

	FIELD_WITH_KEY(TestEnum, _enum, _enumval);

	FIELD_WITH_KEY(std::vector<int>, _vector, _vectorval);

	FIELD_WITH_KEY(NestedObjectWithKey, _nested, _nestedval);
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

TEST_CASE("parse from string / save file / load file (key)")
{
	TestObjectWithKey saveObj;

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

	TestObjectWithKey obj;
	obj.loadFile("test.json");

	SECTION("basic types")
	{
		REQUIRE(obj._intval == 5);
		REQUIRE(obj._boolval == true);
		REQUIRE(obj._floatval == 2.13f);
		REQUIRE(obj._doubleval == 2.5);
		REQUIRE(obj._stringval.get() == "hello");
	}
	SECTION("null")
	{
		REQUIRE(obj._nullval.isNull());
	}
	SECTION("enum")
	{
		REQUIRE(obj._enumval == TestEnum::THREE);
	}
	SECTION("std::vector type")
	{
		REQUIRE(obj._vectorval->size() == 5);
		REQUIRE(obj._vectorval == (std::vector<int>{1, 2, 3, 4, 5}));
	}
	SECTION("nested object")
	{
		REQUIRE(obj._nestedval->_intval == 14);
		REQUIRE(obj._nestedval->_boolval == false);
	}
}

TEST_CASE("assignment")
{
	NestedObjectWithKey nested;

	nested._boolval.set(false);
	nested._intval.set(3);

	TestObjectWithKey toFile;

	toFile._boolval.set(false);
	toFile._doubleval.set(2.79);
	toFile._enumval.set(TestEnum::THREE);
	toFile._floatval.set(2.55f);
	toFile._intval.set(3);
	toFile._nestedval.set(nested);

	toFile._stringval.set("abcd");
	
	std::vector<int> vi = { 1,2,3,4 };
	toFile._vectorval.set(vi);

	toFile.toJson("testFile.json");

	TestObjectWithKey test;

	test.loadFile("testFile.json");

	SECTION("basic types")
	{
		REQUIRE(test._intval == 3);
		REQUIRE(test._boolval == false);
		REQUIRE(test._floatval == 2.55f);
		REQUIRE(test._doubleval == 2.79);
		REQUIRE(test._stringval.get() == "abcd");
	}

	SECTION("null")
	{
		REQUIRE(test._nullval.isNull());
	}

	SECTION("enum")
	{
		REQUIRE(test._enumval == TestEnum::THREE);
	}

	SECTION("std::vector type")
	{
		REQUIRE(test._vectorval->size() == 4);
		REQUIRE(test._vectorval == vi);
	}

	SECTION("nested object")
	{
		REQUIRE(test._nestedval->_intval == 3);
		REQUIRE(test._nestedval->_boolval == false);
	}
}