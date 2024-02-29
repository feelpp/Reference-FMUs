#include "heat.h"

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
// using namespace Feel;

extern "C" {

#endif

void heat(int argc, char** argv)
{
    // // Init Feelpp environment inside the class
    // po::options_description heatoptions( "heat options" );
    // heatoptions.add_options()( "no-solve", po::value<bool>()->default_value( false ), "No solve" )
    //     ( "k", po::value<std::string>()->default_value( "1" ), "diffusion coefficient" )
    //     ( "f", po::value<std::string>()->default_value( "" ), "right hand side" )
    //     ( "g", po::value<std::string>()->default_value( "" ), "Dirichlet boundary condition" )
    //     ( "un", po::value<std::string>()->default_value( "" ), "Neumann boundary condition" )
    //     ( "r_1", po::value<std::string>()->default_value( "1" ), "Robin left hand side coefficient" )
    //     ( "r_2", po::value<std::string>()->default_value( "" ), "Robin right hand side  coefficient" );
    // heatoptions.add( case_options( FEELPP_DIM, "P1" ) );
    // heatoptions.add_options()( "marker.name", po::value<std::string>(), "marker on which to solve problem" );
    // heatoptions.add_options()( "marker.levelset", po::value<std::string>(), "marker on which to solve problem" );
    // Environment env( _argc = argc, _argv = argv,
    //                     _desc = heatoptions,
    //                     _about = about( _name = "qs_heat",
    //                                     _author = "Feel++ Consortium",
    //                                     _email = "feelpp-devel@feelpp.org" ) );
    // std::cout << "Ok after env" << std::endl;
    Feel::heatEquation<2> heatClass;

    heatClass.init();
    double meanT, meanDT;
    for(; heatClass.timeStepBdf()->isFinished() == false ; )
    {
        heatClass.step();
        meanT = heatClass.computeOutput();
        meanDT = heatClass.computeOutputDeriv();
        std::cout << fmt::format(" MeanT {}, Mean dT {}",meanT,meanDT) << std::endl;
    }
}



// Expose the heatEquation<Dim> template class to C
HEATCLASS HeatClass_new()
{
    return new Feel::heatEquation<2>();
}
void feelppClass_delete(HEATCLASS hc)
{
    auto pHc = reinterpret_cast<Feel::heatEquation<2>*>(hc);
    delete pHc;
}
void HeatClass_init(HEATCLASS hc)
{
    auto pHc = reinterpret_cast<Feel::heatEquation<2>*>(hc);
    pHc->init();
}
void HeatClass_step(HEATCLASS hc)
{
    auto pHc = reinterpret_cast<Feel::heatEquation<2>*>(hc);
    pHc->step();
}
double HeatClass_computeOutput(HEATCLASS hc)
{
    auto pHc = reinterpret_cast<Feel::heatEquation<2>*>(hc);
    return pHc->computeOutput();
}
double HeatClass_computeOutputDeriv(HEATCLASS hc)
{
    auto pHc = reinterpret_cast<Feel::heatEquation<2>*>(hc);
    return pHc->computeOutputDeriv();
}
#ifdef __cplusplus
    } // extern "C"
#endif