/****************************************************************************
 Copyright (c) 2020-2021 Xiamen Yaji Software Co., Ltd.

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

#include "base/CoreStd.h"

#include "BufferValidator.h"
#include "CommandBufferValidator.h"
#include "DescriptorSetLayoutValidator.h"
#include "DescriptorSetValidator.h"
#include "DeviceValidator.h"
#include "FramebufferValidator.h"
#include "InputAssemblerValidator.h"
#include "PipelineLayoutValidator.h"
#include "PipelineStateValidator.h"
#include "QueueValidator.h"
#include "RenderPassValidator.h"
#include "ShaderValidator.h"
#include "SwapchainValidator.h"
#include "TextureValidator.h"
#include "ValidationUtils.h"
#include "gfx-base/GFXSwapchain.h"

#include <cstring>

namespace cc {
namespace gfx {

DeviceValidator *DeviceValidator::instance = nullptr;

DeviceValidator *DeviceValidator::getInstance() {
    return DeviceValidator::instance;
}

DeviceValidator::DeviceValidator(Device *device) : Agent(device) {
    DeviceValidator::instance = this;
}

DeviceValidator::~DeviceValidator() {
    CC_SAFE_DELETE(_actor);
    DeviceValidator::instance = nullptr;
}

bool DeviceValidator::doInit(const DeviceInfo &info) {
    if (!_actor->initialize(info)) {
        return false;
    }

    _api        = _actor->getGfxAPI();
    _deviceName = _actor->getDeviceName();
    _queue      = CC_NEW(QueueValidator(_actor->getQueue()));
    _cmdBuff    = CC_NEW(CommandBufferValidator(_actor->getCommandBuffer()));
    _renderer   = _actor->getRenderer();
    _vendor     = _actor->getVendor();
    _caps       = _actor->_caps;
    memcpy(_features.data(), _actor->_features.data(), static_cast<uint32_t>(Feature::COUNT) * sizeof(bool));

    static_cast<QueueValidator *>(_queue)->_inited          = true;
    static_cast<CommandBufferValidator *>(_cmdBuff)->_queue = _queue;
    static_cast<CommandBufferValidator *>(_cmdBuff)->initValidator();

    DeviceResourceTracker<CommandBuffer>::push(_cmdBuff);
    DeviceResourceTracker<Queue>::push(_queue);

    CC_LOG_INFO("Device validator enabled.");

    return true;
}

void DeviceValidator::doDestroy() {
    if (_cmdBuff) {
        static_cast<CommandBufferValidator *>(_cmdBuff)->_actor = nullptr;
        CC_DELETE(_cmdBuff);
        _cmdBuff = nullptr;
    }
    if (_queue) {
        static_cast<QueueValidator *>(_queue)->_actor = nullptr;
        CC_DELETE(_queue);
        _queue = nullptr;
    }

    DeviceResourceTracker<CommandBuffer>::checkEmpty();
    DeviceResourceTracker<Queue>::checkEmpty();
    DeviceResourceTracker<Swapchain>::checkEmpty();
    DeviceResourceTracker<Buffer>::checkEmpty();
    DeviceResourceTracker<Texture>::checkEmpty();
    DeviceResourceTracker<Sampler>::checkEmpty();
    DeviceResourceTracker<Shader>::checkEmpty();
    DeviceResourceTracker<InputAssembler>::checkEmpty();
    DeviceResourceTracker<RenderPass>::checkEmpty();
    DeviceResourceTracker<Framebuffer>::checkEmpty();
    DeviceResourceTracker<DescriptorSet>::checkEmpty();
    DeviceResourceTracker<DescriptorSetLayout>::checkEmpty();
    DeviceResourceTracker<PipelineLayout>::checkEmpty();
    DeviceResourceTracker<PipelineState>::checkEmpty();

    _actor->destroy();
}

void DeviceValidator::acquire(Swapchain *const *swapchains, uint32_t count) {
    static vector<Swapchain *> swapchainActors;
    swapchainActors.resize(count);

    for (uint32_t i = 0U; i < count; ++i) {
        auto *swapchain    = static_cast<SwapchainValidator *>(swapchains[i]);
        swapchainActors[i] = swapchain->getActor();
    }

    if (_onAcquire) _onAcquire->execute();
    _actor->acquire(swapchainActors.data(), count);
}

void DeviceValidator::present() {
    _actor->present();

    ++_currentFrame;
}

CommandBuffer *DeviceValidator::createCommandBuffer(const CommandBufferInfo &info, bool hasAgent) {
    CommandBuffer *actor  = _actor->createCommandBuffer(info, hasAgent);
    CommandBuffer *result = CC_NEW(CommandBufferValidator(actor));
    DeviceResourceTracker<CommandBuffer>::push(result);
    return result;
}

Queue *DeviceValidator::createQueue() {
    Queue *actor  = _actor->createQueue();
    Queue *result = CC_NEW(QueueValidator(actor));
    DeviceResourceTracker<Queue>::push(result);
    return result;
}

Swapchain *DeviceValidator::createSwapchain() {
    Swapchain *actor  = _actor->createSwapchain();
    Swapchain *result = CC_NEW(SwapchainValidator(actor));
    DeviceResourceTracker<Swapchain>::push(result);
    return result;
}

Buffer *DeviceValidator::createBuffer() {
    Buffer *actor  = _actor->createBuffer();
    Buffer *result = CC_NEW(BufferValidator(actor));
    DeviceResourceTracker<Buffer>::push(result);
    return result;
}

Texture *DeviceValidator::createTexture() {
    Texture *actor  = _actor->createTexture();
    Texture *result = CC_NEW(TextureValidator(actor));
    DeviceResourceTracker<Texture>::push(result);
    return result;
}

Shader *DeviceValidator::createShader() {
    Shader *actor  = _actor->createShader();
    Shader *result = CC_NEW(ShaderValidator(actor));
    DeviceResourceTracker<Shader>::push(result);
    return result;
}

InputAssembler *DeviceValidator::createInputAssembler() {
    InputAssembler *actor  = _actor->createInputAssembler();
    InputAssembler *result = CC_NEW(InputAssemblerValidator(actor));
    DeviceResourceTracker<InputAssembler>::push(result);
    return result;
}

RenderPass *DeviceValidator::createRenderPass() {
    RenderPass *actor  = _actor->createRenderPass();
    RenderPass *result = CC_NEW(RenderPassValidator(actor));
    DeviceResourceTracker<RenderPass>::push(result);
    return result;
}

Framebuffer *DeviceValidator::createFramebuffer() {
    Framebuffer *actor  = _actor->createFramebuffer();
    Framebuffer *result = CC_NEW(FramebufferValidator(actor));
    DeviceResourceTracker<Framebuffer>::push(result);
    return result;
}

DescriptorSet *DeviceValidator::createDescriptorSet() {
    DescriptorSet *actor  = _actor->createDescriptorSet();
    DescriptorSet *result = CC_NEW(DescriptorSetValidator(actor));
    DeviceResourceTracker<DescriptorSet>::push(result);
    return result;
}

DescriptorSetLayout *DeviceValidator::createDescriptorSetLayout() {
    DescriptorSetLayout *actor  = _actor->createDescriptorSetLayout();
    DescriptorSetLayout *result = CC_NEW(DescriptorSetLayoutValidator(actor));
    DeviceResourceTracker<DescriptorSetLayout>::push(result);
    return result;
}

PipelineLayout *DeviceValidator::createPipelineLayout() {
    PipelineLayout *actor  = _actor->createPipelineLayout();
    PipelineLayout *result = CC_NEW(PipelineLayoutValidator(actor));
    DeviceResourceTracker<PipelineLayout>::push(result);
    return result;
}

PipelineState *DeviceValidator::createPipelineState() {
    PipelineState *actor  = _actor->createPipelineState();
    PipelineState *result = CC_NEW(PipelineStateValidator(actor));
    DeviceResourceTracker<PipelineState>::push(result);
    return result;
}

Sampler *DeviceValidator::createSampler(const SamplerInfo &info, uint32_t hash) {
    return _actor->createSampler(info, hash);
}

GlobalBarrier *DeviceValidator::createGlobalBarrier(const GlobalBarrierInfo &info, uint32_t hash) {
    return _actor->createGlobalBarrier(info, hash);
}

TextureBarrier *DeviceValidator::createTextureBarrier(const TextureBarrierInfo &info, uint32_t hash) {
    return _actor->createTextureBarrier(info, hash);
}

void DeviceValidator::copyBuffersToTexture(const uint8_t *const *buffers, Texture *dst, const BufferTextureCopy *regions, uint32_t count) {
    auto *textureValidator = static_cast<TextureValidator *>(dst);
    textureValidator->sanityCheck();

    /////////// execute ///////////

    _actor->copyBuffersToTexture(buffers, textureValidator->getActor(), regions, count);
}

void DeviceValidator::copyTextureToBuffers(Texture *src, uint8_t *const *buffers, const BufferTextureCopy *regions, uint32_t count) {
    auto *textureValidator = static_cast<TextureValidator *>(src);

    /////////// execute ///////////

    _actor->copyTextureToBuffers(textureValidator->getActor(), buffers, regions, count);
}

void DeviceValidator::flushCommands(CommandBuffer *const *cmdBuffs, uint32_t count) {
    if (!count) return;

    /////////// execute ///////////

    static vector<CommandBuffer *> cmdBuffActors;
    cmdBuffActors.resize(count);

    for (uint32_t i = 0U; i < count; ++i) {
        auto *cmdBuff             = static_cast<CommandBufferValidator *>(cmdBuffs[i]);
        cmdBuff->_commandsFlushed = true;
        cmdBuffActors[i]          = cmdBuff->getActor();
    }

    _actor->flushCommands(cmdBuffActors.data(), count);
}

} // namespace gfx
} // namespace cc
