#pragma once
#include "domain/types/Position.hpp"
#include <cstdint>
#include <string>

class Track
{

  public:
    Track(std::string icao, std::int64_t timestamp, Position position, double altitude,
          double velocity, double heading);
    std::string getIcao() const;
    std::int64_t getTimestamp() const;
    Position getPosition() const;

  private:
    std::string icao_;
    std::int64_t timestamp_;
    Position position_;
    double altitude_;
    double velocity_;
    double heading_;
};