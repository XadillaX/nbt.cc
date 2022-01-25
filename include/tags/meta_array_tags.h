#ifndef INCLUDE_TAGS_META_ARRAY_TAGS_H_
#define INCLUDE_TAGS_META_ARRAY_TAGS_H_

#include "base_tag.h"

namespace nbtcc {
typedef __TagMetaArrayType__<int8_t, TagType::kTagByteArray> TagByteArray;
typedef __TagMetaArrayType__<int32_t, TagType::kTagIntArray> TagIntArray;
}  // namespace nbtcc

#endif  // INCLUDE_TAGS_META_ARRAY_TAGS_H_
