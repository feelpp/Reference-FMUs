#ifndef config_h
#define config_h

// define class name and unique id
#define MODEL_IDENTIFIER ReducedBasis
#define INSTANTIATION_TOKEN "{1AE5E10D-9521-4DE3-80B9-D0EAAA7D5AF9}"

#define CO_SIMULATION
#define MODEL_EXCHANGE

#define HAS_CONTINUOUS_STATES
#define HAS_EVENT_INDICATORS

#define SET_FLOAT64
#define GET_OUTPUT_DERIVATIVE
#define EVENT_UPDATE

#define FIXED_SOLVER_STEP 1
#define DEFAULT_STOP_TIME 1

#include "reducedbasis.h"


typedef enum {
    vr_time, vr_h, vr_p0, vr_p1, vr_p2, vr_p3, vr_p4, vr_p5, vr_p6, vr_p7
} ValueReference;

typedef struct {

    double h; // output of the reduced model

    // Parameters
    double p0;
    double p1;
    double p2;
    double p3;
    double p4;
    double p5;
    double p6;

    MORCLASS feelppclassFMU; // class containing the instance of the reduced model

} ModelData;

#endif /* config_h */
