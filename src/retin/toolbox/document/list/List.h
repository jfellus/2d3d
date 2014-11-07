/**
 * \file List.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_List_h__
#define __retin_List_h__

#include "../Document.h"

namespace retin
{

class	List : public Document
{
public:
virtual	jint		size() const = 0;
virtual	jint		dim() const  = 0;

};

typedef boost::shared_ptr<List> ListRef;

}

#endif

