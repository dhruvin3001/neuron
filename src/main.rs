use clap::{Parser, Subcommand};

#[derive(Parser)]
#[command(name = "neuron", about = "AI-powered command line tool")]
struct Cli {
    #[command(subcommand)]
    command: Commands,
}

#[derive(Subcommand)]
enum Commands {
    /// Send a prompt and get a shell command
    Run {
        #[arg(value_name = "PROMPT")]
        prompt: Vec<String>, // allows multi-word prompts
    },
    /// Start interactive REPL session
    Shell,
}

fn main() {
    let cli = Cli::parse();

    match &cli.command {
        Commands::Run { prompt } => {
            let user_intput = prompt.join(" ");
            println!("User asked: {}", user_intput);
        }
        Commands::Shell => {
            println!("Starting NEURON shell...");
        }
    }
}
