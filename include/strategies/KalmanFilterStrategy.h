#pragma once
#include "Strategy.h"

// Models the "true" price as a noisy observation of a random-walk
// hidden state, then trades on divergence of the observed price from
// the filtered estimate. See project report Section 4.6.
class KalmanFilterStrategy : public Strategy {
public:
    explicit KalmanFilterStrategy(double processNoise = 1e-4, double measurementNoise = 1e-2);

protected:
    std::string generateSignal(const Tick& currentTick, Portfolio& userPortfolio, double riskPct) override;

private:
    double processNoise_;
    double measurementNoise_;
    double estimatedPrice_ = 0.0;
    double errorCovariance_ = 1.0;
    bool isInitialized_ = false;
};
