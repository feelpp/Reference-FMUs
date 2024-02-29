#include "reducedbasis.h"

#ifdef __cplusplus
#include <feel/feelcore/environment.hpp>
#include <feel/feelcore/checker.hpp>
#include <feel/feeldiscr/pch.hpp>
#include <feel/feeldiscr/traits.hpp>
#include <feel/feeldiscr/check.hpp>
#include <feel/feelfilters/loadmesh.hpp>
#include <feel/feelfilters/exporter.hpp>
#include <feel/feelcore/table.hpp>
#include <feel/feelvf/vf.hpp>
#include <feel/feelvf/print.hpp>
using namespace Feel;

extern "C" {

#endif

MORCLASS MORClass_new()
{
    return new reducedBasis<2>();
}
void feelppClass_delete(MORCLASS hc)
{
    auto pHc = reinterpret_cast<reducedBasis<2>*>(hc);
    delete pHc;
}
void MORClass_init(MORCLASS hc,char* cfgfile_path, char* morjson_path)
{
    auto pHc = reinterpret_cast<reducedBasis<2>*>(hc);
    pHc->init(cfgfile_path, morjson_path);
}
void MORClass_step(MORCLASS hc)
{
    auto pHc = reinterpret_cast<reducedBasis<2>*>(hc);
    pHc->step();
}
double MORClass_computeOutput(MORCLASS hc, double p0, double p1, double p2, double p3, double p4, double p5, double p6)
{
    auto pHc = reinterpret_cast<reducedBasis<2>*>(hc);
    return pHc->computeOutput(p0, p1, p2, p3, p4, p5, p6);
}
double MORClass_computeOutputDeriv(MORCLASS hc)
{
    auto pHc = reinterpret_cast<reducedBasis<2>*>(hc);
    return pHc->computeOutputDeriv();
}

double MORClass_minParameter(MORCLASS hc, int i)
{
    auto pHc = reinterpret_cast<reducedBasis<2>*>(hc);
    return pHc->minParameters(i);
}

#ifdef __cplusplus
    } // extern "C"
#endif