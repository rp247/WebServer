Ruchit Patel

rpatel18

Fall 2022

Assignment 0: Split

## Description
- In this program, a file is split at the given delimeter.
- Ex: Running "split file.txt a" would replace all the occurences of 'a' in file.txt with a newline
      and display the output to stdout.

## Usage
- split <file> <delimeter>
- Use '-' to indicate stdin as <file>.
- Only single character delimeters are allowed.
- Returns '0' on success and '1' on failure. All errors are sent to stderr.

## Design
- I chose the buffer size of 1024 bytes because it seemed to be the
  right amount of trade-off between efficiency and memory usage.

## Files
1. split.c
- This source file contains the main method and the implementation of split.

2. test_files/
- This directory contains the test files required to run the tests. Also, contains a README for usage.

3. test_scripts/
- This directory contains the test scripts. Also, contains a README for usage.

4. Makefile
- This is a Makefile that can be used with the make utility to build the executable.

5. README.md 

