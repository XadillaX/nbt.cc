#include "tags/list.h"
#include "tags/compound.h"
#include "tags/meta_array_tags.h"
#include "tags/meta_tags.h"
#include "tags/string.h"

namespace nbtcc {

using std::shared_ptr;

const shared_ptr<BaseTag> TagList::GetChild(size_t index) const {
  return _value.ListValueAt(index);
}

shared_ptr<BaseTag> TagList::GetChild(size_t index) {
  return _value.ListValueAt(index);
}

bool TagList::SetChild(size_t index, shared_ptr<BaseTag> tag) {
  if (_value.ListSize() <= index || !tag.get()) {
    return false;
  }

  if (tag->type() != _child_type) {
    return false;
  }

  (*_value._value._list)[index] = tag;
  return true;
}

bool TagList::EraseChild(size_t index) {
  if (_value.ListSize() <= index) {
    return false;
  }

  _value._value._list->erase(_value._value._list->begin() + index);
  if (GetChildCount() == 0) {
    _child_type = TagType::kTagEnd;
  }
  return true;
}

void TagList::Clear() {
  _value.ClearListValue();
  _child_type = TagType::kTagEnd;
}

bool TagList::Insert(size_t index, shared_ptr<BaseTag> tag) {
  if (!tag.get()) {
    return false;
  }

  if (tag->type() != _child_type && GetChildCount() > 0) {
    return false;
  }

  if (index > GetChildCount()) {
    return false;
  }

  _value._value._list->insert(_value._value._list->begin() + index, tag);
  _child_type = tag->type();
  return true;
}

bool TagList::PushBack(shared_ptr<BaseTag> tag) {
  if (!tag.get()) {
    return false;
  }

  if (tag->type() != _child_type) {
    if (GetChildCount() > 0) {
      return false;
    }

    _child_type = tag->type();
  }

  _value._value._list->push_back(tag);
  return true;
}

bool TagList::PopBack() {
  if (GetChildCount() == 0) {
    return false;
  }

  _value._value._list->pop_back();
  if (GetChildCount() == 0) {
    _child_type = TagType::kTagEnd;
  }

  return true;
}

bool TagList::PushFront(shared_ptr<BaseTag> tag) {
  return Insert(0, tag);
}

bool TagList::PopFront() {
  return EraseChild(0);
}

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
