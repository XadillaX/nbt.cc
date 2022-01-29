#ifndef INCLUDE_TAG_VALUE_H_
#define INCLUDE_TAG_VALUE_H_

#include <assert.h>
#include <string.h>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "nbt_version.h"
#include "tag_type.h"

namespace nbtcc {
/**
 * \defgroup Utils
 * @brief The utilities used in the library.
 * @{
 */
class BaseTag;

/**
 * The raw data union of a tag.
 */
union TagValueUnion {
  int8_t _byte;                                  ///< The byte value.
  int16_t _short;                                ///< The short value.
  int32_t _int;                                  ///< The int32 value.
  int64_t _long;                                 ///< The int64 value.
  float _float;                                  ///< The float value.
  double _double;                                ///< The double value.
  std::vector<int8_t>* _byte_array;              ///< The byte array value.
  std::string* _string;                          ///< The string value.
  std::vector<std::shared_ptr<BaseTag>>* _list;  ///< The list value.
  std::map<std::string, std::shared_ptr<BaseTag>>*
      _compound;                     ///< The compound value.
  std::vector<int32_t>* _int_array;  ///< The int array value.
  void* _ptr;  ///< For getting _byte_array, _string, _list, _compound,
               ///< _int_array's raw pointer.
};

class TagCompound;
class TagList;

/**
 * The underlying TagValueUnion wrapper of tag value class.
 */
class TagValue {
  friend class TagCompound;
  friend class TagList;

 public:
  /**
   * The constructor of TagValue.
   * @param type The type of the tag.
   */
  explicit TagValue(TagType type);

  /**
   * The deconstructor of TagValue.
   */
  ~TagValue();

  /**
   * Get the byte value if it's a TagByte's underlying value.
   * @return The byte value.
   */
  inline int8_t ByteValue() const {
    return _type == kTagByte ? _value._byte : 0;
  }

  /**
   * Set the byte value if it's a TagByte's underlying value.
   * @param value The byte value.
   */
  inline void SetByteValue(int8_t value) {
    assert(_type == kTagByte);
    _value._byte = value;
  }

  /**
   * Get the short value if it's a TagShort's underlying value.
   * @return The short value.
   */
  inline int16_t ShortValue() const {
    return _type == kTagShort ? _value._short : 0;
  }

  /**
   * Set the short value if it's a TagShort's underlying value.
   * @param value The short value.
   */
  inline void SetShortValue(int16_t value) {
    assert(_type == kTagShort);
    _value._short = value;
  }

  /**
   * Get the int32 value if it's a TagInt's underlying value.
   * @return The int32 value.
   */
  inline int32_t IntValue() const { return _type == kTagInt ? _value._int : 0; }

  /**
   * Set the int32 value if it's a TagInt's underlying value.
   * @param value The int32 value.
   */
  inline void SetIntValue(int32_t value) {
    assert(_type == kTagInt);
    _value._int = value;
  }

  /**
   * Get the int64 value if it's a TagLong's underlying value.
   * @return The int64 value.
   */
  inline int64_t LongValue() const {
    return _type == kTagLong ? _value._long : 0;
  }

  /**
   * Set the int64 value if it's a TagLong's underlying value.
   * @param value The int64 value.
   */
  inline void SetLongValue(int64_t value) {
    assert(_type == kTagLong);
    _value._long = value;
  }

  /**
   * Get the float value if it's a TagFloat's underlying value.
   * @return The float value.
   */
  inline float FloatValue() const {
    return _type == kTagFloat ? _value._float : 0.0f;
  }

  /**
   * Set the float value if it's a TagFloat's underlying value.
   * @param value The float value.
   */
  inline void SetFloatValue(float value) {
    assert(_type == kTagFloat);
    _value._float = value;
  }

  /**
   * Get the double value if it's a TagDouble's underlying value.
   * @return The double value.
   */
  inline double DoubleValue() const {
    return _type == kTagDouble ? _value._double : 0.0;
  }

  /**
   * Set the double value if it's a TagDouble's underlying value.
   * @param value The double value.
   */
  inline void SetDoubleValue(double value) {
    assert(_type == kTagDouble);
    _value._double = value;
  }

  /**
   * Get the byte array if it's a TagByteArray's underlying value.
   * @return The byte array.
   */
  inline std::vector<int8_t>& ByteArrayValue() {
    assert(_type == kTagByteArray);
    return *_value._byte_array;
  }

  /**
   * Get the byte array if it's a TagByteArray's underlying value.
   * @return The byte array.
   */
  inline const std::vector<int8_t>& ByteArrayValue() const {
    assert(_type == kTagByteArray);
    return *_value._byte_array;
  }

  /**
   * Get the int array if it's a TagIntArray's underlying value.
   * @return The int array.
   */
  inline std::vector<int32_t>& IntArrayValue() {
    assert(_type == kTagIntArray);
    return *_value._int_array;
  }

  /**
   * Get the int array if it's a TagIntArray's underlying value.
   * @return The int array.
   */
  inline const std::vector<int32_t>& IntArrayValue() const {
    assert(_type == kTagIntArray);
    return *_value._int_array;
  }

  /**
   * Get the string value if it's a TagString's underlying value.
   * @return The string value.
   */
  inline const std::string& StringValue() const {
    assert(_type == kTagString);
    return *_value._string;
  }

  /**
   * Set the string value if it's a TagString's underlying value.
   * @param value The string value.
   */
  inline void SetStringValue(const std::string& value) {
    assert(_type == kTagString);
    *_value._string = value;
  }

