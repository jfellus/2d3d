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
 * \file Keypoint.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_Keypoint_h__
#define __retin_Keypoint_h__

#include "../Document.h"

#include <vector>

namespace retin
{

class	Keypoint : public Document
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(retin/toolbox/document/basic,Keypoint);

	jint			nDim;
	jfloat			fScore;
	std::vector<jfloat>	pCoord;
	std::vector<jfloat>	pDir;
	std::vector<jfloat>	pSize;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, Keypoint& x)
	{
		s.object ("dim",x.nDim);
		s.object ("score",x.fScore);
		s.vector ("coord",x.pCoord,x.nDim);
		s.vector ("dir",x.pDir,x.nDim);
		s.vector ("size",x.pSize,x.nDim);
	}

public:
	Keypoint (int nDim=2) : nDim(nDim),fScore(0),pCoord(nDim),pDir(nDim),pSize(nDim) { }
        
        Keypoint (float x,float y,float sx,float sy,float dx,float dy,float score) {
		nDim = 2;
		fScore = score;
		pCoord.resize(2);
                pCoord[0] = x;
		pCoord[1] = y;
		pSize.resize(2);
		pSize[0] = sx;
		pSize[1] = sy;
		pDir.resize(2);
		pDir[0] = dx;
		pDir[1] = dy;
	}
        
	Keypoint (const Keypoint& l) : nDim(l.nDim),fScore(l.fScore),pCoord(l.pCoord),pDir(l.pDir),pSize(l.pSize) { }

	jfloat*		coordData() { return &pCoord[0]; }
	jfloat*		dirData() { return &pDir[0]; }
	jfloat*		sizeData() { return &pSize[0]; }

	jint		dim() const { return nDim; }
	jfloat&		score() { return fScore; }
	const jfloat&	score() const { return fScore; }

	jfloat&		coord(int k)
	{
		BOOST_ASSERT (k >= 0 && k < nDim);
		return pCoord[k];
	}
	const jfloat&	coord(int k) const
	{
		BOOST_ASSERT (k >= 0 && k < nDim);
		return pCoord[k];
	}

	jfloat&		dir(int k)
	{
		BOOST_ASSERT (k >= 0 && k < nDim);
		return pDir[k];
	}
	const jfloat&	dir(int k) const
	{
		BOOST_ASSERT (k >= 0 && k < nDim);
		return pDir[k];
	}

	jfloat&		size(int k)
	{
		BOOST_ASSERT (k >= 0 && k < nDim);
		return pSize[k];
	}
	const jfloat&	size(int k) const
	{
		BOOST_ASSERT (k >= 0 && k < nDim);
		return pSize[k];
	}
};

typedef boost::shared_ptr<Keypoint> KeypointRef;

}

#endif
