#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include <vector>
#include <codecvt>
#include <locale>


// UTF-8 파일 읽기 함수
std::wstring readFileUTF8(const std::wstring& filePath) {
    std::wifstream file(filePath, std::ios::binary);
    std::wstring content;

    // UTF-8 -> wstring 변환을 위한 locale 설정
    file.imbue(std::locale(file.getloc(), new std::codecvt_utf8<wchar_t, 0x10ffff, std::consume_header>));
    std::wstring line;

    if (file.is_open()) {
        while (std::getline(file, line)) {
            content += line + L"\n";
        }
        file.close();
    }
    else {
        std::wcerr << L"파일을 열 수 없습니다: " << filePath << std::endl;
    }

    return content;
}

// 한글 자모 분리
std::wstring decomposeHangul(wchar_t ch) {
    const wchar_t cho[19] = { L'ㄱ', L'ㄲ', L'ㄴ', L'ㄷ', L'ㄸ', L'ㄹ', L'ㅁ', L'ㅂ', L'ㅃ', L'ㅅ', L'ㅆ', L'ㅇ', L'ㅈ', L'ㅉ', L'ㅊ', L'ㅋ', L'ㅌ', L'ㅍ', L'ㅎ' };
    const wchar_t jung[21] = { L'ㅏ', L'ㅐ', L'ㅑ', L'ㅒ', L'ㅓ', L'ㅔ', L'ㅕ', L'ㅖ', L'ㅗ', L'ㅘ', L'ㅙ', L'ㅚ', L'ㅛ', L'ㅜ', L'ㅝ', L'ㅞ', L'ㅟ', L'ㅠ', L'ㅡ', L'ㅢ', L'ㅣ' };
    const wchar_t jong[28] = { L'\0', L'ㄱ', L'ㄲ', L'ㄳ', L'ㄴ', L'ㄵ', L'ㄶ', L'ㄷ', L'ㄹ', L'ㄺ', L'ㄻ', L'ㄼ', L'ㄽ', L'ㄾ', L'ㄿ', L'ㅀ', L'ㅁ', L'ㅂ', L'ㅄ', L'ㅅ', L'ㅆ', L'ㅇ', L'ㅈ', L'ㅊ', L'ㅋ', L'ㅌ', L'ㅍ', L'ㅎ' };

    std::wstring result = L"";
    if (ch >= 0xAC00 && ch <= 0xD7A3) {
        int a = ch - 0xAC00;
        int choIdx = a / (21 * 28);
        int jungIdx = a % (21 * 28) / 28;
        int jongIdx = a % 28;

        result += cho[choIdx];
        result += jung[jungIdx];
        if (jong[jongIdx] != L'\0') {
            result += jong[jongIdx];
        }
    }
    else {
        result = ch;
    }

    return result;
}

// 문자열 자모 분리
std::vector<std::wstring> decomposeString(const std::wstring& input) {
    std::vector<std::wstring> resultParts;
    for (wchar_t ch : input) {
        resultParts.push_back(decomposeHangul(ch));
    }
    return resultParts;
}

// 주어진 문자를 타이핑
void sendChar(wchar_t ch) {
    INPUT input[2] = { 0 };  // 입력 시작 및 입력 종료를 위한 2개의 INPUT 구조체

    // 키를 누름
    input[0].type = INPUT_KEYBOARD;
    input[0].ki.wScan = ch;
    input[0].ki.dwFlags = KEYEVENTF_UNICODE;

    // 키를 뗌
    input[1] = input[0];
    input[1].ki.dwFlags |= KEYEVENTF_KEYUP;

    SendInput(2, input, sizeof(INPUT));  // 두 입력 동작을 한 번에 전송

    Sleep(100);  // 글자마다의 대기 시간
}

// 주어진 문자열을 타이핑
void typeString(const std::wstring& str) {
    for (wchar_t ch : str) {
        sendChar(ch);
    }
}

int main() {
    std::wstring content = readFileUTF8(L"C:\\song.txt");
    if (content.empty()) {
        return 1;  // 파일 읽기 실패
    }

    // Notepad를 엽니다.
    HINSTANCE result = ShellExecuteW(0, L"open", L"notepad.exe", 0, 0, SW_SHOWNORMAL);
    if ((int)result <= 32) {
        std::cerr << "Notepad를 실행하는 데 실패했습니다. 오류 코드: " << (int)result << std::endl;
        return 1;
    }

    // 메모장이 완전히 열릴 때까지 대기합니다.
    HWND notepad = NULL;
    for (int i = 0; i < 50 && notepad == NULL; i++) {  // 최대 5초 대기
        notepad = FindWindowW(L"Notepad", NULL);
        Sleep(100);
    }

    if (notepad == NULL) {
        std::cerr << "5초 내에 Notepad를 찾을 수 없습니다." << std::endl;
        return 1;
    }

    HWND edit = FindWindowExW(notepad, NULL, L"Edit", NULL);
    if (edit == NULL) {
        std::cerr << "Notepad의 편집 컨트롤을 찾을 수 없습니다." << std::endl;
        return 1;
    }

    SetForegroundWindow(notepad);  // 메모장에 포커스를 설정
    typeString(content);  // 파일 내용을 직접 타이핑

    return 0;
}