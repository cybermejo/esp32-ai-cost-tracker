#pragma once
#include "model.h"
struct Slice { String name; float cost = 0; float pct = 0; };
class Aggregator {
 public:
  static std::vector<Slice> rollup(const std::vector<Day>& days, int topN = 5);
  static String shortName(const String& raw);
  static std::vector<String> uniqueShortNames(const std::vector<String>& names);
  static float weekTotal(const std::vector<Day>& days);
};
