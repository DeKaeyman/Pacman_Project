# Bouw de logic library zonder SFML-dependentie
add_library(logic STATIC
        utils/Stopwatch.cpp
        utils/Stopwatch.h
        entities/Entity.cpp
        entities/Entity.h
        world/World.cpp
        world/World.h
        factory/AbstractFactory.h
)

# Publieke include-paden (zodat app headers uit logic kan includen)
target_include_directories(logic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})