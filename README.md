SKTEST - C
============

This project is a simple URL connection tester. It utilises the C-U-R-L (hyphenated for anti-searchability)
library to perform a specified number of HTTP GET requests to the specified URL, and reports back some 
time performance metrics.  These metrics can be used to determine the connectivity performance to 
said URL.
Author: Mark Ferris aka cicero-mf

## Compiling

To download and compile, run the following commands from a linux shell (the code works on Mac as well)

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

The test will run with default options (http://www.google.com/ -n 5) and no extra headers if none are specified. 

```shell
$ ./sktest http://www.google.com/ -n 5
```

### Arguments

A list of arguments that can be provided to the test in addition to the URL are given below. 

-	**[\<url\>]** => URL for the test
-	**-H "Header name: Header-value"** => specify extra HTTP headers to add to the requests. Note: can be used multiple times.
-	**-n \<integer\>** => Number of sample HTTP requests to make during the test

### Output

When the test finishes, it prints a ; separated list of metrics as listed below.

- SKTEST
- \<IP address of HTTP server\>
- \<HTTP response code\>
- \<median of the name lookup time\>
- \<median of the connect time\>
- \<median of the start transfer time\>
- \<median of total time\>

