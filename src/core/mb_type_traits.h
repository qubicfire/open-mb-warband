#ifndef _MB_TYPE_TRAITS_H
#define _MB_TYPE_TRAITS_H

#ifndef MB_SIGNELTON_INITIALIZE

	#define declare_default_class(type, name)			\
		extern inline type* g_##name = nullptr;			\

	#define declare_unique_class(type, name)			\
		extern inline Unique<type> g_##name = nullptr;	\
	
	#define create_global_class(type, name)			\
		static inline type __s__Instance_##name;		\
		extern inline type* g_##name = &__s__Instance_##name;

#endif // !MB_SIGNELTON_INITIALIZE

#include <memory>

template <class _Tx, class _Dx = std::default_delete<_Tx>>
using Unique = std::unique_ptr<_Tx, _Dx>;

template <class _Tx, class... _Args, 
	std::enable_if_t<!std::is_array_v<_Tx>, int> = 0>
inline Unique<_Tx> create_unique(_Args&&... args)
{
	return std::unique_ptr<_Tx>(new _Tx(std::forward<_Args>(args)...));
}

template <class _Tx, 
	std::enable_if_t<std::is_array_v<_Tx> && std::extent_v<_Tx> == 0, int> = 0>
inline Unique<_Tx> create_unique(const size_t size) 
{
	return std::unique_ptr<_Tx>(new std::remove_extent_t<_Tx>[size]());
}

template <class _Tx>
using Shared = std::shared_ptr<_Tx>;

template <class _Tx, class... _Args,
	std::enable_if_t<!std::is_array_v<_Tx>, int> = 0>
inline Shared<_Tx> create_shared(_Args&&... args)
{
	return std::shared_ptr<_Tx>(new _Tx(std::forward<_Args>(args)...));
}

template <class _Tx,
	std::enable_if_t<std::is_array_v<_Tx>&& std::extent_v<_Tx> == 0, int> = 0>
inline Shared<_Tx> create_shared(const size_t size)
{
	return std::shared_ptr<_Tx>(new std::remove_extent_t<_Tx>[size]());
}

// Attributes
#define _inline_ __forceinline
#define _inline_const_ _inline_ const
#define _no_discard_ [[nodiscard]]
#define _no_return_ [[noreturn]]
#define _implemented_

#define _deprecated_ [[deprecated]]

#define cast_offset(type, v)									\
	sizeof(type), reinterpret_cast<void*>(offsetof(type, v))	\

#define cast_offset_field(type, field) offsetof(type, v)
#define cast_size_field(type, field) sizeof(((type*)0)->field)

#endif // !_MB_TYPE_TRAITS_H
