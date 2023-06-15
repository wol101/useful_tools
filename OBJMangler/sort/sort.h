#ifndef sort_h
#define sort_h

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __P
#define __P(x) x
#endif

void
qsort(void *base, size_t nmemb, size_t size,
             int (*compar) (const void *, const void *));

int
heapsort(void *base, size_t nmemb, size_t size,
        int (*compar) (const void *, const void *));

int
mergesort(void *base, size_t nmemb, size_t size,
        int (*compar) (const void *, const void *));

#ifdef __cplusplus
}
#endif

#endif

