#ifndef INCLUDE_TAG_TYPE_H_
#define INCLUDE_TAG_TYPE_H_

namespace nbtcc {

enum TagType : unsigned char {
  kTagEnd = 0,
  kTagByte,
  kTagShort,
  kTagInt,
  kTagLong,
  kTagFloat,
  kTagDouble,
  kTagByteArray,
  kTagString,
  kTagList,
  kTagCompound,
  kTagIntArray,
  kTagMax,
};

}  // namespace nbtcc

#endif  // INCLUDE_TAG_TYPE_H_
