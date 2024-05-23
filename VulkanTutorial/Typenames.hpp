#if not defined fro_TYPENAMES_H
#define fro_TYPENAMES_H

#include <memory>
#include <functional>

namespace fro
{
	template<typename TYPE>
	using UniquePointer = std::unique_ptr<TYPE, std::function<void(TYPE*)>>;
}

#endif