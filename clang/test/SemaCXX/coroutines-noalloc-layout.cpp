// RUN: %clang_cc1 -std=c++20 -fsyntax-only -verify %s
// RUN: %clang_cc1 -std=c++20 -fsyntax-only -verify %s %s

#include "Inputs/std-coroutine.h"
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
[[coro::noalloc]] task work(int x) {
  alignas(64) char data[64];
  data[0] = x;
  co_await std::suspend_always{};
  consume(data);
}
constexpr auto N = __builtin_coro_frame_size(work);
constexpr auto A = __builtin_coro_frame_align(work);
static_assert(N > 0);
static_assert(A >= 64);
static_assert(N % A == 0);
static_assert(N == __builtin_coro_frame_size(work));
alignas(A) unsigned char storage[N];
template<auto F> struct frame_type {
  alignas(__builtin_coro_frame_align(F))
  char data[__builtin_coro_frame_size(F)];
};
frame_type<work> dependent_frame;
static_assert(sizeof(dependent_frame) == N);
static_assert(alignof(frame_type<work>) == A);

[[coro::noalloc]] task later();
constexpr auto missing = __builtin_coro_frame_size(later); // expected-error {{coroutine definition must be visible before querying its frame}}
task regular() { co_return; }
constexpr auto ordinary = __builtin_coro_frame_size(regular); // expected-error {{frame layout requires a noalloc coroutine}}
int not_a_coroutine();
constexpr auto invalid = __builtin_coro_frame_align(not_a_coroutine); // expected-error {{frame layout requires a noalloc coroutine}}
static_assert(__builtin_coro_frame_size(work) == N);

[[coro::noalloc]] task after_error() { co_return; }
static_assert(__builtin_coro_frame_size(after_error) > 0);

[[coro::noalloc]] task recursive() {
  unsigned char own[__builtin_coro_frame_size(recursive)]; // expected-error {{coroutine definition must be visible before querying its frame}}
  co_return;
}

[[coro::noalloc]] task invalid_body() {
  missing_name(); // expected-error {{use of undeclared identifier}}
  co_return;
}
constexpr auto bad_body = __builtin_coro_frame_size(invalid_body);

[[coro::noalloc]] task variable_array(int n) { // expected-note {{declared here}}
  int data[n]; // expected-error {{variable length arrays in a coroutine are not supported}} expected-warning {{variable length arrays in C++ are a Clang extension}} expected-note {{function parameter 'n' with unknown value cannot be used in a constant expression}}
  co_await std::suspend_always{};
  consume(data);
}
constexpr auto bad_vla = __builtin_coro_frame_size(variable_array);

[[coro::noalloc]] task dynamic_alloca(int n) {
  void *p = __builtin_alloca(n);
  co_await std::suspend_always{};
  consume(p);
}
constexpr auto bad_alloca = __builtin_coro_frame_size(dynamic_alloca); // expected-error {{dynamic stack allocation is unsupported in a noalloc coroutine}}

[[coro::noalloc]] task after_invalid() { co_return; }
static_assert(__builtin_coro_frame_size(after_invalid) > 0);
