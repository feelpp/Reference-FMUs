#include <math.h>  // for fabs()
#include <float.h> // for DBL_MIN
#include "config.h"
#include "model.h"

#define EVENT_EPSILON (1e-10)

void setStartValues(ModelInstance *comp) {
    M(feelppclassFMU) = HeatClass_new();
    HeatClass_init(M(feelppclassFMU));
    M(tAvg) =  HeatClass_computeOutput(M(feelppclassFMU));
}

Status calculateValues(ModelInstance *comp) {
    // UNUSED(comp);
    printf("Calculate values");
    HeatClass_step(M(feelppclassFMU));
    M(tAvg) = HeatClass_computeOutput(M(feelppclassFMU));// nothing to do
    return OK;
}

Status getFloat64(ModelInstance* comp, ValueReference vr, double values[], size_t nValues, size_t* index) {

    switch (vr) {
        case vr_time:
            ASSERT_NVALUES(1);
            values[(*index)++] = comp->time;
            return OK;
        case vr_tAvg:
            ASSERT_NVALUES(1);
            values[(*index)++] = M(tAvg);
            return OK;
        case vr_der_tAvg:
            ASSERT_NVALUES(1);
            values[(*index)++] = HeatClass_computeOutputDeriv(M(feelppclassFMU));
            return OK;
        default:
            logError(comp, "Get Float64 is not allowed for value reference %u.", vr);
            return Error;
    }
}

Status setFloat64(ModelInstance* comp, ValueReference vr, const double value[], size_t nValues, size_t* index) {

    ASSERT_NVALUES(1);

    switch (vr) {

        case vr_tAvg:
            M(tAvg) = value[(*index)++];
            return OK;
        default:
            logError(comp, "Unexpected value reference: %u.", vr);
            return Error;
    }
}

Status getOutputDerivative(ModelInstance *comp, ValueReference valueReference, int order, double *value) {
    if (order != 1) {
        logError(comp, "The output derivative order %d for value reference %u is not available.", order, valueReference);
        return Error;
    }

    switch (valueReference) {
    case vr_der_tAvg:
        *value = HeatClass_computeOutputDeriv(M(feelppclassFMU));
        return OK;
    default:
        logError(comp, "The output derivative for value reference %u is not available.", valueReference);
        return Error;
    }
}

Status eventUpdate(ModelInstance *comp) {
    UNUSED(comp);
    // if (M(h) <= 0 && M(v) < 0) {

    //     M(h) = DBL_MIN;  // slightly above 0 to avoid zero-crossing
    //     M(v) = -M(v) * M(e);

    //     if (M(v) < V_MIN) {
    //         // stop bouncing
    //         M(v) = 0;
    //         M(g) = 0;
    //     }

    //     comp->valuesOfContinuousStatesChanged = true;
    // } else {
    //     comp->valuesOfContinuousStatesChanged = false;
    // }

    comp->nominalsOfContinuousStatesChanged = false;
    comp->terminateSimulation  = false;
    comp->nextEventTimeDefined = false;

    return OK;
}

size_t getNumberOfEventIndicators(ModelInstance* comp) {

    UNUSED(comp);

    return 1;
}

size_t getNumberOfContinuousStates(ModelInstance* comp) {

    UNUSED(comp);

    return 1;
}

Status getContinuousStates(ModelInstance *comp, double x[], size_t nx) {

    UNUSED(nx);

    x[0] = M(tAvg);

    return OK;
}

Status setContinuousStates(ModelInstance *comp, const double x[], size_t nx) {

    UNUSED(nx);
    M(tAvg) = HeatClass_computeOutput(M(feelppclassFMU));

    return OK;
}

Status getDerivatives(ModelInstance *comp, double dx[], size_t nx) {

    UNUSED(nx);
    HeatClass_step(M(feelppclassFMU));
    dx[0] = HeatClass_computeOutputDeriv(M(feelppclassFMU));

    return OK;
}

Status getEventIndicators(ModelInstance *comp, double z[], size_t nz) {

    UNUSED(nz);
    UNUSED(comp);
    UNUSED(z);

    // if (M(h) > -EVENT_EPSILON && M(h) <= 0 && M(v) > 0) {
    //     // hysteresis for better stability
    //     z[0] = -EVENT_EPSILON;
    // } else {
    //     z[0] = M(h);
    // }

    return OK;
}
