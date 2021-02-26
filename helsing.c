/*
 * Copyright (c) 2021, Pierro Zachareas, et al.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <pthread.h>

// Compile with: gcc -O3 -Wall -Wextra -pthread -o helsing helsing.c


/*--------------------------- COMPILATION OPTIONS  ---------------------------*/
#define NUM_THREADS 8 // Thread count above #cores may not improve performance
#define ITERATOR 10000000ULL // How long until new work is assigned to threads

//#define __SANITY_CHECK__
//#define __OEIS_OUTPUT__

#define MEASURE_RUNTIME
#define PRINT_VAMPIRE_COUNT

/*---------------------------- PREPROCESSOR_STUFF ----------------------------*/
#ifndef __OEIS_OUTPUT__

#ifdef PRINT_VAMPIRE_COUNT
#define __RESULTS__
#endif

#if defined(MEASURE_RUNTIME) && (defined(CLOCK_MONOTONIC) || defined(CLOCK_REALTIME))
#define SPD_TEST

#if defined(CLOCK_MONOTONIC)
#define SPDT_CLK_MODE CLOCK_MONOTONIC
#elif defined(CLOCK_REALTIME)
#define SPDT_CLK_MODE CLOCK_REALTIME
#endif

#endif

#endif

/*----------------------------- ULLONG FUNCTIONS -----------------------------*/

unsigned long long atoull (const char *str)	// ASCII to unsigned long long
{
	unsigned long long number = 0;
	for (unsigned long long i = 0; str[i] >= '0' && str[i] <= '9'; i++) {
		number = 10 * number + (str[i] - '0');
	}
	return (number);
}
/*
unsigned long long u_length_normal (unsigned long long number)
{
	unsigned long long length = 1;
	for (; number > 9; length++) {
		number = number / 10;	// Using division to avoid overflow for numbers around 2^64 -1
	}
	return (length);
}
*/
unsigned long long u_length_bin (unsigned long long number)
{
	if (number < 10000000000ULL){
		if (number < 100000ULL) {
			if (number < 1000ULL){
				if (number < 10ULL)
					return 1;
				else if (number < 100ULL)
					return 2;
				else
					return 3;
			} else {
				if (number < 10000ULL)
					return 4;
				else
					return 5;
			}
		} else {
			if (number < 100000000ULL) {
				if (number < 1000000ULL)
					return 6;
				else if (number < 10000000ULL)
					return 7;
				else
					return 8;
			} else {
				if (number < 1000000000ULL)
					return 9;
				else
					return 10;
			}
		}
	} else {
		if (number < 1000000000000000ULL) {
			if (number < 10000000000000ULL) {
				if (number < 100000000000ULL)
					return 11;
				else if (number < 1000000000000ULL)
					return 12;
				else
					return 13;
			} else {
				if (number < 100000000000000ULL)
					return 14;
				else
					return 15;
			}
		} else {
			if (number < 1000000000000000000ULL){
				if (number < 10000000000000000ULL)
					return 16;
				else if (number < 100000000000000000ULL)
					return 17;
				else
					return 18;
			} else {
				if (number < 10000000000000000000ULL)
					return 19;
				else
					return 20;
			}
		}
	}
}

int u_length_isodd (unsigned long long number)
{
	unsigned long long result = 1;
	while (number > 9) {
		number = number / 10;	// Using division to avoid overflow for numbers around 2^64 -1
		result = !result;
	}
	return (result);
}

