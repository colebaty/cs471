# PRODCONS

## Cole Baty and Amelia Ragsdale

### for CS471, Dr. Ravi Mukkamala, Fall 2022, Old Dominion University

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
instructions, placing the output of each run in a file named `sample-p-c-b`,
where `p` is the number of producers, `c` is the number of consumers, and `b` 
is the buffer size.  

It will compile the program if there is no executable named `prodcons` in the 
same directory.

```
# to run this script
./run.sh

# to set executable permissions (if needed)
chmod 755 *.sh
```

## `stats.sh`
This script compiles the data from the sample runs into a file named `stats.csv`
for import into spreadsheet applications for further analysis.  

```
# to run this script
./stats.sh

# to set executable permissions (if needed)
chmod 755 *.sh
```
## Compiling and Running

To compile manually from source code, run this command.
```bash
g++ -g -std=c++17 -o prodcons -fpermissive -fdiagnostics-color=always main.cpp -lpthread
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