  /**
   * Shift a tag from the list if it's a TagList's underlying value.
   * @return The shifted tag. (`nullptr` if the list is empty)
   */
  inline std::shared_ptr<BaseTag> ShiftListValue() {
    assert(_type == kTagList);
    if (_value._list->empty()) {
      return nullptr;
    }

    std::shared_ptr<BaseTag> ret = _value._list->front();
    _value._list->erase(_value._list->begin());
    return ret;
  }

  /**
   * Pop a tag from the list if it's a TagList's underlying value.
   * @return The popped tag. (`nullptr` if the list is empty)
   */
  inline std::shared_ptr<BaseTag> PopListValue() {
    assert(_type == kTagList);
    if (_value._list->empty()) {
      return nullptr;
    }

    std::shared_ptr<BaseTag> ret = _value._list->back();
    _value._list->pop_back();
    return ret;
  }

  /**
   * Erase a tag from the list if it's a TagList's underlying value.
   * @param index The index of the tag to erase.
   */
  inline void EraseListValue(size_t index) {
    assert(_type == kTagList);
    _value._list->erase(_value._list->begin() + index);
  }

  /**
   * Get a tag from the list if it's a TagList's underlying value.
   * @param index The index of the tag to get.
   * @return The tag. (`nullptr` if the index is out of range)
   */
  inline const std::shared_ptr<BaseTag> ListValueAt(size_t index) const {
    assert(_type == kTagList);
    if (index >= _value._list->size()) {
      return nullptr;
    }

    return _value._list->at(index);
  }

  /**
   * Get a tag from the list if it's a TagList's underlying value.
   * @param index The index of the tag to get.
   * @return The tag. (`nullptr` if the index is out of range)
   */
  inline std::shared_ptr<BaseTag> ListValueAt(size_t index) {
    assert(_type == kTagList);
    if (index >= _value._list->size()) {
      return nullptr;
    }

    return _value._list->at(index);
  }

  /**
   * Clear the list if it's a TagList's underlying value.
   */
  inline void ClearListValue() {
    assert(_type == kTagList);
    _value._list->clear();
  }

  /**
   * Get the element count of the list if it's a TagList's underlying value.
   * @return The element count.
   */
  inline size_t ListSize() const {
    assert(_type == kTagList);
    return _value._list->size();
  }

  /**
   * Set a tag to the compound if it's a TagCompound's underlying value.
   * @param name The name of the tag.
   * @param tag The tag to set.
   * @param overwrite Whether to overwrite the tag if it already exists.
   * @return Whether the tag was set.
   */
  bool SetToCompound(const std::string& name,
                     std::shared_ptr<BaseTag> tag,
                     bool overwrite = false);

  /**
   * Get a tag from the compound if it's a TagCompound's underlying value.
   * @param name The name of the tag.
   * @return The tag. (`nullptr` if the tag doesn't exist)
   */
  inline std::shared_ptr<BaseTag> GetFromCompound(const std::string& name) {
    assert(_type == kTagCompound);

    auto it = _value._compound->find(name);
    if (it == _value._compound->end()) {
      return nullptr;
    }

    return it->second;
  }

  /**
   * Get a tag from the compound if it's a TagCompound's underlying value.
   * @param name The name of the tag.
   * @return The tag. (`nullptr` if the tag doesn't exist)
   */
  inline const std::shared_ptr<BaseTag> GetFromCompound(
      const std::string& name) const {
    assert(_type == kTagCompound);

    auto it = _value._compound->find(name);
    if (it == _value._compound->end()) {
      return nullptr;
    }

    return it->second;
  }

  /**
   * Erase a tag from the compound if it's a TagCompound's underlying value.
   * @param name The name of the tag.
   */
  inline void DeleteFromCompound(const std::string& name) {
    assert(_type == kTagCompound);
    _value._compound->erase(name);
  }

  /**
   * Clear the compound if it's a TagCompound's underlying value.
   */
  inline void ClearCompound() {
    assert(_type == kTagCompound);
    _value._compound->clear();
  }

  /**
   * Get the element count of the compound if it's a TagCompound's underlying
   * value.
   * @return The element count.
   */
  inline size_t CompoundSize() const {
    assert(_type == kTagCompound);
    return _value._compound->size();
  }

  /**
   * Get the type of the related tag.
   * @return The type of the related tag.
   */
  TagType type() const { return _type; }

  /**
   * Get the raw data's `void*` pointer. Used for `reinterpret_cast`ing to any
   * type. (**NOT** recommended and use this method under your own risk)
   * @return The raw data's `void*` pointer.
   */
  void* raw_value_ptr() { return &_value; }

  /**
   * Get the raw data's `const void*` pointer. Used for `reinterpret_cast`ing
   * to any type. (**NOT** recommended and use this method under your own
   * risk)
   * @return The raw data's `const void*` pointer.
   */
  const void* raw_value_ptr() const { return &_value; }

  /**
   * Convert TagValueUnion's _byte_array, _string, _list, _compound or
   * _int_array pointer to `void*` and return it. (**NOT** recommended and use
   * this method under your own risk)
   * @return The related `void*` pointer.
   */
  void* void_ptr() { return _value._ptr; }

  /**
   * Convert TagValueUnion's _byte_array, _string, _list, _compound or
   * _int_array pointer to `const void*` and return it. (**NOT** recommended
   * and use this method under your own risk)
   * @return The related `const void*` pointer.
   */
  const void* void_ptr() const { return _value._ptr; }

 private:
  TagType _type;
  TagValueUnion _value;
};
/**
 * @}
 */
}  // namespace nbtcc

#endif  // INCLUDE_TAG_VALUE_H_
