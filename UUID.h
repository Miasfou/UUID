#ifndef UUID_H
#define UUID_H

#include <cstdint>
#include <string>
#include <vector>
#include <chrono>
#include <cstring>
#include <thread>
#include <functional>
#include <atomic>

#if defined(_MSC_VER)
    #include <intrin.h>
    #define FORCE_INLINE __forceinline
#else
    #define FORCE_INLINE inline __attribute__((always_inline))
#endif

namespace UUID {
    alignas(64) inline constexpr char HEX[256][2] = {
        {'0','0'},{'0','1'},{'0','2'},{'0','3'},{'0','4'},{'0','5'},{'0','6'},{'0','7'},{'0','8'},{'0','9'},{'0','a'},{'0','b'},{'0','c'},{'0','d'},{'0','e'},{'0','f'},
        {'1','0'},{'1','1'},{'1','2'},{'1','3'},{'1','4'},{'1','5'},{'1','6'},{'1','7'},{'1','8'},{'1','9'},{'1','a'},{'1','b'},{'1','c'},{'1','d'},{'1','e'},{'1','f'},
        {'2','0'},{'2','1'},{'2','2'},{'2','3'},{'2','4'},{'2','5'},{'2','6'},{'2','7'},{'2','8'},{'2','9'},{'2','a'},{'2','b'},{'2','c'},{'2','d'},{'2','e'},{'2','f'},
        {'3','0'},{'3','1'},{'3','2'},{'3','3'},{'3','4'},{'3','5'},{'3','6'},{'3','7'},{'3','8'},{'3','9'},{'3','a'},{'3','b'},{'3','c'},{'3','d'},{'3','e'},{'3','f'},
        {'4','0'},{'4','1'},{'4','2'},{'4','3'},{'4','4'},{'4','5'},{'4','6'},{'4','7'},{'4','8'},{'4','9'},{'4','a'},{'4','b'},{'4','c'},{'4','d'},{'4','e'},{'4','f'},
        {'5','0'},{'5','1'},{'5','2'},{'5','3'},{'5','4'},{'5','5'},{'5','6'},{'5','7'},{'5','8'},{'5','9'},{'5','a'},{'5','b'},{'5','c'},{'5','d'},{'5','e'},{'5','f'},
        {'6','0'},{'6','1'},{'6','2'},{'6','3'},{'6','4'},{'6','5'},{'6','6'},{'6','7'},{'6','8'},{'6','9'},{'6','a'},{'6','b'},{'6','c'},{'6','d'},{'6','e'},{'6','f'},
        {'7','0'},{'7','1'},{'7','2'},{'7','3'},{'7','4'},{'7','5'},{'7','6'},{'7','7'},{'7','8'},{'7','9'},{'7','a'},{'7','b'},{'7','c'},{'7','d'},{'7','e'},{'7','f'},
        {'8','0'},{'8','1'},{'8','2'},{'8','3'},{'8','4'},{'8','5'},{'8','6'},{'8','7'},{'8','8'},{'8','9'},{'8','a'},{'8','b'},{'8','c'},{'8','d'},{'8','e'},{'8','f'},
        {'9','0'},{'9','1'},{'9','2'},{'9','3'},{'9','4'},{'9','5'},{'9','6'},{'9','7'},{'9','8'},{'9','9'},{'9','a'},{'9','b'},{'9','c'},{'9','d'},{'9','e'},{'9','f'},
        {'a','0'},{'a','1'},{'a','2'},{'a','3'},{'a','4'},{'a','5'},{'a','6'},{'a','7'},{'a','8'},{'a','9'},{'a','a'},{'a','b'},{'a','c'},{'a','d'},{'a','e'},{'a','f'},
        {'b','0'},{'b','1'},{'b','2'},{'b','3'},{'b','4'},{'b','5'},{'b','6'},{'b','7'},{'b','8'},{'b','9'},{'b','a'},{'b','b'},{'b','c'},{'b','d'},{'b','e'},{'b','f'},
        {'c','0'},{'c','1'},{'c','2'},{'c','3'},{'c','4'},{'c','5'},{'c','6'},{'c','7'},{'c','8'},{'c','9'},{'c','a'},{'c','b'},{'c','c'},{'c','d'},{'c','e'},{'c','f'},
        {'d','0'},{'d','1'},{'d','2'},{'d','3'},{'d','4'},{'d','5'},{'d','6'},{'d','7'},{'d','8'},{'d','9'},{'d','a'},{'d','b'},{'d','c'},{'d','d'},{'d','e'},{'d','f'},
        {'e','0'},{'e','1'},{'e','2'},{'e','3'},{'e','4'},{'e','5'},{'e','6'},{'e','7'},{'e','8'},{'e','9'},{'e','a'},{'e','b'},{'e','c'},{'e','d'},{'e','e'},{'e','f'},
        {'f','0'},{'f','1'},{'f','2'},{'f','3'},{'f','4'},{'f','5'},{'f','6'},{'f','7'},{'f','8'},{'f','9'},{'f','a'},{'f','b'},{'f','c'},{'f','d'},{'f','e'},{'f','f'}
    };

    class Generator {
    private:
        uint64_t state;
        
        static FORCE_INLINE uint64_t wyrand(uint64_t& s) {
            s += 0xa0761d6478bd642fULL;
            #if defined(__SIZEOF_INT128__)
                __uint128_t r = (__uint128_t)s * (s ^ 0xe7037ed1a0b428dbULL);
                return (uint64_t)(r >> 64) ^ (uint64_t)r;
            #elif defined(_MSC_VER) && defined(_M_X64)
                uint64_t hi;
                uint64_t lo = _umul128(s, s ^ 0xe7037ed1a0b428dbULL, &hi);
                return lo ^ hi;
            #else
                uint64_t z = s ^ 0xe7037ed1a0b428dbULL;
                z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
                z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
                return z ^ (z >> 31);
            #endif
        }

