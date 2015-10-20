# cppson
cpp json utility.

# Setup
just add 'src/cppson.hpp' file to your project.


# How to use
```JSON_CLASS(className)``` macro create a class mapped to json file.  
```FIELD(type, fieldName)``` macro create a field mapped to json key - value pair. FIELD macro create meta_fieldName field, but you don't need to bother.


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
	
	if(t.InitFromFile("test.json"))
	{
		printf("%d %d %d", t.field1, t.field2); //1 2
		printf("%d", t.field3.size()); //0
	}
	else
	{
		printf("parse failed.");
	}
	
	return 0;
}
```

```json file
//test.json
{
	"field1" : 1,
	"field2" : 2,
	"field3" : []
}
```
