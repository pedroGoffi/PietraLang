#ifndef  __ALLOCATOR_CPP__
#define  __ALLOCATOR_CPP__

#include <cassert>
#include <iostream>
#include <vector>
using namespace std;

template<typename T>
class Allocator{
public:
  // TODO: if need, more allocator functions
  vector<T> vec;
  ~Allocator() = default;
};

#endif /*__ALLOCATOR_CPP__*/
