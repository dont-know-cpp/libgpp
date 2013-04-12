/* libgpp - a general-purpose, portable library
 *
 * Copyright (c) 2009 J.J.
 * All rights reserved.
 *
 * $Id$
 * $DateTime$
 */
 
#ifndef LIBGPP_BYTE_BUFFER_HPP
#define LIBGPP_BYTE_BUFFER_HPP

#include <cstdio>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

#include <boost/cstdint.hpp>
#include <boost/foreach.hpp>

#include <libgpp/exception.hpp>

namespace libgpp {

class byte_buffer
{
public:
  byte_buffer()
  {
  }
  
  byte_buffer(std::size_t size)
  {
    byte_buffer_.reserve(size);
  }

  byte_buffer(std::size_t size, char c)
  {
    fill(c, size);
  }
  
  byte_buffer(const boost::uint8_t *buffer, std::size_t size)
  {
    assert_condition(buffer != 0);
    push_back_data(buffer, size);
  }
  
  byte_buffer(const byte_buffer& buffer)
  {
    push_back_data(buffer);
  }
  
  byte_buffer(const char *buffer)
  {
    assert_condition(buffer != 0);
    push_back_string(buffer);
  }
  
  boost::uint8_t pop_back_uint8()
  {
    boost::uint8_t value = get_uint8(byte_buffer_.size() - 1);
    byte_buffer_.pop_back();
    return value;
  }
  
  boost::uint16_t pop_back_uint16()
  {
    boost::uint16_t value = get_uint16(byte_buffer_.size() - 2);
    byte_buffer_.erase(byte_buffer_.end() - 2, byte_buffer_.end());
    return value;
  }
  
  boost::uint32_t pop_back_uint32()
  {
    boost::uint32_t value = get_uint32(byte_buffer_.size() - 4);
    byte_buffer_.erase(byte_buffer_.end() - 4, byte_buffer_.end());
    return value;
  }
  
  boost::uint8_t pop_front_uint8()
  {
    boost::uint8_t value = get_uint8(0);
    byte_buffer_.erase(byte_buffer_.begin());
    return value;
  }
  
  boost::uint16_t pop_front_uint16()
  {
    boost::uint16_t value = get_uint16(0);
    byte_buffer_.erase(byte_buffer_.begin(), byte_buffer_.begin() + 2);
    return value;
  }

  boost::uint16_t pop_front_uint16_be()
  {
    boost::uint16_t value = pop_front_uint16();
    return ((value >> 8) | (value << 8));
  }
  
  boost::uint32_t pop_front_uint32()
  {
    boost::uint32_t value = get_uint32(0);
    byte_buffer_.erase(byte_buffer_.begin(), byte_buffer_.begin() + 4);
    return value;
  }

  boost::uint32_t pop_front_uint32_be()
  {
    boost::uint32_t value = pop_front_uint32();
    return ((value >> 24) | ((value << 8) & 0xFF0000) | 
      ((value >> 8) & 0xFF00) | (value << 24));
  }
  
  byte_buffer& push_back_uint8(boost::uint8_t value)
  {
    byte_buffer_.push_back(value);
    return *this;
  }
  
  byte_buffer& push_back_uint16(boost::uint16_t value)
  {
    byte_buffer_.push_back(static_cast<boost::uint8_t>(value));
    byte_buffer_.push_back(static_cast<boost::uint8_t>(value >> 8));
    return *this;
  }

  byte_buffer& push_back_uint16_be(boost::uint16_t value)
  {
    boost::uint16_t value_be =  ((value >> 8) | (value << 8));
    return push_back_uint16(value_be);
  }
  
  byte_buffer& push_back_uint32(boost::uint32_t value)
  {
    byte_buffer_.push_back(static_cast<boost::uint8_t>(value));
    byte_buffer_.push_back(static_cast<boost::uint8_t>(value >> 8));
    byte_buffer_.push_back(static_cast<boost::uint8_t>(value >> 16));
    byte_buffer_.push_back(static_cast<boost::uint8_t>(value >> 24));
    return *this;
  }

