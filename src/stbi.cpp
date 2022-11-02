#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_SIMD
#include <stb_image.hpp>

#if USE_EASTL
void *operator new[](size_t size, const char *name, int flags,
                     unsigned debugFlags, const char *file, int line) {
    return new uint8_t[size];
}

void *operator new[](size_t size, size_t alignment, size_t alignmentOffset,
                     const char *pName, int flags, unsigned debugFlags,
                     const char *file, int line) {
    return new uint8_t[size];
}
#endif