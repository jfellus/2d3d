/*
Copyright © CNRS 2012. 
Authors: Philippe-Henri Gosselin, David Picard, Romain Négrel
Contact: philippe-henri.gosselin@ensea.fr

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
#include "DiagGmmQuantizer.h"

#include "retin/toolbox/core/string_retin.h"
#include "retin/toolbox/algebra/vector.h"
#include "retin/toolbox/algebra/matrix.h"
#include "retin/toolbox/algebra/vector_double.h"

#include <vector>
#include <memory>

using namespace std;
using namespace retin;

#define GMM_EPSILON 1E-12

namespace quantization {
    
DiagGmm::DiagGmm(size_t featureDim, size_t gaussianCount) 
    : featureDim(featureDim),gaussianCount(gaussianCount)
{
    weights = new double[gaussianCount];
    means = new double[gaussianCount*featureDim];
    vars = new double[gaussianCount*featureDim];

    defaultVar = new double[featureDim];
    ivars = new double[gaussianCount*featureDim];
    lncsts = new double[gaussianCount];
}

DiagGmm::~DiagGmm() 
{
    delete[] weights;
    delete[] means;
    delete[] vars;
    delete[] defaultVar;
    delete[] ivars;
    delete[] lncsts;
}

void  DiagGmm::setWeight(size_t c,const double weight) {
    if (!isfinite(weight))
            retinThrowException("Error GMM: NaN in weight !! ");
    weights[c] = weight;
}
void  DiagGmm::setWeights(const double* weights) {
    for (size_t c=0;c<gaussianCount;c++)
        setWeight(c,weights[c]);
}
void  DiagGmm::setWeights(const float* weights) {
    for (size_t c=0;c<gaussianCount;c++)
        setWeight(c,weights[c]);
}
void  DiagGmm::setMean(size_t c,const double* mean) {
    for (size_t i=0;i<featureDim;i++) {
        double x = mean[i];
        if (!isfinite(x))
            retinThrowException("Error GMM: NaN in mean !! ");
        means[i+c*featureDim] = x;
    }
}
void  DiagGmm::setMean(size_t c,const float* mean) {
    for (size_t i=0;i<featureDim;i++) {
        double x = mean[i];
        if (!isfinite(x))
            retinThrowException("Error GMM: NaN in mean !! ");
        means[i+c*featureDim] = x;
    }
}
void  DiagGmm::setMeans(const double* means) {
    for (size_t c=0;c<gaussianCount;c++)
        setMean(c,means+c*featureDim);
}
void  DiagGmm::setMeans(const float* means) {
    for (size_t c=0;c<gaussianCount;c++)
        setMean(c,means+c*featureDim);
}
void  DiagGmm::setVar(size_t c,const double* var) {
    for (size_t i=0;i<featureDim;i++) {
        double x = var[i];
        if (x < GMM_EPSILON) {
            cout << "Warning GMM: very low variance " << endl;
            x = defaultVar[i];
        }
        vars[i+c*featureDim] = x;
        x = 1.0 / x;
        if (!isfinite(x))
            retinThrowException("Error GMM: NaN in vars !! ");
        ivars[i+c*featureDim] = x;
    }
}
void  DiagGmm::setVar(size_t c,const float* var) {
    for (size_t i=0;i<featureDim;i++) {
        double x = var[i];
        if (x < GMM_EPSILON) {
            cout << "Warning GMM: very low variance " << endl;
            x = defaultVar[i];
        }
        vars[i+c*featureDim] = x;
        x = 1.0 / x;
        if (!isfinite(x))
            retinThrowException("Error GMM: NaN in vars !! ");
        ivars[i+c*featureDim] = x;
    }
}
void  DiagGmm::setVars(const double* vars) {
    for (size_t c=0;c<gaussianCount;c++)
        setVar(c,vars+c*featureDim);
}
void  DiagGmm::setVars(const float* vars) {
    for (size_t c=0;c<gaussianCount;c++)
        setVar(c,vars+c*featureDim);
}
void  DiagGmm::setDefaultVar(const double* var) {
    for (size_t i=0;i<featureDim;i++) {
        double x = var[i];
        if (x < GMM_EPSILON)
            cout << "Warning GMM: very low default variance " << endl;
        if (!isfinite(x))
            retinThrowException("Error GMM: NaN in default vars !! ");
        defaultVar[i] = x;
    }
}

void    DiagGmm::updateTempVars(size_t c) {
    if (weights[c] > 0) {
        lncsts[c] = double(featureDim)*log(2.0*M_PI);
        lncsts[c] += log(weights[c]);
        for (size_t i=0;i<featureDim;i++) {
            lncsts[c] += log(vars[i+c*featureDim]);
        }
    }
    else {
        lncsts[c] = 0;
        vector_zero(vars+c*featureDim,featureDim);
        vector_zero(ivars+c*featureDim,featureDim);
    }
}

void    DiagGmm::updateTempVars() {
    for (size_t c=0;c<gaussianCount;c++) 
        updateTempVars(c);
}

size_t    DiagGmm::getNonZeroGaussianCount() const {
    size_t count = 0;
    for (size_t c=0;c<gaussianCount;c++) {
        if (weights[c] > 0)
            count ++;
    }
    return count;
}

void    DiagGmm::getWeights(float* buffer,size_t count) const {
    if (count != gaussianCount)
        retinThrowException2("Invalid count %d != %d",count,gaussianCount);
    vector_convert(buffer,weights,gaussianCount);
}

void    DiagGmm::getMeans(float* buffer,size_t dim,size_t count) const {
    if (dim != featureDim)
        retinThrowException2("Invalid dim %d != %d",dim,featureDim);
    if (count != gaussianCount)
        retinThrowException2("Invalid count %d != %d",count,gaussianCount);
    matrix_convert(buffer,means,featureDim,gaussianCount);
}

void    DiagGmm::getVars(float* buffer,size_t dim,size_t count) const {
    if (dim != featureDim)
        retinThrowException2("Invalid dim %d != %d",dim,featureDim);
    if (count != gaussianCount)
        retinThrowException2("Invalid count %d != %d",count,gaussianCount);
    matrix_convert(buffer,vars,featureDim,gaussianCount);
}

double    DiagGmm::relevances(double* gamma,const float* sample) const
{
    gamma[0] = logRelevance(0,sample);
    double gamma_max = gamma[0];
    for (size_t j=1;j<gaussianCount;j++) {
        double g = logRelevance(j,sample);
        gamma[j] = g;
        if (g > gamma_max) 
            gamma_max = g;
    }
    // log( sum_j exp(gamma[j]) ) 
    // = log( sum_j exp(gamma[j] - gamma_max + gamma_max) )
    // = log( sum_j exp(gamma[j] - gamma_max ) * exp(gamma_max) )
    // = log( sum_j exp(gamma[j] - gamma_max ) + log( exp(gamma_max) )
    // = log( sum_j exp(gamma[j] - gamma_max ) + gamma_max
    double safe_sum = 0;
    for (size_t j=0;j<gaussianCount;j++) {
        // on minimize la valeur à passer dans l'exponentielle
        double x = exp( gamma[j]-gamma_max );
        safe_sum += x; 
        gamma[j] = x;
    }
    vector_sdiv_double(gamma,safe_sum,gaussianCount);
    return gamma_max + log(safe_sum); // = log ( sum_j weight_j*Gaussian_j(sample) )
}

double  DiagGmm::add_stats (double* sweights,double* smeans,double* svars,
    const float* data,size_t dataDim,size_t dataCount) const
{
    if (dataDim != featureDim)
        retinThrowException2("Invalid dim %d != %d",dataDim,featureDim);
    double llh = 0;
    std::vector<double> gamma(gaussianCount);
    for (size_t i=0;i<dataCount;i++) {
        const float* sample = data+i*featureDim;
        llh += relevances(&gamma[0],sample);
        //matrix_print(cout,&gamma[0],1,gaussianCount);
        //cout << "n1=" << vector_n1_double(&gamma[0],gaussianCount) << endl;
        for (size_t j=0;j<gaussianCount;j++) {
            sweights[j] += gamma[j];
            vector_add_stats_double(&smeans[j*featureDim],&svars[j*featureDim],gamma[j],sample,featureDim);
        }
    }    
    return llh;
}

void    DiagGmm::update(size_t c,const double sweight,const double* smean,const double* svar,size_t dataCount)
{
    double weight = sweight / dataCount;
    if (weight > GMM_EPSILON) {
        double* mean = means+c*featureDim;
        double* var = vars+c*featureDim;
        for (size_t k=0;k<featureDim;k++) {
            double m = smean[k] / sweight;
            double x = (svar[k] - smean[k]*m) / sweight;
            mean[k] = m;
            if (x > GMM_EPSILON)
                var[k] = x;
        }
        weights[c] = weight;
    }
    else {
        weights[c] = 0;
    }
    updateTempVars(c);
}

void   DiagGmm::update(const double* sweights,const double* smeans,const double* svars,size_t dataCount)
{
    for (size_t c=0;c<gaussianCount;c++) {
        update(c,sweights[c],smeans+c*featureDim,svars+c*featureDim,dataCount);
    }    
}

////////////////////////////////////////////////////////////////////////////////////

DiagGmmQuantizer::DiagGmmQuantizer(size_t featureDim,size_t gaussianCount) : gmm(featureDim,gaussianCount),workpool(NULL) {
    gaussianCount = 64;
    maxIterations = 100;
    verboseLevel = 1;
}

DiagGmmQuantizer::~DiagGmmQuantizer() {
}



void    DiagGmmQuantizer::init(const float* centers,const float* tensors,size_t centersDim,size_t centersCount) {
    size_t featureDim = gmm.getFeatureDim();
    size_t gaussianCount = gmm.getGaussianCount();
    if (centersCount != gaussianCount)
        retinThrowException2("Invalid count %d != %d",centersCount,gaussianCount);
    if (centersDim != featureDim)
        retinThrowException2("Invalid dim %d != %d",centersDim,featureDim);
    
    vector<double> temp(featureDim);
    for (size_t c=0;c<gaussianCount;c++) {
        const float* tensor = tensors+c*featureDim*featureDim;
        for (size_t k=0;k<featureDim;k++)
            temp[k] += tensor[k+k*featureDim];
    }
    vector_sdiv (&temp[0],gaussianCount,featureDim);
    gmm.setDefaultVar(&temp[0]);
    
    for (size_t c=0;c<gaussianCount;c++) {
        const float* tensor = tensors+c*featureDim*featureDim;
        for (size_t k=0;k<featureDim;k++)
            temp[k] = tensor[k+k*featureDim];
        gmm.setWeight(c,1.0/gaussianCount);
        gmm.setMean(c,centers+c*featureDim);
        gmm.setVar(c,&temp[0]);
        gmm.updateTempVars(c);
    }
}

DiagGmmStatsConqueror::DiagGmmStatsConqueror(DiagGmm& gmm,double& sllh,
    double* sweights,double* smeans,double* svars,const float* data) 
    : gmm(gmm),sllh(sllh),sweights(sweights),smeans(smeans),svars(svars),data(data) 
{
}
void    DiagGmmStatsConqueror::reset() {
    sllh = 0;
    vector_zero(sweights,gmm.getGaussianCount());
    vector_zero(smeans,gmm.getGaussianCount()*gmm.getFeatureDim());
    vector_zero(svars,gmm.getGaussianCount()*gmm.getFeatureDim());
}
void    DiagGmmStatsConqueror::conquer (size_t i0,size_t n)
{
    size_t featureDim = gmm.getFeatureDim();
    size_t gaussianCount = gmm.getGaussianCount();
    vector<double> weights(gaussianCount);
    vector<double> means(featureDim*gaussianCount);
    vector<double> vars(featureDim*gaussianCount);
    double llh = gmm.add_stats(&weights[0],&means[0],&vars[0],data+i0*featureDim,featureDim,n);
    #ifdef RETIN_ENABLE_BOOST
    boost::mutex::scoped_lock lock(the_mutex);
    #endif
    sllh += llh;
    vector_add_double(sweights,&weights[0],gaussianCount);
    vector_add_double(smeans,&means[0],gaussianCount*featureDim);
    vector_add_double(svars,&vars[0],gaussianCount*featureDim);
}

void    DiagGmmQuantizer::run(const float* data,size_t dataDim,size_t dataCount)
{
    size_t featureDim = gmm.getFeatureDim();
    size_t gaussianCount = gmm.getGaussianCount();
    if (dataDim != featureDim)
        retinThrowException2("Invalid dim %d != %d",dataDim,featureDim);

    // Boucle principale
    size_t ite = 0;
    double llh1 = -1E99;
    while(true)
    {
    // Exp
        double llh = 0;
        vector<double> sweights(gaussianCount);
        vector<double> smeans(featureDim*gaussianCount);
        vector<double> svars(featureDim*gaussianCount);
        DiagGmmStatsConqueror conqueror(gmm,llh,&sweights[0],&smeans[0],&svars[0],data);
        work_set workset(workpool);
        workset.divide_and_conquer(conqueror,0,dataCount);
        workset.join();
        
        //llh = gmm.add_stats(&sweights[0],&smeans[0],&svars[0],data,dataDim,dataCount);
        llh /= dataCount;
        
    // Test d'arrêt
        ite ++;
        if (verboseLevel >= 1)
            cout << "ite " << ite << " log-likelihood = " << llh << ", " << gmm.getNonZeroGaussianCount() << " gms" << endl;
        if (ite >= maxIterations || fabs(llh) < 1E-3 || fabs(llh1-llh)/fabs(llh) < 1E-5)
            break;
		if (ite >= 2) {
			bool bOk = true;
			for (size_t c=0;c<gaussianCount;c++) {
				if ( (sweights[c]/dataCount) < GMM_EPSILON ) {
					bOk = false;
					break;
				}
			}
			if (!bOk) {
				break;
			}
		}
        llh1 = llh;
        
    // Max
        gmm.update(&sweights[0],&smeans[0],&svars[0],dataCount);
    }
}

}

