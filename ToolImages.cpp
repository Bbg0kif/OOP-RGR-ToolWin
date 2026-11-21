#include "framework.h"
#include "ToolImages.h"

#include <string>

ToolImages::~ToolImages()
{
    Clear();
}

void ToolImages::Clear()
{
    for (HBITMAP bitmap : images)
    {
        DeleteObject(bitmap);
    }

    images.clear();
}

const std::vector<HBITMAP>& ToolImages::GetImages() const
{
    return images;
}

bool ToolImages::Create(int count, int imageSize)
{
    // Images must stay unchanged while buttons use them.
    if (!images.empty() || count <= 0 || imageSize <= 0)
    {
        return false;
    }

    HDC screenDC = GetDC(nullptr);

    if (screenDC == nullptr)
    {
        return false;
    }

    HDC memoryDC = CreateCompatibleDC(screenDC);

    if (memoryDC == nullptr)
    {
        ReleaseDC(nullptr, screenDC);
        return false;
    }

    const COLORREF colors[]{
        RGB(210, 235, 255),
        RGB(215, 245, 220),
        RGB(255, 230, 200),
        RGB(235, 220, 255)
    };

    HGDIOBJ oldFont = SelectObject(
        memoryDC,
        GetStockObject(DEFAULT_GUI_FONT)
    );

    SetBkMode(memoryDC, TRANSPARENT);
    SetTextColor(memoryDC, RGB(25, 25, 25));

    bool success = true;

    for (int index = 0; index < count; ++index)
    {
        HBITMAP bitmap = CreateCompatibleBitmap(
            screenDC,
            imageSize,
            imageSize
        );

        if (bitmap == nullptr)
        {
            success = false;
            break;
        }

        HGDIOBJ oldBitmap = SelectObject(memoryDC, bitmap);

        if (oldBitmap == nullptr || oldBitmap == HGDI_ERROR)
        {
            DeleteObject(bitmap);
            success = false;
            break;
        }

        RECT rect{ 0, 0, imageSize, imageSize };

        SetDCBrushColor(memoryDC, colors[index % 4]);

        FillRect(
            memoryDC,
            &rect,
            static_cast<HBRUSH>(GetStockObject(DC_BRUSH))
        );

        FrameRect(
            memoryDC,
            &rect,
            static_cast<HBRUSH>(GetStockObject(GRAY_BRUSH))
        );

        const std::wstring label = std::to_wstring(index + 1);

        DrawTextW(
            memoryDC,
            label.c_str(),
            -1,
            &rect,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE
        );

        SelectObject(memoryDC, oldBitmap);
        images.push_back(bitmap);
    }

    SelectObject(memoryDC, oldFont);
    DeleteDC(memoryDC);
    ReleaseDC(nullptr, screenDC);

    if (!success)
    {
        Clear();
    }

    return success;
}