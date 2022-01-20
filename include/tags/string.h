#ifndef INCLUDE_TAGS_STRING_H_
#define INCLUDE_TAGS_STRING_H_

#include "base_tag.h"

namespace nbtcc {

class TagString : public BaseTag {
 public:
  inline TagString() : BaseTag(TagType::kTagString) {}
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

    uint16_t length_value = str.length();
    memcpy(buffer + offset, &length_value, sizeof(uint16_t));
    offset += sizeof(uint16_t);

    memcpy(buffer + offset, str.c_str(), sizeof(char) * str.length());
    offset += sizeof(char) * str.length();

    return offset;
  }

 protected:
  virtual inline size_t ReadBodyFromBuffer(const char* buffer,
                                           size_t length,
                                           size_t offset) {
    if (offset + sizeof(uint16_t) > length) {
      return NBTCC_READ_ERROR;
    }

    uint16_t length_value;
    memcpy(&length_value, buffer + offset, sizeof(uint16_t));
    offset += sizeof(uint16_t);

    if (offset + sizeof(char) * length_value > length) {
      return NBTCC_READ_ERROR;
    }

    std::string str(buffer + offset, length_value);
    _value.SetStringValue(str);

    return offset + sizeof(char) * length_value;
  }
};

}  // namespace nbtcc

#endif  // INCLUDE_TAGS_STRING_H_
