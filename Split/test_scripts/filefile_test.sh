#/bin/bash

# passes same file $num times

num=3
bin="../split"
refbin="../rsplit"
delim="2"
str="../test_files/filefile/1"
tempstr=""

# color creds: https://stackoverflow.com/questions/5947742/how-to-change-the-output-color-of-echo-in-linux
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

# appends " $str" num times to str
for i in $(seq 1 $num)
    do
    tempstr+="$str "
    done

$bin $tempstr $delim 1> ./temp_myoutput 2>/dev/null                # run the command
$refbin $tempstr $delim 1> ./temp_refout 2>/dev/null

diff ./temp_myoutput ./temp_refout > /dev/null

# fail if return value not 0
if [ $? -ne 0 ]; then
    echo -e "Filefile Test: ${RED}Fail${NC}"
    rm ./temp_myoutput ./temp_refout
    exit 1
fi

echo -e "Filefile Test: ${GREEN}Pass${NC}"

rm ./temp_myoutput ./temp_refout
exit 0