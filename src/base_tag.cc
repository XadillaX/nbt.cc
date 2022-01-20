#include "base_tag.h"

namespace nbtcc {

using std::string;

#define TAG_TYPE_OFFSET (1)

BaseTag::BaseTag(TagType type) : _type(type), _name(""), _value(type) {}
BaseTag::~BaseTag() {}

size_t BaseTag::ReadFromBuffer(const char* buffer,
                               size_t length,
                               size_t offset) {
  buffer += TAG_TYPE_OFFSET;
  size_t name_length =
      ReadNameFromBuffer(buffer, length, offset + TAG_TYPE_OFFSET);
  if (name_length == NBTCC_READ_ERROR) {
    return NBTCC_READ_ERROR;
  }
  size_t body_length = ReadBodyFromBuffer(
      buffer, length, offset + name_length + TAG_TYPE_OFFSET);
  if (body_length == NBTCC_READ_ERROR) {
    return NBTCC_READ_ERROR;
  }

  return TAG_TYPE_OFFSET + name_length + body_length;
}

size_t BaseTag::ReadNameFromBuffer(const char* buffer,
                                   size_t length,
                                   size_t offset) {
  if (offset >= length) {
    return NBTCC_READ_ERROR;
  }

  uint16_t name_length = *reinterpret_cast<const uint16_t*>(buffer + offset);
  if (name_length > length - offset - 1) {
    return NBTCC_READ_ERROR;
  }

  _name = string(buffer + offset + sizeof(uint16_t), name_length);
  return sizeof(uint16_t) + name_length;
}

size_t BaseTag::count() const {
  switch (_type) {
    case kTagByte:
    case kTagShort:
    case kTagInt:
    case kTagLong:
    case kTagFloat:
    case kTagDouble:
    case kTagString:
      return 0;

    case kTagByteArray:
      return _value.ByteArrayValue().size();

    case kTagIntArray:
      return _value.IntArrayValue().size();

    case kTagList:
      return _value.ListSize();

    case kTagCompound:
      return _value.CompoundSize();

    default:
      return 0;
  }
}

}  // namespace nbtcc
