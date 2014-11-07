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
#include "AbstractCluster.h"

#include "retin/toolbox/algebra/vector.h"
#include "retin/toolbox/algebra/vector_double.h"
#include "retin/toolbox/algebra/matrix_double.h"

using namespace std;

namespace quantization {

AbstractCluster::AbstractCluster(const float* mainData,size_t featureDim) :
        mainData(mainData),featureDim(featureDim),
        center(featureDim),distortion(0),
        split(false),fusion(false) {
}

AbstractCluster::~AbstractCluster() {
}

void    AbstractCluster::setSamples(size_t dataCount)
{
    samples.resize(dataCount);
    for (size_t i=0;i<dataCount;i++) {
        samples[i] = i;
    }
}

void    AbstractCluster::setCenter(const float* buf,size_t dim)
{
    if (dim != featureDim)
        retinThrowException2("Invalid dim %d != %d",dim,featureDim);
    vector_cpy(&center[0],buf,dim);
}

void    AbstractCluster::computeCenter()
{
    if (samples.size() > 0) {
        vector<double> temp(featureDim);
        for (size_t i=0;i<samples.size();i++) {
            const float* x = getSample(samples[i]);
            for (size_t k=0;k<featureDim;k++)
                temp[k] += x[k];
        }
        for (size_t k=0;k<featureDim;k++)
            center[k] = temp[k] / ((double)samples.size());
    }
    else {
        for (size_t k=0;k<featureDim;k++)
            center[k] = 0;
    }
}

void    AbstractCluster::computeDistortion()
{
    distortion = 0;
    for (size_t i=0;i<samples.size();i++) {
        distortion += vector_l2p2_float(&center[0],getSample(samples[i]),featureDim);
    }
}

bool    AbstractCluster::mergeClusters(const AbstractCluster& cluster1,const AbstractCluster& cluster2)
{
    size_t nTailleF = cluster1.samples.size() + cluster2.samples.size();
    // hein ?
    if (nTailleF == 0) {
        return false;
    }
    // centre de fusion
    double weight = 0;
    vector<double> temp(featureDim);
    for (size_t t = 0; t < cluster1.samples.size(); t++) {
        size_t is = cluster1.samples[t];
        const float* data = getSample(is);
        for (size_t k = 0; k < featureDim; k++)
            temp[k] += data[k];
        weight ++;
    }
    for (size_t t = 0; t < cluster2.samples.size(); t++) {
        size_t is = cluster2.samples[t];
        const float* data = getSample(is);
        for (size_t k = 0; k < featureDim; k++)
            temp[k] += data[k];
        weight ++;
    }
    for (size_t k = 0; k < featureDim; k++)
        center[k] = temp[k] / weight;

    // distortion de la classe fusionnée.
    distortion = 0;
    samples.clear();
    for (size_t t = 0; t < cluster1.samples.size(); t++) {
        size_t is = cluster1.samples[t];
        samples.push_back(is);
        distortion += distTo(is);
    }
    for (size_t t = 0; t < cluster2.samples.size(); t++) {
        size_t is = cluster2.samples[t];
        samples.push_back(is);
        distortion += distTo(is);
    }
    fusion = true;
    return true;
}

bool    AbstractCluster::splitCluster(AbstractCluster& cluster1,AbstractCluster& cluster2) const
{
    double weight1 = 0, weight2 = 0;
    vector<double> center1(featureDim),center2(featureDim);
    cluster1.split = true;
    cluster2.split = true;
    // Initialisation aléatoire
    for (size_t t=0;t<samples.size();t++)
    {
            const float* data = getSample(samples[t]);
            if ( (t%2) == 0)
            {
                    for (size_t k=0;k<featureDim;k++)
                            center1[k] += data[k];
                    weight1 ++;
            }
            else {
                    for (size_t k=0;k<featureDim;k++)
                            center2[k] += data[k];
                    weight2 ++;
            }
    }



// Quelques itération de LBG classique
    size_t ite=0;
    while (true)
    {
        if (weight1 == 0 || weight2 == 0)
            return false;

        for (size_t k = 0; k < featureDim; k++) {
            cluster1.center[k] = center1[k] / weight1;
            cluster2.center[k] = center2[k] / weight2;
        }

        // Calcul des classes
        weight1 = 0;
        weight2 = 0;
        cluster1.distortion = 0;
        cluster2.distortion = 0;
        cluster1.samples.clear();
        cluster2.samples.clear();
        for (size_t k=0;k<featureDim;k++) {
            center1[k] = 0;
            center2[k] = 0;
        }
        for (size_t t=0;t<samples.size();t++)
        {
            size_t is = samples[t];
            const float* data = getSample(is);
            float d1 = cluster1.distTo(data);
            float d2 = cluster2.distTo(data);
            if (d1 < d2)
            {
                cluster1.distortion += d1;
                cluster1.samples.push_back(is);
                for (size_t k=0;k<featureDim;k++)
                        center1[k] += data[k];
                weight1 ++;
            }
            else
            {
                cluster2.distortion += d2;
                cluster2.samples.push_back(is);
                for (size_t k=0;k<featureDim;k++)
                        center2[k] += data[k];
                weight2 ++;
            }
        }

        //cout << ite << ": " << cluster1.distortion << "," << cluster2.distortion << " <= " << distortion << endl;

        if (ite >= 3
        || featureDim >= 16
        || 1.1*(cluster1.distortion+cluster2.distortion) < distortion) {
            break;
        }
        ite ++;

    }
    if (cluster1.samples.size() == 0 || cluster2.samples.size() == 0)
        return false;
    if (cluster1.distortion+cluster2.distortion >= distortion)
        return false;
    return true;
}

void    AbstractCluster::updateLabels (std::vector<int>& labels,size_t cluster) const
{
    for (size_t i=0;i<samples.size();i++) {
        labels[samples[i]] = cluster;
    }
}

void    AbstractCluster::computeTensor(float* buf,size_t bufSize,int order) const
{
    if (samples.size() == 0) {
        memset(buf,0,bufSize*sizeof(float));
        return;
    }

    vector<double> mean(featureDim);
    for (size_t i=0;i<samples.size();i++) {
        const float* data = getClusterSample(i);
        for (size_t k=0;k<featureDim;k++)
            mean[k] += data[k];
    }
    vector_sdiv_double(&mean[0],samples.size(),featureDim);

    if (order == 1) {
        if (bufSize != featureDim)
            retinThrowException2("Invalid buf size %d != %d",bufSize,featureDim);
        for (size_t k=0;k<featureDim;k++)
            buf[k] = (float)(mean[k]);
    }
    else if (order == 2) {
        if (bufSize != featureDim*featureDim)
            retinThrowException2("Invalid buf size %d != %d",bufSize,featureDim*featureDim);
        vector<double> temp(featureDim),covs(featureDim*featureDim);
        for (size_t i=0;i<samples.size();i++) {
            const float* data = getClusterSample(i);
            for (size_t k=0;k<featureDim;k++)
                temp[k] = data[k] - mean[k];
            matrix_Cpaat_double(&covs[0],&temp[0],featureDim);
        }
        for (size_t k=0;k<featureDim*featureDim;k++)
            buf[k] = (float)(covs[k]/samples.size());
    }
    else
        retinThrowException1("Unsupported order %d",order);
}

}
