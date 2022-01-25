#include "tag_value.h"
#include "base_tag.h"

namespace nbtcc {

using std::map;
using std::shared_ptr;
using std::string;
using std::vector;

TagValue::TagValue(TagType type) : _type(type) {
  switch (_type) {
    case kTagByte:
      _value._byte = 0;
      break;

    case kTagShort:
      _value._short = 0;
      break;

    case kTagInt:
      _value._int = 0;
      break;

    case kTagLong:
      _value._long = 0;
      break;

    case kTagFloat:
      _value._float = 0.0f;
      break;

    case kTagDouble:
      _value._double = 0.0;
      break;

    case kTagByteArray:
      _value._byte_array = new vector<int8_t>();
      break;

    case kTagString:
      _value._string = new string();
      break;

    case kTagList:
      _value._list = new vector<shared_ptr<BaseTag>>();
      break;

    case kTagCompound:
      _value._compound = new map<string, shared_ptr<BaseTag>>();
      break;

    case kTagIntArray:
      _value._int_array = new vector<int32_t>();
      break;

    default:
      break;
  }
}

TagValue::~TagValue() {
  switch (_type) {
    case kTagByteArray:
      delete _value._byte_array;
      break;

    case kTagString:
      delete _value._string;
      break;

    case kTagList:
      delete _value._list;
      break;

    case kTagCompound:
      delete _value._compound;
      break;

    case kTagIntArray:
      delete _value._int_array;
      break;

    default:
      break;
  }
}

bool TagValue::SetToCompound(const std::string& name,
                             std::shared_ptr<BaseTag> tag,
                             bool overwrite) {
  assert(_type == kTagCompound);

  if (tag == nullptr) {
    return false;
  }

  if (!overwrite && _value._compound->find(name) != _value._compound->end()) {
    return false;
  }

  (*_value._compound)[name] = tag;
  tag->set_name(name);
  return true;
}

}  // namespace nbtcc
