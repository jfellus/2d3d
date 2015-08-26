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
 * \file GridIntegralImage.cpp
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#include "retin/toolbox/document/GridIntegralImage.h"

#include "retin/toolbox/document/serialization/Serializers.h"
#include "retin/toolbox/document/basic/Feature.h"
#include "retin/toolbox/document/FeatureMatrix.h"

RETIN_SERIALIZABLE_OBJECT_DEFINITIONS(GridIntegralImage);

using namespace std;
using namespace boost;

namespace retin {

void	GridIntegralImage::closestBox (BoxLabel& box,int x1,int y1,int x2,int y2) const
{
	int i1 = (int) (x1 / fxSampling);
	if (j1 < 0) i1 = 0;
	int j1 = (int) (y1 / fySampling);
	if (j1 < 0) j1 = 0;

	int i2 = (int) ( x2 / fxSampling );
	if (i2 >= nGridWidth) i2 = nGridWidth-1;
	int j2 = (int) ( y2 / fySampling );
	if (j2 >= nGridHeight) j2 = nGridHeight-1;

	box.setX1(i1*fxSampling);
	box.setY1(j1*fySampling);
	box.setX2(i2*fxSampling);
	box.setY2(j2*fySampling);
}

float	GridIntegralImage::closestCount (int x1,int y1,int x2,int y2,int k) const
{
	int i1 = (int) (x1 / fxSampling);
	if (j1 < 0) i1 = 0;
	int j1 = (int) (y1 / fySampling);
	if (j1 < 0) j1 = 0;

	int i2 = (int) ( x2 / fxSampling );
	if (i2 >= nGridWidth) i2 = nGridWidth-1;
	int j2 = (int) ( y2 / fySampling );
	if (j2 >= nGridHeight) j2 = nGridHeight-1;

	return vData[k+nDim*(i1+j1*nGridWidth)] + vData[k+nDim*(i2+j2*nGridWidth)]
	     - vData[k+nDim*(i1+j2*nGridWidth)] - vData[k+nDim*(i2+j1*nGridWidth)];
}

float	GridIntegralImage::searchCount (int width,int height,int k) const
{
	int m = (int) (height / fySampling); // Hauteur en cases de la grille
	int n = (int) (width / fxSampling); // Largeur en cases de la grille

	if (n >= nGridWidth) n = nGridWidth-1;
	if (m >= nGridHeight) m = nGridHeight-1;

	float bestWeight = 0;
	for (int sj=0;sj<nGridHeight-m;sj++) {
	    float* pData = vData.get(k+nDim*sj*nGridWidth);
	    for (int si=0;si<nGridWidth-n;si++) {
		float weight = pData[0] + pData[nDim*(n+m*nGridWidth)]
		    - pData[nDim*n] - pData[nDim*m*nGridWidth];
		if (weight > bestWeight) {
		    bestWeight = weight;
		    //cout << si*fxSampling << "," << sj*fySampling << " = " << weight << endl;
		}
		pData += nDim;
	    }
	}
	return bestWeight;
}

void	GridIntegralImage::searchBox (BoxLabel& box,int width,int height,int k) const
{
	box.setWeight(0);
	iterator ite = make_iterator(width,height);
	while (ite.next()) {
	    float weight = ite.count(k);
	    if (weight > box.getWeight()) {
		box.setWeight(weight);
		box.setX1(ite.x);
		box.setY1(ite.y);
		box.setX2(ite.x+ite.width);
		box.setY2(ite.y+ite.height);
	    }
	}

/*	int m = (int) (height / fySampling);
	int n = (int) (width / fxSampling);

	if (n >= nGridWidth) n = nGridWidth-1;
	if (m >= nGridHeight) m = nGridHeight-1;

	int minI = 0;
	int maxI = nGridWidth-n;
	int minJ = 0;
	int maxJ = nGridHeight-m;


	box.setWeight(0);
	box.setLabel(0);
	box.setX1(0);
	box.setY1(0);

	for (int sj=minJ;sj<maxJ;sj++) {
	    float* pData = vData.get(k+nDim*sj*nGridWidth);
	    for (int si=minI;si<maxI;si++) {
		float weight = pData[0] + pData[nDim*(n+m*nGridWidth)]
		    - pData[nDim*n] - pData[nDim*m*nGridWidth];

		if (weight > box.getWeight()) {
		    box.setWeight(weight);
		    box.setX1(si*fxSampling);
		    box.setY1(sj*fySampling);
		    //cout << si*fxSampling << "," << sj*fySampling << " = " << weight << endl;
		}

		pData += nDim;
	    }
	}
	box.setX2(box.getX1()+n*fxSampling);
	box.setY2(box.getY1()+m*fySampling);*/
}

bool	GridIntegralImage::searchBox (BoxLabel& box,int width,int height,int k,int x1,int y1,int x2,int y2) const
{
	int m = (int) (height / fySampling); // Hauteur en cases de la grille
	int n = (int) (width / fxSampling); // Largeur en cases de la grille

	if (n >= nGridWidth) n = nGridWidth-1;
	if (m >= nGridHeight) m = nGridHeight-1;

	int minI = (int) (x1 / fxSampling);
	if (minI < 0) minI = 0;
	int maxI = (int) (x2 / fxSampling);
	if (maxI > nGridWidth-n) maxI = nGridWidth-n;

	int minJ = (int) (y1 / fySampling);
	if (minJ < 0) minJ = 0;
	int maxJ = (int) (y2 / fySampling);
	if (maxJ > nGridHeight-m) maxJ = nGridHeight-m;


	bool found = false;
	for (int sj=minJ;sj<maxJ;sj++) {
	    float* pData = vData.get(k+nDim*sj*nGridWidth);
	    for (int si=minI;si<maxI;si++) {
		float weight = pData[0] + pData[nDim*(n+m*nGridWidth)]
		    - pData[nDim*n] - pData[nDim*m*nGridWidth];

		if (weight > box.getWeight()) {
		    found = true;
		    box.setWeight(weight);
		    x1 = si*fxSampling;
		    y1 = sj*fySampling;
		}

		pData += nDim;
	    }
	}
	if (found) {
	    box.setX1(x1);
	    box.setY1(y1);
	    box.setX2(x1+n*fxSampling);
	    box.setY2(y1+m*fySampling);
	}
	return found;
}




void	GridIntegralImage::countMatches (float* output,float* hist,int n,int m) const
{
    float minValue = 1E-5;

    uint kMax = 0;
    for (uint k=1;k<(uint)nDim;k++) {
	float r = hist[k];
	if (hist[kMax] < r) kMax = k;
    }

    printf ("kMax=%d fMax=%f\n",kMax,hist[kMax]);

    for (int j=0;j<nImageHeight;j++) {
	if ((j+m) >= nImageHeight)
	    break;

	for (int i=0;i<nImageWidth;i++) {
	    if ((i+n) >= nImageWidth)
		break;

	    float w = 0;
	    for (uint k=0;k<(uint)nDim;k++) {
		float s = getCount(i,j,n,m,kMax);
		float r = hist[kMax];
		float x = r - s;
		float y = r + s;
		float x1 = -x;
		if (x1 > x) x = x1;
		if (y < minValue) y = minValue;
		w += x / y;
		/*if (i==0 && j==0) {
			printf ("%f %f %f %f %f\n",r,s,x,y,w);
		}*/
	    }

	    w = 1-w/nDim;
	    for (int s=0;s<m;s++)
		    for (int r=0;r<n;r++)
			output[(i+r)+(j+s)*nImageWidth] += w;
	}
    }
}

}

