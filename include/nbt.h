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

  bool Load(const char* buffer, size_t size);

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
