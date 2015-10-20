# cppson
cpp json utility.

# Setup
just add 'src/cppson.hpp' file to your project.


# How to use
```JSON_CLASS(className)``` macro create a class mapped to json file.  
```FIELD(type, fieldName)``` macro create a field mapped to json key - value pair. FIELD macro create meta_fieldName field, but you don't need to bother.

parse vector from json file, use cppson::loadFile.


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
	
	if(t.loadFile("test.json"))
	{
		printf("%d %d %d\n", t.field1, t.field2); //1 2
		printf("%d\n", t.field3.size()); //0
	}
	else
	{
		printf("parse failed.\n");
	}

	std::vector<int> vi;

	if(vi.loadFile("test2.json"))
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