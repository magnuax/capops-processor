#include "publish/RedisPublisher.hpp"
#include "publish/ProtoMapper.hpp"

RedisPublisher::RedisPublisher(const Configuration &config) : config_(config)
{
    grid_ = config_.grid();
}

void RedisPublisher::publish(const ProcessingResult &result)
{
    FlightDataProto proto = mapToProto(result, config_, grid_);

    std::string serialized;
    bool ok = proto.SerializeToString(&serialized);

    if (!ok)
    {
        throw std::runtime_error("Failed to serialize FlightDataProto");
    }

    std::cout << "Serialized protobuf size: " << serialized.size() << std::endl;
    std::cout << proto.DebugString() << std::endl;
}