unsigned long long ten_pow_bin (unsigned long long exponent) // Returns 10 ^ exponent
{
#ifdef __SANITY_CHECK__
	assert(exponent <= u_length_bin(ULLONG_MAX));
#endif

	if (exponent < 10){
		if (exponent < 5) {
			if (exponent < 3){
				if (exponent == 0)
					return 1ULL;
				else if (exponent == 1)
					return 10ULL;
				else
					return 100ULL;
			} else {
				if (exponent == 3)
					return 1000ULL;
				else
					return 10000ULL;
			}
		} else {
			if (exponent < 8) {
				if (exponent == 5)
					return 100000ULL;
				else if (exponent == 6)
					return 1000000ULL;
				else
					return 10000000ULL;
			} else {
				if (exponent == 8)
					return 100000000ULL;
				else
					return 1000000000ULL;
			}
		}
	} else {
		if (exponent < 15) {
			if (exponent < 13) {
				if (exponent == 10)
					return 10000000000ULL;
				else if (exponent == 11)
					return 100000000000ULL;
				else
					return 1000000000000ULL;
			} else {
				if (exponent == 13)
					return 10000000000000ULL;
				else
					return 100000000000000ULL;
			}
		} else {
			if (exponent < 18){
				if (exponent == 15)
					return 1000000000000000ULL;
				else if (exponent == 16)
					return 10000000000000000ULL;
				else
					return 100000000000000000ULL;
			} else {
				if (exponent == 18)
					return 1000000000000000000ULL;
				else
					return 1000000000000000000ULL;
			}
		}
	}
}

unsigned long long u_lastzero (unsigned long long number){
	return( !(number - ((number / 10) * 10)) );
}

/*---------------------------------- ULIST  ----------------------------------*/

typedef struct ulist	// List of unsigned long long
{
	unsigned long long *array;	// Set to NULL when the array is empty. Check if NULL before traversing the array!
	unsigned long long last;	// Code should traverse the array up to [last]. May be smaller or equal than the actual size of the array.
} ulist;

ulist *ulist_init(unsigned long long number)
{
	ulist *new = malloc(sizeof(ulist));
	assert(new != NULL);

	new->last = u_length_bin(number) - 1;

	new->array = malloc(sizeof(unsigned long long) * (new->last + 1));
	assert(new->array != NULL);

	for (unsigned long long i = new->last; i > 0; i--) {
		new->array[i] = number % 10;
		number = number/10;
	}
	new->array[0] = number % 10;
	return (new);
}

int ulist_free(ulist* ulist_ptr)
{
#ifdef __SANITY_CHECK__
	assert(ulist_ptr != NULL);
#endif

	if (ulist_ptr->array != NULL) {
		free(ulist_ptr->array);
		ulist_ptr->array = NULL;
	}
	free(ulist_ptr);
	return (0);
}

ulist *ulist_copy(ulist *original)
{
#ifdef __SANITY_CHECK__
	assert(original != NULL);
#endif

	ulist *copy = malloc(sizeof(ulist));
	assert(copy != NULL);

	copy->last = original->last;

	if (original->array != NULL) {
		copy->array = malloc(sizeof(unsigned long long)*(copy->last + 1));
		assert(copy->array != NULL);

		for (unsigned long long i = 0; i<=copy->last; i++) {
			copy->array[i] = original->array[i];
		}
	} else
		copy->array = NULL;

	return (copy);
}

int ulist_remove(ulist* ulist_ptr, unsigned long long element)
{
#ifdef __SANITY_CHECK__
	assert(ulist_ptr != NULL);
	assert(element <= ulist_ptr->last);
#endif

	if (ulist_ptr->array != NULL) {
		if (ulist_ptr->last == 0) {
			free(ulist_ptr->array);
			ulist_ptr->array = NULL;
		} else {
			for (; element < ulist_ptr->last; element++) {
				ulist_ptr->array[element] = ulist_ptr->array[element + 1];
			}
			ulist_ptr->last = ulist_ptr->last - 1;
		}
	}
	return(0);
}

int ulist_pop(ulist* ulist_ptr, unsigned long long number)
{
#ifdef __SANITY_CHECK__
	assert(ulist_ptr != NULL);
#endif

	if (ulist_ptr->array != NULL) {
		for (unsigned long long i = 0; i <= ulist_ptr->last; i++) {
			if (ulist_ptr->array[i] == number) {
				if (ulist_ptr->last == 0) {
					free(ulist_ptr->array);
					ulist_ptr->array = NULL;
				} else {
					for (; i < ulist_ptr->last; i++) {
						ulist_ptr->array[i] = ulist_ptr->array[i+1];
					}
					ulist_ptr->last = ulist_ptr->last - 1;
				}
				return (1);
			}
		}
	}
	return(0);
}

