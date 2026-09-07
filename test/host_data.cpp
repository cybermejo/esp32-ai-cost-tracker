// Host verification: SampleProvider week shape + per-day cost sums.
// Run: make -f test/Makefile data
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>
#include "Arduino.h"
#include "../src/data/SampleProvider.h"

HostSerial Serial;
static int failures = 0;
#define EXPECT_TRUE(name, cond) do { printf("%-44s %s\n", name, (cond) ? "OK" : "FAIL"); if (!(cond)) failures++; } while (0)

int main() {
  SampleProvider p; std::vector<Day> days;
  EXPECT_TRUE("load returns true", p.load(days));
  EXPECT_TRUE("7 days", days.size() == 7);
  for (auto &d : days) {
    float sum = 0; for (auto &m : d.models) sum += m.costUSD;
    char n[64]; snprintf(n, sizeof n, "models sum==total %s", d.date.c_str());
    EXPECT_TRUE(n, fabsf(sum - d.costUSD) < 0.001f);
  }
  EXPECT_TRUE("days[0] is 2026-09-04", days[0].date == "2026-09-04");
  EXPECT_TRUE("days[3] is 2026-09-01 $7.6579", days[3].date == "2026-09-01" && fabsf(days[3].costUSD - 7.6579f) < 0.001f);
  EXPECT_TRUE("09-02 total $0.6147", fabsf(days[2].costUSD - 0.6147f) < 0.001f);
  EXPECT_TRUE("09-03 total $2.9003", fabsf(days[1].costUSD - 2.9003f) < 0.001f);
  EXPECT_TRUE("09-04 total $1.2095", fabsf(days[0].costUSD - 1.2095f) < 0.001f);
  EXPECT_TRUE("09-01 deepseek flash 306tr $3.875",
    days[3].models[0].name == "deepseek/deepseek-v4-flash-0731" && days[3].models[0].traces == 306 && fabsf(days[3].models[0].costUSD - 3.875f) < 0.001f);
  EXPECT_TRUE("09-03 glm-5.3 $2.4879", fabsf(days[1].models[0].costUSD - 2.4879f) < 0.001f);
  EXPECT_TRUE("last 3 days synthetic", days[4].synthetic && days[5].synthetic && days[6].synthetic);
  if (failures) { printf("%d FAILURES\n", failures); return 1; }
  printf("all host data checks passed\n"); return 0;
}
