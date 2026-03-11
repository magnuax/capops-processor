#pragma once
#include "config/Config.hpp"
#include "domain/types/Position.hpp"

class Grid
{
public:
    Grid(const GridConfig& config);

    int sectorCount() const;
    bool isInside(const Position& position) const;
    int determineSector(const Position& position) const;

    int rows() const;
    int cols() const;

private:
    GridConfig config_;
    double cellHeightDeg_;
    double cellWidthDeg_;
};