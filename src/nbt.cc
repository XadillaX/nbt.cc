#include "nbt.h"

namespace nbtcc {

using std::shared_ptr;

bool NBT::Load(const char* buffer, size_t size) {
  _root.clear();
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

size_t NBT::GetBufferSize() const {
  size_t size = 0;
  for (const auto& pair : _root) {
    // child type id for 1 byte, child name length for 2 bytes and child
    // name for (child name length) byte(s).
    size += sizeof(uint8_t) + sizeof(uint16_t);
    size += pair.first.length();
    size += pair.second->CalculateSize();
  }

  return size;
}

size_t NBT::Write(char* buffer, size_t size) const {
  size_t offset = 0;
  size_t temp = 0;
  for (const auto& pair : _root) {
    if (offset + sizeof(uint8_t) + sizeof(uint16_t) + pair.first.length() >
            size ||
        pair.first.length() > UINT16_MAX) {
      return NBTCC_WRITE_ERROR;
    }

    *(buffer + offset) = static_cast<uint8_t>(pair.second->type());
    offset += sizeof(uint8_t);
    uint16_t name_length = htobe16(pair.first.length());
    memcpy(buffer + offset, &name_length, sizeof(uint16_t));
    offset += sizeof(uint16_t);
    memcpy(buffer + offset, pair.first.c_str(), pair.first.length());
    offset += pair.first.length();

    temp = pair.second->WriteToBuffer(buffer, size, offset);
    if (temp == NBTCC_WRITE_ERROR) {
      return NBTCC_WRITE_ERROR;
    }

    offset += temp;
  }

  return offset;
}

}  // namespace nbtcc
