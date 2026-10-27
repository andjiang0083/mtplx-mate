MTPLX Mate v0.1.0 — 把 MTPLX 的加速和省下的钱放到 240×135 桌面小屏。

**首个版本**：MTP 加速页（今日接受率、每次 verify 产出、各档 depth 接受曲线）+ 实时速度页（tok/s、TTFT、缓存命中、显存、24 点波形）+ 省钱/用量/趋势/模型四页。

## 刷机

```bash
esptool.py --chip esp32s3 write_flash 0x0 mtplx-mate-v0.1.0-merged.bin
```

或在 M5Burner 里搜 **MTPLX Mate**（审核通过后）。

## 装 Mac 端

设备连上 Wi-Fi 后屏幕会显示它自己的地址，在 Mac 上打开并执行：

```bash
curl -sL http://mtplx-mate-xxxx.local/setup.sh | bash
```

## 前提

- M5Stack StickS3
- Apple Silicon Mac + 已安装 **MTPLX 2.12+**，且至少跑过一次请求（否则设备只能显示等待页）

## 校验

```
sha256  2da7a09b30cdcadf596453a40dd61a6a76e07bc2dbca20449d5e770f591acaf9
```

开机横幅：`==== MTPLX Mate v0.1.0 ==== `（`BOOT host=mtplx-mate-xxxx.local`）

## 已知限制

- 只支持 StickS3；Cardputer ADV 版待验证
- 全片镜像（0x0）会清掉配网信息 —— 升级可用 `write_flash 0x10000 firmware.bin` 保留
- Windows / Intel Mac 不受支持（MTPLX 只跑 Apple Silicon）

## What's inside (EN)

First release: MTP speedup page (acceptance rate, tokens per verify, per-depth acceptance curve), realtime page
(tok/s, TTFT, cache hit, memory, 24-sample waveform), plus savings / usage / trend / models.
Requires an M5StickS3 and MTPLX 2.12+ running on an Apple Silicon Mac.
