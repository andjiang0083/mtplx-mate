# Changelog

## 0.1.0 — 2026-10-27

首个版本。给 MTPLX 用户的桌面小屏：

- **MTP 加速页**：今日投机解码接受率、每 verify 产出 token、平均 draft depth、depth 1/2/3 接受概率柱
- **实时速度页**：最新 decode tok/s、TTFT、缓存命中、服务显存占用、服务运行时长、24 点采样波形
- **省钱 / 用量 / 趋势 / 模型**四页沿用 oMLX Mate 的口径：DeepSeek 峰谷价 + 缓存价 × 本地电费
- 数据源改为 MTPLX 自有信源：增量索引 `~/.mtplx/logs/request-log-*.jsonl`，实时段取自 `/v1/mtplx/snapshot`
- 快照协议升到 v2（新增 `rt` / `mtp` 段），设备对缺段降级显示 `--`
- 本地控制台新增 MTP 接受率 KPI 与两种页面类型，预览同步
- 省电沿用上代：熄屏睡到按键（light sleep）、插线豁免熄屏、WiFi modem sleep、CPU 80MHz、功放断电
- 端口 8799，可与 oMLX Mate（8788）同机共存
- 琥珀配色，与 oMLX Mate 的青绿区分
