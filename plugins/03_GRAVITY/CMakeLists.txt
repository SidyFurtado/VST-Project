# ==============================================================================
# CMakeLists.txt — GRAVITY (ASTRA Audio Suite)
# Dynamic Compressor — Peak/RMS detection, Soft-Knee, ARC
# Fase inicial: plugin skeleton compilável
# ==============================================================================

if(TARGET juce::juce_audio_processors)

    juce_add_plugin(GRAVITY
        # ---- Produto visível -------------------------------------------------------
        PRODUCT_NAME     "GRAVITY"
        VERSION          "0.1.0"

        # ---- Identificação única do plugin -----------------------------------------
        # PLUGIN_CODE deve ser 4 chars únicos — diferente do AUREQ (Aurq) e LUMINAR (Lmnr)
        PLUGIN_CODE       Grvt
        MANUFACTURER_CODE AStr

        # ---- Bundle / Company ------------------------------------------------------
        BUNDLE_ID        "br.com.sidyfurtado.gravity"
        COMPANY_NAME     "ASTRA Audio"

        # ---- Formatos de saída -----------------------------------------------------
        FORMATS          VST3 Standalone

        # ---- Características -------------------------------------------------------
        IS_SYNTH              FALSE
        NEEDS_MIDI_INPUT      FALSE
        NEEDS_MIDI_OUTPUT     FALSE
        IS_MIDI_EFFECT        FALSE
        MICROPHONE_ACTIVITIES FALSE
        WITH_AUDIO_ACTIVE_REQUIREMENT TRUE
    )

    # Padrão C++17 (mesmo do AUREQ, LUMINAR e do monorepo)
    target_compile_features(GRAVITY PRIVATE cxx_std_17)

    # Desativar substituição de VST2 para evitar conflito de automação
    target_compile_definitions(GRAVITY PRIVATE JUCE_VST3_CAN_REPLACE_VST2=0)

    # ---- Arquivos-fonte -----------------------------------------------------------
    target_sources(GRAVITY PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/ParameterIDs.h
        DSP/GravityProcessor.cpp
        DSP/GravityProcessor.h
        UI/PluginEditor.cpp
        UI/PluginEditor.h
        UI/GravityTheme.cpp
        UI/GravityTheme.h
    )

    # ---- Include paths ------------------------------------------------------------
    target_include_directories(GRAVITY PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
        ${CMAKE_CURRENT_SOURCE_DIR}/DSP
        ${CMAKE_CURRENT_SOURCE_DIR}/UI
    )

    # ---- Dependências -------------------------------------------------------------
    target_link_libraries(GRAVITY PRIVATE
        shared_code               # Código compartilhado do monorepo
        juce::juce_audio_utils    # AudioProcessor, AudioProcessorEditor, etc.
        juce::juce_dsp            # juce::SmoothedValue, juce::dsp::*
        juce::juce_gui_extra      # Componentes gráficos extras
    )

    # ---- Warnings de compilação ---------------------------------------------------
    if(MSVC)
        target_compile_options(GRAVITY PRIVATE /W4)
    else()
        target_compile_options(GRAVITY PRIVATE -Wall -Wextra -Wpedantic)
    endif()

else()
    message(STATUS "GRAVITY: JUCE targets não disponíveis ainda. Setup do plugin pulado.")
endif()
