#include "framework.h"
#include "ToolWin.h"

MyToolWinClass::~MyToolWinClass()
{
    Close();
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
        window = nullptr;
    }
}

HWND MyToolWinClass::GetHandle() const
{
    return window;
}