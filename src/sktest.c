/*
 *
 *      Year   : 2018
 *      Author : Mark Ferris
 *      
 *      Program to illustrate interface to urlconntest - SKTEST
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h" 
#include "urlconntest.h"

#define MAX_URL_LEN     (50)
#define MAX_HEADER_LEN  (100)
int reqn = 0;
char urlToTest[MAX_URL_LEN+1];
char header[MAX_HEADER_LEN+1];

static int process_args(int argc, char* argv[]);


int main (int argc, char* argv[]) {
  int err;
  
  err = 0;
  
  urlconntest_init();
  
  err |= process_args(argc, argv);   
  
  if ((0 == err) && (0 == urlconntest_gethttp (urlToTest, reqn))) {
  
    printf("\nSKTEST;%s;%ld;%.0f;%.0f;%.0f;%.0f\n",
       urlconntest_getip(),
       urlconntest_getrespcode(),
       urlconntest_getmedianMetric(URLCONNTEST_METRIC_NAME_LOOKUP_TIME),
       urlconntest_getmedianMetric(URLCONNTEST_METRIC_CONNECT_TIME),
       urlconntest_getmedianMetric(URLCONNTEST_METRIC_START_TRANSFER_TIME),
       urlconntest_getmedianMetric(URLCONNTEST_METRIC_TOTAL_TIME));
  }
  
  return 0;
}


static int process_args(int argc, char* argv[]) {
	for(int i=1; i<argc; i++)
	{
		if(0 == strcmp(argv[i], "-n")) {
			reqn = atoi(argv[++i]);
#ifdef PRINT_DEBUG
      printf("Reqn = %i\n", reqn);
#endif
    } else if (0 == strcmp(argv[i], "-url")) {
      if(strlen(argv[++i]) > MAX_URL_LEN) {
				printf("URL provided as argument is too long, max length is %i\n", MAX_URL_LEN);
				return 1;
			}	else {
				strcpy(urlToTest, argv[i]);
#ifdef PRINT_DEBUG
        printf("URL is %s\n", urlToTest);
#endif
      }
    } else if(0 == strcmp(argv[i], "-verboseOn")) {
			urlconntest_setverbose(1);
#ifdef PRINT_DEBUG
      printf("Verbose On\n");
#endif
    } else if(0 == strcmp(argv[i], "-verboseOff")) {
			urlconntest_setverbose(0);
#ifdef PRINT_DEBUG
      printf("Verbose Off\n");
#endif
    } else if(0 == strcmp(argv[i], "-H"))	{
      if(strlen(argv[++i]) > MAX_HEADER_LEN) {
				printf("Header provided as argument is too long, max length is %i\n", MAX_HEADER_LEN);
				return 1;
			}	else {
				strcpy(header, argv[i]);
#ifdef PRINT_DEBUG
        printf("Header found %s\n", header);
#endif
        if (1 == urlconntest_addheader(header)) {
          printf("Header provided invalid, will be ignored\n");
        }
      }
		}
	}
	if(strlen(urlToTest) == 0) {
		printf("URL not specified, using http://www.google.com/ as default\n");
    strcpy(urlToTest, "http://www.google.com/");
	}
  
  if ((0 == reqn) || (reqn > 100)) {
    printf("reqn not specified or too large - using default of 5\n");
    reqn = 5;
  }

	return 0;
}
