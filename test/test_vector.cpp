/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#include <catch.hpp>
#include <type_traits>
#include <sstl/__internal/_preprocessor.h>
#include <sstl/__internal/_except.h>
#include <sstl/vector.h>

#include "utility.h"
#include "counted_type.h"
#include "counted_type_stream_iterator.h"

namespace sstl_test
{
using vector_int_base_t = sstl::vector<int>;
using vector_int_t = sstl::vector<int, 11>;
using vector_counted_type_t = sstl::vector<counted_type, 11>;

TEST_CASE("vector - user cannot directly construct the base class")
{
   #if !_sstl_is_gcc()
      REQUIRE(!std::is_default_constructible<vector_int_base_t>::value);
   #endif
   REQUIRE(!std::is_copy_constructible<vector_int_base_t>::value);
   REQUIRE(!std::is_move_constructible<vector_int_base_t>::value);
}

TEST_CASE("vector - user cannot directly destroy the base class")
{
   #if !_is_msvc() //MSVC (VS2013) has a buggy implementation of std::is_destructible
   REQUIRE(!std::is_destructible<vector_int_base_t>::value);
   #endif
}

TEST_CASE("vector - default constructor")
{
   auto v = vector_int_t();
   REQUIRE(v.empty());
}

TEST_CASE("vector - count constructor")
{
   SECTION("default value")
   {
      auto expected = {0, 0, 0, 0, 0};
      auto v = vector_int_t(5);
      REQUIRE(are_containers_equal(v, expected));
   }
   SECTION("custom value")
   {
      auto expected = {3, 3, 3, 3, 3};
      auto v = vector_int_t(5, 3);
      REQUIRE(are_containers_equal(v, expected));
   }
   #if _sstl_has_exceptions()
   SECTION("exception handling")
   {
      counted_type::reset_counts();
      counted_type::throw_at_nth_copy_construction(3);
      REQUIRE_THROWS_AS(vector_counted_type_t(7), counted_type::copy_construction::exception);
      REQUIRE(counted_type::check().default_constructions(1).copy_constructions(2).destructions(3));
   }
   #endif
}

TEST_CASE("vector - copy constructor")
{
   SECTION("contained values")
   {
      SECTION("rhs' capacity is same")
      {
         auto expected = {1, 2, 3};
         auto rhs = vector_int_t{1, 2, 3};
         auto lhs = vector_int_t{ rhs };
         REQUIRE(are_containers_equal(lhs, expected));
         REQUIRE(are_containers_equal(rhs, expected));
      }
      SECTION("rhs' capacity is different")
      {
         auto expected = {1, 2, 3};
         auto rhs = sstl::vector<int, 30>{1, 2, 3};
         auto lhs = sstl::vector<int, 10>{ rhs };
         REQUIRE(are_containers_equal(lhs, expected));
         REQUIRE(are_containers_equal(rhs, expected));
      }
   }
   SECTION("number of copy constructions")
   {
      SECTION("rhs' capacity is the same")
      {
         auto rhs = vector_counted_type_t{1, 2, 3};
         counted_type::reset_counts();
         auto lhs = vector_counted_type_t{ rhs };
         REQUIRE(counted_type::check().copy_constructions(3));
      }
      SECTION("rhs' capacity is different")
      {
         auto rhs = sstl::vector<counted_type, 30>{1, 2, 3};
         counted_type::reset_counts();
         auto lhs = sstl::vector<counted_type, 10>{ rhs };
         REQUIRE(counted_type::check().copy_constructions(3));
      }
   }
   #if _sstl_has_exceptions()
   SECTION("exception handling")
   {
      auto rhs = vector_counted_type_t{1, 2, 3, 4, 5};
      counted_type::reset_counts();
      counted_type::throw_at_nth_copy_construction(3);
      REQUIRE_THROWS_AS(vector_counted_type_t{ rhs }, counted_type::copy_construction::exception);
      REQUIRE(counted_type::check().copy_constructions(2).destructions(2));
   }
   #endif
}

TEST_CASE("vector - move constructor")
{
   SECTION("contained values")
   {
      SECTION("rhs' capacity is same")
      {
         auto expected = {1, 2, 3};
         auto rhs = vector_int_t{1, 2, 3};
         auto lhs = vector_int_t{ std::move(rhs) };
         REQUIRE(are_containers_equal(lhs, expected));
         REQUIRE(rhs.empty());
      }
      SECTION("rhs' capacity is different")
      {
         auto expected = {1, 2, 3};
         auto rhs = sstl::vector<int, 30>{1, 2, 3};
         auto lhs = sstl::vector<int, 10>{ std::move(rhs) };
         REQUIRE(are_containers_equal(lhs, expected));
         REQUIRE(rhs.empty());
      }
   }
   SECTION("number of operations")
   {
      SECTION("rhs' capacity is same")
      {
         auto rhs = vector_counted_type_t{1, 2, 3};
         counted_type::reset_counts();
         auto lhs = vector_counted_type_t{ std::move(rhs) };
         REQUIRE(counted_type::check().move_constructions(3).destructions(3));
      }
      SECTION("rhs' capacity is different")
      {
         auto rhs = sstl::vector<counted_type, 30>{1, 2, 3};
         counted_type::reset_counts();
         auto lhs = sstl::vector<counted_type, 10>{ std::move(rhs) };
         REQUIRE(counted_type::check().move_constructions(3).destructions(3));
      }
   }
   #if _sstl_has_exceptions()
   SECTION("exception handling")
   {
      auto rhs = vector_counted_type_t{1, 2, 3, 4, 5};
      counted_type::reset_counts();
      counted_type::throw_at_nth_move_construction(3);
      REQUIRE_THROWS_AS(vector_counted_type_t{ std::move(rhs) }, counted_type::move_construction::exception);
      REQUIRE(counted_type::check().move_constructions(2).destructions(7));
   }
   #endif
}

TEST_CASE("vector - range constructor")
{
   SECTION("contained values")
   {
      auto range = {1, 2, 3};
      auto v = vector_int_t(range.begin(), range.end());
      REQUIRE(are_containers_equal(v, range));
   }
   #if _sstl_has_exceptions()
   SECTION("exception handling")
   {
      auto range = std::initializer_list<counted_type>{1, 2, 3, 4, 5};
      counted_type::reset_counts();
      counted_type::throw_at_nth_copy_construction(3);
      REQUIRE_THROWS_AS(vector_counted_type_t(range.begin(), range.end()), counted_type::copy_construction::exception);
      REQUIRE(counted_type::check().copy_constructions(2).destructions(2));
   }
   #endif
}

TEST_CASE("vector - initializer-list constructor")
{
   SECTION("contained values")
   {
      auto init = std::initializer_list<int>{1, 2, 3};
      auto v = vector_int_t(init);
      REQUIRE(are_containers_equal(v, init));
   }
   #if _sstl_has_exceptions()
   SECTION("exception handling")
   {
      auto init = std::initializer_list<counted_type>{1, 2, 3, 4, 5};
      counted_type::reset_counts();
      counted_type::throw_at_nth_copy_construction(3);
      REQUIRE_THROWS_AS(vector_counted_type_t{ init }, counted_type::copy_construction::exception);
      REQUIRE(counted_type::check().copy_constructions(2).destructions(2));
   }
   #endif
}

TEST_CASE("vector - destructor (contained values are destroyed)")
{
   {
      auto v = vector_counted_type_t(7);
      counted_type::reset_counts();
   }
   REQUIRE(counted_type::check().destructions(7));
}

TEST_CASE("vector - copy assignment operator")
{
   SECTION("contained values")
   {
      SECTION("rhs' capacity is same")
      {
         auto rhs = vector_int_t{1, 2, 3};
         auto lhs = vector_int_t{};
         SECTION("through base class reference")
         {
            static_cast<vector_int_base_t&>(lhs) = rhs;
            REQUIRE(are_containers_equal(lhs, rhs));
         }
         SECTION("through derived class reference")
         {
            static_cast<vector_int_t&>(lhs) = rhs;
            REQUIRE(are_containers_equal(lhs, rhs));
         }
      }
      SECTION("rhs' capacity is different")
      {
         auto rhs = sstl::vector<int, 30>{1, 2, 3};
         auto lhs = sstl::vector<int, 10>{};
         SECTION("through base class reference")
         {
            static_cast<vector_int_base_t&>(lhs) = rhs;
            REQUIRE(are_containers_equal(lhs, rhs));
         }
         SECTION("through derived class reference")
         {
            static_cast<sstl::vector<int, 10>&>(lhs) = rhs;
            REQUIRE(are_containers_equal(lhs, rhs));
         }
      }
   }
   SECTION("number of copy assignments + copy constructions")
   {
      SECTION("rhs' capacity is same")
      {
         auto rhs = vector_counted_type_t{1, 2, 3, 4, 5};
         auto lhs = vector_counted_type_t{1, 2, 3};
         counted_type::reset_counts();
         lhs = rhs;
         REQUIRE(counted_type::check().copy_assignments(3).copy_constructions(2));
      }
      SECTION("rhs' capacity is different")
      {
         auto rhs = sstl::vector<counted_type, 30>{1, 2, 3, 4, 5};
         auto lhs = sstl::vector<counted_type, 10>{1, 2, 3};
         counted_type::reset_counts();
         lhs = rhs;
         REQUIRE(counted_type::check().copy_assignments(3).copy_constructions(2));
      }
   }
   SECTION("number of destructions")
   {
      SECTION("rhs' capacity is same")
      {
         auto rhs = vector_counted_type_t{1, 2, 3};
         auto lhs = vector_counted_type_t{1, 2, 3, 4, 5};
         counted_type::reset_counts();
         lhs = rhs;
         REQUIRE(counted_type::check().destructions(2));
      }
      SECTION("rhs' capacity is different")
      {
         auto rhs = sstl::vector<counted_type, 30>{1, 2, 3};
         auto lhs = sstl::vector<counted_type, 10>{1, 2, 3, 4, 5};
         counted_type::reset_counts();
         lhs = rhs;
         REQUIRE(counted_type::check().destructions(2));
      }
   }
   #if _sstl_has_exceptions()
   SECTION("exception handling")
   {
      auto lhs = vector_counted_type_t{1, 2, 3, 4, 5};
      auto rhs = vector_counted_type_t{1, 2, 3};
      counted_type::reset_counts();
      counted_type::throw_at_nth_copy_assignment(3);
      REQUIRE_THROWS_AS(lhs = rhs, counted_type::copy_assignment::exception);
      REQUIRE(counted_type::check().copy_assignments(2).destructions(5));
      REQUIRE(lhs.empty());
   }
   #endif
}

TEST_CASE("vector - move assignment operator")
{
   SECTION("contained values")
   {
      SECTION("rhs' capacity is same")
      {
         auto expected = {1, 2, 3};
         auto rhs = vector_int_t{1, 2, 3};
         auto lhs = vector_int_t{};
         SECTION("through base class reference")
         {
            static_cast<vector_int_base_t&>(lhs) = std::move(rhs);
            REQUIRE(are_containers_equal(lhs, expected));
            REQUIRE(rhs.empty());
         }
         SECTION("through derived class reference")
         {
            static_cast<vector_int_t&>(lhs) = std::move(rhs);
            REQUIRE(are_containers_equal(lhs, expected));
            REQUIRE(rhs.empty());
         }
      }
      SECTION("rhs' capacity is different")
      {
         auto expected = {1, 2, 3};
         auto rhs = sstl::vector<int, 30>{1, 2, 3};
         auto lhs = sstl::vector<int, 10>{};
         SECTION("through base class reference")
         {
            static_cast<vector_int_base_t&>(lhs) = std::move(rhs);
            REQUIRE(are_containers_equal(lhs, expected));
            REQUIRE(rhs.empty());
         }
         SECTION("through derived class reference")
         {
            static_cast<sstl::vector<int, 10>&>(lhs) = std::move(rhs);
            REQUIRE(are_containers_equal(lhs, expected));
            REQUIRE(rhs.empty());
         }
      }
   }
   SECTION("number of move assignments + copy constructions")
   {
      SECTION("rhs' capacity is same")
      {
         auto rhs = vector_counted_type_t{1, 2, 3, 4, 5};
         auto lhs = vector_counted_type_t{1, 2, 3};
         counted_type::reset_counts();
         lhs = std::move(rhs);
         REQUIRE(counted_type::check().move_assignments(3).move_constructions(2));
      }
      SECTION("rhs' capacity is different")
      {
         auto rhs = sstl::vector<counted_type, 30>{1, 2, 3, 4, 5};
         auto lhs = sstl::vector<counted_type, 20>{1, 2, 3};
         counted_type::reset_counts();
         lhs = std::move(rhs);
         REQUIRE(counted_type::check().move_assignments(3).move_constructions(2));
      }
   }
   SECTION("number of destructions")
   {
      SECTION("rhs' capacity is same")
      {
         auto rhs = vector_counted_type_t{1, 2, 3};
         auto lhs = vector_counted_type_t{1, 2, 3, 4, 5};
         counted_type::reset_counts();
         lhs = std::move(rhs);
         REQUIRE(counted_type::check().destructions(5));
      }
      SECTION("rhs' capacity is different")
      {
         auto rhs = sstl::vector<counted_type, 30>{1, 2, 3};
         auto lhs = sstl::vector<counted_type, 10>{1, 2, 3, 4, 5};
         counted_type::reset_counts();
         lhs = std::move(rhs);
         REQUIRE(counted_type::check().destructions(5));
      }
   }
   #if _sstl_has_exceptions()
   SECTION("exception handling")
   {
      auto lhs = vector_counted_type_t{1, 2, 3, 4, 5};
      auto rhs = vector_counted_type_t{1, 2, 3};
      counted_type::reset_counts();
      counted_type::throw_at_nth_move_assignment(3);
      REQUIRE_THROWS_AS(lhs = std::move(rhs), counted_type::move_assignment::exception);
      REQUIRE(counted_type::check().move_assignments(2).destructions(8));
      REQUIRE(lhs.empty());
   }
   #endif
}

TEST_CASE("vector - initializer-list assignment operator")
{
   SECTION("contained values")
   {
      auto rhs = {1, 2, 3};
      auto lhs = vector_int_t{};
      SECTION("through base class reference")
      {
         static_cast<vector_int_base_t&>(lhs) = rhs;
         REQUIRE(are_containers_equal(lhs, rhs));
      }
      SECTION("through derived class reference")
      {
         static_cast<vector_int_t&>(lhs) = rhs;
         REQUIRE(are_containers_equal(lhs, rhs));
      }
   }
   SECTION("number of copy assignments")
   {
      auto rhs = std::initializer_list<counted_type>{1, 2, 3, 4, 5};
      auto lhs = vector_counted_type_t{1, 2, 3};
      counted_type::reset_counts();
      lhs = rhs;
      REQUIRE(counted_type::check().copy_assignments(3));
   }
   SECTION("number of copy constructions")
   {
      auto rhs = std::initializer_list<counted_type>{1, 2, 3, 4, 5};
      auto lhs = vector_counted_type_t{1, 2, 3};
      counted_type::reset_counts();
      lhs = rhs;
      REQUIRE(counted_type::check().copy_constructions(2));
   }
   SECTION("number of destructions")
   {
      auto rhs = std::initializer_list<counted_type>{1, 2, 3};
      auto lhs = vector_counted_type_t{1, 2, 3, 4, 5};
      counted_type::reset_counts();
      lhs = rhs;
      REQUIRE(counted_type::check().destructions(2));
   }
   #if _sstl_has_exceptions()
   SECTION("exception handling")
   {
      auto rhs = std::initializer_list<counted_type>{1, 2, 3};
      auto lhs = vector_counted_type_t{1, 2, 3, 4, 5};
      counted_type::reset_counts();
      counted_type::throw_at_nth_copy_assignment(3);
      REQUIRE_THROWS_AS(lhs = rhs, counted_type::copy_assignment::exception);
      REQUIRE(counted_type::check().copy_assignments(2).destructions(5));
      REQUIRE(lhs.empty());
   }
   #endif
}

TEST_CASE("vector - count assign")
{
   SECTION("contained values")
   {
      auto v = vector_int_t{1, 2, 3};
      v.assign(5, 7);
      auto expected = {7, 7, 7, 7, 7};
      REQUIRE(are_containers_equal(v, expected));
   }
   SECTION("number of copy assignments / copy constructions")
   {
      auto v = vector_counted_type_t{1, 2, 3};
      auto value = counted_type{ 7 };
      counted_type::reset_counts();
      v.assign(5, value);
      REQUIRE(counted_type::check().copy_assignments(3).copy_constructions(2));
   }
   SECTION("number of destructions")
   {
      auto v = vector_counted_type_t{1, 2, 3, 4, 5};
      auto value = counted_type{ 7 };
      counted_type::reset_counts();
      v.assign(3, value);
      REQUIRE(counted_type::check().destructions(2));
   }
   #if _sstl_has_exceptions()
   SECTION("exception handling")
   {
      auto v = vector_counted_type_t{1, 2, 3, 4, 5};
      auto value = counted_type{ 7 };
      counted_type::reset_counts();
      counted_type::throw_at_nth_copy_assignment(3);
      REQUIRE_THROWS_AS(v.assign(3, value), counted_type::copy_assignment::exception);
      REQUIRE(counted_type::check().copy_assignments(2).destructions(5));
      REQUIRE(v.empty());
   }
   #endif
}

TEST_CASE("vector - range assign")
{
   SECTION("contained values")
   {
      auto range = {7, 7, 7, 7, 7};
      auto v = vector_int_t{1, 2, 3};
      v.assign(range.begin(), range.end());
      REQUIRE(are_containers_equal(v, range));
   }
   SECTION("number of copy assignments / copy constructions")
   {
      auto range = std::initializer_list<counted_type>{7, 7, 7, 7, 7};
      auto v = vector_counted_type_t{1, 2, 3};
      counted_type::reset_counts();
      v.assign(range.begin(), range.end());
      REQUIRE(counted_type::check().copy_assignments(3).copy_constructions(2));
   }
   SECTION("number of destructions")
   {
      auto range = std::initializer_list<counted_type>{7, 7, 7};
      auto v = vector_counted_type_t{1, 2, 3, 4, 5};
      counted_type::reset_counts();
      v.assign(range.begin(), range.end());
      REQUIRE(counted_type::check().destructions(2));
   }
   #if _sstl_has_exceptions()
   SECTION("exception handling")
   {
      auto range = std::initializer_list<counted_type>{1, 2, 3};
      auto v = vector_counted_type_t{1, 2, 3, 4, 5};
      counted_type::reset_counts();
      counted_type::throw_at_nth_copy_assignment(3);
      REQUIRE_THROWS_AS(v.assign(range.begin(), range.end()), counted_type::copy_assignment::exception);
      REQUIRE(counted_type::check().copy_assignments(2).destructions(5));
      REQUIRE(v.empty());
   }
   #endif
}

TEST_CASE("vector - initializer list assign")
{
   SECTION("contained values")
   {
      auto ilist = {7, 7, 7, 7, 7};
      auto v = vector_int_t{1, 2, 3};
      v.assign(ilist);
      REQUIRE(are_containers_equal(v, ilist));
   }
   SECTION("number of copy assignments / copy constructions")
   {
      auto ilist = std::initializer_list<counted_type>{7, 7, 7, 7, 7};
      auto v = vector_counted_type_t{1, 2, 3};
      counted_type::reset_counts();
      v.assign(ilist);
      REQUIRE(counted_type::check().copy_assignments(3).copy_constructions(2));
   }
   SECTION("number of destructions")
   {
      auto ilist = std::initializer_list<counted_type>{7, 7, 7};
      auto v = vector_counted_type_t{1, 2, 3, 4, 5};
      counted_type::reset_counts();
      v.assign(ilist);
      REQUIRE(counted_type::check().destructions(2));
   }
   #if _sstl_has_exceptions()
   SECTION("exception handling")
   {
      auto ilist = std::initializer_list<counted_type>{1, 2, 3};
      auto v = vector_counted_type_t{1, 2, 3, 4, 5};
      counted_type::reset_counts();
      counted_type::throw_at_nth_copy_assignment(3);
      REQUIRE_THROWS_AS(v.assign(ilist), counted_type::copy_assignment::exception);
      REQUIRE(counted_type::check().copy_assignments(2).destructions(5));
      REQUIRE(v.empty());
   }
   #endif
}

TEST_CASE("vector - at")
{
   auto v = vector_int_t{1, 2, 3, 4, 5};
   SECTION("read access")
   {
      const auto& cv = v;
      REQUIRE(cv.at(0) == 1);
      REQUIRE(cv.at(2) == 3);
      REQUIRE(cv.at(4) == 5);
   }
   SECTION("write access")
   {
      v.at(0) = 10;
      v.at(2) = 30;
      v.at(4) = 50;
      REQUIRE(v.at(0) == 10);
      REQUIRE(v.at(2) == 30);
      REQUIRE(v.at(4) == 50);
   }
   #if _sstl_has_exceptions()
   SECTION("exception handling")
   {
      REQUIRE_THROWS_AS(v.at(5), std::out_of_range);
   }
   #endif
}

TEST_CASE("vector - operator[]")
{
   auto v = vector_int_t{1, 2, 3, 4, 5};
   SECTION("read access")
   {
      const auto& cv = v;
      REQUIRE(cv[0] == 1);
      REQUIRE(cv[2] == 3);
      REQUIRE(cv[4] == 5);
   }
   SECTION("write access")
   {
      v[0] = 10;
      v[2] = 30;
      v[4] = 50;
      REQUIRE(v[0] == 10);
      REQUIRE(v[2] == 30);
      REQUIRE(v[4] == 50);
   }
}

TEST_CASE("vector - front")
{
   auto v = vector_int_t{1, 2, 3};
   SECTION("read access")
   {
      const auto& cv = v;
      REQUIRE(cv.front() == 1);
   }
   SECTION("write access")
   {
      v.front() = 10;
      REQUIRE(v.front() == 10);
   }
}

TEST_CASE("vector - data")
{
   auto v = vector_int_t{1, 2, 3};
   SECTION("read access")
   {
      const auto& cv = v;
      REQUIRE(*cv.data() == 1);
   }
   SECTION("write access")
   {
      *v.data() = 10;
      REQUIRE(v[0] == 10);
   }
}

TEST_CASE("vector - iterators")
{
   SECTION("zero elements")
   {
      auto v = vector_int_t{};
      const auto& cv = v;

      REQUIRE(v.begin() == v.end());
      REQUIRE(cv.cbegin() == cv.cend());
      REQUIRE(v.rbegin() == v.rend());
      REQUIRE(cv.crbegin() == cv.crend());
   }
   SECTION("one elements")
   {
      auto v = vector_int_t{1};
      const auto& cv = v;

      REQUIRE(std::distance(v.begin(), v.end()) == 1);
      REQUIRE(std::distance(cv.cbegin(), cv.cend()) == 1);
      REQUIRE(std::distance(v.rbegin(), v.rend()) == 1);
      REQUIRE(std::distance(cv.crbegin(), cv.crend()) == 1);

      REQUIRE(*v.begin() == 1);
      REQUIRE(*cv.cbegin() == 1);
      REQUIRE(*v.rbegin() == 1);
      REQUIRE(*cv.crbegin() == 1);
   }
   SECTION("many elements")
   {
      auto l = std::initializer_list<int>{1, 2, 3, 4, 5};
      auto lbegin = l.begin();
      auto lend = l.end();
      auto lrbegin = std::reverse_iterator<decltype(lend)>{ lend };
      auto lrend = std::reverse_iterator<decltype(lbegin)>{ lbegin };

      auto v = vector_int_t(l);
      const auto& cv = v;

      REQUIRE(std::distance(v.begin(), v.end()) == 5);
      REQUIRE(std::distance(cv.cbegin(), cv.cend()) == 5);
      REQUIRE(std::distance(v.rbegin(), v.rend()) == 5);
      REQUIRE(std::distance(cv.crbegin(), cv.crend()) == 5);

      REQUIRE(std::equal(lbegin, lend, v.begin()));
      REQUIRE(std::equal(lbegin, lend, cv.cbegin()));
      REQUIRE(std::equal(lrbegin, lrend, v.rbegin()));
      REQUIRE(std::equal(lrbegin, lrend, cv.crbegin()));
   }
}

TEST_CASE("vector - empty / size")
{
   auto v = vector_int_t{};
   SECTION("zero elements")
   {
      REQUIRE(v.empty());
      REQUIRE(v.size() == 0);
   }
   SECTION("one elements")
   {
      v.push_back(0);
      REQUIRE(!v.empty());
      REQUIRE(v.size() == 1);

      SECTION("many elements")
      {
         v.push_back(0); v.push_back(0);
         REQUIRE(!v.empty());
         REQUIRE(v.size() == 3);

         SECTION("zero elements (again)")
         {
            v.pop_back(); v.pop_back(); v.pop_back();
            REQUIRE(v.empty());
            REQUIRE(v.size() == 0);
         }
      }
   }
}

TEST_CASE("vector - max size / capacity")
{
   SECTION("capacity is 1")
   {
      auto v = sstl::vector<int, 1>{};
      REQUIRE(v.max_size() == 1);
      REQUIRE(v.capacity() == 1);
   }
   SECTION("capacity is 11")
   {
      auto v = sstl::vector<int, 11>{};
      REQUIRE(v.max_size() == 11);
      REQUIRE(v.capacity() == 11);
   }
}

TEST_CASE("vector - clear")
{
   SECTION("contained values")
   {
      auto v = vector_int_t{1, 2, 3, 4, 5, 6, 7};
      v.clear();
      REQUIRE(v.empty());
      v.assign(7, 0);
      REQUIRE(!v.empty());
      v.clear();
      REQUIRE(v.empty());
   }
   SECTION("number of destructions")
   {
      auto v = vector_counted_type_t(7);
      counted_type::reset_counts();
      v.clear();
      REQUIRE(counted_type::check().destructions(7));
      v.assign(7, 0);
      counted_type::reset_counts();
      v.clear();
      REQUIRE(counted_type::check().destructions(7));
   }
}

TEST_CASE("vector - by-lvalue-reference insert")
{
   SECTION("begin")
   {
      auto expected = std::initializer_list<counted_type>{7, 3, 3, 3, 3, 3};
      auto v = vector_counted_type_t{3, 3, 3, 3, 3};
      auto value = counted_type{ 7 };

      counted_type::reset_counts();
      auto pos = v.insert(v.begin(), value);

      REQUIRE(pos == v.begin());
      REQUIRE(are_containers_equal(v, expected));
      REQUIRE(counted_type::check().move_constructions(1).move_assignments(4).copy_assignments(1));
   }
   SECTION("end")
   {
      auto expected = std::initializer_list<counted_type>{3, 3, 3, 3, 3, 7};
      auto v = vector_counted_type_t{3, 3, 3, 3, 3};
      auto value = counted_type{ 7 };

      counted_type::reset_counts();
      auto pos = v.insert(v.end(), value);

      REQUIRE(pos == v.end()-1);
      REQUIRE(are_containers_equal(v, expected));
      REQUIRE(counted_type::check().copy_constructions(1));
   }
   SECTION("middle")
   {
      auto expected = std::initializer_list<counted_type>{3, 3, 7, 3, 3, 3};
      auto value = counted_type{ 7 };
      auto v = vector_counted_type_t{3, 3, 3, 3, 3};

      counted_type::reset_counts();
      auto pos = v.insert(v.begin() + 2, value);

      REQUIRE(pos == v.begin()+2);
      REQUIRE(are_containers_equal(v, expected));
      REQUIRE(counted_type::check().move_constructions(1).move_assignments(2).copy_assignments(1));
   }
   #if _sstl_has_exceptions()
   SECTION("exception handling")
   {
      auto value = counted_type{ 7 };
      auto v = vector_counted_type_t{1, 2, 3, 4, 5};
      SECTION("end (strong exception safety)")
      {
         auto expected = std::initializer_list<counted_type>{1, 2, 3, 4, 5};
         counted_type::reset_counts();
         counted_type::throw_at_nth_copy_construction(1);
         REQUIRE_THROWS_AS(v.insert(v.end(), value), counted_type::copy_construction::exception);
         REQUIRE(counted_type::check().constructions(0).destructions(0));
         REQUIRE(are_containers_equal(v, expected));
      }
      SECTION("middle (basic exception safety)")
      {
         counted_type::reset_counts();
         counted_type::throw_at_nth_copy_assignment(1);
         REQUIRE_THROWS_AS(v.insert(v.begin()+2, value), counted_type::copy_assignment::exception);
         REQUIRE(counted_type::check().move_constructions(1).move_assignments(2).destructions(6));
         REQUIRE(v.empty());
      }
   }
   #endif
}

TEST_CASE("vector - by-rvalue-reference insert")
{
   SECTION("begin")
   {
      auto expected = std::initializer_list<counted_type>{7, 3, 3, 3, 3, 3};
      auto v = vector_counted_type_t{3, 3, 3, 3, 3};
      auto value = counted_type{ 7 };

      counted_type::reset_counts();
      auto pos = v.insert(v.begin(), std::move(value));

      REQUIRE(pos == v.begin());
      REQUIRE(are_containers_equal(v, expected));
      REQUIRE(counted_type::check().move_constructions(1).move_assignments(5));
   }
   SECTION("end")
   {
      auto expected = std::initializer_list<counted_type>{3, 3, 3, 3, 3, 7};
      auto v = vector_counted_type_t{3, 3, 3, 3, 3};
      auto value = counted_type{ 7 };

      counted_type::reset_counts();
      auto pos = v.insert(v.end(), std::move(value));

      REQUIRE(pos == v.end()-1);
      REQUIRE(are_containers_equal(v, expected));
      REQUIRE(counted_type::check().move_constructions(1));
   }
   SECTION("middle")
   {
      auto expected = std::initializer_list<counted_type>{3, 3, 7, 3, 3, 3};
      auto value = counted_type{ 7 };
      auto v = vector_counted_type_t{3, 3, 3, 3, 3};

      counted_type::reset_counts();
      auto pos = v.insert(v.begin() + 2, std::move(value));

      REQUIRE(pos == v.begin()+2);
      REQUIRE(are_containers_equal(v, expected));
      REQUIRE(counted_type::check().move_constructions(1).move_assignments(3));
   }
   #if _sstl_has_exceptions()
   SECTION("exception handling")
   {
      auto value = counted_type{ 7 };
      auto v = vector_counted_type_t{1, 2, 3, 4, 5};
      SECTION("end (strong exception safety)")
      {
         auto expected = std::initializer_list<counted_type>{1, 2, 3, 4, 5};
         counted_type::reset_counts();
         counted_type::throw_at_nth_move_construction(1);
         REQUIRE_THROWS_AS(v.insert(v.end(), std::move(value)), counted_type::move_construction::exception);
         REQUIRE(counted_type::check().constructions(0).destructions(0));
         REQUIRE(are_containers_equal(v, expected));
      }
      SECTION("middle (basic exception safety)")
      {
         counted_type::reset_counts();
         counted_type::throw_at_nth_move_assignment(3);
         REQUIRE_THROWS_AS(v.insert(v.begin()+2, std::move(value)), counted_type::move_assignment::exception);
         REQUIRE(counted_type::check().move_constructions(1).move_assignments(2).destructions(6));
         REQUIRE(v.empty());
      }
   }
   #endif
}

TEST_CASE("vector - insert (count)")
{
   auto v = vector_counted_type_t{3, 3, 3, 3, 3};
   auto value = counted_type{ 7 };

   SECTION("begin")
   {
      SECTION("count=0")
      {
         auto expected = std::initializer_list<counted_type>{3, 3, 3, 3, 3};
         counted_type::reset_counts();
         auto pos = v.insert(v.begin(), 0, value);
         REQUIRE(pos == v.begin());
         REQUIRE(are_containers_equal(v, expected));
         REQUIRE(counted_type::check().constructions(0));
      }
      SECTION("count=1")
      {
         auto expected = std::initializer_list<counted_type>{7, 3, 3, 3, 3, 3};
         counted_type::reset_counts();
         auto pos = v.insert(v.begin(), 1, value);
         REQUIRE(pos == v.begin());
         REQUIRE(are_containers_equal(v, expected));
         REQUIRE(counted_type::check().move_constructions(1).move_assignments(4).copy_assignments(1));
      }
      SECTION("count=2")
      {
         auto expected = std::initializer_list<counted_type>{7, 7, 3, 3, 3, 3, 3};
         counted_type::reset_counts();
         auto pos = v.insert(v.begin(), 2, value);
         REQUIRE(pos == v.begin());
         REQUIRE(are_containers_equal(v, expected));
         REQUIRE(counted_type::check().move_constructions(2).move_assignments(3).copy_assignments(2));
      }
      SECTION("count=4")
      {
         auto expected = std::initializer_list<counted_type>{7, 7, 7, 7, 3, 3, 3, 3, 3};
         counted_type::reset_counts();
         auto pos = v.insert(v.begin(), 4, value);
         REQUIRE(pos == v.begin());
         REQUIRE(are_containers_equal(v, expected));
         REQUIRE(counted_type::check().move_constructions(4).move_assignments(1).copy_assignments(4));
      }
   }
   SECTION("end")
   {
      SECTION("count=0")
      {
         auto expected = std::initializer_list<counted_type>{3, 3, 3, 3, 3};
         counted_type::reset_counts();
         auto pos = v.insert(v.end(), 0, value);
         REQUIRE(pos == v.begin()+5);
         REQUIRE(are_containers_equal(v, expected));
         REQUIRE(counted_type::check().constructions(0));
      }
      SECTION("count=1")
      {
         auto expected = std::initializer_list<counted_type>{3, 3, 3, 3, 3, 7};
         counted_type::reset_counts();
         auto pos = v.insert(v.end(), 1, value);
         REQUIRE(pos == v.begin()+5);
         REQUIRE(are_containers_equal(v, expected));
         REQUIRE(counted_type::check().copy_constructions(1));
      }
      SECTION("count=2")
      {
         auto expected = std::initializer_list<counted_type>{3, 3, 3, 3, 3, 7, 7};
         counted_type::reset_counts();
         auto pos = v.insert(v.end(), 2, value);
         REQUIRE(pos == v.begin()+5);
         REQUIRE(are_containers_equal(v, expected));
         REQUIRE(counted_type::check().copy_constructions(2));
      }
      SECTION("count=4")
      {
         auto expected = std::initializer_list<counted_type>{3, 3, 3, 3, 3, 7, 7, 7, 7};
         counted_type::reset_counts();
         auto pos = v.insert(v.end(), 4, value);
         REQUIRE(pos == v.begin()+5);
         REQUIRE(are_containers_equal(v, expected));
         REQUIRE(counted_type::check().copy_constructions(4));
      }
   }
   SECTION("middle")
   {
      SECTION("count=0")
      {
         auto expected = std::initializer_list<counted_type>{3, 3, 3, 3, 3};
         counted_type::reset_counts();
         auto pos = v.insert(v.begin()+2, 0, value);
         REQUIRE(pos == v.begin()+2);
         REQUIRE(are_containers_equal(v, expected));
         REQUIRE(counted_type::check().constructions(0));
      }
      SECTION("count=1")
      {
         auto expected = std::initializer_list<counted_type>{3, 3, 7, 3, 3, 3};
         counted_type::reset_counts();
         auto pos = v.insert(v.begin()+2, 1, value);
         REQUIRE(pos == v.begin()+2);
         REQUIRE(are_containers_equal(v, expected));
         REQUIRE(counted_type::check().move_constructions(1).move_assignments(2).copy_assignments(1));
      }
      SECTION("count=2")
      {
         auto expected = std::initializer_list<counted_type>{3, 3, 7, 7, 3, 3, 3};
         counted_type::reset_counts();
         auto pos = v.insert(v.begin()+2, 2, value);
         REQUIRE(pos == v.begin()+2);
         REQUIRE(are_containers_equal(v, expected));
         REQUIRE(counted_type::check().move_constructions(2).move_assignments(1).copy_assignments(2));
      }
      SECTION("count=3")
      {
         auto expected = std::initializer_list<counted_type>{3, 3, 7, 7, 7, 3, 3, 3};
         counted_type::reset_counts();
         auto pos = v.insert(v.begin()+2, 3, value);
         REQUIRE(pos == v.begin()+2);
         REQUIRE(are_containers_equal(v, expected));
         REQUIRE(counted_type::check().move_constructions(3).copy_assignments(3));
      }
      SECTION("count=4")
      {
         auto expected = std::initializer_list<counted_type>{3, 3, 7, 7, 7, 7, 3, 3, 3};
         counted_type::reset_counts();
         auto pos = v.insert(v.begin()+2, 4, value);
         REQUIRE(pos == v.begin()+2);
         REQUIRE(are_containers_equal(v, expected));
         REQUIRE(counted_type::check().move_constructions(3).copy_constructions(1).copy_assignments(3));
      }
      SECTION("count=5")
      {
         auto expected = std::initializer_list<counted_type>{3, 3, 7, 7, 7, 7, 7, 3, 3, 3};
         counted_type::reset_counts();
         auto pos = v.insert(v.begin()+2, 5, value);
         REQUIRE(pos == v.begin()+2);
         REQUIRE(are_containers_equal(v, expected));
         REQUIRE(counted_type::check().move_constructions(3).copy_constructions(2).copy_assignments(3));
      }
   }
   #if _sstl_has_exceptions()
   SECTION("exception handling")
   {
      auto value = counted_type{ 7 };
      auto v = vector_counted_type_t{1, 2, 3, 4, 5};
      SECTION("end (strong exception safety)")
      {
         auto expected = std::initializer_list<counted_type>{1, 2, 3, 4, 5};
         counted_type::reset_counts();
         counted_type::throw_at_nth_copy_construction(3);
         REQUIRE_THROWS_AS(v.insert(v.end(), 3, value), counted_type::copy_construction::exception);
         REQUIRE(counted_type::check().copy_constructions(2).destructions(2));
         REQUIRE(are_containers_equal(v, expected));
      }
      SECTION("middle (basic exception safety)")
      {
         counted_type::reset_counts();
         counted_type::throw_at_nth_copy_assignment(3);
         REQUIRE_THROWS_AS(v.insert(v.begin()+2, 3, value), counted_type::copy_assignment::exception);
         REQUIRE(counted_type::check().move_constructions(3).copy_assignments(2).destructions(8));
         REQUIRE(v.empty());
      }
   }
   #endif
}

TEST_CASE("vector - insert (input iterators + forward iterators + initializer-list)")
{
   auto v = vector_counted_type_t{3, 3, 3, 3, 3};

   SECTION("begin")
   {
      SECTION("count=0")
      {
         auto expected = std::initializer_list<counted_type>{3, 3, 3, 3, 3};
         auto values = std::initializer_list<counted_type>{};
         counted_type::reset_counts();
         SECTION("range (input iterator)")
         {
            auto pos = v.insert(v.begin(), counted_type_stream_iterator{values}, counted_type_stream_iterator{});
            REQUIRE(pos == v.begin());
            REQUIRE(are_containers_equal(v, expected));
         }
         SECTION("range (forward iterator)")
         {
            auto pos = v.insert(v.begin(), values.begin(), values.end());
            REQUIRE(pos == v.begin());
            REQUIRE(are_containers_equal(v, expected));
            REQUIRE(counted_type::check().constructions(0));
         }
         SECTION("initializer list")
         {
            auto pos = v.insert(v.begin(), values);
            REQUIRE(pos == v.begin());
            REQUIRE(are_containers_equal(v, expected));
            REQUIRE(counted_type::check().constructions(0));
         }
      }
      SECTION("count=1")
      {
         auto expected = std::initializer_list<counted_type>{7, 3, 3, 3, 3, 3};
         auto values = std::initializer_list<counted_type>{7};
         counted_type::reset_counts();
         SECTION("range (input iterator)")
         {
            auto pos = v.insert(v.begin(), counted_type_stream_iterator{values}, counted_type_stream_iterator{});
            REQUIRE(pos == v.begin());
            REQUIRE(are_containers_equal(v, expected));
         }
         SECTION("range (forward iterator)")
         {
            auto pos = v.insert(v.begin(), values.begin(), values.end());
            REQUIRE(pos == v.begin());
            REQUIRE(are_containers_equal(v, expected));
            REQUIRE(counted_type::check().move_constructions(1).move_assignments(4).copy_assignments(1));
         }
         SECTION("initializer list")
         {
            auto pos = v.insert(v.begin(), values);
            REQUIRE(pos == v.begin());
            REQUIRE(are_containers_equal(v, expected));
            REQUIRE(counted_type::check().move_constructions(1).move_assignments(4).copy_assignments(1));
         }
      }
      SECTION("count=2")
      {
         auto expected = std::initializer_list<counted_type>{7, 11, 3, 3, 3, 3, 3};
         auto values = std::initializer_list<counted_type>{7, 11};
         counted_type::reset_counts();
         SECTION("range (input iterator)")
         {
            auto pos = v.insert(v.begin(), counted_type_stream_iterator{values}, counted_type_stream_iterator{});
            REQUIRE(pos == v.begin());
            REQUIRE(are_containers_equal(v, expected));
         }
         SECTION("range (forward iterator)")
         {
            auto pos = v.insert(v.begin(), values.begin(), values.end());
            REQUIRE(pos == v.begin());
            REQUIRE(are_containers_equal(v, expected));
            REQUIRE(counted_type::check().move_constructions(2).move_assignments(3).copy_assignments(2));
         }
         SECTION("initializer list")
         {
            auto pos = v.insert(v.begin(), values);
            REQUIRE(pos == v.begin());
            REQUIRE(are_containers_equal(v, expected));
            REQUIRE(counted_type::check().move_constructions(2).move_assignments(3).copy_assignments(2));
         }
      }
      SECTION("count=4")
      {
         auto expected = std::initializer_list<counted_type>{7, 11, 13, 17, 3, 3, 3, 3, 3};
         auto values = std::initializer_list<counted_type>{7, 11, 13, 17};
         counted_type::reset_counts();
         SECTION("range (input iterator)")
         {
            auto pos = v.insert(v.begin(), counted_type_stream_iterator{values}, counted_type_stream_iterator{});
            REQUIRE(pos == v.begin());
            REQUIRE(are_containers_equal(v, expected));
         }
         SECTION("range (forward iterator)")
         {
            auto pos = v.insert(v.begin(), values.begin(), values.end());
            REQUIRE(pos == v.begin());
            REQUIRE(are_containers_equal(v, expected));
            REQUIRE(counted_type::check().move_constructions(4).move_assignments(1).copy_assignments(4));
         }
         SECTION("initializer list")
         {
            auto pos = v.insert(v.begin(), values);
            REQUIRE(pos == v.begin());
            REQUIRE(are_containers_equal(v, expected));
            REQUIRE(counted_type::check().move_constructions(4).move_assignments(1).copy_assignments(4));
         }
      }
   }
   SECTION("end")
   {
      SECTION("count=0")
      {
         auto expected = std::initializer_list<counted_type>{3, 3, 3, 3, 3};
         auto values = std::initializer_list<counted_type>{};
         counted_type::reset_counts();
         SECTION("range (input iterator)")
         {
            auto pos = v.insert(v.begin(), counted_type_stream_iterator{values}, counted_type_stream_iterator{});
            REQUIRE(pos == v.begin());
            REQUIRE(are_containers_equal(v, expected));
         }
         SECTION("range (forward iterator)")
         {
            auto pos = v.insert(v.end(), values.begin(), values.end());
            REQUIRE(pos == v.begin()+5);
            REQUIRE(are_containers_equal(v, expected));
            REQUIRE(counted_type::check().constructions(0));
         }
         SECTION("initializer list")
         {
            auto pos = v.insert(v.end(), values);
            REQUIRE(pos == v.begin()+5);
            REQUIRE(are_containers_equal(v, expected));
            REQUIRE(counted_type::check().constructions(0));
         }
      }
      SECTION("count=1")
      {
         auto expected = std::initializer_list<counted_type>{3, 3, 3, 3, 3, 7};
         auto values = std::initializer_list<counted_type>{7};
         counted_type::reset_counts();
         SECTION("range (input iterator)")
         {
            auto pos = v.insert(v.end(), counted_type_stream_iterator{values}, counted_type_stream_iterator{});
            REQUIRE(pos == v.begin()+5);
            REQUIRE(are_containers_equal(v, expected));
         }
         SECTION("range (forward iterator)")
         {
            auto pos = v.insert(v.end(), values.begin(), values.end());
            REQUIRE(pos == v.begin()+5);
            REQUIRE(are_containers_equal(v, expected));
            REQUIRE(counted_type::check().copy_constructions(1));
         }
         SECTION("initializer list")
         {
            auto pos = v.insert(v.end(), values);
            REQUIRE(pos == v.begin()+5);
            REQUIRE(are_containers_equal(v, expected));
            REQUIRE(counted_type::check().copy_constructions(1));
         }
      }
      SECTION("count=2")
      {
         auto expected = std::initializer_list<counted_type>{3, 3, 3, 3, 3, 7, 11};
         auto values = std::initializer_list<counted_type>{7, 11};
         counted_type::reset_counts();
         SECTION("range (input iterator)")
         {
            auto pos = v.insert(v.end(), counted_type_stream_iterator{values}, counted_type_stream_iterator{});
            REQUIRE(pos == v.begin()+5);
            REQUIRE(are_containers_equal(v, expected));
         }
         SECTION("range (forward iterator)")
         {
            auto pos = v.insert(v.end(), values.begin(), values.end());
            REQUIRE(pos == v.begin()+5);
            REQUIRE(are_containers_equal(v, expected));
            REQUIRE(counted_type::check().copy_constructions(2));
         }
         SECTION("initializer list")
         {
            auto pos = v.insert(v.end(), values);
            REQUIRE(pos == v.begin()+5);
            REQUIRE(are_containers_equal(v, expected));
            REQUIRE(counted_type::check().copy_constructions(2));
         }
      }
      SECTION("count=4")
      {
         auto expected = std::initializer_list<counted_type>{3, 3, 3, 3, 3, 7, 11, 13, 17};
         auto values = std::initializer_list<counted_type>{7, 11, 13, 17};
         counted_type::reset_counts();
         SECTION("range (input iterator)")
         {
            auto pos = v.insert(v.end(), counted_type_stream_iterator{values}, counted_type_stream_iterator{});
            REQUIRE(pos == v.begin()+5);
            REQUIRE(are_containers_equal(v, expected));
         }
         SECTION("range (forward iterator)")
         {
            auto pos = v.insert(v.end(), values.begin(), values.end());
            REQUIRE(pos == v.begin()+5);
            REQUIRE(are_containers_equal(v, expected));
            REQUIRE(counted_type::check().copy_constructions(4));
         }
         SECTION("initializer list")
         {
            auto pos = v.insert(v.end(), values);
            REQUIRE(pos == v.begin()+5);
            REQUIRE(are_containers_equal(v, expected));
            REQUIRE(counted_type::check().copy_constructions(4));
         }
      }
   }
   SECTION("middle")
   {
      SECTION("count=0")
      {
         auto expected = std::initializer_list<counted_type>{3, 3, 3, 3, 3};
         auto values = std::initializer_list<counted_type>{};
         counted_type::reset_counts();
         SECTION("range (input iterator)")
         {
            auto pos = v.insert(v.begin()+2, counted_type_stream_iterator{values}, counted_type_stream_iterator{});
            REQUIRE(pos == v.begin()+2);
            REQUIRE(are_containers_equal(v, expected));
         }
         SECTION("range (forward iterator)")
         {
            auto pos = v.insert(v.begin()+2, values.begin(), values.end());
            REQUIRE(pos == v.begin()+2);
            REQUIRE(are_containers_equal(v, expected));
            REQUIRE(counted_type::check().constructions(0));
         }
         SECTION("initializer list")
         {
            auto pos = v.insert(v.begin()+2, values);
            REQUIRE(pos == v.begin()+2);
            REQUIRE(are_containers_equal(v, expected));
            REQUIRE(counted_type::check().constructions(0));
         }
      }
      SECTION("count=1")
      {
         auto expected = std::initializer_list<counted_type>{3, 3, 7, 3, 3, 3};
         auto values = std::initializer_list<counted_type>{7};
         counted_type::reset_counts();
         SECTION("range (input iterator)")
         {
            auto pos = v.insert(v.begin()+2, counted_type_stream_iterator{values}, counted_type_stream_iterator{});
            REQUIRE(pos == v.begin()+2);
            REQUIRE(are_containers_equal(v, expected));
         }
         SECTION("range (forward iterator)")
         {
            auto pos = v.insert(v.begin()+2, values.begin(), values.end());
            REQUIRE(pos == v.begin()+2);
            REQUIRE(are_containers_equal(v, expected));
            REQUIRE(counted_type::check().move_constructions(1).move_assignments(2).copy_assignments(1));
         }
         SECTION("initializer list")
         {
            auto pos = v.insert(v.begin()+2, values);
            REQUIRE(pos == v.begin()+2);
            REQUIRE(are_containers_equal(v, expected));
            REQUIRE(counted_type::check().move_constructions(1).move_assignments(2).copy_assignments(1));
         }
      }
      SECTION("count=2")
      {
         auto expected = std::initializer_list<counted_type>{3, 3, 7, 11, 3, 3, 3};
         auto values = std::initializer_list<counted_type>{7, 11};
         counted_type::reset_counts();
         SECTION("range (input iterator)")
         {
            auto pos = v.insert(v.begin()+2, counted_type_stream_iterator{values}, counted_type_stream_iterator{});
            REQUIRE(pos == v.begin()+2);
            REQUIRE(are_containers_equal(v, expected));
         }
         SECTION("range (forward iterator)")
         {
            auto pos = v.insert(v.begin()+2, values.begin(), values.end());
            REQUIRE(pos == v.begin()+2);
            REQUIRE(are_containers_equal(v, expected));
            REQUIRE(counted_type::check().move_constructions(2).move_assignments(1).copy_assignments(2));
         }
         SECTION("initializer list")
         {
            auto pos = v.insert(v.begin()+2, values);
            REQUIRE(pos == v.begin()+2);
            REQUIRE(are_containers_equal(v, expected));
            REQUIRE(counted_type::check().move_constructions(2).move_assignments(1).copy_assignments(2));
         }
      }
      SECTION("count=3")
      {
         auto expected = std::initializer_list<counted_type>{3, 3, 7, 11, 13, 3, 3, 3};
         auto values = std::initializer_list<counted_type>{7, 11, 13};
         counted_type::reset_counts();
         SECTION("range (input iterator)")
         {
            auto pos = v.insert(v.begin()+2, counted_type_stream_iterator{values}, counted_type_stream_iterator{});
            REQUIRE(pos == v.begin()+2);
            REQUIRE(are_containers_equal(v, expected));
         }
         SECTION("range (forward iterator)")
         {
            auto pos = v.insert(v.begin()+2, values.begin(), values.end());
            REQUIRE(pos == v.begin()+2);
            REQUIRE(are_containers_equal(v, expected));
            REQUIRE(counted_type::check().move_constructions(3).copy_assignments(3));
         }
         SECTION("initializer list")
         {
            auto pos = v.insert(v.begin()+2, values);
            REQUIRE(pos == v.begin()+2);
            REQUIRE(are_containers_equal(v, expected));
            REQUIRE(counted_type::check().move_constructions(3).copy_assignments(3));
         }
      }
      SECTION("count=4")
      {
         auto expected = std::initializer_list<counted_type>{3, 3, 7, 11, 13, 17, 3, 3, 3};
         auto values = std::initializer_list<counted_type>{7, 11, 13, 17};
         counted_type::reset_counts();
         SECTION("range (input iterator)")
         {
            auto pos = v.insert(v.begin()+2, counted_type_stream_iterator{values}, counted_type_stream_iterator{});
            REQUIRE(pos == v.begin()+2);
            REQUIRE(are_containers_equal(v, expected));
         }
         SECTION("range (forward iterator)")
         {
            auto pos = v.insert(v.begin()+2, values.begin(), values.end());
            REQUIRE(pos == v.begin()+2);
            REQUIRE(are_containers_equal(v, expected));
            REQUIRE(counted_type::check().move_constructions(3).copy_constructions(1).copy_assignments(3));
         }
         SECTION("initializer list")
         {
            auto pos = v.insert(v.begin()+2, values);
            REQUIRE(pos == v.begin()+2);
            REQUIRE(are_containers_equal(v, expected));
            REQUIRE(counted_type::check().move_constructions(3).copy_constructions(1).copy_assignments(3));
         }
      }
      SECTION("count=5")
      {
         auto expected = std::initializer_list<counted_type>{3, 3, 7, 11, 13, 17, 23, 3, 3, 3};
         auto values = std::initializer_list<counted_type>{7, 11, 13, 17, 23};
         counted_type::reset_counts();
         SECTION("range (input iterator)")
         {
            auto pos = v.insert(v.begin()+2, counted_type_stream_iterator{values}, counted_type_stream_iterator{});
            REQUIRE(pos == v.begin()+2);
            REQUIRE(are_containers_equal(v, expected));
         }
         SECTION("range (forward iterator)")
         {
            auto pos = v.insert(v.begin()+2, values.begin(), values.end());
            REQUIRE(pos == v.begin()+2);
            REQUIRE(are_containers_equal(v, expected));
            REQUIRE(counted_type::check().move_constructions(3).copy_constructions(2).copy_assignments(3));
         }
         SECTION("initializer list")
         {
            auto pos = v.insert(v.begin()+2, values);
            REQUIRE(pos == v.begin()+2);
            REQUIRE(are_containers_equal(v, expected));
            REQUIRE(counted_type::check().move_constructions(3).copy_constructions(2).copy_assignments(3));
         }
      }
   }
   #if _sstl_has_exceptions()
   SECTION("exception handling")
   {
      auto values = std::initializer_list<counted_type>{ 1, 2, 3 };
      SECTION("end (strong exception safety)")
      {
         auto expected = v;
         SECTION("range (input iterator)")
         {
            counted_type::reset_counts();
            counted_type::throw_at_nth_copy_construction(3);
            REQUIRE_THROWS_AS(v.insert(v.end(), counted_type_stream_iterator{values}, counted_type_stream_iterator{}), counted_type::copy_construction::exception);
            REQUIRE(counted_type::check().copy_constructions(2).destructions(2));
            REQUIRE(are_containers_equal(v, expected));
         }
         SECTION("range (forward iterator)")
         {
            counted_type::reset_counts();
            counted_type::throw_at_nth_copy_construction(3);
            REQUIRE_THROWS_AS(v.insert(v.end(), values.begin(), values.end()), counted_type::copy_construction::exception);
            REQUIRE(counted_type::check().copy_constructions(2).destructions(2));
            REQUIRE(are_containers_equal(v, expected));
         }
         SECTION("initializer list")
         {
            counted_type::reset_counts();
            counted_type::throw_at_nth_copy_construction(3);
            REQUIRE_THROWS_AS(v.insert(v.end(), values), counted_type::copy_construction::exception);
            REQUIRE(counted_type::check().copy_constructions(2).destructions(2));
            REQUIRE(are_containers_equal(v, expected));
         }
      }
      SECTION("middle (basic exception safety)")
      {
         SECTION("range (input iterator)")
         {
            auto expected = v;
            std::copy(values.begin(), values.end(), std::back_inserter(expected));
            counted_type::reset_counts();
            counted_type::throw_at_nth_move_construction(1);
            REQUIRE_THROWS_AS(v.insert(v.begin()+2, counted_type_stream_iterator{values}, counted_type_stream_iterator{}), counted_type::move_construction::exception);
            REQUIRE(counted_type::check().copy_constructions(3).destructions(0));
            REQUIRE(are_containers_equal(v, expected));
         }
         SECTION("range (forward iterator)")
         {
            counted_type::reset_counts();
            counted_type::throw_at_nth_copy_assignment(3);
            REQUIRE_THROWS_AS(v.insert(v.begin()+2, values.begin(), values.end()), counted_type::copy_assignment::exception);
            REQUIRE(counted_type::check().move_constructions(3).copy_assignments(2).destructions(8));
            REQUIRE(v.empty());
         }
         SECTION("initializer list")
         {
            counted_type::reset_counts();
            counted_type::throw_at_nth_copy_assignment(3);
            REQUIRE_THROWS_AS(v.insert(v.begin()+2, values), counted_type::copy_assignment::exception);
            REQUIRE(counted_type::check().move_constructions(3).copy_assignments(2).destructions(8));
            REQUIRE(v.empty());
         }
      }
   }
   #endif
}

TEST_CASE("vector - emplace")
{
   SECTION("begin")
   {
      auto expected = std::initializer_list<counted_type>{7, 3, 3, 3, 3, 3};
      auto v = vector_counted_type_t{3, 3, 3, 3, 3};

      counted_type::reset_counts();
      auto pos = v.emplace(v.begin(), 7);

      REQUIRE(pos == v.begin());
      REQUIRE(are_containers_equal(v, expected));
      REQUIRE(counted_type::check().parameter_constructions(1).move_constructions(1).move_assignments(5));
   }
   SECTION("end")
   {
      auto expected = std::initializer_list<counted_type>{3, 3, 3, 3, 3, 7};
      auto v = vector_counted_type_t{3, 3, 3, 3, 3};

      counted_type::reset_counts();
      auto pos = v.emplace(v.end(), 7);

      REQUIRE(pos == v.end()-1);
      REQUIRE(are_containers_equal(v, expected));
      REQUIRE(counted_type::check().parameter_constructions(1).move_constructions(1));
   }
   SECTION("middle")
   {
      auto expected = std::initializer_list<counted_type>{3, 3, 7, 3, 3, 3};
      auto v = vector_counted_type_t{3, 3, 3, 3, 3};

      counted_type::reset_counts();
      auto pos = v.emplace(v.begin()+2, 7);

      REQUIRE(pos == v.begin()+2);
      REQUIRE(are_containers_equal(v, expected));
      REQUIRE(counted_type::check().parameter_constructions(1).move_constructions(1).move_assignments(3));
   }
   #if _sstl_has_exceptions()
   SECTION("exception handling (of parameter construction)")
   {
      auto v = vector_counted_type_t{3, 3, 3, 3, 3};
      auto expected = v;
      SECTION("end (strong exception safety)")
      {
         counted_type::reset_counts();
         counted_type::throw_at_nth_parameter_construction(1);
         REQUIRE_THROWS_AS(v.emplace(v.end(), 5), counted_type::parameter_construction::exception);
         REQUIRE(counted_type::check().constructions(0).destructions(0));
         REQUIRE(are_containers_equal(v, expected));
      }
      SECTION("middle (strong exception safety)")
      {
         counted_type::reset_counts();
         counted_type::throw_at_nth_parameter_construction(1);
         REQUIRE_THROWS_AS(v.insert(v.begin()+2, 5), counted_type::parameter_construction::exception);
         REQUIRE(counted_type::check().constructions(0).destructions(0));
         REQUIRE(are_containers_equal(v, expected));
      }
   }
   SECTION("exception handling (of move construction)")
   {
      auto v = vector_counted_type_t{3, 3, 3, 3, 3};
      SECTION("end (strong exception safety)")
      {
         auto expected = v;
         counted_type::reset_counts();
         counted_type::throw_at_nth_move_construction(1);
         REQUIRE_THROWS_AS(v.emplace(v.end(), 5), counted_type::move_construction::exception);
         REQUIRE(counted_type::check().parameter_constructions(1).destructions(1));
         REQUIRE(are_containers_equal(v, expected));
      }
      SECTION("middle (basic exception safety)")
      {
         counted_type::reset_counts();
         counted_type::throw_at_nth_move_construction(1);
         REQUIRE_THROWS_AS(v.insert(v.begin()+2, 5), counted_type::move_construction::exception);
         REQUIRE(counted_type::check().parameter_constructions(1).destructions(6));
         REQUIRE(v.empty());
      }
   }
   #endif
}

TEST_CASE("vector - erase")
{
   SECTION("begin")
   {
      auto expected = std::initializer_list<counted_type>{3, 3, 3, 3};
      auto v = vector_counted_type_t{7, 3, 3, 3, 3};

      counted_type::reset_counts();
      auto pos = v.erase(v.begin());

      REQUIRE(pos == v.begin());
      REQUIRE(are_containers_equal(v, expected));
      REQUIRE(counted_type::check().move_assignments(4).destructions(1));
   }
   SECTION("end-1")
   {
      auto expected = std::initializer_list<counted_type>{3, 3, 3, 3};
      auto v = vector_counted_type_t{3, 3, 3, 3, 7};

      counted_type::reset_counts();
      auto pos = v.erase(v.end()-1);

      REQUIRE(pos == v.end());
      REQUIRE(are_containers_equal(v, expected));
      REQUIRE(counted_type::check().destructions(1));
   }
   SECTION("middle")
   {
      auto expected = std::initializer_list<counted_type>{3, 3, 3, 3};
      auto v = vector_counted_type_t{3, 3, 7, 3, 3};

      counted_type::reset_counts();
      auto pos = v.erase(v.begin()+2);

      REQUIRE(pos == v.begin()+2);
      REQUIRE(are_containers_equal(v, expected));
      REQUIRE(counted_type::check().move_assignments(2).destructions(1));
   }
   #if _sstl_has_exceptions()
   SECTION("exception handling (basic exception safety)")
   {
      auto v = vector_counted_type_t{3, 3, 3, 3, 3};
      counted_type::reset_counts();
      counted_type::throw_at_nth_move_assignment(2);
      REQUIRE_THROWS_AS(v.erase(v.begin()+2), counted_type::move_assignment::exception);
      REQUIRE(counted_type::check().move_assignments(1).destructions(5));
      REQUIRE(v.empty());
   }
   #endif
}

TEST_CASE("vector - range erase")
{
   auto v = vector_counted_type_t{1, 3, 7, 11, 13};
   SECTION("begin")
   {
      SECTION("range is empty")
      {
         auto expected = std::initializer_list<counted_type>{1, 3, 7, 11, 13};
         counted_type::reset_counts();
         auto pos = v.erase(v.begin(), v.begin());
         REQUIRE(pos == v.begin());
         REQUIRE(are_containers_equal(v, expected));
         REQUIRE(counted_type::check().move_assignments(5).destructions(0));
      }
      SECTION("range = [begin; begin+1)")
      {
         auto expected = std::initializer_list<counted_type>{3, 7, 11, 13};
         counted_type::reset_counts();
         auto pos = v.erase(v.begin(), v.begin()+1);
         REQUIRE(pos == v.begin());
         REQUIRE(are_containers_equal(v, expected));
         REQUIRE(counted_type::check().move_assignments(4).destructions(1));
      }
      SECTION("range = [begin; begin+2)")
      {
         auto expected = std::initializer_list<counted_type>{7, 11, 13};
         counted_type::reset_counts();
         auto pos = v.erase(v.begin(), v.begin()+2);
         REQUIRE(pos == v.begin());
         REQUIRE(are_containers_equal(v, expected));
         REQUIRE(counted_type::check().move_assignments(3).destructions(2));
      }
      SECTION("range = [begin; end-1)")
      {
         auto expected = std::initializer_list<counted_type>{13};
         counted_type::reset_counts();
         auto pos = v.erase(v.begin(), v.end()-1);
         REQUIRE(pos == v.begin());
         REQUIRE(are_containers_equal(v, expected));
         REQUIRE(counted_type::check().move_assignments(1).destructions(4));
      }
      SECTION("range = [begin; end)")
      {
         counted_type::reset_counts();
         auto pos = v.erase(v.begin(), v.end());
         REQUIRE(pos == v.begin());
         REQUIRE(pos == v.end());
         REQUIRE(v.empty());
         REQUIRE(counted_type::check().move_assignments(0).destructions(5));
      }

   }
   SECTION("end (range is empty)")
   {
      auto expected = std::initializer_list<counted_type>{1, 3, 7, 11, 13};
      counted_type::reset_counts();
      auto pos = v.erase(v.end(), v.end());
      REQUIRE(pos == v.end());
      REQUIRE(are_containers_equal(v, expected));
      REQUIRE(counted_type::check().move_assignments(0).destructions(0));
   }
   SECTION("end-1")
   {
      SECTION("range is empty")
      {
         auto expected = std::initializer_list<counted_type>{1, 3, 7, 11, 13};
         counted_type::reset_counts();
         auto pos = v.erase(v.end()-1, v.end()-1);
         REQUIRE(pos == v.end()-1);
         REQUIRE(are_containers_equal(v, expected));
         REQUIRE(counted_type::check().move_assignments(1).destructions(0));
      }
      SECTION("range = [end-1; end)")
      {
         auto expected = std::initializer_list<counted_type>{1, 3, 7, 11};
         counted_type::reset_counts();
         auto pos = v.erase(v.end()-1, v.end());
         REQUIRE(pos == v.end());
         REQUIRE(are_containers_equal(v, expected));
         REQUIRE(counted_type::check().move_assignments(0).destructions(1));
      }
   }
   SECTION("middle")
   {
      SECTION("range is empty")
      {
         auto expected = std::initializer_list<counted_type>{1, 3, 7, 11, 13};
         counted_type::reset_counts();
         auto pos = v.erase(v.begin()+2, v.begin()+2);
         REQUIRE(pos == v.begin()+2);
         REQUIRE(are_containers_equal(v, expected));
         REQUIRE(counted_type::check().move_assignments(3).destructions(0));
      }
      SECTION("range = [middle; middle+1)")
      {
         auto expected = std::initializer_list<counted_type>{1, 3, 11, 13};
         counted_type::reset_counts();
         auto pos = v.erase(v.begin()+2, v.begin()+3);
         REQUIRE(pos == v.begin()+2);
         REQUIRE(are_containers_equal(v, expected));
         REQUIRE(counted_type::check().move_assignments(2).destructions(1));
      }
      SECTION("range = [middle; middle+2) , i.e. [middle; end-1)")
      {
         auto expected = std::initializer_list<counted_type>{1, 3, 13};
         counted_type::reset_counts();
         auto pos = v.erase(v.begin()+2, v.begin()+4);
         REQUIRE(pos == v.begin()+2);
         REQUIRE(are_containers_equal(v, expected));
         REQUIRE(counted_type::check().move_assignments(1).destructions(2));
      }
      SECTION("range = [middle; end)")
      {
         auto expected = std::initializer_list<counted_type>{1, 3};
         counted_type::reset_counts();
         auto pos = v.erase(v.begin()+2, v.end());
         REQUIRE(pos == v.begin()+2);
         REQUIRE(are_containers_equal(v, expected));
         REQUIRE(counted_type::check().move_assignments(0).destructions(3));
      }
   }
   #if _sstl_has_exceptions()
   SECTION("exception handling (basic exception safety)")
   {
      auto v = vector_counted_type_t{3, 3, 3, 3, 3};
      counted_type::reset_counts();
      counted_type::throw_at_nth_move_assignment(2);
      REQUIRE_THROWS_AS(v.erase(v.begin()+1, v.begin()+3), counted_type::move_assignment::exception);
      REQUIRE(counted_type::check().move_assignments(1).destructions(5));
      REQUIRE(v.empty());
   }
   #endif
}

TEST_CASE("vector - push_back")
{
   SECTION("contained values")
   {
      auto v = vector_int_t{};
      auto expected = std::vector<int>{};

      v.push_back(1);
      expected.push_back(1);
      REQUIRE(are_containers_equal(v, expected));

      v.push_back(3);
      expected.push_back(3);
      REQUIRE(are_containers_equal(v, expected));

      v.push_back(5);
      expected.push_back(5);
      REQUIRE(are_containers_equal(v, expected));
   }
   SECTION("number of constructions")
   {
      auto v = vector_counted_type_t{};
      auto value = counted_type{};

      counted_type::reset_counts();
      v.push_back(value);
      REQUIRE(counted_type::check().copy_constructions(1));

      counted_type::reset_counts();
      v.push_back(std::move(value));
      REQUIRE(counted_type::check().move_constructions(1));
   }
   #if _sstl_has_exceptions()
   SECTION("exception handling (strong exception safety)")
   {
      auto v = vector_counted_type_t{3, 3, 3, 3, 3};
      auto expected = v;
      auto value = counted_type{ 5 };
      SECTION("by-lvalue-reference version")
      {
         counted_type::reset_counts();
         counted_type::throw_at_nth_copy_construction(1);
         REQUIRE_THROWS_AS(v.push_back(value), counted_type::copy_construction::exception);
         REQUIRE(counted_type::check().constructions(0).destructions(0));
         REQUIRE(are_containers_equal(v, expected));
      }
      SECTION("by-rvalue-reference version")
      {
         counted_type::reset_counts();
         counted_type::throw_at_nth_move_construction(1);
         REQUIRE_THROWS_AS(v.push_back(std::move(value)), counted_type::move_construction::exception);
         REQUIRE(counted_type::check().constructions(0).destructions(0));
         REQUIRE(are_containers_equal(v, expected));
      }
   }
   #endif
}

TEST_CASE("vector - emplace_back")
{
   SECTION("contained values")
   {
      auto v = vector_int_t{};
      auto expected = std::vector<int>{};

      v.emplace_back(1);
      expected.emplace_back(1);
      REQUIRE(are_containers_equal(v, expected));

      v.emplace_back(3);
      expected.emplace_back(3);
      REQUIRE(are_containers_equal(v, expected));

      v.emplace_back(5);
      expected.emplace_back(5);
      REQUIRE(are_containers_equal(v, expected));
   }
   SECTION("number of constructions")
   {
      auto v = vector_counted_type_t{};

      counted_type::reset_counts();
      v.emplace_back();
      REQUIRE(counted_type::check().default_constructions(1));

      counted_type::reset_counts();
      v.emplace_back(1);
      REQUIRE(counted_type::check().parameter_constructions(1));
   }
   #if _sstl_has_exceptions()
   SECTION("exception handling (strong exception safety)")
   {
      auto v = vector_counted_type_t{3, 3, 3, 3, 3};
      auto expected = v;
      counted_type::reset_counts();
      counted_type::throw_at_nth_parameter_construction(1);
      REQUIRE_THROWS_AS(v.emplace_back(5), counted_type::parameter_construction::exception);
      REQUIRE(counted_type::check().constructions(0).destructions(0));
      REQUIRE(are_containers_equal(v, expected));
   }
   #endif
}

TEST_CASE("vector - pop_back")
{
   SECTION("contained values")
   {
      auto v = vector_int_t{1, 3, 5};
      auto expected = std::vector<int>{1, 3, 5};

      v.pop_back();
      expected.pop_back();
      REQUIRE(are_containers_equal(v, expected));

      v.pop_back();
      expected.pop_back();
      REQUIRE(are_containers_equal(v, expected));

      v.pop_back();
      expected.pop_back();
      REQUIRE(are_containers_equal(v, expected));
   }
   SECTION("number of destructions")
   {
      auto v = vector_counted_type_t{1, 3, 5};

      counted_type::reset_counts();
      v.pop_back();
      REQUIRE(counted_type::check().destructions(1));

      counted_type::reset_counts();
      v.pop_back();
      REQUIRE(counted_type::check().destructions(1));

      counted_type::reset_counts();
      v.pop_back();
      REQUIRE(counted_type::check().destructions(1));
   }
}

TEST_CASE("vector - swap")
{
   SECTION("contained values")
   {
      auto expected_lhs = {1, 3, 5, 7, 13, 17, 19};
      auto expected_rhs = {23, 29};
      SECTION("rhs' capacity is same")
      {
         auto lhs = vector_int_t{expected_rhs};
         auto rhs = vector_int_t{expected_lhs};
         swap(lhs, rhs);
         REQUIRE(are_containers_equal(lhs, expected_lhs));
         REQUIRE(are_containers_equal(rhs, expected_rhs));
      }
      SECTION("rhs' capacity is different")
      {
         auto lhs = sstl::vector<int, 10>{expected_rhs};
         auto rhs = sstl::vector<int, 30>{expected_lhs};
         swap(lhs, rhs);
         REQUIRE(are_containers_equal(lhs, expected_lhs));
         REQUIRE(are_containers_equal(rhs, expected_rhs));
      }
   }
   SECTION("number of operations")
   {
      auto expected_lhs = std::initializer_list<counted_type>{1, 3, 5, 7, 13, 17, 19};
      auto expected_rhs = std::initializer_list<counted_type>{23, 29};

      auto lhs = sstl::vector<counted_type, 10>{expected_rhs};
      auto rhs = sstl::vector<counted_type, 30>{expected_lhs};
      counted_type::reset_counts();
      swap(lhs, rhs);
      REQUIRE(counted_type::check().move_constructions(7).move_assignments(4).destructions(7));
   }
   #if _sstl_has_exceptions()
   SECTION("exception handling (basic exceptin safety)")
   {
      auto lhs = sstl::vector<counted_type, 10>{1, 2, 3, 4, 5, 6};
      auto rhs = sstl::vector<counted_type, 30>{1, 2, 3};
      SECTION("exception thrown during first half of swap operation (while swapping lhs and rhs elements)")
      {
         counted_type::reset_counts();
         counted_type::throw_at_nth_move_assignment(4);
         REQUIRE_THROWS_AS(swap(lhs, rhs), counted_type::move_assignment::exception);
         REQUIRE(counted_type::check().move_constructions(2).move_assignments(3).destructions(2 + 3 + 6));
         REQUIRE(lhs.empty());
         REQUIRE(rhs.empty());
      }
      SECTION("exception thrown during second half of swap operation (while move constructing lhs elements into rhs)")
      {
         counted_type::reset_counts();
         counted_type::throw_at_nth_move_construction(5);
         REQUIRE_THROWS_AS(swap(lhs, rhs), counted_type::move_construction::exception);
         REQUIRE(counted_type::check().move_constructions(4).move_assignments(6).destructions(3 + 4 + 6));
         REQUIRE(lhs.empty());
         REQUIRE(rhs.empty());
      }
   }
   #endif
}

TEST_CASE("vector - non-member relative operators")
{
   SECTION("lhs < rhs")
   {
      {
         auto lhs = vector_int_t{0, 1, 2};
         auto rhs = vector_int_t{0, 1, 2, 3};
         REQUIRE(!(lhs == rhs));
         REQUIRE(lhs != rhs);
         REQUIRE(lhs < rhs);
         REQUIRE(lhs <= rhs);
         REQUIRE(!(lhs > rhs));
         REQUIRE(!(lhs >= rhs));
      }
      {
         auto lhs = vector_int_t{0, 1, 2, 3};
         auto rhs = vector_int_t{0, 1, 3, 3};
         REQUIRE(!(lhs == rhs));
         REQUIRE(lhs != rhs);
         REQUIRE(lhs < rhs);
         REQUIRE(lhs <= rhs);
         REQUIRE(!(lhs > rhs));
         REQUIRE(!(lhs >= rhs));
      }
   }
   SECTION("lhs == rhs")
   {
      {
         auto lhs = vector_int_t{0, 1, 2};
         auto rhs = vector_int_t{0, 1, 2};
         REQUIRE(lhs == rhs);
         REQUIRE(!(lhs != rhs));
         REQUIRE(!(lhs < rhs));
         REQUIRE(lhs <= rhs);
         REQUIRE(!(lhs > rhs));
         REQUIRE(lhs >= rhs);
      }
   }
   SECTION("lhs > rhs")
   {
      {
         auto lhs = vector_int_t{0, 1, 2, 3};
         auto rhs = vector_int_t{0, 1, 2};
         REQUIRE(!(lhs == rhs));
         REQUIRE(lhs != rhs);
         REQUIRE(!(lhs < rhs));
         REQUIRE(!(lhs <= rhs));
         REQUIRE(lhs > rhs);
         REQUIRE(lhs >= rhs);
      }
      {
         auto lhs = vector_int_t{0, 1, 3, 3};
         auto rhs = vector_int_t{0, 1, 2, 3};
         REQUIRE(!(lhs == rhs));
         REQUIRE(lhs != rhs);
         REQUIRE(!(lhs < rhs));
         REQUIRE(!(lhs <= rhs));
         REQUIRE(lhs > rhs);
         REQUIRE(lhs >= rhs);
      }
   }
}

TEST_CASE("vector - memory footprint")
{
   using word_size_t = void*;
   REQUIRE(sizeof(sstl::vector<word_size_t, 1>) == (1+1+1)*sizeof(word_size_t));
   REQUIRE(sizeof(sstl::vector<word_size_t, 10>) == (1+10+1)*sizeof(word_size_t));
}

}
