//
// CSU33014 Summer 2020 Additional Assignment
// Part B of a two-part assignment
//
// Please write your solution in this file

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "csu33014-annual-partB-person.h"
#include <time.h>
#include <string.h>
#include <omp.h>

struct queue {
  int front;
  int rear;
  int size;
  int capacity;
  struct person ** people;
};

void enqueue(struct queue * queue, struct person * person){
  if(queue->size == queue-> capacity){
    assert(false);
  }
  queue -> rear = (queue->rear + 1) % queue -> capacity;
  queue -> people[queue -> rear] = person;
  queue -> size = queue -> size + 1;
}

struct person * dequeue(struct queue * queue){
    if(queue->size == 0){
      assert(false);
    }
  struct person * person = queue -> people[queue->front];
  queue -> front = (queue -> front + 1)%queue->capacity;
  queue -> size = queue -> size - 1;
  return person;
}

struct queue * new_queue(int capacity){
  struct queue * queue = malloc(sizeof(struct queue));
  queue -> capacity = capacity;
  queue -> size = 0;
  queue -> front = 0;
  queue -> rear = capacity - 1;
  queue -> people = malloc(queue->capacity * sizeof(struct person *));
  return queue;
}

void find_reachable_recursive(struct person * current, int steps_remaining,
			      bool * reachable) {
  // mark current root person as reachable
  reachable[person_get_index(current)] = true;
  // now deal with this person's acquaintances
  if ( steps_remaining > 0 ) {
    int num_known = person_get_num_known(current);
    for ( int i = 0; i < num_known; i++ ) {
      struct person * acquaintance = person_get_acquaintance(current, i);
      find_reachable_recursive(acquaintance, steps_remaining-1, reachable);
    }
  }
}

// computes the number of people within k degrees of the start person
int number_within_k_degrees(struct person * start, int total_people, int k) {
  bool * reachable;
  int count;

  // maintain a boolean flag for each person indicating if they are visited
  reachable = malloc(sizeof(bool)*total_people);
  for ( int i = 0; i < total_people; i++ ) {
    reachable[i] = false;
  }

  // now search for all people who are reachable with k steps
  find_reachable_recursive(start, k, reachable);

  // all visited people are marked reachable, so count them
  count = 0;
  for ( int i = 0; i < total_people; i++ ) {
    if ( reachable[i] == true ) {
      count++;
    }
  }
  return count;
}


/*
  Part B.1: the simple function number_within_k_degrees performs a lot 
  of redundant work because it visits the same nodes of the graph again 
  and again, even though they may have already been visited. Please 
  write a new version of the function that reduces or eliminates the 
  amount of redundant work. Please provide a very large (perhaps a page) 
  comment with the function explaining your algorithm for reducing 
  redundant visits, the complexity of your new algorithm, and the 
  complexity of the original algorithm in number_within_k_degrees. 
  Please feel free to upload supplemental material, such as drawings or 
  a PDF, if you think it would help to explain your solution and the 
  complexity. Your new function should replace the code in  
  less_redundant_number_within_k_degrees. [15 marks]
*/

/*
  The reduced redundancy function carries out breath-first search (bfs) for
  people reachable within k degrees, while the original function carried 
  out depth-first search (dfs).
  The original function carried out dfs with no checks, meaning it traversed 
  every possible path that was k-degrees long. There was no check if part of
  the path was travelled already, so if two seperate nodes converged into the
  same path, this path would be traveresed again. This will happen many times
  for many paths, which is redundant as once a path is marked as visited it 
  does not need to be checked again.
  More generally, as every path (of length k from the start) from a node is 
  explored, once a node is marked as visited, it does not need to be expanded
  again. To implement this we put a simple check; if the node is already marked
  as visited, don't look at its acquaintances, as these acquaintances have also
  been visited already. This means that no path is explored more than once, 
  removing all redundant checks, reducing the run-time by a large factor.
  bfs builds on this by visiting each person at its shortest distance from the
  start. First it checks all the people that are one degree away, then two degrees,
  then three degrees etc by queueing them, dequeueing them and counting them in 
  that order. So dfs with our redundancy check would have eliminated a person being
  expanded more than once, but when they were expanded they may have been expanded 
  after five degrees, whereas bfs will expand them after three degrees. So the 
  implementation notes each person only once, after finding it by the shortest amount
  of hops, therefore in the shortest amount of time. The solution is also iterative, 
  rather than recursive, meaning much less movement of variables and function calls
  to/from the function stack, which saves time. We also see an efficiency increase by
  keeping track of the people visited within an integer which we increment every 
  iteration, rahter than noting them in an array then iterating over the array again 
  afterwards. Using memset rather than a for loop to initalize the depths allows the
  compiler more freedom to implement it, which may increase efficiency. Implementing 
  the queue with an array rather than a linked list is also more efficient time wise,
  as access is O(1) rather than O(N) as we can access at an index rather than traversing
  pointers from the head/tail.
  The reduced redundancy algorithm processes every node reachable within k degrees once,
  so the complexity is O(N), where N is the number of nodes reachable.
  The unoptimized algorithm traverses every path of length k. In the worst case all N nodes
  are connected, so each node has N-1 ≈ N acquaintances. If k = 1, then the algorithm checks 
  about N acquaintances of start. If k = 2, then the algorithm checks about N acquaintances 
  of start, and each of their about N acquaintances, so will check N*N people, then if k = 3, 
  it will check N*N*N people, and so on. So the complexity is O(N^k). In practise the nodes 
  are not usually all connected so the run time will be faster, but we care about worst case 
  when talking about complexity. 
*/

