#pragma once

#include <string>

class Sector
{
  public:
    Sector(std::string id, double baseCapacity);

  private:
    std::string id_;
    double baseCapacity_;
};