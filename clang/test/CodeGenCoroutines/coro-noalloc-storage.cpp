// RUN: %clang_cc1 -std=c++20 -triple x86_64-unknown-linux-gnu -emit-llvm -disable-llvm-passes -o - %s | FileCheck %s

#include "Inputs/coroutine.h"
struct task {
  struct promise_type {
    task get_return_object();
    std::suspend_always initial_suspend() noexcept;
    std::suspend_always final_suspend() noexcept;
    void return_void();
    void unhandled_exception();
    static void *operator new(__SIZE_TYPE__) = delete;
    static void operator delete(void *) = delete;
  };
};

// CHECK-LABEL: define internal void @_Z4workRiPi(ptr {{.*}}%__coro_storage, ptr {{.*}}%__coro_occupied, ptr {{.*}}%value, ptr {{.*}}%data)
// CHECK-NOT: @llvm.coro.alloc
// CHECK-NOT: @_Zn
// CHECK-NOT: @_Zd
// CHECK-LABEL: define internal void @_Z4workRiPi.destroy(
// CHECK: store i8 0, ptr {{.*}}, align 1
// CHECK-NOT: @llvm.coro.free
[[coro::noalloc]] __attribute__((noinline, used)) task work(int &value, int *data) {
  co_await std::suspend_always{};
  *data = ++value;
}
constexpr auto frame_bytes = __builtin_coro_frame_size(work);