        static FORCE_INLINE void write_uuid(char* ptr, uint64_t r1, uint64_t r2) {
            uint64_t ab = (r1 & 0xFFFFFFFFFFFF0FFFULL) | 0x0000000000004000ULL;
            uint64_t cd = (r2 & 0x3FFFFFFFFFFFFFFFULL) | 0x8000000000000000ULL;

            std::memcpy(ptr,      HEX[static_cast<uint8_t>(ab >> 56)], 2);
            std::memcpy(ptr + 2,  HEX[static_cast<uint8_t>(ab >> 48)], 2);
            std::memcpy(ptr + 4,  HEX[static_cast<uint8_t>(ab >> 40)], 2);
            std::memcpy(ptr + 6,  HEX[static_cast<uint8_t>(ab >> 32)], 2);
            ptr[8] = '-';
            std::memcpy(ptr + 9,  HEX[static_cast<uint8_t>(ab >> 24)], 2);
            std::memcpy(ptr + 11, HEX[static_cast<uint8_t>(ab >> 16)], 2);
            ptr[13] = '-';
            std::memcpy(ptr + 14, HEX[static_cast<uint8_t>(ab >> 8)], 2);
            std::memcpy(ptr + 16, HEX[static_cast<uint8_t>(ab)], 2);
            ptr[18] = '-';
            std::memcpy(ptr + 19, HEX[static_cast<uint8_t>(cd >> 56)], 2);
            std::memcpy(ptr + 21, HEX[static_cast<uint8_t>(cd >> 48)], 2);
            ptr[23] = '-';
            std::memcpy(ptr + 24, HEX[static_cast<uint8_t>(cd >> 40)], 2);
            std::memcpy(ptr + 26, HEX[static_cast<uint8_t>(cd >> 32)], 2);
            std::memcpy(ptr + 28, HEX[static_cast<uint8_t>(cd >> 24)], 2);
            std::memcpy(ptr + 30, HEX[static_cast<uint8_t>(cd >> 16)], 2);
            std::memcpy(ptr + 32, HEX[static_cast<uint8_t>(cd >> 8)], 2);
            std::memcpy(ptr + 34, HEX[static_cast<uint8_t>(cd)], 2);
        }

        static void fill_chunk(char* ptr, size_t count, uint64_t seed) {
            uint64_t s0 = wyrand(seed);
            uint64_t s1 = wyrand(seed);
            uint64_t s2 = wyrand(seed);
            uint64_t s3 = wyrand(seed);

            size_t i = 0;
            
            for (; i + 3 < count; i += 4) {
                uint64_t r0_1 = wyrand(s0); uint64_t r0_2 = wyrand(s0);
                uint64_t r1_1 = wyrand(s1); uint64_t r1_2 = wyrand(s1);
                uint64_t r2_1 = wyrand(s2); uint64_t r2_2 = wyrand(s2);
                uint64_t r3_1 = wyrand(s3); uint64_t r3_2 = wyrand(s3);
                
                write_uuid(ptr, r0_1, r0_2);       ptr[36] = '\n';
                write_uuid(ptr + 37, r1_1, r1_2);  ptr[73] = '\n';
                write_uuid(ptr + 74, r2_1, r2_2);  ptr[110] = '\n';
                write_uuid(ptr + 111, r3_1, r3_2); ptr[147] = '\n';
                
                ptr += 148;
            }

            for (; i < count; ++i) {
                uint64_t r1 = wyrand(s0);
                uint64_t r2 = wyrand(s0);
                write_uuid(ptr, r1, r2);
                ptr[36] = '\n';
                ptr += 37;
            }
        }

    public:
        Generator() {
            static std::atomic<uint64_t> instance_counter{1};
            uint64_t unique_inc = instance_counter.fetch_add(0x9e3779b97f4a7c15ULL, std::memory_order_relaxed);
            
            uint64_t time_seed = static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
            uint64_t thread_seed = static_cast<uint64_t>(std::hash<std::thread::id>{}(std::this_thread::get_id()));
            
            uint64_t initial_seed = time_seed ^ thread_seed ^ unique_inc;
            state = initial_seed;
            wyrand(state);
            wyrand(state);
        }

        std::string next() {
            std::string s(36, '\0');
            next_into(&s[0]);
            return s;
        }

        FORCE_INLINE void next_into(char* ptr) {
            uint64_t r1 = wyrand(state);
            uint64_t r2 = wyrand(state);
            write_uuid(ptr, r1, r2);
        }

        void fill_buffer(char* buffer, size_t count) {
            if (count < 20000) {
                uint64_t chunk_seed = wyrand(state);
                fill_chunk(buffer, count, chunk_seed);
                return;
            }

            unsigned int cores = std::thread::hardware_concurrency();
            if (cores == 0) cores = 4;
            
            std::vector<std::thread> threads;
            size_t chunk = count / cores;
            char* p = buffer;

            for(unsigned int i = 0; i < cores; ++i) {
                size_t n = (i == cores - 1) ? (count - i * chunk) : chunk;
                uint64_t thread_seed = wyrand(state); 
                threads.emplace_back(fill_chunk, p, n, thread_seed);
                p += n * 37;
            }

            for(auto& t : threads) {
                t.join();
            }
        }
    };

    inline std::string next() {
        static thread_local Generator gen;
        return gen.next();
    }
    
    inline void next_into(char* ptr) {
        static thread_local Generator gen;
        gen.next_into(ptr);
    }
}
#endif