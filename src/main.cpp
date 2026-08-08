#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "httplib.h"
#include "json.hpp"
#include "BacktestEngine.h"
#include "StrategyFactory.h"

using json = nlohmann::json;

// The only file in the project that knows about HTTP. It parses the
// request, delegates strategy construction to StrategyFactory and the
// simulation itself to BacktestEngine, and serializes the response.
// All trading/statistical logic lives elsewhere and is reachable
// without this file at all.
int main() {
    httplib::Server backtestServer;

    backtestServer.Get("/", [](const httplib::Request&, httplib::Response& response) {
        std::ifstream htmlFile("index.html");
        std::stringstream fileBuffer;
        fileBuffer << htmlFile.rdbuf();
        response.set_content(fileBuffer.str(), "text/html; charset=utf-8");
    });

    backtestServer.Post("/api/run_backtest", [](const httplib::Request& request, httplib::Response& response) {
        try {
            auto requestJson = json::parse(request.body);

            double riskPct = requestJson.value("risk_pct", 0.10);
            double startingCash = requestJson.at("start_cash");
            std::string strategyName = requestJson.value("strat_type", "SMA");

            std::unique_ptr<Strategy> strategy = StrategyFactory::create(strategyName, requestJson);
            BacktestEngine engine(std::move(strategy), startingCash, riskPct);

            json result = engine.run(requestJson.at("market_data"));
            response.set_content(result.dump(), "application/json");

        } catch (const json::exception& e) {
            std::cerr << "JSON Error: " << e.what() << std::endl;
            response.status = 400;
            response.set_content(std::string("JSON Error: ") + e.what(), "text/plain");
        } catch (const std::exception& e) {
            std::cerr << "Server Error: " << e.what() << std::endl;
            response.status = 500;
            response.set_content(std::string("Server Error: ") + e.what(), "text/plain");
        } catch (...) {
            std::cerr << "Unknown Fatal Error" << std::endl;
            response.status = 500;
            response.set_content("Unknown Error occurred during backtest", "text/plain");
        }
    });

    const char* portEnv = std::getenv("PORT");
    int port = portEnv ? std::stoi(portEnv) : 10000;

    std::cout << "Server starting on port " << port << std::endl;
    backtestServer.listen("0.0.0.0", port);
    return 0;
}
