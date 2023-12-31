cmake_minimum_required(VERSION 3.12)

# Change your executable name to something creative!
set(NAME virtualpet) # <-- Name your project/executable here!

include(pimoroni_pico_import.cmake)
include(pico_sdk_import.cmake)

# Gooey boilerplate
project(${NAME} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_compile_definitions(PET_SCREEN_W=64)
add_compile_definitions(PLATFORM_PICO)

# Initialize the SDK
pico_sdk_init()

# Add your source files
add_executable(${NAME}
    source/vpet_pico.cpp
	vpet/pet_animation.c
	vpet/pet_foods.c
	vpet/pet_logic.c
	vpet/pet_main.c
	vpet/pet_minigame.c
	vpet/pet_sprites.c
	vpet/pet_video.c
	vpet/random.c
	vpet/utility.c
)

# Include required libraries
# This assumes `pimoroni-pico` is stored alongside your project
include(common/pimoroni_i2c)
include(common/pimoroni_bus)
#include(libraries/bitmap_fonts/bitmap_fonts)
#include(libraries/hershey_fonts/hershey_fonts)
include(libraries/gfx_pack/gfx_pack)

# Don't forget to link the libraries you need!
target_link_libraries(${NAME}
	pico_stdlib
	gfx_pack
)

# create map/bin/hex file etc.
pico_add_extra_outputs(${NAME})

# Set up files for the release packages
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.uf2
    ${CMAKE_CURRENT_LIST_DIR}/README.md
    DESTINATION .
)

set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_GENERATOR "ZIP" "TGZ")
include(CPack)
