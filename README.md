# Wireless Sensor Network with Contiki-NG

This project implements a wireless sensor network using Contiki-NG's NullNet protocol. It consists of a parent node that collects temperature and humidity data from multiple leaf nodes, which then gets stored in a MongoDB database in Atlas Cloud.

## System Architecture

The system is composed of:

1. **Leaf Nodes**: Sensor motes that periodically measure temperature and humidity using SHT11 sensors
2. **Parent Node**: A central node that receives data from all leaf nodes and forwards it to the backend
3. **Backend**: A Python script that processes the received data and stores it in MongoDB

## Components

### Contiki-NG Code
- `nullnet-broadcast.c`: Implementation of the parent node that receives data from leaf nodes
- `nullnet-unicast.c`: Implementation of the leaf nodes that send sensor data to the parent
- `utils.h`: Contains utility functions for data conversion

### Backend
- `backend.py`: Python script that reads data from a named pipe, parses it, and stores it in MongoDB

## How It Works

1. The parent node broadcasts its address periodically
2. Leaf nodes receive this broadcast and register with the parent by sending their address
3. The parent responds to each leaf with a unique ID
4. Leaf nodes periodically collect temperature and humidity data and send it to the parent
5. The parent receives the data and logs it to stdout
6. The backend script reads the logs through a named pipe and stores the data in a MongoDB database in Atlas Cloud

## Prerequisites

- Contiki-NG development environment
- Python 3.x
- MongoDB account with connection string
- Required Python packages: pymongo, python-dotenv, pyserial

## Setup and Installation

### 1. General Setup
- Follow instructions in [this repo](https://github.com/IoTLabUpatras/End-to-end-WSN-Project-2025)
  
**Make sure to keep contiki-ng in the parent folder of the project.**

### 2. Project Setup

1. Place the project files in a directory structure like:
```
my-project/
├── Makefile
├── nullnet-broadcast.c
├── nullnet-unicast.c
├── backend.py
└── .env  # Create this file with your MongoDB URI
```

2. Create a `.env` file with your MongoDB connection string:
```
MONGODB_URI=mongodb+srv://username:password@your-cluster.mongodb.net/
```

3. Create a named pipe for communication:
```bash
mkdir -p tmp
mkfifo tmp/out_pipe
```

### 3. Building the Firmware

#### For TelosB Devices 
##### Replace X for your specific configuration (commonly 0 for the 1st mote etc)

```bash
make TARGET=sky PORT=/dev/ttyUSBX
```

## Running the System

### 1. Start the Backend

```bash
python backend.py
```

### 2. Flash the Devices
##### Replace X and Y for your specific configuration (commonly 0 for the 1st mote etc)

For the parent node:
```bash
make TARGET=sky nullnet-broadcast.upload PORT=/dev/ttyUSBX
```

For leaf nodes:
```bash
make TARGET=z1 nullnet-unicast.upload PORT=/dev/ttyUSBY
```

### 3. Monitoring

Redirect the output to the named pipe:
```bash
make TARGET=sky login PORT=/dev/ttyUSBX > tmp/out_pipe
```


If you want to monitor the output of a specific mote:
```bash
make TARGET=sky login PORT=/dev/ttyUSBX
```


## Data Format

The data sent from leaf nodes to the parent has the following structure:

- **ID**: Unique identifier assigned by the parent
- **Count**: Message counter to track transmission sequence
- **Temperature**: Measured temperature in Celsius
- **Humidity**: Measured humidity percentage

The MongoDB collection stores this data along with a timestamp.

## Features

- **Auto-discovery**: Leaf nodes automatically find and register with the parent node
- **Unique ID assignment**: Each leaf node receives a unique identifier
- **Periodic sensing**: Configurable intervals for sensor readings
- **Real-time database storage**: All readings are stored in MongoDB with timestamps

## Troubleshooting

- If devices are not connecting, check that they are within wireless range
- Verify that the MongoDB connection string is correct in the `.env` file
- Ensure the named pipe exists and has the correct permissions
