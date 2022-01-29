#ifndef INCLUDE_TAGS_META_TAGS_H_
#define INCLUDE_TAGS_META_TAGS_H_

#include "base_tag.h"
#include "nbt_version.h"

namespace nbtcc {
/**
 * \defgroup Tags
 * @brief The NBT tags.
 * @{
 */
#define DECLARE_TAG_META_CLASS(raw_type, tag_type)                             \
  class tag_type : public __TagMetaType__<raw_type, TagType::k##tag_type> {}

/**
 * The tag class for \ref TagType::kTagByte.
 */
DECLARE_TAG_META_CLASS(int8_t, TagByte);
/**
 * The tag class for \ref TagType::kTagShort.
 */
DECLARE_TAG_META_CLASS(int16_t, TagShort);
/**
 * The tag class for \ref TagType::kTagInt.
 */
DECLARE_TAG_META_CLASS(int32_t, TagInt);
/**
 * The tag class for \ref TagType::kTagLong.
 */
DECLARE_TAG_META_CLASS(int64_t, TagLong);
/**
 * The tag class for \ref TagType::kTagFloat.
 */
DECLARE_TAG_META_CLASS(float, TagFloat);
/**
 * The tag class for \ref TagType::kTagDouble.
 */
DECLARE_TAG_META_CLASS(double, TagDouble);

#undef DECLARE_TAG_META_CLASS
/**
 * @}
 */
}  // namespace nbtcc

#endif  // INCLUDE_TAGS_META_TAGS_H_
