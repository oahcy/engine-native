/****************************************************************************
 Copyright (c) 2019-2021 Xiamen Yaji Software Co., Ltd.

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

#include "GLES3Std.h"

#include "GLES3Commands.h"
#include "GLES3Device.h"
#include "GLES3RenderPass.h"
#include "gfx-base/GFXDef-common.h"

namespace cc {
namespace gfx {

GLES3RenderPass::GLES3RenderPass() {
    _typedID = generateObjectID<decltype(this)>();
}

GLES3RenderPass::~GLES3RenderPass() {
    destroy();
}

void GLES3RenderPass::doInit(const RenderPassInfo & /*info*/) {
    _gpuRenderPass                         = CC_NEW(GLES3GPURenderPass);
    _gpuRenderPass->colorAttachments       = _colorAttachments;
    _gpuRenderPass->depthStencilAttachment = _depthStencilAttachment;
    _gpuRenderPass->subpasses              = _subpasses;

    // assign a dummy subpass if not specified
    if (_gpuRenderPass->subpasses.empty()) {
        _gpuRenderPass->subpasses.emplace_back();
        auto &subpass = _gpuRenderPass->subpasses.back();
        subpass.colors.resize(_colorAttachments.size());
        for (uint32_t i = 0U; i < _colorAttachments.size(); ++i) {
            subpass.colors[i] = i;
        }
        if (_depthStencilAttachment.format != Format::UNKNOWN) {
            subpass.depthStencil = utils::toUint(_colorAttachments.size());
        }
    } else {
        // unify depth stencil index
        uint32_t colorCount = utils::toUint(_gpuRenderPass->colorAttachments.size());
        for (auto &subpass : _gpuRenderPass->subpasses) {
            if (subpass.depthStencil != INVALID_BINDING && subpass.depthStencil > colorCount) {
                subpass.depthStencil = colorCount;
            }
            if (subpass.depthStencilResolve != INVALID_BINDING && subpass.depthStencilResolve > colorCount) {
                subpass.depthStencilResolve = colorCount;
            }
        }
    }

    cmdFuncGLES3CreateRenderPass(GLES3Device::getInstance(), _gpuRenderPass);
}

void GLES3RenderPass::doDestroy() {
    if (_gpuRenderPass) {
        cmdFuncGLES3DestroyRenderPass(GLES3Device::getInstance(), _gpuRenderPass);
        CC_DELETE(_gpuRenderPass);
        _gpuRenderPass = nullptr;
    }
}

} // namespace gfx
} // namespace cc
