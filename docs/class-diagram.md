# Діаграма класів

```mermaid
classDiagram
    direction TB

    class MyToolWinClass {
        <<abstract>>
        -HWND window
        -int buttonCount
        -int selectedMode
        -vector modeButtons
        -vector buttonHints
        -vector buttonImages
        +Create(instance, owner, nx, ny, buttonSize) bool
        +SetHints(hints) bool
        +SetImages(images) bool
        +SetModeButtons(indices) bool
        +Show() void
        +Close() void
        +GetHandle() HWND
        #OnToolClick(buttonIndex) void*
        -IsModeButton(index) bool
        -UpdateModeButtons() void
    }

    class DemoToolWin {
        +DemoToolWin()
        #OnToolClick(buttonIndex) void
    }

    class CounterToolWin {
        -long long value
        +CounterToolWin()
        #OnToolClick(buttonIndex) void
    }

    class DrawingCanvas {
        -DrawingTool currentTool
        -COLORREF currentColor
        -int currentLineWidth
        -bool currentFill
        -vector figures
        -Figure preview
        -bool drawing
        +SetTool(tool) void
        +SetColor(color) void
        +SetThickLine(enabled) void
        +ToggleFill() bool
        +Undo() void
        +Clear() void
        +Begin(x, y) bool
        +Update(x, y) bool
        +End(x, y) bool
        +Cancel() bool
        +Paint(dc) void
    }

    class Figure {
        <<struct>>
        +DrawingTool tool
        +POINT start
        +POINT end
        +COLORREF color
        +int lineWidth
        +bool filled
    }

    class ToolImages {
        -vector images
        +Create(count, imageSize) bool
        +GetImages() vector
        -Clear() void
    }

    MyToolWinClass <|-- DemoToolWin
    MyToolWinClass <|-- CounterToolWin
    DemoToolWin ..> DrawingCanvas : викликає методи
    DrawingCanvas *-- Figure : зберігає фігури та попередній перегляд
```

## Пояснення

- MyToolWinClass — абстрактний клас багаторазово використовуваного
  модуля ToolWin.
- DemoToolWin і CounterToolWin успадковують його та визначають
  власні дії кнопок через OnToolClick().
- DemoToolWin звертається до окремого об’єкта DrawingCanvas.
  Поля DrawingCanvas у самому DemoToolWin немає.
- Figure — вкладена структура класу DrawingCanvas.
  Завершені фігури та попередній перегляд зберігаються за значенням.
- ToolImages створює зображення і володіє ними. У головному файлі
  програми їхні дескриптори передаються до ToolWin через SetImages().
- ToolWin не залежить від класів DrawingCanvas і ToolImages.

Позначення:
- «+» — public;
- «−» — private;
- «#» — protected;
- порожній трикутник — успадкування;
- пунктирна стрілка — залежність;
- зафарбований ромб — композиція.

Для читабельності на діаграмі скорочено типи контейнерів
і не показано службові обробники повідомлень, деструктори
та заборонені операції копіювання.