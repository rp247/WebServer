#/bin/bash

# splits the binary itself

bin="../split"
delim="a"

# color creds: https://stackoverflow.com/questions/5947742/how-to-change-the-output-color-of-echo-in-linux
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

$bin $bin $delim > /dev/null 2> /dev/null

if [ $? -ne 0 ]; then
    echo -e "Splitself Test: ${RED}Fail${NC}"
    exit 1
    fi

echo -e "Splitself Test: ${GREEN}Pass${NC}"

exit 0