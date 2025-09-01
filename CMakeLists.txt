cmake_minimum_required(VERSION 4.0.2)
project(Runner)
set(CMAKE_CXX_STANDARD 17)

# --- Dependencies install location ---
set(DEPS_DIR ${CMAKE_BINARY_DIR}/deps)
set(CMAKE_INSTALL_PREFIX ${DEPS_DIR})

include(FetchContent)

# GLFW
set(GLFW_VERSION 3.3.9)
FetchContent_Declare(
    glfw
    URL https://github.com/glfw/glfw/archive/refs/tags/${GLFW_VERSION}.tar.gz
)
FetchContent_MakeAvailable(glfw)

# GLM
set(GLM_VERSION 1.0.1)
FetchContent_Declare(
    glm
    URL https://github.com/g-truc/glm/archive/refs/tags/${GLM_VERSION}.tar.gz
)
FetchContent_MakeAvailable(glm)

# FreeType
set(FREETYPE_VERSION 2.13.2)
FetchContent_Declare(
    freetype
    URL https://download.savannah.gnu.org/releases/freetype/freetype-${FREETYPE_VERSION}.tar.xz
)
FetchContent_MakeAvailable(freetype)

# GLAD
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG c
)
FetchContent_MakeAvailable(glad)
include_directories(${glad_SOURCE_DIR}/include)

# --- Project sources ---
file(GLOB_RECURSE PROJECT_HEADERS src/*.h)
file(GLOB_RECURSE PROJECT_SOURCES src/*.cpp)
file(GLOB VENDORS_SOURCES ${glad_SOURCE_DIR}/src/glad.c)

add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES} ${PROJECT_HEADERS}
    ${VENDORS_SOURCES}
)

target_link_libraries(${PROJECT_NAME} glfw glm freetype)

# --- Install target ---
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

