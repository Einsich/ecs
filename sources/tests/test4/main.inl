#include <stdio.h>
#include <iostream>
#include <ecs/ecs.h>
#include <ecs/type_registration.h>
#include <ecs/time_profiler.h>

struct Point4
{
  float x = 0, y = 0, z = 0, w = 0;
};
struct float4
{
  float x = 0, y = 0, z = 0, w = 0;
};

ECS_TYPE_REGISTRATION(Point4, "Point4", ecs::DefaultType)
ECS_TYPE_REGISTRATION(ecs::vector<Point4>, "vector<p4>", ecs::DefaultType)

ECS_TYPE_REGISTRATION(float4, "float4", ecs::PODType)
ECS_TYPE_REGISTRATION(ecs::vector<float4>, "vector<f4>", ecs::TrivialMoveConstructor)

int main()
{

  auto p4Annotation = *ecs::TypeIndex<Point4>::fabric;
  auto vecP4Annotation = *ecs::TypeIndex<ecs::vector<Point4>>::fabric;

  auto f4Annotation = *ecs::TypeIndex<float4>::fabric;
  auto vecF4Annotation = *ecs::TypeIndex<ecs::vector<float4>>::fabric;

  ECS_LOG("Point4 %d", ecs::TypeIndex<Point4>::value);
  ECS_LOG("float %d", ecs::TypeIndex<float>::value);
  const int N = 1000;
  Point4 *x = new Point4[N];
  float4 *y = new float4[N];
  ecs::vector<Point4> *v = new ecs::vector<Point4>[N];
  ecs::vector<float4> *w = new ecs::vector<float4>[N];

  {
    TimeProfile a("copy constuctor Default");
    for (int i = 0; i < N; i++)
      p4Annotation.copy_constructor(x + i, y + i);
  }
  {
    TimeProfile a("copy constuctor POD");
    for (int i = 0; i < N; i++)
      f4Annotation.copy_constructor(y + i, x + i);
  }
  {
    TimeProfile a("destructor Default");
    for (int i = 0; i < N; i++)
      p4Annotation.destructor(x + i);
  }
  {
    for (int i = 0; i < N; i++)
      f4Annotation.destructor(y + i);
    TimeProfile a("destructor POD");
  }
  {
    TimeProfile a("move constuctor Default");
    for (int i = 0; i < N; i++)
      vecP4Annotation.move_constructor(v + i, w + i);
  }
  {
    TimeProfile a("move constuctor Relocatable");
    for (int i = 0; i < N; i++)
      vecF4Annotation.move_constructor(w + i, v + i);
  }
  std::fflush(stdout);
}

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