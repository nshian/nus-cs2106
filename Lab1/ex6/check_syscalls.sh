#!/bin/bash

####################
# Lab 1 Exercise 6
# Name: Ng Sihan, Ian 
# Student No: A0231053X
# Lab Group: B09
####################

echo "Printing system call report"

# Compile file
gcc -std=c99 pid_checker.c -o ex6

# Use strace to get report
strace -c ./ex6
