// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_RANGE_HPP
#define ORIGIN_RANGE_HPP

#include <origin/iterator.hpp>

namespace origin
{
  // Begin and end
  // These functions wrap the calls to std::begin() and std::end() and are
  // essentially only provided for convenience.
    
  template <typename R>
    auto begin(R& range) -> decltype(range.begin()) { return range.begin(); }

  template <typename R>
    auto end(R& range) -> decltype(range.end()) { return range.end(); }

  template <typename R>
    auto begin(const R& range) -> decltype(range.begin()) { return range.begin(); }

  template <typename R>
    auto end(const R& range) -> decltype(range.end()) { return range.end(); }

  // Range adapation for arrays
  template <typename T, std::size_t N>
    T* begin(T (&a)[N]) { return a; }

  template <typename T, std::size_t N>
    T* end(T (&a)[N]) { return a + N; }


  // The Range concept
  //
  // A range is simply a class that exposes a pair of iterators called begin(r)
  // and end(r). It is, in some senses, a very lightweight container.
  //
  // Note that for a range R, the following type aliases are available:
  //
  //    - Iterator_type<R>
  //    - Iterator_type<R const>
  //
  // By const-qualifying R, we can differentiate between const and non-const
  // iterators for the range. This is particularly helpful for containers.
  


  // Infrastructure for deducing the results of begin and end.
  namespace traits
  {
    template <typename T>
      struct begin_result
      {
      private:
        template <typename X>
          static auto check(X x) -> decltype(begin(x));
        static subst_failure check(...);
      public:
        using type = decltype(check(std::declval<T>()));
      };

