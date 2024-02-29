#ifndef C_REDUCEDBASIS_H
#define C_REDUCEDBASIS_H

#ifdef __cplusplus
#include <feel/feelmor/mormodels.hpp>
#include <feel/feelcore/environment.hpp>
#include <feel/feelfilters/loadcsv.hpp>
#include <feel/feelmor/options.hpp>
namespace Feel{

    auto
    setupSampling( MORModels const& m )
    {
        auto muspace = m.parameterSpace();

        // std::ostringstream ostrmumin, ostrmumax;
        // auto mumin = muspace->min();
        // auto mumax = muspace->max();
        // for ( uint16_type d = 0; d < muspace->dimension(); ++d )
        // {
        //     ostrmumin << mumin( d ) << " ";
        //     ostrmumax << mumax( d ) << " ";
        // }
        // std::cout << "dimension of parameter space : " << muspace->dimension() << "\n";
        // std::cout << "min element in parameter space : " << ostrmumin.str() << "\n";
        // std::cout << "max element in parameter space : " << ostrmumax.str() << "\n";

        auto mysampling = muspace->sampling();

        std::vector<double> inputParameter;
        if ( Environment::vm().count( "parameter" ) )
        {
            auto inputParameterParsed = Environment::vm()["parameter"].as<std::vector<std::string>>();

            if ( inputParameterParsed.size() == 1 )
            {
                std::vector<std::string> stringParsedSplitted;
                boost::split( stringParsedSplitted, inputParameterParsed.front(), boost::is_any_of( " " ), boost::token_compress_on );
                inputParameterParsed = stringParsedSplitted;
            }

            for ( std::string const& paramParsed : inputParameterParsed )
                inputParameter.push_back( std::stod( paramParsed ) );
        }
        else if ( Environment::vm().count( "parameter.filename" ) )
        {
            std::string fname = Environment::vm()["parameter.filename"].as<std::string>();
            auto r = loadXYFromCSV( fname, muspace->parameterNames() );
            auto mu = muspace->element();
            for ( auto const& p : r )
            {
                mu = p;
                mysampling->push_back( mu );
            }
        }
        // inputParameter = Environment::vm()["parameter"].as<std::vector<double> >();
        if ( !inputParameter.empty() )
        {
            CHECK( inputParameter.size() == muspace->dimension() ) << "parameter has a wrong size : " << inputParameter.size() << " but must be " << muspace->dimension() << ":" << inputParameter;
            auto mu = muspace->element();
            for ( uint16_type d = 0; d < muspace->dimension(); ++d )
                mu( d ) = inputParameter[d];
            mysampling->push_back( mu );
        }
        else if ( mysampling->empty() )
        {
            int nSample = ioption( _name = "sampling.size" );
            std::string sampler = soption( "sampling.type" );
            mysampling->sample( nSample, sampler );
        }
        return mysampling;
    }

    auto
    parameterDefinition( MORModels const& m, double p0, double p1, double p2, double p3, double p4, double p5, double p6 )
    {
        auto muspace = m.parameterSpace();
        auto mysampling = muspace->sampling();
        auto mu = muspace->element();
        mu( 0 ) = p0;
        mu( 1 ) = p1;
        mu( 2 ) = p2;
        mu( 3 ) = p3;
        mu( 4 ) = p4;
        mu( 5 ) = p5;
        mu( 6 ) = p6;
        mysampling->push_back( mu );
        return mysampling;

    }

    template< int Dim>
    class reducedBasis
    {

    public:

