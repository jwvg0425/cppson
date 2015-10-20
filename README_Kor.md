# cppson

c++에서 json 파싱을 편하게 할 수 있도록 도와주는 라이브러리입니다.

# 설치
src 폴더의 cppson.hpp 파일을 프로젝트에 추가하기만 하면 됩니다.

# 대응 관계

| json        | C++            |
| -------     | -------------- |
| integer     | int            |
| real number | double         |
| bool        | bool           |
| string      | std::string    |
| array       | std::vector<T> |
| object      | JSON_CLASS     |

그리고 json 포맷의 **null** 값은 허용하지 않습니다(parse에 실패합니다).

# 사용법
```JSON_CLASS(className)``` 매크로는 해당 클래스가 자동으로 json file과 대응되게 만들어줍니다.  
```FIELD(type, fieldName)``` 매크로는 해당 필드가 json 파일에서 해당 필드와 같은 이름의 키 값과 자동으로 대응되게 만들어줍니다. FIELD 매크로는 각각 해당 클래스에 meta_fieldName이라는 멤버를 생성하는데, 이 멤버는 그냥 무시하시면 됩니다.

이렇게 만든 JSON_CLASS에 대해 멤버함수 loadFile을 호출하시면 자동으로 클래스의 멤버 변수와 json file의 값을 대응시킵니다.

만약 std::vector에 대해 json 파일을 매핑하고 싶다면 cppson::loadFile 함수를 사용하시면 됩니다.

아래 예시를 참고하세요.

## 예시

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
	
	//loadFile 함수를 호출하면 json파일 데이터를 기반으로 클래스의 멤버 변수 값을 초기화 시킵니다.
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
