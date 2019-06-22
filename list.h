//
// Created by taras on 13.06.19.
//

#ifndef LIST__LIST_H_
#define LIST__LIST_H_

#include <iterator>
#include <utility>
#include <memory>
#include <assert.h>

template<typename>
struct node_base;
template<typename>
struct node_val;

template<typename T>
struct node_base {
  public:
  node_base *next;
  node_base *prev;

  typedef node_val<T> *value_node_ptr;

  template<typename> friend
  class list;

  node_base() : next(nullptr), prev(nullptr) {};

  node_base(node_base *next, node_base *prev)
      : next(next), prev(prev) {}

  ~node_base() = default;

  value_node_ptr to_value_node_ptr() {
      return static_cast<value_node_ptr>(this);
  }

  void swap(node_base &other) {
      bool loop1 = (this->next == this);
      bool loop2 = (other.next == &other);
      std::swap(next, other.next);
      std::swap(prev, other.prev);
      if (!loop2) {
          if (next != nullptr) {
              next->prev = this;
          }
          if (prev != nullptr) {
              prev->next = this;
          }
      } else {
          next = this;
          prev = this;
      }
      if (!loop1) {
          if (other.next != nullptr) {
              other.next->prev = &other;
          }
          if (other.prev != nullptr) {
              other.prev->next = &other;
          }
      } else {
          other.next = &other;
          other.prev = &other;
      }
  }
};

template<typename T>
struct node_val : node_base<T> {
  public:
  T val;

  typedef node_base<T> *link_node_ptr;

  template<typename> friend
  class list;

  node_val() = delete;

  node_val(link_node_ptr next, link_node_ptr prev, T const &elem)
      : node_base<T>(next, prev), val(elem) {};

  link_node_ptr to_link_node_ptr() {
      return static_cast<link_node_ptr>(this);
  }

};

template<typename T>
struct list_iterator {
  typedef T value_type;
  typedef T &reference;
  typedef std::ptrdiff_t difference_type;
  typedef T *pointer;
  typedef std::bidirectional_iterator_tag iterator_category;

  template<typename> friend
  class list;

  template<typename> friend
  class list_const_iterator;

  list_iterator() = default;

  list_iterator(list_iterator const &) = default;

  list_iterator &operator=(list_iterator const &) = default;

  reference operator*() const {
      return ptr_->to_value_node_ptr()->val;
  }

  pointer operator->() const {
      return &ptr_->to_value_node_ptr()->val;
  }

  list_iterator &operator++() {
      ptr_ = ptr_->next;
      return *this;
  }

  list_iterator operator++(int) {
      list_iterator res(*this);
      ptr_ = ptr_->next;
      return res;
  }

  list_iterator &operator--() {
      ptr_ = ptr_->prev;
      return *this;
  }

  list_iterator operator--(int) {
      list_iterator res(*this);
      ptr_ = ptr_->prev;
      return res;
  }

  friend bool operator==(list_iterator const &a, list_iterator const &b) {
      return a.ptr_ == b.ptr_;
  }

  friend bool operator!=(list_iterator const &a, list_iterator const &b) {
      return a.ptr_ != b.ptr_;
  }

  private:
  explicit list_iterator(node_base<T> *n) : ptr_(n) {};

  node_base<T> *ptr_ = nullptr;

};

template<typename T>
struct list_const_iterator {
  typedef T value_type;
  typedef T const &reference;
  typedef std::ptrdiff_t difference_type;
  typedef T const *pointer;
  typedef std::bidirectional_iterator_tag iterator_category;

  template<typename> friend
  class list;

  template<typename> friend
  class list_iterator;

  list_const_iterator() = default;

  list_const_iterator(list_const_iterator const &) = default;

  list_const_iterator(list_iterator<T> const &src) : ptr_(src.ptr_) {}

  list_const_iterator &operator=(list_const_iterator const &) = default;

  reference operator*() const {
      return ptr_->to_value_node_ptr()->val;
  }

  pointer operator->() const {
      return &ptr_->to_value_node_ptr()->val;
  }

  list_const_iterator &operator++() {
      ptr_ = ptr_->next;
      return *this;
  }

  list_const_iterator operator++(int) {
      list_const_iterator res(*this);
      ptr_ = ptr_->next;
      return res;
  }

  list_const_iterator &operator--() {
      ptr_ = ptr_->prev;
      return *this;
  }

  list_const_iterator operator--(int) {
      list_const_iterator res(*this);
      ptr_ = ptr_->prev;
      return res;
  }

  friend bool operator==(list_const_iterator const &a, list_const_iterator const &b) {
      return a.ptr_ == b.ptr_;
  }

  friend bool operator!=(list_const_iterator const &a, list_const_iterator const &b) {
      return a.ptr_ != b.ptr_;
  }

  private:
  explicit list_const_iterator(node_base<T> *n) : ptr_(n) {};

  node_base<T> *ptr_ = nullptr;
};

template<typename T>
class list {
  public:
  typedef T value_type;
  typedef T &reference;
  typedef T const &const_reference;
  typedef T const* pointer;
  private:
  typedef node_base<T> link_node;
  typedef node_val<T> value_node;
  typedef node_base<T> *link_node_ptr;
  typedef node_val<T> *value_node_ptr;

