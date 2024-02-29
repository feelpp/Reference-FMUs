set(FMI_VERSION 2)

set (MODEL_NAMES Heat)

foreach (MODEL_NAME ${MODEL_NAMES})

set(TARGET_NAME ${MODEL_NAME})

SET(HEADERS
    ${MODEL_NAME}/config.h
    ${MODEL_NAME}/heat.h
    ${CMAKE_SOURCE_DIR}/include/cosimulation.h
    ${CMAKE_SOURCE_DIR}/include/model.h
)

if (${FMI_VERSION} EQUAL 2)
  SET(HEADERS
    ${HEADERS}
    ${CMAKE_SOURCE_DIR}/include/fmi2Functions.h
    ${CMAKE_SOURCE_DIR}/include/fmi2FunctionTypes.h
    ${CMAKE_SOURCE_DIR}/include/fmi2TypesPlatform.h
  )
elseif (${FMI_VERSION} EQUAL 3)
  SET(HEADERS
    ${HEADERS}
    ${CMAKE_SOURCE_DIR}/include/fmi3Functions.h
    ${CMAKE_SOURCE_DIR}/include/fmi3FunctionTypes.h
    ${CMAKE_SOURCE_DIR}/include/fmi3PlatformTypes.h
  )
endif()

SET(SOURCES
  ${MODEL_NAME}/model.c
  # ${MODEL_NAME}/heat.c
  ${MODEL_NAME}/heat.cpp
  ${CMAKE_SOURCE_DIR}/src/fmi${FMI_VERSION}Functions.c
  ${CMAKE_SOURCE_DIR}/src/cosimulationFeelpp.c
)

add_library(${TARGET_NAME} SHARED
  ${HEADERS}
  ${SOURCES}
  ${MODEL_NAME}/FMI${FMI_VERSION}${FMI_TYPE}.xml
  ${MODEL_NAME}/buildDescription.xml
)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/fmus)

target_compile_definitions(${TARGET_NAME} PRIVATE
  FMI_VERSION=${FMI_VERSION}
  DISABLE_PREFIX
  FEELPP_DIM=2
)

if (MSVC)
  target_compile_options(${TARGET_NAME} PRIVATE /W4 /WX)
else()
  target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -Wpedantic )
endif()

if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
  target_link_options(${TARGET_NAME} PRIVATE "-static-libgcc")
endif()

if (CMAKE_C_COMPILER_ID STREQUAL "Intel")
  target_link_options(${TARGET_NAME} PRIVATE "-static-intel" "-static-libgcc")
endif()

if (${FMI_VERSION} EQUAL 1 AND "${FMI_TYPE}" STREQUAL CS)
  target_compile_definitions(${TARGET_NAME} PRIVATE FMI_COSIMULATION)
endif()

target_include_directories(${TARGET_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${MODEL_NAME}
)

set(FMU_BUILD_DIR temp/${MODEL_NAME})

set_target_properties(${TARGET_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY         "${FMU_BUILD_DIR}/binaries/${FMI_PLATFORM}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${FMU_BUILD_DIR}/binaries/${FMI_PLATFORM}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${FMU_BUILD_DIR}/binaries/${FMI_PLATFORM}"
    LIBRARY_OUTPUT_DIRECTORY         "${FMU_BUILD_DIR}/binaries/${FMI_PLATFORM}"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG   "${FMU_BUILD_DIR}/binaries/${FMI_PLATFORM}"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${FMU_BUILD_DIR}/binaries/${FMI_PLATFORM}"
    ARCHIVE_OUTPUT_DIRECTORY         "${FMU_BUILD_DIR}/binaries/${FMI_PLATFORM}"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG   "${FMU_BUILD_DIR}/binaries/${FMI_PLATFORM}"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${FMU_BUILD_DIR}/binaries/${FMI_PLATFORM}"
)

set_target_properties(${TARGET_NAME} PROPERTIES PREFIX "")
set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME ${MODEL_NAME})

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)

target_link_libraries( ${TARGET_NAME} PUBLIC Feelpp::feelpp)

# modelDescription.xml
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy
  ${CMAKE_CURRENT_SOURCE_DIR}/${MODEL_NAME}/FMI${FMI_VERSION}${FMI_TYPE}.xml
  "${FMU_BUILD_DIR}/modelDescription.xml"
)

# model specific header and source
foreach (SOURCE_FILE config.h model.c)
  add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/${MODEL_NAME}/${SOURCE_FILE}"
    "${FMU_BUILD_DIR}/sources/${SOURCE_FILE}"
  )
endforeach(SOURCE_FILE)

# common headers
foreach (SOURCE_FILE model.h cosimulation.h)
  add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_SOURCE_DIR}/include/${SOURCE_FILE}"
    "${FMU_BUILD_DIR}/sources/${SOURCE_FILE}"
  )
endforeach(SOURCE_FILE)

# common sources
foreach (SOURCE_FILE fmi${FMI_VERSION}Functions.c cosimulationFeelpp.c)
  add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_SOURCE_DIR}/src/${SOURCE_FILE}"
    "${FMU_BUILD_DIR}/sources/${SOURCE_FILE}"
  )
endforeach(SOURCE_FILE)

# all.c / buildDescription.xml
if (${FMI_VERSION} LESS 3)
  add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_BINARY_DIR}/all.c"
    "${FMU_BUILD_DIR}/sources/all.c"
  )
else()
  add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/${MODEL_NAME}/buildDescription.xml"
    "${FMU_BUILD_DIR}/sources/buildDescription.xml"
  )
endif()

set(ARCHIVE_FILES "modelDescription.xml" "binaries" "sources")

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${MODEL_NAME}/resources")
  add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/${MODEL_NAME}/resources"
    "${FMU_BUILD_DIR}/resources/"
  )
  set(ARCHIVE_FILES ${ARCHIVE_FILES} "resources")
endif()

# delete unintended files from binaries (Release configuration only)
if (MSVC)
  add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND if $<CONFIG:Release> neq 0 (
      ${CMAKE_COMMAND} -E rm -f
      "${FMU_BUILD_DIR}/binaries/${FMI_PLATFORM}/${MODEL_NAME}.exp"
      "${FMU_BUILD_DIR}/binaries/${FMI_PLATFORM}/${MODEL_NAME}.lib"
     )
  )
endif()

# create ZIP archive
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E tar "cfv" ${CMAKE_BINARY_DIR}/fmus/${MODEL_NAME}.fmu --format=zip
  ${ARCHIVE_FILES}
  WORKING_DIRECTORY ${FMU_BUILD_DIR} COMMENT "Creating ZIP archive"
)

install(FILES ${CMAKE_BINARY_DIR}/fmus/${MODEL_NAME}.fmu DESTINATION ${CMAKE_INSTALL_PREFIX})

endforeach(MODEL_NAME)