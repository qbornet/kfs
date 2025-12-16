#ifndef SHARED_H
#define SHARED_H
#define __glibc_has_attribute(attr) __has_attribute(attr)
#if __glibc_has_attribute(__always_inline__)
#undef __always_inline
#define __always_inline inline __attribute__((always_inline))
#endif
#endif
