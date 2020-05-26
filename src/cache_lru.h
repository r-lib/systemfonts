#ifndef SYSTEMFONT_LRU_CACHE
#define SYSTEMFONT_LRU_CACHE

#include <unordered_map>
#include <list>

template<typename key_t, typename value_t>
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
  
  bool add(const key_t key, value_t value, value_t& removed_entry) {
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
      removed_entry = _cache_map[last->first];
      _cache_map.erase(last->first);
      _cache_list.pop_back();
      return true;
    }
    return false;
  }
  
  bool get(const key_t key, value_t& value) {
    cache_map_it_t it = _cache_map.find(key);
    if (it == _cache_map.end()) {
      return false;
    }
    
    value = it->second->second;
    _cache_list.splice(_cache_list.begin(), _cache_list, it->second);
    
    return true;
  }
  
  bool exist(const key_t key) {
    return _cache_map.find(key) != _cache_map.end();
  }
  
  void remove(const key_t key) {
    cache_map_it_t it = _cache_map.find(key);
    if (it == _cache_map.end()) {
      return;
    }
    _cache_map.erase(it->first);
    _cache_list.erase(it);
  }
  
private:
  size_t _max_size;
  std::list<key_value_t> _cache_list;
  std::unordered_map<key_t, cache_list_it_t> _cache_map;
};

#endif
