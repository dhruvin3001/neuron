# 🧬 Neuron AI

A fast, safe, and intelligent command-line assistant powered by AI.

## Quick Start

1. **Get your API key** from GitHub Models or OpenAI
2. **Set your API key**:
   ```bash
   export NEURON_API_KEY=your_key_here
   ```
3. **Start using Neuron**:
   ```bash
   # Generate and execute commands
   neuron run "find large files"
   neuron run "install docker" --yes
   
   # Get explanations
   neuron tell "what is kubernetes"
   neuron tell "explain git rebase"
   ```

## Installation

### From Source
```bash
git clone https://github.com/Dhruvin711/neuron.git
cd neuron
mkdir build && cd build
cmake ..
make
```

### Prerequisites
- C++20 compatible compiler
- CMake 3.20+
- libcurl
- Internet connection

## Features

- ✅ **Safe command generation** with built-in danger detection
- ✅ **Cross-platform** support (macOS, Linux)
- ✅ **Interactive confirmations** for potentially dangerous operations
- ✅ **Command explanations** on demand
- ✅ **Fast native performance** (no Python dependencies)

## Usage Examples

### Generate Commands
```bash
# File operations
neuron run "show disk usage"
neuron run "find files modified today"

# System management
neuron run "check running processes"
neuron run "monitor system resources"

# Development tasks
neuron run "start a local web server"
neuron run "compress this folder"
```

### Get Explanations
```bash
neuron tell "difference between git merge and rebase"
neuron tell "how to set up nginx reverse proxy"
neuron tell "explain docker containers"
```

### Safety Features
```bash
# Dangerous commands will prompt for confirmation
neuron run "delete temporary files"
# ⚠️ WARNING: This command might be destructive...

# Use --yes to auto-execute safe commands
neuron run "list directory contents" --yes
```

## Configuration

Neuron looks for configuration in:
1. Environment variables
2. `.env` file in current directory

### Environment Variables
- `NEURON_API_KEY` - Your AI API key (required)

## Safety

Neuron includes built-in safety features:
- Detects potentially dangerous commands
- Requires confirmation for destructive operations
- Never suggests system-breaking commands
- Validates user input

## Contributing

This is an open-source project. Contributions welcome!

## License

MIT License - see LICENSE file for details.
