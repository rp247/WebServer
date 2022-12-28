#/bin/bash

# check incorrect delimeters

bin="../split"
file="../test_files/empty/1"

# color creds: https://stackoverflow.com/questions/5947742/how-to-change-the-output-color-of-echo-in-linux
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

# array learn creds: https://opensource.com/article/18/5/you-dont-know-bash-intro-bash-arrays
bad=("ai" "poqwep" "--" "[][]" "-r" "12" ";;")

for delim in ${bad[@]}; do
    $bin $file $delim 2> /dev/null 

    if [ $? -ne 1 ]; then
        echo -e "Baddelim Test: ${RED}Fail${NC}"
        exit 1
    fi

done

echo -e "Baddelim Test: ${GREEN}Pass${NC}"
exit 0