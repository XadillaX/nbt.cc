#include <nbt.h>
#include <stdio.h>
#include <iostream>

using std::cout;
using std::endl;
using std::shared_ptr;
using std::string;
using std::vector;

void Dump(const nbtcc::BaseTag& tag,
          size_t level,
          bool list_children = false,
          int force_indent = -1) {
  if (force_indent < 0) {
    for (size_t i = 0; i < level; ++i) {
      cout << "  ";
    }
  } else {
    for (int i = 0; i < force_indent; ++i) {
      cout << " ";
    }
  }

  if (list_children) {
    cout << "-";
  } else {
    cout << tag.name() << ":";
  }
  switch (tag.type()) {
    case nbtcc::TagType::kTagByte:
      cout << " Byte(" << static_cast<int32_t>(tag.value().ByteValue()) << ")"
           << endl;
      break;

    case nbtcc::TagType::kTagShort:
      cout << " Short(" << static_cast<int32_t>(tag.value().ShortValue()) << ")"
           << endl;
      break;

    case nbtcc::TagType::kTagInt:
      cout << " Int(" << tag.value().IntValue() << ")" << endl;
      break;

    case nbtcc::TagType::kTagLong:
      cout << " Long(" << tag.value().LongValue() << ")" << endl;
      break;

    case nbtcc::TagType::kTagFloat:
      cout << " Float(" << tag.value().FloatValue() << ")" << endl;
      break;

    case nbtcc::TagType::kTagDouble:
      cout << " Double(" << tag.value().DoubleValue() << ")" << endl;
      break;

    case nbtcc::TagType::kTagByteArray:
      cout << endl;
      for (size_t i = 0; i < tag.value().ByteArrayValue().size(); ++i) {
        for (size_t j = 0; j < level + 1; ++j) {
          cout << "  ";
        }

        cout << "- " << static_cast<int32_t>(tag.value().ByteArrayValue()[i])
             << endl;
      }
      break;

    case nbtcc::TagType::kTagIntArray:
      cout << endl;
      for (size_t i = 0; i < tag.value().IntArrayValue().size(); ++i) {
        for (size_t j = 0; j < level + 1; ++j) {
          cout << "  ";
        }

        cout << "- " << tag.value().IntArrayValue()[i] << endl;
      }
      break;

    case nbtcc::TagType::kTagString:
      cout << " String(\"" << tag.value().StringValue() << "\")" << endl;
      break;

    case nbtcc::TagType::kTagCompound: {
      if (!list_children) cout << endl;

      const std::map<std::string, shared_ptr<nbtcc::BaseTag>>* children =
          reinterpret_cast<
              const std::map<std::string, shared_ptr<nbtcc::BaseTag>>*>(
              tag.value().void_ptr());
      bool first = true;
      for (const auto& child : *children) {
        Dump(*child.second, level + 1, false, first && list_children ? 1 : -1);
        first = false;
      }
      break;
    }

    case nbtcc::TagType::kTagList: {
      cout << endl;
      const std::vector<shared_ptr<nbtcc::BaseTag>>* children =
          reinterpret_cast<const std::vector<shared_ptr<nbtcc::BaseTag>>*>(
              tag.value().void_ptr());
      for (const auto& child : *children) {
        Dump(*child, level + 1, true);
      }
      break;
    }

    default:
      cout << " __Type(" << tag.type() << ")" << endl;
      break;
  }
}

int main(int argc, char** argv) {
  nbtcc::NBT nbt;
  if (argc < 2) {
    cout << "Usage: " << argv[0] << " <file.nbt>\n";
    return 1;
  }

  char* filename = argv[1];
  FILE* f = fopen(filename, "rb");
  if (!f) {
    cout << "Could not open file " << filename << "\n";
    return 1;
  }

  fseek(f, 0, SEEK_END);
  size_t size = ftell(f);
  fseek(f, 0, SEEK_SET);
  char* data = new char[size];
  fread(data, 1, size, f);
  fclose(f);

  cout << "File size: " << size << "\n";
  if (!nbt.Load(data, size)) {
    cout << "Could not load NBT data\n";
    return 1;
  }

  vector<string> root_keys;
  nbt.RootKeys(&root_keys);
  for (size_t i = 0; i < root_keys.size(); i++) {
    shared_ptr<nbtcc::BaseTag> tag = nbt.GetFromRoot(root_keys[i]);
    assert(tag.get());
    Dump(*tag, 0);
  }

  return 0;
}
