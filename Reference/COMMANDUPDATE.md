- Core Commands - 
exit - Exit the shell
echo [args] - Print arguments to the terminal
pwd - Print the current working directory path
cd [dir] - Change directory to specified path or HOME
whoami - Display the current session username
identity [name] - Set a cosmetic session username for the prompt

- Filesystem Commands - 
pulse [dir] - List files with size, date, type, and permissions
warp [dir] - Change directory with Void-styled output
vanish - Clear the terminal screen
echoes [file] - Display file contents with line numbers and syntax highlighting
shroud [name] [dir] - Create a new file or directory
transmute [old] [new] - Move or rename files and folders
veil [file] - Show detailed file permissions and metadata
seal [file] [mode] - Change file permissions via chmod wrapper
watch [dir] - Monitor a directory for changes every 2 seconds
scavenge [regex] - Recursively search files for a specific pattern or string
marrow [file] - Extract readable text strings from a binary file
dissect [binary] - Show assembly instructions of a compiled file
entropy-check - Analyze data density to find hidden or encrypted data
fossil [file] - Display access, modify, and change timestamps for an entity

- Process and Job Management - 
spawn [command] - Run a command in the background
jobs - List all active background jobs and their status
ps - List system processes by reading the proc filesystem
terminate [pid] - Kill a specific process by its ID
foreground [pid] - Bring a background job to the foreground
silence [pid] - Freeze a process without killing it (SIGSTOP)
awaken [pid] - Resume a frozen process (SIGCONT)

- Environment and Configuration -
env - Display all current environment variables
export KEY=VALUE - Set or update an environment variable
unset KEY - Remove an environment variable from the session
reload - Reload configuration settings from the .voidrc file
alias [name=command] - Create a custom command shortcut
unalias [name] - Remove an existing alias
aliases - List all active aliases
mimic [alias] - Find the true origin path of a command or alias

- Git Integration -
tether [git-command] - Execute any git command directly through the shell
anchor [url] [dir] - Clone a git repository from a remote source
sync - Pull the latest changes from the remote repository
transmit - Push local changes to the remote repository
commit [message] - Stage all changes and commit with the provided message
status - Display a rich formatted git status with branch information

- Project Awareness -
context - Display project type, git status, dependencies, and security hints

- Developer Utilities -
summon [file] - Execute Python, Node.js, or C files based on extension
forgepy - Launch an interactive Python REPL
multitool hash [type] [input] - Generate MD5 or SHA256 hashes
multitool base64 [mode] [input] - Encode or decode Base64 strings
multitool json [input] - Validate a JSON structure
multitool uuid - Generate a unique UUID v4
multitool timestamp [unix] - Convert between Unix and human-readable time
fabricate [name] - Generate a standard code template and compile it
refactor [string] - Perform project-wide search and replace for strings
minify - Strip comments and whitespace from code files
blueprint - Count lines of code and provide a language breakdown
debug-vision - Launch the internal debugger interface for binary analysis

- Networking Tools - 
intercept [url] - Preview HTTP request headers and body
ping [host] - Check network latency to a specific host
ports - Display all currently listening network ports
scan [host] - Perform a network port scan on a target host
serve [port] - Start a local HTTP server in the current directory
tunnel-info - Display network interfaces and local/public IP addresses
throb [interface] - Monitor real-time bandwidth on a network interface
rift [user@host] - Establish a remote SSH connection
wormhole [file] [dest] - Transfer a file to a remote destination via SCP
beacon [port] - Open a listening port for incoming signals
trace-route - Map the network path to a specific destination
ghost-ping - Send custom TCP/UDP packets to bypass firewalls

- System Monitoring -
entropy - Display live CPU, RAM, and Disk usage with process counts
pulse-graph - Display a scrolling ASCII sparkline of system resource usage
synapse [pid] - Trace system calls of a specific process
organelle - List physical storage drives and partitions
vitals - Display a stylized system hardware and OS summary
tempest - Read hardware thermal sensors and temperature data
neural-map - List active kernel modules
overclock-view - Display real-time clock speeds for all CPU cores
voltage - Check battery and AC power status

- Security and Automation -
startup-check - Run a security audit on Git, SSH, and environment files
perm-audit [dir] - Audit directory for world-writable or risky files
net-watch - Display all active network connections
incantation [file] - Grant execution permissions to a script and run it
black-hole [target] - Irreversibly delete a target without confirmation
echo-chamber - Compare two files and highlight the differences
stasis [time] - Pause shell execution for a set duration
black-box - Encrypt or decrypt a file using AES-256
signal-lost - Emergency killswitch to sever all network connections
chronos [time] [cmd] - Schedule a command to run at a specific time
singularity - Spawn containerized environments via docker-compose
glitch - Generate high-entropy random data or keys

- Plugin System - 
forge install [path] - Install a new plugin from a file path
forge list - List all currently installed plugins