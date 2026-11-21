#pragma once

#include <Windows.h>
#include <vector>

class ToolImages
{
public:
    ToolImages() = default;
    ~ToolImages();

    ToolImages(const ToolImages&) = delete;
    ToolImages& operator=(const ToolImages&) = delete;

    bool Create(int count, int imageSize);

    const std::vector<HBITMAP>& GetImages() const;

private:
    void Clear();

    std::vector<HBITMAP> images;
};