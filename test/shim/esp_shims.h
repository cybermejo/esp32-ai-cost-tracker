// Host shims for ESP-IDF headers used by the portable modules.
#pragma once

#include <cstdlib>
#include <chrono>
#include <cstdint>

// esp_random.h
static inline uint32_t esp_random() { return arc4random(); }

// esp_timer.h
static inline int64_t esp_timer_get_time() {
  using namespace std::chrono;
  return duration_cast<microseconds>(steady_clock::now().time_since_epoch())
      .count();
}

// esp_heap_caps.h
#define MALLOC_CAP_INTERNAL 0x01
#define MALLOC_CAP_SPIRAM 0x02
#define MALLOC_CAP_8BIT 0x04
static inline void *heap_caps_malloc(size_t size, uint32_t) { return malloc(size); }
static inline void *heap_caps_calloc(size_t n, size_t size, uint32_t) { return calloc(n, size); }
static inline void heap_caps_free(void *p) { free(p); }
static inline size_t heap_caps_get_free_size(uint32_t) { return 0; }
