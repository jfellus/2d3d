/*
Copyright © CNRS 2012. 
Authors: David Picard, Philippe-Henri Gosselin, Romain Negrel, Hedi 
Tabia
Contact: picard@ensea.fr

This software is governed by the CeCILL license under French law and
abiding by the rules of distribution of free software.  You can  use, 
modify and/ or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info". 

As a counterpart to the access to the source code and rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability. 

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or 
data to be ensured and,  more generally, to use and operate it in the 
same conditions as regards security. 

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.

*/
/**
 * \file IntegralImage.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_IntegralImage_h__
#define __retin_IntegralImage_h__

#include "retin/toolbox/document/Document.h"
#include "retin/toolbox/document/BoxLabel.h"

namespace retin
{

class	IntegralImage : public Document
{
public:

virtual	uint	dim() const = 0;
virtual	uint	width() const = 0;
virtual	uint	height() const = 0;
virtual	float	value(int i,int j,int k) const = 0;
virtual	float	getCount (int i1,int j1,int n,int m,int k) const {
	    float result = value(i1+n,j1+m,k);
	    result -= value(i1,j1+m,k);
	    result -= value(i1+n,j1,k);
	    result += value(i1,j1,k);
	    return result;
	}
virtual	float	getCount (float i1,float j1,float n,float m,int k) const {
	    return getCount((int)(i1*width()),(int)(j1*height()),(int)(n*width()),(int)(m*height()),k);
	}
virtual	float	searchCount (int width,int height,int bin) const = 0;
virtual	void	searchBox (BoxLabel& box,int width,int height,int bin) const = 0;
virtual bool	searchBox (BoxLabel& box,int width,int height,int k,int x1,int y1,int x2,int y2) const = 0;



};

typedef boost::shared_ptr<IntegralImage>	IntegralImageRef;

}



#endif