  byte_buffer& push_back_uint32_be(boost::uint32_t value)
  {
    boost::uint32_t value_be = 
      ((value >> 24) | ((value << 8) & 0xFF0000) | 
      ((value >> 8) & 0xFF00) | (value << 24));
    return push_back_uint32(value_be);
  }
  
  byte_buffer& push_front_uint8(boost::uint8_t value)
  {
    byte_buffer_.insert(byte_buffer_.begin(), value);
    return *this;
  }
  
  byte_buffer& push_front_uint16(boost::uint16_t value)
  {
    byte_buffer_.insert(byte_buffer_.begin(), 
        static_cast<boost::uint8_t>(value >> 8));
    byte_buffer_.insert(byte_buffer_.begin(), 
        static_cast<boost::uint8_t>(value));
    return *this;
  }

  byte_buffer& push_front_uint16_be(boost::uint16_t value)
  {
    boost::uint16_t value_be = ((value >> 8) | (value << 8));
    return push_front_uint16(value_be);
  }
  
  byte_buffer& push_front_uint32(boost::uint32_t value)
  {
    byte_buffer_.insert(byte_buffer_.begin(), 
        static_cast<boost::uint8_t>(value >> 24));
    byte_buffer_.insert(byte_buffer_.begin(), 
        static_cast<boost::uint8_t>(value >> 16));
    byte_buffer_.insert(byte_buffer_.begin(), 
        static_cast<boost::uint8_t>(value >> 8));
    byte_buffer_.insert(byte_buffer_.begin(), 
        static_cast<boost::uint8_t>(value));
    return *this;
  }
  
  byte_buffer& push_back_string(const std::string& buffer)
  {
    byte_buffer_.insert(byte_buffer_.end(), buffer.begin(), buffer.end());
    byte_buffer_.push_back(0);
    return *this;
  }
  
  byte_buffer& push_back_data(const std::string& buffer)
  {
    byte_buffer_.insert(byte_buffer_.end(), buffer.begin(), buffer.end());
    return *this;
  }

  byte_buffer& push_back_data(const boost::uint8_t *buffer, 
      boost::uint32_t size)
  {
    assert_pointer(buffer);
    byte_buffer_.insert(byte_buffer_.end(), buffer, buffer + size);
    return *this;
  }
  
  byte_buffer& push_back_data(const byte_buffer& buffer)
  {
    byte_buffer_.insert(byte_buffer_.end(), buffer.buffer(), buffer.buffer() + buffer.size());
    return *this;
  }
  
  byte_buffer& set_uint32(boost::uint32_t offset, boost::uint32_t value)
  {
    assert_condition((offset + 4) <= byte_buffer_.size());
    byte_buffer_[offset] = static_cast<boost::uint8_t>(value);
    byte_buffer_[offset + 1] = static_cast<boost::uint8_t>(value >> 8);
    byte_buffer_[offset + 2] = static_cast<boost::uint8_t>(value >> 16);
    byte_buffer_[offset + 3] = static_cast<boost::uint8_t>(value >> 24);
    return *this;
  }
  
  byte_buffer& set_uint16(boost::uint32_t offset, boost::uint16_t value)
  {
    assert_condition((offset + 2) <= byte_buffer_.size());
    byte_buffer_[offset] = static_cast<boost::uint8_t>(value);
    byte_buffer_[offset + 1] = static_cast<boost::uint8_t>(value >> 8);
    return *this;
  }
  
  byte_buffer& set_uint8(boost::uint32_t offset, boost::uint8_t value)
  {
    assert_condition((offset +1) <= byte_buffer_.size());
    byte_buffer_[offset] = value;
    return *this;
  }
  
