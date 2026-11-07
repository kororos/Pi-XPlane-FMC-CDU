#!/usr/bin/env bash
# deploy.sh -- install a cross-built Pi-XPlane-FMC-CDU on the Pi as the
# boot service.
#
#   ./deploy.sh [user@pi] [build-dir]      (defaults: pi@192.168.1.217 build-pi)
#
# Copies piXPlaneFMCCDU, testFMCLED, testFMCKeypad and the systemd service
# to the Pi, installs them in /usr/bin and /etc/systemd/system, keeps the
# previous program as /usr/bin/piXPlaneFMCCDU.prev, and (re)starts the
# service, which is enabled to start at boot.
# Tip: run `ssh-copy-id pi@<pi>` once to avoid password prompts.
set -euo pipefail

PI="${1:-pi@192.168.1.217}"
BUILD="${2:-build-pi}"
cd "$(dirname "$0")"
BIN="$BUILD/src/piXPlaneFMCCDU"
SERVICE="root/etc/systemd/system/piXPlaneFMCCDU.service"
FILES=("$BIN/piXPlaneFMCCDU" "$BIN/testFMCLED" "$BIN/testFMCKeypad" "$SERVICE")

for f in "${FILES[@]}"; do
  [ -f "$f" ] || { echo "Missing $f - build first (cmake/make in $BUILD)"; exit 1; }
done
file "$BIN/piXPlaneFMCCDU" | grep -q "ARM" \
  || { echo "$BIN/piXPlaneFMCCDU is not an ARM program - wrong build folder?"; exit 1; }

# one SSH connection for all steps, so the password is asked only once
SSH_OPTS=(-o ControlMaster=auto -o "ControlPath=/tmp/fmc-deploy-%r@%h:%p" -o ControlPersist=60)

echo "==> Copying to $PI"
ssh "${SSH_OPTS[@]}" "$PI" "rm -rf /tmp/fmc-deploy && mkdir -p /tmp/fmc-deploy"
scp "${SSH_OPTS[@]}" -q "${FILES[@]}" "$PI:/tmp/fmc-deploy/"

echo "==> Installing on $PI (sudo)"
ssh "${SSH_OPTS[@]}" -t "$PI" '
set -e
cd /tmp/fmc-deploy
sudo systemctl stop piXPlaneFMCCDU 2>/dev/null || true
if [ -f /usr/bin/piXPlaneFMCCDU ]; then sudo cp -p /usr/bin/piXPlaneFMCCDU /usr/bin/piXPlaneFMCCDU.prev; fi
sudo install -m 755 piXPlaneFMCCDU testFMCLED testFMCKeypad /usr/bin/
sudo install -m 644 piXPlaneFMCCDU.service /etc/systemd/system/
sudo systemctl daemon-reload
sudo systemctl enable --quiet piXPlaneFMCCDU
sudo systemctl start piXPlaneFMCCDU
rm -rf /tmp/fmc-deploy
echo
echo "Installed: $(/usr/bin/piXPlaneFMCCDU --version)"
echo "Service:   $(systemctl is-active piXPlaneFMCCDU), starts at boot: $(systemctl is-enabled piXPlaneFMCCDU)"
echo "Previous program kept as /usr/bin/piXPlaneFMCCDU.prev"
'
ssh "${SSH_OPTS[@]}" -O exit "$PI" 2>/dev/null || true
