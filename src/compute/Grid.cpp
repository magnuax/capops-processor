#include "compute/Grid.hpp"
#include <stdexcept>

Grid::Grid(const GridConfig &config) : config_(config)
{
    if (config_.rows <= 0 || config_.cols <= 0)
    {
        throw std::invalid_argument("Grid rows and cols must be greater than 0");
    }

    if (config_.maxLat <= config_.minLat)
    {
        throw std::invalid_argument("Grid maxLat must be greater than minLat");
    }

    if (config_.maxLon <= config_.minLon)
    {
        throw std::invalid_argument("Grid maxLon must be greater than minLon");
    }

    cellHeightDeg_ = (config_.maxLat - config_.minLat) / config_.rows;
    cellWidthDeg_ = (config_.maxLon - config_.minLon) / config_.cols;
}

int Grid::sectorCount() const
{
    return config_.rows * config_.cols;
}

bool Grid::isInside(const Position &position) const
{
    return position.latDeg > config_.minLat && position.latDeg < config_.maxLat &&
           position.lonDeg > config_.minLon && position.lonDeg < config_.maxLon;
}

int Grid::determineSector(const Position &position) const
{
    if (!isInside(position))
    {
        return -1;
    }

    int row = static_cast<int>((position.latDeg - config_.minLat) / cellHeightDeg_);
    int col = static_cast<int>((position.lonDeg - config_.minLon) / cellWidthDeg_);

    return row * config_.cols + col;
}

int Grid::rows() const
{
    return config_.rows;
}

int Grid::cols() const
{
    return config_.cols;
}

int Grid::row(int sectorId) const
{
    if (sectorId < 0 || sectorId >= sectorCount())
    {
        throw std::out_of_range("sectorId is out of range");
    }

    return sectorId / config_.cols;
}

int Grid::column(int sectorId) const
{
    if (sectorId < 0 || sectorId >= sectorCount())
    {
        throw std::out_of_range("sectorId is out of range");
    }

    return sectorId % config_.cols;
}