#ifndef INCLUDE_NBT_H_
#define INCLUDE_NBT_H_

#include "nbt_version.h"
#include "tags/compound.h"
#include "tags/list.h"
#include "tags/meta_array_tags.h"
#include "tags/meta_tags.h"
#include "tags/string.h"

namespace nbtcc {

/**
 * \defgroup NBT Entity
 * @brief The entity of NBT.
 * @{
 */
/**
 * The NBT entity class (without compressed).
 */
class NBT {
 public:
  /**
   * The NBT constructor.
   */
  NBT() {}

  /**
   * The NBT destructor.
   */
  ~NBT() {}

  /**
   * Get a Tag (typename T) object from current NBT entity via tag name.
   * @param name The tag name.
   * @return The tag shared pointer. (nullptr if not found)
   */
  template <typename T>
  inline std::shared_ptr<T> GetFromRoot(const std::string& name) {
    return reinterpret_cast<T*>(_root[name]);
  }

  /**
   * Get the ancester class BaseTag of a Tag object from current NBT entity via
   * tag name.
   * @param name The tag name.
   * @return The tag shared pointer. (nullptr if not found)
   */
  inline std::shared_ptr<BaseTag> GetFromRoot(const std::string& name) {
    return _root[name];
  }

  /**
   * Get the keys of all root tags in current NBT entity.
   * @param keys The keys that should be filled by this method.
   */
  inline void RootKeys(std::vector<std::string>* keys) {
    keys->clear();
    for (auto& kv : _root) {
      keys->push_back(kv.first);
    }
  }

  /**
   * Add a tag to current NBT entity.
   * @param name The tag name.
   * @param tag The tag shared pointer.
   * @param overwrite If true, overwrite the tag if it already exists.
   * @return If the tag is added successfully.
   */
  inline bool AddToRoot(const std::string& name,
                        std::shared_ptr<BaseTag> tag,
                        bool overwrite = false) {
    if (_root.find(name) != _root.end() && !overwrite) {
      return false;
    }
    _root[name] = tag;
    return true;
  }

  /**
   * Remove a tag from current NBT entity.
   * @param name The tag name.
   * @return If the tag is removed successfully.
   */
  inline bool RemoveFromRoot(const std::string& name) {
    if (_root.find(name) == _root.end()) {
      return false;
    }
    _root.erase(name);
    return true;
  }

  /**
   * Load an NBT format buffer and fill it into current NBT entity.
   * @param buffer The NBT format buffer.
   * @param size The buffer size.
   * @return If the buffer is loaded successfully.
   */
  bool Load(const char* buffer, size_t size);

  /**
   * Get the NBT format buffer size of current NBT entity.
   * @return The NBT format buffer size.
   */
  size_t GetBufferSize() const;

  /**
   * Write the NBT format buffer of current NBT entity into a buffer.
   * @param buffer The buffer that should be filled by this method.
   * @param buffer_length The buffer size.
   * @return If the buffer is written successfully.
   */
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
/**
 * @}
 */

}  // namespace nbtcc

#endif  // INCLUDE_NBT_H_
