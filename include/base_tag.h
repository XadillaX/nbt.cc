#ifndef INCLUDE_BASE_TAG_H_
#define INCLUDE_BASE_TAG_H_

#include <stdarg.h>
#include "__endian.h"
#include "nbt_version.h"
#include "tag_value.h"

namespace nbtcc {

/**
 * \defgroup Constants
 * @brief The constants used in the library.
 * @{
 */
/**
 * @name Error codes
 */
///@{
#define NBTCC_READ_ERROR static_cast<size_t>(-1)   ///< Error reading buffer
#define NBTCC_WRITE_ERROR static_cast<size_t>(-1)  ///< Error writing buffer
///@}
/**
 * @}
 */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"

/**
 * \defgroup Utils
 * @brief The utilities used in the library.
 * @{
 */
/**
 * Print debug information.
 *
 * @brief Only prints while `_PRINT_DEBUG` is defined at compilation time.
 *
 * @param format The format string.
 * @param ... The arguments.
 */
void PrintDebug(const char* format, ...);
/**
 * @}
 */

class TagList;

/**
 * \defgroup Tags
 * @brief The NBT tags.
 * @{
 */
/**
 * The base class for all tags.
 */
class BaseTag {
  friend class TagList;

 public:
  /**
   * The constructor for the base tag.
   * @param type The type of the tag.
   */
  explicit BaseTag(TagType type);

  /**
   * The destructor for the base tag.
   */
  virtual ~BaseTag();

  /**
   * Read information from a buffer and parse it into the tag.
   * @param buffer The buffer to read from.
   * @param length The size of the buffer.
   * @param offset The offset to start reading from.
   * @return The number of bytes read. \ref NBTCC_READ_ERROR indicates an error.
   */
  size_t ReadFromBuffer(const char* buffer, size_t length, size_t offset);

  /**
   * Cast this base tag into a subclass pointer.
   * @return The subclass pointer.
   */
  template <typename T>
  inline T* AsPtr() {
    return reinterpret_cast<T*>(this);
  }

  /**
   * Cast this base tag into a subclass pointer.
   * @return The subclass pointer.
   */
  template <typename T>
  inline const T* AsPtr() const {
    return reinterpret_cast<const T*>(this);
  }

  /**
   * The type of the tag.
   * @return The type of the tag.
   */
  inline TagType type() const { return _type; }

  /**
   * The name of the tag.
   * @return The name of the tag.
   */
  inline const std::string& name() const { return _name; }

  /**
   * Set the name of the tag.
   * @param name The name of the tag.
   */
  inline void set_name(const std::string& name) { _name = name; }

  /**
   * Return the underlying value of the tag. (**NOT** recommended and use this
   * method under your own risk)
   * @return The underlying value of the tag.
   */
  inline TagValue& __value__() { return _value; }

  /**
   * Return the underlying value of the tag. (**not** recommended and use this
   * method under your own risk)
   * @return The underlying value of the tag.
   */
  inline const TagValue& __value__() const { return _value; }

  /**
   * Return the children count of the tag. (if any)
   */
  size_t count() const;

  // Alias
  /**
   * Alias for `name()`.
   * @see name()
   */
  inline const std::string& id() const { return _name; }

  /**
   * Alias for `type()`
   * @see type()
   */
  inline TagType GetType() const { return _type; }

  /**
   * Alias for `name()`
   * @see name()
   */
  inline const std::string& GetName() const { return _name; }

  /**
   * Alias for `name()`
   * @see name()
   */
  inline const std::string& GetId() const { return _name; }

  /**
   * Alias for `set_name()`
   * @see set_name()
   */
  inline void SetName(const std::string& name) { _name = name; }

  /**
   * Alias for `set_name()`
   * @see set_name()
   */
  inline void SetId(const std::string& id) { _name = id; }

  /**
   * Alias for `count()`
   * @see count()
   */
  inline size_t GetChildCount() const { return count(); }

  /**
   * Write this tag to a buffer.
   * @param buffer The buffer to write to.
   * @param length The size of the buffer.
   * @param offset The offset to start writing from.
   * @return The number of bytes written. \ref NBTCC_WRITE_ERROR indicates an
   * error.
   */
  virtual size_t WriteToBuffer(char* buffer,
                               size_t length,
                               size_t offset) const = 0;

  /**
   * Calculate the buffer size needed to write this tag.
   * @return The buffer size needed to write this tag.
   */
  virtual size_t CalculateSize() const = 0;

 protected:
  /**
   * Read tag body from a buffer.
   * @param buffer The buffer to read from.
   * @param length The size of the buffer.
   * @param offset The offset to start reading from.
   * @return The number of bytes read. \ref NBTCC_READ_ERROR indicates an error.
   */
  virtual size_t ReadBodyFromBuffer(const char* buffer,
                                    size_t length,
                                    size_t offset) = 0;
  /**
   * Read tag name from a buffer.
   * @param buffer The buffer to read from.
   * @param length The size of the buffer.
   * @param offset The offset to start reading from.
   * @return The number of bytes read. \ref NBTCC_READ_ERROR indicates an error.
   */
  virtual size_t ReadNameFromBuffer(const char* buffer,
                                    size_t length,
                                    size_t offset);

