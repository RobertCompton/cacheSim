#include "cacheSim.h"

/*
 *  Name: printResults
 *  Description: This function prints the results of the cache simulation.
 *  Parameters:
 *      accesses - total number of memory accesses
 *      hits - total number of cache hits
 *      misses - total number of cache misses
 *
*/
void printResults(cacheType * cache){
     printf("Accesses: %d, Hits: %d, Misses: %d, Miss Rate: %.4f\n",
             cache->accesses, cache->hits, cache->misses, ((float)cache->misses/(float)cache->accesses));
}

/*
 *  Name: printCache
 *  Description: This function prints out the most recent access to memory
 *               and the state of the cache after the most recent access
 *  Parameters:
 *      cache - any array of setType
 *      e - number of lines in each set
 *      s - number of sets in the cache
 *      address - address of the most recent access
 *      tag - tag portion of the address
 *      index - index portion of the address
 *      offset - offset portion of the address
 *      hit - 1 if the most recent access result in a hit, 0 otherwise
 */
void printCache(cacheType * cache){
    int i, j, empty, address, tag, index, offset, hit;
    address = cache->addr;
    tag     = getTag(cache, address);
    index   = getSet(cache, address);
    offset  = getOffset(cache, address);
    hit     = cache->hit;
    printf(" Address: %8x, Tag: %x, Index: %x, Offset: %x", address, tag,
             index, offset);
    if (hit) printf(" (hit)\n");
    else printf(" (miss)\n");
    printf("     Set: (tag, valid) (tag, valid) ... lruQueue: i0, i1,..\n");
    for (i = 0; i < cache->s; i++){
      empty = 1;
      for (j = 0; j < cache->e; j++){
        if (cache->set[i].line[j].valid) empty = 0;
      }
      //don't print an empty set; go back to top of loop
      if (empty) continue;
      printf("%8x: ", i);
      for (j = 0; j < cache->e; j++){
        printf("(%x, %d) ", cache->set[i].line[j].tag,
                            cache->set[i].line[j].valid);
      }
      printf("LRUs: ");
      for (j = 0; j < cache->e; j++)
          printf("%d, ", cache->set[i].lruQueue[j]);
      printf("\n");
   }
   printf("\n");
}


/*
 *  Name: initCache
 *  Description: This function creates the structure that represents the cache and
 *               returns a pointer to that structure.  The cache is an array of
 *               setTypes.  Each setType is array of lines and an array of
 *               line indices.  The array of line indices is used for implementing
 *               lru replacement.  Each line contains a valid bit and a tag.
 * Parameters:
 *      s - number of sets in the cace
 *      e - number of lines in each set
 * Returns:
 *      pointer to the cache structure
 */
cacheType *initCache(int s, int e, int b){
  int i, j;
  setType *sets;
  cacheType *cache;
  //create the s sets
  sets  = malloc(sizeof(setType) * s);
  cache = malloc(sizeof(cacheType));
  cache->s        = s;
  cache->e        = e;
  cache->b        = b;
  cache->sb       = getNumBits(s);
  cache->eb       = getNumBits(e);
  cache->bb       = getNumBits(b);
  cache->accesses = 0;
  cache->hits     = 0;
  cache->misses   = 0;
  cache->set      = sets;
  //*cache = {.s=s, .e=e, .b=b,
          //.sb=getNumBits(s),
          //.eb=getNumBits(e),
          //.bb=getNumBits(b),
          //.set=sets};
  for (i = 0; i < s; i++){
    //create the e lines and the array of line indices
    sets[i].line     = malloc(sizeof(lineType) * e);
    sets[i].lruQueue = malloc(sizeof(int) * e);
  }
  //initialize all valid bits to 0 and the lruQueue entries to -1
  for (i = 0; i < s; i++)
    for (j = 0; j < e; j++){
      sets[i].line[j].valid = 0;
      sets[i].lruQueue[j]   = -1;
    }
  return cache;
}

/**
 * Name: getSet
 * Description: Returns the set index for the given addr under the parameters
 *              contained within the given cacheType object.
 * Parameters:
 *  cache - pointer to the cacheType object.
 *  addr  - address of interest, cast to an unsigned int.
 * Returns:
 *  The set index for the address as an int.
 */
int getSet(cacheType * cache, unsigned int addr){
  int sBits, bBits, tBits;
  sBits = cache->sb;
  bBits = cache->bb;
  tBits = sizeof(addr)*8 - sBits - bBits;
  return ((addr << tBits) >> tBits) >> bBits;
}

