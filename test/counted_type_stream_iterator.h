/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#ifndef _SSTL_COUNTED_TYPE_STREAM_ITERATOR__
#define _SSTL_COUNTED_TYPE_STREAM_ITERATOR__

#include <cassert>
#include <iterator>
#include <initializer_list>
#include <memory>
#include "counted_type.h"

namespace sstl_test
{

class counted_type_stream_iterator
{
public:
   using iterator_category = std::input_iterator_tag;
   using value_type = counted_type;
   using difference_type = ptrdiff_t;
   using pointer = value_type*;
   using reference = value_type&;

public:
   counted_type_stream_iterator()
      : init{}
      , pos{ init.begin() }
   {}

   counted_type_stream_iterator(std::initializer_list<counted_type> init)
      : init( init )
      , pos{ init.begin() }
   {}

   const value_type& operator*() const _sstl_noexcept_
   {
      assert(pos != init.end());
      return *pos;
   }

   const value_type* operator->() const _sstl_noexcept_
   {
      assert(pos != init.end());
      return std::addressof(*pos);
   }

   counted_type_stream_iterator& operator++() _sstl_noexcept_
   {
      assert(pos != init.end());
      ++pos;
      return *this;
   }

   bool operator==(const counted_type_stream_iterator& rhs) const _sstl_noexcept_
   {
      return (pos==init.end()) && (rhs.pos==rhs.init.end());
   }

   bool operator!=(const counted_type_stream_iterator& rhs) const _sstl_noexcept_
   {
      return !operator==(rhs);
   }

private:
   std::initializer_list<counted_type> init;
   std::initializer_list<counted_type>::iterator pos;
};

}

#endif
