# ==============================================================================
# Biblioteca de Código Compartilhado (shared_code)
# ==============================================================================

# Criação de biblioteca estática contendo código compartilhado de DSP e GUI
add_library(shared_code STATIC)

# Configurar C++17 para a biblioteca compartilhada
target_compile_features(shared_code PUBLIC cxx_std_17)

# Mapear diretórios de inclusão públicos
target_include_directories(shared_code PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Adicionar arquivos de cabeçalho (.h) e implementação (.cpp)
target_sources(shared_code PRIVATE
    shared_dummy.cpp
    # dsp/SmoothedParameter.h
    # gui/theme/ThemeManager.cpp
    # gui/theme/ThemeManager.h
    # utilities/AudioMath.h
)

# Vincular ao JUCE caso o framework tenha sido carregado no escopo raiz
if(TARGET juce::juce_audio_processors)
    target_link_libraries(shared_code PUBLIC
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_gui_extra
    )
else()
    message(STATUS "shared_code: JUCE targets não disponíveis ainda. Mapeamento de dependências pulado.")
endif()
