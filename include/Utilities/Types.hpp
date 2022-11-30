#pragma once
#include <any>
#include <cstdint>
#include <memory>

#define UNUSED(x) (void)(x)

/**
 * Unsigned Types
 */
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

/**
 * Signed Types
 */
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

/**
 * Define Scoped and Reference Pointers (Unique / Shared)
 */
namespace Stardust_Celeste {

/**
 * ScopePtr or Scoped Pointers are pointers which exist during a certain scope
 * of time
 */
template <typename T> using ScopePtr = std::unique_ptr<T>;

/**
 * RefPtr or Referenced Pointers are pointers which will exist until all
 * references fall out of scope.
 */
template <typename T> using RefPtr = std::shared_ptr<T>;

template <typename T, typename... Args>
/**
 * Creates a ScopePtr Object
 * @tparam T  - Type of the ScopePtr
 * @tparam Args - The args needed
 * @param args  - Passes the args for the constructor
 * @return  - Return new ScopePtr of type T
 */
constexpr ScopePtr<T> create_scopeptr(Args &&...args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T, typename... Args>
/**
 * Creates a RefPtr Object
 * @tparam T - Type of the RefPtr
 * @tparam Args - The args needed
 * @param args - Passes the args for the constructor
 * @return - Return new RefPtr of type T
 */
constexpr RefPtr<T> create_refptr(Args &&...args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

/**
 * @brief A command (void function) made to be executable
 *
 */
struct Command {
    void (*func)(std::any);
    std::any data;
};

} // namespace Stardust_Celeste
