namespace mantatensor {

#ifndef _MT_UTIL_H
#define _MT_UTIL_H

#if GOOGLE_CUDA == 0

#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

#endif

#endif // _MT_UTIL_H

} // namespace
