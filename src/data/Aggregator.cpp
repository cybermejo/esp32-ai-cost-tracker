#include "Aggregator.h"
#include <algorithm>
#include <cstring>
#include <cstdio>

// Portable across host (UNIT_TEST: String == std::string) and device
// (Arduino String): only c_str(), length(), == and const-char* construction
// are used; the divergent truncate call is selected by ifdef.
String Aggregator::shortName(const String& raw) {
  const char* c = raw.c_str();
  const char* p = strrchr(c, '/');
  String s = p ? (p + 1) : c;
#ifdef UNIT_TEST
  if (s.size() > 22) return s.substr(0, 21) + "~";
#else
  if (s.length() > 22) return s.substring(0, 21) + "~";
#endif
  return s;
}
float Aggregator::weekTotal(const std::vector<Day>& d) {
  float t = 0; for (auto &x : d) t += x.costUSD; return t;
}
std::vector<Slice> Aggregator::rollup(const std::vector<Day>& days, int topN) {
  // Linear accumulation instead of std::map: Arduino String has operator==
  // but no operator<, so it cannot be a map key on-device.
  std::vector<Slice> v;
  for (auto &d : days) for (auto &m : d.models) {
    bool found = false;
    for (auto &s : v) if (s.name == m.name) { s.cost += m.costUSD; found = true; break; }
    if (!found) { Slice s; s.name = m.name; s.cost = m.costUSD; v.push_back(s); }
  }
  std::sort(v.begin(), v.end(), [](const Slice &a, const Slice &b){ return a.cost > b.cost; });
  float total = 0; for (auto &s : v) total += s.cost;
  std::vector<Slice> out;
  for (int i = 0; i < (int)v.size() && i < topN; i++) {
    v[i].pct = total > 0 ? 100.0f * v[i].cost / total : 0; out.push_back(v[i]);
  }
  if ((int)v.size() > topN) {
    Slice o; o.name = "Other"; o.cost = 0;
    for (int i = topN; i < (int)v.size(); i++) o.cost += v[i].cost;
    o.pct = total > 0 ? 100.0f * o.cost / total : 0; out.push_back(o);
  }
  return out;
}
std::vector<String> Aggregator::uniqueShortNames(const std::vector<String>& names) {
  std::vector<String> out;
  for (size_t i = 0; i < names.size(); i++) {
    String b = shortName(names[i]);
    bool dup = false;
    for (size_t j = 0; j < names.size(); j++)
      if (j != i && shortName(names[j]) == b) { dup = true; break; }
    if (!dup) { out.push_back(b); continue; }
    const char *raw = names[i].c_str();
    const char *sl = strchr(raw, '/');
    const char *lr = strrchr(raw, '/');
    if (!sl || !lr) { out.push_back(b); continue; }
    size_t plen = (size_t)(sl - raw);
    const char *tail = lr + 1;
    char prov[24]; snprintf(prov, sizeof prov, "%.*s", (int)plen, raw);
    char tb[24]; size_t room = (plen + 1 < 22) ? (22 - plen - 1) : 0;
    if (strlen(tail) <= room) snprintf(tb, sizeof tb, "%s", tail);
    else if (room > 1) snprintf(tb, sizeof tb, "%.*s~", (int)(room - 1), tail);
    else snprintf(tb, sizeof tb, "~");
    char buf[48]; snprintf(buf, sizeof buf, "%s/%s", prov, tb);
    out.push_back(String(buf));
  }
  return out;
}
