#pragma once

#include <new>

class memory_tracker
{
private:
	static size_t _current_alloc_size;
	static size_t _peak_alloc_size;

	static size_t _new_count;
	static size_t _delete_count;

#define __size_type_deref(__block, __idx) (((size_t*)__block)[__idx])
#define __size_type_off(__block) (&__size_type_deref(__block, 1))
#define __ensure_notnull(__block, __op) if(__block!=nullptr)__op
#define __ensure_notnull_offset(__block) ((__block!=nullptr)?__size_type_off(__block):__block)
#define __ensure_notnull_origin(__block) ((__block!=nullptr)?&__size_type_deref(__block, -1):__block)
#define __ensure_notnull_store_size(__block, __size) __ensure_notnull(__block, __size_type_deref(__block, 0) = __size)
#define __ensure_notnull_retrieve_size(__block) ((__block!=nullptr)?__size_type_deref(__block, -1):0)
public:
	memory_tracker() = delete;
	memory_tracker(const memory_tracker&) = delete;
	memory_tracker(memory_tracker&&) = delete;
	memory_tracker& operator=(const memory_tracker&) = delete;
	memory_tracker& operator=(memory_tracker&&) = delete;

	static size_t get_current_heap_usage();
	static size_t get_peak_heap_usage();

	static size_t get_new_count();
	static size_t get_delete_count();

	static inline void reset()
	{
		_current_alloc_size = 0;
		_peak_alloc_size = 0;
	}

	static inline void* _alloc(size_t _size)
	{
		++_new_count;
		_current_alloc_size += _size;
		if (_current_alloc_size > _peak_alloc_size)
			_peak_alloc_size = _current_alloc_size;

		void* _block = __vcrt_malloc_normal(_size + sizeof(size_t));
		__ensure_notnull_store_size(_block, _size);

		return __ensure_notnull_offset(_block);
	}
	static inline void* _alloc_buf(size_t _size)
	{
		++_new_count;
		_current_alloc_size += _size;
		if (_current_alloc_size > _peak_alloc_size)
			_peak_alloc_size = _current_alloc_size;

		void* _block = __vcrt_malloc_normal(_size + sizeof(size_t));
		__ensure_notnull_store_size(_block, _size);

		return __ensure_notnull_offset(_block);
	}

	static inline void _dealloc(void* _block)
	{
		size_t _size = __ensure_notnull_retrieve_size(_block);
		++_delete_count;
		_current_alloc_size -= _size;
		return __vcrt_free_normal(__ensure_notnull_origin(_block));
	}
	static inline void _dealloc_buf(void* _block)
	{
		size_t _size = __ensure_notnull_retrieve_size(_block);
		++_delete_count;
		_current_alloc_size -= _size;
		return __vcrt_free_normal(__ensure_notnull_origin(_block));
	}
};
