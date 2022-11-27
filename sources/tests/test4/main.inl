#include <stdio.h>
#include <iostream>
#include <ecs/ecs.h>
#include <ecs/type_registration.h>
#include <ecs/time_profiler.h>

struct Point4
{
  float x = 0, y = 0, z = 0, w = 0;
};

ECS_TYPE_REGISTRATION(Point4, "Point4", false, false, false)
ECS_TYPE_REGISTRATION(ecs::vector<Point4>, "vector<p4>", false, false, false)

int main()
{

  ecs::TypeAnnotation p4Annotation = *ecs::get_type_annotation<Point4>();

  ecs::TypeAnnotation vecP4Annotation = *ecs::get_type_annotation<ecs::vector<Point4>>();
  printf("Point4 %d\n", ecs::TypeIndex<Point4>::value);
  printf("float %d\n", ecs::TypeIndex<float>::value);
  const int N = 1000;
  Point4 *x = new Point4[N];
  Point4 *y = new Point4[N];
  ecs::vector<Point4> *v = new ecs::vector<Point4>[N];
  ecs::vector<Point4> *w = new ecs::vector<Point4>[N];

  {
    TimeProfile a("copy constuctor");
    for (int i = 0; i < N; i++)
      p4Annotation.copyConstructor(x + i, y + i);
  }
  {
    TimeProfile a("memcpy");
    for (int i = 0; i < N; i++)
      memcpy(x + i, y + i, sizeof(Point4));
  }
  {
    TimeProfile a("deconstuctor");
    for (int i = 0; i < N; i++)
      p4Annotation.destructor(x + i);
  }
  {
    TimeProfile a("no deconstuctor");
  }
  {
    TimeProfile a("move constuctor");
    for (int i = 0; i < N; i++)
      vecP4Annotation.moveConstructor(v + i, w + i);
  }
  {
    TimeProfile a("memmove");
    for (int i = 0; i < N; i++)
    {
      memcpy(w + i, v + i, sizeof(std::vector<Point4>));
      memset(v + i, 0, sizeof(std::vector<Point4>));
    }
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