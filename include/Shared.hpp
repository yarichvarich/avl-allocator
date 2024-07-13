#pragma once

#include <memory>

namespace Yaro
{
namespace Utility
{

template <typename Derived>
class Shared
{
  public:
    Shared() = default;
    virtual ~Shared() = default;
    using Ptr = std::shared_ptr<Derived>;
};

} // namespace Utility
} // namespace Yaro