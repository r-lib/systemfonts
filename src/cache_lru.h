#ifndef SYSTEMFONT_LRU_CACHE
#define SYSTEMFONT_LRU_CACHE

#include <unordered_map>
#include <list>

template<typename key_t, typename value_t, typename key_h = std::hash<key_t>, typename key_eq = std::equal_to<key_t> >
class LRU_Cache {
  typedef typename std::pair<key_t, value_t> key_value_t;
  typedef typename std::list<key_value_t>::iterator cache_list_it_t;
  typedef typename std::unordered_map<key_t, cache_list_it_t>::iterator cache_map_it_t;
  
public:
  LRU_Cache() :
  _max_size(32) {
    
  }
  LRU_Cache(size_t max_size) :
  _max_size(max_size) {
    
  }
  
  ~LRU_Cache() {
    clear();
  }
  
  // Add a key-value pair, potentially passing a removed key and value back 
  // through the removed_key and removed_value argument. Returns true if a value 
  // was removed and false otherwise
  bool add(const key_t key, value_t value, key_t& removed_key, value_t& removed_value) {
    cache_map_it_t it = _cache_map.find(key);
    _cache_list.push_front(key_value_t(key, value));
    if (it != _cache_map.end()) {
      _cache_list.erase(it->second);
      _cache_map.erase(it);
    }
    _cache_map[key] = value;
    
    if (_cache_map.size() > _max_size) {
      cache_list_it_t last = _cache_list.end();
      last--;
      removed_key = last->first;
      removed_value = last->second;
      _cache_map.erase(last->first);
      _cache_list.pop_back();
      return true;
    }
    return false;
  }
  
  // Add a key-value pair, potentially passing a value back through the 
  // removed_value argument. Returns true if a value was removed and false 
  // otherwise
  bool add(const key_t key, value_t value, value_t removed_value) {
    key_t removed_key;
    return add(key, value, removed_key, removed_value);
  }
  
  // Add a key-value pair, potentially passing a key back through the 
  // removed_key argument. Returns true if a value was removed and false 
  // otherwise. Will destroy the value
  bool add(const key_t key, value_t value, key_t removed_key) {
    value_t removed_value;
    bool overflow = add(key, value, removed_key, removed_value);
    if (overflow) {
      value_dtor(removed_value);
    }
    return overflow;
  }
  
  // Add a key-value pair, automatically destroying any removed value
  void add(const key_t key, value_t value) {
    value_t removed_value;
    key_t removed_key;
    if (add(key, value, removed_key, removed_value)) {
      value_dtor(removed_value);
    }
  }
  
  // Retrieve a value based on a key, returning true if a value was found. Will
  // move the key-value pair to the top of the list
  bool get(const key_t key, value_t& value) {
    cache_map_it_t it = _cache_map.find(key);
    if (it == _cache_map.end()) {
      return false;
    }
    
    value = it->second->second;
    _cache_list.splice(_cache_list.begin(), _cache_list, it->second);
    
    return true;
  }
  
  // Retrieve a value based on a key without bumping the pair to the top of the
  // list
  bool steal(const key_t key, value_t& value) {
    cache_map_it_t it = _cache_map.find(key);
    if (it == _cache_map.end()) {
      return false;
    }
    
    value = it->second->second;
    return true;
  }
  
  // Check for the existence of a key-value pair
  bool exist(const key_t key) {
    return _cache_map.find(key) != _cache_map.end();
  }
  
  // Remove a key-value pair, destroying the value
  void remove(const key_t key) {
    cache_map_it_t it = _cache_map.find(key);
    if (it == _cache_map.end()) {
      return;
    }
    value_dtor(_cache_map[it->first]);
    _cache_map.erase(it->first);
    _cache_list.erase(it);
  }
  
  // Clear the cache, destroying all values with it
  void clear() {
    for (cache_list_it_t it = _cache_list.begin(); it != _cache_list.end(); ++it) {
      value_dtor(it->second);
    }
    _cache_list.clear();
    _cache_map.clear();
  }
  
private:
  size_t _max_size;
  std::list<key_value_t> _cache_list;
  std::unordered_map<key_t, cache_list_it_t, key_h, key_eq> _cache_map;
  
  // Should be overridden for children with value types that needs special
  // dtor handling
  virtual void value_dtor(value_t& value) {
    // Allow children to destroy values properly
  }
};

#endif
