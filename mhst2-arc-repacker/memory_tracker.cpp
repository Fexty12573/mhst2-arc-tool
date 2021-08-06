
#include "memory_tracker.h"

size_t memory_tracker::_current_alloc_size = 0;
size_t memory_tracker::_peak_alloc_size = 0;

size_t memory_tracker::_new_count = 0;
size_t memory_tracker::_delete_count = 0;

_NODISCARD _Ret_notnull_ _Post_writable_byte_size_(_Size) _VCRT_ALLOCATOR
void* __CRTDECL operator new(size_t _Size)
{
	return memory_tracker::_alloc(_Size);
}

_NODISCARD _Ret_notnull_ _Post_writable_byte_size_(_Size) _VCRT_ALLOCATOR
void* __CRTDECL operator new[](size_t _Size)
{
	return memory_tracker::_alloc_buf(_Size);
}

void __CRTDECL operator delete(void* _Block) noexcept
{
	return memory_tracker::_dealloc(_Block);
}

void __CRTDECL operator delete[](void* _Block) noexcept
{
	return memory_tracker::_dealloc_buf(_Block);
}

size_t memory_tracker::get_current_heap_usage()
{
	return memory_tracker::_current_alloc_size;
}

size_t memory_tracker::get_peak_heap_usage()
{
	return memory_tracker::_peak_alloc_size;
}

size_t memory_tracker::get_new_count()
{
	return memory_tracker::_new_count;
}
size_t memory_tracker::get_delete_count()
{
	return memory_tracker::_delete_count;
}
