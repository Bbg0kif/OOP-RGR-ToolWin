// ToolWinDemo.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "ToolWinDemo.h"
#include "ToolWin.h"
#include "ToolImages.h"
#include <string>

namespace
{
    constexpr UINT ShowToolWinCommand = 40001;
    constexpr UINT Layout2x7Command = 40002;
    constexpr UINT Layout1x14Command = 40003;
    constexpr UINT Layout14x1Command = 40004;

    constexpr UINT SmallButtonsCommand = 40005;
    constexpr UINT MediumButtonsCommand = 40006;
    constexpr UINT LargeButtonsCommand = 40007;

    int toolColumns = 2;
    int toolRows = 7;
    int toolButtonSize = 40;

    class DemoToolWin final : public MyToolWinClass
    {
    public:
        DemoToolWin()
        {
            SetModeButtons({ 0, 1, 2, 3 });
        }
    protected:
        void OnToolClick(int buttonIndex) override
        {
            const std::wstring message =
                std::wstring(L"Selected button: ") +
                std::to_wstring(buttonIndex + 1);

            MessageBoxW(
                GetHandle(),
                message.c_str(),
                L"Tool action",
                MB_OK | MB_ICONINFORMATION
            );
        }
    };

    ToolImages toolImages;
    DemoToolWin toolWin;
}

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
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

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TOOLWINDEMO, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TOOLWINDEMO));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (toolWin.GetHandle() != nullptr &&
            IsDialogMessageW(toolWin.GetHandle(), &msg))
        {
            continue;
        }

        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TOOLWINDEMO));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TOOLWINDEMO);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

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
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        HMENU toolsMenu = CreatePopupMenu();

        AppendMenuW(
            toolsMenu,
            MF_STRING,
            ShowToolWinCommand,
            L"Show ToolWin"
        );

        AppendMenuW(toolsMenu, MF_SEPARATOR, 0, nullptr);

        AppendMenuW(
            toolsMenu, MF_STRING,
            Layout2x7Command, L"Layout: 2 x 7"
        );

        AppendMenuW(
            toolsMenu, MF_STRING,
            Layout1x14Command, L"Layout: 1 x 14"
        );

        AppendMenuW(
            toolsMenu, MF_STRING,
            Layout14x1Command, L"Layout: 14 x 1"
        );

        AppendMenuW(toolsMenu, MF_SEPARATOR, 0, nullptr);

        AppendMenuW(
            toolsMenu, MF_STRING,
            SmallButtonsCommand, L"Button size: 32"
        );

        AppendMenuW(
            toolsMenu, MF_STRING,
            MediumButtonsCommand, L"Button size: 40"
        );

        AppendMenuW(
            toolsMenu, MF_STRING,
            LargeButtonsCommand, L"Button size: 48"
        );

        InsertMenuW(
            GetMenu(hWnd),
            1,
            MF_BYPOSITION | MF_POPUP,
            reinterpret_cast<UINT_PTR>(toolsMenu),
            L"&Tools"
        );

        DrawMenuBar(hWnd);
        return 0;
    }
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case ShowToolWinCommand:
            {
                if (toolWin.GetHandle() != nullptr)
                {
                    toolWin.Show();
                    break;
                }

                if (toolImages.GetImages().empty())
                {
                    if (!toolImages.Create(14, 28))
                    {
                        MessageBoxW(
                            hWnd,
                            L"Could not create tool images.",
                            L"Error",
                            MB_OK | MB_ICONERROR
                        );
                        break;
                    }
                }

                if (!toolWin.SetImages(toolImages.GetImages()))
                {
                    MessageBoxW(
                        hWnd,
                        L"Could not configure tool images.",
                        L"Error",
                        MB_OK | MB_ICONERROR
                    );
                    break;
                }

                const std::vector<std::wstring> hints{
                    L"Tool 1",
                    L"Tool 2",
                    L"Tool 3",
                    L"Tool 4",
                    L"Tool 5",
                    L"Tool 6",
                    L"Tool 7",
                    L"Tool 8",
                    L"Tool 9",
                    L"Tool 10",
                    L"Tool 11",
                    L"Tool 12",
                    L"Tool 13",
                    L"Tool 14"
                };

                if (!toolWin.SetHints(hints) ||
                    !toolWin.Create(
                        hInst,
                        hWnd,
                        toolColumns,
                        toolRows,
                        toolButtonSize))
                {
                    MessageBoxW(
                        hWnd,
                        L"Could not configure or create ToolWin.",
                        L"Error",
                        MB_OK | MB_ICONERROR
                    );
                }

                break;
            }

            case Layout2x7Command:
            case Layout1x14Command:
            case Layout14x1Command:
            case SmallButtonsCommand:
            case MediumButtonsCommand:
            case LargeButtonsCommand:
            {
                switch (wmId)
                {
                case Layout2x7Command:
                    toolColumns = 2;
                    toolRows = 7;
                    break;

                case Layout1x14Command:
                    toolColumns = 1;
                    toolRows = 14;
                    break;

                case Layout14x1Command:
                    toolColumns = 14;
                    toolRows = 1;
                    break;

                case SmallButtonsCommand:
                    toolButtonSize = 32;
                    break;

                case MediumButtonsCommand:
                    toolButtonSize = 40;
                    break;

                case LargeButtonsCommand:
                    toolButtonSize = 48;
                    break;
                }

                RECT previousRect{};
                const bool hadWindow =
                    toolWin.GetHandle() != nullptr &&
                    GetWindowRect(toolWin.GetHandle(), &previousRect);

                toolWin.Close();

                SendMessageW(
                    hWnd,
                    WM_COMMAND,
                    MAKEWPARAM(ShowToolWinCommand, 0),
                    0
                );

                if (hadWindow && toolWin.GetHandle() != nullptr)
                {
                    SetWindowPos(
                        toolWin.GetHandle(),
                        nullptr,
                        previousRect.left,
                        previousRect.top,
                        0,
                        0,
                        SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE
                    );
                }

                break;
            }

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
            BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        toolWin.Close();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
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
