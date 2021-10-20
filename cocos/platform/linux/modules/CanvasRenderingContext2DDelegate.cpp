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

#include "platform/linux/modules/CanvasRenderingContext2DDelegate.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_syswm.h"
#include "platform/linux/LinuxPlatform.h"

namespace {
#define RGB(r,g,b)          ((unsigned long)(((char)(r)|((unsigned long)((char)(g))<<8))|(((unsigned long)(char)(b))<<16)))
}

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
CanvasRenderingContext2DDelegate::CanvasRenderingContext2DDelegate() {
    // char *display_name = getenv("DISPLAY");  /* address of the X display.      */
    // _dis = XOpenDisplay(display_name);
    // _screen = DefaultScreen(_dis);
    // _gc = create_gc(_dis, win, 0);
    // XSync(display, False);
    
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    LinuxPlatform *platform = dynamic_cast<LinuxPlatform *>(BasePlatform::getPlatform());
    CCASSERT(platform != nullptr, "Platform pointer can't be null");
    SDL_GetWindowWMInfo(reinterpret_cast<SDL_Window*>(platform->getWindow()), &wmInfo);
    _dis = wmInfo.info.x11.display;
    _win = wmInfo.info.x11.window;
    XGCValues values;	
    _gc = XCreateGC(_dis, _win, 0, &values);
}

CanvasRenderingContext2DDelegate::~CanvasRenderingContext2DDelegate() {
    /* flush all pending requests to the X server. */
    //XFlush(display);
    //XCloseDisplay(display);
}

void CanvasRenderingContext2DDelegate::recreateBuffer(float w, float h) {
    _bufferWidth  = w;
    _bufferHeight = h;
    if (_bufferWidth < 1.0F || _bufferHeight < 1.0F) {
        return;
    }
}

void CanvasRenderingContext2DDelegate::beginPath() {
    // called: set_lineWidth() -> beginPath() -> moveTo() -> lineTo() -> stroke(), when draw line
    XSetLineAttributes(_dis, _gc, static_cast<int>(_lineWidth), LineSolid, _lineCap,_lineJoin);
    XSetForeground(_dis,_gc, RGB(255, 255, 255));
}

void CanvasRenderingContext2DDelegate::closePath() {
}

void CanvasRenderingContext2DDelegate::moveTo(float x, float y) {
    //MoveToEx(_DC, static_cast<int>(x), static_cast<int>(-(y - _bufferHeight - _fontSize)), nullptr);
    _x = x;
    _y = y;
}

void CanvasRenderingContext2DDelegate::lineTo(float x, float y) {
    //LineTo(_DC,  static_cast<int>(x),  static_cast<int>(-(y - _bufferHeight - _fontSize)));
    XDrawLine(_dis, _win, _gc, _x, _y, x, y);
}

void CanvasRenderingContext2DDelegate::stroke() {
}

void CanvasRenderingContext2DDelegate::saveContext() {
}

void CanvasRenderingContext2DDelegate::restoreContext() {
}

void CanvasRenderingContext2DDelegate::clearRect(float x, float y, float w, float h) {
    if (_bufferWidth < 1.0F || _bufferHeight < 1.0F) {
        return;
    }

    XClearArea(_dis, _win, x, y, w, h, False);
}

void CanvasRenderingContext2DDelegate::fillRect(float x, float y, float w, float h) {
    if (_bufferWidth < 1.0F || _bufferHeight < 1.0F) {
        return;
    }
    XFillRectangle(_dis, _win, _gc, x, y, w, h);
}

void CanvasRenderingContext2DDelegate::fillText(const std::string &text, float x, float y, float /*maxWidth*/) {
    if (text.empty() || _bufferWidth < 1.0F || _bufferHeight < 1.0F) {
        return;
    }
    XTextItem item{ const_cast<char*>(text.c_str()), static_cast<int>(text.length()), 0, None};
    XDrawText(_dis, _win, _gc, x, y, &item, 1);
}

void CanvasRenderingContext2DDelegate::strokeText(const std::string &text, float /*x*/, float /*y*/, float /*maxWidth*/) const {
    if (text.empty() || _bufferWidth < 1.0F || _bufferHeight < 1.0F) {
        return;
    }
}

CanvasRenderingContext2DDelegate::Size CanvasRenderingContext2DDelegate::measureText(const std::string &text) {
    if (text.empty())
        return std::array<float, 2>{0.0f, 0.0f};
    XFontStruct *fs = XLoadQueryFont(_dis, "cursor");
    assert(fs);
    int font_ascent = 0;
    int font_descent = 0;
    int direction = 0;
    XCharStruct overall;
    XQueryTextExtents(_dis, fs -> fid, text.c_str(), text.length(), &direction, &font_ascent, &font_descent, &overall);
    return std::array<float, 2>{static_cast<float>(overall.lbearing), 
                                static_cast<float>(overall.rbearing)};
}

