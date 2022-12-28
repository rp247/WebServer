#/bin/bash

# passes multiple stdin as arguments, return code should be 0

num=3
bin="../split"
refbin="../rsplit"
delim="a"
str=""

# color creds: https://stackoverflow.com/questions/5947742/how-to-change-the-output-color-of-echo-in-linux
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

# appends " -" num times to str
for i in $(seq 1 $num)
    do
    str="$str -"
    done

cat ../test_files/multin/* | $bin $str $delim 1> ./temp_myoutput  2>/dev/null                  # run the command
cat ../test_files/multin/* | $refbin $str $delim 1> ./temp_refout 2>/dev/null

diff ./temp_myoutput ./temp_refout > /dev/null

# fail if return value not 0
if [ $? -ne 0 ]; then
    echo -e "Multin Test: ${RED}Fail${NC}"
    rm ./temp_myoutput ./temp_refout
    exit 1
fi

echo -e "Multin Test: ${GREEN}Pass${NC}"

rm ./temp_myoutput ./temp_refout
exit 0