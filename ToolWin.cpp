#include "framework.h"
#include "ToolWin.h"
#include <string>
#include <CommCtrl.h>
#include <algorithm>

#pragma comment(lib, "Comctl32.lib")

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

    // Keep control identifiers within the WM_COMMAND range.
    if (nx > 60000 / ny)
    {
        return false;
    }

    if (!buttonHints.empty() &&
        buttonHints.size() != static_cast<size_t>(nx * ny))
    {
        return false;
    }

    for (int index : modeButtons)
    {
        if (index >= nx * ny)
        {
            return false;
        }
    }

    if (!buttonImages.empty() &&
        buttonImages.size() != static_cast<size_t>(nx * ny))
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

    buttonCount = nx * ny;

    for (int index = 0; index < buttonCount; ++index)
    {
        const int column = index % nx;
        const int row = index / nx;

        const std::wstring text = std::to_wstring(index + 1);

        DWORD buttonStyle =
            IsModeButton(index)
            ? (BS_RADIOBUTTON | BS_PUSHLIKE)
            : BS_PUSHBUTTON;

        if (!buttonImages.empty())
        {
            buttonStyle |= BS_BITMAP;
        }

        HWND button = CreateWindowExW(
            0,
            L"BUTTON",
            text.c_str(),
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | buttonStyle,
            column * buttonSize,
            row * buttonSize,
            buttonSize,
            buttonSize,
            window,
            reinterpret_cast<HMENU>(
                static_cast<INT_PTR>(FirstButtonId + index)
                ),
            instance,
            nullptr
        );

        if (button == nullptr)
        {
            Close();
            return false;
        }

        if (!buttonImages.empty())
        {
            SendMessageW(
                button,
                BM_SETIMAGE,
                IMAGE_BITMAP,
                reinterpret_cast<LPARAM>(buttonImages[index])
            );
        }

        if (!SetWindowSubclass(
            button,
            ButtonProc,
            static_cast<UINT_PTR>(index),
            reinterpret_cast<DWORD_PTR>(this)))
        {
            Close();
            return false;
        }
    }

    UpdateModeButtons();
    Show();
    return true;
}

bool MyToolWinClass::SetHints(
    const std::vector<std::wstring>& hints)
{
    if (window != nullptr &&
        hints.size() != static_cast<size_t>(buttonCount))
    {
        return false;
    }

    buttonHints = hints;

    if (window != nullptr)
    {
        SetWindowTextW(window, L"ToolWin");
    }

    return true;
}

bool MyToolWinClass::SetModeButtons(
    const std::vector<int>& indices)
{
    // Configure button types before creating the dialog.
    if (window != nullptr)
    {
        return false;
    }

    for (int index : indices)
    {
        if (index < 0)
        {
            return false;
        }
    }

    modeButtons = indices;

    if (!IsModeButton(selectedMode))
    {
        selectedMode = -1;
    }

    return true;
}

bool MyToolWinClass::IsModeButton(int index) const
{
    return std::find(
        modeButtons.begin(),
        modeButtons.end(),
        index
    ) != modeButtons.end();
}

void MyToolWinClass::UpdateModeButtons()
{
    if (window == nullptr)
    {
        return;
    }

    for (int index : modeButtons)
    {
        HWND button = GetDlgItem(window, FirstButtonId + index);

        SendMessageW(
            button,
            BM_SETCHECK,
            index == selectedMode ? BST_CHECKED : BST_UNCHECKED,
            0
        );
    }
}

bool MyToolWinClass::SetImages(
    const std::vector<HBITMAP>& images)
{
    // Configure images before creating the dialog.
    if (window != nullptr)
    {
        return false;
    }

    for (HBITMAP bitmap : images)
    {
        if (bitmap == nullptr ||
            GetObjectType(bitmap) != OBJ_BITMAP)
        {
            return false;
        }
    }

    buttonImages = images;
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
    {
        const int commandId = LOWORD(wParam);

        if (commandId == IDCANCEL)
        {
            self->Close();
            return TRUE;
        }

        const int buttonIndex = commandId - FirstButtonId;

        if (lParam != 0 &&
            HIWORD(wParam) == BN_CLICKED &&
            buttonIndex >= 0 &&
            buttonIndex < self->buttonCount)
        {
            if (self->IsModeButton(buttonIndex))
            {
                self->selectedMode = buttonIndex;
                self->UpdateModeButtons();
            }

            self->OnToolClick(buttonIndex);
            return TRUE;
        }

        break;
    }

    case WM_NCDESTROY:
        self->window = nullptr;
        self->buttonCount = 0;
        SetWindowLongPtrW(hDlg, DWLP_USER, 0);
        return FALSE;
    }

    return FALSE;
}

LRESULT CALLBACK MyToolWinClass::ButtonProc(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam,
    UINT_PTR subclassId,
    DWORD_PTR referenceData)
{
    auto* self = reinterpret_cast<MyToolWinClass*>(referenceData);

    switch (message)
    {
    case WM_MOUSEMOVE:
    {
        const size_t index = static_cast<size_t>(subclassId);

        const wchar_t* hint = L"ToolWin";

        if (index < self->buttonHints.size() &&
            !self->buttonHints[index].empty())
        {
            hint = self->buttonHints[index].c_str();
        }

        SetWindowTextW(self->window, hint);

        TRACKMOUSEEVENT tracking{};
        tracking.cbSize = sizeof(tracking);
        tracking.dwFlags = TME_LEAVE;
        tracking.hwndTrack = hWnd;

        TrackMouseEvent(&tracking);
        break;
    }

    case WM_MOUSELEAVE:
        SetWindowTextW(self->window, L"ToolWin");
        break;

    case WM_NCDESTROY:
        RemoveWindowSubclass(hWnd, ButtonProc, subclassId);
        break;
    }

    return DefSubclassProc(hWnd, message, wParam, lParam);
}