#pragma once

#include <Windows.h>
#include <string>
#include <vector>

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

    bool SetHints(const std::vector<std::wstring>& hints);

    bool SetModeButtons(const std::vector<int>& indices);

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

    static LRESULT CALLBACK ButtonProc(
        HWND hWnd,
        UINT message,
        WPARAM wParam,
        LPARAM lParam,
        UINT_PTR subclassId,
        DWORD_PTR referenceData
    );

    bool IsModeButton(int index) const;
    void UpdateModeButtons();

    std::vector<int> modeButtons;
    int selectedMode = -1;

    std::vector<std::wstring> buttonHints;

    static constexpr int FirstButtonId = 1000;

    HWND window = nullptr;
    int buttonCount = 0;
};