  // Safely get the type returned by std::end(x).
  template <typename T>
    struct end_result
    {
    private:
      template <typename X>
        static auto check(X x) -> decltype(end(x));
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>()));
    };
  } // namespace traits



  // Begin result (alias)    
  // An alias to the result of the expression empty(x).
  template <typename T>
    using Begin_result = typename traits::begin_result<T>::type;
    


  // Has begin (constraint)
  // Return true if begin(r) is a valid expression.
  template <typename T>
    bool constexpr Has_begin()
    {
      return Subst_succeeded<Begin_result<T>>();
    }


  // End result (alias)    
  // An alias to the result of the expression empty(x).
  template <typename T>
    using End_result = typename traits::end_result<T>::type;



  // Has begin (constraint)    
  // Return true if end(x) is a valid expression.
  template <typename T>
    bool constexpr Has_end()
    {
      return Subst_succeeded<End_result<T>>();
    }
    
    
    
  // Iterator type
  // Any type that exposes begin()/end() members has an associated iterator
  // type. The actual iterator type depends on the const-ness of the alias'
  // argument type. For example, a const-container will have a constant
  // iterator, while a non-const container will have a non-constant iterator.
  
  // An alias to the iterator type of a range. This is the same as the
  // result of the begin operation on the same type. Note that "const R" may
  // yield a different type than an unqualified "R".
  template <typename R>
    using Iterator_type = Begin_result<R>;
    

 
  // Range (concept)
  // A range provides a pair of iterators called begin and end. Ranges are not 
  // required to be copyable or even movable. Examples of more degenerate 
  // implementations include non-copyable Containers and C-arrays.
  template <typename R>
    constexpr bool Range()
    {
      return Has_begin<R>()   // begin(r)
          && Has_end<R>()     // end(r)
          && Same<Begin_result<R>, End_result<R>>()
          && Iterator<Begin_result<R>>();
    }



  // Strict range (concept)
  // A strict range is a range that is not also a container. The two are
  // differentiated by the presence of the size and empty member functions.
  template <typename R>
    constexpr bool Strict_range() { return Range<R>() && !Has_member_size<R>(); }
    


  // Iterator range concepts
    
  // NOTE: The meaning of saying "is fooable everywhere except its limit" is
  // analogous to asserting the corresponding property for all ranges r of 
  // some Range type R:
  //
  //   is_fooable_range(begin(r), end(r))
  //
  // as an invariant of the type.
    


  // Input range (concept)
  // Returns true if R is an input range. An input range is a range of input
  // iterators. An input range is readable everywhere except its limit.
  template <typename R>
    constexpr bool Input_range()
    {
      return Range<R>() && Input_iterator<Iterator_type<R>>();
    }



  // Strict input range (concept)
  // Returns true if R is a range of strict input iterators.
  template <typename R>
    constexpr bool Strict_input_range()
    {
      return Range<R>() && Strict_input_iterator<Iterator_type<R>>();
    }


  
  // Output range (concept)
  // Returns true if R is an output range. An output range is a range of 
  // writable iterators and is writable everywhere except its limit.
  //
  // If T is an rvalue reference, then this concept additionally requires that
  // T implement move semantics. If T is not an rvalue reference, then copy
  // semantics are required.
  template <typename R, typename T>
    constexpr bool Output_range()
    {
      return Range<R>() && Output_iterator<Iterator_type<R>, T>();
    }
    
    

  // Forward range (concept)
  // Returns true if R is a forward range. A forward range is a range whose 
  // iterator type is a forward iterator.
  template <typename R>
    constexpr bool Forward_range()
    {
      return Range<R>() && Forward_iterator<Iterator_type<R>>();
    }


  
  // Bidirectional range (concept)
  // Returns true if R is a bidirectional range.
  template <typename R>
    constexpr bool Bidirectional_range()
    {
      return Range<R>() && Bidirectional_iterator<Iterator_type<R>>();
    }



  // Random access range (concept)
  // Returns true if R is a random access range.
  template <typename R>
    constexpr bool Random_access_range()
    {
      return Range<R>() && Random_access_iterator<Iterator_type<R>>();
    }
  

  
  // Value type (deduction)
  // The value type of a range is deduced from the expression *begin(range) for
  // all Range types.
  template <typename R>
    auto deduce_value_type(default_t, const R& range) 
    -> Unqualified<decltype(*begin(range))>;



  // Distance type (deduction)
  // The distance type of a range is the same as that of its iterator type.
  template <typename R>
    auto deduce_distance_type(default_t, const R& range)
      -> Distance_type<decltype(begin(range))>;

  


  // Size
  // Returns the size of the range. The size of a range is equivalent to its
  // distance, but the result type is guaranteed to be unsigned.
  //
  // If the range has a member function named size(), then that operation is
  // selected over the default comptuation.

  // Return the size of x using the expression x.size(). Note that this is
  // declared as constexpr because some container-like types have constexpr
  // size member functions.
  template <typename T>
    inline constexpr auto size(const T& x) -> decltype(x.size()) 
    { 
      return x.size(); 
    }

  // For static arrays, the size is obvious.
  template <typename T, std::size_t N>
    inline constexpr std::size_t size(T(&)[N])
    {
      return N;
    }
    
  // Returns the size for non-container-like ranges (without a member function
  // size).
  template <typename R>
    inline auto size(const R& range) 
      -> Requires<Strict_range<R>(), Make_unsigned<Distance_type<R>>>
    {
      return distance(begin(range), end(range));
    }
  


  // Size type (deduction)
  // The size type of a range is deduced from the expression size(range) for
  // any Range range.
  template <typename R>
    auto deduce_size_type(default_t, const R& range) -> decltype(size(range));



  // Infrastructure for deducing the validity of the expression size(x). Note
  // that this cannot be implemented by simply checking the validity of
  // Size_type<T> since there could be other expressions used to deduce that 
  // type for different concepts.
  namespace traits
  {
    template <typename T>
      struct get_size_result
      {
      private:
        template <typename X>
          static auto check(const X& x) -> decltype(size(x));
        static subst_failure check(...);
      public:
        using type = decltype(check(std::declval<T>()));
      };
  }

  // Has size (trait)
  // Returns true if the expression size(t) is valid.
  template <typename T>
    constexpr bool Has_size()
    {
      return Subst_succeeded<typename traits::get_size_result<T>::type>();
    }



  // Empty
  // A type may support empty queries as in empty(x). Like size(), empty is
  // defined in 3 ways:
  //    - For containers, it is x.empty().
  //    - For ranges, it is begin(x) == end(x)
  //    - For statically sized arrays of type T[N], it is N == 0.
  

  
  // Empty (member)
  // Return true if x is empty. The meaning of empty is dependent on the type
  // of T.
  //
  // NOTE: Some containers may have constexpr empty (e.g., arrays).
  template <typename T>
    constexpr auto empty(T const& x) -> decltype(x.empty())
    {
      return x.empty();
    }



  // Empty (array)
  // Specialization for statically sized arrays.
  template <typename T, std::size_t N>
    constexpr bool empty(T(&a)[N])
    {
      return N == 0;
    }



  // Empty (range)
  // Specialization for ranges, which don't have a member empty.
  template <typename R>
    inline Requires<!Has_member_empty<R>(), bool> empty(R const& r)
    {
      return std::begin(r) == std::end(r);
    }



  // Safely get the result type of the expression empty(r).
  template <typename T>
    struct empty_result
    {
    private:
      template <typename X>
        static auto check(X const& x) -> decltype(empty(x));
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>()));
    };
  
  // An alias for the result of the empty(t) expression.
  template <typename T>
    using Empty_result = typename empty_result<T>::type;

  // Returns true if empty(t) is a valid expression.
  template <typename T>
    constexpr bool Has_empty()
    {
      return Subst_succeeded<Empty_result<T>>();
    }


    
  // Distance (range)
  // Return the distance between the first and last elements of the range.
  // Note that the result type of distance may be different than that of the
  // equivalent computation size().
  template <typename R>
    auto distance(const R& range) 
      -> decltype(o_distance(begin(range), end(range)))
    {
      return o_distance(begin(range), end(range));
    }


  
  // Range adaptors
  // A range adaptor is a model of a range. These typically wrap other data
  // structures or data elements in such a way that they can be iterated over.
  //
  // Note that a range adaptor must declare its value type and difference type.
  // The latter is required so that Distance_type<R> is a valid expression for
  // ranges (as is Size_type<R>). It is not strictly required that a Range
  // declare a nested iterator name; that is uniquely deduced from the begin()
  // and end() operations.
  
  


  // Counted range
  // A counted range adapts an iterator, distance pair into a bounded range.
  //
  // Note that the semantics of counted ranges differ from the EoP notion. In
  // Origin, a counted range has the same semantics as a weak range in EoP. The
  // acyclic property of EoP counted is considered separately in Origin.
  //
  // FIXME: Finish writing this class.
  /*
  template <typename I>
    class counted_range
    {    
    public:
      using value_type = Value_type<I>;
      using difference_type = Distance_type<I>;
      using iterator = counted_iterator<I>;
      
      counted_range(I first, Distance_type<I> n)
        : first{first}, count{n}
      { }

      // Returns the underlying iterator
      I base() const { return first; }
      
      // Returns the number of times the iterator can be incremeted.
      Distance_type<I> distance() const { return count; }
      
      // Range
      iterator begin() const { return {first, count}; }
      iterator end() const { return {first, 0}; }
    
    private:
      I first;
      Distance_type<I> count;
    };
  */

  
  // Bounded range
  // A bounded range encapsulates a pair of iterator and has the 
  // is_bounded_range precondition as an invariant.
  //
  // wraps a pair of iterators. This is essentially the same as the Boost 
  // iterator_range, or pair<I, I> with appropriate overloads.
  //
  // Invariants: 
  //    bounded_range(this->begin(), this->end());
  template <typename I>
    class bounded_range
    {
      static_assert(Weakly_incrementable<I>(), "");
      static_assert(Equality_comparable<I>(), "");
    public:
      using value_type = Value_type<I>;
      using difference_type = Distance_type<I>;

      // Initialize the bounded range so that both values are the same. The
      // range is initially empty.
      bounded_range() : first(), last(first) { }
    
      // Initialize the bounded range over [first, last).
      bounded_range(I first, I last)
        : first(first), last(last)
      { 
        assert(( is_bounded_range(first, last) ));
      }
      
      // Range
      I begin() const { return first; }
      I end() const { return last; }
      
    private:
      I first;
      I last;
    };



  // Iterator range
  // An iterator range defines a bounded range over a set of iterators. This is
  // to say that the elements of an iterator range are iterators. The range is
  // parameterized over the underlying iterator type and an action that 
  // describes how the range is iterated (increment by default).
  /*
  template <typename I, typename Act = increment_action<I>>
    class iterator_range
    {
      static_assert(Weakly_incrementable<I>(), "");
      static_assert(Equality_comparable<I>(), "");
      static_assert(Function<Act, I&>(), "");

    public:
      using iterator = counter<I, Act>;
      using value_type = Value_type<iterator>;
      using difference_type = Distance_type<iterator>;

      iterator_range(I first, I last)
        : first{first}, last{last}
      {
        assert(( is_bounded_range(first, last) ));
      }
    
      iterator begin() const { return first; }
      iterator end()   const { return last; }

    private:
      I first;
      I last;
    };
  */


  // Iter range
  // Returns a bounded range over the iterators in the range [first, last).
  // For example:
  //
  //    vector<int> v = {1, 2, 3};
  //    for(auto i : range_over(v)) cout << *i << ' ';
  //
  // prints "1 2 3". Each element in the range over v is vector iterator and
  // not an element in v.
  
  
  
  // Iter range
  // Returns an iterator range over the iterators in [first, last).
  /*
  template <typename I>
    inline iterator_range<I> iter_range(I first, I last)
    {
      assert(( is_bounded_range(first, last) ));

      return {first, last};
    }



  // Iter range (range)
  template <typename R>
    inline iterator_range<Iterator_type<R>> iter_range(R& range)
    {
      return {std::begin(range), std::end(range)};
    }



  // Iter range (const range)
  template <typename R>
    inline iterator_range<Iterator_type<const R>> iter_range(const R& range)
    {
      return {std::begin(range), std::end(range)};
    }
  */


  // Range
  // Returns a (right) half-open range [first, last) over the elements in that
  // range. For example:
  //
  //    for(auto i : range(0, 5)) count << i << ' ';
  //
  // prints "0 1 2 3 4". Similarly:
  //
  //    vector<int> v = {1, 2, 3}
  //    for(auto i : range(v.begin(), v.end())) cout << i << ' ';
  //
  // prints "1 2 3".
  //
  // This function tries to be clever about its uses. If the type of the first
  // and last arguments are integer types, then the result will range over the
  // integer values in that set. If the type is an iterator, then the result 
  // ranges over the elements in the underlying range.
  
  
  
  // Range (integral)
  /*
  template <typename I>
    inline Requires<Integral<I>(), iterator_range<I>> range(I first, I last)
    {
      assert(( is_bounded_range(first, last) ));

      return {first, last};
    }
  */

  
  // Range (iterator)
  template <typename I>
    inline Requires<Iterator<I>(), bounded_range<I>> range(I first, I last)
    {
      assert(( is_bounded_range(first, last) ));
      
      return {first, last};
    }



  // Closed range
  // Return a closed range over the elements of [first, last]. The behavor
  // of this constructor is analogous to that of the range function. It tries
  // to select an appropriate range based on the argument types provided.
  
  
  
  // Closed range (integral)
  /*
  template <typename I>
    inline Requires<Integral<I>(), iterator_range<I>> closed_range(I first, I last)
    {
      assume(( is_bounded_range(first, o_next(last)) ));
      return {first, ++last};
    }
  */



  // Closed range (iterator)
  template <typename I>
    inline Requires<Iterator<I>(), bounded_range<I>> closed_range(I first, I last)
    {
      assume(( is_bounded_range(first, o_next(last)) ));
      return {first, ++last};
    }

} // namespace origin


#endif