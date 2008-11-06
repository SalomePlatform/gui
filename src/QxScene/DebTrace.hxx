#ifndef __DEBTRACE_HXX__
#define __DEBTRACE_HXX__

#include <iostream>

#ifdef _DEVDEBUG_
#define DEBTRACE(msg) {std::cerr<<std::flush<<__FILE__<<" ["<<__LINE__<<"] : "<<msg<<std::endl<<std::flush;}
#else
#define DEBTRACE(msg)
#endif

#endif
