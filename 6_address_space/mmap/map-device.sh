#!/bin/bash

# Navigate to the script directory
cd "$(dirname "$0")"

# Create a virtual environment if it does not exist
if [ ! -d "venv" ]; then
    echo "Creating virtual environment..."
    python3 -m venv venv
fi

# Activate the virtual environment
source venv/bin/activate

# Ensure the `hexdump` package is installed
if ! python -c "import hexdump" &>/dev/null; then
    echo "Installing hexdump..."
    pip install --quiet hexdump
fi

# Run the Python script with root privileges
echo "Running map-device.py with sudo..."
sudo venv/bin/python map-device.py
