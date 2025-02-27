# RevXT Client Implementation

## Overview

This repository contains a game client implementation for the `RevXT` board game competition. The client connects to a game server, receives game state information, and makes strategic moves using advanced AI algorithms such as minimax with alpha-beta pruning and Best Reply Search (BRS).

## Game Description

RevXT is a strategic board game where players compete to control territory on a map. The game features:

- Multiple players (2-8 players supported)
- Various map layouts with special fields
- Two phases: placement phase and bomb phase
- Special stones and abilities:
  - **Overwrite stones**: Allow placing stones on occupied fields
  - **Bombs**: Remove stones from the board
  - **Special fields**: 'I' (inversion), 'C' (choice), 'B' (bonus)

## Getting Started

### Prerequisites

- C++ compiler (g++ recommended)
- Linux/Unix environment
- Make build system

### Compiling

The project uses a Makefile for compilation. To build the client:

```bash
# Regular build
make release

# Debug build with additional information
make debug

# Build with color output
make color
```

The compiled binary will be available in the `bin` directory.

### Running the Client

```bash
# Basic execution
./bin/client01

# Connect to a specific server
./bin/client01 -i <IP_ADDRESS> -p <PORT>

# Additional options
./bin/client01 -n  # Disable move sorting
./bin/client01 -q  # Enable quiet mode (minimal console output)
./bin/client01 -h  # Display help information
```

## Command Line Arguments

| Argument | Description |
|----------|-------------|
| `-i <ip>` | Specify server IP address (default: 127.0.0.1) |
| `-p <port>` | Specify server port (default: 7777) |
| `-n` | Disable move sorting (improves performance but may reduce strategic play) |
| `-q` | Enable quiet mode (reduces console output) |
| `-h` | Display help information |

## Project Structure

### Key Directories

- `/src/src`: Implementation files
- `/src/lib`: Header files
- `/compMaps`: Competition map files

### Key Components

#### Core Game Logic

- `Board`: Represents the game state including player positions, stones, and field types
- `Game`: Manages game flow and communication with the server
- `Initializer`: Reads and processes map data and prepares the board

#### AI Algorithm Components

- `Algorithms`: Implements minimax and Best Reply Search (BRS) strategies
- `MoveGenerator`: Generates valid moves for a given game state
- `MoveExecuter`: Executes moves and updates the game state
- `Evaluator`: Evaluates board positions and determines the best moves

#### Networking

- `Network`: Handles communication with the game server
- Supports protocol-specific message formatting and parsing

## Algorithm Overview

The client uses several advanced search algorithms:

1. **Minimax with Alpha-Beta Pruning**: A depth-first search algorithm that evaluates game states by looking ahead multiple moves and selecting the optimal path.

2. **Best Reply Search (BRS)**: A variation of minimax that is particularly effective in multiplayer games where more than two players are competing.

The search depth adapts dynamically based on:
- Current map occupation percentage
- Available time for decision-making
- Map size and complexity

## Configuration Settings

Key algorithm parameters can be adjusted in the header files:

- `MAX_SEARCH_DEPTH`: Maximum depth for search algorithms (default: 20)
- `MEMORY_SIZE_WITH_BUFFER`: Memory allocation for move calculation
- Various evaluation parameters in `evaluator.hpp`

## Evaluation Metrics

The position evaluation considers multiple factors:

- Stone count and distribution
- Protected fields
- Special field control
- Move mobility
- Strategic positions (corners, walls)

## Current Development Status

This client has been successfully tested in competition environments and performs well against other implementations. The core algorithms are stable and effective, but there's always room for fine-tuning the evaluation parameters for specific map types.

## Competition Resources

- FightClub: https://172.20.0.41:8091
- MatchPoint: http://172.20.0.41:8088/current
- MatchPoint - Aachen: https://matchpoint.moves.rwth-aachen.de/current
