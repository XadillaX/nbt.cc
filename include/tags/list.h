#ifndef INCLUDE_TAGS_LIST_H_
#define INCLUDE_TAGS_LIST_H_

#include "base_tag.h"
#include "nbt_version.h"

namespace nbtcc {
/**
 * \defgroup Tags
 * @brief The NBT tags.
 * @{
 */
/**
 * The list tag. (for TagType::kTagList)
 * @note All children tags must be of the same type.
 */
class TagList : public BaseTag {
 public:
  /**
   * Get child via index.
   * @param index The index.
   * @return The child. (`nullptr` if index is out of range)
   */
  const std::shared_ptr<BaseTag> GetChild(size_t index) const;

  /**
   * Get child via index.
   * @param index The index.
   * @return The child. (`nullptr` if index is out of range)
   */
  std::shared_ptr<BaseTag> GetChild(size_t index);

  /**
   * Set child via index.
   * @param index The index.
   * @param tag The child.
   * @return `true` if success.
   */
  bool SetChild(size_t index, std::shared_ptr<BaseTag> tag);

  /**
   * Remove child via index.
   * @param index The index.
   * @return `true` if success.
   * @note The next children will be moved to the left.
   */
  bool EraseChild(size_t index);

  /**
   * Clear all children.
   */
  void Clear();

  /**
   * Insert a child at `index`.
   * @param index The index.
   * @param tag The child.
   * @return `true` if success.
   */
  bool Insert(size_t index, std::shared_ptr<BaseTag> tag);

  /**
   * Push a child to the end.
   * @param tag The child.
   * @return `true` if success.
   * @note This is an alias of `Insert(count() - 1, std::shared_ptr<BaseTag>)`.
   */
  bool PushBack(std::shared_ptr<BaseTag> tag);

  /**
   * Pop a child from the end.
   * @return `true` if success.
   * @note This is an alias of `EraseChild(count() - 1)`.
   */
  bool PopBack();

  /**
   * Push a child to the front.
   * @param tag The child.
   * @return `true` if success.
   * @note This is the same as `Insert(0, tag)`.
   */
  bool PushFront(std::shared_ptr<BaseTag> tag);

  /**
   * Pop a child from the front.
   * @return `true` if success.
   * @note This is the same as `Erase(0)`.
   */
  bool PopFront();

 public:
  /**
   * Constructor of the list tag.
   */
  inline TagList()
      : BaseTag(TagType::kTagList), _child_type(TagType::kTagEnd) {}

  /**
   * Constructor of the list tag.
   * @param child_type The child type.
   */
  explicit inline TagList(TagType child_type)
      : BaseTag(TagType::kTagList), _child_type(child_type) {}

  /**
   * Destructor of the list tag.
   */
  virtual inline ~TagList() {}

  virtual inline size_t CalculateSize() const {
    // First byte is the type of the list, then followed by the length (4 bytes)
    // of the list.
    size_t size = sizeof(TagType) + sizeof(int32_t);

    const std::vector<std::shared_ptr<BaseTag>>& tags = *_value._value._list;
    if (tags.empty()) {
      return size;
    }

    for (auto it = tags.begin(); it != tags.end(); ++it) {
      size += (*it)->CalculateSize();
    }
    return size;
  }

  virtual inline size_t WriteToBuffer(char* buffer,
                                      size_t length,
                                      size_t offset) const {
    size_t total_size = CalculateSize();
    if (offset + total_size > length) {
      return NBTCC_WRITE_ERROR;
    }

    const std::vector<std::shared_ptr<BaseTag>>& tags = *_value._value._list;
    if (tags.empty()) {
      memset(buffer + offset, 0, total_size);
      return total_size;
    }

    // Write the type of the list.
    buffer[offset] = static_cast<char>(_child_type);
    offset += sizeof(TagType);

    // Write the length of the list.
    uint32_t length_value = htobe32(static_cast<uint32_t>(tags.size()));
    memcpy(buffer + offset, &length_value, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    // Write the tags.
    for (auto it = tags.begin(); it != tags.end(); it++) {
      size_t tag_size = (*it)->WriteToBuffer(buffer, length, offset);
      if (tag_size == NBTCC_WRITE_ERROR) {
        return NBTCC_WRITE_ERROR;
      }
      offset += tag_size;
    }

    return total_size;
  }

  /**
   * Get the child type.
   */
  inline TagType child_type() const { return _child_type; }

 protected:
  virtual size_t ReadBodyFromBuffer(const char* buffer,
                                    size_t length,
                                    size_t offset) {
    if (offset + sizeof(TagType) + sizeof(uint32_t) > length) {
      return NBTCC_READ_ERROR;
    }

    // Read the type of the list.
    uint8_t type = static_cast<uint8_t>(buffer[offset]);
    if (type >= TagType::kTagMax) {
      return NBTCC_READ_ERROR;
    }
    _child_type = static_cast<TagType>(type);

    // Read the length of the list.
    uint32_t length_value;
    memcpy(&length_value, buffer + offset + sizeof(TagType), sizeof(uint32_t));
    length_value = be32toh(length_value);
    if (_child_type == TagType::kTagEnd) {
      if (length_value != 1) {
        return NBTCC_READ_ERROR;
      }

      return sizeof(TagType) + sizeof(uint32_t);
    }

    size_t size = sizeof(TagType) + sizeof(uint32_t);
    offset += size;
    std::vector<std::shared_ptr<BaseTag>>& tags = *_value._value._list;
    tags.resize(length_value);
    for (size_t i = 0; i < length_value; i++) {
      size_t read_size = ReadNextTag(buffer, length, offset, i);
      if (read_size == NBTCC_READ_ERROR) {
        return NBTCC_READ_ERROR;
      }
      offset += read_size;
      size += read_size;
    }

    return size;
  }

  /**
   * Read the next child tag from a buffer.
   * @param buffer The buffer.
   * @param length The length of the buffer.
   * @param offset The offset of the buffer.
   * @param idx The index of the child tag.
   * @return The size of read.
   */
  size_t ReadNextTag(const char* buffer,
                     size_t length,
                     size_t offset,
                     size_t idx);

 private:
  TagType _child_type;
};
/**
 * @}
 */
}  // namespace nbtcc

#endif  // INCLUDE_TAGS_LIST_H_
