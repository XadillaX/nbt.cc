#include "tags/list.h"
#include "tags/compound.h"
#include "tags/meta_array_tags.h"
#include "tags/meta_tags.h"
#include "tags/string.h"

namespace nbtcc {

size_t TagList::ReadNextTag(const char* buffer,
                            size_t length,
                            size_t offset,
                            size_t idx) {
  if (offset >= length) {
    return NBTCC_READ_ERROR;
  }

  std::shared_ptr<BaseTag> tag;
  switch (_child_type) {
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

  size_t tag_size = tag->ReadBodyFromBuffer(buffer, length, offset);
  if (tag_size == NBTCC_READ_ERROR) {
    return NBTCC_READ_ERROR;
  }

  (*_value._value._list)[idx] = tag;
  return tag_size;
}

}  // namespace nbtcc
