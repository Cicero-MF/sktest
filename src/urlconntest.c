/*
 *
 *      Year   : 2018
 *      Author : Mark Ferris
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#include <curl/curl.h>
#include "urlconntest.h"

typedef struct {
  CURLoption option;
  char *optionName;  
} t_sCURLoptMetrics;

const t_sCURLoptMetrics curlTimeTests[nTIME_TESTS] = {
  {CURLINFO_NAMELOOKUP_TIME,    "Name Lookup Time"},
  {CURLINFO_CONNECT_TIME,       "Connect Time"},
  {CURLINFO_STARTTRANSFER_TIME, "Start Transfer Time"},
  {CURLINFO_TOTAL_TIME,         "Total Time"}
};

t_sMetrics metricStructArr [nTIME_TESTS] = {0};

struct memoryStruct {
  char *memory;
  size_t size;
};


/* Function prototypes */
static size_t writeMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
static CURLcode get_time_metrics (CURL *handle, int *rett);


/* Connection test handler and interface to libcurl */
int urlconntest_gethttp (char *url, unsigned int reqn) {
  CURL *curl_handle;
  CURLcode res;
  int i;
  
  struct memoryStruct chunk;
  
  /* 2D array storing all raw sample times for nTIME_TESTS*reqn metrics */
  int (*timeVals)[nTIME_TESTS] = malloc(sizeof(int[reqn][nTIME_TESTS]));
  
  if (NULL == url) {
    fprintf(stderr, "invalid url specified: NULL pointer\n");
    return 1;
  }
  
  if(NULL == timeVals) {
    /* out of memory! */ 
    printf("not enough memory (malloc returned NULL)\n");
    return 0;
  }
 
  chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */ 
  chunk.size = 0;    /* no data at this point */ 
 
  curl_global_init(CURL_GLOBAL_ALL);
  
  for (i = 0; i < reqn; i++) {    
    /* init new curl session for consistent measurements */ 
    curl_handle = curl_easy_init();

    curl_easy_setopt(curl_handle, CURLOPT_URL, url);

    /* Register writedata callback to suppress stdio output */ 
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, writeMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

    /* some servers don't like requests that are made without a user-agent
       field, so we provide one */ 
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "urlconnest-agent/1.0");
 
    res = curl_easy_perform(curl_handle);

    /* check for errors */ 
    if(CURLE_OK != res) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
      break;
    } else {
      /*
       * Now, our chunk.memory points to a memory block that is chunk.size
       * bytes big and contains the remote file.
       *
       * Do something nice with it!
       */ 
      res = get_time_metrics(curl_handle, &timeVals[i][0]);
      if (CURLE_OK == res) {
        printf("Request %i, of %i completed\n", i + 1, reqn);
#ifdef URLCONNTEST_STORE_RESP_DATA
        printf("%lu bytes retrieved\n", (unsigned long)chunk.size);
#endif
      }
    }
    
    /* cleanup curl stuff - next request will be brand new */ 
    curl_easy_cleanup(curl_handle);
  } 
 
  free(chunk.memory);
  
  free(timeVals);
 
  /* we're done with libcurl, so clean it up */ 
  curl_global_cleanup();
 
  return 0;
}


/* Private functions */

static size_t
writeMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct memoryStruct *mem = (struct memoryStruct *)userp;
#ifdef URLCONNTEST_STORE_RESP_DATA
  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if(NULL == ptr) {
    /* out of memory! */ 
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }
 
  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
#endif 
  return realsize;
}

/* Get nTIME_TESTS time values from curl request
   Convert from double (seconds) to int (microsecs)
   Assumptions: rett is a pointer to an array of size nTIME_TESTS
*/
static CURLcode get_time_metrics (CURL *handle, int *rett) {
  CURLcode res;
  double time;
  int i;
  
  for (i = 0; i < nTIME_TESTS; i++) {
    res = curl_easy_getinfo(handle, curlTimeTests[i].option, &time);
    printf ("\tGot %.6fs for %s\n", time, curlTimeTests[i].optionName);
    if(CURLE_OK == res) {
      time *= 1000000;
      rett[i] = (int)time;
    }
    printf ("\tConverted -> %ius\n", rett[i]);
  }
  
  return res;
}