void CanvasRenderingContext2DDelegate::updateFont(const std::string &fontName,
                                                  float              fontSize,
                                                  bool               bold,
                                                  bool /* italic */,
                                                  bool /* oblique */,
                                                  bool /* smallCaps */) {
    do {
        _fontName = fontName;
        _fontSize = static_cast<int>(fontSize);
        std::string fontPath;
        if (!_fontName.empty()) {
            // firstly, try to create font from ttf file
            const auto &fontInfoMap = getFontFamilyNameMap();
            auto        iter        = fontInfoMap.find(_fontName);
            if (iter != fontInfoMap.end()) {
                fontPath                = iter->second;
                std::string tmpFontPath = fontPath;
                size_t         nFindPos = tmpFontPath.rfind("/");
                tmpFontPath             = &tmpFontPath[nFindPos + 1];
                nFindPos                = tmpFontPath.rfind(".");
                // IDEA: draw ttf failed if font file name not equal font face name
                // for example: "DejaVuSansMono-Oblique" not equal "DejaVu Sans Mono"  when using DejaVuSansMono-Oblique.ttf
                _fontName = tmpFontPath.substr(0, nFindPos);
            } else {
                auto nFindPos = fontName.rfind("/");
                if (nFindPos != fontName.npos) {
                    if (fontName.length() == nFindPos + 1) {
                        _fontName = "";
                    } else {
                        _fontName = &_fontName[nFindPos + 1];
                    }
                }
            }
            //tFont.lfCharSet = DEFAULT_CHARSET;
            //strcpy_s(tFont.lfFaceName, LF_FACESIZE, _fontName.c_str());
        }
        XFontStruct *font = XLoadQueryFont(_dis, _fontName.c_str());
        /* If the font could not be loaded, revert to the "fixed" font. */
        if (!font) {
            font = XLoadQueryFont(_dis, "fixed");
        }
        XSetFont(_dis, _gc, font->fid);
    } while (false);
}

void CanvasRenderingContext2DDelegate::setTextAlign(CanvasTextAlign align) {
    _textAlign = align;
}

void CanvasRenderingContext2DDelegate::setTextBaseline(CanvasTextBaseline baseline) {
    _textBaseLine = baseline;
}

void CanvasRenderingContext2DDelegate::setFillStyle(float r, float g, float b, float a) {
    _fillStyle = {r, g, b, a};
}

void CanvasRenderingContext2DDelegate::setStrokeStyle(float r, float g, float b, float a) {
    _strokeStyle = {r, g, b, a};
}

void CanvasRenderingContext2DDelegate::setLineWidth(float lineWidth) {
    _lineWidth = lineWidth;
}

const cc::Data &CanvasRenderingContext2DDelegate::getDataRef() const {
    static cc::Data data;
    int size = 0;
    unsigned char* buf = reinterpret_cast<unsigned char*>(XFetchBuffer(_dis, &size, 0));
    data.copy(static_cast<const unsigned char*>(buf), size);
    return data;
}

void CanvasRenderingContext2DDelegate::removeCustomFont() {
    XFreeFont(_dis, None);
}

// x, y offset value
int CanvasRenderingContext2DDelegate::drawText(const std::string &text, int x, int y) {
    XTextItem item{  const_cast<char*>(text.c_str()), static_cast<int>(text.length()), 0, None};
    return XDrawText(_dis, _win, _gc, x, y, &item, 1);
}

CanvasRenderingContext2DDelegate::Size CanvasRenderingContext2DDelegate::sizeWithText(const wchar_t *pszText, int nLen) {
    // if (text.empty())
    //     return std::array<float, 2>{0.0f, 0.0f};
    // XFontStruct *fs = XLoadQueryFont(dpy, "cursor");
    // assert(fs);
    // int font_ascent = 0;
    // int font_descent = 0;
    // XCharStruct overall;
    // XQueryTextExtents(_dis, fs -> fid, text.c_str(), text.length(), nullptr, &font_ascent, &font_descent, &overall);
    // return std::array<float, 2>{static_cast<float>(overall.lbearing), 
    //                             static_cast<float>(overall.rbearing)};
    return std::array<float, 2>{0.0F,0.0F};
}

void CanvasRenderingContext2DDelegate::prepareBitmap(int nWidth, int nHeight) {

}

void CanvasRenderingContext2DDelegate::deleteBitmap() {

}

void CanvasRenderingContext2DDelegate::fillTextureData() {
}

std::array<float, 2> CanvasRenderingContext2DDelegate::convertDrawPoint(Point point, const std::string &text) {
    return std::array<float, 2>{0,0};
}

void CanvasRenderingContext2DDelegate::fill() {
}

void CanvasRenderingContext2DDelegate::setLineCap(const std::string & lineCap ) {
    _lineCap = LineSolid;
}

void CanvasRenderingContext2DDelegate::setLineJoin(const std::string & lineJoin ) {
    _lineJoin = JoinRound;
}

void CanvasRenderingContext2DDelegate::fillImageData(const Data & /* imageData */,
                                                     float /* imageWidth */,
                                                     float /* imageHeight */,
                                                     float /* offsetX */,
                                                     float /* offsetY */) {
    //XCreateImage(display, visual, DefaultDepth(display,DefaultScreen(display)), ZPixmap, 0, image32, width, height, 32, 0);
    //XPutImage(dpy, w, gc, image, 0, 0, 50, 60, 40, 30);
}

void CanvasRenderingContext2DDelegate::strokeText(const std::string & /* text */,
                                                  float /* x */,
                                                  float /* y */,
                                                  float /* maxWidth */) {
}

void CanvasRenderingContext2DDelegate::rect(float /* x */,
                                            float /* y */,
                                            float /* w */,
                                            float /* h */) {
}

} // namespace cc