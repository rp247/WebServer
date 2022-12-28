#/bin/bash

# Invalid HTTP version

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

expected=400

actual=$(curl -I -s -w "%{http_code}" -o $outfile localhost:$port/$infile --http1.0)

if [[ $actual -ne $expected ]]; then
    echo -e "Bad HTTP Test: ${RED}Fail${NC}"
    kill -9 $pid
    wait $pid
    rm -f $infile $outfile
    exit 1
fi

actual=$(curl -s -w "%{http_code}" -o $outfile localhost:$port/$infile --http1.0)
if [[ $actual -ne $expected ]]; then
    echo -e "Bad HTTP Test: ${RED}Fail${NC}"
    kill -9 $pid
    wait $pid
    rm -f $infile $outfile
    exit 1
fi

actual=$(curl -s -w "%{http_code}" -o $outfile localhost:$port/$infile --http1.0)
if [[ $actual -ne $expected ]]; then
    echo -e "Bad HTTP Test: ${RED}Fail${NC}"
    kill -9 $pid
    wait $pid
    rm -f $infile $outfile
    exit 1
fi

actual=$(printf "HEAD /$infile \r\n\r\n\r\n" | nc localhost $port | awk '{print $2}' |grep $expected)
if [[ $actual -ne $expected ]]; then
    echo -e "Bad HTTP Test: ${RED}Fail${NC}"
    kill -9 $pid
    wait $pid
    rm -f $infile $outfile
    exit 1
fi

actual=$(printf "PUT /$infile BAD/1.1 \r\nContent-Length: 10\r\n\r\n" | nc localhost $port | awk '{print $2}' |grep $expected)
if [[ $actual -ne $expected ]]; then
    echo -e "Bad HTTP Test: ${RED}Fail${NC}"
    kill -9 $pid
    wait $pid
    rm -f $infile $outfile
    exit 1
fi

echo -e "Bad HTTP Test: ${GREEN}Pass${NC}"
kill -9 $pid
wait $pid
rm -f $infile $outfile
exit 0