#pragma once
#include "ctti/type_id.hpp"

namespace TypeInfo
{
    using type = ctti::type_id_t;
	
    template <typename T>
    constexpr type type_name() noexcept
	{
        return ctti::type_id<T>();
    }
}
