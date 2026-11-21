# Діаграма include-залежностей

Стрілка спрямована від файлу, який виконує #include,
до заголовка, який він підключає.

```mermaid
flowchart TB
    subgraph Applications["Головні файли програм"]
        Main["main.cpp — графічний редактор"]
        Reuse["ToolWinReuse/main.cpp — лічильник"]
    end

    subgraph Sources["Реалізації модулів"]
        ToolCpp["ToolWin.cpp"]
        CanvasCpp["DrawingCanvas.cpp"]
        ImagesCpp["ToolImages.cpp"]
    end

    subgraph Headers["Інтерфейси модулів"]
        ToolH["ToolWin.h"]
        CanvasH["DrawingCanvas.h"]
        ImagesH["ToolImages.h"]
    end

    subgraph Support["Заголовки демонстраційного проєкту"]
        Framework["framework.h"]
        Target["targetver.h"]
        AppH["ToolWinDemo.h"]
        Resources["Resource.h"]
    end

    Main --> ToolH
    Main --> CanvasH
    Main --> ImagesH
    Main --> Framework
    Main --> AppH

    Reuse --> ToolH

    ToolCpp --> ToolH

    CanvasCpp --> CanvasH
    CanvasCpp --> Framework

    ImagesCpp --> ImagesH
    ImagesCpp --> Framework

    Framework --> Target
    AppH --> Resources
```

## Пояснення

- Обидві програми підключають один і той самий ToolWin.h.
- Обидва проєкти компілюють той самий ToolWin.cpp.
- Файли .cpp компілюються окремо, а не підключаються через #include.
- ToolWin.cpp не підключає framework.h та не залежить
  від заголовків графічного редактора.
- DrawingCanvas і ToolImages використовуються лише
  демонстраційним графічним редактором.
- Діаграма показує локальні залежності вихідного коду C++.
  Стандартні заголовки C++ та заголовки Windows SDK опущено.
- Ресурсний файл ToolWinDemo.rc компілюється окремо
  і також використовує Resource.h.