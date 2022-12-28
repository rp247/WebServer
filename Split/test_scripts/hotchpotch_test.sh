#/bin/bash

# split a mix of files (binary, stdin, text, etc.)

bin="../split"
refbin="../rsplit"
delim="a"

# color creds: https://stackoverflow.com/questions/5947742/how-to-change-the-output-color-of-echo-in-linux
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

# array learn creds: https://opensource.com/article/18/5/you-dont-know-bash-intro-bash-arrays
files=("../test_files/bin/1.jpeg" "-" "../test_files/empty/1" "../test_files/filefile/1")
str=" "

# dne
for f in ${files[@]}; do
    str+="$f "
done

cat ../test_files/multin/1 | $bin $str $delim > ./temp_myoutput 2> /dev/null
cat ../test_files/multin/1 | $refbin $str $delim > ./temp_refout 2> /dev/null

diff ./temp_myoutput ./temp_refout > /dev/null

# fail if return value not 0
if [ $? -ne 0 ]; then
    echo -e "Hotchpotch Test: ${RED}Fail${NC}"
    rm ./temp_myoutput ./temp_refout
    exit 1
fi

echo -e "Hotchpotch Test: ${GREEN}Pass${NC}"

rm ./temp_myoutput ./temp_refout
exit 0