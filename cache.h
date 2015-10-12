#ifndef CACHE_H
#define CACHE_H
#include <stdbool.h>
//each line consists of a valid bit and
//a tag (note we aren't keeping track of
//the data in the cache)
typedef struct{
  int valid;
  int tag;
}lineType;

// A cache is an array of these (sets)
typedef struct{
  //Each set is an array of lines and an
  //array of line indices (lruQueue) used for
  //replacement
  lineType * line;
  int * lruQueue;
}setType;

// Contained the sets of the cache, and other internal variables.
typedef struct{
  //Sets, associativity, and block size
  int s,e,b;
  //Log2 of each of the above, respectively
  int sb,eb,bb;
  //Last accessed address
  unsigned int addr;
  //Internal tracked stats
  long accesses, hits, misses;
  //Code of last access
  bool hit;
  //The array of sets for the cache
  setType * set;
}cacheType;

cacheType * initCache(int, int, int);
void printCache(cacheType *);
void printResults(cacheType *);
bool check(cacheType *, unsigned int);
int getNextLine(cacheType *, int);
void store(cacheType *, unsigned int);
bool access(cacheType *, int, unsigned int);
int getSet(cacheType *, unsigned int);
int getTag(cacheType *, unsigned int);
int getOffset(cacheType *, unsigned int);
int getNumBits(unsigned int);
#endif
