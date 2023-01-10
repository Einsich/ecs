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

  ecs::TypeAnnotation p4Annotation = *ecs::get_type_annotation<Point4>();
  ecs::TypeAnnotation vecP4Annotation = *ecs::get_type_annotation<ecs::vector<Point4>>();

  ecs::TypeAnnotation f4Annotation = *ecs::get_type_annotation<float4>();
  ecs::TypeAnnotation vecF4Annotation = *ecs::get_type_annotation<ecs::vector<float4>>();

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
      p4Annotation.copy(x + i, y + i);
  }
  {
    TimeProfile a("copy constuctor POD");
    for (int i = 0; i < N; i++)
      f4Annotation.copy(y + i, x + i);
  }
  {
    TimeProfile a("destructor Default");
    for (int i = 0; i < N; i++)
      p4Annotation.destruct(x + i);
  }
  {
    for (int i = 0; i < N; i++)
      f4Annotation.destruct(y + i);
    TimeProfile a("destructor POD");
  }
  {
    TimeProfile a("move constuctor Default");
    for (int i = 0; i < N; i++)
      vecP4Annotation.move(v + i, w + i);
  }
  {
    TimeProfile a("move constuctor Relocatable");
    for (int i = 0; i < N; i++)
      vecF4Annotation.move(w + i, v + i);
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