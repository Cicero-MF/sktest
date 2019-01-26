/*
 *
 *      Year   : 2018
 *      Author : Mark Ferris
 *
 *      urlconntest - url connection tester - interface to libcurl
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#include <curl/curl.h>
#include "util.h"
#include "urlconntest.h"

int printVerbose = 0;

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

struct {
  t_sMetrics metricStructArr [nTIME_TESTS];
  char ip[ULRCONNTEST_IP_STR_LEN];
  long respCode;
} storeValsStruct = {0};

struct t_sMemoryStruct {
  char *memory;
  size_t size;
};

struct curl_slist *headerList = NULL;


/* ============================ Function prototypes ================================================================ */
static size_t write_memory_callback(void *contents, size_t size, size_t nmemb, void *userp);
static CURLcode get_time_metrics (CURL *handle, int *rett);
static void process_metric (int *arrToSort, int timeTest, int reqn);


/* ============================ Public functions =================================================================== */
void urlconntest_init (void) {
  memset(&storeValsStruct, 0, sizeof(storeValsStruct));
  curl_slist_free_all(headerList);
}

/* Connection test handler and interface to libcurl */
int urlconntest_gethttp (char *url, unsigned int reqn) {
  CURL *curl_handle;
  CURLcode res;
  int i;
  
  struct t_sMemoryStruct chunk;
  
  /* 2D array storing all raw sample times for nTIME_TESTS*reqn metrics */
  int (*timeVals)[nTIME_TESTS] = malloc(sizeof(int[reqn][nTIME_TESTS]));
  
  if (NULL == url) {
    fprintf(stderr, "invalid url specified: NULL pointer\n");
    return 1;
  }
  
  if(NULL == timeVals) {
    /* out of memory! */ 
    if (printVerbose) printf("not enough memory (malloc returned NULL)\n");
    return 1;
  }
 
  chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */ 
  chunk.size = 0;    /* no data at this point */ 
 
  curl_global_init(CURL_GLOBAL_ALL);
  
  for (i = 0; i < reqn; i++) {    
    /* init new curl session for consistent measurements */ 
    curl_handle = curl_easy_init();

    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    
    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headerList);

    /* Register writedata callback to suppress stdio output */ 
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_memory_callback);
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
      /* Get non-variable info (URL, rep code) on first run */
      if (i == 1) {
        char *ip;
        res = curl_easy_getinfo(curl_handle, CURLINFO_PRIMARY_IP, &ip);
        strcpy (storeValsStruct.ip, ip);
        if (printVerbose) printf ("IP = %s\n", ip);
        res = curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &storeValsStruct.respCode);
      }
      
      res = get_time_metrics(curl_handle, &timeVals[i][0]);
      if (CURLE_OK == res) {
        if (printVerbose) printf("Request %i, of %i completed\n", i + 1, reqn);
#ifdef URLCONNTEST_STORE_RESP_DATA
        if (printVerbose) printf("%lu bytes retrieved\n", (unsigned long)chunk.size);
#endif
      }
    }
    
    /* cleanup curl stuff - next request will be brand new */ 
    curl_easy_cleanup(curl_handle);
  } 
  
  curl_slist_free_all(headerList);
 
  free(chunk.memory);
  
  /* Process metrics if all completed */
  if (i == reqn) {
    int j;
    int *arrToSort;
    
    arrToSort = malloc(reqn * sizeof(int));
    if (NULL == arrToSort) {
      fprintf(stderr, "not enough memory to sort: NULL pointer\n");
      return 1;
    }   
  
    for (i = 0; i < nTIME_TESTS; i++) {      
      for (j=0; j < reqn; j++) {
        arrToSort[j] = timeVals[j][i];        
      }
     
      process_metric (arrToSort, i, reqn);
    }
    
    free(arrToSort);
  } 
  
  free(timeVals);
 
  /* we're done with libcurl, so clean it up */ 
  curl_global_cleanup();
  
  if(CURLE_OK != res) {
    return 1;
  }
  return 0;
}

char *urlconntest_getip (void) {
  /* Force NULL termination */
  storeValsStruct.ip[ULRCONNTEST_IP_STR_LEN-1] = 0;
  return storeValsStruct.ip;
}

long urlconntest_getrespcode (void) {
  return storeValsStruct.respCode;
}

double urlconntest_getmedianMetric (t_urlconntest_metrics metric) {  
  if (metric >= nTIME_TESTS) return 0;
  
  return storeValsStruct.metricStructArr[metric].median;
}

int urlconntest_addheader (char *headerStr) {
  struct curl_slist *temp=NULL;
  
  temp = curl_slist_append(headerList, headerStr);
  if (NULL == temp) {
    return 1;
  }
  
  headerList = temp;
  return 0;
}

void urlconntest_setverbose (int verbose) {
  printVerbose = verbose; 
}


/* ============================ Private functions ================================================================== */
static void process_metric (int *arrToSort, int timeTest, int reqn) {
  if (timeTest >= nTIME_TESTS) return;
  
  storeValsStruct.metricStructArr[timeTest].median = util_median(arrToSort, reqn);
  storeValsStruct.metricStructArr[timeTest].min = arrToSort[0];
  storeValsStruct.metricStructArr[timeTest].max = arrToSort[reqn-1];
  storeValsStruct.metricStructArr[timeTest].jitter = 
    storeValsStruct.metricStructArr[timeTest].max - storeValsStruct.metricStructArr[timeTest].min;

  if (printVerbose) printf ("\n%s:\n\tMedian = %.0fus\n", curlTimeTests[timeTest].optionName, 
                                        storeValsStruct.metricStructArr[timeTest].median);
  if (printVerbose) printf ("\tMin = %ius\n", storeValsStruct.metricStructArr[timeTest].min);
  if (printVerbose) printf ("\tMax = %ius\n", storeValsStruct.metricStructArr[timeTest].max);
  if (printVerbose) printf ("\tJitter = %ius\n", storeValsStruct.metricStructArr[timeTest].jitter);
}

static size_t
write_memory_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
#ifdef URLCONNTEST_STORE_RESP_DATA
  struct t_sMemoryStruct *mem = (struct t_sMemoryStruct *)userp;

  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if(NULL == ptr) {
    /* out of memory! */ 
    if (printVerbose) printf("not enough memory (realloc returned NULL)\n");
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
   Assumptions: rett is a pointer to an array of at least size nTIME_TESTS
*/
static CURLcode get_time_metrics (CURL *handle, int *rett) {
  CURLcode res;
  double time;
  int i;
  
  for (i = 0; i < nTIME_TESTS; i++) {
    res = curl_easy_getinfo(handle, curlTimeTests[i].option, &time);
    if (printVerbose) printf ("\tGot %.6fs for %s\n", time, curlTimeTests[i].optionName);
    if(CURLE_OK == res) {
      time *= 1000000;
      rett[i] = (int)time;
    }
    if (printVerbose) printf ("\tConverted -> %ius\n", rett[i]);
  }
  
  return res;
}
