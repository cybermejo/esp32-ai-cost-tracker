// Host verification: Aggregator roll-up, shortName, weekTotal, day shapes.
// Run: make -f test/Makefile agg
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>
#include "Arduino.h"
#include "../src/data/SampleProvider.h"
#include "../src/data/Aggregator.h"

HostSerial Serial;
static int failures = 0;
#define EXPECT_TRUE(name, cond) do { printf("%-44s %s\n", name, (cond) ? "OK" : "FAIL"); if (!(cond)) failures++; } while (0)

int main() {
  SampleProvider p; std::vector<Day> days; p.load(days);
  const char *dates[7] = {"2026-09-04","2026-09-03","2026-09-02","2026-09-01","2026-08-31","2026-08-30","2026-08-29"};
  const int counts[7] = {7,12,11,15,3,3,3};
  for (int i = 0; i < 7; i++) {
    char n1[64], n2[64];
    snprintf(n1, sizeof n1, "days[%d] date %s", i, dates[i]);
    snprintf(n2, sizeof n2, "days[%d] %d models", i, counts[i]);
    EXPECT_TRUE(n1, days[i].date == dates[i]);
    EXPECT_TRUE(n2, (int)days[i].models.size() == counts[i]);
  }
  EXPECT_TRUE("shortName strips prefix",
    Aggregator::shortName("openrouter/deepseek/deepseek-v4-flash-0731") == "deepseek-v4-flash-0731");
  EXPECT_TRUE("shortName qwen",
    Aggregator::shortName("openai/Qwen/Qwen3.8-27B") == "Qwen3.8-27B");
  EXPECT_TRUE("weekTotal ~18.2824", fabsf(Aggregator::weekTotal(days) - 18.2824f) < 0.01f);
  auto slices = Aggregator::rollup(days, 5);
  EXPECT_TRUE("slices <= 6", slices.size() <= 6);
  float sum = 0; for (auto &s : slices) sum += s.cost;
  EXPECT_TRUE("slices sum == weekTotal", fabsf(sum - Aggregator::weekTotal(days)) < 0.01f);
  EXPECT_TRUE("top slice deepseek flash",
    slices.size() > 0 && slices[0].name == "deepseek/deepseek-v4-flash-0731");
  { std::vector<String> nm; nm.push_back("deepseek/deepseek-v4-flash-0731"); nm.push_back("openrouter/deepseek/deepseek-v4-flash-0731"); nm.push_back("z-ai/glm-5.3");
    auto u = Aggregator::uniqueShortNames(nm);
    EXPECT_TRUE("unique labels distinct+fit", u[0] != u[1] && u[0].size() <= 22 && u[1].size() <= 22 && u[2] == "glm-5.3"); }
  if (failures) { printf("%d FAILURES\n", failures); return 1; }
  printf("all host aggregator checks passed\n"); return 0;
}
