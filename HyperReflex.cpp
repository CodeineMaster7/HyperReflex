#include <iostream>
#include <windows.h>
#include <gdiplus.h>
#include <vector>
#include <thread>
#include <chrono>
#include <sstream> // Для генерации строк

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

#include <fstream>  // Для работы с файлами

// Функция для установки цвета текста в консоли
void SetConsoleColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// Функция очистки консоли и вывода баннера
void PrintBanner() {
    system("cls");

    SetConsoleColor(5);
    std::cout << "====================\n";

    SetConsoleColor(12);
    std::cout << "  by CodeineMaster  \n";

    SetConsoleColor(5);
    std::cout << "====================\n\n";

    SetConsoleColor(7); // Возвращаем стандартный цвет (белый)
}

struct Config {
    int zone;
    int color_r, color_g, color_b;
    int color_tolerance;
    int capslock_delay;
    int alt_delay;
};

// Функция загрузки конфигурации из файла
bool LoadConfig(const std::string& filename, Config& config) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Failed to open config file!" << std::endl;
        return false;
    }

    file >> config.zone
         >> config.color_r >> config.color_g >> config.color_b
         >> config.color_tolerance
         >> config.capslock_delay;

    file.close();
    return true;
}

// Структура для представления цвета RGB
struct RGBColor {
    int R, G, B;
};

// Функция для инициализации GDI+
void InitGDIPlus() {
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

// Функция захвата экрана и получения изображения
HBITMAP CaptureScreenRegion(int x, int y, int width, int height) {
    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, width, height);
    SelectObject(hDC, hBitmap);
    BitBlt(hDC, 0, 0, width, height, hScreen, x, y, SRCCOPY);
    ReleaseDC(NULL, hScreen);
    DeleteDC(hDC);
    return hBitmap;
}

// Функция для определения совпадения цвета
bool IsColorMatch(const RGBColor& color, const RGBColor& target, int tolerance) {
    return (abs(color.R - target.R) <= tolerance &&
            abs(color.G - target.G) <= tolerance &&
            abs(color.B - target.B) <= tolerance);
}

// Функция для поиска цвета в заданной области
bool SearchForColorInRegion(int x, int y, int width, int height, const RGBColor& targetColor, int tolerance) {
    HBITMAP hBitmap = CaptureScreenRegion(x, y, width, height);

    BITMAPINFO bmpInfo;
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biWidth = width;
    bmpInfo.bmiHeader.biHeight = -height; // Вверх ногами, чтобы не переворачивать
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biBitCount = 32;
    bmpInfo.bmiHeader.biCompression = BI_RGB;

    std::vector<int> pixels(width * height * 4);
    HDC hDC = GetDC(NULL);
    GetDIBits(hDC, hBitmap, 0, height, pixels.data(), &bmpInfo, DIB_RGB_COLORS);
    ReleaseDC(NULL, hDC);
    DeleteObject(hBitmap);

    for (const auto& pixel : pixels) {
        RGBColor color = {
            GetRValue(pixel),
            GetGValue(pixel),
            GetBValue(pixel)
        };
        if (IsColorMatch(color, targetColor, tolerance)) {
            return true;
        }
    }
    return false;
}

// Функция эмуляции клика мыши
void ClickMouse() {
    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);

    PrintBanner();
    std::cout << "Mouse clicked!" << std::endl;
}

// Функция для перезапуска скрипта
void RestartScript() {
    char filename[MAX_PATH];
    GetModuleFileNameA(NULL, filename, MAX_PATH);

    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    if (CreateProcessA(filename, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    if (!CreateProcessA(filename, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        std::cerr << "Failed to restart the script!" << std::endl;
    }
}

// Главная функция
int main() {
    InitGDIPlus();
    
    Config config;
    if (!LoadConfig("config.txt", config)) {
        return 1;
    }

    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);
    RGBColor targetColor = { config.color_r, config.color_g, config.color_b };
    std::cout << "Starting Script..." << std::endl;
    while (true) {
        bool isAltPressed = GetAsyncKeyState(VK_MENU) & 0x8000;
        bool isCapsLockPressed = GetAsyncKeyState(VK_CAPITAL) & 0x8000;

        if (isAltPressed || isCapsLockPressed) {
            std::cout << "Pressed" << std::endl;
            auto start_time = std::chrono::high_resolution_clock::now();

            int x = (width / 2) - config.zone;
            int y = (height / 2) - config.zone;

            if (SearchForColorInRegion(x, y, config.zone * 2, config.zone * 2, targetColor, config.color_tolerance)) {
                if (isCapsLockPressed) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(config.capslock_delay));
                }
                if (isAltPressed) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(config.alt_delay));
                }
                ClickMouse();

                auto end_time = std::chrono::high_resolution_clock::now();
                auto reaction_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
                std::cout << "Reaction time: " << reaction_time.count() << " ms" << std::endl;

                RestartScript();
            }
        }

        if (GetAsyncKeyState(VK_END) & 0x8000) {
            std::cout << "Exiting script..." << std::endl;
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(3));
    }

    return 0;
}