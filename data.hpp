#ifndef __MORLOC_BASE_CPP_DATA_HPP__
#define __MORLOC_BASE_CPP_DATA_HPP__

#include <cassert>
#include "mlccpptypes/prelude.hpp"

// packUnit Cpp :: pack => "mlc::Unit" -> "int"
mlc::Unit morloc_packUnit(int u){
    return mlc::Unit();
}

// unpackUnit Cpp :: unpack => "int" -> "mlc::Unit"
int morloc_unpackUnit(mlc::Unit x){
    return 0;
}

#endif
