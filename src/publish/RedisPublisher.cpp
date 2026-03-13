#include "publish/RedisPublisher.hpp"
#include "publish/ProtoMapper.hpp"

RedisPublisher::RedisPublisher(const Configuration &config, const Grid &grid)
    : config_(config), grid_(grid)
{
}

void RedisPublisher::publish(const ProcessingResult &result)
{
    FlightDataProto proto = mapToProto(result, config_, grid_);

    //proto.SerializeToString("serialized");
}