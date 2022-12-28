#/bin/bash

# TESTS FOR NOT IMPLEMENTED METHODS

# Creds for port: Resources
# Get available port.
port=$(bash ./get_port.sh)

# Start up server.
./httpserver $port > /dev/null &
pid=$!

# Wait until we can connect.
while ! nc -zv localhost $port; do
    sleep 0.01
done

# color creds: https://stackoverflow.com/questions/5947742/how-to-change-the-output-color-of-echo-in-linux
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

# array learn creds: https://opensource.com/article/18/5/you-dont-know-bash-intro-bash-arrays
bad=("ai" "GETT" "\0" "\r\n" "head" "pUt" "\\")
outfile="outtemp.txt"
infile="temp.txt"

touch $infile
touch $outfile

for delim in ${bad[@]}; do
    # Expected status code.
    expected=501

    actual=$(curl -X $delim -s -w "%{http_code}" -o $outfile localhost:$port/$infile)

    if [[ $actual -ne $expected ]]; then
        echo -e "Not Implemented Test: ${RED}Fail${NC}"
        kill -9 $pid
        wait $pid
        rm -f $infile $outfile
        exit 1
    fi

done

echo -e "Not Implemented Test: ${GREEN}Pass${NC}"
kill -9 $pid
wait $pid
rm -f $infile $outfile
exit 0