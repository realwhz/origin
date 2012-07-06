// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and condit

#ifndef TEST_HPP
#define TEST_HPP

#include <iosfwd>

// Initialize an empty vector with n default-initialized elements of the
// test size. Elements are destroyed and memory deallocated when the test
// completes.
template <typename Vec>
  struct fill_init_test
  {
    template <typename Env>
      void operator()(Env& env, std::size_t n, std::size_t r) const
      {
        Vec v(n);
      }
  };

template <typename T, typename Env>
  void exec(Env& env)
  {
    fill_init_test<T> test;
    run(env, test, 10, 7);
    env.print(std::cout);
  }

#endif