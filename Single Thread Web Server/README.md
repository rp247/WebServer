Ruchit Patel

rpatel18

Fall 2022

Assignment 1: HTTP Server

## Description
- In this program, an HTTP server (on localhost) is made that supports three operations: GET, PUT, and HEAD.
- GET retrieves data of a file.
- HEAD retrieves header of a file.
- PUT puts content of a file to a file.
- Ex: "./httpserver 1024" would start a server listening to localhost port 1024. User could send HTTP requests on port 1024.
- Only HTTP/1.1 supported. Files must not be a directory and be valid.

## Usage
- httpserver <port>
- Returns standard HTTP error codes for responses.

## Design
- Buffer size: I chose the buffer size of 2048 bytes because it is easier to parse
  the inital request (as both the request line and header fields are guaranteed to 
  be < 2048 bytes).
- Structure of code: The structure of the code is hierarchical meaning that each function 
  parses one thing and moves "up" the hierarchy for further processing. Thus, whole
  processing and parsing is done in one hierarchy climbing per client.
- Send (msg) buffer size 80. The highest value of a file size cannot go over 
  this value (checked mathematically) and thus I chose it to be of size 80.
- There are no other out-of-ordinary design discussions.

## Files
1. httpserver.c
- This source file contains the main method and the implementation of the httpserver.

2. bind.h
- This header file contains the interface to the bind functions.

3. bind.c
- This source file contains the implementations of methods to bind to a socket (on localhost).

4. test_files/
- This directory contains the test files required to run the tests.

5. test_scripts/
- This directory contains the test scripts.

6. Makefile
- This is a Makefile that can be used with the make utility to build the executable.

7. README.md 