// OSPractic2.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "OSPractic2.h"
#include <ctime>

#define MAX_LOADSTRING 100
int R;
int G;
int B;
// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.
    srand(unsigned(time(0)));
    R = rand() % 255;
    G = rand() % 255;
    B = rand() % 255;
    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_OSPRACTIC2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OSPRACTIC2));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}


unsigned long createRGB(int r, int g, int b)
{
    return ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
}

//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    HBRUSH whiteBrush = CreateSolidBrush(RGB(R, G, B));

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OSPRACTIC2));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = whiteBrush;
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_OSPRACTIC2);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

#define COLORREF2RGB(Color) (Color & 0xff00) | ((Color >> 16) & 0xff) \
 | ((Color << 16) & 0xff0000)

//-------------------------------------------------------------------------------
// ReplaceColor
//
// Author : Dimitri Rochette drochette@coldcat.fr
// Specials Thanks to Joe Woodbury for his comments and code corrections
//
// Includes : Only <windows.h>

//
// hBmp : Source Bitmap
// cOldColor : Color to replace in hBmp
// cNewColor : Color used for replacement
// hBmpDC : DC of hBmp ( default NULL ) could be NULL if hBmp is not selected
//
// Retcode : HBITMAP of the modified bitmap or NULL for errors
//
//-------------------------------------------------------------------------------
HBITMAP ReplaceColor(HBITMAP hBmp, COLORREF cNewColor, HDC hBmpDC)
{
    HBITMAP RetBmp = NULL;
    if (hBmp)
    {
        HDC BufferDC = CreateCompatibleDC(NULL); // DC for Source Bitmap
        if (BufferDC)
        {
            HBITMAP hTmpBitmap = (HBITMAP)NULL;
            if (hBmpDC)
                if (hBmp == (HBITMAP)GetCurrentObject(hBmpDC, OBJ_BITMAP))
                {
                    hTmpBitmap = CreateBitmap(1, 1, 1, 1, NULL);
                    SelectObject(hBmpDC, hTmpBitmap);
                }

            HGDIOBJ PreviousBufferObject = SelectObject(BufferDC, hBmp);
            // here BufferDC contains the bitmap

            HDC DirectDC = CreateCompatibleDC(NULL); // DC for working
            if (DirectDC)
            {
                // Get bitmap size
                BITMAP bm;
                GetObject(hBmp, sizeof(bm), &bm);

                // create a BITMAPINFO with minimal initilisation 
                               // for the CreateDIBSection
                BITMAPINFO RGB32BitsBITMAPINFO;
                ZeroMemory(&RGB32BitsBITMAPINFO, sizeof(BITMAPINFO));
                RGB32BitsBITMAPINFO.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                RGB32BitsBITMAPINFO.bmiHeader.biWidth = bm.bmWidth;
                RGB32BitsBITMAPINFO.bmiHeader.biHeight = bm.bmHeight;
                RGB32BitsBITMAPINFO.bmiHeader.biPlanes = 1;
                RGB32BitsBITMAPINFO.bmiHeader.biBitCount = 32;

                // pointer used for direct Bitmap pixels access
                UINT* ptPixels;

                HBITMAP DirectBitmap = CreateDIBSection(DirectDC,
                    (BITMAPINFO*)&RGB32BitsBITMAPINFO,
                    DIB_RGB_COLORS,
                    (void**)&ptPixels,
                    NULL, 0);
                if (DirectBitmap)
                {
                    // here DirectBitmap!=NULL so ptPixels!=NULL no need to test
                    HGDIOBJ PreviousObject = SelectObject(DirectDC, DirectBitmap);
                    BitBlt(DirectDC, 0, 0,
                        bm.bmWidth, bm.bmHeight,
                        BufferDC, 0, 0, SRCCOPY);

                    // here the DirectDC contains the bitmap

                                       // Convert COLORREF to RGB (Invert RED and BLUE)
                    cNewColor = COLORREF2RGB(cNewColor);

                    COLORREF cOldColor = ptPixels[0];
                    // After all the inits we can do the job : Replace Color
                    for (int i = ((bm.bmWidth * bm.bmHeight) - 1); i >= 0; i--)
                    {
                        if (ptPixels[i] == cOldColor) ptPixels[i] = cNewColor;
                    }
                    // little clean up
                                       // Don't delete the result of SelectObject because it's 
                                       // our modified bitmap (DirectBitmap)
                    SelectObject(DirectDC, PreviousObject);

                    // finish
                    RetBmp = DirectBitmap;
                }
                // clean up
                DeleteDC(DirectDC);
            }
            if (hTmpBitmap)
            {
                SelectObject(hBmpDC, hBmp);
                DeleteObject(hTmpBitmap);
            }
            SelectObject(BufferDC, PreviousBufferObject);
            // BufferDC is now useless
            DeleteDC(BufferDC);
        }
    }
    return RetBmp;
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // загрузка bitmap-а из файла:
            HBITMAP hBmp = (HBITMAP)LoadImage(NULL, L"..\\Фигура3.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            // создание memory DC для этого bitmap-а:     
            BITMAP bmp;
            GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp);
            HBITMAP hBmp2 = ReplaceColor(hBmp, createRGB(B,G,R), hdc);
            HDC bmpDC = CreateCompatibleDC(hdc);
            SelectObject(bmpDC, hBmp2);
            // вывод (wndDC - HDC окна):
            RECT rect;
            if (GetWindowRect(hWnd, &rect))
            {
                int width = rect.right - rect.left;
                int height = rect.bottom - rect.top;
                BitBlt(hdc, 0, height - bmp.bmHeight - 50, bmp.bmWidth, bmp.bmHeight, bmpDC, 0, 0, SRCCOPY);
            }
            // освобождение ресурсов:           
            DeleteDC(bmpDC);
            DeleteObject(hBmp);
            DeleteObject(hBmp2);
            // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
//L"C:\\Users\\dn_fe\\OneDrive\\Documents\\Учеба\\5 семестр\\Операционные системы\\Фигура1.bmp"
