/**
 * @file Types.hpp
 * @author Nathan Bourgeois <iridescentrosesfall@gmail.com>
 * @brief Defines helpful utility types / renames
 * @version 0.1
 * @date 2021-12-11
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#pragma once
#include <memory>
#include <cstdint>

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
     * ScopePtr or Scoped Pointers are pointers which exist during a certain scope of time
     */
    template<typename T>
    using ScopePtr = std::unique_ptr<T>;

    /**
     * RefPtr or Referenced Pointers are pointers which will exist until all references fall out of scope.
     */
    template<typename T>
    using RefPtr = std::shared_ptr<T>;

    template<typename T, typename ... Args>
    /**
     * Creates a ScopePtr Object
     * @tparam T  - Type of the ScopePtr
     * @tparam Args - The args needed
     * @param args  - Passes the args for the constructor
     * @return  - Return new ScopePtr of type T
     */
    constexpr ScopePtr<T> createScopePtr(Args&& ... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }


    template<typename T, typename ... Args>
    /**
     * Creates a RefPtr Object
     * @tparam T - Type of the RefPtr
     * @tparam Args - The args needed
     * @param args - Passes the args for the constructor
     * @return - Return new RefPtr of type T
     */
    constexpr RefPtr<T> createRefPtr(Args&& ... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
}
