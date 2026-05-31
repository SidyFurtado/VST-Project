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
# INTEGRAÇÃO DO JUCE FRAMEWORK
# ==============================================================================
add_subdirectory(external/JUCE)

# ==============================================================================
# SUBDIRETÓRIOS DO MONOREPO
# ==============================================================================
# Incluir código compartilhado entre múltiplos plugins
add_subdirectory(shared)

# Incluir o primeiro plugin (EQ Paramétrico)
add_subdirectory(plugins/01_EQ)

# Incluir o subdiretório de testes unitários do DSP (Fase 5.21)
add_subdirectory(tests)

# Segundo plugin da ASTRA Audio Suite — LUMINAR (High-Frequency Enhancer)
add_subdirectory(plugins/02_AIR)

# Placeholders para inclusão futura dos próximos plugins
# add_subdirectory(plugins/02_Compressor)   # compressor — fase futura
# add_subdirectory(plugins/03_Saturation)
# add_subdirectory(plugins/04_Limiter)
