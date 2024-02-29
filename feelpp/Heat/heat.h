#ifndef C_HEAT_H
#define C_HEAT_H

#ifdef __cplusplus
#include<feel/feelmesh/simplex.hpp>
#include<feel/feeldiscr/pch.hpp>
#include<feel/feelts/bdf.hpp>
#include<feel/feelpoly/lagrange.hpp>
#include<feel/feeldiscr/functionspace.hpp>
#include<feel/feeldiscr/mesh.hpp>
#include<feel/feelfilters/exporter.hpp>
#include <feel/feelcore/environment.hpp>
#include <feel/feelfilters/loadmesh.hpp>
namespace Feel
{

    template< int Dim>
    class heatEquation
    {
    public:
        typedef Simplex<Dim> convex_type;
        typedef Mesh<convex_type> mesh_type;
        typedef std::shared_ptr<mesh_type> mesh_ptrtype;
        typedef bases<Lagrange<2,Scalar>> basis_type;
        typedef FunctionSpace<mesh_type, basis_type> space_type;
        typedef std::shared_ptr<space_type> space_ptrtype;
        typedef typename space_type::element_type element_type;
        typedef Exporter<mesh_type> exporter_type;
        typedef std::shared_ptr<exporter_type> exporter_ptrtype;
        typedef Bdf<space_type>  bdf_type;
        typedef std::shared_ptr<bdf_type> bdf_ptrtype;
        typedef Backend<double> backend_type;
        typedef std::shared_ptr<backend_type> backend_ptrtype;
        typedef typename backend_type::sparse_matrix_type sparse_matrix_type;
        typedef typename backend_type::sparse_matrix_ptrtype sparse_matrix_ptrtype;
        typedef typename backend_type::vector_type vector_type;
        typedef typename backend_type::vector_ptrtype vector_ptrtype;



    public:

        // heatEquation(int argc, char** argv)
        heatEquation()
        {
            std::cout << "Ok before before options" << std::endl;
            // Init Feelpp environment inside the class
            po::options_description heatoptions( "heat options" );
            std::cout << "Ok before options" << std::endl;
            heatoptions.add_options()
                ( "bdf.time-step", po::value<double>()->default_value( 0.1 ), "time step" )
                ( "bdf.time-initial", po::value<double>()->default_value( 0 ), "diffusion coefficient" )
                ( "bdf.time-final", po::value<double>()->default_value( 3. ), "diffusion coefficient" );
            heatoptions.add( case_options( FEELPP_DIM, "P1" ) );
            std::cout << "Ok before env" << std::endl;
            // Environment M_env( _argc = argc, _argv = argv,
            //                     _desc = heatoptions,
            //                     _about = about( _name = "qs_heat",
            //                                     _author = "Feel++ Consortium",
            //                                     _email = "feelpp-devel@feelpp.org" ) );
#if 1
            int argc=1;
            char ** argv = new char*[2];
            argv[0] = strdup("heat_fmu");
            argv[1] = NULL;
#elif
    int argc = 5;
    char ** argv = new char*[6];
    argv[0] = strdup("rb_fmu");
    argv[1] = strdup("--v");//--ksp-monitor
    argv[2] = strdup("3");
    argv[3] = strdup("--logtostderr");
    argv[4] = strdup("1");
    argv[5] = NULL;
#endif
            if(!Environment::initialized())
                M_env = new Environment( _argc = argc, _argv = argv,
                                _desc = heatoptions,
                                _about = about( _name = "qs_heat",
                                                _author = "Feel++ Consortium",
                                                _email = "feelpp-devel@feelpp.org" ) );
            // Environment M_env( _argc = argc, _argv = argv,
            //                     _desc = heatoptions,
            //                     _about = about( _name = "qs_heat",
            //                                     _author = "Feel++ Consortium",
            //                                     _email = "feelpp-devel@feelpp.org" ) );

            Feel::Environment::setOptionValue("journal", false );
        }

        ~heatEquation()
        {
            delete(M_env);
        }

        void init();
        void step();
        double computeOutput();
        double computeOutputDeriv();
        void exportResults();
        bdf_ptrtype timeStepBdf(){return M_bdf;}
        Environment * env(){return M_env;}
        Environment * M_env;
    private:
        bdf_ptrtype M_bdf;
        exporter_ptrtype M_exporter;
        mesh_ptrtype M_mesh;
        space_ptrtype M_Vh;
        sparse_matrix_ptrtype M_A;
        vector_ptrtype M_F;
        backend_ptrtype M_backend;
    };


    template< int Dim>
    void heatEquation<Dim>::init()
    {
        M_mesh = loadMesh(_mesh = new mesh_type);
        M_Vh = space_type::New(_mesh = M_mesh);
        M_exporter = exporter( _mesh = M_mesh, _name = "cheat");
        M_bdf = bdf( _space=M_Vh);
        M_backend = backend_type::build();

        auto a = form2(_trial=M_Vh, _test=M_Vh, _matrix = M_A);
        // auto l = form1(_test=M_Vh, _vector = M_F);
        // l.zero();

        auto u = M_Vh->element();
        auto v = M_Vh->element();

        a = integrate(_range = elements(M_mesh), _expr = inner(M_bdf->polyDerivCoefficient(0)*idt(u), id(v)));
        a += integrate(_range = elements(M_mesh), _expr = inner(gradt(u), grad(v)));

        u.on(_range = elements(M_mesh), _expr = cst(100)*sin(pi*Px())*sin(pi*Px())*sin(pi*Py())*sin(pi*Py()));
        M_bdf->start(u);
    }
    template< int Dim>
    void heatEquation<Dim>::step()
    {
        if(M_bdf->isFinished() == false)
        {
            auto a0 = form2(_trial = M_Vh, _test = M_Vh, _matrix = M_A);
            auto a = form2(_trial = M_Vh, _test = M_Vh);
            auto f = form1(_test = M_Vh);
            auto v = M_Vh->element();
            auto u = M_Vh->element();
            // a=a0;
            a = integrate(_range = elements(M_mesh), _expr = inner(M_bdf->polyDerivCoefficient(0)*idt(u), id(v)));
            a += integrate(_range = elements(M_mesh), _expr = inner(gradt(u), grad(v)));


            f = integrate(_range = elements(M_mesh), _expr = inner(idv(M_bdf->polyDeriv()),id(v)));


            a+=on(_range=boundaryfaces(M_mesh), _element = u, _rhs = f, _expr = cst(100));

            a.solve(_rhs = f, _solution = u );

            M_bdf->next(u);
        }
    }
    template< int Dim>
    double heatEquation<Dim>::computeOutput()
    {
        auto u = M_bdf->unknown(0);
        double u_average = mean(_range = elements(M_mesh), _expr = idv(u) )(0,0);
        return u_average;
    }
    template< int Dim>
    double heatEquation<Dim>::computeOutputDeriv()
    {
        auto u = M_bdf->unknown(0);
        double du_average = mean(_range =elements(M_mesh), _expr = laplacianv(u))(0,0);
        return du_average;
    }
    template< int Dim>
    void heatEquation<Dim>::exportResults()
    {}
}

extern "C" {
#endif

//
typedef void* HEATCLASS;
HEATCLASS HeatClass_new();
void feelppClass_delete(HEATCLASS hc);
void HeatClass_init(HEATCLASS hc);
void HeatClass_step(HEATCLASS hc);
double HeatClass_computeOutput(HEATCLASS hc);
double HeatClass_computeOutputDeriv(HEATCLASS hc);



void heat(int, char**);

#ifdef __cplusplus
}
#endif


#endif