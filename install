#!/bin/bash
# ==============================================================================
#  aetherwm — automated installer for vxwm & dotfiles
# ==============================================================================
#  Usage:
#    ./install.sh [--all] [--wm] [--configs] [--dry-run]
#    ./install.sh --uninstall
#
#  Flags:
#    --wm        build and install the vxwm window manager from ./vxwm
#    --configs   deploy scripts, configs, wallpapers and .xinitrc
#    --all       do everything (default when no flag is given)
#    --dry-run   print what would be done without changing anything
#    --uninstall remove deployed files and restore the backup
# ==============================================================================
set -Eeuo pipefail

GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
CYAN='\033[0;36m'
NC='\033[0m'

REPO_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BACKUP_DIR="${XDG_CACHE_HOME:-$HOME/.cache}/aetherwm/backup.$(date +%Y%m%d-%H%M%S)"

DO_WM=0
DO_CONFIGS=0
DRY_RUN=0
UNINSTALL=0

info()  { echo -e "${CYAN}[*]${NC} $*"; }
ok()    { echo -e "${GREEN}[+]${NC} $*"; }
warn()  { echo -e "${YELLOW}[!]${NC} $*"; }
err()   { echo -e "${RED}[!]${NC} $*" >&2; }

failure_handler() {
    err "Installation failed. Run 'install.sh --uninstall' to roll back."
}
trap 'failure_handler' ERR

usage() {
    sed -n '3,17p' "${BASH_SOURCE[0]}"
    exit 0
}

# --- backup helper: move an existing path aside before it is overwritten -----
backup() {
    local src="$1"
    [ -e "$src" ] || return 0
    if [ "$DRY_RUN" = "1" ]; then
        info "would back up $src"
        return
    fi
    mkdir -p "$BACKUP_DIR"
    mv "$src" "$BACKUP_DIR/" 2>/dev/null || true
    info "backed up: $src"
}

copy_file() {
    local src="$1" dst="$2" mode="${3:-644}"
    if [ "$DRY_RUN" = "1" ]; then
        info "would install $src -> $dst (mode $mode)"
        return
    fi
    install -D -m "$mode" "$src" "$dst"
    ok "installed $dst"
}

copy_dir() {
    local src="$1" dst="$2"
    if [ "$DRY_RUN" = "1" ]; then
        info "would install directory $src -> $dst"
        return
    fi
    backup "$dst"
    cp -r "$src" "$dst"
    ok "installed $dst/"
}

# --- argument parsing --------------------------------------------------------
[ $# -eq 0 ] && DO_WM=1 && DO_CONFIGS=1
for arg in "$@"; do
    case "$arg" in
        --wm)        DO_WM=1 ;;
        --configs)   DO_CONFIGS=1 ;;
        --all)       DO_WM=1; DO_CONFIGS=1 ;;
        --dry-run)   DRY_RUN=1 ;;
        --uninstall) UNINSTALL=1 ;;
        --help|-h)   usage ;;
        *)           err "Unknown argument: $arg"; usage ;;
    esac
done

