/**
 * \file Indexes.cpp
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#include "basic/Indexes.h"

#include "../serialization/Serializers.h"

RETIN_SERIALIZABLE_OBJECT_DEFINITIONS(Indexes);

using namespace std;

namespace retin {

void    Indexes::show() const {
    cout << "Indexes" << endl;
    for (size_t i=0;i<(size_t)nLength;i++)
        cout << vData[i] << " ";
    cout << endl;
}

}

