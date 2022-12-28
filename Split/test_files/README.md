# Test File directory
This directory should contain all files your tests want.
The directory itself will be in the same location as the executable being tested
Tests themselves should be put into the `test_scripts` directory and should return 0 on success and nonzero on failure.

# Usage
Files: 
- Keep the test_files and test_scripts directories under the same parent directory.
- Also, keep split (my executable) and rsplit (provided by prof) in the same parent directory as the test_files and test_script folders.
- Run, test_scripts/all.sh to run all test scripts.