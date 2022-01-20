#ifndef INCLUDE_TAGS_META_TAGS_H_
#define INCLUDE_TAGS_META_TAGS_H_

#include "base_tag.h"

namespace nbtcc {
typedef __TagMetaType__<int8_t, TagType::kTagByte> TagByte;
typedef __TagMetaType__<int16_t, TagType::kTagShort> TagShort;
typedef __TagMetaType__<int32_t, TagType::kTagInt> TagInt;
typedef __TagMetaType__<int64_t, TagType::kTagLong> TagLong;
typedef __TagMetaType__<float, TagType::kTagFloat> TagFloat;
typedef __TagMetaType__<double, TagType::kTagDouble> TagDouble;
}  // namespace nbtcc

#endif  // INCLUDE_TAGS_META_TAGS_H_
