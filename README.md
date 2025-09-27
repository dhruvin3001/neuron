# 🧬 Neuron AI

A fast, safe, and intelligent command-line assistant powered by AI that automatically detects your operating system and provides system-specific command suggestions.

## Quick Start

### Option 1: Interactive Setup (Recommended)
```bash
# Run the interactive setup wizard
neuron setup

# This will guide you through:
# 1. Setting up your API key
# 2. Choosing your preferred AI model
# 3. Configuring other preferences
```

### Option 2: Manual Setup
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

## Setup

After installation, run the setup wizard to configure Neuron:

```bash
neuron setup
```

This interactive setup will:
1. **API Key Setup**: Enter your AI API key from GitHub Models or OpenAI
2. **Model Selection**: Choose from available AI models:
   - GPT-4 Omni (most capable)
   - GPT-4 Omni Mini (faster, cost-effective)
   - Claude 3.5 Sonnet (excellent reasoning)
   - Llama 3.1 70B (open source)

### Individual Setup Commands

```bash
# Set up API key only
neuron setup --api-key

# Choose model only
neuron setup --model
```

### Getting API Keys

**GitHub Models (Recommended)**:
1. Visit [GitHub Models](https://github.com/marketplace/models)
2. Sign in with your GitHub account
3. Generate a new API key
4. Use during setup or set `NEURON_API_KEY` environment variable

**OpenAI**:
1. Visit [OpenAI API Keys](https://platform.openai.com/api-keys)
2. Create an account and generate an API key
3. Note: Requires payment setup for usage

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

## Safety

Neuron includes built-in safety features:
- Detects potentially dangerous commands
- Requires confirmation for destructive operations
- Never suggests system-breaking commands
- Validates user input

```bash
# Dangerous commands will prompt for confirmation
neuron run "delete temporary files"
# ⚠️ WARNING: This command might be destructive...

# Use --yes to auto-execute safe commands
neuron run "list directory contents" --yes
```

## Configuration

### Setup Command
Neuron provides an interactive setup wizard to configure your system:

```bash
# Interactive setup wizard
neuron setup

# Individual setup options
neuron setup --api-key          # Set API key only
neuron setup --model            # Choose AI model only
neuron setup --show-config      # Display current configuration
```

### Configuration Sources
Neuron looks for configuration in this order:
1. Environment variables
2. `.env` file in current directory
3. User configuration file (`~/.neuron/config`)

### Environment Variables
- `NEURON_API_KEY` - Your AI API key (required)
- `NEURON_MODEL` - Preferred AI model (optional)
