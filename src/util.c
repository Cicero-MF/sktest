/*
 *
 *      Year   : 2018
 *      Author : Mark Ferris
 *
 */



void util_bubble_sort (int *arr, int n) {
  int temp;
  int i, j;
  
  /* the following two loops sort the array arr in ascending order */
  for(i=0; i<n-1; i++) {
    for(j=i+1; j<n; j++) {
      if(arr[j] < arr[i]) {
        /* swap */
        temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
      }
    }
  }
}

void util_quick_sort(int *arr, int first, int last) {
   int i, j, pivot, temp;

   if (first < last) {
      pivot = first;
      i = first;
      j = last;

      while (i < j) {
         while ( (arr[i] <= arr[pivot]) && (i < last) ) {
            i++;
         }
        
         while (arr[j] > arr[pivot]) {
            j--;
         }
         if (i < j) {
            temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
         }
      }

      temp = arr[pivot];
      arr[pivot] = arr[j];
      arr[j] = temp;
      util_quick_sort(arr, first, j-1);
      util_quick_sort(arr, j+1, last);
   }
}

double util_median(int *arr, int n) {
  double ret;

#ifdef USE_QUICK_SORT
  util_quick_sort(arr, 0, n-1);
#else
  util_bubble_sort(arr, n);
#endif  

  if(n%2==0) {
    ret = (arr[n/2] + arr[n/2 - 1]) / 2.0;
  } else {
    ret = arr[n/2];
  }
  
  return ret;
}