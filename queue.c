#include "cacheSim.h"

/**
 * Name: enqueue
 * Description: Enqueues given int into the given queue. If the target is
 *              already in the queue, it is moved to the end.
 *
 * Parameters:
 *  queue  - int pointer to the target queue
 *  target - target value to be queued
 *  n      - length of the given queue
 */
void enqueue(int * queue, int target, int n){
  int i;
  for(i = 0; i < n; i++){
    //target is already in queue, move to end
    if(queue[i] == target){
      dequeue(queue, i, n);
      //recheck current index
      if(queue[i] == -1){
        queue[i] = target;
        return;
      }
    }else if(queue[i] == -1){
      queue[i] = target;
      return;
    }
  }
}

/**
 * Name: dequeue
 * Description: Dequeues the given target index from the queue.
 *
 * Parameters:
 *  queue - int pointer to the target queue
 *  index - index of the target value to be dequeued
 *  n     - length of the given queue
 */
void dequeue(int * queue, int index, int n){
  int i = index;
  for(i++; i < n; i++){
    queue[i-1] = queue[i];
  }
  queue[n-1] = -1;
}
