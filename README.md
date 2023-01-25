# Overview

This project is a directory sniffer that monitors a specific directory for text files, and whenever a new file is created, it sends the file to a worker process to extract URLs from the file. The worker process counts the URLs and writes them to an output file in the "files" directory. The manager process uses inotifywait to monitor the directory and creates new worker processes as needed.

# Compiling the program
Use the command "make" to compile the program. This will create one executables, "manager".

# Running the program
To run the "manager" executable, use the command:

`./manager -p <directory_path>`
where -p is the directory path that you want to monitor. if no path is provided it will use the current path.

# Functionality
* The manager process creates a new child process to run inotifywait and monitors the specified directory for new files.
* The worker process reads the file sent by the manager and extracts URLs from the file, counting the occurrences of each URL.
* The worker process then writes the URLs and their count to an output file in the "files" directory.
* If a worker process finishes processing a file and there is another file waiting to be processed, it takes the next file. If no files are waiting, the worker process sends a signal to the manager and goes to sleep.
* The manager process maintains a queue of available worker processes and assigns new files to them as they become available.
* The manager process also handles signals from the user (SIGINT) and worker processes (SIGCHLD) to terminate the program gracefully.


