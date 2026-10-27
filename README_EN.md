# MTPLX Mate

> MTPLX **speedup** and **savings**, on a 240x135 desktop screen.

![MTP speedup page](docs/images/device-mtp.png)

A desk companion for [MTPLX](https://github.com/youssofal/MTPLX) (local inference on Apple Silicon).
It shows something no other tool does -- **how much speculative decoding actually buys you**:

- **MTP dashboard**: today's acceptance rate, tokens produced per verify, acceptance curve by draft depth (1/2/3)
- **Realtime**: latest decode tok/s, TTFT, cache hit, memory in use, last 24 samples as a waveform
- **Savings**: what the same tokens would cost on DeepSeek, minus local electricity
- **Usage & trends**: today / yesterday / 7d / 30d tokens, requests, cache hit, per-model ranking

The device only displays. A Mac-side agent reads MTPLX's request log and live endpoint and POSTs a
snapshot every 5 seconds. MTPLX itself never has to listen on the LAN.

| MTP speedup | Realtime | Savings |
|---|---|---|
| ![mtp](docs/images/device-mtp.png) | ![speed](docs/images/device-speed.png) | ![savings](docs/images/device-savings.png) |

---

## Requirements

| Item | Requirement |
|---|---|
| Device | M5Stack StickS3 (ESP32-S3, 240x135) |
| Mac | Apple Silicon + macOS, with **MTPLX** installed and at least one request served |
| MTPLX | 2.12+ (uses `/v1/mtplx/snapshot` and request-log) |
| Python | 3.9+ (system Python is fine) |

> No MTPLX yet? Install it and run one conversation first -- otherwise MTPLX Mate can only show
> the setup / waiting screen.

---

## Getting started

### 1. Flash the firmware

Search **MTPLX Mate** in M5Burner, or flash the merged image with esptool:

```bash
esptool.py --chip esp32s3 write_flash 0x0 mtplx-mate-v0.1.0-merged.bin
```

Build it yourself:

```bash
pio run -e sticks3 -t upload --upload-port /dev/cu.usbmodem*
```

### 2. Join Wi-Fi (one time)

On first boot the screen shows a QR code:

1. Join `MTPLX-Mate-XXXX` from your phone (password `mtplxmate`)
2. Open `http://192.168.4.1`
3. Pick your 2.4 GHz network, enter the password, save
4. The device reboots and connects

### 3. Install the Mac agent (one time)

Once online, the device shows its own address. Open it on the Mac and run the command it gives you:

```bash
curl -sL http://mtplx-mate-xxxx.local/setup.sh | bash
```

That checks python3, installs `agent.py` into `~/.mtplx-mate/`, registers a LaunchAgent (auto-start),
pushes one snapshot and opens the console.

### 4. Done

The screen starts showing numbers within seconds. The console URL is written to `~/.mtplx-mate/url.txt`
(default `http://127.0.0.1:8799`).

Uninstall:

```bash
curl -sL http://mtplx-mate-xxxx.local/uninstall.sh | bash
```

---

## The six pages

Default order (add / remove / reorder in the console):

| # | Page | Content |
|---|---|---|
| 1 | **MTP speedup** | big number = today's acceptance rate; tokens per verify, mean depth; acceptance bars for depth 1/2/3 |
| 2 | **Realtime** | big number = latest decode tok/s; TTFT, cache hit, memory, uptime; 24-sample waveform |
| 3 | **Savings** | saved today (cloud bill - power), API estimate, power cost, cache savings |
| 4 | **Usage** | total tokens, requests, cache hit rate, current model |
| 5 | **Trend** | net savings for the selected range + 30d / 7d / hourly bars |
| 6 | **Models** | share of the bill per model, last 7 days |

Two more pages are available: **Cost** (cloud / power / cache / net grid) and **Device**
(Wi-Fi, IP, service version, model, battery).

Buttons:

- **A** next page / **B** previous page
- hold **B** to re-run Wi-Fi setup
- When charging the screen only dims; on battery it turns off after 2 minutes -- any key wakes it.

## Console

`http://127.0.0.1:8799` (auto-shifts if the port is taken; the real URL lands in `url.txt`):

- Four KPI cards: saved today / today's tokens / MTP acceptance / current model + live tok/s
- 30-day savings chart
- **Display config**: add or remove pages, pick type and range, compact mode, with a live 240x135 preview
- **Device control**: online state, battery, RSSI, current page; remote page flip / flash / reboot / remove
- Settings: reference pricing (deepseek-flash / pro), currency, electricity price, power draw, depreciation, push interval, language

---

## Troubleshooting

**Screen says "Connect Mac"** -- the device is online but the agent is not installed. Run the `curl` line from step 3.

**MTP page shows `--`** -- no requests today, or the last generation ran without MTP. Run a conversation and look again.

**Realtime shows `--`** -- MTPLX is not running (or not on port 8001). Start it with `mtplx serve`.

**Savings stay at 0** -- the agent needs at least one real request in the log: `mtplx ask "hi"`, then wait for the next push (5 s).

**Wi-Fi setup is needed again after a full flash** -- images written to `0x0` cover the whole flash including the config area. To upgrade the app only and keep credentials:

```bash
esptool.py --chip esp32s3 write_flash 0x10000 .pio/build/sticks3/firmware.bin
```

---

## How it works

```
MTPLX (127.0.0.1:8001)                     StickS3 (ESP32-S3)
   |  ~/.mtplx/logs/request-log-*.jsonl        |
   |  /v1/mtplx/snapshot (localhost)            |
   v                                           v
~/.mtplx-mate/agent.py  --POST /api/snapshot-->  8 page renderers
   (LaunchAgent)        <--state + cmd--------  A/B paging
   +-- 127.0.0.1:8799 local console
```

- **Incremental indexing**: the agent reads only new bytes of the log (tracked per inode + offset) and rolls them
  into a local SQLite (hour x model), backfilling 33 days once
- **The device being offline never loses data**: the Mac keeps computing and pushes the latest snapshot on reconnect
- **Commands ride back on the push response**, so no inbound port is opened on the Mac
- **Token auth**: the device generates a pairing token on first boot; only an agent holding it may push

---

## Development

```bash
pio run -e sticks3                    # build
pio run -e sticks3 -t upload          # flash app partition (keeps credentials)
bash tools/make_release.sh            # merged image + structure check + sha256
python3 ~/.mtplx-mate/agent.py --once # compute one snapshot, no push
python3 ~/.mtplx-mate/agent.py --serve --port 8799
```

Firmware and agent live in one repo: `tools/embed_assets.py` embeds the agent into the firmware, so the
device serves `setup.sh` / `agent.py` itself and installation does not depend on this repo being reachable.

`docs/` holds the data source and protocol (`SPEC-mtplx-data.md`), power behaviour (`SPEC-0.2.4-power.md`)
and the acceptance checklist (`ACCEPTANCE.md`).

## License

MIT. MTPLX is an independent third-party project (Apache-2.0); no MTPLX code is included here.