unsigned long long ulist_combine_digits(ulist *digits)
{
#ifdef __SANITY_CHECK__
	assert(digits != NULL);
#endif

	unsigned long long number = 0;
	if (digits->array != NULL) {
		for (unsigned long long i = 0; i <= digits->last; i++) {
			number = (number * 10)+ digits->array[i];
		}
	}
	return (number);
}

unsigned long long ulist_lastnotzero(ulist *ulist_ptr)
{
#ifdef __SANITY_CHECK__
	assert(ulist_ptr != NULL);
#endif

	if (ulist_ptr->array != NULL) {
		return(ulist_ptr->array[ulist_ptr->last] != 0);
	}
	return (0);
}

/*---------------------------------- LLIST  ----------------------------------*/

typedef struct llist	/* Linked list of unsigned long longegers */
{
	unsigned long long number;
	struct llist *next;
} llist;

llist *llist_init(unsigned long long number, llist* next)
{
	llist *new = malloc(sizeof(llist));
	assert(new != NULL);
	new->number = number;
	new->next = next;
	return (new);
}

int llist_free(llist *llist_ptr)
{
	for (llist *temp = NULL; llist_ptr != NULL ;) {
		temp = llist_ptr;
		llist_ptr = llist_ptr->next;
		free(temp);
	}
	return (0);
}

llist *get_fangs(unsigned long long dividend)
{
	llist *divisors = NULL;
	unsigned long long dividend_length = u_length_bin(dividend);
	unsigned long long fang_length = dividend_length / 2;
	unsigned long long divisor = ten_pow_bin(fang_length - 1);
	for (; u_length_bin(dividend/divisor) > fang_length; divisor++) {}

	for (llist *current = NULL; divisor * divisor <= dividend; divisor++) {
		if (dividend % divisor == 0 && !( u_lastzero(divisor) && u_lastzero(dividend/divisor))) {
			if (current == NULL) {
				divisors = llist_init(divisor, NULL);
				current = divisors;
			} else {
				current->next = llist_init(divisor, current->next);
				current = current->next;
			}
		}
	}
	return (divisors);
}

llist *llist_copy(llist *original)
{
	llist *copy = NULL;
	if (original != NULL) {
		copy = llist_init(original->number, NULL);
		llist *current = copy;
		for (llist *i = original->next; i != NULL; i = i->next) {
			llist *llist_ptr = llist_init(i->number, NULL);
			current->next = llist_ptr;
			current = current->next;
		}
	}
	return (copy);
}

/*---------------------------------- LLHEAD ----------------------------------*/

typedef struct llhead
{
	struct llist *first;
	struct llist *last;
} llhead;

llhead *llhead_init(llist *first, llist *last)
{
	llhead *new = malloc(sizeof(llhead));
	assert(new != NULL);
	new->first = first;
	if (last != NULL)
		new->last = last;
	else
		new->last = new->first;
	return (new);
}

int llhead_free(llhead *llhead_ptr)
{
	if (llhead_ptr != NULL) {
		llist_free(llhead_ptr->first);
	}
	free(llhead_ptr);
	return (0);
}

llhead *llhead_copy(llhead *original) {
	llist *first = NULL;
	first = llist_copy(original->first);
	llist *last = NULL;	/* this works but technically it shouldn't be just NULL. */
	llhead *copy = llhead_init(first,last);
	return (copy);
}

int llhead_pop(llhead *llhead_ptr, unsigned long long number)
{
	llist *prev = NULL;
	for (llist *i = llhead_ptr->first; i != NULL; i = i->next) {
		if (i->number == number) {
			if (i == llhead_ptr->first) {
				llhead_ptr->first = i->next;
			}
			if (i == llhead_ptr->last) {
				llhead_ptr->last = prev;
			}
			if (prev != NULL) {
				prev->next = i->next;
			}
			free(i);
			return (1);
		}
		else
			prev = i;
	}
	return (0);
}

/*----------------------------------------------------------------------------*/

