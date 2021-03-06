#ifndef _iterator_tpl_h_
#define _iterator_tpl_h_

namespace iterator_tpl {

// Use this define to declare both:
// - `iterator`
// - `const_iterator`:
// As members of your class
#define SETUP_ITERATORS(C, T, S)  \
  SETUP_MUTABLE_ITERATOR(C, T, S) \
  SETUP_CONST_ITERATOR(C, T, S)

// Use this define to declare only `iterator`
#define SETUP_MUTABLE_ITERATOR(C, T, S)              \
  typedef iterator_tpl::iterator<C, T, S> iterator;  \
  iterator begin() { return iterator::begin(this); } \
  iterator end()   { return iterator::end(this);   }

// Use this define to declare only `const_iterator`
#define SETUP_CONST_ITERATOR(C, T, S)                                  \
  typedef iterator_tpl::const_iterator<C, T, S> const_iterator;        \
  const_iterator begin() const { return const_iterator::begin(this); } \
  const_iterator end()   const { return const_iterator::end(this);   }

// S should be the state struct used to forward iteration:
#define SETUP_REVERSE_ITERATORS(C, T, S)                            \
  struct S##_reversed : public S {                                  \
    inline void next (const C* ref) { S::prev(ref); }               \
    inline void prev (const C* ref) { S::next(ref); }               \
    inline void next (const C* ref, int n) { S::prev(ref, n); }     \
    inline void prev (const C* ref, int n) { S::next(ref, n); }     \
    inline void begin(const C* ref) { S::end(ref); S::prev(ref); }  \
    inline void end  (const C* ref) { S::begin(ref); S::prev(ref); }\
    inline void find (const C* ref, std::string& key) { S::find(ref, key); }  \
  };                                                                \
  SETUP_MUTABLE_RITERATOR(C, T, S)                                  \
  SETUP_CONST_RITERATOR(C, T, S)

#define SETUP_MUTABLE_RITERATOR(C, T, S) \
  typedef iterator_tpl::iterator<C, T, S##_reversed > reverse_iterator; \
  reverse_iterator rbegin() { return reverse_iterator::begin(this); }   \
  reverse_iterator rend()   { return reverse_iterator::end(this); }     \

#define SETUP_CONST_RITERATOR(C, T, S)                                              \
  typedef iterator_tpl::const_iterator<C, T, S##_reversed > const_reverse_iterator; \
  const_reverse_iterator rbegin() const {                                           \
    return const_reverse_iterator::begin(this);                                     \
  }                                                                                 \
  const_reverse_iterator rend() const {                                             \
    return const_reverse_iterator::end(this);                                       \
  }

#define STL_TYPEDEFS(T)                    \
  typedef std::ptrdiff_t difference_type;  \
  typedef size_t size_type;                \
  typedef T value_type;                    \
  typedef T* pointer;                      \
  typedef const T* const_pointer;          \
  typedef T& reference;                    \
  typedef const T& const_reference

// Forward declaration of const_iterator:
template <class C, typename T, class S>
struct const_iterator;

/* * * * * MUTABLE ITERATOR TEMPLATE: * * * * */

// C - The container type
// T - The content type
// S - The state keeping structure
template <class C, typename T, class S>
// The non-specialized version is used for T=rvalue:
struct iterator {
  // Keeps a reference to the container:
  C* ref;

  // User defined struct to describe the iterator state:
  // This struct should provide the functions listed below,
  // however, note that some of them are optional
  S state;

  // Set iterator to next() state:
  void next()  { state.next(ref);  }
  // Set iterator to next() state:
  void next(int n)  { state.next(ref, n);  }
  // Initialize iterator to first state:
  void begin() { state.begin(ref); }
  // Initialize iterator to end state:
  void end()   { state.end(ref);   }
  // Initialize iterator to key equile state:
  void find(const C* ref, const std::string& key) { state.find(ref, key); }
  // Returns current `value`
  T get() { return state.get(ref); }
  // Return true if `state != s`:
  bool cmp(const S& s) const { return state.cmp(s); }

  // Optional function for reverse iteration:
  void prev() { state.prev(ref); }
  // Optional function for reverse iteration:
  void prev(int n) { state.prev(ref, n); }

 public:
  static iterator begin(C* ref) {
    iterator it(ref);
    it.begin();
    return it;
  }
  static iterator end(C* ref) {
    iterator it(ref);
    it.end();
    return it;
  }
  static iterator find(C* ref, const std::string& key) {
    iterator it(ref);
    it.find(key);
    return it;
  }

 protected:
  iterator(C* ref) : ref(ref) {}

