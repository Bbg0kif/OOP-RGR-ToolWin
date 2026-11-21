#pragma once

#include <Windows.h>

class MyToolWinClass
{
public:
    MyToolWinClass() = default;
    virtual ~MyToolWinClass();

    MyToolWinClass(const MyToolWinClass&) = delete;
    MyToolWinClass& operator=(const MyToolWinClass&) = delete;

    bool Create(
        HINSTANCE instance,
        HWND owner,
        int nx,
        int ny,
        int buttonSize
    );

    void Show();
    void Close();

    HWND GetHandle() const;

protected:
    virtual void OnToolClick(int buttonIndex) = 0;

private:
    static INT_PTR CALLBACK DialogProc(
        HWND hDlg,
        UINT message,
        WPARAM wParam,
        LPARAM lParam
    );

    static constexpr int FirstButtonId = 1000;

    HWND window = nullptr;
    int buttonCount = 0;
};