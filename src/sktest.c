/*
 *
 *      Year   : 2018
 *      Author : Mark Ferris
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#include "urlconntest.h"

int main (void) {
  printf ("Simple get to google returned %i\n", urlconntest_gethttp ("http://www.google.com/", 3));
  return 0;
}