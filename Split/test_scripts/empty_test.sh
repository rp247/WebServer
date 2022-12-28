#/bin/bash

# passes empty files to split: there should be no output and the return code should be 0

file="../test_files/empty/"
bin="../split"
refbin="../rsplit"
delim="a"

# color creds: https://stackoverflow.com/questions/5947742/how-to-change-the-output-color-of-echo-in-linux
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

for i in {1..5}
    do
    str=" "
    for j in $(seq 1 $i)
        do
            str+="$file$j "
        done
    
    $bin $str $delim 1> ./temp_myoutput 2>/dev/null                    # run the command
    $refbin $str $delim 1> ./temp_refout 2>/dev/null

    diff ./temp_myoutput ./temp_refout > /dev/null

    # fail if return value not 0
    if [ $? -ne 0 ]; then
        echo -e "Empty Test: ${RED}Fail${NC}"
        rm ./temp_myoutput ./temp_refout
        exit 1
    fi
    done

echo -e "Empty Test: ${GREEN}Pass${NC}"

rm ./temp_myoutput ./temp_refout
exit 0