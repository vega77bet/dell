#!/bin/bash
# Run in the SDK container. No access to the host X11 session is required.
set -euo pipefail
cd /src
export DISPLAY=:93
export PATH="/src/out/native:$PATH"
export QT_QUICK_BACKEND=software
export QT_MEDIA_BACKEND=ffmpeg
Xvfb :93 -screen 0 1280x800x24 -nolisten tcp &
xpid=$!
trap 'kill "$xpid" 2>/dev/null || true' EXIT
sleep 1
openbox --config-file /src/os/overlay/etc/xdg/openbox/dell-rc.xml >out/openbox-test.log 2>&1 &
dell-shell >out/shell-test.log 2>&1 &
shell_pid=$!
sleep 2
wmctrl -l | grep 'Dell OS Panel'
for mode in text paint files media capture settings; do
    dell-app "$mode" >"out/app-$mode.log" 2>&1 &
    pid=$!
    sleep 2
    kill -0 "$pid"
    wmctrl -l
    kill "$pid"
    wait "$pid" 2>/dev/null || true
done
# An X11 mouse event must reach a real application, not be swallowed by the WM.
dell-app text >out/mouse-test.log 2>&1 &
pid=$!
sleep 2
wid=$(xdotool search --onlyvisible --name '^Texto · Dell OS$' | head -1)
xdotool windowactivate --sync "$wid"
xdotool type --clearmodifiers 'mouse test'
xdotool mousemove --window "$wid" 42 24 click 1
sleep 1
test "$(xdotool getactivewindow getwindowname)" = Texto
xdotool key Escape
kill "$pid"
wait "$pid" 2>/dev/null || true
# Prove that the terminal executes a command in a real PTY rather than displaying text.
rm -f out/terminal-proof.txt
dell-app terminal -e /bin/bash -c 'printf "PTY_OK\n" > /src/out/terminal-proof.txt; sleep 3' >out/app-terminal.log 2>&1 &
pid=$!
sleep 2
grep -q PTY_OK out/terminal-proof.txt
wait "$pid"
out/native/dell-screenshot /src/out/native-desktop.png
kill "$shell_pid"
printf 'All native app launch and PTY smoke checks passed.\n'
