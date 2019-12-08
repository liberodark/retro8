#include "machine.h"

using namespace retro8;

void Machine::color(color_t color)
{
  _state.penColor = color;
}

void Machine::pset(coord_t x, coord_t y, color_t color)
{
  const auto& c = ColorTable[color];
  _memory.setScreenData(x, y, color);
  static_cast<uint32_t*>(_surface->pixels)[y*_surface->w + x] = (c.r << 16) | (c.g << 8) | (c.b) | 0xff000000;
}

color_t Machine::pget(coord_t x, coord_t y)
{
  return _memory.screenData(x, y);
}

void Machine::line(coord_t x0, coord_t y0, coord_t x1, coord_t y1, color_t color)
{
  // vertical
  if (y0 == y1)
  {
    if (x0 > x1) std::swap(x0, x1);
    
    for (coord_t x = x0; x <= x1; ++x)
      pset(x, y0, color);
  }
  // horizontal
  else if (x0 == x1)
  {
    if (y0 > y1) std::swap(y0, y1);
    
    for (coord_t y = y0; y <= y1; ++y)
      pset(x0, y, color);
  }
  else
  {
    float dx = x1 - (float)x0;
    float dy = y1 - y0;
    float derror = std::abs(dy / dx);
    float error = 0.0f;

    coord_t y = y0;
    for (coord_t x = x0; x <= x1; ++x)
    {
      pset(x, y, color);
      error += derror;
      if (error >= 0.5f)
      {
        y += std::copysignf(1, dy);
        error -= 1.0f;
      }
    }
  }

  // TODO: shouldn't be updated when invoked from other primitives, eg rect
  _state.lastLineEnd.x = x1;
  _state.lastLineEnd.x = y1;
}

void Machine::rect(coord_t x0, coord_t y0, coord_t x1, coord_t y1, color_t color)
{
  line(x0, y0, x1, y0, color);
  line(x1, y0, x1, y1, color);
  line(x0, y1, x1, y1, color);
  line(x0, y1, x0, y0, color);
}
