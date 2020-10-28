cmake_minimum_required(VERSION 3.1)

set(PRODUCT_NAME test)

project(${PRODUCT_NAME})

include(navigation_crosscompile.cmake)

add_executable(${PRODUCT_NAME} ${PRODUCT_NAME}.c)

add_custom_target(build
        COMMAND avr-gcc -g -O1 -mmcu=${MCU} -o ${PRODUCT_NAME}.elf ${PRODUCT_NAME}.c
        WORKING_DIRECTORY ../
        )

add_custom_target(to-hex
        COMMAND avr-objcopy -O ihex -R .eeprom ${PRODUCT_NAME}.elf ${PRODUCT_NAME}.hex
        WORKING_DIRECTORY ../
        DEPENDS build
        )

add_custom_target(upload
        COMMAND avrdude -v -p ${MCU} -P ${PORT} -c arduino -U flash:w:${PRODUCT_NAME}.hex
        WORKING_DIRECTORY ../
        DEPENDS to-hex)