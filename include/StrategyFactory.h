#pragma once
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include "Strategy.h"
#include "json.hpp"

// Self-registering Factory Method (a Registry pattern).
//
// The original file constructed strategies via an if/else chain in
// main(), keyed on a request string — adding a strategy meant editing
// that chain, which violates the Open/Closed Principle at the
// construction site. Here, each strategy's own .cpp file registers
// its creator function at static-initialization time via the
// REGISTER_STRATEGY macro below. Adding strategy #7 means adding one
// new file; StrategyFactory and main.cpp never change.
class StrategyFactory {
public:
    using Creator = std::function<std::unique_ptr<Strategy>(const nlohmann::json&)>;

    // Called by REGISTER_STRATEGY in each strategy's .cpp file.
    static bool registerStrategy(const std::string& name, Creator creator);

    // Looks up `name`; falls back to "SMA" if unrecognized, matching
    // the original server's default behavior.
    static std::unique_ptr<Strategy> create(const std::string& name, const nlohmann::json& params);

private:
    // Function-local static avoids the static-initialization-order
    // fiasco: the registry is created lazily on first use, regardless
    // of which translation unit's REGISTER_STRATEGY runs first.
    static std::unordered_map<std::string, Creator>& registry();
};

#define REGISTER_STRATEGY_CONCAT_INNER(a, b) a##b
#define REGISTER_STRATEGY_CONCAT(a, b) REGISTER_STRATEGY_CONCAT_INNER(a, b)

// Usage, at the bottom of a strategy's .cpp file:
//   REGISTER_STRATEGY("BBAND", [](const nlohmann::json& p) {
//       return std::make_unique<BollingerBands>(p.value("fast_sma", 20), 2.0);
//   });
#define REGISTER_STRATEGY(name, creatorFn)                                            \
    namespace {                                                                       \
    const bool REGISTER_STRATEGY_CONCAT(_strategy_registered_, __LINE__) =            \
        StrategyFactory::registerStrategy(name, creatorFn);                           \
    }
