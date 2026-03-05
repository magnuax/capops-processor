#pragma once 
#include <cstdint>
#include <string>
#include "domain/types/Position.hpp"

class Track {

public: 
    Track(std::string icao, std::int64_t timestamp, Position position, double altitude, double velocity, double heading); 

private: 
    std::string icao_; 
    std::int64_t timestamp_;
    Position position_; 
    double altitude_; 
    double velocity_; 
    double heading_; 

}; 