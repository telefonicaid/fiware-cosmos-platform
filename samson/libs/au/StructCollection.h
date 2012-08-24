

#ifndef _H_AU_STRUCT_COLLECTION
#define _H_AU_STRUCT_COLLECTION

namespace au {
template <typename C>
class StructCollection {
public:

  C *v;                     // Global vector with all structs
  size_t size;              // Current number of instances
  size_t max_size;          // Alloc-size of the vector of structs

  StructCollection() {
    v = NULL;
    size = 0;
    max_size = 0;
  }

  ~StructCollection() {
    if (v) {
      free(v);
    }
  }

  void add(C c) {
    if (size == max_size) {
      // Alloc new space
      if (max_size == 0) {
        max_size = 2;
      } else {
        max_size *= 2;
      }

      C *vv = (C *)malloc(max_size * sizeof(C));
      if (size > 0) {
        memcpy(vv, v, size * sizeof(C));
        free(v);
      }

      v = vv;
    }

    v[size++] = c;
  }

  size_t getSize() {
    return size * sizeof(C);
  }

  size_t write(FILE *file) {
    size_t t = fwrite(v, size * sizeof( C ), 1, file);

    if (t == 1) {
      return size * sizeof( C );
    } else {
      return 0;
    }
  }

  void read(FILE *file, size_t _size) {
    // Free previous buffer if any
    if (v) {
      free(v);
    }

    size = _size / sizeof(C);

    if (size * sizeof(C) != _size) {
      LM_X(1, ("Problem reading %lu bytes for structs of size %lu", _size, sizeof(C)));
    }

    v = static_cast<C *>(malloc(_size * sizeof(C)));
    if (v == NULL) {
      // Error detected when running comscore on a machine with small memory (VM)
      LM_E((
             "Error reserving memory for %lu elements of size %lu (total: %lu bytes) for reading a StructCollection from file",
             _size, sizeof(C), _size * sizeof(C)));
      exit(1);
    }
    max_size = _size;

    // Read content from file
    size_t s = fread(v, _size, 1, file);
    if (s != 1) {
      LM_W(("Error reading StructColleciton"));
    }
  }
};
}

#endif  // ifndef _H_AU_STRUCT_COLLECTION
