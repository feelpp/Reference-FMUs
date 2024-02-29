In order to build the FMU for the reduced basis (stationary case only, for the moment):

- create a "resources/<MODEL_NAME>" folder inside ReducedBasis
- put the MOR plugin inside "resources/<MODEL_NAME>/binaries"
- put the "crbdb" folder created by the MOR offline phase in "resources/<MODEL_NAME>"
- put the mormodels.json and the cfg file for the MOR simulation in "resources/<MODEL_NAME>" as well

The structure of the resource folder is, mimicking the structure of the .fpp archive that contains Feel++ MOR reduced basis information
```
resources
└── <MODEL_NAME>
    ├── binaries
    │   └── libplugin.so
    ├── crbdb
    ├── mormodels.json
    └── <MODEL_NAME>.cfg
```

The ReducedBasis FMU example was created with the `thermalbuilding` reduced model, which has 7 parameters.