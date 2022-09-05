#include <stdio.h>
#include <iostream>

#include <functional>

#include <ecs/type_annotation.h>
#include <ecs/type_registration.h>
#include <ecs/time_profiler.h>

#include <ecs/c.h>

struct Point4
{
  float x = 0, y = 0, z = 0, w = 0;
};

ECS_TYPE_REGISTRATION(Point4, "Point4", false, false, false, false, {})
ECS_TYPE_REGISTRATION(ecs::vector<Point4>, "vector<p4>", false, false, false, false, {})

void f()
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
    TimeProfile a("default constuctor");
    for (int i = 0; i < N; i++)
      p4Annotation.defaultConstructor(x + i);
  }
  {
    TimeProfile a("memset");
    for (int i = 0; i < N; i++)
      memset(y + i, 0, sizeof(Point4));
  }
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

  printf("))) ");
  std::cout << std::endl;
  /// dfff
}