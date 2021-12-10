// Copyright 2019-2021 Lawrence Livermore National Security, LLC and other YGM
// Project Developers. See the top-level COPYRIGHT file for details.
//
// SPDX-License-Identifier: MIT

#pragma once

#include <ygm/container/detail/maptrix_impl.hpp>
namespace ygm::container {

/* Find out if you need to change the 
 * name of the row/col variables: key_i, key_j? */
template <typename Key, typename Value, 
          typename Partitioner = detail::hash_partitioner<Key>, 
          typename Compare     = std::less<Key>,
          class Alloc          = std::allocator<std::pair<const Key, Value>>>
class maptrix {
 public:
  using key_type    = Key;
  using value_type  = Value;
  using self_type   = maptrix<key_type, value_type, Partitioner, Compare, Alloc>;
  using map_type    = ygm::container::map<key_type, value_type>;
  //using map_type    = ygm::container::assoc_vector<key_type, value_type>;
  using impl_type   =
      detail::maptrix_impl<key_type, value_type, Partitioner, Compare, Alloc>;

  maptrix() = delete;

  maptrix(ygm::comm& comm) : m_impl(comm) {}

  maptrix(ygm::comm& comm, const value_type& dv) : m_impl(comm, dv) {}

  maptrix(const self_type& rhs) : m_impl(rhs.m_impl) {}

  ygm::comm& comm() { return m_impl.comm(); }

  /* async_insert accepts a directed graph. */
  void async_insert(const key_type& row, const key_type& col, const value_type& value) {
    m_impl.async_insert(row, col, value);
  }

  template <typename Function>
  void for_all(Function fn) {
    m_impl.for_all(fn);
  }

  template <typename Function>
  void for_all_row(Function fn) {
    m_impl.for_all_row(fn);
  }

  template <typename Function>
  void for_all_col(Function fn) {
    m_impl.for_all_col(fn);
  }

  template <typename Visitor, typename... VisitorArgs>
  void async_visit_if_exists(const key_type& row, const key_type& col, Visitor visitor,
                             const VisitorArgs&... args) {
    m_impl.async_visit_if_exists(row, col, visitor,
                                 std::forward<const VisitorArgs>(args)...);
  }

  template <typename Visitor, typename... VisitorArgs>
  void async_visit_col_mutate(const key_type& col, Visitor visitor,
                             const VisitorArgs&... args) {
    m_impl.async_visit_col_mutate(col, visitor, std::forward<const VisitorArgs>(args)...);
  }

  template <typename Visitor, typename... VisitorArgs>
  void async_visit_col_const(const key_type& col, Visitor visitor,
                             const VisitorArgs&... args) {
    m_impl.async_visit_col_const(col, visitor, std::forward<const VisitorArgs>(args)...);
  }

  template <typename Visitor, typename... VisitorArgs>
  void async_visit_or_insert(const key_type& row, const key_type& col, const value_type &value, 
                                Visitor visitor, const VisitorArgs&... args) {
    std::cout << "Reached outer container." << std::endl;
    m_impl.async_visit_or_insert(row, col, value, visitor, std::forward<const VisitorArgs>(args)...); 
  }

  void clear() { m_impl.clear(); }

  /* Use this if you want to interact with more that one containers. */
  typename ygm::ygm_ptr<impl_type> get_ygm_ptr() const {
    return m_impl.get_ygm_ptr();
  }

  void swap(self_type &s) {
    m_impl.swap(s);
  }

  /*****************************************************************************************/
  /*                                     TO BE IMPLEMENTED                                 */
  /*****************************************************************************************/

  #ifdef api_creation
  void async_erase(const key_type& row, const key_type& col) { m_impl.async_erase(row, col); }
  const value_type& default_value() const { return m_impl.default_value(); }
  #endif

 private:
  impl_type m_impl;
};
}  // namespace ygm::container

#include <ygm/container/detail/algorithms/spmv.hpp>
