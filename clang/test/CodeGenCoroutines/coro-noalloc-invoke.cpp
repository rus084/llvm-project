// RUN: %clang_cc1 -std=c++20 -triple x86_64-unknown-linux-gnu -emit-llvm -O0 -o - %s | FileCheck %s
#include "Inputs/coroutine.h"
struct task {
  task(task &&);
  ~task();
  struct promise_type {
    task get_return_object();
    std::suspend_always initial_suspend() noexcept;
    std::suspend_always final_suspend() noexcept;
    void return_void();
    void unhandled_exception();
  };
};
[[coro::noalloc]] __attribute__((noinline, used)) task work(int &x, long y) {
  co_await std::suspend_always{};
  x += y;
}
// CHECK-LABEL: define {{.*}} @_Z6launchPvPbRi(
// CHECK: call void @_Z4workRil(ptr {{[^,]*}}sret(%struct.task) align {{[0-9]+}} {{.*}}, ptr {{.*}}, ptr {{.*}}, ptr {{.*}}, i64 {{.*}})
task launch(void *storage, bool *occupied, int &x) {
  return __builtin_coro_invoke(work, storage, occupied, x, short(2));
}
// CHECK: define internal void @_Z4workRil(ptr {{.*}}, ptr {{.*}}, ptr {{.*}}, ptr {{.*}}, i64 {{.*}})
