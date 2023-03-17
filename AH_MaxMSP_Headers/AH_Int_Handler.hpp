
#ifndef _AH_INT_HANDLER_HPP_
#define _AH_INT_HANDLER_HPP_

#include <ext.h>
#include <algorithm>
#include <limits>

template <class T, class U = T>
T limit_int(t_atom_long a)
{
    t_atom_long limit = static_cast<t_atom_long>(std::numeric_limits<U>::max());
    return static_cast<long>(std::min(a, limit));
}

#endif /* _AH_INT_HANDLER_HPP_ */