        reducedBasis()
        {
            // Init Feelpp environment inside the class
            po::options_description rbOptions( "rb options" );
            rbOptions.add( case_options( FEELPP_DIM, "P1" ) );
            rbOptions.add_options()
            ( "plugin.dir", po::value<std::string>()->default_value( Info::libdir().string() ) , "plugin directory" )
            ( "morjson", po::value<std::string>(), "filename describing the mor models associated to the case study" )
            ( "parameter", po::value<std::vector<std::string> >()->multitoken(), "database filename" )
            ( "parameter.filename", po::value<std::string>(), "parameters from csv file" )
            ( "sampling.size", po::value<int>()->default_value( 10 ), "size of sampling" )
            ( "sampling.type", po::value<std::string>()->default_value( "random" ), "type of sampling" )
            ( "rb-dim", po::value<int>()->default_value( -1 ), "reduced basis dimension used (-1 use the max dim)" )
            ( "output_results.save.path", po::value<std::string>(), "output_results.save.path" )
            ( "output_results.precision", po::value<int>()->default_value( 6 ), "float precision for output results")
            ( "output_results.print", po::value<bool>()->default_value( true ), "print results in shell")

            ( "query", po::value<std::string>(), "query string for mongodb DB feelpp.crbdb" )
            ( "compare", po::value<std::string>(), "compare results from query in mongodb DB feelpp.crbdb" )
            ( "list", "list registered DB in mongoDB  in feelpp.crbdb" )
            ( "export-solution", po::value<bool>()->default_value(false), "export the solutions for visualization")
            ;
        po::options_description crbonlinerunliboptions( "crb online run lib options" );
        crbonlinerunliboptions.add(crbOptions())
            .add(crbSEROptions())
            .add(eimOptions())
            .add(podOptions())
            .add(backend_options("backend-primal"))
            .add(backend_options("backend-dual"))
            .add(backend_options("backend-l2"))
            ;
#if 0
            int argc=1;
            char ** argv = new char*[2];
            argv[0] = strdup("rb_fmu");
            argv[1] = NULL;
#elif 1
    int argc = 5;
    char ** argv = new char*[6];
    argv[0] = strdup("rb_fmu");
    argv[1] = strdup("--v");//--ksp-monitor
    argv[2] = strdup("4");
    argv[3] = strdup("--logtostderr");
    argv[4] = strdup("1");
    argv[5] = NULL;
#endif
            if(!Environment::initialized())
                M_env = new Environment( _argc = argc, _argv = argv,
                                _desc = rbOptions,
                                _desc_lib = crbonlinerunliboptions.add( feel_options() ),
                                _about = about( _name = "rb_fmu",
                                                _author = "Feel++ Consortium",
                                                _email = "feelpp-devel@feelpp.org" ) );

            Feel::Environment::setOptionValue("journal", false );
        }

        ~reducedBasis()
        {
            delete(M_env);
        }

        void init(char* configfile_path, char* morjson_path);
        void step();
        double minParameters(int i);
        double computeOutput(double p0, double p1, double p2, double p3, double p4, double p5, double p6);
        double computeOutputDeriv();
        void exportResults();
        Environment * M_env;
    private:
        MORModels M_morModel;
    };


    template< int Dim>
    void reducedBasis<Dim>::init(char * cfgfile_path, char* morjson_path)
    {
        std::ifstream is( morjson_path );
        nl::json js = nl::json::parse(is);
        LOG(INFO) << "js = " << js.dump(4) << std::endl;

        M_env->setConfigFile(cfgfile_path);

        MORModels morModel{ js };
        M_morModel = std::move(morModel);

        M_morModel.load();
        // MORTable table( M_morModel );
        // M_morModel.addObserver( std::make_shared<MORTable>( table ) );
    }

    template< int Dim>
    void reducedBasis<Dim>::step()
    {
    }

    template< int Dim>
    double reducedBasis<Dim>::computeOutput(double p0, double p1, double p2, double p3, double p4, double p5, double p6)
    {
        auto results = M_morModel.run(parameterDefinition(M_morModel, p0, p1, p2, p3, p4, p5, p6), nl::json{ { "N", ioption(_name="rb-dim") }, { "print_rb_matrix", true }, { "tolerance", 1e-2 } } );

        auto crb_result_first_plugin = results[0][0];
        auto output_first_plugin = crb_result_first_plugin.template get<0>();
        return output_first_plugin[0];
    }

    template< int Dim>
    double reducedBasis<Dim>::computeOutputDeriv()
    {
        return 1.0;
    }

    template< int Dim>
    void reducedBasis<Dim>::exportResults()
    {
    }

    template< int Dim>
    double reducedBasis<Dim>::minParameters(int i)
    {
        return M_morModel.parameterSpace()->min()(i);
    }
}
extern "C" {
#endif

typedef void* MORCLASS;
MORCLASS MORClass_new();
void feelppClass_delete(MORCLASS mc);
void MORClass_init(MORCLASS mc, char * cfgfile_path, char * morjson_path);
void MORClass_step(MORCLASS mc);
double MORClass_computeOutput(MORCLASS mc, double p0, double p1, double p2, double p3, double p4, double p5, double p6);
double MORClass_computeOutputDeriv(MORCLASS mc);
double MORClass_minParameter(MORCLASS mc, int i);


#ifdef __cplusplus
}
#endif


#endif