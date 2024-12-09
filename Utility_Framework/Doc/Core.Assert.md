# Core::ASSERT

`Core::ASSERT`는 프로그램에서 조건이 만족하지 않을 때, 오류 메시지를 출력하고 디버거를 중단하는 함수입니다. 이 코드는 `EASTL`의 `ASSERT` 동작을 모방하여 작성되었습니다.

## 동작 설명

`Core::ASSERT`는 주어진 조건이 `false`일 경우, 아래와 같은 동작을 수행합니다:

1. **오류 메시지 출력**: 조건이 `false`일 경우, 해당 조건을 출력하는 오류 메시지가 콘솔에 표시됩니다.
2. **디버깅 도구에 메시지 전달**: 디버거가 연결되어 있는 경우, `OutputDebugStringA`를 통해 디버깅 도구에 메시지가 전달됩니다.
3. **디버깅 중단**: `__debugbreak()`를 사용하여 디버거가 활성화된 경우 프로그램 실행을 중단합니다.
4. **프로그램 종료**: 프로그램을 종료시키기 위해 `std::abort()`를 호출합니다.

## 사용 방법

### 1. `AssertionFailureFunction`과 `AssertionFailure` 함수

- `AssertionFailureFunction`: Assertion 실패 시 호출될 함수 포인터입니다. 기본적으로 `AssertionFailureFunctionDefault`가 사용되며, 이를 사용자 정의 함수로 변경할 수 있습니다.
  
- `SetAssertionFailureFunction`: Assertion 실패 시 사용할 사용자 정의 함수와 그 컨텍스트를 설정하는 함수입니다.
  
- `AssertionFailure`: Assertion 실패 시 호출되어 위에서 설정한 함수 포인터를 통해 Assertion을 처리합니다.

### 2. `CORE_ASSERT` 매크로

`CORE_ASSERT`는 조건이 `false`일 때 Assertion을 발생시키는 매크로입니다. 예를 들어:

```cpp
CORE_ASSERT(expression);
```
