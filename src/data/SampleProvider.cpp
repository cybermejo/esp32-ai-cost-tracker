#include "SampleProvider.h"
static ModelEntry M(const char* n, int t, long i, long o, long tot, float c) {
  ModelEntry m; m.name = n; m.traces = t; m.inTok = i; m.outTok = o; m.totalTok = tot; m.costUSD = c; return m;
}
bool SampleProvider::load(std::vector<Day>& out) {
  out.clear(); Day d;
  d = Day(); d.date = "2026-09-04"; d.traces = 637; d.inTok = 29349979; d.outTok = 266524; d.totalTok = 41112311; d.costUSD = 1.2095f;
  d.models = {
    M("openrouter/z-ai/glm-5.3", 12, 131424, 23468, 1600716, 0.6632f),
    M("openrouter/deepseek/deepseek-v4-flash-0731", 10, 691418, 7437, 811367, 0.3172f),
    M("openrouter/z-ai/glm-5.3-flash", 105, 743769, 97233, 10778474, 0.2292f),
    M("openai/Qwen/Qwen3.5-27B", 3, 71610, 100, 71710, 0.0f),
    M("deepseek-v4-flash-0731-gmi-ray", 10, 0, 0, 0, 0.0f),
    M("openai/Qwen/Qwen3.8-27B", 492, 27615507, 135391, 27750898, 0.0f),
    M("openai/deepseek-ai/DeepSeek-V4-Flash-0731", 5, 96251, 2895, 99146, 0.0f),
  }; out.push_back(d);
  d = Day(); d.date = "2026-09-03"; d.traces = 383; d.inTok = 8672062; d.outTok = 329513; d.totalTok = 36996647; d.costUSD = 2.9003f;
  d.models = {
    M("z-ai/glm-5.3", 64, 115714, 184318, 6126528, 2.4879f),
    M("openrouter/z-ai/glm-5.3-flash", 169, 266588, 89486, 21753130, 0.3633f),
    M("z-ai/glm-5.3-flash", 12, 300781, 19067, 1091368, 0.0389f),
    M("openrouter/deepseek/deepseek-v4-flash-0731", 1, 20632, 2748, 23380, 0.0102f),
    M("openai/Qwen/Qwen3.8-27B", 8, 129157, 311, 129468, 0.0f),
    M("openai/Qwen/Qwen3.8-27B", 13, 332551, 2442, 334993, 0.0f),
    M("openai/Qwen/Qwen3.8-27B", 40, 1817212, 14972, 1832184, 0.0f),
    M("qwen3.8-max[1m]", 8, 0, 0, 0, 0.0f),
    M("deepseek-v4-flash-0731-gmi-ray", 2, 0, 0, 0, 0.0f),
    M("openai/Qwen/Qwen3.5-27B", 4, 3291, 62, 3353, 0.0f),
    M("glm-5.3-flash", 2, 0, 0, 0, 0.0f),
    M("Qwen/Qwen3.5-27B", 60, 5686136, 16107, 5702243, 0.0f),
  }; out.push_back(d);
  d = Day(); d.date = "2026-09-02"; d.traces = 889; d.inTok = 71496361; d.outTok = 577485; d.totalTok = 74040864; d.costUSD = 0.6147f;
  d.models = {
    M("deepseek/deepseek-v4-flash-0731", 27, 999631, 42486, 2904047, 0.4176f),
    M("openrouter/deepseek/deepseek-v4-flash-0731", 7, 359794, 14113, 478995, 0.1427f),
    M("DeepSeek-V4-Flash-0731", 2, 198024, 4666, 202690, 0.04f),
    M("azure/DeepSeek-V4-Flash-0731", 1, 75096, 305, 75401, 0.0144f),
    M("Qwen/Qwen3.5-27B", 437, 44526153, 374892, 44901045, 0.0f),
    M("deepseek-v4-flash-0731-gmi-ray", 12, 0, 0, 0, 0.0f),
    M("Qwen/Qwen3.8-27B", 1, 0, 0, 0, 0.0f),
    M("deepseek-v4-flash-0731-gmi-ray", 34, 0, 0, 0, 0.0f),
    M("openai/Qwen/Qwen3.8-27B", 350, 24438048, 120900, 24558948, 0.0f),
    M("openai/deepseek-ai/DeepSeek-V4-Flash-0731", 10, 260686, 6704, 267390, 0.0f),
    M("deepseek-ai/DeepSeek-V4-Flash-0731", 8, 638929, 13419, 652348, 0.0f),
  }; out.push_back(d);
  d = Day(); d.date = "2026-09-01"; d.traces = 2175; d.inTok = 56581331; d.outTok = 1000098; d.totalTok = 93327721; d.costUSD = 7.6579f;
  d.models = {
    M("deepseek/deepseek-v4-flash-0731", 306, 9268679, 288732, 29509568, 3.875f),
    M("openrouter/deepseek/deepseek-v4-flash-0731", 244, 6576885, 389138, 19500220, 2.9212f),
    M("DeepSeek-V4-Flash-0731", 47, 2208856, 29020, 4557492, 0.4994f),
    M("azure/DeepSeek-V4-Flash-0731", 23, 710400, 3703, 1452919, 0.1576f),
    M("anthropic/claude-sonnet-5", 3, 52994, 364, 152235, 0.1557f),
    M("openrouter/deepseek/deepseek-v4-flash-0731", 16, 111919, 4950, 219498, 0.049f),
    M("deepseek-v4-flash-0731-gmi-ray", 470, 0, 0, 0, 0.0f),
    M("Qwen/Qwen3.8-27B", 2, 0, 0, 0, 0.0f),
    M("Qwen/Qwen3.5-27B", 112, 11779118, 100982, 11880100, 0.0f),
    M("openai/Qwen/Qwen3.8-27B", 368, 21011922, 129872, 21141794, 0.0f),
    M("openai/deepseek-ai/DeepSeek-V4-Flash-0731", 3, 0, 0, 0, 0.0f),
    M("deepseek-ai/DeepSeek-V4-Flash-0731", 56, 3099998, 31966, 3131964, 0.0f),
    M("openai/deepseek-ai/DeepSeek-V4-Flash-0731", 222, 1760560, 21371, 1781931, 0.0f),
    M("deepseek-v4-flash-0731-gmi-ray", 10, 0, 0, 0, 0.0f),
    M("deepseek-v4-flash-0731-gmi-ray", 293, 0, 0, 0, 0.0f),
  }; out.push_back(d);
  d = Day(); d.date = "2026-08-31"; d.synthetic = true; d.traces = 410; d.inTok = 5200000; d.outTok = 180000; d.totalTok = 19000000; d.costUSD = 0.90f;
  d.models = {
    M("z-ai/glm-5.3-flash", 120, 900000, 60000, 7000000, 0.55f),
    M("deepseek/deepseek-v4-flash-0731", 60, 1500000, 40000, 4000000, 0.35f),
    M("Qwen/Qwen3.5-27B", 230, 2800000, 80000, 8000000, 0.0f),
  }; out.push_back(d);
  d = Day(); d.date = "2026-08-30"; d.synthetic = true; d.traces = 980; d.inTok = 20000000; d.outTok = 400000; d.totalTok = 45000000; d.costUSD = 3.20f;
  d.models = {
    M("z-ai/glm-5.3", 90, 3000000, 150000, 9000000, 2.10f),
    M("openrouter/deepseek/deepseek-v4-flash-0731", 140, 5000000, 100000, 12000000, 1.10f),
    M("openai/Qwen/Qwen3.8-27B", 750, 12000000, 150000, 24000000, 0.0f),
  }; out.push_back(d);
  d = Day(); d.date = "2026-08-29"; d.synthetic = true; d.traces = 520; d.inTok = 9000000; d.outTok = 220000; d.totalTok = 26000000; d.costUSD = 1.80f;
  d.models = {
    M("openrouter/z-ai/glm-5.3", 80, 2500000, 90000, 8000000, 1.20f),
    M("deepseek/deepseek-v4-flash-0731", 90, 2000000, 50000, 6000000, 0.60f),
    M("Qwen/Qwen3.5-27B", 350, 4500000, 80000, 12000000, 0.0f),
  }; out.push_back(d);
  return true;
}
