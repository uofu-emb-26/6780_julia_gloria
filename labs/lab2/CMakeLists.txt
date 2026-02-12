# Add C source files here.
add_executable(lab2
    src/lab2.c
    src/hal_gpio.c
    src/stm32f0xx_hal_msp.c
    src/stm32f0xx_it.c
)

# Add the map file to the list of files to be removed with 'clean' target
set_target_properties(lab2 PROPERTIES ADDITIONAL_CLEAN_FILES lab2.map)

# Add user defined include paths
target_include_directories(lab2 PRIVATE
    inc
)

# Add project symbols (macros)
target_compile_definitions(lab2 PRIVATE
)

# Add linked libraries
target_link_libraries(lab2
    STM32_Drivers
)

flash_target(lab2)
