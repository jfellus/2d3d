/**
 * \file BoxLabel.cpp
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#include "retin/toolbox/document/BoxLabel.h"

#include "retin/toolbox/document/serialization/Serializers.h" 

RETIN_SERIALIZABLE_OBJECT_DEFINITIONS(BoxLabel);

using namespace std;

namespace retin {

void	BoxLabel::draw(float* image,size_t width,size_t height) const
{
	for (int y=y1;y<y2;y++) {
	    if (y >=0 && y < (int)height) {
		for (int x=x1;x<x2;x++) {
		    if (x >= 0 && x < (int)width)
			image[x+y*width] ++;
		}
	    }
	}
}

void	BoxLabel::show() const
{
	cout << "(" << x1 << "," << y1 << ";" << x2 << "," << y2 << ")" << endl;
}

}