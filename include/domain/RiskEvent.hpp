#pragma once
#include "domain/types/SectorState.hpp"
#include <cstdint>
#include <string>

class RiskEvent
{
  public:
    RiskEvent(int riskEventId, int sectorId, std::int64_t timestamp, SectorState state);

    int getSectorId() const; 
    std::int64_t getTimestamp() const; 
    SectorState getState() const; 

  private:
    int riskEventId_; 
    int sectorId_;
    std::int64_t timestamp_;
    SectorState state_;
    std::string message_; 
    bool acknowledged_ = false; 
};