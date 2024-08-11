#!/bin/bash
set -e

log() { echo "$(date): $1"; }
error() { log "ERROR: $1"; exit 1; }

export GIT_SSH_COMMAND="ssh -i /home/alfie/.ssh/id_rsa -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no"

# Function to check if git pull updated the repository
git_pull_and_check() {
    local repo_path="$1"
    local original_dir=$(pwd)
    cd "$repo_path" || error "Failed to change to directory: $repo_path"

    # Fetch updates
    git fetch origin || error "Failed to fetch updates for $repo_path"

    # Check if there are any changes
    local changes=$(git rev-list HEAD...origin/$(git rev-parse --abbrev-ref HEAD) --count)

    if [ "$changes" -gt 0 ]; then
        log "Updates found for $repo_path"
        git pull || error "Failed to pull updates for $repo_path"
        cd "$original_dir" || error "Failed to return to original directory"
        return 0
    else
        log "No updates found for $repo_path"
        cd "$original_dir" || error "Failed to return to original directory"
        return 1
    fi
}

MAIN_REPO="/home/alfie/codehub/cwww"
FIFISV_REPO="/home/alfie/.config/fifisv"
SERVICE_NAME="cwww.service"
BUILD_DIR="build"
DB_NAME="alfie.db"
DB="${BUILD_DIR}/${DB_NAME}"

cd "$MAIN_REPO" || error "Failed to change to main repository directory"

main_updated=false
second_updated=false

if git_pull_and_check "$MAIN_REPO"; then
    main_updated=true
fi

if git_pull_and_check "$FIFISV_REPO"; then
    second_updated=true
fi

if ! $main_updated && ! $second_updated; then
    log "No updates found in either repository. Exiting."
    exit 0
fi

cd "$MAIN_REPO" || error "Failed to change to main repository directory"

mkdir -p tmp || error "Failed to create tmp directory"
trap 'rm -rf ./tmp' EXIT

# DB BACKUP
if [ -f "$DB" ]; then
    echo "DB FOUND $DB"
    mv "$DB" tmp/ || error "Failed to backup database"
fi

# Prepare build directory
mkdir -p "$BUILD_DIR" || error "Failed to create build directory"
if [ -d "$BUILD_DIR" ]; then
    rm -rf "${BUILD_DIR:?}"/* || error "Failed to clean build directory"
fi

# Build
cd "$BUILD_DIR" || error "Failed to change to build directory"
cmake -DCMAKE_BUILD_TYPE=Release .. || error "CMake configuration failed"
cmake --build . || error "Build failed"
cd - || error "Failed to return to original directory"

# Restore DB
log "Restoring DB"
if [ -f "tmp/$DB_NAME" ]; then
    mv "tmp/$DB_NAME" "$DB" || error "Failed to restore database"
fi

log "Build completed successfully"

log "Restarting service $SERVICE_NAME"
systemctl is-active --quiet $SERVICE_NAME && systemctl restart $SERVICE_NAME