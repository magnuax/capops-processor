#pragma once
#include "domain/types/SectorState.hpp"
#include <cstdint>
#include <string>

class RiskEvent
{
  public:
    RiskEvent(int sectorId, std::int64_t timestamp, SectorState state);

  private:
    int sectorId_;
    std::int64_t timestamp_;
    SectorState state_;
};