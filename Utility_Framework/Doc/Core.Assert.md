# Core.Assert.hpp

`Core.Assert.hpp`는 코드에서 조건을 확인하고, 조건이 거짓일 경우 디버깅을 위해 중단시키고 프로그램을 종료하는 **Assert** 매크로를 정의한 헤더 파일입니다. 이 매크로는 디버깅 빌드에서는 오류를 강제로 중단시키고, 릴리즈 빌드에서는 비활성화됩니다.

## 매크로 정의

### `DEFINE__ASSERT`

`DEFINE__ASSERT` 매크로는 특정 조건이 `false`일 경우 디버깅 세션에서 중단을 유도하고 프로그램을 종료시킵니다. 조건이 참이면 아무런 동작을 하지 않습니다.

```cpp
#define DEFINE__ASSERT(expression, ...) \
	if(!(expression)) \
	{ \
		 __debugbreak(); \
		std::abort(); \
	} \
	else {}
```
### 동작 설명

#### `DEFINE__ASSERT`

- **`expression`**: 검증할 조건식입니다. 이 조건식이 `false`일 경우, 디버거가 활성화되어 있으면 프로그램이 중단되고, 이후 `std::abort()`가 호출되어 프로그램이 종료됩니다.
- **`__debugbreak()`**: 디버거가 활성화되어 있을 때 실행되면, 프로그램을 중단하고 디버거를 활성화시켜 문제를 추적할 수 있게 합니다. 이 명령은 주로 Visual Studio와 같은 디버깅 환경에서 사용됩니다.
- **`std::abort()`**: 조건식이 `false`일 경우, 디버거가 활성화되었든 아니든 프로그램을 즉시 종료합니다. 이 함수는 비정상적인 종료를 유도하는 데 사용됩니다.

#### `CORE_ASSERT`

- **릴리즈 빌드 (`NDEBUG` 정의됨)**: `CORE_ASSERT`는 코드에서 아무 동작도 하지 않습니다. `NDEBUG` 매크로가 정의되면, assert 기능이 비활성화되어 성능 최적화가 이루어집니다.
- **디버그 빌드 (`NDEBUG` 미정의)**: `CORE_ASSERT`는 `DEFINE__ASSERT` 매크로를 호출하여 조건을 평가하고, 조건이 `false`일 경우, 프로그램을 중단시킵니다. `__debugbreak()`를 호출하여 디버깅 환경에서 중단하고, `std::abort()`를 통해 프로그램을 종료시킵니다. 이 동작은 디버깅 중에 코드의 문제를 빠르게 파악하고 수정할 수 있도록 돕습니다.

## 사용 예시

이 매크로는 코드에서 특정 조건을 검증하고, 조건이 만족하지 않을 경우 디버깅 중에 즉시 프로그램을 중단시켜 문제를 추적할 수 있게 도와줍니다.

```cpp
#include "Core.Assert.hpp"

void SomeFunction()
{
    int value = 10;

    // value가 0이 아닌지 확인
    CORE_ASSERT(value != 0, "value cannot be zero!");
}

int main()
{
    SomeFunction();
    return 0;
}
```
#### 디버그 빌드에서

디버그 빌드에서는 `NDEBUG`가 정의되지 않기 때문에 `CORE_ASSERT`가 활성화됩니다. 이 경우, 조건식 `value != 0`이 `false`일 경우 `DEFINE__ASSERT` 매크로가 실행됩니다. 그 결과, 다음과 같은 동작이 발생합니다:

1. **`__debugbreak()`**: 디버거가 활성화된 경우, 프로그램이 중단되고 디버거로 제어가 넘어갑니다. 이를 통해 코드 실행 흐름을 중지하고, 조건이 `false`인 이유를 디버깅할 수 있습니다.
2. **`std::abort()`**: 프로그램을 즉시 종료시킵니다. 조건이 `false`일 경우, `__debugbreak()`로 디버깅을 시작한 후, `std::abort()`가 호출되어 프로그램을 강제로 종료시키므로, 문제를 빠르게 파악할 수 있도록 돕습니다.

디버그 빌드에서 `CORE_ASSERT`는 코드의 논리적 오류를 식별하고, 이를 디버깅 환경에서 즉시 해결할 수 있도록 돕습니다.

#### 릴리즈 빌드에서

릴리즈 빌드에서는 `NDEBUG`가 정의되므로 `CORE_ASSERT`는 아무런 동작도 하지 않습니다. 즉, `CORE_ASSERT`가 포함된 코드가 실행되더라도 조건이 `false`일 경우에도 프로그램은 중단되지 않고 정상적으로 실행됩니다. 이는 성능 최적화를 위해 assert 기능이 비활성화된 상태입니다.