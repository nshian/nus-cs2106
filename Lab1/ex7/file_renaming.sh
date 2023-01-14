#!/bin/bash

####################
# Lab 1 Exercise 7
# Name: Ng Sihan, Ian
# Student No: A0231053X
# Lab Group: B09
####################

####################
# Strings that you may need for prompt (not in order)
####################
input_numbers="Enter $N numbers:"
input_new_prefix="Enter NEW prefix (only alphabets):"
input_original_prefix="Enter prefix (only alphabets):"
ERROR_INVALID="INVALID"
input_num_files="Number of files to create:"
input_valid_number="Please enter a valid number [0-9]:"
input_valid_prefix="Please enter a valid prefix [a-z A-Z]:"

####################
# Strings that you may need for prompt (not in order)
####################

#################### Steps #################### 

# Fill in the code to request user for the prefix
echo "$input_original_prefix"
read -a prefix
# Check the validity of the prefix #
while [[ "$prefix" =~ [^a-zA-Z] ]]
do
  echo "$ERROR_INVALID"
  echo "$input_valid_prefix"
  read -a prefix
done

# Enter numbers an create files #
echo "$input_num_files"
read -a N
#echo "$input_numbers"
echo "Enter $N numbers: "
declare -a numbers
for ((i=0; i<N; i++))
do
  read -a num
  while [[ "$num" =~ [^0-9] ]]
  do
    echo "$ERROR_INVALID"
    echo "$input_valid_number"
    read -a num
  done
  numbers+=($num)
done
for number in "${numbers[@]}"
do
  filename=${prefix}_${number}
  touch ${filename}.txt
done

echo ""
echo "Files Created"
ls *.txt
echo ""

# Fill in the code to request user for the new prefix
echo "$input_new_prefix"
read -a new_prefix
while [[ "$new_prefix" =~ [^a-zA-Z] ]]
do
  echo "$ERROR_INVALID"
  echo "$input_valid_prefix"
  read -a new_prefix
done
# Renaming to new prefix #
for number in "${numbers[@]}"
do
  original=${prefix}_${number}
  new=${new_prefix}_${number}
  mv ${original}.txt ${new}.txt
done

echo ""
echo "Files Renamed"
ls *.txt
echo ""
