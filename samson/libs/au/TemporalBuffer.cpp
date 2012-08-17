#include <stdlib.h>
#include "TemporalBuffer.h" // Own interface

namespace au 
{
  TemporalBuffer::TemporalBuffer( size_t size )
  {
    data_ = (char*) malloc(size);
    size_ = size;
  }
  
  char* TemporalBuffer::data()
  {
    return data_;
  }
  
  size_t TemporalBuffer::size()
  {
    return size_;
  }
  
  
  TemporalBuffer::~TemporalBuffer()
  {
    if(data_)
      free (data_);
  }
}
