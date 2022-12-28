#/bin/bash

# split nonexisent files and permission denied files

# check incorrect delimeters

bin="../split"
file="../test_files/perm_denied.txt"
delim="a"

# color creds: https://stackoverflow.com/questions/5947742/how-to-change-the-output-color-of-echo-in-linux
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

# array learn creds: https://opensource.com/article/18/5/you-dont-know-bash-intro-bash-arrays
dne=("ai.txt" "poqwep.out" "--" "[][sa]" "-rasdf" "12.com" ";;")

# perm denied
chmod -r ../test_files/perm_denied.txt
$bin ../test_files/perm_denied.txt $delim 2> /dev/null

# dne
for f in ${dne[@]}; do

    if [ $? -ne 1 ]; then
        echo -e "DNE Test: ${RED}Fail${NC}"
        exit 1
    fi

    $bin $dne $delim 2> /dev/null 

done

echo -e "DNE Test: ${GREEN}Pass${NC}"
exit 0