#include "../ToolWin.h"

#include <string>

namespace
{
    constexpr UINT ShowToolsCommand = 101;

    class CounterToolWin final : public MyToolWinClass
    {
    public:
        CounterToolWin()
        {
            SetHints({
                L"Add 1",
                L"Subtract 1",
                L"Add 10",
                L"Subtract 10",
                L"Reset counter",
                L"Help"
                });
        }

    protected:
        void OnToolClick(int buttonIndex) override
        {
            switch (buttonIndex)
            {
            case 0:
                ++value;
                break;

            case 1:
                --value;
                break;

            case 2:
                value += 10;
                break;

            case 3:
                value -= 10;
                break;

            case 4:
                value = 0;
                break;

            case 5:
                MessageBoxW(
                    GetHandle(),
                    L"Buttons:\n"
                    L"1: Add 1\n"
                    L"2: Subtract 1\n"
                    L"3: Add 10\n"
                    L"4: Subtract 10\n"
                    L"5: Reset counter\n"
                    L"6: Help\n\n"
                    L"The current value is shown in "
                    L"the main window title.",
                    L"Counter help",
                    MB_OK | MB_ICONINFORMATION
                );
                return;

            default:
                return;
            }

            const std::wstring title =
                std::wstring(L"ToolWin reuse - Counter: ") +
                std::to_wstring(value);

            SetWindowTextW(
                GetParent(GetHandle()),
                title.c_str()
            );
        }

    private:
        long long value = 0;
    };

    CounterToolWin toolWin;

    LRESULT CALLBACK MainWindowProc(
        HWND window,
        UINT message,
        WPARAM wParam,
        LPARAM lParam)
    {
        switch (message)
        {
        case WM_COMMAND:
            if (LOWORD(wParam) == ShowToolsCommand)
            {
                HINSTANCE instance =
                    reinterpret_cast<HINSTANCE>(
                        GetWindowLongPtrW(window, GWLP_HINSTANCE)
                        );

                if (!toolWin.Create(instance, window, 2, 3, 60))
                {
                    MessageBoxW(
                        window,
                        L"Could not create ToolWin.",
                        L"Error",
                        MB_OK | MB_ICONERROR
                    );
                }

                return 0;
            }
            break;

        case WM_DESTROY:
            toolWin.Close();
            PostQuitMessage(0);
            return 0;
        }

        return DefWindowProcW(
            window,
            message,
            wParam,
            lParam
        );
    }
}

int WINAPI wWinMain(
    HINSTANCE instance,
    HINSTANCE,
    PWSTR,
    int showCommand)
{
    const wchar_t* className = L"ToolWinReuseMain";

    WNDCLASSW windowClass{};
    windowClass.lpfnWndProc = MainWindowProc;
    windowClass.hInstance = instance;
    windowClass.lpszClassName = className;
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    windowClass.hbrBackground =
        reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);

    if (!RegisterClassW(&windowClass))
    {
        return 1;
    }

    HWND window = CreateWindowExW(
        0,
        className,
        L"ToolWin reuse - Counter: 0",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        700,
        450,
        nullptr,
        nullptr,
        instance,
        nullptr
    );

    if (window == nullptr)
    {
        return 1;
    }

    HMENU menu = CreateMenu();
    HMENU toolsMenu = CreatePopupMenu();

    if (menu == nullptr || toolsMenu == nullptr)
    {
        if (toolsMenu != nullptr)
        {
            DestroyMenu(toolsMenu);
        }

        if (menu != nullptr)
        {
            DestroyMenu(menu);
        }

        DestroyWindow(window);
        return 1;
    }

    AppendMenuW(
        toolsMenu,
        MF_STRING,
        ShowToolsCommand,
        L"Show ToolWin"
    );

    AppendMenuW(
        menu,
        MF_POPUP,
        reinterpret_cast<UINT_PTR>(toolsMenu),
        L"&Tools"
    );

    SetMenu(window, menu);

    ShowWindow(window, showCommand);
    UpdateWindow(window);

    MSG message{};
    BOOL result;

    while ((result = GetMessageW(&message, nullptr, 0, 0)) > 0)
    {
        if (toolWin.GetHandle() != nullptr &&
            IsDialogMessageW(toolWin.GetHandle(), &message))
        {
            continue;
        }

        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    return result == -1
        ? 1
        : static_cast<int>(message.wParam);
}