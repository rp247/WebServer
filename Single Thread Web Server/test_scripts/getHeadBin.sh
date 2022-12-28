#/bin/bash

# GET and HEAD on binary

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
file="../test_files/unt.pdf"

touch $infile
touch $outfile

cat $file > $infile

expected=200

actual=$(curl -I -s -w "%{http_code}" -o $outfile localhost:$port/$infile)
if [[ $actual -ne $expected ]]; then
    echo -e "HEADGET Bin Test: ${RED}Fail${NC}"
    kill -9 $pid
    wait $pid
    rm -f $infile $outfile
    exit 1
fi

actual=$(curl -s -w "%{http_code}" -o $outfile localhost:$port/$infile)
if [[ $actual -ne $expected ]]; then
    echo -e "HEADGET Bin Test: ${RED}Fail${NC}"
    kill -9 $pid
    wait $pid
    rm -f $infile $outfile
    exit 1
fi

diff $file $infile
if [[ $? -ne 0 ]]; then
    echo -e "HEADGET Bin Test: ${RED}Fail${NC}"
    kill -9 $pid
    wait $pid
    rm -f $infile $outfile
    exit 1
fi

echo -e "HEADGET Bin Test: ${GREEN}Pass${NC}"
kill -9 $pid
wait $pid
rm -f $infile $outfile
exit 0