 public:
  // Note: Instances build with this constructor should
  // be used only after copy-assigning from other iterator!
  iterator() {}

 public:
  T operator*() { return get(); }
  iterator& operator++() { next(); return *this; }
  iterator operator++(int) { iterator temp(*this); next(); return temp; }
  iterator& operator--() { prev(); return *this; }
  iterator operator--(int) { iterator temp(*this); prev(); return temp; }
  iterator& operator+=(int n) { next(n); return *this; }
  iterator& operator-=(int n) { prev(n); return *this; }
  bool operator!=(const iterator& other) const {
    return ref != other.ref || cmp(other.state);
  }
  bool operator==(const iterator& other) const {
    return !operator!=(other);
  }

  friend struct iterator_tpl::const_iterator<C,T,S>;

  // Comparisons between const and normal iterators:
  bool operator!=(const const_iterator<C,T,S>& other) const {
    return ref != other.ref || cmd(other.state);
  }
  bool operator==(const const_iterator<C,T,S>& other) const {
    return !operator!=(other);
  }
};

template <class C, typename T, class S>
// This specialization is used for iterators to reference types:
struct iterator<C,T&,S> {
  // Keeps a reference to the container:
  C* ref;

  // User defined struct to describe the iterator state:
  // This struct should provide the functions listed below,
  // however, note that some of them are optional
  S state;

  // Set iterator to next() state:
  void next()  { state.next(ref);  }
  // Set iterator to next() state:
  void next(int n)  { state.next(ref, n);  }
  // Initialize iterator to first state:
  void begin() { state.begin(ref); }
  // Initialize iterator to end state:
  void end()   { state.end(ref);   }
  // Initialize iterator to key equile state:
  void find(const std::string& key) { state.find(ref, key); }
  // Returns current `value`
  T& get() { return state.get(ref); }
  // Return true if `state != s`:
  bool cmp(const S& s) const { return state.cmp(s); }

  // Optional function for reverse iteration:
  void prev() { state.prev(ref); }
  // Optional function for reverse iteration:
  void prev(int n) { state.prev(ref, n); }

 public:
  static iterator begin(C* ref) {
    iterator it(ref);
    it.begin();
    return it;
  }
  static iterator end(C* ref) {
    iterator it(ref);
    it.end();
    return it;
  }
  static iterator find(C* ref, const std::string& key) {
    iterator it(ref);
    it.find(key);
    return it;
  }

 protected:
  iterator(C* ref) : ref(ref) {}

 public:
  // Note: Instances build with this constructor should
  // be used only after copy-assigning from other iterator!
  iterator() {}

 public:
  T& operator*()  { return  get(); }
  T* operator->() { return &get(); }
  iterator& operator++() { next(); return *this; }
  iterator operator++(int) { iterator temp(*this); next(); return temp; }
  iterator& operator--() { prev(); return *this; }
  iterator operator--(int) { iterator temp(*this); prev(); return temp; }
  iterator& operator+=(int n) { next(n); return *this; }
  iterator& operator-=(int n) { prev(n); return *this; }
  bool operator!=(const iterator& other) const {
    return ref != other.ref || cmp(other.state);
  }
  bool operator==(const iterator& other) const {
    return !operator!=(other);
  }

  friend struct iterator_tpl::const_iterator<C,T&,S>;

  // Comparisons between const and normal iterators:
  bool operator!=(const const_iterator<C,T&,S>& other) const {
    return ref != other.ref || cmd(other.state);
  }
  bool operator==(const const_iterator<C,T&,S>& other) const {
    return !operator!=(other);
  }
};

/* * * * * CONST ITERATOR TEMPLATE: * * * * */

// C - The container type
// T - The content type
// S - The state keeping structure
template <class C, typename T, class S>
// The non-specialized version is used for T=rvalue:
struct const_iterator {
  // Keeps a reference to the container:
  const C* ref;

  // User defined struct to describe the iterator state:
  // This struct should provide the functions listed below,
  // however, note that some of them are optional
  S state;

  // Set iterator to next() state:
  void next()  { state.next(ref);  }
  // Set iterator to next() state:
  void next(int n)  { state.next(ref, n);  }
  // Initialize iterator to first state:
  void begin() { state.begin(ref); }
  // Initialize iterator to end state:
  void end()   { state.end(ref);   }
  // Returns current `value`
  const T get() { return state.get(ref); }
  // Return true if `state != s`:
  bool cmp(const S& s) const { return state.cmp(s); }

  // Optional function for reverse iteration:
  void prev() { state.prev(ref); }
  // Optional function for reverse iteration:
  void prev(int n) { state.prev(ref, n); }

