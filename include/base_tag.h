#ifndef INCLUDE_BASE_TAG_H_
#define INCLUDE_BASE_TAG_H_

#include <endian.h>
#include <stdarg.h>
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

  template <typename T>
  inline T* AsPtr() {
    return reinterpret_cast<T*>(this);
  }

  template <typename T>
  inline const T* AsPtr() const {
    return reinterpret_cast<const T*>(this);
  }

  inline TagType type() const { return _type; }
  inline const std::string& name() const { return _name; }
  inline void set_name(const std::string& name) { _name = name; }
  inline TagValue& value() { return _value; }
  inline const TagValue& value() const { return _value; }

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

 public:
  static size_t read_level;

 protected:
  TagType _type;
  std::string _name;
  TagValue _value;
};

#ifdef _PRINT_DEBUG
inline void PrintDebug(const char* format, ...) {
  va_list args;
  va_start(args, format);
  std::string fmt = "";
  for (size_t i = 0; i < BaseTag::read_level; i++) {
    fmt += "  ";
  }
  fmt += format;

  vfprintf(stderr, fmt.c_str(), args);
  va_end(args);
}
#else
inline void PrintDebug(const char*, ...) {}
#endif

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

    T value = *reinterpret_cast<const T*>(_value.raw_value_ptr());
    if (sizeof(T) == 2) {
      value = htobe16(value);
    } else if (sizeof(T) == 4) {
      value = htobe32(value);
    } else if (sizeof(T) == 8) {
      value = htobe64(value);
    }

    memcpy(buffer + offset, &value, sizeof(T));
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
    if (sizeof(T) == 2) {
      *reinterpret_cast<uint16_t*>(_value.raw_value_ptr()) =
          be16toh(*reinterpret_cast<uint16_t*>(_value.raw_value_ptr()));
    } else if (sizeof(T) == 4) {
      *reinterpret_cast<uint32_t*>(_value.raw_value_ptr()) =
          be32toh(*reinterpret_cast<uint32_t*>(_value.raw_value_ptr()));
    } else if (sizeof(T) == 8) {
      *reinterpret_cast<uint64_t*>(_value.raw_value_ptr()) =
          be64toh(*reinterpret_cast<uint64_t*>(_value.raw_value_ptr()));
    }

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
    uint32_t size_be = htobe32(size);
    memcpy(buffer + offset, &size_be, sizeof(uint32_t));

    // Then `sizeof(T) * size` bytes is the array.
    size_t next_offset = offset + sizeof(uint32_t);
    size_t end_offset = next_offset + size * sizeof(T);

    if (end_offset > length) {
      return NBTCC_WRITE_ERROR;
    }

    memcpy(buffer + next_offset, array->data(), size * sizeof(T));
    for (char* i = buffer + next_offset; i < buffer + end_offset;
         i += sizeof(T)) {
      if (sizeof(T) == 2) {
        *reinterpret_cast<uint16_t*>(i) =
            be16toh(*reinterpret_cast<uint16_t*>(i));
      } else if (sizeof(T) == 4) {
        *reinterpret_cast<uint32_t*>(i) =
            be32toh(*reinterpret_cast<uint32_t*>(i));
      } else if (sizeof(T) == 8) {
        *reinterpret_cast<uint64_t*>(i) =
            be64toh(*reinterpret_cast<uint64_t*>(i));
      }
    }

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
    size_t array_size =
        be32toh(*reinterpret_cast<const uint32_t*>(buffer + offset));

    // Then `sizeof(T) * size` bytes is the array.
    size_t next_offset = offset + sizeof(uint32_t);
    size_t end_offset = next_offset + array_size * sizeof(T);

    if (end_offset > length) {
      return NBTCC_READ_ERROR;
    }

    std::vector<T>* array = GetVector();
    array->resize(array_size);
    for (size_t i = next_offset, j = 0; i < end_offset; i += sizeof(T), j++) {
      if (sizeof(T) == 2) {
        (*array)[j] = be16toh(*reinterpret_cast<const T*>(buffer + i));
      } else if (sizeof(T) == 4) {
        (*array)[j] = be32toh(*reinterpret_cast<const T*>(buffer + i));
      } else if (sizeof(T) == 8) {
        (*array)[j] = be64toh(*reinterpret_cast<const T*>(buffer + i));
      } else {
        (*array)[j] = *reinterpret_cast<const T*>(buffer + i);
      }
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
