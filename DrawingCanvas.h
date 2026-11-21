#pragma once

#include <Windows.h>
#include <vector>

enum class DrawingTool
{
    None,
    Point,
    Line,
    Rectangle,
    Ellipse
};

class DrawingCanvas
{
public:
    void SetTool(DrawingTool tool);

    void SetColor(COLORREF color);
    void SetThickLine(bool enabled);
    bool ToggleFill();

    void Undo();
    void Clear();

    bool Begin(int x, int y);
    bool Update(int x, int y);
    bool End(int x, int y);
    bool Cancel();

    void Paint(HDC dc) const;

private:
    struct Figure
    {
        DrawingTool tool = DrawingTool::None;
        POINT start{};
        POINT end{};

        COLORREF color = RGB(30, 30, 30);
        int lineWidth = 1;
        bool filled = false;
    };

    static void DrawFigure(HDC dc, const Figure& figure);

    DrawingTool currentTool = DrawingTool::None;

    COLORREF currentColor = RGB(30, 30, 30);
    int currentLineWidth = 1;
    bool currentFill = false;

    std::vector<Figure> figures;

    Figure preview{};
    bool drawing = false;
};