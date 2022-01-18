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

// clang-format off
#include "base/Macros.h"
// clang-format on

#include <functional>
#if (CC_PLATFORM != CC_PLATFORM_NX)
#include <uv.h>
#endif
namespace cc {

class ReadWriteLock final {
public:
    ReadWriteLock() {
	#if (CC_PLATFORM != CC_PLATFORM_NX)
        uv_rwlock_init(&_lock);
	#endif
    };
    ~ReadWriteLock() {
	#if (CC_PLATFORM != CC_PLATFORM_NX)
        uv_rwlock_destroy(&_lock);
	#endif
    }

    template <typename Function, typename... Args>
    auto lockRead(Function &&func, Args &&...args) noexcept -> decltype(func(std::forward<Args>(args)...));

    template <typename Function, typename... Args>
    auto lockWrite(Function &&func, Args &&...args) noexcept -> decltype(func(std::forward<Args>(args)...));

private:
	#if (CC_PLATFORM != CC_PLATFORM_NX)
    uv_rwlock_t _lock;
	#endif

    struct Defer final {
        explicit Defer(std::function<void()> &&f) : fn(f) {}
        ~Defer() {
            fn();
        }
        std::function<void()> fn;
    };
};

template <typename Function, typename... Args>
auto ReadWriteLock::lockRead(Function &&func, Args &&...args) noexcept -> decltype(func(std::forward<Args>(args)...)) {
	#if (CC_PLATFORM != CC_PLATFORM_NX)
    uv_rwlock_rdlock(&_lock);
    auto defer = Defer([&]() {
        uv_rwlock_rdunlock(&_lock);
    });
	#endif
    return func(std::forward<Args>(args)...);
}

template <typename Function, typename... Args>
auto ReadWriteLock::lockWrite(Function &&func, Args &&...args) noexcept -> decltype(func(std::forward<Args>(args)...)) {
	#if (CC_PLATFORM != CC_PLATFORM_NX)
    uv_rwlock_wrlock(&_lock);
    auto defer = Defer([&]() {
        uv_rwlock_wrunlock(&_lock);
    });
	#endif
    return func(std::forward<Args>(args)...);
}

} // namespace cc
