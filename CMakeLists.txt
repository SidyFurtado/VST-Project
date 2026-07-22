cmake_minimum_required(VERSION 3.15)

# ==============================================================================
# PROJETO VST - Project (Monorepo)
# ==============================================================================
project(VSTProject VERSION 1.0.0 LANGUAGES C CXX)

# Exigir C++17 ou superior
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Configurar diretório de saída para binários construídos
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ==============================================================================
# DOWNLOAD E CONFIGURAÇÃO DO ONNX RUNTIME (v1.18.1)
# ==============================================================================
include(FetchContent)

if(APPLE)
    set(ONNXRUNTIME_URL "https://github.com/microsoft/onnxruntime/releases/download/v1.18.1/onnxruntime-osx-universal2-1.18.1.tgz")
elseif(WIN32)
    set(ONNXRUNTIME_URL "https://github.com/microsoft/onnxruntime/releases/download/v1.18.1/onnxruntime-win-x64-1.18.1.zip")
else()
    set(ONNXRUNTIME_URL "https://github.com/microsoft/onnxruntime/releases/download/v1.18.1/onnxruntime-linux-x64-1.18.1.tgz")
endif()

message(STATUS "Configuring ONNX Runtime from: ${ONNXRUNTIME_URL}")

FetchContent_Declare(
    onnxruntime_bin
    URL ${ONNXRUNTIME_URL}
)

FetchContent_GetProperties(onnxruntime_bin)
if(NOT onnxruntime_bin_POPULATED)
    FetchContent_Populate(onnxruntime_bin)
endif()

# Criar biblioteca importada global para ser consumida pelos subdiretórios
add_library(onnxruntime SHARED IMPORTED GLOBAL)

if(WIN32)
    set_target_properties(onnxruntime PROPERTIES
        IMPORTED_LOCATION "${onnxruntime_bin_SOURCE_DIR}/lib/onnxruntime.dll"
        IMPORTED_IMPLIB "${onnxruntime_bin_SOURCE_DIR}/lib/onnxruntime.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${onnxruntime_bin_SOURCE_DIR}/include"
    )
else()
    if(APPLE)
        set(ONNXRUNTIME_LIB_NAME "libonnxruntime.dylib")
    else()
        set(ONNXRUNTIME_LIB_NAME "libonnxruntime.so")
    endif()
    
    set_target_properties(onnxruntime PROPERTIES
        IMPORTED_LOCATION "${onnxruntime_bin_SOURCE_DIR}/lib/${ONNXRUNTIME_LIB_NAME}"
        INTERFACE_INCLUDE_DIRECTORIES "${onnxruntime_bin_SOURCE_DIR}/include"
    )
endif()

# Variável de cache para expor o caminho raiz do binário para os plugins copiarem pós-build
set(ONNXRUNTIME_BIN_DIR "${onnxruntime_bin_SOURCE_DIR}" CACHE INTERNAL "Caminho raiz do ONNX Runtime")

# ==============================================================================
# INTEGRAÇÃO DO JUCE FRAMEWORK
# ==============================================================================
add_subdirectory(external/JUCE)

# ==============================================================================
# SUBDIRETÓRIOS DO MONOREPO
# ==============================================================================
# Incluir código compartilhado entre múltiplos plugins
add_subdirectory(shared)

# Incluir os plugins da ASTRA Audio Suite
add_subdirectory(plugins/AUREQ)
add_subdirectory(plugins/LUMINAR)
add_subdirectory(plugins/GRAVITY)
add_subdirectory(plugins/VOID)

# Incluir o subdiretório de testes unitários do DSP
add_subdirectory(tests)
