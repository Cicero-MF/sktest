SKTEST - C
============

This project is a simple URL connection tester. It utilises the C-U-R-L (hyphenated for anti-searchability)
library to perform a specified number of HTTP GET requests to the specified URL, and reports back some 
time performance metrics.  These metrics can be used to determine the connectivity performance to 
said URL.
Author: Mark Ferris aka cicero-mf

## Compiling

If you're on a completely clean system you will need the following installed:

```shell
$ sudo apt install git
$ sudo apt install make
$ sudo apt install curl
$ sudo apt install libssl-dev
$ sudo apt install zlib1g-dev
```

To download and compile and run, run the following commands from a linux shell (tested in ubuntu)

```shell
$ git clone https://github.com/cicero-mf/sktest.git
$ cd sktest
$ make
```

The Makefile should download and compile the libc-u-r-l library, build and run the sktest
program all automatically. 
If you do not wish to run the program immediately after a build please comment the last 
line of the MAKEFILE.

## Running the Test 

The test will run with default options (-url http://www.google.com/ -n 5) and no extra headers if none are specified. 

```shell
$ ./sktest -url http://www.google.com/ -n 5 -verboseOn
```

### Arguments

A list of arguments that can be provided to the test in addition to the URL are given below. 

-	**-url \<url\>** => URL for the test
-	**-H "Header name: Header-value"** => specify extra HTTP headers to add to the requests. Note: can be used multiple times.
-	**-n \<integer\>** => Number of sample HTTP requests to make during the test
-	**-verbose\<On/Off\>** => verboseOn prints progress and raw metrics from each test, verboseOff or not adding the verbose argument will suppress any extra run-time prints

### Output

When the test finishes, it prints a ; separated list of metrics as listed below - timings in microseconds.

- SKTEST
- \<IP address of HTTP server\>
- \<HTTP response code\>
- \<median of the name lookup time\>
- \<median of the connect time\>
- \<median of the start transfer time\>
- \<median of total time\>

### TODO

Given time, I'd have like to have done the following:

- Probably should've used mutlithreading capabilities. 
- Would've have liked to test a lot more, written tests for the median calc functions etc. 
- Error checking and catching could be better in places.  
- Would've liked to further investigate whether tests are more consistent and reliable without forcing a new connection every time - this would speed up the testing considerably. Only forced a new connection to try to get a consistent name lookup time.
- Would like to investigate SSL connections, and compare
- Test larger sample sets.  
- Periodic testing to get a better picture at different times of the day.
- Static analysis using splint was attempted but would've taken too long to remove the curl rabbithole it wanted to go down
