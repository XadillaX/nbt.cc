#ifndef INCLUDE_TAGS_STRING_H_
#define INCLUDE_TAGS_STRING_H_

#include "base_tag.h"
#include "nbt_version.h"

namespace nbtcc {
/**
 * \defgroup Tags
 * @brief The NBT tags.
 * @{
 */
/**
 * The string tag. (for TagType::kTagString)
 */
class TagString : public BaseTag {
 public:
  /**
   * Get the string value of the tag.
   * @return The string value.
   */
  inline std::string ToString() const { return _value.StringValue(); }

  /**
   * Set the string value of the tag.
   * @param value The string value.
   */
  inline void SetString(const std::string& value) {
    _value.SetStringValue(value);
  }

 public:
  /**
   * The constructor of the tag.
   */
  inline TagString() : BaseTag(TagType::kTagString) {}

  /**
   * The constructor of the tag.
   * @param value The string value.
   */
  explicit inline TagString(const std::string& value)
      : BaseTag(TagType::kTagString) {
    _value.SetStringValue(value);
  }

  /**
   * The destructor of the tag.
   */
  virtual inline ~TagString() {}

  virtual inline size_t CalculateSize() const {
    return sizeof(uint16_t) + sizeof(char) * _value.StringValue().length();
  }

  virtual inline size_t WriteToBuffer(char* buffer,
                                      size_t length,
                                      size_t offset) const {
    if (offset + CalculateSize() > length) {
      return NBTCC_WRITE_ERROR;
    }

    const std::string& str = _value.StringValue();

    uint16_t length_value = htobe16(str.length());
    memcpy(buffer + offset, &length_value, sizeof(uint16_t));
    offset += sizeof(uint16_t);

    memcpy(buffer + offset, str.c_str(), sizeof(char) * str.length());
    offset += sizeof(char) * str.length();

    return sizeof(uint16_t) + sizeof(char) * str.length();
  }

 protected:
  virtual inline size_t ReadBodyFromBuffer(const char* buffer,
                                           size_t length,
                                           size_t offset) {
    if (offset + sizeof(uint16_t) > length) {
      return NBTCC_READ_ERROR;
    }

    PrintDebug("TagString::ReadBodyFromBuffer offset: %zu\n", offset);
    uint16_t length_value;
    memcpy(&length_value, buffer + offset, sizeof(uint16_t));
    length_value = be16toh(length_value);
    offset += sizeof(uint16_t);

    PrintDebug(" > length_value: %d\n", length_value);

    if (offset + sizeof(char) * length_value > length) {
      return NBTCC_READ_ERROR;
    }

    std::string str(buffer + offset, length_value);
    _value.SetStringValue(str);
    PrintDebug(" > str: %s\n", str.c_str());

    return sizeof(uint16_t) + sizeof(char) * length_value;
  }
};
/**
 * @}
 */
}  // namespace nbtcc

#endif  // INCLUDE_TAGS_STRING_H_
