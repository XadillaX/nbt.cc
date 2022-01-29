#ifndef INCLUDE_TAG_TYPE_H_
#define INCLUDE_TAG_TYPE_H_

#include "nbt_version.h"

namespace nbtcc {

/**
 * \defgroup Tags
 * @brief The NBT tags.
 * @{
 */
/**
 * The tag types.
 *
 * @see BaseTag
 * @see https://minecraft.fandom.com/wiki/NBT_format#TAG_definition
 */
enum TagType : unsigned char {
  kTagEnd = 0,  ///< Used to mark the end of compound tags. This tag does not
                ///< have a name, so it is only ever a single byte 0. It may
                ///< also be the type of empty List tags.
  kTagByte,     ///< A signed integral type. Sometimes used for booleans. Full
                ///< range of \f$-(2^7)\f$ to \f$(2^7-1)\f$.\n
                ///< \f$(-128\ to\ 127)\f$
  kTagShort,    ///< A signed integral type. Full range of \f$-(2^{15})\f$ to
                ///< \f$(2^{15} - 1)\f$.\n \f$(-32,768\ to\ 32,767)\f$
  kTagInt,      ///< A signed integral type. Full range of \f$-(2^{31})\f$ to
                ///< \f$(2^{31} - 1)\f$.\n
                ///< \f$(-2,147,483,648\ to \ 2,147,483,647)\f$
  /**
   * A signed integral type. Full range of \f$-(2^{63})\f$ to
   * \f$(2^{63} - 1)\f$.\n
   * \f$(-9,223,372,036,854,775,808\ to \ 9,223,372,036,854,775,807)\f$
   */
  kTagLong,
  kTagFloat,  ///< A signed floating point type. Precision varies throughout
              ///< number line;\n See [Single-precision floating-point
              ///< format](https://en.wikipedia.org/wiki/Single-precision_floating-point_format).
              ///< Maximum value about \f$3.4 * 10^{38}\f$
  kTagDouble,  ///< A signed floating point type. Precision varies
               ///< throughout number line;\n See [Double-precision
               ///< floating-point
               ///< format](https://en.wikipedia.org/wiki/Double-precision_floating-point_format).
               ///< Maximum value about \f$1.8 * 10^{308}\f$
  kTagByteArray,  ///< An array of bytes.
  kTagString,     ///< A UTF-8 string. It has a size, rather than being null
               ///< terminated. \f$65,535\f$ bytes interpretable as UTF-8 (see
               ///< [modified UTF-8
               ///< format](https://en.wikipedia.org/wiki/UTF-8#Modified_UTF-8);
               ///< most commonly-used characters are a single byte).
  kTagList,  ///< A list of tag payloads, without repeated tag IDs or any tag
             ///< names.
  kTagCompound,  ///< A list of fully formed tags, including their IDs, names,
                 ///< and payloads. No two tags may have the same name.
  kTagIntArray,  ///< An array of TAG_Int's payloads.
  kTagMax,       ///< The maximum tag type. (not a valid tag type)
};
/**
 * @}
 */

}  // namespace nbtcc

#endif  // INCLUDE_TAG_TYPE_H_
