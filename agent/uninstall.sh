#!/bin/bash
# MTPLX Mate — 卸载 Mac 端 agent
set -uo pipefail
echo ""
echo "  MTPLX Mate · 卸载"
echo "  ─────────────────────────────"
launchctl bootout "gui/$(id -u)/com.mtplx.mate" 2>/dev/null || true
rm -f "$HOME/Library/LaunchAgents/com.mtplx.mate.plist"
rm -rf "$HOME/.mtplx-mate"
echo "  ✅ 已移除后台服务、配置与日志"
echo "  设备上的数据仍在，重新运行 setup.sh 可再次安装。"
echo ""
