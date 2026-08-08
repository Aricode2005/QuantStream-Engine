#include "StrategyFactory.h"
#include <stdexcept>

std::unordered_map<std::string, StrategyFactory::Creator>& StrategyFactory::registry() {
    static std::unordered_map<std::string, Creator> instance;
    return instance;
}

bool StrategyFactory::registerStrategy(const std::string& name, Creator creator) {
    registry()[name] = std::move(creator);
    return true;
}

std::unique_ptr<Strategy> StrategyFactory::create(const std::string& name, const nlohmann::json& params) {
    auto& reg = registry();
    auto it = reg.find(name);
    if (it != reg.end()) {
        return it->second(params);
    }
    auto defaultIt = reg.find("SMA");
    if (defaultIt == reg.end()) {
        throw std::runtime_error("No strategies registered — did every strategy .cpp get compiled in?");
    }
    return defaultIt->second(params);
}
