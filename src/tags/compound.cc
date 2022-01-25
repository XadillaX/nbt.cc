#include "tags/compound.h"
#include "tags/list.h"
#include "tags/meta_array_tags.h"
#include "tags/meta_tags.h"
#include "tags/string.h"

namespace nbtcc {

using std::shared_ptr;
using std::string;
using std::vector;

const shared_ptr<BaseTag> TagCompound::GetChild(const string& key) const {
  return _value.GetFromCompound(key);
}

shared_ptr<BaseTag> TagCompound::GetChild(const string& key) {
  return _value.GetFromCompound(key);
}

bool TagCompound::SetChild(const string& key,
                           shared_ptr<BaseTag> tag,
                           bool overwrite) {
  return _value.SetToCompound(key, tag, overwrite);
}

bool TagCompound::EraseChild(const string& key) {
  if (HasChild(key)) {
    _value._value._compound->erase(key);
    return true;
  }

  return false;
}

bool TagCompound::HasChild(const string& key) const {
  return _value._value._compound->find(key) != _value._value._compound->end();
}

void TagCompound::GetChildKeys(vector<string>* keys) const {
  keys->clear();
  for (auto& it : *_value._value._compound) {
    keys->push_back(it.first);
  }
}

void TagCompound::Clear() {
  _value._value._compound->clear();
}

size_t TagCompound::ReadNextTag(const char* buffer,
                                size_t length,
                                size_t offset) {
  if (offset + sizeof(TagType) > length) {
    return NBTCC_READ_ERROR;
  }

  PrintDebug("TagCompound::ReadNextTag offset: %zu\n", offset);

  TagType type = static_cast<TagType>(buffer[offset]);
  if (type == TagType::kTagEnd) {
    return 0;
  }

  PrintDebug(" > next type: %d\n", type);
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
