#pragma once
#include "domain/types/Position.hpp"
#include <cstdint>
#include <string>

class Track
{

  public:
    Track(std::string icao, std::string timestamp, Position position, double altitude,
          double velocity, double heading);
    std::string getIcao() const;
    std::string getTimestamp() const;
    Position getPosition() const;

  private:
    std::string icao_;
    std::string timestamp_;
    Position position_;
    double altitude_;
    double velocity_;
    double heading_;
};