/**
 * Name: getTag
 * Description: Returns the tag of the given addr under the parameters
 *              contained within the given cacheType object.
 * Parameters:
 *  cache - pointer to the cacheType object.
 *  addr  - address of interest, cast to an unsigned int.
 * Returns:
 *  The tag of the address as an int.
 */
int getTag(cacheType * cache, unsigned int addr){
  int sBits, bBits;
  sBits = cache->sb;
  bBits = cache->bb;
  return (addr >> (sBits + bBits));
}

/**
 * Name: getOffset
 * Description: Returns the byte offset of the given addr under the parameters
 *              contained within the given cacheType object.
 * Parameters:
 *  cache - pointer to the cacheType object.
 *  addr  - address of interest, cast to an unsigned int.
 * Returns:
 *  The byte offset of the address as an int.
 */
int getOffset(cacheType * cache, unsigned int addr){
  int bBits = cache->bb;
  int shift = sizeof(addr)*8 - bBits;
  return (addr << shift) >> shift;
}

/**
 * Name: check
 * Description: Checks whether the given address is a hit in the passed cache
 *              structure. Returns a bool to represent the answer. Also updates
 *              the lru queue for the set on a cache hit.
 * Parameters:
 *  cache - pointer to the cacheType object.
 *  addr  - address of interest. cast to an unsigned int.
 * Returns:
 *  A bool of value 'true' if a hit, and 'false' if a miss.
 */
bool check(cacheType * cache, unsigned int addr){
  int set, tag, i;
  set = getSet(cache, addr);
  tag = getTag(cache, addr);
  for(i = 0; i < cache->e; i++){
    if (cache->set[set].line[i].tag == tag
        && cache->set[set].line[i].valid == 1){
      enqueue(cache->set[set].lruQueue, i, cache->e);
      return true;
    }
  }return false;
}

/**
 * Name: getNextLine
 * Description: Gets the next line in the given set in the given cache that
 *              should be written to. If there is an invalid line, the first
 *              index that is invalid will be returned. If there are no invalid
 *              lines in the set, then the first index in the lru queue will be
 *              returned.
 * Parameters:
 *  cache - pointer to the cacheType object.
 *  set   - index of the set in question.
 *
 * Returns:
 *  An int for the index of the line that should be next stored to.
 */
int getNextLine(cacheType * cache, int set){
  int i;
  for(i = 0; i < cache->e; i++){
    if(cache->set[set].line[i].valid == 0) return i;
  }return cache->set[set].lruQueue[0];
}

/**
 * Name: store
 * Description: Stored the given addr into the given cache object, sets the
 *              valid bit of the modified line to 'true', updates the tag, and
 *              updates the lru queue for the set.
 * Parameters:
 *  cache - pointer to the cacheType object.
 *  addr  - address of interest, cast to an unsigned int.
 */
void store(cacheType * cache, unsigned int addr){
  int line, set, tag;
  set  = getSet(cache, addr);
  tag  = getTag(cache, addr);
  line = getNextLine(cache, set);
  cache->set[set].line[line].tag = tag;
  cache->set[set].line[line].valid = 1;
  enqueue(cache->set[set].lruQueue, line, cache->e);
}

/**
 * Name: access
 * Description: Simulates an access to into memory with the passed cache object,
 *              access flag, and given address. Updates the cache as needed and
 *              returns a bool based on success of the access.
 * Parameters:
 *  cache - pointer to the cacheTYpe object.
 *  flag  - int representing the type of memory access
 *  addr  - address of interest
 *
 * Returns:
 *  A bool of value 'true' if a cache hit and 'false' if a cache miss.
 */
bool access(cacheType * cache, int flag, unsigned int addr){
    if(flag < 8){
      cache->accesses++;
      cache->addr = addr;
      cache->hit = check(cache, addr);
      if(cache->hit){
        cache->hits++;
      }else{
        cache->misses++;
        store(cache, addr);
      }
    }
    return cache->hit;
}

/**
 * Name: getNumBits
 * Description: Returns the number of bits needed to represent the given value.
 *              Assumes the given value is a power of two.
 * Parameters:
 *  num - number to be loged of 2.
 *
 * Returns:
 *  An int which is the log2(num).
 */
int getNumBits(unsigned int num){
  int i = 0;
  while(num > 1){
    num = num >> 1;
    i++;
  }
  return i;
}
