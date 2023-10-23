#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include <vector>
#include <codecvt>
#include <locale>


// UTF-8 ���� �б� �Լ�
std::wstring readFileUTF8(const std::wstring& filePath) {
    std::wifstream file(filePath, std::ios::binary);
    std::wstring content;

    // UTF-8 -> wstring ��ȯ�� ���� locale ����
    file.imbue(std::locale(file.getloc(), new std::codecvt_utf8<wchar_t, 0x10ffff, std::consume_header>));
    std::wstring line;

    if (file.is_open()) {
        while (std::getline(file, line)) {
            content += line + L"\n";
        }
        file.close();
    }
    else {
        std::wcerr << L"������ �� �� �����ϴ�: " << filePath << std::endl;
    }

    return content;
}

// �ѱ� �ڸ� �и�
std::wstring decomposeHangul(wchar_t ch) {
    const wchar_t cho[19] = { L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��' };
    const wchar_t jung[21] = { L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��' };
    const wchar_t jong[28] = { L'\0', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��' };

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

// ���ڿ� �ڸ� �и�
std::vector<std::wstring> decomposeString(const std::wstring& input) {
    std::vector<std::wstring> resultParts;
    for (wchar_t ch : input) {
        resultParts.push_back(decomposeHangul(ch));
    }
    return resultParts;
}

// �־��� ���ڸ� Ÿ����
void sendChar(wchar_t ch) {
    INPUT input[2] = { 0 };  // �Է� ���� �� �Է� ���Ḧ ���� 2���� INPUT ����ü

    // Ű�� ����
    input[0].type = INPUT_KEYBOARD;
    input[0].ki.wScan = ch;
    input[0].ki.dwFlags = KEYEVENTF_UNICODE;

    // Ű�� ��
    input[1] = input[0];
    input[1].ki.dwFlags |= KEYEVENTF_KEYUP;

    SendInput(2, input, sizeof(INPUT));  // �� �Է� ������ �� ���� ����

    Sleep(100);  // ���ڸ����� ��� �ð�
}

// �־��� ���ڿ��� Ÿ����
void typeString(const std::wstring& str) {
    for (wchar_t ch : str) {
        sendChar(ch);
    }
}

int main() {
    std::wstring content = readFileUTF8(L"C:\\song.txt");
    if (content.empty()) {
        return 1;  // ���� �б� ����
    }

    // Notepad�� ���ϴ�.
    HINSTANCE result = ShellExecuteW(0, L"open", L"notepad.exe", 0, 0, SW_SHOWNORMAL);
    if ((int)result <= 32) {
        std::cerr << "Notepad�� �����ϴ� �� �����߽��ϴ�. ���� �ڵ�: " << (int)result << std::endl;
        return 1;
    }

    // �޸����� ������ ���� ������ ����մϴ�.
    HWND notepad = NULL;
    for (int i = 0; i < 50 && notepad == NULL; i++) {  // �ִ� 5�� ���
        notepad = FindWindowW(L"Notepad", NULL);
        Sleep(100);
    }

    if (notepad == NULL) {
        std::cerr << "5�� ���� Notepad�� ã�� �� �����ϴ�." << std::endl;
        return 1;
    }

    HWND edit = FindWindowExW(notepad, NULL, L"Edit", NULL);
    if (edit == NULL) {
        std::cerr << "Notepad�� ���� ��Ʈ���� ã�� �� �����ϴ�." << std::endl;
        return 1;
    }

    SetForegroundWindow(notepad);  // �޸��忡 ��Ŀ���� ����
    typeString(content);  // ���� ������ ���� Ÿ����

    return 0;
}