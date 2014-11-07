/**
 * \file KeypointList.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_KeypointList_h__
#define __retin_KeypointList_h__

#include "../basic/Keypoint.h"

namespace retin
{

class	KeypointList : public Document
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(retin/toolbox/document/list,KeypointList);

	jint				nCount;
	std::vector<KeypointRef>	pKeypoints;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, KeypointList& x)
	{
		s.object ("count",x.nCount);
		s.object ("keypoints",x.pKeypoints);
	}

public:
	KeypointList (uint nCount=1,uint nDim=2);
	KeypointList (const KeypointList& l) : nCount(l.nCount),pKeypoints(l.pKeypoints) { }

	jint		count() const { return nCount; }

	jfloat&		score(int i)
	{
		BOOST_ASSERT (i >= 0 && i < nCount);
		return pKeypoints[i]->score();
	}
	const jfloat&	score(int i) const
	{
		BOOST_ASSERT (i >= 0 && i < nCount);
		return pKeypoints[i]->score();
	}

	jfloat&		coord(int k,int i)
	{
		BOOST_ASSERT (i >= 0 && i < nCount);
		return pKeypoints[i]->coord(k);
	}
	const jfloat&	coord(int k,int i) const
	{
		BOOST_ASSERT (i >= 0 && i < nCount);
		return pKeypoints[i]->coord(k);
	}

	jfloat&		dir(int k,int i)
	{
		BOOST_ASSERT (i >= 0 && i < nCount);
		return pKeypoints[i]->dir(k);
	}
	const jfloat&	dir(int k,int i) const
	{
		BOOST_ASSERT (i >= 0 && i < nCount);
		return pKeypoints[i]->dir(k);
	}

	jfloat&		size(int k,int i)
	{
		BOOST_ASSERT (i >= 0 && i < nCount);
		return pKeypoints[i]->size(k);
	}
	const jfloat&	size(int k,int i) const
	{
		BOOST_ASSERT (i >= 0 && i < nCount);
		return pKeypoints[i]->size(k);
	}
        
        void add(KeypointRef kp) {
                pKeypoints.push_back(kp);
                nCount = pKeypoints.size();
        }
};

typedef boost::shared_ptr<KeypointList> KeypointListRef;

}

#endif

