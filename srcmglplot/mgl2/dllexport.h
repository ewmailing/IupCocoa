
#ifndef MGL_EXPORT_H
#define MGL_EXPORT_H

#ifdef MGL_STATIC_DEFINE
#  define MGL_EXPORT
#  define MGL_NO_EXPORT
#else
#  ifndef MGL_EXPORT
#    ifdef mgl_EXPORTS
        /* We are building this library */
#      define MGL_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define MGL_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef MGL_NO_EXPORT
#    define MGL_NO_EXPORT 
#  endif
#endif

#ifndef MGL_DEPRECATED
#  define MGL_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef MGL_DEPRECATED_EXPORT
#  define MGL_DEPRECATED_EXPORT MGL_EXPORT MGL_DEPRECATED
#endif

#ifndef MGL_DEPRECATED_NO_EXPORT
#  define MGL_DEPRECATED_NO_EXPORT MGL_NO_EXPORT MGL_DEPRECATED
#endif

#define DEFINE_NO_DEPRECATED 0
#if DEFINE_NO_DEPRECATED
# define MGL_NO_DEPRECATED
#endif

#endif
