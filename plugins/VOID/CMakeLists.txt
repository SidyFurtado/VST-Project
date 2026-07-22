# ==============================================================================
# CMakeLists.txt — VOID (ASTRA Audio Suite)
# AI-Powered Voice Isolation and Noise Suppressor
# ==============================================================================

if(TARGET juce::juce_audio_processors)

    juce_add_plugin(VOID
        # ---- Produto visível -------------------------------------------------------
        PRODUCT_NAME     "VOID"
        VERSION          "1.0.0"

        # ---- Identificação única do plugin -----------------------------------------
        # PLUGIN_CODE deve ser 4 chars únicos.
        PLUGIN_CODE       VOID
        PLUGIN_MANUFACTURER_CODE AStr

        # ---- Bundle / Company ------------------------------------------------------
        BUNDLE_ID        "br.com.sidyfurtado.void"
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

    # Padrão C++17 (mesmo do resto do monorepo)
    target_compile_features(VOID PRIVATE cxx_std_17)

    # Desativar substituição de VST2 para evitar conflito de automação
    target_compile_definitions(VOID PRIVATE JUCE_VST3_CAN_REPLACE_VST2=0)

    # ---- Arquivos-fonte -----------------------------------------------------------
    target_sources(VOID PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
        Source/ParameterIDs.h
        Source/InferenceCore.h
        Source/InferenceCore.cpp
        Source/InferenceThread.h
        Source/InferenceThread.cpp
        Source/SpectralNoiseReducer.h
        Source/SpectralNoiseReducer.cpp
    )

    # ---- Include paths ------------------------------------------------------------
    target_include_directories(VOID PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
    )

    # ---- Dependências -------------------------------------------------------------
    target_link_libraries(VOID PRIVATE
        shared_code               # Código compartilhado do monorepo
        onnxruntime               # Biblioteca importada do ONNX Runtime
        juce::juce_audio_utils    # AudioProcessor, AudioProcessorEditor, etc.
        juce::juce_dsp            # Filtros, smoothed values, etc.
        juce::juce_gui_extra      # Componentes de interface extras
    )

    # ---- Otimização de CPU e Vetorização ------------------------------------------
    if(MSVC)
        target_compile_options(VOID PRIVATE /W4)
        target_compile_options(VOID PRIVATE $<$<CONFIG:Release>:/arch:AVX2 /O2>)
        target_link_options(VOID PRIVATE $<$<CONFIG:Release>:/LTCG>)
    else()
        target_compile_options(VOID PRIVATE -Wall -Wextra -Wpedantic)
        target_compile_options(VOID PRIVATE $<$<CONFIG:Release>:-O3 -flto=auto -fvisibility=hidden>)
        target_link_options(VOID PRIVATE $<$<CONFIG:Release>:-flto=auto -fvisibility=hidden>)
        # SIMD flags por arquitetura (Release only)
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|amd64|AMD64")
            target_compile_options(VOID PRIVATE $<$<CONFIG:Release>:-mavx2 -mfma>)
        elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
            # Apple Silicon NEON é ativado por padrão; flags otimizadas p/ pipeline M-series
            target_compile_options(VOID PRIVATE $<$<CONFIG:Release>:-mcpu=apple-m1>)
        endif()
    endif()

    # ---- Universal Binary (macOS: x86_64 + arm64) no CMake global -----------------
    if(APPLE)
        set_target_properties(VOID PROPERTIES
            OSX_ARCHITECTURES "arm64;x86_64"
        )
    endif()

    # ---- Cópia Pós-Build da Biblioteca Dinâmica do ONNX Runtime -----------------
    if(APPLE)
        # No macOS, precisamos copiar a dylib para dentro do MacOS/ do bundle de cada formato
        if(TARGET VOID_Standalone)
            add_custom_command(TARGET VOID_Standalone POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${ONNXRUNTIME_BIN_DIR}/lib/libonnxruntime.dylib"
                    "$<TARGET_FILE_DIR:VOID_Standalone>/"
            )
        endif()

        if(TARGET VOID_VST3)
            add_custom_command(TARGET VOID_VST3 POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${ONNXRUNTIME_BIN_DIR}/lib/libonnxruntime.dylib"
                    "$<TARGET_FILE_DIR:VOID_VST3>/"
            )
        endif()

        if(TARGET VOID_AU)
            add_custom_command(TARGET VOID_AU POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${ONNXRUNTIME_BIN_DIR}/lib/libonnxruntime.dylib"
                    "$<TARGET_FILE_DIR:VOID_AU>/"
            )
        endif()
    elseif(WIN32)
        # No Windows, copiamos a DLL para junto do executável
        if(TARGET VOID_Standalone)
            add_custom_command(TARGET VOID_Standalone POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${ONNXRUNTIME_BIN_DIR}/lib/onnxruntime.dll"
                    "$<TARGET_FILE_DIR:VOID_Standalone>/"
            )
        endif()

        if(TARGET VOID_VST3)
            add_custom_command(TARGET VOID_VST3 POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${ONNXRUNTIME_BIN_DIR}/lib/onnxruntime.dll"
                    "$<TARGET_FILE_DIR:VOID_VST3>/"
            )
        endif()
    endif()

else()
    message(STATUS "VOID: JUCE targets não disponíveis ainda. Setup do plugin pulado.")
endif()
