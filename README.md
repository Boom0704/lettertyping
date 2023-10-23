# 코드 설명

## 개요
이 프로그램은 주어진 UTF-8 인코딩된 파일의 내용을 읽어, Notepad에 자동으로 타이핑합니다. 또한 한글 문자열의 자모 분리 기능도 제공합니다.

## 프로그램 구조

### 1. `main` 함수
프로그램의 주 실행 부분입니다.

#### 작업 순서:
1. **UTF-8 파일 읽기:** `readFileUTF8` 함수를 호출하여 지정된 경로에서 파일의 내용을 읽습니다.
2. **Notepad 실행:** `ShellExecuteW` 함수를 사용하여 Notepad를 실행합니다.
3. **Notepad 찾기:** 최대 5초 동안 `FindWindowW` 함수를 반복적으로 호출하여 Notepad의 윈도우 핸들을 찾습니다.
4. **편집 컨트롤 찾기:** `FindWindowExW` 함수를 사용하여 Notepad 내의 편집 컨트롤 핸들을 찾습니다.
5. **Notepad 포커스 설정:** `SetForegroundWindow` 함수를 사용하여 Notepad에 포커스를 설정합니다.
6. **내용 타이핑:** `typeString` 함수를 호출하여 읽어온 파일의 내용을 Notepad에 타이핑합니다.

### 2. `readFileUTF8` 함수
UTF-8 인코딩된 파일의 내용을 읽어 wstring으로 반환합니다.

### 3. `typeString` 함수
문자열을 하나씩 타이핑하는 함수입니다. `sendChar` 함수를 사용하여 각 문자를 입력합니다.

### 4. `sendChar` 함수
주어진 문자를 타이핑합니다. `INPUT` 구조체 배열을 사용하여 키 동작을 설정하고 `SendInput` 함수로 동작을 전송합니다.

### 5. 한글 자모 분리
- `decomposeHangul` 함수: 한글 문자를 자모로 분리하여 반환합니다.
- `decomposeString` 함수: 문자열 내의 모든 문자를 자모로 분리합니다.

---

이 프로그램의 주된 목적은 특정 파일의 내용을 자동으로 Notepad에 타이핑하는 것입니다.
