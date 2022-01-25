#include "nbt.h"

namespace nbtcc {

using std::shared_ptr;

bool NBT::Load(const char* buffer, size_t size) {
  size_t offset = 0;
  while (offset < size) {
    NBT::ReturnedTag value = NBT::GetNextTag(buffer, size, offset);
    if (!value.tag.get()) {
      return false;
    }

    offset = value.length;
    _root[value.tag->name()] = value.tag;
  }

  return true;
}

NBT::ReturnedTag NBT::GetNextTag(const char* buffer,
                                 size_t size,
                                 size_t offset) {
  if (offset + sizeof(TagType) > size) {
    return {nullptr, 0};
  }

  uint8_t maybe_type = *(buffer + offset);
  if (maybe_type >= TagType::kTagMax) {
    return {nullptr, 0};
  }

  TagType type = static_cast<TagType>(maybe_type);
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
      return {nullptr, 0};
  }

  assert(tag.get());

  size_t read = tag->ReadFromBuffer(buffer, size, offset);
  if (read == NBTCC_READ_ERROR) {
    return {nullptr, 0};
  }

  return {tag, read};
}

}  // namespace nbtcc
