#include "vector.h"
#include "search.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>



const int BASE_SIZE = 10;

void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation){
    assert(elemSize > 0);
    if (initialAllocation <= 0){
        v->aloc_len = BASE_SIZE;
    } else v->aloc_len = initialAllocation;

    v->log_len = 0;
    v->elem_size = elemSize;
    v->data      = malloc (v->elem_size * v->aloc_len);
    assert(v->data != NULL);
    v->freeFn    = freeFn;
     
}

void VectorDispose(vector *v){
    if (v->freeFn != NULL){
        int i;
        for (i = 0; i < v->log_len; i++){
            void *curr = (char*)v->data + i * v->elem_size;
            v->freeFn(curr);
        }
    }

    free(v->data);
}

int VectorLength(const vector *v){ 
    assert(v != NULL);
    return v->log_len; 
 }

void *VectorNth(const vector *v, int position){
     assert(position >= 0 && position < v->log_len);
     return (char*)v->data + position * v->elem_size; 
}

void VectorReplace(vector *v, const void *elemAddr, int position){
    assert(elemAddr != NULL);
    assert(position >= 0 && position < v->log_len);
    void *curr = (char*)v->data + position * v->elem_size;
    if (v->freeFn != NULL) v->freeFn(curr);
    memcpy(curr, 
           elemAddr, 
           v->elem_size);
}


void grow(vector *v){
    v->aloc_len *= 2;
    v->data = realloc (v->data, 
                       v->aloc_len * v->elem_size);
    assert (v->data != NULL);
}

void VectorAppend(vector *v, const void *elemAddr){
    if (v->log_len == v->aloc_len) grow(v);
    memcpy((char*)v->data + v->log_len * v->elem_size, 
           elemAddr, 
           v->elem_size);
    v->log_len++;
}


void VectorInsert(vector *v, const void *elemAddr, int position){

    assert(elemAddr != NULL);
    

     if (position == v->log_len){
        VectorAppend(v, elemAddr);
        return;
    }

    assert(position >= 0 && position < v->log_len);

    if (v->log_len == v->aloc_len) grow(v);

    memmove((char*)v->data + (position + 1)*v->elem_size,
            (char*)v->data + position*v->elem_size,
            v->elem_size*(v->log_len - position));
    
    memcpy((char*)v->data + position * v->elem_size,
            elemAddr,
            v->elem_size);

    v->log_len++;
}


void VectorDelete(vector *v, int position){
    assert(position >= 0 && position < v->log_len);

    if (v->freeFn != NULL){
        v->freeFn((char*)v->data + position * v->elem_size);
    }
    
    memmove((char*)v->data + position*v->elem_size,
            (char*)v->data + (position + 1) * v->elem_size,
            v->elem_size * (v->log_len - position - 1));
    v->log_len--;
}

void VectorSort(vector *v, VectorCompareFunction compare){
    assert(compare != NULL && v != NULL);
    qsort(v->data, 
         v->log_len,
         v->elem_size,
         compare);
}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData){
    assert(mapFn != NULL);
    int i;
    for (i = 0; i < v->log_len; i++){
        mapFn((char*)v->data + i * v->elem_size, auxData);
    }
}

static const int kNotFound = -1;
int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex, bool isSorted){
    assert(startIndex >= 0 && startIndex <= v->log_len);
    assert(key != NULL);
    assert(searchFn != NULL);
    char* found_loc;
    size_t memb = v->log_len - startIndex;
    if (isSorted){
        found_loc = (char*)bsearch(key, 
                             (char*)v->data + startIndex * v->elem_size, 
                             memb, 
                             v->elem_size, 
                             searchFn);
    } else {
        found_loc = (char*)lfind(key, 
                          (char*)v->data + startIndex * v->elem_size, 
                          &memb, 
                          v->elem_size, 
                          searchFn);        
    }
    if (found_loc == NULL){
        return kNotFound;
    } else return ((char*)found_loc - (char*)v->data) / v->elem_size;
} 
