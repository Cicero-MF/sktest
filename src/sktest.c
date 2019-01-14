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

int main (void) {
  
  if ( 0 == urlconntest_gethttp ("http://www.google.com/", 3) ) {
  
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