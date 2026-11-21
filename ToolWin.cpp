#include "framework.h"
#include "ToolWin.h"

namespace
{
    // An empty dialog template without menu, custom class or title.
    struct alignas(DWORD) EmptyDialogTemplate
    {
        DLGTEMPLATE dialog{};
        WORD menu = 0;
        WORD windowClass = 0;
        WORD title = 0;
    };
}

MyToolWinClass::~MyToolWinClass()
{
    Close();
}

bool MyToolWinClass::Create(
    HINSTANCE instance,
    HWND owner,
    int nx,
    int ny,
    int buttonSize)
{
    if (window != nullptr)
    {
        Show();
        return true;
    }

    // Validate dimensions and prevent integer overflow.
    if (nx <= 0 || ny <= 0 || buttonSize <= 0)
    {
        return false;
    }

    if (nx > 16000 / buttonSize ||
        ny > 16000 / buttonSize)
    {
        return false;
    }

    EmptyDialogTemplate dialogTemplate{};

    dialogTemplate.dialog.style =
        WS_POPUP | WS_CAPTION | WS_SYSMENU;

    dialogTemplate.dialog.dwExtendedStyle = WS_EX_TOOLWINDOW;
    dialogTemplate.dialog.cdit = 0;
    dialogTemplate.dialog.x = 0;
    dialogTemplate.dialog.y = 0;
    dialogTemplate.dialog.cx = 100;
    dialogTemplate.dialog.cy = 100;

    HWND createdWindow = CreateDialogIndirectParamW(
        instance,
        &dialogTemplate.dialog,
        owner,
        DialogProc,
        reinterpret_cast<LPARAM>(this)
    );

    if (createdWindow == nullptr)
    {
        return false;
    }

    SetWindowTextW(window, L"ToolWin");

    // Calculate the outer size from the required client area.
    RECT windowRect{
        0,
        0,
        nx * buttonSize,
        ny * buttonSize
    };

    if (!AdjustWindowRectEx(
        &windowRect,
        dialogTemplate.dialog.style,
        FALSE,
        dialogTemplate.dialog.dwExtendedStyle))
    {
        Close();
        return false;
    }

    if (!SetWindowPos(
        window,
        nullptr,
        100,
        100,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        SWP_NOZORDER | SWP_NOACTIVATE))
    {
        Close();
        return false;
    }

    Show();
    return true;
}

void MyToolWinClass::Show()
{
    if (window != nullptr)
    {
        ShowWindow(window, SW_SHOW);
    }
}

void MyToolWinClass::Close()
{
    if (window != nullptr)
    {
        DestroyWindow(window);
    }
}

HWND MyToolWinClass::GetHandle() const
{
    return window;
}

INT_PTR CALLBACK MyToolWinClass::DialogProc(
    HWND hDlg,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    if (message == WM_INITDIALOG)
    {
        auto* self = reinterpret_cast<MyToolWinClass*>(lParam);

        self->window = hDlg;

        SetWindowLongPtrW(
            hDlg,
            DWLP_USER,
            reinterpret_cast<LONG_PTR>(self)
        );

        return TRUE;
    }

    auto* self = reinterpret_cast<MyToolWinClass*>(
        GetWindowLongPtrW(hDlg, DWLP_USER)
        );

    if (self == nullptr)
    {
        return FALSE;
    }

    switch (message)
    {
    case WM_CLOSE:
        self->Close();
        return TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDCANCEL)
        {
            self->Close();
            return TRUE;
        }
        break;

    case WM_NCDESTROY:
        self->window = nullptr;
        SetWindowLongPtrW(hDlg, DWLP_USER, 0);
        return FALSE;
    }

    return FALSE;
}