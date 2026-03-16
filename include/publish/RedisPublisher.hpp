#pragma once

#include "compute/Grid.hpp"
#include "config/Config.hpp"
#include "domain/types/ProcessingResult.hpp"

class RedisPublisher
{
  public:
    RedisPublisher(const Configuration &config);

    void publish(const ProcessingResult &result);

  private:
    const Configuration &config_;
    GridConfig grid_;
};