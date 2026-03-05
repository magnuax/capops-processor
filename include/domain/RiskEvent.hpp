#pragma once
#include "domain/types/SectorState.hpp"
#include <cstdint>
#include <string>

class RiskEvent
{
  public:
    RiskEvent(std::string sectorId, std::int64_t timestamp, SectorState state);

  private:
    std::string sectorId_;
    std::int64_t timestamp_;
    SectorState state_;
};