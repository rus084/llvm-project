// RUN: %clang_cc1 -std=c++20 -emit-module-interface -verify -o %t.pcm %s
module;
#include "Inputs/std-coroutine.h"
export module noalloc;
struct task {
  struct promise_type {
    task get_return_object();
    std::suspend_always initial_suspend() noexcept;
    std::suspend_always final_suspend() noexcept;
    void return_void();
    void unhandled_exception();
  };
};
[[coro::noalloc]] task work() { co_return; }
constexpr auto bytes = __builtin_coro_frame_size(work); // expected-error {{noalloc frame layout does not support PCH or modules}}
