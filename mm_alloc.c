/*
 * mm_alloc.c
 *
 * Stub implementations of the mm_* routines. Remove this comment and provide
 * a summary of your allocator's design here.
 */

#include "mm_alloc.h"
#define align4(x) ((((x) - 1) >> 2) << 2) + 4
#include <stdlib.h>

/* Your final implementation should comment out this macro. */
#define MM_USE_STUBS
#define BLOCK_SIZE 20

void *base = NULL;

s_block_ptr extend_heap(s_block_ptr last, size_t size);
s_block_ptr find_block(s_block_ptr last, size_t size);
s_block_ptr get_block(void *p);
void split_block(s_block_ptr b, size_t size);
void copy_block(s_block_ptr src, s_block_ptr dst);


void copy_block(s_block_ptr src, s_block_ptr dst){
	int *sdata, *ddata;
	size_t i;
	sdata = src->ptr;
	ddata = dst->ptr;
	for(i = 0; i * 4 < src->size && i * 4 < dst->size; i++)
	ddata[i] = sdata[i];
}


s_block_ptr find_block(s_block_ptr last, size_t size){
	s_block_ptr b = base;
	while (b && !(b->free && b->size >= size)){
	last = b;
	b = b->next;
	}
	return b;
}
void* mm_malloc(size_t size)
{
	 s_block_ptr b, last;
	size_t s;
	s = align4(size);
	if(base){
	last = base;
	b = find_block(last, s);
	if(b){
	if((b->size - s) >= (BLOCK_SIZE + 4)) split_block(b, s);
	b->free = 0;
	}
	else{
	b = extend_heap(last, s);
	if(!b) return NULL;
	}
	}
	else{
	b = extend_heap(NULL, s);
	if(!b) return NULL;
	base = b;
	}
	return b->data;
}

void* mm_realloc(void* ptr, size_t size)
{
	 size_t s;
	s_block_ptr b, new;
	void *newp;
	if(!ptr) return (malloc(size));
	if (valid_addr(ptr)){
	s = align4(size);
	b = get_block(ptr);
	if (b->size >= s){
	if(b->size - s >= (BLOCK_SIZE + 4)) split_block(b, s);
	}
	else{
	if(b->next && b->next->free && (b->size + BLOCK_SIZE + b->next->size) >= s){
	fusion(b);
	if(b->size - s >= (BLOCK_SIZE + 4)) split_block(b, s);
	}
	else{
	newp = malloc(s);
	if (!newp) return NULL;
	new = get_block(newp);
	copy_block(b, new);
	free(ptr);
	return newp;
	}
	}
	return ptr;
	}
	return NULL;
}

int valid_addr(void *p){
	if (base){
	if(p > base && p < sbrk(0)) return (p == (get_block(p))->ptr);
	}
	return 0;
}

void mm_free(void* ptr)
{

	s_block_ptr b;
	if (valid_addr(ptr)){
	b = get_block(ptr);
	b->free = 1;
	if(b->prev && b->prev->free) b = fusion(b->prev);
	if(b->next){
	fusion(b);
	}
	else{
	if(b->prev) b->prev->next = NULL;
	else base = NULL;
	brk(b);
	}
	}
}
