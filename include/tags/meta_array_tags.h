#ifndef INCLUDE_TAGS_META_ARRAY_TAGS_H_
#define INCLUDE_TAGS_META_ARRAY_TAGS_H_

#include "base_tag.h"
#include "nbt_version.h"

namespace nbtcc {
/**
 * \defgroup Tags
 * @brief The NBT tags.
 * @{
 */
#define DECLARE_TAG_META_ARRAY_CLASS(raw_type, element_tag_type)               \
  class element_tag_type##Array                                                \
      : public __TagMetaArrayType__<raw_type,                                  \
                                    TagType::k##element_tag_type##Array> {     \
   public:                                                                     \
    inline element_tag_type##Array()                                           \
        : __TagMetaArrayType__<raw_type,                                       \
                               TagType::k##element_tag_type##Array>() {}       \
  }

/**
 * The tag class for \ref TagType::kTagByteArray.
 */
DECLARE_TAG_META_ARRAY_CLASS(int8_t, TagByte);

/**
 * The tag class for \ref TagType::kTagIntArray.
 */
DECLARE_TAG_META_ARRAY_CLASS(int32_t, TagInt);
/**
 * @}
 */
}  // namespace nbtcc

#endif  // INCLUDE_TAGS_META_ARRAY_TAGS_H_
