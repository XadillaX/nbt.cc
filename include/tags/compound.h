#ifndef INCLUDE_TAGS_COMPOUND_H_
#define INCLUDE_TAGS_COMPOUND_H_

#include "base_tag.h"
#include "nbt_version.h"

namespace nbtcc {
/**
 * \defgroup Tags
 * @brief The NBT tags.
 * @{
 */
/**
 * The compound tag. (for TagType::kTagCompound)
 */
class TagCompound : public BaseTag {
 public:
  /**
   * Get child tag by name.
   * @param name The name of the child tag.
   * @return The child tag. (`nullptr` if not found)
   */
  const std::shared_ptr<BaseTag> GetChild(const std::string& key) const;

  /**
   * Get child tag by name.
   * @param name The name of the child tag.
   * @return The child tag. (`nullptr` if not found)
   */
  std::shared_ptr<BaseTag> GetChild(const std::string& key);

  /**
   * Get child tag by name.
   * @param name The name of the child tag.
   * @param tag The child tag.
   * @param overwrite If `true`, overwrite the child tag if it exists.
   * @return `true` if the child tag is set.
   */
  bool SetChild(const std::string& key,
                std::shared_ptr<BaseTag> tag,
                bool overwrite = false);

  /**
   * Check if the child tag exists.
   * @param name The name of the child tag.
   * @return `true` if the child tag exists.
   */
  bool HasChild(const std::string& key) const;

  /**
   * Remove the child tag.
   * @param name The name of the child tag.
   * @return `true` if the child tag is removed.
   */
  bool EraseChild(const std::string& key);

  /**
   * Get all children's names.
   * @param vec The `vector` to store the names.
   */
  void GetChildKeys(std::vector<std::string>* vec) const;

  /**
   * Clear all children.
   */
  void Clear();

 public:
  /**
   * The constructor of the compound tag.
   */
  inline TagCompound() : BaseTag(TagType::kTagCompound) {}

  /**
   * The destructor of the compound tag.
   */
  virtual inline ~TagCompound() {}

  virtual inline size_t CalculateSize() const {
    size_t size = 0;
    for (auto& pair : *_value._value._compound) {
      // child type id for 1 byte, child name length for 2 bytes and child name
      // for (child name length) byte(s).
      size += sizeof(TagType) + sizeof(uint16_t);
      size += pair.first.length();

      // add the child body's length
      size += pair.second->CalculateSize();
    }

    // TAG_End
    size += sizeof(TagType);
    return size;
  }

  virtual inline size_t WriteToBuffer(char* buffer,
                                      size_t length,
                                      size_t offset) const {
    size_t written = 0;
    for (auto& pair : *_value._value._compound) {
      if (offset + written + sizeof(TagType) + sizeof(uint16_t) +
              pair.first.length() + pair.second->CalculateSize() >
          length) {
        return NBTCC_WRITE_ERROR;
      }

      buffer[offset + written] = static_cast<int8_t>(pair.second->type());
      written += sizeof(TagType);
      *reinterpret_cast<uint16_t*>(buffer + offset + written) =
          htobe16(pair.first.length());
      written += sizeof(uint16_t);
      memcpy(
          buffer + offset + written, pair.first.c_str(), pair.first.length());
      written += pair.first.length();
      written += pair.second->WriteToBuffer(buffer, length, offset + written);
    }

    if (offset + written + sizeof(TagType) > length) {
      return NBTCC_WRITE_ERROR;
    }

    buffer[offset + written] = static_cast<int8_t>(TagType::kTagEnd);
    written += sizeof(TagType);
    return written;
  }

 protected:
  virtual inline size_t ReadBodyFromBuffer(const char* buffer,
                                           size_t length,
                                           size_t offset) {
    PrintDebug("TagCompound::ReadBodyFromBuffer offset: %zu\n", offset);
    size_t next_offset = offset;
    BaseTag::read_level++;
    while (1) {
      size_t read_size = ReadNextTag(buffer, length, next_offset);
      if (read_size == NBTCC_READ_ERROR) {
        return NBTCC_READ_ERROR;
      }

      if (read_size == 0) {
        next_offset += sizeof(TagType);
        break;
      }

      next_offset += read_size;
    }
    BaseTag::read_level--;

    return next_offset - offset;
  }

 private:
  size_t ReadNextTag(const char* buffer, size_t length, size_t offset);
};
/**
 * @}
 */
}  // namespace nbtcc

#endif  // INCLUDE_TAGS_COMPOUND_H_
