#/bin/bash

# split multiple binary files

bin="../split"
refbin="../rsplit"
delim="+"

# color creds: https://stackoverflow.com/questions/5947742/how-to-change-the-output-color-of-echo-in-linux
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

cat ../test_files/bin/* | $bin - $delim 1> ./temp_myoutput 2>/dev/null                # run the command
cat ../test_files/bin/* | $refbin - $delim 1> ./temp_refout 2>/dev/null  

diff ./temp_myoutput ./temp_refout > /dev/null

# fail if return value not 0
if [ $? -ne 0 ]; then
    echo -e "Multbin Test: ${RED}Fail${NC}"
    rm ./temp_myoutput ./temp_refout
    exit 1
fi

echo -e "Multbin Test: ${GREEN}Pass${NC}"

rm ./temp_myoutput ./temp_refout
exit 0