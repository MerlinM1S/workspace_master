namespace mantatensor {

#ifndef _MT_UTIL_H
#define _MT_UTIL_H

#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

#endif // _MT_UTIL_H

} // namespace
