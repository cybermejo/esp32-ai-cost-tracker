// Host verification: idle-dim state machine edges.
// Run: make -f test/Makefile idle
#include <cstdio>
#include <stdint.h>
#include "../src/ui/IdleDim.h"

static int failures = 0;
#define EXPECT_TRUE(name, expr) do { bool _ok = (expr); printf("%-44s %s\n", name, _ok ? "OK" : "FAIL"); if (!_ok) failures++; } while (0)

int main() {
  IdleDim d; d.begin(1000);
  EXPECT_TRUE("starts awake", !d.isDimmed());
  EXPECT_TRUE("no edge before timeout", !d.poll(1000 + 29999));
  EXPECT_TRUE("edge at 30s", d.poll(1000 + 30000));
  EXPECT_TRUE("now dimmed", d.isDimmed());
  EXPECT_TRUE("edge fires once", !d.poll(1000 + 31000));
  EXPECT_TRUE("activity wakes", d.activity(1000 + 32000));
  EXPECT_TRUE("awake again", !d.isDimmed());
  EXPECT_TRUE("activity while awake is quiet", !d.activity(1000 + 33000));
  EXPECT_TRUE("timer restarted by activity", !d.poll(1000 + 33000 + 29999));
  EXPECT_TRUE("re-dims after idle", d.poll(1000 + 33000 + 30000));
  EXPECT_TRUE("dim level sane", IdleDim::DIM > 0 && IdleDim::DIM < 150);
  if (failures) { printf("%d FAILURES\n", failures); return 1; }
  printf("all host idle checks passed\n"); return 0;
}
