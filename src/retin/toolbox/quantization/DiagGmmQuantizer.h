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

#ifndef __quantization__DiagGmmQuantizer_h__
#define __quantization__DiagGmmQuantizer_h__

#include "retin/toolbox/core/core.h"
#include "retin/toolbox/core/work_set.h"

#include <vector>

namespace quantization {

class DiagGmm {
protected:
    // model
    size_t      featureDim;
    size_t      gaussianCount;
    double*     weights;
    double*     means;
    double*     vars;
    // default model
    double*     defaultVar;
    // temp vars
    double*     ivars;
    double*     lncsts;
public:
    DiagGmm(size_t featureDim,size_t gaussianCount);
    ~DiagGmm();

    void        setDefaultVar(const double* var);
    
    void        setWeight(size_t c,const double weight);
    void        setWeights(const double* weights);
    void        setWeights(const float* weights);
    void        setMean(size_t c,const double* mean);
    void        setMean(size_t c,const float* mean);
    void        setMeans(const double* means);
    void        setMeans(const float* means);
    void        setVar(size_t c,const double* var);
    void        setVar(size_t c,const float* var);
    void        setVars(const double* vars);
    void        setVars(const float* vars);
    
    void        updateTempVars(size_t c);
    void        updateTempVars();
    
    size_t      getFeatureDim() const { return featureDim; }
    size_t      getGaussianCount() const { return gaussianCount; }
    size_t      getNonZeroGaussianCount() const;
    void        getWeights(float* buffer,size_t count) const;
    void        getMeans(float* buffer,size_t dim,size_t count) const;
    void        getVars(float* buffer,size_t dim,size_t count) const;

    const double* getWeightsBuffer() const { return weights; }
    const double* getMeansBuffer() const { return means; }
    const double* getVarsBuffer() const { return vars; }
    const double* getIVarsBuffer() const { return ivars; }

    double      logRelevance (size_t c,const float* sample) const {
        double r = 0;
        for (size_t i=0;i<featureDim;i++) {
            double x = sample[i] - means[i+c*featureDim];
            r += ivars[i+c*featureDim] * x*x;
        }
        return -0.5 * (lncsts[c] + r);
    }
    double      relevances(double* gamma,const float* sample) const;
    double      add_stats (double* sweights,double* smeans,double* svars,
        const float* data,size_t dataDim,size_t dataCount) const;
    void        update(size_t c,const double sweights,const double* smeans,const double* svars,size_t dataCount);
    void        update(const double* sweights,const double* smeans,const double* svars,size_t dataCount);
    
};  

class DiagGmmStatsConqueror : public retin::work_set::conqueror {
public:
    DiagGmm& gmm;
    double& sllh;
    double* sweights;
    double* smeans;
    double* svars;
    const float* data;
    std::vector<double> main_weights,main_means,main_vars;
    #ifdef RETIN_ENABLE_BOOST
    mutable boost::mutex    the_mutex;
    #endif
public:
    DiagGmmStatsConqueror(DiagGmm& gmm,double& sllh,
        double* sweights,double* smeans,double* svars,const float* data);
    void    reset();
    virtual void    conquer (size_t i0,size_t n);
};
    
class DiagGmmQuantizer
{
protected:
    DiagGmm         gmm;
    size_t          maxWords;
    size_t          maxIterations;
    size_t          verboseLevel;
    retin::work_pool* workpool;
    
public:
    DiagGmmQuantizer(size_t featureDim,size_t gaussianCount);
    virtual ~DiagGmmQuantizer();
    
    void            clear();

    void            setVerboseLevel (size_t x) { verboseLevel = x; }
    void            setMaxIterations (size_t x) { maxIterations = x; }
    void            setWorkPool(retin::work_pool* wp) { workpool = wp; }
    
    size_t          getMaxWords() const { return maxWords; }
    const DiagGmm&  getGmm() const { return gmm; }
    DiagGmm&        getGmm() { return gmm; }
    
    virtual void    init(const float* centers,const float* tensors,size_t centersDim,size_t centersCount);
    virtual void    run(const float* data,size_t dataDim,size_t dataCount);
    
    double          log_likelihood(const float* sample) const;
    double          posterior(double* gamma,const float* sample) const;
};

}

#endif
