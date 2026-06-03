# ==============================================================================
# CMakeLists.txt — LUMINAR (ASTRA Audio Suite)
# High-Frequency Enhancer / Air Exciter
# Fase 18.1: Plugin skeleton compilável
# ==============================================================================

if(TARGET juce::juce_audio_processors)

    juce_add_plugin(LUMINAR
        # ---- Produto visível -------------------------------------------------------
        PRODUCT_NAME     "LUMINAR"
        VERSION          "0.1.0"

        # ---- Identificação única do plugin -----------------------------------------
        # PLUGIN_CODE deve ser 4 chars únicos — diferente do AUREQ (Aurq).
        PLUGIN_CODE       Lmnr
        MANUFACTURER_CODE AStr

        # ---- Bundle / Company ------------------------------------------------------
        BUNDLE_ID        "br.com.sidyfurtado.luminar"
        COMPANY_NAME     "ASTRA Audio"

        # ---- Formatos de saída -----------------------------------------------------
        FORMATS          VST3 AU Standalone

        # ---- Características -------------------------------------------------------
        IS_SYNTH              FALSE
        NEEDS_MIDI_INPUT      FALSE
        NEEDS_MIDI_OUTPUT     FALSE
        IS_MIDI_EFFECT        FALSE
        MICROPHONE_ACTIVITIES FALSE
        WITH_AUDIO_ACTIVE_REQUIREMENT TRUE
    )

    # Padrão C++17 (mesmo do AUREQ e do monorepo)
    target_compile_features(LUMINAR PRIVATE cxx_std_17)

    # Desativar substituição de VST2 para evitar conflito de automação
    target_compile_definitions(LUMINAR PRIVATE JUCE_VST3_CAN_REPLACE_VST2=0)

    # ---- Arquivos-fonte -----------------------------------------------------------
    target_sources(LUMINAR PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/ParameterIDs.h
        DSP/AirProcessor.cpp
        DSP/AirProcessor.h
        DSP/AirWaveshaper.cpp
        DSP/AirWaveshaper.h
        UI/PluginEditor.cpp
        UI/PluginEditor.h
        UI/LuminarTheme.cpp
        UI/LuminarTheme.h
    )

    # ---- Include paths ------------------------------------------------------------
    target_include_directories(LUMINAR PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
        ${CMAKE_CURRENT_SOURCE_DIR}/DSP
        ${CMAKE_CURRENT_SOURCE_DIR}/UI
    )

    # ---- Dependências -------------------------------------------------------------
    target_link_libraries(LUMINAR PRIVATE
        shared_code               # Código compartilhado do monorepo
        juce::juce_audio_utils    # AudioProcessor, AudioProcessorEditor, etc.
        juce::juce_dsp            # juce::SmoothedValue, juce::dsp::*
        juce::juce_gui_extra      # Componentes gráficos extras
    )

    # ---- Warnings de compilação ---------------------------------------------------
    if(MSVC)
        target_compile_options(LUMINAR PRIVATE /W4)
    else()
        target_compile_options(LUMINAR PRIVATE -Wall -Wextra -Wpedantic)
    endif()

else()
    message(STATUS "LUMINAR: JUCE targets não disponíveis ainda. Setup do plugin pulado.")
endif()
