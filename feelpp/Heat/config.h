#ifndef config_h
#define config_h

// define class name and unique id
#define MODEL_IDENTIFIER Heat
#define INSTANTIATION_TOKEN "{1AE5E10D-9521-4DE3-80B9-D0EAAA7D5AF5}"

#define CO_SIMULATION
#define MODEL_EXCHANGE

#define HAS_CONTINUOUS_STATES
#define HAS_EVENT_INDICATORS

#define SET_FLOAT64
#define GET_OUTPUT_DERIVATIVE
#define EVENT_UPDATE

#define FIXED_SOLVER_STEP 1e-1
#define DEFAULT_STOP_TIME 3

#include "heat.h"

typedef enum {
    vr_time, vr_tAvg, vr_der_tAvg
} ValueReference;

typedef struct {

    double tAvg;
    HEATCLASS feelppclassFMU;

} ModelData;

#endif /* config_h */
