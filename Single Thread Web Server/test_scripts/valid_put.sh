#/bin/bash

# VALID PUT ONLY RETURN PARTIAL DATA

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
outfile="outtemp.txt"
infile="temp.txt"

touch $outfile
touch $infile

echo -n "abc" > $infile
printf "PUT /$outfile HTTP/1.1\r\nContent-Length: 3\r\n\r\nabcdefg" | nc localhost $port > /dev/null
diff $infile $outfile
if [[ $? -ne 0 ]]; then
    echo -e "Valid PUT Test: ${RED}Fail${NC}"
    kill -9 $pid
    wait $pid
    rm -f $infile $outfile
    exit 1
fi

echo -n "abc123" > $infile
printf "PUT /$outfile HTTP/1.1\r\nContent-Length: 6\r\n\r\nabc123g" | nc localhost $port > /dev/null
diff $infile $outfile
if [[ $? -ne 0 ]]; then
    echo -e "Valid PUT Test: ${RED}Fail${NC}"
    kill -9 $pid
    wait $pid
    rm -f $infile $outfile
    exit 1
fi

echo -e "Valid PUT Test: ${GREEN}Pass${NC}"
kill -9 $pid
wait $pid
rm -f $infile $outfile
exit 0