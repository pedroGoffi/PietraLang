#ifndef  __ALLOCATOR_CPP__
#define  __ALLOCATOR_CPP__

#include <cassert>
#include <iostream>
#include <memory>
#include <vector>
using namespace std;

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

typedef struct BufHdr{
  size_t len;
  size_t cap;
  char   buf[1]; // NOTE: if the allocator buggy, change the array size to zero

} BufHdr;

#define BUF_SIZE_MAX 1024*1024*1024
#define buf__hdr(b)       ((BufHdr *)((char *)b - offsetof(BufHdr, buf)))
#define buf__fits(b, x)   (buf__len(b) + (x) <= buf__cap(b))
#define buf__fit(b, x)    (buf__fits(b, x) ? 0 : *(void **)&(b) = buf__grow(b, buf__len(b) + (x), sizeof(*(b))))
#define buf__end(b)       ((b) + buf__len(b))
			  
#define buf__len(b)       ((b) ? buf__hdr(b)->len  : 0)
#define buf__cap(b)       ((b) ? buf__hdr(b)->cap  : 0)
#define buf__push(b, ...) (buf__fit((b), 1 + buf__len(b)), (b)[buf__hdr(b)->len++] = (__VA_ARGS__))

#define buf__free(b)      ((b) ? free(buf__hdr(b)) : (void)0), ((b) = NULL)



void *buf__grow(const void *buf, size_t new_len, size_t elem_size){
  size_t new_cap = MAX(1+2*buf__cap(buf), new_len);
  assert(buf__cap(buf) <= (BUF_SIZE_MAX - 1) / 2);
  assert(new_len <= new_cap);
  size_t new_size = offsetof(BufHdr, buf) + new_cap*elem_size;
  BufHdr *new_hdr;
  if(buf){
    new_hdr  = (BufHdr*)std::realloc(buf__hdr(buf), new_size);
  } else {
    new_hdr  = (BufHdr*)std::malloc(new_size);
    new_hdr->len = 0;
  }
  new_hdr->cap = new_cap;
  return new_hdr->buf;
}
template<typename T>
std::vector<T> vector_copy(std::vector<T> src){
  vector<T> ret;
  for(size_t i=0; i < src.size(); ++i){    
    ret.push_back(move(src[i]));
  }
  return ret;
}

#endif /*__ALLOCATOR_CPP__*/
