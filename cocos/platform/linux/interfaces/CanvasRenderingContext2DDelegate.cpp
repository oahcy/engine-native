/****************************************************************************
 Copyright (c) 2018-2021 Xiamen Yaji Software Co., Ltd.

 http://www.cocos.com

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.                                                                    \
 ****************************************************************************/

#include "platform/linux/interfaces/CanvasRenderingContext2DDelegate.h"

namespace {
void fillRectWithColor(uint8_t *buf, uint32_t totalWidth, uint32_t totalHeight, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    assert(x + width <= totalWidth);
    assert(y + height <= totalHeight);

    uint32_t y0 = y;
    uint32_t y1 = y + height;
    uint8_t *p;
    for (uint32_t offsetY = y0; offsetY < y1; ++offsetY) {
        for (uint32_t offsetX = x; offsetX < (x + width); ++offsetX) {
            p    = buf + (totalWidth * offsetY + offsetX) * 4;
            *p++ = r;
            *p++ = g;
            *p++ = b;
            *p++ = a;
        }
    }
}
} // namespace

namespace cc {
    CanvasRenderingContext2DDelegate::CanvasRenderingContext2DDelegate() {};
    CanvasRenderingContext2DDelegate::~CanvasRenderingContext2DDelegate() {}

    void CanvasRenderingContext2DDelegate::CanvasRenderingContext2DDelegate::recreateBuffer(float w, float h) {}
    void CanvasRenderingContext2DDelegate::beginPath() {}
    void CanvasRenderingContext2DDelegate::closePath() {}
    void CanvasRenderingContext2DDelegate::moveTo(float x, float y) {}
    void CanvasRenderingContext2DDelegate::lineTo(float x, float y) {}
    void CanvasRenderingContext2DDelegate::stroke() {}
    void CanvasRenderingContext2DDelegate::saveContext() {}
    void CanvasRenderingContext2DDelegate::restoreContext() {}
    void CanvasRenderingContext2DDelegate::clearRect(float /*x*/, float /*y*/, float w, float h) {}
    void CanvasRenderingContext2DDelegate::fillRect(float x, float y, float w, float h) {}
    void CanvasRenderingContext2DDelegate::fillText(const std::string &text, float x, float y, float /*maxWidth*/) {}
    void CanvasRenderingContext2DDelegate::strokeText(const std::string &text, float /*x*/, float /*y*/, float /*maxWidth*/) const {};
    cc::CanvasRenderingContext2DDelegate::Size CanvasRenderingContext2DDelegate::measureText(const std::string &text) { return std::array<float, 2>{0,0};}
    void CanvasRenderingContext2DDelegate::updateFont(const std::string &fontName, float fontSize, bool bold, bool italic, bool oblique, bool smallCaps) {}
    void CanvasRenderingContext2DDelegate::setTextAlign(CanvasTextAlign align) {}
    void CanvasRenderingContext2DDelegate::setTextBaseline(CanvasTextBaseline baseline) {}
    void CanvasRenderingContext2DDelegate::setFillStyle(float r, float g, float b, float a) {}
    void CanvasRenderingContext2DDelegate::setStrokeStyle(float r, float g, float b, float a) {}
    void CanvasRenderingContext2DDelegate::setLineWidth(float lineWidth) {}
    const cc::Data &CanvasRenderingContext2DDelegate::getDataRef() const {}
    void CanvasRenderingContext2DDelegate::fill() {}
    void CanvasRenderingContext2DDelegate::setLineCap(const std::string &lineCap) {}
    void CanvasRenderingContext2DDelegate::setLineJoin(const std::string &lineCap) {}
    void CanvasRenderingContext2DDelegate::fillImageData(const Data &imageData, float imageWidth, float imageHeight, float offsetX, float offsetY) {}
    void CanvasRenderingContext2DDelegate::strokeText(const std::string &text, float /*x*/, float /*y*/, float /*maxWidth*/) {}
    void CanvasRenderingContext2DDelegate::rect(float x, float y, float w, float h) {}
} // namespace cc