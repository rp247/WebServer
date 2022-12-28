#/bin/bash

# Permission denied head and get

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
chmod -r $infile


expected=403

actual=$(curl -I -s -w "%{http_code}" -o $outfile localhost:$port/$infile)

if [[ $actual -ne $expected ]]; then
    echo -e "Permission Denied Test: ${RED}Fail${NC}"
    kill -9 $pid
    wait $pid
    rm -f $infile $outfile
    exit 1
fi

actual=$(curl -s -w "%{http_code}" -o $outfile localhost:$port/$infile)

if [[ $actual -ne $expected ]]; then
    echo -e "Permission Denied Test: ${RED}Fail${NC}"
    kill -9 $pid
    wait $pid
    rm -f $infile $outfile
    exit 1
fi

echo -e "Permission Denied Test: ${GREEN}Pass${NC}"
kill -9 $pid
wait $pid
rm -f $infile $outfile
exit 0