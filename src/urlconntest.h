#ifndef __URLCONNTEST_H__
#define __URLCONNTEST_H__

#define nTIME_TESTS (4)

typedef struct {
  int min;
  int max;
  int jitter;
  int median;  
} t_sMetrics;

int urlconntest_gethttp(char *url, unsigned int reqn);

#endif /* __URLCONNTEST_H__ */