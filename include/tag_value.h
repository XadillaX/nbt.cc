#ifndef INCLUDE_TAG_VALUE_H_
#define INCLUDE_TAG_VALUE_H_

#include <assert.h>
#include <string.h>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "tag_type.h"

namespace nbtcc {

class BaseTag;
union TagValueUnion {
  int8_t _byte;
  int16_t _short;
  int32_t _int;
  int64_t _long;
  float _float;
  double _double;
  std::vector<int8_t>* _byte_array;
  std::string* _string;
  std::vector<std::shared_ptr<BaseTag>>* _list;
  std::map<std::string, std::shared_ptr<BaseTag>>* _compound;
  std::vector<int32_t>* _int_array;
  void* _ptr;
};

class TagCompound;
class TagList;
class TagValue {
  friend class TagCompound;
  friend class TagList;

 public:
  explicit TagValue(TagType type);
  ~TagValue();

  inline int8_t ByteValue() const {
    return _type == kTagByte ? _value._byte : 0;
  }

  inline void SetByteValue(int8_t value) {
    assert(_type == kTagByte);
    _value._byte = value;
  }

  inline int16_t ShortValue() const {
    return _type == kTagShort ? _value._short : 0;
  }

  inline void SetShortValue(int16_t value) {
    assert(_type == kTagShort);
    _value._short = value;
  }

  inline int32_t IntValue() const { return _type == kTagInt ? _value._int : 0; }

  inline void SetIntValue(int32_t value) {
    assert(_type == kTagInt);
    _value._int = value;
  }

  inline int64_t LongValue() const {
    return _type == kTagLong ? _value._long : 0;
  }

  inline void SetLongValue(int64_t value) {
    assert(_type == kTagLong);
    _value._long = value;
  }

  inline float FloatValue() const {
    return _type == kTagFloat ? _value._float : 0.0f;
  }

  inline void SetFloatValue(float value) {
    assert(_type == kTagFloat);
    _value._float = value;
  }

  inline double DoubleValue() const {
    return _type == kTagDouble ? _value._double : 0.0;
  }

  inline void SetDoubleValue(double value) {
    assert(_type == kTagDouble);
    _value._double = value;
  }

  inline std::vector<int8_t>& ByteArrayValue() {
    assert(_type == kTagByteArray);
    return *_value._byte_array;
  }

  inline const std::vector<int8_t>& ByteArrayValue() const {
    assert(_type == kTagByteArray);
    return *_value._byte_array;
  }

  inline std::vector<int32_t>& IntArrayValue() {
    assert(_type == kTagIntArray);
    return *_value._int_array;
  }

  inline const std::vector<int32_t>& IntArrayValue() const {
    assert(_type == kTagIntArray);
    return *_value._int_array;
  }

  inline const std::string& StringValue() const {
    assert(_type == kTagString);
    return *_value._string;
  }

  inline void SetStringValue(const std::string& value) {
    assert(_type == kTagString);
    *_value._string = value;
  }

  inline std::shared_ptr<BaseTag> ShiftListValue() {
    assert(_type == kTagList);
    if (_value._list->empty()) {
      return nullptr;
    }

    std::shared_ptr<BaseTag> ret = _value._list->front();
    _value._list->erase(_value._list->begin());
    return ret;
  }

  inline std::shared_ptr<BaseTag> PopListValue() {
    assert(_type == kTagList);
    if (_value._list->empty()) {
      return nullptr;
    }

    std::shared_ptr<BaseTag> ret = _value._list->back();
    _value._list->pop_back();
    return ret;
  }

  inline void EraseListValue(size_t index) {
    assert(_type == kTagList);
    _value._list->erase(_value._list->begin() + index);
  }

  inline const std::shared_ptr<BaseTag> ListValueAt(size_t index) const {
    assert(_type == kTagList);
    if (index >= _value._list->size()) {
      return nullptr;
    }

    return _value._list->at(index);
  }

  inline std::shared_ptr<BaseTag> ListValueAt(size_t index) {
    assert(_type == kTagList);
    if (index >= _value._list->size()) {
      return nullptr;
    }

    return _value._list->at(index);
  }

  inline void ClearListValue() {
    assert(_type == kTagList);
    _value._list->clear();
  }

  inline size_t ListSize() const {
    assert(_type == kTagList);
    return _value._list->size();
  }

  bool SetToCompound(const std::string& name,
                     std::shared_ptr<BaseTag> tag,
                     bool overwrite = false);

  inline std::shared_ptr<BaseTag> GetFromCompound(const std::string& name) {
    assert(_type == kTagCompound);

    auto it = _value._compound->find(name);
    if (it == _value._compound->end()) {
      return nullptr;
    }

    return it->second;
  }

  inline const std::shared_ptr<BaseTag> GetFromCompound(
      const std::string& name) const {
    assert(_type == kTagCompound);

    auto it = _value._compound->find(name);
    if (it == _value._compound->end()) {
      return nullptr;
    }

    return it->second;
  }

  inline void DeleteFromCompound(const std::string& name) {
    assert(_type == kTagCompound);
    _value._compound->erase(name);
  }

  inline void ClearCompound() {
    assert(_type == kTagCompound);
    _value._compound->clear();
  }

  inline size_t CompoundSize() const {
    assert(_type == kTagCompound);
    return _value._compound->size();
  }

  TagType type() const { return _type; }
  void* raw_value_ptr() { return &_value; }
  const void* raw_value_ptr() const { return &_value; }
  void* void_ptr() { return _value._ptr; }
  const void* void_ptr() const { return _value._ptr; }

 private:
  TagType _type;
  TagValueUnion _value;
};

}  // namespace nbtcc

#endif  // INCLUDE_TAG_VALUE_H_
