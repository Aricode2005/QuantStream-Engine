#pragma once
#include <string>

// Plain data aggregate representing a single OHLC bar.
//
// Intentionally has no invariants and no behavior — it is pure data,
// in deliberate contrast to Portfolio, which owns business rules and
// is therefore encapsulated (see Portfolio.h).
struct Tick {
    std::string timestamp;
    double open;
    double high;
    double low;
    double close;
};
