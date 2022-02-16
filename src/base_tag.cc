#include "base_tag.h"

namespace nbtcc {

using std::string;

#define TAG_TYPE_OFFSET sizeof(TagType)

#ifdef _PRINT_DEBUG
void PrintDebug(const char* format, ...) {
  va_list args;
  va_start(args, format);
  std::string fmt = "";
  for (size_t i = 0; i < BaseTag::read_level; i++) {
    fmt += "  ";
  }
  fmt += format;

  vfprintf(stderr, fmt.c_str(), args);
  va_end(args);
}
#else
void PrintDebug(const char*, ...) {}
#endif

size_t BaseTag::read_level = 0;

BaseTag::BaseTag(TagType type) : _type(type), _name(""), _value(type) {}
BaseTag::~BaseTag() {}

size_t BaseTag::ReadFromBuffer(const char* buffer,
                               size_t length,
                               size_t offset) {
  PrintDebug("BaseTag::ReadFromBuffer type: %d, offset: %zu\n", _type, offset);
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

  PrintDebug("BaseTag::ReadFromBuffer name: %s, body_length: %zu\n",
             _name.c_str(),
             body_length);
  return TAG_TYPE_OFFSET + name_length + body_length;
}

size_t BaseTag::ReadNameFromBuffer(const char* buffer,
                                   size_t length,
                                   size_t offset) {
  PrintDebug(
      "BaseTag::ReadNameFromBuffer type: %d, offset: %zu\n", _type, offset);
  if (offset + sizeof(uint16_t) > length) {
    return NBTCC_READ_ERROR;
  }

  uint16_t name_length;
  memcpy(&name_length, buffer + offset, sizeof(uint16_t));
  name_length = be16toh(name_length);
  PrintDebug(" > name_length: %d\n", name_length);
  if (name_length > length - offset - 1) {
    return NBTCC_READ_ERROR;
  }

  _name = string(buffer + offset + sizeof(uint16_t), name_length);
  PrintDebug(" > name: %s\n", _name.c_str());
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
