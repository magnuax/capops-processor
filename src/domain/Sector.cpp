#include "domain/Sector.hpp"

Sector::Sector(std::string id, double baseCapacity)
    : id_(std::move(id)), baseCapacity_(baseCapacity)
{
}