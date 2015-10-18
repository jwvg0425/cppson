# cppson
cpp json utility

# usage

```C++

class Test : cppson::parsable
{
public:
  FIELD(int, field1);
  FIELD(int, field2);
  OPTIONAL(int, field3, 0);
  FIELD(std::vector<int>, field4);
};

int main()
{
  Test t;
  
  t.parse("test.json");
  
  printf("%d %d %d", t.field1, t.field2, t.field3); //1 2 0
  printf("%d", t.field4.size()); //0
  return 0;
}
```

```json
//test.json
{
  "field1" : 1,
  "field2" : 2,
  "field4" : []
}
```