using namespace retin;

JNIEXPORT jobject JNICALL Java_retin_toolbox_document_GridIntegralImage_countMatches
  (JNIEnv *env, jobject thisObject, jobject histObject, jint n, jint m)
{
	RETIN_JAVA_METHOD_BEGIN

	JavaEnv je(env);
	GridIntegralImageRef ii = je.createDocument<GridIntegralImage>(thisObject);
	FeatureRef hist = je.createDocument<Feature>(histObject);

	FeatureMatrixRef output = boost::make_shared<FeatureMatrix>(ii->width(),ii->height(),1);
	ii->countMatches (output->data(),hist->data(),n,m);
	return je.createObject (output);

	RETIN_JAVA_METHOD_END
	return NULL;
}

JNIEXPORT jfloat JNICALL Java_retin_toolbox_document_GridIntegralImage_searchCount
  (JNIEnv *env, jobject thisObject, jint width, jint height, jint bin)
{
	RETIN_JAVA_METHOD_BEGIN

	JavaEnv je(env);
	GridIntegralImageRef ii = je.createDocument<GridIntegralImage>(thisObject);
	return ii->searchCount(width,height,bin);

	RETIN_JAVA_METHOD_END
	return 0;
}

JNIEXPORT jobject JNICALL Java_retin_toolbox_document_GridIntegralImage_searchBox
  (JNIEnv *env, jobject thisObject, jint width, jint height, jint bin)
{
	RETIN_JAVA_METHOD_BEGIN

	JavaEnv je(env);
	GridIntegralImageRef ii = je.createDocument<GridIntegralImage>(thisObject);
	BoxLabelRef box = make_shared<BoxLabel>();
	ii->searchBox(*box,width,height,bin);
	return je.createObject(box);

	RETIN_JAVA_METHOD_END
	return NULL;
}
