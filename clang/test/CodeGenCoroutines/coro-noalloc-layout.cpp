// RUN: %clang_cc1 -std=c++20 -triple x86_64-unknown-linux-gnu -emit-llvm -O0 -o - %s | FileCheck %s
// RUN: %clang_cc1 -std=c++20 -triple x86_64-unknown-linux-gnu -emit-llvm -O2 -o - %s | FileCheck %s
// RUN: %clang_cc1 -std=c++20 -triple thumbv6m-unknown-none-eabi -emit-llvm -O0 -o - %s | FileCheck %s

#include "Inputs/coroutine.h"
struct task {
  struct promise_type {
    task get_return_object();
    std::suspend_always initial_suspend() noexcept;
    std::suspend_always final_suspend() noexcept;
    void return_void();
    void unhandled_exception();
  };
};
void consume(void *);
[[coro::noalloc]] __attribute__((noinline, used)) task layout(int x) {
  alignas(64) char data[64];
  data[0] = x;
  co_await std::suspend_always{};
  consume(data);
}
extern "C" {
extern const unsigned long long frame_size = __builtin_coro_frame_size(layout);
extern const unsigned long long frame_align = __builtin_coro_frame_align(layout);
alignas(__builtin_coro_frame_align(layout))
unsigned char storage[__builtin_coro_frame_size(layout)];
}
// CHECK-DAG: @{{.*}}frame_size = {{.*}}constant i64 [[SIZE:[0-9]+]]
// CHECK-DAG: @{{.*}}frame_align = {{.*}}constant i64 [[ALIGN:[0-9]+]]
// CHECK-DAG: @storage = {{.*}}global [{{[0-9]+}} x i8] zeroinitializer, align [[ALIGN]]
// CHECK: define internal {{.*}} @_Z6layouti.resume(ptr {{.*}}align [[ALIGN]] dereferenceable([[SIZE]])
// CHECK-NOT: @_Zn
// CHECK-NOT: @_Zd
