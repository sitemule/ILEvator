#!/usr/bin/env bash
# sync_build.sh — [optionally] rsync to IBM i then run gmake via SSH
#
# Usage: .sitemule/sync_build.sh [gmake-target] [VAR=value ...]
#
# Config (in .env or as environment variables):
#   I_HOST       SSH hostname or alias for the IBM i
#   DELPOY_PATH  Deployment path on the IBM i  (e.g. /prj/ILEvator)
#   SKIP_RSYNC   Set to "true" to force-skip rsync (auto-detected for network mounts)
#
# DIR=<folder> runs gmake in that folder of the project (examples, issues, ...).
# When the build writes error.txt (the makefiles' "current" / "example" targets send the
# compiler output there), it is shown afterwards, so VS Code's problem matcher sees it.
#
# Examples:
#   .sitemule/sync_build.sh all
#   .sitemule/sync_build.sh current SRC=src/ilevator.c OUTPUT=*PRINT
#   .sitemule/sync_build.sh compile DIR=examples "SRC=IVEX01-Simple GET request.rpgle"
#   .sitemule/sync_build.sh sync-only

# Load .env if present (does not override already-exported vars)
if [ -f ".env" ]; then
    set -o allexport
    # shellcheck disable=SC1091
    source .env
    set +o allexport
fi

: "${I_HOST:?I_HOST is not set. Add it to .env or export it before running.}"
: "${DELPOY_PATH:?DELPOY_PATH is not set. Add it to .env or export it before running.}"

MAKE_TARGET="${1:-all}"
shift 1 || true

# The remaining args are make variables; DIR= is ours. Quote each one for the remote
# shell - file names may contain blanks.
BUILD_DIR="$DELPOY_PATH"
MAKE_VARS=""
for arg in "$@"; do
    case "$arg" in
        DIR=*) BUILD_DIR="$DELPOY_PATH/${arg#DIR=}" ;;
        *)     MAKE_VARS="$MAKE_VARS $(printf '%q' "$arg")" ;;
    esac
done

# ---------------------------------------------------------------
# Auto-detect if the workspace is on a network/IFS share.
# When the project folder is mounted from the IBM i over SMB/NFS
# and opened directly (rather than cloned locally), rsync is not
# needed — edits already land on the IFS.
# ---------------------------------------------------------------
if [ -z "$SKIP_RSYNC" ]; then
    _fs=$(df -P . 2>/dev/null | awk 'NR==2{print $1}')
    case "$_fs" in
        //*) SKIP_RSYNC=true ;;
        *)   SKIP_RSYNC=false ;;
    esac
fi

# ---------------------------------------------------------------
# 1. Sync source files (local clone → IBM i only)
#
# --inplace: write directly to the target file instead of using a
#   temp file + rename. Avoids "Device busy (16)" on IBM i IFS when
#   another job has the directory open.
# --ignore-errors: treat per-file errors as warnings so the build
#   still runs even if one locked file couldn't be updated.
# ---------------------------------------------------------------
if [ "$SKIP_RSYNC" = "true" ]; then
    echo ">>> Working from IFS share — skipping rsync."
else
    echo ">>> Syncing to $I_HOST:$DELPOY_PATH ..."
    rsync -az \
        --inplace \
        --ignore-errors \
        --exclude '.git/' \
        --exclude '.gitignore' \
        --exclude '.gitattributes' \
        --exclude '.vscode/' \
        --exclude '.DS_Store' \
        --exclude '.claude/' \
        --exclude '.github/' \
        --exclude 'error.txt' \
        --exclude 'examples/errorlist.txt' \
        --rsync-path="/QOpenSys/pkgs/bin/rsync" \
        --omit-dir-times \
        --no-perms \
        "./" \
        "$I_HOST:$DELPOY_PATH" || echo ">>> rsync finished with warnings (continuing to build)"
    # Leave a note on the IFS: this folder is a copy of the git repo, edits here are lost
    .sitemule/git_managed_note.sh "$I_HOST" "$DELPOY_PATH"
fi

# ---------------------------------------------------------------
# 2. Build on IBM i (skip if target is sync-only)
# ---------------------------------------------------------------
if [ "$MAKE_TARGET" = "sync-only" ]; then
    echo ">>> Sync complete (no build requested)."
    exit 0
fi

echo ">>> Building in $BUILD_DIR: gmake $MAKE_TARGET$MAKE_VARS"
ssh "$I_HOST" "PATH=/QOpenSys/pkgs/bin:\$PATH
    cd '$BUILD_DIR' || exit 1
    rm -f error.txt
    gmake $MAKE_TARGET$MAKE_VARS; rc=\$?
    [ -s error.txt ] && cat error.txt
    exit \$rc"