typedef struct vargs	/* Vampire arguments */
{
	unsigned long long min;
	unsigned long long max;
	unsigned long long step;
	unsigned long long count;
	double	runtime;
	double 	algorithm_runtime;
	llhead *result;
} vargs;

vargs *vargs_init(unsigned long long min, unsigned long long max, unsigned long long step)
{
	vargs *new = malloc(sizeof(vargs));
	assert(new != NULL);
	new->min = min;
	new->max = max;
	new->step = step;
	new->count = 0;
	new->runtime = 0.0;
	new->algorithm_runtime = 0.0;
	new->result = llhead_init(NULL, NULL);
	return new;
}

int vargs_free(vargs *vargs_ptr)
{
	llhead_free(vargs_ptr->result);
	free (vargs_ptr);
	return (0);
}
int vargs_split(vargs *input[], unsigned long long min, unsigned long long max)
{
	unsigned long long current = 0;

	input[current]->min = min;
	input[current]->max = (max-min)/(NUM_THREADS - current) + min;

	for (; input[current]->max < max;) {
		min = input[current]->max + 1;
		current ++;
		input[current]->min = min;
		input[current]->max = (max-min)/(NUM_THREADS - current) + min;
	}

	return(current);
}
/*----------------------------------------------------------------------------*/

void *vampire(void *void_args)
{

#ifdef SPD_TEST
	struct timespec start, finish;
	double elapsed;
	double fang_time;
	clock_gettime(SPDT_CLK_MODE, &start);
#endif

	vargs *args = (vargs *)void_args;
	int is_not_vampire;
	int is_vampire;
	unsigned long long j;

	/* iterate all numbers */
	for (unsigned long long number = args->min; number <= args->max; number += args->step) {
		if (u_length_isodd(number)) {
			assert(0);
			continue;
		}
		is_vampire = 0;

#ifdef SPD_TEST
	struct timespec fang_start, fang_finish;
	clock_gettime(SPDT_CLK_MODE, &fang_start);
#endif
		llist *divisors = get_fangs(number);
#ifdef SPD_TEST
	clock_gettime(SPDT_CLK_MODE, &fang_finish);

	fang_time = (fang_finish.tv_sec - fang_start.tv_sec);
	fang_time += (fang_finish.tv_nsec - fang_start.tv_nsec) / 1000000000.0;
	args->algorithm_runtime += fang_time;
#endif

		ulist *digits = ulist_init(number);
		for (llist *i = divisors; i != NULL && is_vampire != 1; i = i->next) { /* iterate all divisors */

			ulist *digit = ulist_copy(digits);
			ulist* divisor = ulist_init(i->number);
			ulist* quotient = ulist_init(number / i->number);

			is_not_vampire = 0;
			/* iterate all divisor digits */
			for (j = 0; !is_not_vampire && j <= divisor->last && digit->array != NULL; j++) {
				if (!ulist_pop(digit,divisor->array[j]))
					is_not_vampire = 1;
			}
			for (j = 0; !is_not_vampire && j <= quotient->last && digit->array != NULL; j++) {
				if (!ulist_pop(digit,quotient->array[j]))
					is_not_vampire = 1;
			}
			if (!is_not_vampire && digit->array == NULL) {
#if defined(__OEIS_OUTPUT__)
				if (args->result->first == NULL) {
					args->result->first = llist_init(number, NULL);
					args->result->last = args->result->first;
				} else {
					args->result->last->next = llist_init(number, NULL);
					args->result->last = args->result->last->next;
				}
#endif
				args->count ++;
				is_vampire = 1;
				/*
				printf("%llu %llu\n", args->count, number);
				printf("%llu / %llu = %llu\n", number, i->number, number / i->number);
				*/
			}
			ulist_free(digit);
			ulist_free(divisor);
			ulist_free(quotient);
		}
		llist_free(divisors);
		ulist_free(digits);
	}

#ifdef SPD_TEST
	clock_gettime(SPDT_CLK_MODE, &finish);
	elapsed = (finish.tv_sec - start.tv_sec);
	elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
	args->runtime += elapsed;
	/*
	printf("%llu \t%llu \t%llu\tin %lf\n", args->min, args->max, counter, elapsed);
	*/
#endif
	return(0);
}

