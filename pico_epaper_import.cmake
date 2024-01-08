# Copy this to any project.
# And include this file after calling pico_sdk_init().
# EPAPER_C_PATH is expected to set to Pico_ePaper_Code/c/

if (DEFINED ENV{EPAPER_C_PATH} AND (NOT EPAPER_C_PATH))
    set(EPAPER_C_PATH $ENV{EPAPER_C_PATH})
    message("Using EPAPER_C_PATH from environment ('${EPAPER_C_PATH}')")
endif ()

# add a compilation subdirectory
add_subdirectory(${EPAPER_C_PATH}/lib/Config ${CMAKE_CURRENT_SOURCE_DIR}/build/Config)
add_subdirectory(${EPAPER_C_PATH}/lib/e-Paper ${CMAKE_CURRENT_SOURCE_DIR}/build/e-Paper)
add_subdirectory(${EPAPER_C_PATH}/lib/Fonts ${CMAKE_CURRENT_SOURCE_DIR}/build/Fonts)
add_subdirectory(${EPAPER_C_PATH}/lib/GUI ${CMAKE_CURRENT_SOURCE_DIR}/build/GUI)
add_subdirectory(${EPAPER_C_PATH}/examples ${CMAKE_CURRENT_SOURCE_DIR}/build/examples)

# add a header directory
include_directories(${EPAPER_C_PATH}/examples)
include_directories(${EPAPER_C_PATH}/./lib/Config)
include_directories(${EPAPER_C_PATH}/./lib/GUI)
include_directories(${EPAPER_C_PATH}/./lib/e-Paper)