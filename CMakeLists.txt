cmake_minimum_required(VERSION 3.16)
project(TheBattles VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Player.cpp
    src/Bot.cpp
    src/Map.cpp
    src/Storm.cpp
    src/BattlePass.cpp
)

# Header files
set(HEADERS
    include/Game.h
    include/Player.h
    include/Bot.h
    include/Map.h
    include/Storm.h
    include/BattlePass.h
    include/Weapon.h
    include/Entity.h
    include/Constants.h
)

# Add executable
add_executable(TheBattles ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(TheBattles PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Find SFML
find_package(SFML 2.5 COMPONENTS graphics window system QUIET)

if(SFML_FOUND)
    target_link_libraries(TheBattles sfml-graphics sfml-window sfml-system)
else()
    # Manual SFML linking for cross-compilation
    if(CMAKE_CROSSCOMPILING AND WIN32)
        # For MinGW cross-compilation
        target_link_libraries(TheBattles 
            sfml-graphics sfml-window sfml-system
            opengl32 winmm gdi32
        )
    else()
        # Try pkg-config
        find_package(PkgConfig QUIET)
        if(PKG_CONFIG_FOUND)
            pkg_check_modules(SFML REQUIRED sfml-graphics sfml-window sfml-system)
            target_include_directories(TheBattles PRIVATE ${SFML_INCLUDE_DIRS})
            target_link_libraries(TheBattles ${SFML_LIBRARIES})
        else()
            message(FATAL_ERROR "SFML not found. Install SFML 2.5+ development libraries.")
        endif()
    endif()
endif()

# Windows-specific settings
if(WIN32)
    # Hide console window in release
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        set_target_properties(TheBattles PROPERTIES WIN32_EXECUTABLE TRUE)
    endif()
    
    # Static linking for MinGW
    if(MINGW)
        target_link_options(TheBattles PRIVATE -static-libgcc -static-libstdc++)
    endif()
endif()

# Install
install(TARGETS TheBattles DESTINATION bin)
