/* libgpp - a general-purpose, portable library
 *
 * Copyright (c) 2009 J.J.
 * All rights reserved.
 *
 * $Id$
 * $DateTime$
 */
 
#ifndef LIBGPP_EXCEPTION_HPP
#define LIBGPP_EXCEPTION_HPP

#include <exception>
#include <boost/system/error_code.hpp>

namespace libgpp {
namespace error {

enum basic_error
{
  invalid_condition = 1,
  invalid_pointer
};

namespace detail {

class basic_category : public boost::system::error_category 
{ 
public:
  const char *name() const 
  { 
    return "basic"; 
  } 

  std::string message(int error_code) const 
  { 
    return "basic error"; 
  }
}; 

} // detail

inline const boost::system::error_category& get_basic_category()
{
  static detail::basic_category instance;
  return instance;
}

static const boost::system::error_category& basic_category =
  get_basic_category();

inline boost::system::error_code make_error_code(basic_error error)
{
  return boost::system::error_code(
      static_cast<int>(error), get_basic_category());
}

class basic_exception : public std::exception
{
public:
   basic_exception(basic_error error)
  : error_(error)
  {
  }
  
  boost::system::error_code error_code() const
  {
    return make_error_code(error_);
  }

  const char *what() const throw()
  {
    return "basic_exception";
  }

private:
  const basic_error error_;
};

} // namespace error
} // namespace libgpp

template<typename T>
void assert_pointer(T pointer)
{
  if (pointer == NULL)
    throw libgpp::error::basic_exception(libgpp::error::invalid_pointer);
}

template<typename T>
void assert_condition(T condition)
{
  if (condition == 0)
    throw libgpp::error::basic_exception(libgpp::error::invalid_condition);
}

#endif /* LIBGPP_EXCEPTION_HPP */
