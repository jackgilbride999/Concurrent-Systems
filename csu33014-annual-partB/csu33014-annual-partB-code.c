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

void find_reachable_reduced_recursive(struct person * current, int k, int steps_remaining,
			      bool * reachable, bool * visited, int * reachable_steps) {
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
      int acquaintance_index = person_get_index(acquaintance);
      // if the acquaintance has already been visited, and the number of steps to it is lower than our current number to it,
      // then we will not carry out any updates on this recursive branch, so we don't take it
      if(!visited[acquaintance_index] || (visited[acquaintance_index] && (k-(steps_remaining-1) < reachable_steps[acquaintance_index]))   )
      {
        find_reachable_reduced_recursive(acquaintance, k, steps_remaining-1, reachable, visited, reachable_steps);
      }
    }
  }
}

// computes the number of people within k degrees of the start person;
// less repeated computation than the simple original version
int less_redundant_number_within_k_degrees(struct person * start,
					   int total_people, int k) {
  bool * reachable;
  int count;
  bool * visited;         // to mark whether a person has already been visited by the algorithm
  int * reachable_steps;  // to mark how many steps a person is known to be reachable by, if they have been marked as reachable

  // maintain a boolean flag for each person indicating if they are reachable
  reachable = malloc(sizeof(bool)*total_people);
  visited = malloc(sizeof(bool)*total_people);
  reachable_steps = malloc(sizeof(int)*total_people);
  for ( int i = 0; i < total_people; i++ ) {
    reachable[i] = false;
    visited[i] = false;
    reachable_steps[i] = 0;
  }

  // now search for all people who are reachable with k steps
  find_reachable_reduced_recursive(start, k, k, reachable, visited, reachable_steps);

  // all visited people are marked reachable, so count them
  count = 0;
  for ( int i = 0; i < total_people; i++ ) {
    if ( reachable[i] == true ) {
      count++;
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