// computes the number of people within k degrees of the start person;
// less repeated computation than the simple original version
int less_redundant_number_within_k_degrees(struct person * start,
					   int total_people, int k) {

  // initalize the array to hold the depths of each person from the start node
  // depth[index] == -1 indicates that the person has not been found yet
  // the depth of the start person is 0
  int * depths = malloc(sizeof(int)*total_people);
  memset(depths, -1, sizeof(int)*total_people);
  depths[person_get_index(start)] = 0;

  // initalize the queue to hold the people to allow for breadth-first search
  // the start person is at the head of the queue
  struct queue * queue = new_queue(total_people);
  enqueue(queue, start);

  int count = 0;
  while(queue -> size != 0){
    // dequeue another person and include them in the count
    struct person * current = dequeue(queue);
    count++;
    int current_index = person_get_index(current);
    // only expand the current node if its depth is less than k
    if(depths[current_index] < k) {
      // get each acquaintance of the current person
      int num_known = person_get_num_known(current);
      for (int i = 0; i < num_known; i++ ) {
        struct person * acquaintance = person_get_acquaintance(current, i);
        int acquaintance_index = person_get_index(acquaintance);
        // only enqueue the acquaintance if they haven't been found already
        // if they have been found, then they have/will be expanded so no need to expand again
        if(depths[acquaintance_index] == -1)
        {
          enqueue(queue, acquaintance);
          depths[acquaintance_index] = depths[current_index] + 1;
        }
      }
    }
  }
  return count;
}

/*
    Part B.2, write an efficient parallel implementation of your function that reduces 
    the amount of redundant work from Part B.1. You should write the parallel function 
    using OpenMP or pthreads. If it is difficult to parallelize your algorithm that 
    reduces redundancy, then please explain why it is difficult to parallelize, and 
    instead provide a parallel version of another reduced-redundancy algorithm, or the 
    original algorithm. Please provide a very large (perhaps a page) comment with the 
    function explaining your parallelization strategy and algorithm, and the complexity 
    of your algorithm if there are P parallel processing cores. Please feel free to 
    upload supplemental material, such as drawings or a PDF, if you think it would help 
    to explain your solution and the complexity. Your new function should replace the code 
    in parallel_number_within_k_degrees. [35 marks]
*/

// computes the number of people within k degrees of the start person;
// parallel version of the code
int parallel_number_within_k_degrees(struct person * start,
				     int total_people, int k) {
  // initalize the array to hold the depths of each person from the start node
  // depth[index] == -1 indicates that the person has not been found yet
  // the depth of the start person is 0
  int * depths = malloc(sizeof(int)*total_people);
  memset(depths, -1, sizeof(int)*total_people);
  depths[person_get_index(start)] = 0;

  // initalize the queue to hold the people to allow for breadth-first search
  // the start person is at the head of the queue
  struct queue * queue = new_queue(total_people);
  enqueue(queue, start);

  int count = 0;
  while(queue -> size != 0){
    // dequeue another person and include them in the count
    struct person * current = dequeue(queue);
    count++;
    int current_index = person_get_index(current);
    // only expand the current node if its depth is less than k
    if(depths[current_index] < k) {
      // get each acquaintance of the current person
      int num_known = person_get_num_known(current);
      #pragma omp parallel for
      for (int i = 0; i < num_known; i++ ) {
        struct person * acquaintance = person_get_acquaintance(current, i);
        int acquaintance_index = person_get_index(acquaintance);
        // only enqueue the acquaintance if they haven't been found already
        // if they have been found, then they have/will be expanded so no need to expand again
        if(depths[acquaintance_index] == -1)
        {
          #pragma omp critical
          {
            enqueue(queue, acquaintance);
          }
          depths[acquaintance_index] = depths[current_index] + 1;
        }
      }
    }
  }
  return count;  
}