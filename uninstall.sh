#!/bin/bash
# uninstall.sh — remove everything deployed by install.sh and restore backups.
# Thin wrapper around `install.sh --uninstall`.
set -Eeuo pipefail

REPO_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
exec "$REPO_DIR/install.sh" --uninstall "$@"
