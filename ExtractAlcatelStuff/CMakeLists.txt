# CMakeList.txt: proyecto de CMake para ExtractAlcatelStuff, incluya el origen y defina
# la lógica específica del proyecto aquí.
#


if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)

    option(STATIC_CRT "Use static CRT libraries" OFF)

    # Rewrite command line flags to use /MT if necessary
    if(STATIC_CRT)
        foreach(flag_var
                CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
                CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
            if(${flag_var} MATCHES "/MD")
                string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
            endif(${flag_var} MATCHES "/MD")
        endforeach(flag_var)
    endif()
endif()

set(SRCS
    midifile/src/Options.cpp
    midifile/src/Binasc.cpp
    midifile/src/MidiEvent.cpp
    midifile/src/MidiEventList.cpp
    midifile/src/MidiFile.cpp
    midifile/src/MidiMessage.cpp
)

set(HDRS
    midifile/include/Binasc.h
    midifile/include/MidiEvent.h
    midifile/include/MidiEventList.h
    midifile/include/MidiFile.h
    midifile/include/MidiMessage.h
    midifile/include/Options.h
)

include_directories(midifile/include)

add_library(midifile STATIC ${SRCS} ${HDRS})

# Agregue un origen al ejecutable de este proyecto.
add_executable (ExtractAlcatelStuff "ExtractAlcatelStuff.cpp" "ExtractAlcatelStuff.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ExtractAlcatelStuff PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(ExtractAlcatelStuff midifile)

# TODO: Agregue pruebas y destinos de instalación si es necesario.
#target_link_libraries(ExtractAlcatelStuff gdiplus.lib)