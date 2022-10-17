#pragma once

/**
 * @brief Class without copy constructors
 *
 */
class NonCopy {
  public:
    NonCopy(const NonCopy &) = delete;
    NonCopy &operator=(const NonCopy &) = delete;

  protected:
    NonCopy() = default;
    ~NonCopy() = default;
};