 public:
  static const_iterator begin(const C* ref) {
    const_iterator it(ref);
    it.begin();
    return it;
  }
  static const_iterator end(const C* ref) {
    const_iterator it(ref);
    it.end();
    return it;
  }
  static const_iterator find(const C* ref, const std::string& key) {
    const_iterator it(ref);
    it.find(key);
    return it;
  }

 protected:
  const_iterator(const C* ref) : ref(ref) {}

 public:
  // Note: Instances build with this constructor should
  // be used only after copy-assigning from other iterator!
  const_iterator() {}

  // To make possible copy-construct non-const iterators:
  const_iterator(const iterator<C,T,S>& other) : ref(other.ref) {
    state = other.state;
  }

 public:
  const T operator*() { return get(); }
  const_iterator& operator++() { next(); return *this; }
  const_iterator operator++(int) { const_iterator temp(*this); next(); return temp;  }
  const_iterator& operator--() { prev(); return *this; }
  const_iterator operator--(int) { const_iterator temp(*this); prev(); return temp;  }
  const_iterator& operator+=(int n) { next(n); return *this; }
  const_iterator& operator-=(int n) { prev(n); return *this; }
  bool operator!=(const const_iterator& other) const {
    return ref != other.ref || cmp(other.state);
  }
  bool operator==(const const_iterator& other) const {
    return !operator!=(other);
  }
  const_iterator& operator=(const iterator<C,T,S>& other) {
    ref = other.ref;
    state = other.state;
    return *this;
  }

  friend struct iterator_tpl::iterator<C,T,S>;

  // Comparisons between const and normal iterators:
  bool operator!=(const iterator<C,T,S>& other) const {
    return ref != other.ref || cmp(other.state);
  }
  bool operator==(const iterator<C,T,S>& other) const {
    return !operator!=(other);
  }
};

// This specialization is used for iterators to reference types:
template <class C, typename T, class S>
struct const_iterator<C,T&,S> {
  // Keeps a reference to the container:
  const C* ref;

  // User defined struct to describe the iterator state:
  // This struct should provide the functions listed below,
  // however, note that some of them are optional
  S state;

  // Set iterator to next() state:
  void next()  { state.next(ref);  }
  // Set iterator to next() state:
  void next(int n)  { state.next(ref, n);  }
  // Initialize iterator to first state:
  void begin() { state.begin(ref); }
  // Initialize iterator to end state:
  void end()   { state.end(ref);   }
  // Returns current `value`
  const T& get() { return state.get(ref); }
  // Return true if `state != s`:
  bool cmp(const S& s) const { return state.cmp(s); }

  // Optional function for reverse iteration:
  void prev() { state.prev(ref); }
  // Optional function for reverse iteration:
  void prev(int n) { state.prev(ref, n); }

 public:
  static const_iterator begin(const C* ref) {
    const_iterator it(ref);
    it.begin();
    return it;
  }
  static const_iterator end(const C* ref) {
    const_iterator it(ref);
    it.end();
    return it;
  }
  static const_iterator find(const C* ref, const std::string& key) {
    const_iterator it(ref);
    it.find(key);
    return it;
  }

 protected:
  const_iterator(const C* ref) : ref(ref) {}

 public:
  // Note: Instances build with this constructor should
  // be used only after copy-assigning from other iterator!
  const_iterator() {}

  // To make possible copy-construct non-const iterators:
  const_iterator(const iterator<C,T&,S>& other) : ref(other.ref) {
    state = other.state;
  }

 public:
  const T& operator*()  { return  get(); }
  const T* operator->() { return &get(); }
  const_iterator& operator++() { next(); return *this; }
  const_iterator operator++(int) { const_iterator temp(*this); next(); return temp; }
  const_iterator& operator--() { prev(); return *this; }
  const_iterator operator--(int) { const_iterator temp(*this); prev(); return temp; }
  const_iterator& operator+=(int n) { next(n); return *this; }
  const_iterator& operator-=(int n) { prev(n); return *this; }
  bool operator!=(const const_iterator& other) const {
    return ref != other.ref || cmp(other.state);
  }
  bool operator==(const const_iterator& other) const {
    return !operator!=(other);
  }
  const_iterator& operator=(const iterator<C,T&,S>& other) {
    ref = other.ref;
    state = other.state;
    return *this;
  }

  friend struct iterator_tpl::iterator<C,T&,S>;

  // Comparisons between const and normal iterators:
  bool operator!=(const iterator<C,T&,S>& other) const {
    return ref != other.ref || cmp(other.state);
  }
  bool operator==(const iterator<C,T&,S>& other) const {
    return !operator!=(other);
  }
};

}  // namespace iterator_tpl

#endif
