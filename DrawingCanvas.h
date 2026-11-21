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
    };

    static void DrawFigure(HDC dc, const Figure& figure);

    DrawingTool currentTool = DrawingTool::None;
    std::vector<Figure> figures;

    Figure preview{};
    bool drawing = false;
};