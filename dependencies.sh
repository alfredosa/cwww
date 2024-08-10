#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Default directories (can be overridden by environment variables)
MD4C_DIR="${MD4C_DIR:-md4c}"
BLOG_POSTS_DIR="${BLOG_POSTS_DIR:-/home/${USER}/.config/fifisv}"

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to install a package if it's not already installed
install_if_missing() {
    if ! command_exists "$1"; then
        echo "Installing $1..."
        sudo apt install -y "$1"
    else
        echo "$1 is already installed."
    fi
}

# Cache sudo credentials
sudo -v

# Update package lists
sudo apt update

# Install essential build tools and libraries
install_if_missing build-essential
install_if_missing git
install_if_missing libsqlite3-dev

# Install md4c if not already installed
if ! pkg-config --modversion md4c >/dev/null 2>&1; then
    echo "Installing md4c..."
    git clone https://github.com/mity/md4c.git "$MD4C_DIR" || { echo "Failed to clone md4c repository"; exit 1; }
    cd "$MD4C_DIR"
    mkdir -p build && cd build
    cmake .. || { echo "cmake configuration failed"; exit 1; }
    make || { echo "make failed"; exit 1; }
    sudo make install || { echo "make install failed"; exit 1; }
    sudo ldconfig
    cd ../..
else
    echo "md4c is already installed."
fi

# Verify installations
echo "Verifying installations:"

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

# Clean up
echo "Cleaning up..."
if [ -d "$MD4C_DIR" ]; then
    rm -rf "$MD4C_DIR"
    echo "$MD4C_DIR directory removed."
else
    echo "$MD4C_DIR directory not found. Skipping cleanup."
fi

# Install Fifi's Vault / blog posts
echo "Installing Fifi's Vault / blog posts"
if ! git clone git@github.com:alfredosa/fifisv.git "$BLOG_POSTS_DIR"; then
    echo "Failed to clone Fifi's Vault repository"
    exit 1
fi

echo "Script execution completed successfully!"