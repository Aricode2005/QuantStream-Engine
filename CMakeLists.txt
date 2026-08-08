cmake_minimum_required(VERSION 3.15)
project(trading_backtester CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

# Pull in every .cpp under src/ automatically — this is what lets new
# strategies self-register with StrategyFactory without editing this
# build file: drop a new src/strategies/Foo.cpp and it's compiled in.
file(GLOB_RECURSE BACKTESTER_SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_executable(backtester ${BACKTESTER_SOURCES})

target_include_directories(backtester PRIVATE
    include
    include/strategies
    third_party
)

target_link_libraries(backtester PRIVATE Threads::Threads)
