cmake_minimum_required(VERSION 3.16)
project(aerothemeplasma-sounds)

set(PROJECT_VERSION "6.6.1")

set(soundthemes
    "Windows 7"
    "Windows 7 Afternoon"
    "Windows 7 Calligraphy"
    "Windows 7 Characters"
    "Windows 7 Cityscape"
    "Windows 7 Delta"
    "Windows 7 Festival"
    "Windows 7 Garden"
    "Windows 7 Heritage"
    "Windows 7 Landscape"
    "Windows 7 Quirky"
    "Windows 7 Raga"
    "Windows 7 Savanna"
    "Windows 7 Sonata"
)

foreach(theme IN LISTS soundthemes)
    install(DIRECTORY "${CMAKE_SOURCE_DIR}/${theme}" DESTINATION ${CMAKE_INSTALL_PREFIX}/share/sounds)
endforeach()

install()

