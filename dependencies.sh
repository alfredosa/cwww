#!/bin/bash

set -e # Exit immediately if a command exits with a non-zero status.

MD4C_DIR="md4c"

sudo apt update
sudo apt install -y build-essential cmake git libsqlite3-dev

git clone https://github.com/mity/md4c.git "$MD4C_DIR"
cd "$MD4C_DIR"
mkdir build && cd build
cmake ..
make
sudo make install

sudo ldconfig

echo "Checking md4c installation:"
if ! pkg-config --modversion md4c; then
    echo "md4c installation failed"
    exit 1
fi

echo "Checking SQLite3 installation:"
if ! sqlite3 --version; then
    echo "SQLite3 installation failed"
    exit 1
fi

echo "Installation complete!"

echo "Cleaning up..."
cd ../..
if [ -d "$MD4C_DIR" ]; then
    rm -rf "$MD4C_DIR"
    echo "$MD4C_DIR directory removed."
else
    echo "$MD4C_DIR directory not found. Skipping cleanup."
fi
