# UUID Header Only C++

A very high performance, header only C++ library for generating **RFC 4122 compliant V4 UUIDs**.
Designed for maximum throughput, low latency, and thread safety, utilizing the `wyrand` PRNG algorithm and SIMD friendly batch processing.

*   **Performance:** Generates and formats UUIDs significantly faster than `boost::uuid` or `libuuid`.
*   **Header Only:** No building, no linking. Just `#include "UUID.h"`.
*   **RFC 4122 Compliant:** Correctly sets the Version 4 and Variant (RFC) bits.
*   **Thread Safety:** Uses `thread_local` generators with collision resistant seeding logic.
*   **Multi Threading:** Automatic multi threading for large buffer fills (>20k).
*   **Cache Optimized:** Lookup tables are aligned to CPU cache lines (64 byte) to minimize cache misses.

*   **RNG Step:** ~3-5 CPU cycles (thanks to purely arithmetic operations).
*   **Formatting:** The conversion from 128-bit integer to a 36-byte string is heavily unrolled.
*   **Total Throughput:** On modern hardware (e.g., Ryzen 5000/Intel 12th gen+), this implementation can theoretically saturate memory bandwidth when running in bulk mode.

*  **Global Atomic Counter:** Ensures no two generator instances start with the same sequence, even if created at the exact same nanosecond!
*  **High Resolution Clock:** Adds temporal entropy.
*  **Thread ID Hash:** Adds spatial entropy based on execution context.

# Usage Examples
```cpp
#include <iostream>
#include "uuid.h"

int main() {
    std::string id = UUID::next(); 
    std::cout << id << std::endl;
    return 0;
}
```
```cpp
#include <vector>
#include "uuid.h"

int main() {
    size_t count = 1000000;
    std::vector<char> buffer(count * 37);
    
    UUID::Generator gen;
    gen.fill_buffer(buffer.data(), count);
    return 0;
}
```
