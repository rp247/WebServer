cleanup(){
	rm -r test_dir
}

# color creds: https://stackoverflow.com/questions/5947742/how-to-change-the-output-color-of-echo-in-linux
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

QUEUE="queue.o queue.h"
TEST_FILES="test_files/correctness.c test_files/Makefile"

mkdir test_dir

cp $QUEUE test_dir
cp  $TEST_FILES test_dir

make -C test_dir -s

./test_dir/queue-test

if [ $? != 0 ]
then
	cleanup
    echo -e "Correctness Test: ${RED}Bad${NC}"
	exit 1
fi

echo -e "Correctness Test: ${GREEN}Good${NC}"
cleanup
