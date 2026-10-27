# SPEC — MTPLX Mate 数据层与 MTP 仪表盘（v0.1.0）

沿用 oMLX Mate 的产品骨架（Mac agent 推送 / 设备只渲染 / 本地控制台），把数据源从
`~/.omlx/usage.sqlite3` 换成 MTPLX 的请求日志与实时接口，并新增两块 MTPLX 独有的仪表盘。

## 1. 数据源（实测于 MTPLX 2.12.2）

| 用途 | 来源 | 关键字段 |
|---|---|---|
| 历史用量（每请求一行） | `~/.mtplx/logs/request-log-<port>.jsonl` | `logged_at_s` `prompt_tokens` `cached_tokens` `new_prefill_tokens` `completion_tokens` `decode_elapsed_s` `request_elapsed_s` `ttft_s` `prefill_eval_time`(`prompt_eval_time_s`) `decode_tok_s` `mtp_depth` `verify_calls` `accepted_by_depth[]` `drafted_by_depth[]` `verify_time_s` `request_model` `served_model_id` |
| 实时状态 | `GET /v1/mtplx/snapshot`（本机端口） | `model_id` `lifetime{...}` `rolling{mean,p50,p95,live_history[]}` `latest{ttft_s,cached_tokens,prompt_tokens}` `mem{active_memory_bytes}` `uptime_s` `thermal` `active_requests` `in_flight[]` |
| 模型清单 | `GET /v1/models` | `data[].id` `context_length` |
| 服务版本 | `mtplx --version`（子进程，10 min 缓存） | `2.12.2` |
| 端口/主机 | `~/Library/Application Support/MTPLX/settings.json` | `port`(8001) `host`(127.0.0.1) |

端口发现顺序：`MTPLX_PORT` env → settings.json `port` → 8001；若探测失败再扫 8001..8010，
最后回退用日志文件名 `request-log-<port>.jsonl` 里最大的 port。

## 2. 索引（agent 侧）

`~/.mtplx-mate/usage.sqlite3`，单表 `req`（每个请求一行，索引 `ts`）：

```
ts INTEGER, model TEXT, prompt INT, cached INT, completion INT,
decode_s REAL, req_s REAL, ttft REAL, verify_s REAL, depth INT,
verify_calls INT, drafted INT, accepted INT,
acc1 INT, acc2 INT, acc3 INT, drf1 INT, drf2 INT, drf3 INT
```

增量读取：`files` 表存 `(path, inode, offset)`，每轮只读新增字节（jsonl 追加写，半行丢弃）。
首轮从「33 天前」开始全量回填；单轮最多读 N MB 保护。

## 3. 快照协议（agent → 设备，v2）

在 oMLX Mate v1 基础上把 `omlx` 段换成 `mtplx`，并加 `rt`、`mtp` 两段（设备缺段时降级显示 `--`）：

```jsonc
{
  "v": 2, "ok": 1, "ts": 0, "srv": "12:34:56", "peak": 0,
  "metrics": {"today|y|d7|d30": {net,api,elec,cs,pk,hit,tok,req}},   // 同 v1，口径见 §5
  "series":  {"d7":[7],"d30":[30],"hourly":[24]},
  "models":  [{"n","c","t","r"}],
  "mtplx":   {"up":1,"ver":"2.12.2","model":"flash-next","active":0},
  "rt":      {"tps":46.9,"ttft":0.37,"hit":0.999,"mem":83.5,"up":1027,
              "thermal":0,"live":[..24 个 tok/s..],"n":24},
  "mtp":     {"acc":0.31,"dep":3.0,"spd":2.9,"share":0.81,
              "d":[0.61,0.25,0.17]},   // 各 depth 平均接受概率
  "display": {...}, "cmd": {...}, "err": ""
}
```

## 4. 新增两块页面

- **PT_MTP（MTP 加速）**：Hero = 今日接受率（accepted/drafted 加权 `acc`），副行 = `每 verify 产出 spd tok · depth dep`，
  图 = depth 1/2/3 接受概率柱（实时值：取今日 + 上一小时滚动窗口）。
- **PT_SPEED（实时）**：Hero = 当前 decode tok/s（`rolling.mean`，in_flight 时取最新 `live_history`），
  副行 = `TTFT · 缓存命中 · 显存 mem GB · 服务 uptime`，图 = `live_history` 最近 24 点波形（事件型，无常驻）。

两页都随 refresh 更新，页面类型字符串 `mtp` / `speed`，控制台可选区间/图表沿用现有 4 种。

## 5. 省钱口径

与 oMLX Mate 完全一致（DeepSeek flash/pro 峰谷 + 缓存价 × 本地电费），只是 token 来自 MTPLX 请求日志。
`cached_tokens` 对应 MTPLX 的 SessionBank 前缀复用。

## 6. 兼容性

- 设备端 `parseSnapshot` 对缺失的 `rt`/`mtp` 段置 `valid=false`，页面显示 `--`，不崩。
- 老 agent（v1，只有 `omlx` 段）推送时：设备把 `omlx` 当 `mtplx` 读（字段名兼容：`up/ver/model`）。
- 端口/品牌全部走常量：`FW_NAME="MTPLX Mate"`、hostname `mtplx-mate-xxxx`、AP `MTPLX-Mate-XXXX`、NVS `mtmate`、LaunchAgent `com.mtplx.mate`。
