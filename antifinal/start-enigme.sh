#!/bin/sh
# Double-click launcher for GNOME Files/Nautilus when the binary itself won't run.
# chmod +x start-enigme.sh   then allow "Run as a program" if asked.
cd "$(dirname "$0")" || exit 1
exec ./enigme
