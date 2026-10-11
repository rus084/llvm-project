// RUN: %clang_cc1 -std=c++20 -ast-dump -ast-dump-filter noalloc_definition %s | FileCheck %s

#include "../SemaCXX/Inputs/std-coroutine.h"
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
[[coro::noalloc]] task noalloc_definition();
task noalloc_definition() { co_return; }

// CHECK: FunctionDecl {{.*}} noalloc_definition 'task ()'
// CHECK: CoroNoAllocAttr
// CHECK: FunctionDecl {{.*}} noalloc_definition 'task ()'
// CHECK: CoroutineBodyStmt
// CHECK-NOT: operator new
// CHECK-NOT: operator delete
// CHECK: CoroNoAllocAttr {{.*}} Inherited
