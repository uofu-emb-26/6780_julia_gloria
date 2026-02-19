# Add C source files here.
add_executable(lab3
    src/lab3.c
    src/hal_gpio.c
    src/stm32f0xx_hal_msp.c
    src/stm32f0xx_it.c
)

# Add the map file to the list of files to be removed with 'clean' target
set_target_properties(lab3 PROPERTIES ADDITIONAL_CLEAN_FILES lab3.map)

# Add user defined include paths
target_include_directories(lab3 PRIVATE
    inc
)

# Add project symbols (macros)
target_compile_definitions(lab3 PRIVATE
)

# Add linked libraries
target_link_libraries(lab3
    STM32_Drivers
)

flash_target(lab3)
