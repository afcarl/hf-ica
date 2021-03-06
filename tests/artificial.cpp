/* ===========================
 *
 * Copyright (c) 2013 Philippe Tillet - National Chiao Tung University
 *
 * NEO-ICA - Dynamically Sampled Hessian Free Independent Comopnent Analaysis
 *
 * License : MIT X11 - See the LICENSE file in the root folder
 * ===========================*/

#include <cmath>
#include <cstdlib>
#include <algorithm>
#include "benchmark-utils.hpp"

#include "neo_ica/ica.h"
#include "neo_ica/backend/backend.hpp"

#define BENCHMARK_COUNT 1

typedef float ScalarType;
static const unsigned int NC=4;
static const unsigned int NF=10000;
static const unsigned int T=20;

int main(){
    ScalarType * src = new ScalarType[NC*NF];
    ScalarType * mixed_src = new ScalarType[NC*NF];
    ScalarType * white_src = new ScalarType[NC*NF];
    ScalarType * independent_components = new ScalarType[NC*NF];

    ScalarType * mixing = new ScalarType[NC*NC];
    ScalarType * sphere = new ScalarType[NC*NC];
    ScalarType * weights = new ScalarType[NC*NC];

    for(unsigned int f=0 ; f< NF ; ++f){
        double t = (double)f/(NF-1)*T - T/2;
        src[0*NF + f] = std::sin(3*t) + std::cos(6*t);
        src[1*NF + f] = std::max(.9, std::cos(10*t));
        src[2*NF + f] = std::sin(5*t);
        src[3*NF + f]  = rand()/(double)RAND_MAX;
    }

    std::srand(0);
    for(size_t i = 0 ; i < NC ; ++i)
        for(size_t j = 0 ; j < NC ; ++j)
            mixing[i*NC+j] = static_cast<double>(std::rand())/RAND_MAX;

    neo_ica::backend<ScalarType>::gemm('N','N',NF,NC,NC,1,src,NF,mixing,NC,0,mixed_src,NF);

    neo_ica::options options;
    options.verbose = 1;
    options.extended = false;
    Timer t;
    t.start();
    for(unsigned int i = 0 ; i < BENCHMARK_COUNT ; ++i){
        neo_ica::ica(mixed_src,weights,sphere,NC,NF,options);
        neo_ica::backend<ScalarType>::gemm('N','N',NF,NC,NC,1,mixed_src,NF,sphere,NC,0,white_src,NF);
        neo_ica::backend<ScalarType>::gemm('N','N',NF,NC,NC,1,white_src,NF,weights,NC,0,independent_components,NF);
    }

    std::cout << "Execution Time : " << t.get()/BENCHMARK_COUNT << "s" << std::endl;

    delete[] src;
    delete[] mixing;
    delete[] mixed_src;
    delete[] independent_components;
}
