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


*/

void find_reachable_reduced_recursive(struct person * current, int k, int steps_remaining,
			      bool * reachable, bool * visited, int * reachable_steps, struct queue * queue) {
  // mark current root person as reachable
  int current_index = person_get_index(current);
  reachable[current_index] = true;
  visited[current_index] = true;
  reachable_steps[current_index] = k - steps_remaining;
  // now deal with this person's acquaintances
  if ( steps_remaining > 0 ) {
    int num_known = person_get_num_known(current);
    for ( int i = 0; i < num_known; i++ ) {
      struct person * acquaintance = person_get_acquaintance(current, i);
      //enqueue(queue, acquaintance);
      int acquaintance_index = person_get_index(acquaintance);
      // if the acquaintance has already been visited, and the number of steps to it is lower than our current number to it,
      // then we will not carry out any updates on this recursive branch, so we don't take it
      if(!visited[acquaintance_index] || (visited[acquaintance_index] && (k-(steps_remaining-1) < reachable_steps[acquaintance_index])))
      {
        //find_reachable_reduced_recursive(acquaintance, k, steps_remaining-1, reachable, visited, reachable_steps, queue);
        enqueue(queue, acquaintance);
      }
    }
  }
}

// computes the number of people within k degrees of the start person;
// less repeated computation than the simple original version
int less_redundant_number_within_k_degrees(struct person * start,
					   int total_people, int k) {
  int count = 0;
  int * depths;         // to mark whether a person has already been visited by the algorithm
  struct queue * queue;
  int depth;

  depths = malloc(sizeof(int)*total_people);
  queue = new_queue(total_people);
  for ( int i = 0; i < total_people; i++ ) {
    depths[i] = -1;
  }

  // now search for all people who are reachable with k steps
  enqueue(queue, start);
  depth = depths[person_get_index(start)] = 0;

  while(queue -> size != 0){
    struct person * current = dequeue(queue);
    int current_index = person_get_index(current);
    depth = depths[current_index];
    count++;
    int num_known = person_get_num_known(current);
    if(depths[current_index] < k) {
      for (int i = 0; i < num_known; i++ ) {
        struct person * acquaintance = person_get_acquaintance(current, i);
        int acquaintance_index = person_get_index(acquaintance);
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


// computes the number of people within k degrees of the start person;
// parallel version of the code
int parallel_number_within_k_degrees(struct person * start,
				     int total_people, int k) {
  return number_within_k_degrees(start, total_people, k);
}