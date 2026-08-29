# Trading Backtester — Modular Edition

This is a refactor of the original single-file `server.cpp` into a multi-file, pattern-driven C++ project. **Behavior is unchanged** — same endpoints, same JSON contract, same trading math — only the structure is different.

## Layout

```
trading_backtester/
├── CMakeLists.txt
├── include/
│   ├── Tick.h                  # plain OHLC data aggregate
│   ├── Portfolio.h             # encapsulated account state
│   ├── Strategy.h              # abstract base (Template Method)
│   ├── StrategyFactory.h       # strategy creator interface and subclasses
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
│   ├── StatsUtils.cpp
│   ├── BacktestEngine.cpp
│   ├── main.cpp                 # only file that touches httplib
│   └── strategies/*.cpp         # one file per strategy
└── third_party/
    ├── httplib.h
    └── json.hpp
```

## What changed structurally (and why)

| Change | Pattern / Principle | Benefit |
|---|---|---|
| One class per header/source file | Single Responsibility | Each file compiles, reviews, and tests independently |
| `Strategy::evaluateTrade()` is now a fixed, non-virtual method that calls `checkStopLoss()` then `generateSignal()` | **Template Method** | Removes the 6x duplicated stop-loss block — every strategy now only implements its own signal logic |
| `BacktestEngine` extracted from the HTTP handler | Separation of Concerns | The simulation loop is now callable/testable without an HTTP server |
| `Portfolio`'s `winningTrades`/`losingTrades`/PnL sums moved from public fields to private + getters | Encapsulation | External code can read but never corrupt trade-outcome bookkeeping |
| `StatsUtils` namespace for Hurst | Cohesion | Statistical diagnostics grouped separately from trading/execution logic |

## How to Clone and Use

### 1. Clone the repository
```bash
git clone <repository_url>
cd trading_backtester
```

### 2. Build the project
Make sure you have CMake and a C++17 compatible compiler (e.g., GCC 13+, Clang, or MSVC) installed. If using MinGW on Windows, ensure it supports the POSIX thread model.

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### 3. Run the Backtester Server
Start the HTTP server on port 10000 (default):
```bash
./backtester
```
*(On Windows, run `.\backtester.exe`)*

### 4. Test the API (Using provided Python scripts)
You can use the included Python scripts to generate mock OHLC data and test the backtester API endpoint (`/api/run_backtest`).

In a separate terminal, run:
```bash
# Generate mock data and send it as JSON to the server
python test_backtest.py
```
Or use the CSV script, which generates a `mock_ohlc.csv`, loads it, and sends it to the API:
```bash
python test_with_csv.py
```

## Adding a new strategy

1. Create `include/strategies/MyStrategy.h` and `src/strategies/MyStrategy.cpp`, subclassing `Strategy` and implementing only `generateSignal(...)`.
2. Add a `MyStrategyCreator` subclass to `include/StrategyFactory.h`.
3. Add a condition for `MyStrategy` inside the factory routing logic in `src/main.cpp`.
4. Rebuild — CMake's `GLOB_RECURSE` picks up the new `.cpp` automatically.

## Known pre-existing behavior (carried over, not fixed)

These were present in the original file and are intentionally preserved as-is during this structural refactor:

- `Portfolio::averageEntryPrice_` is overwritten (not volume-weighted) on every additional buy while a position is already open.
- `RSIMomentum` uses a simple rolling average of gains/losses, not Wilder's exponential smoothing.
- `calculateHurstExponent()` is a single-scale variance-ratio proxy, not full Rescaled Range (R/S) analysis.
