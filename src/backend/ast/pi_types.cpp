#ifndef  __PI_TYPES_CPP__
#define  __PI_TYPES_CPP__

#include <string>
#include <memory>

using namespace std;
class pi_type{
  string   name_;
  size_t   size_;
  bool     const_;

  // TODO: pi_type_template


public:
  pi_type() : const_(false){}
  pi_type(string name, size_t size)
    : name_(name),
      size_(size),
      const_(false)
  {}

  size_t& size() {
    return this->size_;
  }
  string& name() {
    return this->name_;
  }  
  bool& cte(){    
    return this->const_;
  }
  void set_cte(bool state){
    this->const_ = state;
  }
  string human_name(){    
    string ret = this->const_? "const ": "";
    ret += this->name_;
    return ret;
  }
  
};
//auto ty_i64 = make_unique<pi_type>(pi_type("i64", sizeof(long int)));

#endif /*__PI_TYPES_CPP__*/
