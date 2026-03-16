#include "publish/RedisPublisher.hpp"
#include "publish/ProtoMapper.hpp"

RedisPublisher::RedisPublisher(const Configuration &config)
    : config_(config), redis_(config.getRedisUrl()), channel_(config.getRedisChannel()),
      grid_(config_.grid())
{
}

void RedisPublisher::publish(const ProcessingResult &result)
{
    FlightDataProto proto = mapToProto(result, config_, grid_);

    std::string serialized;
    bool ok = proto.SerializeToString(&serialized);

    if (!ok)
    {
        throw std::runtime_error("Failed to serialize FlightDataProtof");
    }

    redis_.publish(channel_, serialized);
}