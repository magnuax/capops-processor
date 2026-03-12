#pragma once

#include <string>

class Sector
{
  public:
    Sector(int id, double baseCapacity);

  private:
    int id_;
    double baseCapacity_;
};