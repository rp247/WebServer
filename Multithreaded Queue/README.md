Ruchit Patel

rpatel18

Fall 2022

Assignment 2: Bounded Buffer

## Description
- In this program, a multithreaded queue is implemented: multiple threads can pop and push to a queue.
- The queue is thread safe.

## Usage
- Bind with other files and look at the interface in queue.h to use the queue.

## Design
- Semaphores: I used semaphores because they were more intuitive and simpler then 
  mutexes. Mutex signals were less intuitive with multiple producers. On the other
  hand semaphores are quite easy to understand: Push while space, otherwise block 
  and Pop while not empty, block otherwise.

## Files
1. queue.c
- This source file contains the implementations of the queue API.

2. queue.h
- This header file contains the interface to the queue functions.

3. test_files/
- This directory contains the test 'C' files required to run the test scrits.

4. test_scripts/
- This directory contains the test scripts.

5. Makefile
- This is a Makefile that can be used with the make utility to build the object file.

6. README.md