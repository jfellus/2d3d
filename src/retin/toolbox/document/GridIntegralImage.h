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
 * \file GridIntegralImage.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_GridIntegralImage_h__
#define __retin_GridIntegralImage_h__

#include "retin/toolbox/document/IntegralImage.h"
#include "retin/toolbox/core/auto_array_ptr.h"

#include <math.h>

namespace retin
{

class	GridIntegralImage : public IntegralImage
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(retin/toolbox/document,GridIntegralImage);
protected:
	retin::auto_array_ptr<jfloat>	vData;
	jint			nDim;
	jint			nImageWidth,nImageHeight;
	jint			nGridWidth,nGridHeight;
	jint			fxSampling,fySampling;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, GridIntegralImage& x)
	{
		s.object ("dim",x.nDim);
		s.object ("imageWidth",x.nImageWidth);
		s.object ("imageHeight",x.nImageHeight);
		s.object ("gridWidth",x.nGridWidth);
		s.object ("gridHeight",x.nGridHeight);
	       	s.object ("xSampling",x.fxSampling);
		s.object ("ySampling",x.fySampling);
		s.matrix ("counts",x.vData,x.nDim,x.nGridWidth*x.nGridHeight);
	}


public:
	GridIntegralImage (int nGridWidth=0,int nGridHeight=0,int nDim=1,int fxSampling=1,int fySampling=1) : vData(nDim*nGridWidth*nGridHeight),nDim(nDim),nGridWidth(nGridWidth),nGridHeight(nGridHeight),fxSampling(fxSampling),fySampling(fySampling) { }

	uint	dim() const { return nDim; }
	uint	width() const { return nImageWidth; }
	uint	height() const { return nImageHeight; }
	float	value (int i,int j,int k) const {
		int si = i/fxSampling;
		int sj = j/fySampling;
		float* pData = vData.get(k+nDim*(si+sj*nGridWidth));
		float x = i%fxSampling; x /= fxSampling;
		float y = j%fySampling; y /= fySampling;
		return pData[0] * (1-x) * (1-y)	+ pData[nDim] * x * (1-y) + pData[nDim*nGridWidth] * (1-x) * y + pData[nDim*(1+nGridWidth)] * x * y;
	}
	void	closestBox (BoxLabel& box,int x1,int y1,int x2,int y2) const;
	void	closestBox (BoxLabel& box,float x1,float y1,float x2,float y2) const {
		closestBox(box,(int)(x1*nImageWidth),(int)(y1*nImageHeight),(int)(x2*nImageWidth),(int)(y2*nImageHeight));
	}
	float	closestCount(int x1,int y1,int x2,int y2,int k) const;
	float	closestCount(float x1,float y1,float x2,float y2,int k) const {
		return closestCount((int)(x1*nImageWidth),(int)(y1*nImageHeight),(int)(x2*nImageWidth),(int)(y2*nImageHeight),k);
	}


virtual	float	searchCount (int width,int height,int bin) const;
virtual	void	searchBox (BoxLabel& box,int width,int height,int bin) const;
virtual bool	searchBox (BoxLabel& box,int width,int height,int k,int x1,int y1,int x2,int y2) const;

	class	iterator {
		const GridIntegralImage* ii;
		float* pData;
		int n,m,si,sj;
		int minI,maxI,minJ,maxJ;
	public:
		int x,y,width,height;
		BoxLabel    boxLabel() const { return BoxLabel(x,y,x+width,y+height); }
		iterator() : ii(NULL),pData(NULL),n(-1),m(-1),si(-1),sj(-1),minI(-1),maxI(-1),minJ(-1),maxJ(-1) { }
		iterator(const GridIntegralImage* ii,int w,int h) : ii(ii) {
		    n = (int) (w / ii->fxSampling);
		    m = (int) (h / ii->fySampling);

		    if (n >= ii->nGridWidth) n = ii->nGridWidth-1;
		    if (m >= ii->nGridHeight) m = ii->nGridHeight-1;

		    x = 0;
		    y = 0;
		    width = n*ii->fxSampling;
		    height = m*ii->fySampling;

		    minI = 0;
		    maxI = ii->nGridWidth-n;
		    minJ = 0;
		    maxJ = ii->nGridHeight-m;

		    si = minI;
		    sj = minJ;
		    pData = ii->vData.get(ii->nDim*(si+sj*ii->nGridWidth));
		}
		float count(int k) {
		    return pData[k] + pData[k+ii->nDim*(n+m*ii->nGridWidth)]
		    - pData[k+ii->nDim*n] - pData[k+ii->nDim*m*ii->nGridWidth];
		}
		bool  next() {
		    si ++;
		    if (si < maxI) {
			x += ii->fxSampling;
			pData += ii->nDim;
			return true;
		    }
		    si = minI;
		    sj ++;
		    if (sj < maxJ) {
			x = minI*ii->fxSampling;
			y += ii->fySampling;
			pData = ii->vData.get(ii->nDim*sj*ii->nGridWidth);
			return true;
		    }
		    return false;
		}
	};

	iterator    make_iterator(int width,int height) const {
	    return iterator(this,width,height);
	}

	// DEBUG
	void	countMatches (float* output,float* hist,int n,int m) const;


};

typedef boost::shared_ptr<GridIntegralImage>	GridIntegralImageRef;

}

extern "C" {

JNIEXPORT jobject JNICALL Java_retin_toolbox_document_GridIntegralImage_countMatches
  (JNIEnv *, jobject, jobject, jint, jint);
JNIEXPORT jfloat JNICALL Java_retin_toolbox_document_GridIntegralImage_searchCount
  (JNIEnv *, jobject, jint, jint, jint);
JNIEXPORT jobject JNICALL Java_retin_toolbox_document_GridIntegralImage_searchBox
  (JNIEnv *, jobject, jint, jint, jint);

}


#endif
