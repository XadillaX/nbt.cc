#ifndef INCLUDE_TAGS_COMPOUND_H_
#define INCLUDE_TAGS_COMPOUND_H_

#include "base_tag.h"

namespace nbtcc {

class TagCompound : public BaseTag {
 public:
  inline TagCompound() : BaseTag(TagType::kTagCompound) {}
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

}  // namespace nbtcc

#endif  // INCLUDE_TAGS_COMPOUND_H_
