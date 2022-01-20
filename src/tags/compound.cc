#include "tags/compound.h"
#include "tags/list.h"
#include "tags/meta_array_tags.h"
#include "tags/meta_tags.h"
#include "tags/string.h"

namespace nbtcc {

size_t TagCompound::ReadNextTag(const char* buffer,
                                size_t length,
                                size_t offset) {
  if (offset + sizeof(TagType) > length) {
    return NBTCC_READ_ERROR;
  }

  TagType type = static_cast<TagType>(buffer[offset]);
  offset += sizeof(TagType);
  if (type == TagType::kTagEnd) {
    return 0;
  }

  std::shared_ptr<BaseTag> tag;
  switch (type) {
#define CASE(type)                                                             \
  case TagType::k##type:                                                       \
    tag = std::make_shared<type>();                                            \
    break
    CASE(TagByte);
    CASE(TagShort);
    CASE(TagInt);
    CASE(TagLong);
    CASE(TagFloat);
    CASE(TagDouble);
    CASE(TagByteArray);
    CASE(TagIntArray);
    CASE(TagCompound);
    CASE(TagList);
    CASE(TagString);
#undef CASE

    default:
      // Invalid type
      return NBTCC_READ_ERROR;
  }

  assert(tag.get());
  size_t read_size = tag->ReadFromBuffer(buffer, length, offset);
  if (read_size == NBTCC_READ_ERROR) {
    return NBTCC_READ_ERROR;
  }

  (*_value._value._compound)[tag->name()] = tag;
  return read_size;
}

}  // namespace nbtcc
