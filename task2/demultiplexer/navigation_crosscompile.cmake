set(CMAKE_SYSTEM_NAME Generic)

set(F_CPU 16000000UL)
set(MCU atmega328p)
set(PORT COM3)

set(CMAKE_C_COMPILER avr-gcc)
set(CMAKE_CXX_COMPILER avr-g++)

add_definitions(
        -DF_CPU=${F_CPU}
)

set(CMAKE_EXE_LINKER_FLAGS -mmcu=${MCU})

add_compile_options(
        -mmcu=${MCU}
        -O1
        -g
)