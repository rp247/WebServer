#/bin/bash

# No Header Request (HEAD, GET OK, PUT BAD)

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

touch $infile
touch $outfile

echo "Testtest" > $infile

expected=200

actual=$(printf "GET /$infile HTTP/1.1\r\n\r\nShouldBeFine" | nc localhost $port | awk '{print $2}' |grep $expected)
if [[ $actual -ne $expected ]]; then
    echo -e "No Header Test: ${RED}Fail${NC}"
    kill -9 $pid
    wait $pid
    rm -f $infile $outfile
    exit 1
fi

actual=$(printf "HEAD /$infile HTTP/1.1\r\n\r\nNoErrOnThisOne " | nc localhost $port | awk '{print $2}' |grep $expected)
if [[ $actual -ne $expected ]]; then
    echo -e "No Header Test: ${RED}Fail${NC}"
    kill -9 $pid
    wait $pid
    rm -f $infile $outfile
    exit 1
fi

expected=400
actual=$(printf "PUT /$infile HTTP/1.1\r\n\r\nINVALID" | nc localhost $port | awk '{print $2}' |grep $expected)
if [[ $actual -ne $expected ]]; then
    echo -e "No Header Test: ${RED}Fail${NC}"
    kill -9 $pid
    wait $pid
    rm -f $infile $outfile
    exit 1
fi

echo -e "No Header Test: ${GREEN}Pass${NC}"
kill -9 $pid
wait $pid
rm -f $infile $outfile
exit 0