 public:
  /**
   * The depth of this tag in the tree. (used in PrintDebug())
   */
  static size_t read_level;

 protected:
  TagType _type;      ///< The type of the tag.
  std::string _name;  ///< The name of the tag.
  TagValue _value;    ///< The underlying value of the tag.
};

/**
 * The base class for TagByte, TagShort, TagInt, TagLong, TagFloat and
 * TagDouble.
 *
 * @see TagByte
 * @see TagShort
 * @see TagInt
 * @see TagLong
 * @see TagFloat
 * @see TagDouble
 */
template <typename T, TagType TAG_ENUM>
class __TagMetaType__ : public BaseTag {
 public:
  /**
   * Get the value of the tag.
   * @return The value of the tag.
   */
  inline T GetValue() const {
    return *reinterpret_cast<const T*>(_value.raw_value_ptr());
  }

  /**
   * Set the value of the tag.
   * @param value The value of the tag.
   */
  inline void SetValue(const T& value) {
    *reinterpret_cast<T*>(_value.raw_value_ptr()) = value;
  }

 public:
  /**
   * The constructor for the tag.
   */
  inline __TagMetaType__() : BaseTag(TAG_ENUM) {}

  /**
   * The constructor for the tag.
   * @param value The initial value of the tag.
   */
  inline explicit __TagMetaType__(const T value) : BaseTag(TAG_ENUM) {
    memcpy(_value.raw_value_ptr(), &value, sizeof(T));
  }

  /**
   * The destructor for the tag.
   */
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
      *reinterpret_cast<uint16_t*>(&value) =
          htobe16(*reinterpret_cast<uint16_t*>(&value));
    } else if (sizeof(T) == 4) {
      *reinterpret_cast<uint32_t*>(&value) =
          htobe32(*reinterpret_cast<uint32_t*>(&value));
    } else if (sizeof(T) == 8) {
      *reinterpret_cast<uint64_t*>(&value) =
          htobe64(*reinterpret_cast<uint64_t*>(&value));
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

/**
 * The base class for TagByteArray and TagIntArray.
 *
 * @see TagByteArray
 * @see TagIntArray
 */
template <typename T, TagType TAG_ENUM>
class __TagMetaArrayType__ : public BaseTag {
 public:
  /**
   * Get the stl vector that contains the values of the tag.
   * @return The stl vector that contains the values of the tag.
   */
  inline const std::vector<T>& GetValue() const {
    return *reinterpret_cast<const std::vector<T>*>(_value.void_ptr());
  }

  /**
   * Set the stl vector that contains the values of the tag (that you may modify
   * this tag's values).
   * @param value The stl vector that contains the values of the tag.
   */
  inline std::vector<T>& GetValue() {
    return *reinterpret_cast<std::vector<T>*>(_value.void_ptr());
  }

 public:
  /**
   * The constructor for the tag.
   */
  inline __TagMetaArrayType__() : BaseTag(TAG_ENUM) {}

  /**
   * The destructor for the tag.
   */
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
    T* data = array->data();
    for (size_t i = next_offset, j = 0; i < end_offset; i += sizeof(T), j++) {
      if (sizeof(T) == 2) {
        *reinterpret_cast<uint16_t*>(data + j) =
            be16toh(*reinterpret_cast<const uint16_t*>(buffer + i));
      } else if (sizeof(T) == 4) {
        *reinterpret_cast<uint32_t*>(data + j) =
            be32toh(*reinterpret_cast<const uint32_t*>(buffer + i));
      } else if (sizeof(T) == 8) {
        *reinterpret_cast<uint64_t*>(data + j) =
            be64toh(*reinterpret_cast<const uint64_t*>(buffer + i));
      } else {
        *reinterpret_cast<uint8_t*>(data + j) =
            *reinterpret_cast<const uint8_t*>(buffer + i);
      }
    }

    return sizeof(uint32_t) + array_size * sizeof(T);
  }

  /**
   * Get the stl vector that contains the values of the tag.
   * @return The stl vector that contains the values of the tag.
   */
  std::vector<T>* GetVector() {
    return reinterpret_cast<std::vector<T>*>(_value.void_ptr());
  }

  /**
   * Get the stl vector that contains the values of the tag.
   * @return The stl vector that contains the values of the tag.
   */
  const std::vector<T>* GetVector() const {
    return reinterpret_cast<const std::vector<T>*>(_value.void_ptr());
  }
};
/**
 * @}
 */

#pragma GCC diagnostic pop

}  // namespace nbtcc

#endif  // INCLUDE_BASE_TAG_H_
