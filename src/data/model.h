#pragma once
#ifdef UNIT_TEST
#include <string>
#include <vector>
using String = std::string;
#else
#include <Arduino.h>
#include <vector>
#endif
struct ModelEntry { String name; int traces = 0; long inTok = 0, outTok = 0, totalTok = 0; float costUSD = 0; };
struct Day { String date; int traces = 0; long inTok = 0, outTok = 0, totalTok = 0; float costUSD = 0; std::vector<ModelEntry> models; bool synthetic = false; };
