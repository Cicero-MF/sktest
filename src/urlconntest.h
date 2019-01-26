#ifndef __URLCONNTEST_H__
#define __URLCONNTEST_H__

#define ULRCONNTEST_IP_STR_LEN (20)

typedef enum {
  URLCONNTEST_METRIC_NAME_LOOKUP_TIME,
  URLCONNTEST_METRIC_CONNECT_TIME,
  URLCONNTEST_METRIC_START_TRANSFER_TIME,
  URLCONNTEST_METRIC_TOTAL_TIME,
  nTIME_TESTS
} t_urlconntest_metrics;

typedef struct {
  int min;
  int max;
  int jitter;
  double median;  
} t_sMetrics;

void urlconntest_init (void);
int urlconntest_gethttp(char *url, unsigned int reqn);
char *urlconntest_getip (void) ;
long urlconntest_getrespcode (void);
double urlconntest_getmedianMetric (t_urlconntest_metrics metric);
int urlconntest_addheader (char *headerStr);
void urlconntest_setverbose (int verbose);

#endif /* __URLCONNTEST_H__ */