# --- uninstall ---------------------------------------------------------------
if [ "$UNINSTALL" = "1" ]; then
    info "Removing deployed aetherwm files..."
    for f in screenshot.sh wallpaper.sh volume.sh brightness.sh wal-dunst.sh; do
        [ "$DRY_RUN" = "1" ] && { info "would remove ~/$f"; continue; }
        rm -f "$HOME/$f" && ok "removed ~/$f"
    done
    for d in kitty rofi picom fish wal dunst; do
        if [ -d "$HOME/.config/$d" ]; then
            [ "$DRY_RUN" = "1" ] && { info "would remove ~/.config/$d"; continue; }
            rm -rf "$HOME/.config/$d" && ok "removed ~/.config/$d"
        fi
    done
    if [ -f "$HOME/.xinitrc" ]; then
        [ "$DRY_RUN" = "1" ] && info "would remove ~/.xinitrc" || { rm -f "$HOME/.xinitrc" && ok "removed ~/.xinitrc"; }
    fi

    if compgen -G "${XDG_CACHE_HOME:-$HOME/.cache}/aetherwm/backup.*" >/dev/null; then
        latest="$(ls -d "${XDG_CACHE_HOME:-$HOME/.cache}"/aetherwm/backup.* | sort | tail -1)"
        info "Restoring backup from $latest ..."
        if [ "$DRY_RUN" != "1" ]; then
            for entry in "$latest"/*; do
                [ -e "$entry" ] || continue
                base="$(basename "$entry")"
                rm -rf "$HOME/.config/$base"
                mv "$entry" "$HOME/.config/"
            done
            ok "backup restored"
        fi
    else
        warn "No backup found to restore."
    fi
    info "Uninstall finished. (vxwm binary is not removed; use: sudo make -C $REPO_DIR/vxwm uninstall)"
    exit 0
fi

# --- dependency installation ------------------------------------------------
PACKAGES_WM=(libx11 libxft libxinerama base-devel make)
PACKAGES_CONFIG=(
    kitty dunst xwallpaper picom rofi brightnessctl scrot xdotool
    thunar python-pywal lxappearance git xorg-xinit xterm
    papirus-icon-theme ttf-jetbrains-mono-nerd
)
# wpctl (volume.sh) comes from wireplumber; pipewire provides the audio stack.
PACKAGES_AUDIO=(pipewire pipewire-pulse wireplumber)

need_deps() {
    local missing=()
    for p in "$@"; do
        pacman -Q "$p" >/dev/null 2>&1 || missing+=("$p")
    done
    if [ ${#missing[@]} -gt 0 ]; then
        info "Installing missing packages: ${missing[*]}"
        if [ "$DRY_RUN" = "1" ]; then
            info "(dry-run) would run: sudo pacman -S --needed ${missing[*]}"
            return
        fi
        sudo pacman -S --needed --noconfirm "${missing[@]}"
        ok "packages installed"
    else
        ok "all dependencies already installed"
    fi
}

install_wm() {
    info "Building vxwm..."
    if [ "$DRY_RUN" = "1" ]; then
        info "(dry-run) would run: make -C $REPO_DIR/vxwm clean install"
        return
    fi
    need_deps "${PACKAGES_WM[@]}"
    make -C "$REPO_DIR/vxwm" clean >/dev/null
    make -C "$REPO_DIR/vxwm" install
    ok "vxwm installed to /usr/local/bin"
}

install_configs() {
    need_deps "${PACKAGES_CONFIG[@]}" "${PACKAGES_AUDIO[@]}"

    mkdir -p "$HOME/.config" "$HOME/.cache" "$HOME/Pictures/Screenshot" \
             "$HOME/Pictures/Wallpaper" "$HOME/.config/dunst"

    # scripts -> $HOME (keybinds in vxwm/config.def.h reference these paths)
    for script in screenshot.sh wallpaper.sh volume.sh brightness.sh wal-dunst.sh; do
        copy_file "$REPO_DIR/scripts/$script" "$HOME/$script" 755
    done

    # config directories
    copy_dir "$REPO_DIR/config/kitty"  "$HOME/.config/kitty"
    copy_dir "$REPO_DIR/config/rofi"   "$HOME/.config/rofi"
    copy_dir "$REPO_DIR/config/picom"  "$HOME/.config/picom"
    copy_dir "$REPO_DIR/config/fish"   "$HOME/.config/fish"
    copy_dir "$REPO_DIR/config/wal"    "$HOME/.config/wal"

    # dunst bootstrap config (wal-dunst.sh regenerates it from the pywal template)
    copy_file "$REPO_DIR/config/dunst/dunstrc" "$HOME/.config/dunst/dunstrc"

    # wallpapers
    info "Copying wallpapers to ~/Pictures/Wallpaper ..."
    if [ "$DRY_RUN" = "1" ]; then
        info "(dry-run) would copy $REPO_DIR/wallpapers/*.jpg to ~/Pictures/Wallpaper/"
    else
        cp -n "$REPO_DIR"/wallpapers/*.jpg "$HOME/Pictures/Wallpaper/" 2>/dev/null || true
        ok "wallpapers installed ($(ls "$HOME/Pictures/Wallpaper"/*.jpg 2>/dev/null | wc -l) images)"
    fi

    # xinitrc
    copy_file "$REPO_DIR/.xinitrc" "$HOME/.xinitrc" 755

    # If pywal has already run, derive the themed dunstrc from the template.
    if [ -f "$HOME/.cache/wal/dunst" ] && [ "$DRY_RUN" = "0" ]; then
        cp "$HOME/.cache/wal/dunst" "$HOME/.config/dunst/dunstrc"
        ok "dunstrc regenerated from pywal template"
    fi
}

# --- main --------------------------------------------------------------------
if [ "$DRY_RUN" = "1" ]; then
    info "DRY-RUN: nothing will be changed."
fi

[ "$DO_WM" = "1" ]      && install_wm
[ "$DO_CONFIGS" = "1" ] && install_configs

echo -e "\n${GREEN}Done. Start the session with:${NC} startx"
echo -e "  or add the following to your login manager:"
echo -e "  ${CYAN}exec /usr/local/bin/vxwm${NC}\n"
