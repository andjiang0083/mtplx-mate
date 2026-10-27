#pragma once
#include <stdint.h>

// 一次推送里单个区间的数字 (agent 的 metrics.{today,y,d7,d30})
struct RangeData {
  bool valid = false;
  float net = 0;   // 净省 = API 账单 - 电费
  float api = 0;   // 折算的 DeepSeek 账单
  float elec = 0;  // 本地电费
  float cs = 0;    // 缓存命中省下的钱
  float pk = 0;    // 其中峰时段账单
  float hit = 0;   // 缓存命中率 0..1
  uint64_t tok = 0;
  uint32_t req = 0;
};

// MTP 投机解码指标 (agent 的 mtp 段, 今日累计)
struct MtpData {
  bool valid = false;
  float acc = 0;     // 接受率 0..1 = accepted / drafted
  float dep = 0;     // 平均 draft depth
  float spd = 0;     // 每次 verify 产出的 token 数
  float share = 0;   // verify 时间占比 0..1
  float d[3] = {};   // depth 1..3 的平均接受概率
};

// 实时指标 (agent 的 rt 段, 来自 MTPLX 的 /v1/mtplx/snapshot)
struct RtData {
  bool valid = false;
  float tps = 0;      // 最近一次 decode tok/s
  float ttft = 0;     // 首 token 延迟秒
  float hit = 0;      // 最近一次请求的缓存命中率 0..1
  float mem = 0;      // 服务占用统一内存 GB
  uint32_t up = 0;    // 服务 uptime 秒
  uint8_t press = 0;  // 系统内存压力档 (1 正常 / 2 警告 / 4 严重)
  uint8_t active = 0; // 正在生成的请求数
  uint8_t therm = 0;  // 温度压力档 0..3
  uint8_t n = 0;      // live 采样点数
  float live[24] = {};  // 最近 24 个 tok/s 采样
};

struct ModelRow {
  char name[28] = "";
  float cost = 0;
  uint64_t tok = 0;
  uint32_t req = 0;
};

struct Snapshot {
  bool ok = false;
  uint32_t ts = 0;
  char srv[10] = "";
  bool peak = false;
  RangeData today, y, d7, d30;
  float series7[7] = {};
  float series30[30] = {};
  float hourly[24] = {};
  ModelRow models[5];
  uint8_t modelCount = 0;
  MtpData mtp;
  RtData rt;
  bool mtplxUp = false;
  char mtplxVer[12] = "";
  char mtplxModel[28] = "";
  char err[72] = "";
  uint32_t receivedAtMs = 0;  // 本机 millis(), 用于判断数据新鲜度
};

// 按配置取区间
const RangeData& snapshotRange(const Snapshot& s, uint8_t range);
