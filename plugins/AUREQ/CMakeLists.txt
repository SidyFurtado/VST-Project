# ==============================================================================
# CMakeLists para o plugin AUREQ (flagship parametric equalizer)
# ==============================================================================

if(TARGET juce::juce_audio_processors)
    # Criar e configurar o plugin JUCE
    juce_add_plugin(AUREQ
        # Nome do projeto/produto
        PRODUCT_NAME "AUREQ"
        # Versão do plugin
        VERSION "0.9.0"
        # Bundle ID reverso sem espaços para empacotamento macOS/VST3
        BUNDLE_ID "br.com.sidyfurtado.aureq"
        # Formatos de build de saída (adicionado AU para macOS)
        FORMATS VST3 AU Standalone
        # Fabricante do plugin
        COMPANY_NAME "ASTRA Audio"
        # Identificador exclusivo do plugin (4 caracteres)
        PLUGIN_CODE Aurq
        # Código do fabricante (4 caracteres)
        PLUGIN_MANUFACTURER_CODE AStr
        # Categorias de efeito
        IS_SYNTH FALSE
        NEEDS_MIDI_INPUT FALSE
        NEEDS_MIDI_OUTPUT FALSE
        IS_MIDI_EFFECT FALSE
        
        # Ícone do App Standalone (macOS / Windows)
        ICON_BIG "../../assets/brand/previews/aureq-plugin-icon-preview-1024.png"
        # Canais de I/O padrão e requisitos adicionais
        MICROPHONE_ACTIVITIES FALSE
        WITH_AUDIO_ACTIVE_REQUIREMENT TRUE
    )

    # Configurar padrão C++17 para o plugin
    target_compile_features(AUREQ PRIVATE cxx_std_17)

    # Desativar compatibilidade de substituição de VST2 para evitar conflito de automação de parâmetros
    target_compile_definitions(AUREQ PRIVATE JUCE_VST3_CAN_REPLACE_VST2=0)

    # Vincular fontes e bibliotecas do plugin
    target_sources(AUREQ PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/ParameterIDs.cpp
        Source/ParameterIDs.h
        Source/AnalyzerFIFO.cpp
        Source/AnalyzerFIFO.h
        Source/AnalyzerProcessor.cpp
        Source/AnalyzerProcessor.h
        Source/PresetManager.cpp
        Source/PresetManager.h
        DSP/BiquadFilter.cpp
        DSP/BiquadFilter.h
        DSP/ParameterSmoother.cpp
        DSP/ParameterSmoother.h
        DSP/EQBand.cpp
        DSP/EQBand.h
        DSP/EQProcessorCore.cpp
        DSP/EQProcessorCore.h
        UI/PluginEditor.cpp
        UI/PluginEditor.h
        UI/Theme.cpp
        UI/Theme.h
        UI/Localization.cpp
        UI/EQGraphView.cpp
        UI/EQGraphView.h
    )

    # Incluir diretórios de cabeçalhos locais do plugin
    target_include_directories(AUREQ PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
        ${CMAKE_CURRENT_SOURCE_DIR}/DSP
        ${CMAKE_CURRENT_SOURCE_DIR}/UI
    )

    # Mapear dependências do framework JUCE e código compartilhado
    target_link_libraries(AUREQ PRIVATE
        # Vincula a biblioteca compartilhada do monorepo
        shared_code
        # Vincula os módulos nativos do JUCE
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_gui_extra
    )

    # Habilitar proteção de runtime ou configurações especiais de compilação
    if(MSVC)
        target_compile_options(AUREQ PRIVATE /W4)
    else()
        target_compile_options(AUREQ PRIVATE -Wall -Wextra -Wpedantic)
    endif()

else()
    message(STATUS "AUREQ: JUCE targets não disponíveis ainda. Setup do plugin pulado.")
endif()
