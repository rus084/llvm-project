// RUN: %clang_cc1 -std=c++20 -fsyntax-only -verify %s
#include "Inputs/std-coroutine.h"
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
[[coro::noalloc]] task work(int &, long) { co_return; }
[[coro::noalloc]] task empty() { co_return; }
task ordinary() { co_return; }
void run(void *storage, bool *occupied, int &value, char *bad_flag) {
  auto a = __builtin_coro_invoke(work, storage, occupied, value, short(2));
  auto b = __builtin_coro_invoke(empty, storage, occupied);
  __builtin_coro_invoke(ordinary, storage, occupied); // expected-error {{frame layout requires a noalloc coroutine}}
  __builtin_coro_invoke(work, storage, occupied, value); // expected-error {{incorrect number of arguments to noalloc coroutine}}
  __builtin_coro_invoke(work, storage, occupied, 1, 2); // expected-error {{non-const lvalue reference to type 'int' cannot bind to a temporary of type 'int'}}
  __builtin_coro_invoke(empty, 17, occupied); // expected-error {{cannot initialize a parameter of type 'void *' with an rvalue of type 'int'}}
  __builtin_coro_invoke(empty, storage, bad_flag); // expected-error {{cannot initialize a parameter of type 'bool *' with an lvalue of type 'char *'}}
}
template<auto F, class... Args>
auto invoke(void *storage, bool *occupied, Args&&... args) {
  return __builtin_coro_invoke(F, storage, occupied, static_cast<Args&&>(args)...);
}
task dependent(void *storage, bool *occupied, int &value) {
  return invoke<work>(storage, occupied, value, 3);
}
