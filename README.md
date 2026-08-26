# Trading Backtester — Modular Edition

This is a refactor of the original single-file `server.cpp` into a
multi-file, pattern-driven C++ project. **Behavior is unchanged** —
same endpoints, same JSON contract, same trading math — only the
structure is different.

## Layout

```
trading_backtester/
├── CMakeLists.txt
├── include/
│   ├── Tick.h                  # plain OHLC data aggregate
│   ├── Portfolio.h             # encapsulated account state
│   ├── Strategy.h              # abstract base (Template Method)
│   ├── StrategyFactory.h       # self-registering factory/registry
│   ├── StatsUtils.h            # Hurst / Kelly diagnostics
│   ├── BacktestEngine.h        # simulation loop, no HTTP knowledge
│   └── strategies/
│       ├── SMACrossover.h
│       ├── EMACrossover.h
│       ├── BollingerBands.h
│       ├── RSIMomentum.h
│       ├── ZScoreArbitrage.h
│      
├── src/
│   ├── Portfolio.cpp
│   ├── Strategy.cpp
│   ├── StrategyFactory.cpp
│   ├── StatsUtils.cpp
│   ├── BacktestEngine.cpp
│   ├── main.cpp                 # only file that touches httplib
│   └── strategies/*.cpp         # one file per strategy, self-registering
└── third_party/
    ├── httplib.h
    └── json.hpp
```

## What changed structurally (and why)

| Change | Pattern / Principle | Benefit |
|---|---|---|
| One class per header/source file | Single Responsibility | Each file compiles, reviews, and tests independently |
| `Strategy::evaluateTrade()` is now a fixed, non-virtual method that calls `checkStopLoss()` then `generateSignal()` | **Template Method** | Removes the 6x duplicated stop-loss block — every strategy now only implements its own signal logic |
| `StrategyFactory` with `REGISTER_STRATEGY(...)` self-registration in each strategy's `.cpp` | **Factory Method / Registry** | Adding strategy #7 = add one new file. `main.cpp` and the factory itself never change (fixes the Open/Closed violation noted in the architecture report) |
| `BacktestEngine` extracted from the HTTP handler | Separation of Concerns | The simulation loop is now callable/testable without an HTTP server |
| `Portfolio`'s `winningTrades`/`losingTrades`/PnL sums moved from public fields to private + getters | Encapsulation | External code can read but never corrupt trade-outcome bookkeeping |
| `StatsUtils` namespace for Hurst | Cohesion | Statistical diagnostics grouped separately from trading/execution logic |

## Adding a new strategy

1. Create `include/strategies/MyStrategy.h` and `src/strategies/MyStrategy.cpp`, subclassing `Strategy` and implementing only `generateSignal(...)`.
2. At the bottom of the `.cpp`, register it:
   ```cpp
   REGISTER_STRATEGY("MYSTRAT", [](const nlohmann::json& p) -> std::unique_ptr<Strategy> {
       return std::make_unique<MyStrategy>(p.value("some_param", 42));
   });
   ```
3. Rebuild — CMake's `GLOB_RECURSE` picks up the new `.cpp` automatically, and the factory registry picks up the new strategy name automatically. Nothing else in the project needs to change.

## Build

```bash
mkdir build && cd build
cmake ..
cmake --build .
./backtester
```

## Known pre-existing behavior (carried over, not fixed)

These were present in the original file and are intentionally
preserved as-is during this structural refactor:

- `Portfolio::averageEntryPrice_` is overwritten (not volume-weighted)
  on every additional buy while a position is already open.
- `RSIMomentum` uses a simple rolling average of gains/losses, not
  Wilder's exponential smoothing.
- `calculateHurstExponent()` is a single-scale variance-ratio proxy,
  not full Rescaled Range (R/S) analysis.

See the accompanying architecture report for details and suggested
fixes if you'd like these addressed in a follow-up change.
