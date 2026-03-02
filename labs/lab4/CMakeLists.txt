# Add C source files here.
add_executable(lab4
    src/lab4.c
    src/hal_gpio.c
    src/stm32f0xx_hal_msp.c
    src/stm32f0xx_it.c
)

# Add the map file to the list of files to be removed with 'clean' target
set_target_properties(lab4 PROPERTIES ADDITIONAL_CLEAN_FILES lab4.map)

# Add user defined include paths
target_include_directories(lab4 PRIVATE
    inc
)

# Add project symbols (macros)
target_compile_definitions(lab4 PRIVATE
)

# Add linked libraries
target_link_libraries(lab4
    STM32_Drivers
)

flash_target(lab4)
