#ifndef INCLUDE_BASE_TAG_H_
#define INCLUDE_BASE_TAG_H_

#include "tag_value.h"

namespace nbtcc {

#define NBTCC_READ_ERROR static_cast<size_t>(-1)
#define NBTCC_WRITE_ERROR static_cast<size_t>(-1)

class TagList;
class BaseTag {
  friend class TagList;

 public:
  explicit BaseTag(TagType type);
  virtual ~BaseTag();

  size_t ReadFromBuffer(const char* buffer, size_t length, size_t offset);

  inline TagType type() const { return _type; }
  inline const std::string& name() const { return _name; }
  inline void set_name(const std::string& name) { _name = name; }
  inline TagValue& value() { return _value; }

  // Alias for `name()`.
  inline const std::string& id() const { return _name; }

  size_t count() const;

  virtual size_t WriteToBuffer(char* buffer,
                               size_t length,
                               size_t offset) const = 0;
  virtual size_t CalculateSize() const = 0;

 protected:
  virtual size_t ReadBodyFromBuffer(const char* buffer,
                                    size_t length,
                                    size_t offset) = 0;
  virtual size_t ReadNameFromBuffer(const char* buffer,
                                    size_t length,
                                    size_t offset);

 protected:
  TagType _type;
  std::string _name;
  TagValue _value;
};

template <typename T, TagType TAG_ENUM>
class __TagMetaType__ : public BaseTag {
 public:
  inline __TagMetaType__() : BaseTag(TAG_ENUM) {}
  inline explicit __TagMetaType__(const T value) : BaseTag(TAG_ENUM) {
    memcpy(_value.raw_value_ptr(), &value, sizeof(T));
  }
  virtual inline ~__TagMetaType__() {}

  virtual inline size_t CalculateSize() const { return sizeof(T); }

  virtual inline size_t WriteToBuffer(char* buffer,
                                      size_t length,
                                      size_t offset) const {
    if (offset + sizeof(T) > length) {
      return NBTCC_WRITE_ERROR;
    }

    memcpy(buffer + offset, _value.raw_value_ptr(), sizeof(T));
    return sizeof(T);
  }

 protected:
  virtual inline size_t ReadBodyFromBuffer(const char* buffer,
                                           size_t length,
                                           size_t offset) {
    if (offset + sizeof(T) > length) {
      return NBTCC_READ_ERROR;
    }

    memcpy(_value.raw_value_ptr(), buffer + offset, sizeof(T));
    return sizeof(T);
  }
};

template <typename T, TagType TAG_ENUM>
class __TagMetaArrayType__ : public BaseTag {
 public:
  inline __TagMetaArrayType__() : BaseTag(TAG_ENUM) {}
  virtual inline ~__TagMetaArrayType__() {}

  virtual inline size_t CalculateSize() const {
    return sizeof(uint32_t) + this->GetVector()->size() * sizeof(T);
  }

  virtual inline size_t WriteToBuffer(char* buffer,
                                      size_t length,
                                      size_t offset) const {
    if (offset + sizeof(uint32_t) > length) {
      return NBTCC_WRITE_ERROR;
    }

    // First `sizeof(uint32_t)` bytes is the size of array.
    const std::vector<T>* array = GetVector();
    uint32_t size = array->size();
    memcpy(buffer + offset, &size, sizeof(uint32_t));

    // Then `sizeof(T) * size` bytes is the array.
    size_t next_offset = offset + sizeof(uint32_t);
    size_t end_offset = next_offset + size * sizeof(T);

    if (end_offset > length) {
      return NBTCC_WRITE_ERROR;
    }

    memcpy(buffer + next_offset, array->data(), size * sizeof(T));
    return size * sizeof(T) + sizeof(uint32_t);
  }

 protected:
  virtual inline size_t ReadBodyFromBuffer(const char* buffer,
                                           size_t length,
                                           size_t offset) {
    if (offset + sizeof(uint32_t) > length) {
      return NBTCC_READ_ERROR;
    }

    // First `sizeof(uint32_t)` bytes is the size of array.
    size_t array_size = *reinterpret_cast<const uint32_t*>(buffer + offset);

    // Then `sizeof(T) * size` bytes is the array.
    size_t next_offset = offset + sizeof(uint32_t);
    size_t end_offset = next_offset + array_size * sizeof(T);

    if (end_offset > length) {
      return NBTCC_READ_ERROR;
    }

    std::vector<T>* array = GetVector();
    array->resize(array_size);
    for (size_t i = next_offset, j = 0; i < end_offset; i += sizeof(T), j++) {
      (*array)[j] = (*reinterpret_cast<const T*>(buffer + i));
    }

    return sizeof(uint32_t) + array_size * sizeof(T);
  }

  std::vector<T>* GetVector() {
    return reinterpret_cast<std::vector<T>*>(_value.void_ptr());
  }

  const std::vector<T>* GetVector() const {
    return reinterpret_cast<const std::vector<T>*>(_value.void_ptr());
  }
};

}  // namespace nbtcc

#endif  // INCLUDE_BASE_TAG_H_
