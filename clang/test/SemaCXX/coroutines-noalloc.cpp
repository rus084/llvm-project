// RUN: %clang_cc1 -std=c++20 -fsyntax-only -verify %s
// RUN: %clang_cc1 -std=c++23 -fsyntax-only -verify %s

#include "Inputs/std-coroutine.h"

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

[[coro::noalloc]] task valid() { co_return; }
template<auto F> struct identity {};
identity<valid> compile_time_identity;
identity<&valid> explicit_compile_time_identity;
template<auto F> auto leak_pointer() { return F; } // expected-error {{noalloc coroutine must be invoked through coro::frame}}
auto escaped_nttp = leak_pointer<&valid>(); // expected-note {{in instantiation of function template specialization}}
auto escaped = valid; // expected-error {{noalloc coroutine must be invoked through coro::frame}}
constexpr auto escaped_constant = &valid; // expected-error {{noalloc coroutine must be invoked through coro::frame}}
auto &escaped_reference = valid; // expected-error {{noalloc coroutine must be invoked through coro::frame}}
void accept(task (*)()); // expected-note {{passing argument to parameter here}}
void pass_pointer() {
  accept(valid); // expected-error {{noalloc coroutine must be invoked through coro::frame}}
}

void direct() {
  valid(); // expected-error {{noalloc coroutine must be invoked through coro::frame}}
}

[[coro::noalloc]] void ordinary() {} // expected-error {{noalloc attribute requires a coroutine definition}}
struct owner {
  [[coro::noalloc]] task method() { co_return; } // expected-error {{noalloc supports only free non-template functions}}
};
template<class T>
[[coro::noalloc]] task generic(T) { co_return; } // expected-error {{noalloc supports only free non-template functions}}
#if __cplusplus >= 202302L
auto lambda = [] [[coro::noalloc]] () -> task { co_return; }; // expected-error {{noalloc supports only free non-template functions}}
#endif

[[coro::noalloc]] task declared();
task declared() { co_return; }
identity<declared> inherited_attribute;

task late(); // expected-note {{previous declaration is here}}
[[coro::noalloc]] task late() { co_return; } // expected-error {{noalloc attribute must appear on the first declaration}}
