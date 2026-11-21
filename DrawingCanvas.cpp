#include "framework.h"
#include "DrawingCanvas.h"

#include <algorithm>

void DrawingCanvas::SetTool(DrawingTool tool)
{
    Cancel();
    currentTool = tool;
}

bool DrawingCanvas::Begin(int x, int y)
{
    if (currentTool == DrawingTool::None)
    {
        return false;
    }

    preview = Figure{
        currentTool,
        POINT{ x, y },
        POINT{ x, y }
    };

    drawing = true;
    return true;
}

bool DrawingCanvas::Update(int x, int y)
{
    if (!drawing)
    {
        return false;
    }

    preview.end = POINT{ x, y };
    return true;
}

bool DrawingCanvas::End(int x, int y)
{
    if (!Update(x, y))
    {
        return false;
    }

    figures.push_back(preview);
    drawing = false;
    return true;
}

bool DrawingCanvas::Cancel()
{
    const bool wasDrawing = drawing;
    drawing = false;
    return wasDrawing;
}

void DrawingCanvas::DrawFigure(HDC dc, const Figure& figure)
{
    const int left =
        (std::min)(figure.start.x, figure.end.x);

    const int top =
        (std::min)(figure.start.y, figure.end.y);

    const int right =
        (std::max)(figure.start.x, figure.end.x);

    const int bottom =
        (std::max)(figure.start.y, figure.end.y);

    switch (figure.tool)
    {
    case DrawingTool::Point:
        MoveToEx(
            dc,
            figure.start.x - 3,
            figure.start.y,
            nullptr
        );

        LineTo(dc, figure.start.x + 4, figure.start.y);

        MoveToEx(
            dc,
            figure.start.x,
            figure.start.y - 3,
            nullptr
        );

        LineTo(dc, figure.start.x, figure.start.y + 4);
        break;

    case DrawingTool::Line:
        MoveToEx(dc, figure.start.x, figure.start.y, nullptr);
        LineTo(dc, figure.end.x, figure.end.y);
        break;

    case DrawingTool::Rectangle:
        ::Rectangle(dc, left, top, right, bottom);
        break;

    case DrawingTool::Ellipse:
        ::Ellipse(dc, left, top, right, bottom);
        break;

    case DrawingTool::None:
        break;
    }
}

void DrawingCanvas::Paint(HDC dc) const
{
    HGDIOBJ oldPen = SelectObject(
        dc,
        GetStockObject(DC_PEN)
    );

    HGDIOBJ oldBrush = SelectObject(
        dc,
        GetStockObject(HOLLOW_BRUSH)
    );

    const COLORREF oldColor =
        SetDCPenColor(dc, RGB(30, 30, 30));

    for (const Figure& figure : figures)
    {
        DrawFigure(dc, figure);
    }

    if (drawing)
    {
        SetDCPenColor(dc, RGB(0, 120, 215));
        DrawFigure(dc, preview);
    }

    SetDCPenColor(dc, oldColor);
    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);
}