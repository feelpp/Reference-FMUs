#include <math.h>  // for fabs()
#include <float.h> // for DBL_MIN
#include "config.h"
#include "model.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define V_MIN (0.1)
#define EVENT_EPSILON (1e-10)

#define MAX_PATH_LENGTH 4096

void setStartValues(ModelInstance *comp) {


// Load the mormodels.json and .cfg files

    FILE *file = NULL;
    char path[MAX_PATH_LENGTH] = "";
    char c = '\0';

    char path_cfg[MAX_PATH_LENGTH] = "";

    if (!comp->resourceLocation) {
        logError(comp, "Resource location must not be NULL.");
    }

#ifdef _WIN32

#if FMI_VERSION < 3
    DWORD pathLen = MAX_PATH_LENGTH;

    if (PathCreateFromUrlA(comp->resourceLocation, path, &pathLen, 0) != S_OK) {
        logError(comp, "Failed to convert resource location to file system path.");
        return Error;
    }
#else
    strncpy(path, comp->resourceLocation, MAX_PATH_LENGTH);
#endif

#if FMI_VERSION == 1
    if (!PathAppendA(path, "resources") || !PathAppendA(path, "y.txt")) return Error;
#elif FMI_VERSION == 2
    if (!PathAppendA(path, "y.txt")) return Error;
#else
    if (!strncat(path, "y.txt", MAX_PATH_LENGTH)) return Error;
#endif

#else

#if FMI_VERSION < 3
    const char *scheme1 = "file:///";
    const char *scheme2 = "file:/";

    if (strncmp(comp->resourceLocation, scheme1, strlen(scheme1)) == 0) {
        strncpy(path, &comp->resourceLocation[strlen(scheme1)] - 1, MAX_PATH_LENGTH-1);
    } else if (strncmp(comp->resourceLocation, scheme2, strlen(scheme2)) == 0) {
        strncpy(path, &comp->resourceLocation[strlen(scheme2) - 1], MAX_PATH_LENGTH-1);
    } else {
        logError(comp, "The resourceLocation must start with \"file:/\" or \"file:///\"");
    }

    // decode percent encoded characters
    char* src = path;
    char* dst = path;

    char buf[3] = { '\0', '\0', '\0' };

    while (*src) {

        if (*src == '%' && (buf[0] = src[1]) && (buf[1] = src[2])) {
            *dst = strtol(buf, NULL, 16);
            src += 3;
        } else {
            *dst = *src;
            src++;
        }

        dst++;
    }

    *dst = '\0';
#else
    strncpy(path, comp->resourceLocation, MAX_PATH_LENGTH);
#endif

strncpy(path_cfg, path, MAX_PATH_LENGTH);

#if FMI_VERSION == 1
    strncat(path, "/thermalbuilding/mormodels.json", MAX_PATH_LENGTH-strlen(path)-1);
    strncat(path_cfg, "/thermalbuilding/thermalbuilding.cfg", MAX_PATH_LENGTH-strlen(path)-1);
#elif FMI_VERSION == 2
    strncat(path, "/thermalbuilding/mormodels.json", MAX_PATH_LENGTH-strlen(path)-1);
    strncat(path_cfg, "/thermalbuilding/thermalbuilding.cfg", MAX_PATH_LENGTH-strlen(path)-1);
#else
    strncat(path, "/thermalbuilding/mormodels.json", MAX_PATH_LENGTH-strlen(path)-1);
    strncat(path_cfg, "/thermalbuilding/thermalbuilding.cfg", MAX_PATH_LENGTH-strlen(path)-1);
#endif
    path[MAX_PATH_LENGTH-1] = 0;
    path_cfg[MAX_PATH_LENGTH-1] = 0;

#endif


    M(feelppclassFMU) = MORClass_new();

    MORClass_init(M(feelppclassFMU),path_cfg,path);

    // Initialize variable

    // Output
    M(h) =  0;

    // Parameters

    M(p0) = MORClass_minParameter(M(feelppclassFMU),0);
    M(p1) = MORClass_minParameter(M(feelppclassFMU),1);
    M(p2) = MORClass_minParameter(M(feelppclassFMU),2);
    M(p3) = MORClass_minParameter(M(feelppclassFMU),3);
    M(p4) = MORClass_minParameter(M(feelppclassFMU),4);
    M(p5) = MORClass_minParameter(M(feelppclassFMU),5);
    M(p6) = MORClass_minParameter(M(feelppclassFMU),6);

}