  link_node fake;

  void set_default_fake() {
      to_self_loop(&fake);
  }

  void to_self_loop(link_node_ptr node_ptr) {
      node_ptr->next = node_ptr;
      node_ptr->prev = node_ptr;
  }

  std::unique_ptr<link_node> copy_list(list const &other) {
      auto copy_fake = std::unique_ptr<link_node>(new link_node(nullptr, nullptr));
      link_node_ptr ptr_ = copy_fake.get();
      for (auto &x : other) {
          try {
              ptr_->next = new value_node(nullptr, ptr_, x);
              ptr_ = ptr_->next;
          } catch (...) {
              free_list(copy_fake.get());
              throw;
          }
      }
      ptr_->next = copy_fake.get();
      copy_fake.get()->prev = ptr_;
      return copy_fake;
  }

  // pre: ptr_ = fake node
  void free_list(link_node_ptr start_fake) {
      if (start_fake->prev != nullptr) {
          start_fake->prev->next = nullptr;
      }
      link_node_ptr ptr_ = start_fake->next;
      while (ptr_ != nullptr) {
          value_node_ptr del_ptr = ptr_->to_value_node_ptr();
          ptr_ = ptr_->next;
          delete del_ptr;
      }
      to_self_loop(start_fake);
  }

  public:
  using iterator = list_iterator<T>;
  using const_iterator = list_const_iterator<T>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  list() noexcept {
      set_default_fake();
  }

  // strong
  list(list const &other) : list() {
      auto ptr = copy_list(other); // strong
      fake.swap(*ptr.get());
  }

  // strong
  list &operator=(list const &other) {
      list(other).swap(*this);
      return *this;
  }

  ~list() noexcept(noexcept(std::declval<value_type>().~value_type())) {
      free_list(&fake);
  }

  void clear() noexcept(noexcept(std::declval<value_type>().~value_type())) {
      free_list(&fake);
  }

  bool empty() const noexcept {
      return &fake == fake.next;
  }

  void push_back(const_reference val) {
      insert(cend(), val);
  }

  void pop_back() {
      erase(std::prev(cend()));
  }

  reference back() {
      return *rbegin();
  }

  const_reference back() const {
      return *rbegin();
  }

  void push_front(const_reference val) {
      insert(cbegin(), val);
  }

  void pop_front() {
      erase(cbegin());
  }

  reference front() {
      return *begin();
  }

  const_reference front() const {
      return *begin();
  }

  iterator begin() {
      return iterator(fake.next);
  }

  const_iterator begin() const {
      return const_iterator(fake.next);
  }

  const_iterator cbegin() const {
      return const_iterator(fake.next);
  }

  iterator end() {
      return iterator(&fake);
  }

  const_iterator end() const {
      return const_iterator(const_cast<link_node_ptr>(&fake));
  }

  const_iterator cend() const {
      return const_iterator(const_cast<link_node_ptr>(&fake));
  }

  reverse_iterator rbegin() {
      return reverse_iterator(end());
  }

  const_reverse_iterator rbegin() const {
      return const_reverse_iterator(cend());
  }

  const_reverse_iterator crbegin() const {
      return const_reverse_iterator(cend());
  }

  reverse_iterator rend() {
      return reverse_iterator(begin());
  }

  const_reverse_iterator rend() const {
      return const_reverse_iterator(cbegin());
  }

  const_reverse_iterator crend() const {
      return const_reverse_iterator(cbegin());
  }

  // strong
  iterator insert(const_iterator pos, const_reference val) {
      assert(pos.ptr_->prev != nullptr);
      link_node_ptr insert_node = new value_node(pos.ptr_, pos.ptr_->prev, val);
      insert_node->prev->next = insert_node;
      insert_node->next->prev = insert_node;
      return iterator(insert_node);
  }

  iterator erase(const_iterator pos) noexcept {
      value_node_ptr delete_ptr = pos.ptr_->to_value_node_ptr();
      assert(delete_ptr->next != nullptr);
      delete_ptr->next->prev = delete_ptr->prev;
      delete_ptr->prev->next = delete_ptr->next;
      link_node_ptr next = delete_ptr->next;
      delete delete_ptr;
      return iterator(next);
  }

  iterator erase(const_iterator first, const_iterator last) noexcept {
      while (first != last) {
          erase(first++);
      }
      return iterator(last.ptr_);
  }

  void splice(const_iterator pos, list &other, const_iterator first, const_iterator last) noexcept {
      if (first == last) {
          return;
      }
      link_node_ptr p = pos.ptr_;
      link_node_ptr f = first.ptr_;
      link_node_ptr l = last.ptr_->prev;
      assert(f->prev != nullptr);
      assert(l->next != nullptr);
      assert(p->prev != nullptr);
      f->prev->next = l->next;
      l->next->prev = f->prev;
      p->prev->next = f;
      f->prev = p->prev;
      l->next = p;
      p->prev = l;
  }

  void swap(list &other) noexcept {
      fake.swap(other.fake);
  }
};

template<typename T>
void swap(list<T> &a, list<T> &b) noexcept {
    a.swap(b);
}

#endif //LIST__LIST_H_