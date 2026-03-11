#pragma once
#include "domain/types/SectorState.hpp"
#include <cstdint>
#include <string>

class RiskEvent
{
  public:
    RiskEvent(int sectorId, std::int64_t timestamp, SectorState state);

    int getSectorId() const; 
    std::int64_t getTimestamp() const; 
    SectorState getState() const; 

  private:
    int sectorId_;
    std::int64_t timestamp_;
    SectorState state_;
};