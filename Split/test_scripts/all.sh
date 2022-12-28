#/bin/bash

# runs every test script in test-scripts

chmod +x ./*_test.sh

# run all scripts creds: https://stackoverflow.com/questions/41079143/run-all-shell-scripts-in-folder
for script in ./*_test.sh; do
    /bin/bash "$script" 
done