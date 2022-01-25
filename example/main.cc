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
      cout << " Byte("
           << static_cast<int32_t>(tag.AsPtr<nbtcc::TagByte>()->GetValue())
           << ")" << endl;
      break;

    case nbtcc::TagType::kTagShort:
      cout << " Short("
           << static_cast<int32_t>(tag.AsPtr<nbtcc::TagShort>()->GetValue())
           << ")" << endl;
      break;

    case nbtcc::TagType::kTagInt:
      cout << " Int(" << tag.AsPtr<nbtcc::TagInt>()->GetValue() << ")" << endl;
      break;

    case nbtcc::TagType::kTagLong:
      cout << " Long(" << tag.AsPtr<nbtcc::TagLong>()->GetValue() << ")"
           << endl;
      break;

    case nbtcc::TagType::kTagFloat:
      cout << " Float(" << tag.AsPtr<nbtcc::TagFloat>()->GetValue() << ")"
           << endl;
      break;

    case nbtcc::TagType::kTagDouble:
      cout << " Double(" << tag.AsPtr<nbtcc::TagDouble>()->GetValue() << ")"
           << endl;
      break;

    case nbtcc::TagType::kTagByteArray: {
      cout << endl;
      const nbtcc::TagByteArray* array = tag.AsPtr<nbtcc::TagByteArray>();
      const vector<int8_t>& bytes = array->GetValue();
      for (size_t i = 0; i < bytes.size(); ++i) {
        for (size_t j = 0; j < level + 1; ++j) {
          cout << "  ";
        }

        cout << "- " << static_cast<int32_t>(bytes[i]) << endl;
      }
      break;
    }

    case nbtcc::TagType::kTagIntArray: {
      cout << endl;
      const nbtcc::TagIntArray* array = tag.AsPtr<nbtcc::TagIntArray>();
      const vector<int32_t>& ints = array->GetValue();
      for (size_t i = 0; i < ints.size(); ++i) {
        for (size_t j = 0; j < level + 1; ++j) {
          cout << "  ";
        }

        cout << "- " << ints[i] << endl;
      }
      break;
    }

    case nbtcc::TagType::kTagString:
      cout << " String(\"" << tag.AsPtr<nbtcc::TagString>()->ToString() << "\")"
           << endl;
      break;

    case nbtcc::TagType::kTagCompound: {
      if (!list_children) cout << endl;
      vector<string> keys;
      const nbtcc::TagCompound* compound = tag.AsPtr<nbtcc::TagCompound>();
      compound->GetChildKeys(&keys);
      for (size_t i = 0; i < keys.size(); ++i) {
        const shared_ptr<nbtcc::BaseTag> child = compound->GetChild(keys[i]);
        if (child.get()) {
          Dump(
              *child.get(), level + 1, false, i == 0 && list_children ? 1 : -1);
        } else {
          cout << "ERROR: Child not found" << endl;
        }
      }

      break;
    }

    case nbtcc::TagType::kTagList: {
      cout << endl;
      const nbtcc::TagList* list = tag.AsPtr<nbtcc::TagList>();
      size_t size = list->GetChildCount();
      for (size_t i = 0; i < size; ++i) {
        const shared_ptr<nbtcc::BaseTag> child = list->GetChild(i);
        if (child.get()) {
          Dump(*child.get(), level + 1, true);
        } else {
          cout << "ERROR: Child not found" << endl;
        }
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
