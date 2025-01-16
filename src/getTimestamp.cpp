#include <chrono>
#include "getTimestamp.h"

int64_t getTimestamp() {
    return std::chrono::system_clock::now().time_since_epoch().count();
}