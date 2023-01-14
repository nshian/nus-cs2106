#!/bin/bash

####################
# Lab 1 Exercise 5
# Name: Ng Sihan, Ian 
# Student No: A0231053X
# Lab Group: B09
####################

# Fill the below up
hostname=$(uname -n)
machine_hardware=$(uname -s -m)
max_user_process_count=$(cat /proc/sys/kernel/pid_max)
user_process_count=$(ps x | wc -l)
user_with_most_processes=$(ps -eo user | sort | uniq -c | sort -n | tail -1 | awk '{print $2}')
mem_free_percentage=$(free | grep Mem | awk '{print $3/$2 * 100.0}')

echo "Hostname: $hostname"
echo "Machine Hardware: $machine_hardware"
echo "Max User Processes: $max_user_process_count"
echo "User Processes: $user_process_count"
echo "User With Most Processes: $user_with_most_processes"
echo "Memory Free (%): $mem_free_percentage"