Status calculateValues(ModelInstance *comp) {
    UNUSED(comp);

    M(h) = MORClass_computeOutput(M(feelppclassFMU),M(p0),M(p1),M(p2),M(p3),M(p4),M(p5),M(p6));
    // nothing to do
    return OK;
}

Status getFloat64(ModelInstance* comp, ValueReference vr, double values[], size_t nValues, size_t* index) {

    ASSERT_NVALUES(1);

    switch (vr) {
        case vr_time:
            values[(*index)++] = comp->time;
            return OK;
        case vr_h:
            values[(*index)++] = M(h) ;
            return OK;
        case vr_p0:
            values[(*index)++] = M(p0) ;
            return OK;
        case vr_p1:
            values[(*index)++] = M(p1) ;
            return OK;
        case vr_p2:
            values[(*index)++] = M(p2) ;
            return OK;
        case vr_p3:
            values[(*index)++] = M(p3) ;
            return OK;
        case vr_p4:
            values[(*index)++] = M(p4) ;
            return OK;
        case vr_p5:
            values[(*index)++] = M(p5) ;
            return OK;
        case vr_p6:
            values[(*index)++] = M(p6) ;
            return OK;
        default:
            logError(comp, "Get Float64 is not allowed for value reference %u.", vr);
            return Error;
    }
}

Status setFloat64(ModelInstance* comp, ValueReference vr, const double value[], size_t nValues, size_t* index) {

    ASSERT_NVALUES(1);

    switch (vr) {

        case vr_h:
            M(h) = value[(*index)++];
            return OK;
        case vr_p0:
            M(p0) = value[(*index)++];
            return OK;
        case vr_p1:
            M(p1) = value[(*index)++];
            return OK;
        case vr_p2:
            M(p2) = value[(*index)++];
            return OK;
        case vr_p3:
            M(p3) = value[(*index)++];
            return OK;
        case vr_p4:
            M(p4) = value[(*index)++];
            return OK;
        case vr_p5:
            M(p5) = value[(*index)++];
            return OK;
        case vr_p6:
            M(p6) = value[(*index)++];
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
    case vr_h:
        *value = M(h);
        return OK;
    default:
        logError(comp, "The output derivative for value reference %u is not available.", valueReference);
        return Error;
    }
}

size_t getNumberOfContinuousStates(ModelInstance* comp) {

    UNUSED(comp);

    return 1;
}

size_t getNumberOfEventIndicators(ModelInstance* comp) {

    UNUSED(comp);

    return 1;
}

Status getContinuousStates(ModelInstance *comp, double x[], size_t nx) {

    UNUSED(nx);

    x[0] = M(h);

    return OK;
}

Status setContinuousStates(ModelInstance *comp, const double x[], size_t nx) {

    UNUSED(nx);

    M(h) = x[0];

    return OK;
}

Status getDerivatives(ModelInstance *comp, double dx[], size_t nx) {

    UNUSED(nx);

    return OK;
}

Status getEventIndicators(ModelInstance *comp, double z[], size_t nz) {

    UNUSED(nz);

    // if (M(h) > -EVENT_EPSILON && M(h) <= 0 && M(v) > 0) {
    //     // hysteresis for better stability
    //     z[0] = -EVENT_EPSILON;
    // } else {
    //     z[0] = M(h);
    // }
    //

    return OK;
}

Status eventUpdate(ModelInstance *comp) {
    UNUSED(comp);
}