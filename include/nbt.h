#ifndef INCLUDE_NBT_H_
#define INCLUDE_NBT_H_

#include "tags/compound.h"
#include "tags/list.h"
#include "tags/meta_array_tags.h"
#include "tags/meta_tags.h"
#include "tags/string.h"

namespace nbtcc {

class NBT {
 public:
  NBT() {}
  ~NBT() {}

  template <typename T>
  inline std::shared_ptr<T> GetFromRoot(const std::string& name) {
    return reinterpret_cast<T*>(_root[name]);
  }

  inline std::shared_ptr<BaseTag> GetFromRoot(const std::string& name) {
    return _root[name];
  }

  inline void RootKeys(std::vector<std::string>* keys) {
    keys->clear();
    for (auto& kv : _root) {
      keys->push_back(kv.first);
    }
  }

  inline bool AddToRoot(const std::string& name,
                        std::shared_ptr<BaseTag> tag,
                        bool overwrite = false) {
    if (_root.find(name) != _root.end() && !overwrite) {
      return false;
    }
    _root[name] = tag;
    return true;
  }

  inline bool RemoveFromRoot(const std::string& name) {
    if (_root.find(name) == _root.end()) {
      return false;
    }
    _root.erase(name);
    return true;
  }

  bool Load(const char* buffer, size_t size);
  size_t GetBufferSize() const;
  size_t Write(char* buffer, size_t buffer_length) const;

 private:
  struct ReturnedTag {
    std::shared_ptr<BaseTag> tag;
    size_t length;
  };
  static ReturnedTag GetNextTag(const char* buffer, size_t size, size_t offset);

 private:
  std::map<std::string, std::shared_ptr<BaseTag>> _root;
};

}  // namespace nbtcc

#endif  // INCLUDE_NBT_H_
