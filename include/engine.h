#pragma once
#include "render.h"

class Engine
{
  public:
    Render render;
    ModelLoader modelLoader;
    void run();
};