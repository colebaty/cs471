# PRODCONS

## Cole Baty and Amelia Ragsdale

### for CS471, Dr. Ravi Mukkamala, Fall 2022, Old Dominion Univrsity

## Requirements

This program was developed and tested on the following operating systems:

* Ubuntu >= 20.04 (5.4.0) (ODU CS Linux server)
* g++ >= 9.4.0 (ODU CS Linux server)

All binaries and shell scripts included in these folders were tested on the ODU 
CS Department Linux servers on December 2, 2022, where their intended behavior 
was observed.

## Scripts

The following scripts are provided to aid in automatically compiling and running
the program.

## `run.sh`

This script will run the program for each of the values given in the
instructions, placing the output of each run in a file named `sample-p-f`,
where `p` is the page size, and `f` is the number of frames allocated to a 
process.

It will compile the program if there is no executable named `main` in the 
same directory. It will also compile the program if the `main` binary in the
same directory is older than 15 minutes.

## `stats.sh`
This script compiles the data from the sample runs into a file named `stats.csv`
for import into spreadsheet applications for further analysis.  

If there are no samples in the current directory, the script calls `run.sh` to
generate samples, and then compiles them into the csv file `stats.csv`

## Compiling and Running

To compile manually from source code, run this command.
```bash
g++ -fpermissive -std=c++17 -g main.cpp -o main
```

To run the program for a given set of inputs, see below.
```bash
./prodcons <p> <c> <b>

# provided below are commands to run each of the possible combinations
# of producer, consumer, and buffer size
./prodcons 2 2 5
./prodcons 2 2 10
./prodcons 2 2 10
./prodcons 2 5 5
./prodcons 2 5 10
./prodcons 2 5 10
./prodcons 2 10 5
./prodcons 2 10 10
./prodcons 2 10 10
./prodcons 5 2 5
./prodcons 5 2 10
./prodcons 5 2 10
./prodcons 5 5 5
./prodcons 5 5 10
./prodcons 5 5 10
./prodcons 5 10 5
./prodcons 5 10 10
./prodcons 5 10 10
./prodcons 10 2 5
./prodcons 10 2 10
./prodcons 10 2 10
./prodcons 10 5 5
./prodcons 10 5 10
./prodcons 10 5 10
./prodcons 10 10 5
./prodcons 10 10 10
./prodcons 10 10 10
```