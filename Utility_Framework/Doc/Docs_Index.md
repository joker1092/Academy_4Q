# Utility_Framework Docs

## Docs 목차

[ [ClassProperty.h](ClassProperty.md) ] 클래스 속성에 관한 헬퍼 클래스들 입니다.

[ [Banchmark.hpp](Banchmark.md) ] 특정 코드블록의 코드 실행시간을 특정하고, 콘솔에 출력해주는 헬퍼 클래스 입니다.

[ [Core.Assert.hpp](Core.Assert.md) ] Core.Assert는 조건이 실패하면 사용자 정의 오류 처리 함수(AssertionFailureFunction)를 호출하여, 오류 메시지 출력과 디버거 중단을 통해 디버깅을 돕고 프로그램을 종료하는 매크로입니다.

[ [Core.Console.hpp](Core.Console.md) ] CoreConsole 클래스는 C#의 Console 클래스처럼 다양한 형식의 문자열을 출력하는 기능을 제공하며, Write 및 WriteLine 메서드를 통해 파일이나 표준 출력에 포매팅된 메시지를 출력할 수 있습니다.

[ [Core.Mathf.h](Core.Mathf.md) ] DirectXMath라이브러리와 DirectX의 simplemath라이브러리를 편리하게 관리하기 위한 헬퍼 클래스들 입니다.

[ [Core.Memory.hpp](Core.Memory.md) ] 할당된 메모리의 안전한 관리를 위한 헬퍼 함수들과 DeferredDeleter 클래스입니다.

[ [Core.Random.h](Core.Random.md) ] Random 클래스는 주어진 범위 내에서 임의의 수를 생성하는 템플릿 클래스이며, 정수와 실수 타입에 대해 각각 적절한 분포를 사용하여 난수를 생성합니다.

[ [Core.Thread.h](Core.Thread.md) ] ThreadPool 클래스는 작업을 큐에 추가하고 여러 스레드에서 병렬로 실행하는 싱글톤 패턴 기반의 스레드 풀 구현입니다. Task 클래스는 결과를 저장하고, 다른 작업을 체인 형태로 연결하는 then 메서드를 제공합니다.

[ [CoreWindow.h](CoreWindow.md) ] CoreWindow 클래스는 윈도우 애플리케이션을 생성하고, 메시지 핸들러를 등록하여 윈도우 메시지를 처리하며, 메시지 루프를 실행하는 기능을 제공하는 클래스입니다.
