#pragma once

class NonMove {
  public:
    NonMove(const NonMove &&) = delete;
    NonMove &operator=(const NonMove &&) = delete;

  protected:
    NonMove() = default;
    ~NonMove() = default;
};
