/*
 * This file is part of libgreat
 *
 * Toolchain helper functions.
 */

#ifndef __LIBGREAT_TOOLCHAIN_H__
#define __LIBGREAT_TOOLCHAIN_H__

#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * Generic attribute wrappers.
 */
#define ATTR_PACKED       __attribute__((packed))
#define ATTR_ALIGNED(x)	  __attribute__((aligned(x)))  //FIXME: use alignas?
#define ATTR_WORD_ALIGNED __attribute__((packed,aligned(4)))
#define ATTR_SECTION(x)   __attribute__((section(x)))
#define ATTR_WEAK         __attribute__((weak))
#define ATTR_NORETURN     __attribute__((noreturn))
#define ATTR_NAKED        __attribute__((naked))
#define ATTR_PRINTF       __attribute__((format (printf, 1, 2)))
#define ATTR_PRINTF_N(n)  __attribute__((format (printf, n, n + 1)))
#define ATTR_USED         __attribute__((used))


/**
 * Attribute helpers for variables that should persist across a reset.
 */
#define ATTR_PERSISTENT ATTR_SECTION(".bss.persistent")


/**
 * Macros for populating the preinit_array and init_array
 * headers -- which are automatically executed in by our crt0 during startup.
 */
#define CALL_ON_PREINIT(preinit) \
	__attribute__((section(".preinit_array"), used)) static typeof(preinit) *preinit##_initcall_p = preinit;
#define CALL_ON_INIT(init) \
	__attribute__((section(".init_array"), used)) static typeof(init) *init##_initcall_p = init;

/* Macros for populating our finializers. */
#define CALL_BEFORE_RESET(fini) \
	__attribute__((section(".fini_array"), used)) static typeof(init) *fini##_finalizer_p = fini;

/**
 * Macros for simple scheduler support. This uses the same type of linker magic as .init/.fini to populate
 * a simple array of pointers that we can easily iterate over.
 */
#define DEFINE_TASK(task) \
	__attribute__((section(".task_array"), used)) static typeof(task) *task##_implementation_p = task;


/**
 * Compile-time error detection.
 */

// Compatibility stub for things that don't support static_assert.
// Ignore these assertions entirely.
#ifndef static_assert
#define static_assert(condition, msg)
#endif

// Helpers for validating struct member alignment.
#define ASSERT_OFFSET(structure, member, offset) \
	static_assert(offsetof(structure, member) == offset, #member " is not at offset " #offset " in struct " #structure)

// Slightly-horrific hack that causes a compiler to emit an error message with a given offset at compile time.
// Useful for debugging failed ASSERT_OFFSET, but otherwise you should never use this macro.
#define ERROR_MESSAGE_WITH_OFFSET(structure, member) char (*_failure)[offsetof(structure, member )] = 1;

#define _CONCAT_TOKENS(a, b)   a##b
#define _CONCAT(a, b) _CONCAT_TOKENS(a, b)

/**
 * Helpers for defining structs that match hardware register layouts.
 */
#define RESERVED_BYTES(n) uint8_t  _CONCAT(reserved, __LINE__) [n]
#define RESERVED_WORDS(n) uint32_t _CONCAT(reserved, __LINE__)[n]


/**
 * Generic helpers.
 */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define CONTAINER_OF(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})
#define MIN(a,b) \
({ __typeof__ (a) _a = (a); \
   __typeof__ (b) _b = (b); \
 _a < _b ? _a : _b; })


/**
 * @returns the "order" (log2) of a binary-sized buffer
 */
static inline uint8_t size_to_order(uint32_t size)
{
	return 31 - __builtin_clz(size);
}

/**
 * @returns true iff the given buffer is binary sized
 */
static inline bool is_binary_sized(uint32_t size)
{
	return (size == (1UL << size_to_order(size)));
}




#ifdef __clang__
#include <toolchain_clang.h>
#endif

#ifdef __GNUC__
#include <toolchain_gcc.h>
#endif





#endif
