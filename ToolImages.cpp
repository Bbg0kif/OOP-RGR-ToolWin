#include "framework.h"
#include "ToolImages.h"

#include <string>

namespace
{
    void DrawToolSymbol(HDC dc, int index, int imageSize)
    {
        const int savedState = SaveDC(dc);

        if (savedState == 0)
        {
            return;
        }

        const COLORREF toolColors[]{
            RGB(30, 30, 30),
            RGB(210, 40, 40),
            RGB(30, 150, 70),
            RGB(35, 90, 210)
        };

        COLORREF color = RGB(30, 30, 30);

        if (index >= 4 && index <= 7)
        {
            color = toolColors[index - 4];
        }

        const int penWidth = index == 9 ? 3 : 1;
        HPEN pen = CreatePen(PS_SOLID, penWidth, color);

        if (pen == nullptr)
        {
            RestoreDC(dc, savedState);
            return;
        }

        SelectObject(dc, pen);
        SelectObject(dc, GetStockObject(HOLLOW_BRUSH));

        // Coordinates are specified for a 28 x 28 image.
        const auto position = [imageSize](int value)
            {
                return MulDiv(value, imageSize, 28);
            };

        const auto line = [&](int x1, int y1, int x2, int y2)
            {
                MoveToEx(dc, position(x1), position(y1), nullptr);
                LineTo(dc, position(x2), position(y2));
            };

        switch (index)
        {
        case 0:
            // Point.
            line(8, 14, 21, 14);
            line(14, 8, 14, 21);
            break;

        case 1:
            // Line.
            line(6, 21, 22, 6);
            break;

        case 2:
            // Rectangle.
            Rectangle(
                dc,
                position(5), position(7),
                position(23), position(21)
            );
            break;

        case 3:
            // Ellipse.
            Ellipse(
                dc,
                position(5), position(7),
                position(23), position(21)
            );
            break;

        case 4:
        case 5:
        case 6:
        case 7:
            // Color sample.
            SelectObject(dc, GetStockObject(DC_BRUSH));
            SetDCBrushColor(dc, color);

            Rectangle(
                dc,
                position(6), position(6),
                position(22), position(22)
            );
            break;

        case 8:
        case 9:
            // Thin or thick line.
            line(5, 14, 23, 14);
            break;

        case 10:
            // Filled shape.
            SelectObject(dc, GetStockObject(DC_BRUSH));
            SetDCBrushColor(dc, RGB(100, 170, 235));

            Rectangle(
                dc,
                position(6), position(6),
                position(22), position(22)
            );

            line(7, 20, 20, 7);
            break;

        case 11:
            // Undo: arrow pointing backwards.
            line(7, 12, 22, 12);
            line(22, 12, 22, 21);
            line(7, 12, 12, 7);
            line(7, 12, 12, 17);
            break;

        case 12:
            // Clear.
            line(7, 7, 22, 22);
            line(7, 21, 22, 6);
            break;

        case 13:
        {
            // Help.
            RECT textRect{ 0, 0, imageSize, imageSize };

            SetBkMode(dc, TRANSPARENT);
            SetTextColor(dc, RGB(35, 90, 210));

            DrawTextW(
                dc,
                L"?",
                -1,
                &textRect,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE
            );
            break;
        }

        default:
        {
            // Fallback for additional buttons.
            RECT textRect{ 0, 0, imageSize, imageSize };
            const std::wstring label = std::to_wstring(index + 1);

            DrawTextW(
                dc,
                label.c_str(),
                -1,
                &textRect,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE
            );
            break;
        }
        }

        RestoreDC(dc, savedState);
        DeleteObject(pen);
    }
}

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

        SetDCBrushColor(memoryDC, RGB(245, 245, 245));

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

        DrawToolSymbol(memoryDC, index, imageSize);

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