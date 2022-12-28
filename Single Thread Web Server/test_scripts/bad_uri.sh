#/bin/bash

# TESTS FOR Invalid URIs (valid from: a-zA-z0-9._)

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
bad=("ai$" "ok-bad" "validso_far!oops" "\r\n" "\0" "\\" "'*'")
outfile="outtemp.txt"

touch $outfile

for delim in ${bad[@]}; do
    expected=400
    
    actual=$(curl -s -w "%{http_code}" -o $outfile localhost:$port/$delim)
    
    if [[ $actual -ne $expected ]]; then
        echo -e "Bad URI Test: ${RED}Fail${NC}"
        kill -9 $pid
        wait $pid
        rm -f $outfile
        exit 1
    fi

done

echo -e "Bad URI Test: ${GREEN}Pass${NC}"
kill -9 $pid
wait $pid
rm -f $outfile
exit 0