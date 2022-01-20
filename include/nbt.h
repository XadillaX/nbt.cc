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
  inline T* Root() {
    return reinterpret_cast<T*>(_root.get());
  }

  inline TagCompound* CompoundRoot() { return Root<TagCompound>(); }

  bool Load(const char* buffer, size_t size);

 private:
  std::unique_ptr<BaseTag> _root;
};

}  // namespace nbtcc

#endif  // INCLUDE_NBT_H_
