#!/bin/bash
set -e

log() { echo "$(date): $1"; }
error() { log "ERROR: $1"; exit 1; }

BUILD_DIR="debug_build"
DB_NAME="alfie.db"
DB="${BUILD_DIR}/${DB_NAME}"

# Cache sudo credentials
sudo -v || error "Failed to cache sudo credentials"

# Cleanup on exit
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
cmake -DCMAKE_BUILD_TYPE=Debug .. || error "CMake configuration failed"
cmake --build . || error "Build failed"
cd - || error "Failed to return to original directory"

# Restore DB
log "Restoring DB"
if [ -f "tmp/$DB_NAME" ]; then
    mv "tmp/$DB_NAME" "$DB" || error "Failed to restore database"
fi

log "Build completed successfully"