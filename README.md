# cppson
cpp json utility.

[korean](README_Kor.md)  

# Setup
just add 'src/cppson.hpp' file to your project.

# Branch
- v-1.0 : simple version. use own parsing system(inefficient, slow, unstable).

- jsoncpp-compatible : use jsoncpp to parse json file. recommand to use this version.

# Requirement
- Visual studio 2013 + 

# mapping

| json        | C++            |
| -------     | -------------- |
| integer     | int            |
| real number | float, double  |
| bool        | bool           |
| string      | std::string    |
| array       | std::vector<T> |
| object      | JSON_CLASS     |

# How to use
```JSON_CLASS(className)``` macro create a class mapped to json file.  
```FIELD(type, fieldName)``` macro create a field mapped to json key - value pair. member created by FIELD macro has below methods:

- isNull : that field's value is null, return true.
- get : return that field's inner value.
- operator -> : if that field's inner value type is class or structure, using operator -> to access inner value's member.

field type is compatible with that field's inner value type.

and, call loadFile function(jsonClass.loadFile), automatically mapped json file to that class.

parse vector from json file, use cppson::loadFile function.

see below example.

## Example

```C++
#include "cppson.hpp"

JSON_CLASS(Test)
{
public:
	FIELD(int, field1);
	FIELD(int, field2);
	FIELD(std::vector<int>, field3);
};

int main()
{
	Test t;
	
	//call loadFile, json file data mapped to class member variable.
	if(t.loadFile("test.json"))
	{
		printf("%d %d %d\n", t.field1, t.field2); //1 2
		printf("%d\n", t.field3->size()); //0
	}
	else
	{
		printf("parse failed.\n");
	}

	std::vector<int> vi;

	if(cppson::loadFile("test2.json"))
	{
		//1, 2, 3, 4, 5, 10
		for(auto i : vi)
		{
			printf("%d ", i);
		}
	}
	
	return 0;
}
```

```json
//test.json
{
	"field1" : 1,
	"field2" : 2,
	"field3" : []
}
```

```json
//test2.json
[
	1, 2, 3, 4, 5, 10
]
```
