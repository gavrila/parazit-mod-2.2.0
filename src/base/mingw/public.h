#ifndef BASE_MINGW_PUBLIC_H
#define BASE_MINGW_PUBLIC_H

///////////////////////////////////////////////////////////////////////////////

#define LF_PUBLIC __declspec(dllexport)
#define LF_PRIVATE

// apparently not required for mingw as it is a built-in
#if 0
#define WIN32
#endif

///////////////////////////////////////////////////////////////////////////////

#endif // BASE_MINGW_PUBLIC_H