  boost::uint32_t get_uint32(boost::uint32_t offset)
  {
    assert_condition((offset + 4) <= byte_buffer_.size());
    return (byte_buffer_[offset] | (byte_buffer_[offset + 1] << 8) | 
        (byte_buffer_[offset + 2] << 16) | (byte_buffer_[offset + 3] << 24));
  }
  
  boost::uint16_t get_uint16(boost::uint32_t offset)
  {
    assert_condition((offset + 2) <= byte_buffer_.size());
    return (byte_buffer_[offset] | (byte_buffer_[offset + 1] << 8));
  }
  
  boost::uint8_t get_uint8(boost::uint32_t offset)
  {
    assert_condition((offset + 1) <= byte_buffer_.size());
    return byte_buffer_[offset];
  }
  
  byte_buffer& fill(boost::uint8_t value, std::size_t size)
  {
    byte_buffer_.insert(byte_buffer_.end(), size, value);
    return *this;
  }

  byte_buffer pop_front_buffer(std::size_t size)
  {
    byte_buffer buffer = sub_buffer(0, size);
    byte_buffer_.erase(byte_buffer_.begin(), byte_buffer_.begin() + size);
    return buffer;
  }

  byte_buffer sub_buffer(boost::uint32_t offset) const
  {
    return sub_buffer(offset, (byte_buffer_.size() - offset));
  }
  
  byte_buffer sub_buffer(boost::uint32_t offset, std::size_t size) const
  {
    assert_condition((offset + size) <= byte_buffer_.size());
    return byte_buffer(&byte_buffer_[offset], size);
  }
  
  void clear()
  {
    byte_buffer_.clear();
  }
  
  std::size_t size() const
  {
    return byte_buffer_.size();
  }

  void resize(std::size_t size)
  {
    byte_buffer_.resize(size);
  }

  bool empty() const
  {
    return (size() == 0);
  }
  
  boost::uint8_t *buffer() const
  {
    if (byte_buffer_.size())
      return const_cast<boost::uint8_t *>(&byte_buffer_[0]);
    else
      return reinterpret_cast<boost::uint8_t *>(const_cast<char *>(""));
  }

  std::string to_string() const
  {
    return std::string(reinterpret_cast<char *>(buffer()), size());
  }

  void print_output() const
  {
    int p = 0;
    BOOST_FOREACH(boost::uint8_t i, byte_buffer_)
    {
      if ((p % 16) == 0 && p)
      {
        printf("\n");
      }
      printf("\\x%02x", i);
      p++;
    }
      
    std::cout << std::endl;
  }
  
  std::vector<boost::uint8_t>::iterator begin()
  {
    return byte_buffer_.begin();
  }
  
  std::vector<boost::uint8_t>::const_iterator begin() const
  {
    return byte_buffer_.begin();
  }
  
  std::vector<boost::uint8_t>::iterator end()
  {
    return byte_buffer_.end();
  }
  
  std::vector<boost::uint8_t>::const_iterator end() const
  {
    return byte_buffer_.end();
  }
  
  operator const boost::uint8_t* () const
  {
    return buffer();
  }
  
  operator const char* () const
  {
    return reinterpret_cast<const char *>(buffer());
  }
  
  std::vector<boost::uint8_t>& v()
  {
    return byte_buffer_;
  }
  
  boost::uint8_t& operator[] (int index)
  {
    assert_condition(static_cast<std::size_t>(index) < byte_buffer_.size());
    return byte_buffer_[index];
  }
  
  const boost::uint8_t& operator[] (int index) const
  {
    assert_condition(static_cast<std::size_t>(index) < byte_buffer_.size());
    return byte_buffer_[index];
  }

private:
  std::vector<boost::uint8_t> byte_buffer_;
};

} // namespace libgpp

template <int N> 
libgpp::byte_buffer data(const char(&pData)[N]) 
{ 
	return libgpp::byte_buffer((boost::uint8_t *)pData, N - 1); 
}

#endif /* LIBGPP_BYTE_BUFFER_HPP */
