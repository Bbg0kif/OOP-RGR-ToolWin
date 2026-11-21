#include "framework.h"
#include "DrawingCanvas.h"

#include <algorithm>

void DrawingCanvas::SetTool(DrawingTool tool)
{
    Cancel();
    currentTool = tool;
}

void DrawingCanvas::SetColor(COLORREF color)
{
    currentColor = color;
}

void DrawingCanvas::SetThickLine(bool enabled)
{
    currentLineWidth = enabled ? 3 : 1;
}

bool DrawingCanvas::ToggleFill()
{
    currentFill = !currentFill;
    return currentFill;
}

void DrawingCanvas::Undo()
{
    Cancel();

    if (!figures.empty())
    {
        figures.pop_back();
    }
}

void DrawingCanvas::Clear()
{
    Cancel();
    figures.clear();
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
    POINT{ x, y },
    currentColor,
    currentLineWidth,
    currentFill
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
    HPEN pen = CreatePen(
        PS_SOLID,
        figure.lineWidth,
        figure.color
    );

    if (pen == nullptr)
    {
        return;
    }

    HBRUSH brush = figure.filled
        ? CreateSolidBrush(figure.color)
        : static_cast<HBRUSH>(GetStockObject(HOLLOW_BRUSH));

    if (brush == nullptr)
    {
        DeleteObject(pen);
        return;
    }

    HGDIOBJ oldPen = SelectObject(dc, pen);
    HGDIOBJ oldBrush = SelectObject(dc, brush);

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

    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);

    if (figure.filled)
    {
        DeleteObject(brush);
    }

    DeleteObject(pen);
    }
}

void DrawingCanvas::Paint(HDC dc) const
{
    for (const Figure& figure : figures)
    {
        DrawFigure(dc, figure);
    }

    if (drawing)
    {
        DrawFigure(dc, preview);
    }
}