int main(int argc, char* argv[])
{
	if (argc != 3) {
		printf("Usage: vampire [min] [max]\n");
		return (0);
	}
	unsigned long long i;
	unsigned long long min = atoull(argv[1]);
	unsigned long long max = atoull(argv[2]);

	unsigned long long new_min = 1;
	unsigned long long min_length = u_length_bin(min);
	new_min = ten_pow_bin(min_length);
	if (u_length_isodd(min)) {
		if (new_min <= max) {
			min = new_min;
		}
	}
	unsigned long long lmax = max;
	if (u_length_isodd(max)) {
		unsigned long long new_max = 0;
		unsigned long long max_length = u_length_bin(max);

		new_max = ten_pow_bin(max_length-1) - 1;
		if (min <= new_max)
			max = new_max;
	}
	unsigned long long lmin = min;
	if (lmax > 10 * new_min -1) {
		lmax = 10 * new_min -1;
	}

	int rc;
	pthread_t threads[NUM_THREADS];
	vargs *input[NUM_THREADS];
	unsigned long long thread;
	for (i = 0; i < NUM_THREADS; i++) {
		input[i] = vargs_init(0, 0, 1);
	}
	llhead *result_list = llhead_init(NULL, NULL);
	unsigned long long iterator = ITERATOR;
	unsigned long long active_threads = NUM_THREADS;

	for (; lmax <= max;) {
		for (i = lmin; i <= lmax; i += iterator + 1) {
			if (lmax-i < iterator) {
				iterator = lmax-i;
			}
			active_threads = vargs_split(input, i, i + iterator) + 1;

			for (thread = 0; thread < active_threads; thread++) {
				rc = pthread_create(&threads[thread], NULL, vampire, (void *)input[thread]);
				if (rc) {
					printf("Error:unable to create thread, %d\n", rc);
					exit(-1);
				}
			}
			for (thread = 0; thread < active_threads; thread++) {
				pthread_join(threads[thread], 0);
#if defined(__OEIS_OUTPUT__)
				if (input[thread]->result->first != NULL) {
					if (result_list->first == NULL) {
						result_list->first = input[thread]->result->first;
						result_list->last = input[thread]->result->last;
					} else {
						result_list->last->next = input[thread]->result->first;
						result_list->last = input[thread]->result->last;
					}
				}
				input[thread]->result->first = NULL;
				input[thread]->result->last = NULL;
#endif
			}
		}
		if (lmin * 100 <= max)
			lmin *= 100;

		if (lmax == max)
			break;

		else if (lmax *100 <= max)
			lmax = lmax * 100 + 99;

		else if (lmax != max)
			lmax = max;

		if (lmax-lmin > ITERATOR)
			iterator = ITERATOR;
		else
			iterator = lmin -lmax;

	}
	unsigned long long result = 0;

#ifdef SPD_TEST
	double algorithm_time = 0.0;
	printf("Thread  Count   Rutime\n");
#endif

	for (thread = 0; thread<NUM_THREADS; thread++) {
		result += input[thread]->count;

#ifdef SPD_TEST
		printf("%llu\t%llu\t%lf\t[%llu\t%llu]\n", thread, input[thread]->count, input[thread]->runtime, input[thread]->min, input[thread]->max);
		algorithm_time += input[thread]->algorithm_runtime,
#endif
		vargs_free(input[thread]);
	}
#ifdef SPD_TEST
	printf("Fang search took: %lf, average: %lf\n", algorithm_time, algorithm_time / NUM_THREADS);
#endif
#if defined(__OEIS_OUTPUT__)
	unsigned long long k = 1;
	llist *temp;
	for (temp = result_list->first; temp != NULL; temp = temp->next) {
		printf("%llu %llu\n", k, temp->number);
		k++;
	}
#endif

#if defined(__RESULTS__)
	printf("Found: %llu vampire numbers.\n", result);
#endif
	llhead_free(result_list);

	pthread_exit(NULL);
	return (0);
}
