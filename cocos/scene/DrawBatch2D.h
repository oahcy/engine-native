/****************************************************************************
 Copyright (c) 2021 Xiamen Yaji Software Co., Ltd.
 
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
 THE SOFTWARE.
 ****************************************************************************/

#pragma once

#include <vector>
#include "renderer/gfx-base/GFXDescriptorSet.h"
#include "renderer/gfx-base/GFXInputAssembler.h"

namespace cc {
namespace scene {

class Pass;

struct DrawCall final {
    gfx::Buffer *         bufferView{nullptr};
    gfx::DescriptorSet *  descriptorSet{nullptr};
    std::vector<uint32_t> dynamicOffsets;
    gfx::DrawInfo *       drawInfo;

    void setDynamicOffsets(uint32_t value) {
        dynamicOffsets.push_back(0);
        dynamicOffsets.push_back(value);
    }
};

struct DrawBatch2D final {
    uint32_t                   visFlags{0};
    gfx::DescriptorSet *       descriptorSet{nullptr};
    gfx::InputAssembler *      inputAssembler{nullptr};
    std::vector<Pass *>        passes;
    std::vector<gfx::Shader *> shaders;
    std::vector<DrawCall *>    drawCalls;

    void pushDrawCall(DrawCall *dc) {
        drawCalls.push_back(dc);
    }

    void clearDrawCalls(){
        drawCalls.clear();
    }
};

} // namespace scene
} // namespace cc
