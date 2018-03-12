#ifndef _TF_MATH_H
#define _TF_MATH_H

#if GOOGLE_CUDA
#endif

namespace Manta {

template< class T >
const T& max( const T& a, const T& b ) {
    return (a < b) ? b : a;
}

template< class T >
const T& min( const T& a, const T& b ) {
    return (a > b) ? b : a;
}

}

#endif // _TF_MATH_H
