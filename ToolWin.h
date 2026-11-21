#pragma once

#include <Windows.h>

class MyToolWinClass
{
public:
    MyToolWinClass() = default;
    virtual ~MyToolWinClass();

    MyToolWinClass(const MyToolWinClass&) = delete;
    MyToolWinClass& operator=(const MyToolWinClass&) = delete;

    void Show();
    void Close();

    HWND GetHandle() const;

protected:
    virtual void OnToolClick(int buttonIndex) = 0;

private:
    HWND window = nullptr;
};