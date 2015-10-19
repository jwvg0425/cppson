# cppson
cpp json utility

# usage

```C++

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

```json
//test.json
{
	"field1" : 1,
	"field2" : 2,
	"field3" : []
}
```
