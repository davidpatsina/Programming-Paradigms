#include "hashset.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "vector.h"

void HashSetNew(hashset *h, int elemSize, int numBuckets,
		HashSetHashFunction hashfn, HashSetCompareFunction comparefn, HashSetFreeFunction freefn){
			assert(elemSize != 0);
			assert(numBuckets > 0);
			assert(hashfn != NULL);
			assert(comparefn != NULL);
			
			h->elems_count = 0;
			h->size        = numBuckets;
			h->buckets     = malloc(sizeof(vector) * h->size);
			
			assert(h->buckets != NULL);

			h->hashFn    = hashfn;
			h->compareFn = comparefn;

			int i;

			for (i = 0; i < numBuckets; i++){
				VectorNew(&h->buckets[i], elemSize, freefn, 0);
			}
}

void HashSetDispose(hashset *h){
	int i;
	for (i = 0; i < h->size; i++){
		VectorDispose(&h->buckets[i]);
	}
	free(h->buckets);
}

int HashSetCount(const hashset *h){ 
	return h->elems_count;
}

void HashSetMap(hashset *h, HashSetMapFunction mapfn, void *auxData){
	assert(mapfn != NULL);
	assert(auxData != NULL);

	int i;
	for(i = 0; i < h->size; i++){
			if(VectorLength(&h->buckets[i]) != 0){
				VectorMap(&h->buckets[i], mapfn, auxData);
			}
	}
}

void HashSetEnter(hashset *h, const void *elemAddr){
	assert(elemAddr != NULL);
	int ind = h->hashFn(elemAddr, h->size);
	assert(ind >= 0);
	assert(ind < h->size);

	int res = VectorSearch(&h->buckets[ind], elemAddr, h->compareFn, 0, false);
	if (res != -1){
		VectorReplace(&h->buckets[ind], elemAddr, res);
	} else {
		h->elems_count++;
		VectorAppend(&h->buckets[ind], elemAddr);
	}

}

void *HashSetLookup(const hashset *h, const void *elemAddr){
	 assert(elemAddr != NULL);

	 int ind = h->hashFn(elemAddr, h->size);
	 assert(ind >= 0 && ind < h-> size);

	 if (VectorLength (&h->buckets[ind]) == 0) return NULL;
	 int i;
	 for (i = 0; i < VectorLength(&h->buckets[ind]); i++){
		int res = VectorSearch(&h->buckets[ind], elemAddr, h->compareFn, 0, false);
		if (res != -1) return VectorNth(&h->buckets[ind], res);
	 }
	 
	 return NULL;
}
