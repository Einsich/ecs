#include <ecs/ecs_std.h>
#include <ecs/event.h>
#include <ecs/request.h>

struct A
{
};

struct B
{
  char b;
};

struct C
{
  int c;
};

struct E : ecs::Event
{
};

struct R : ecs::Request
{
};

static_assert(ecs::is_zero_sizeof<A> == true);
static_assert(ecs::is_zero_sizeof<B> == false);
static_assert(ecs::is_zero_sizeof<C> == false);
static_assert(ecs::is_zero_sizeof<E> == true);
static_assert(ecs::is_zero_sizeof<R> == true);