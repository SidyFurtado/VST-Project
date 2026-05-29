# Resumo Técnico: Fase 8.0.1 — Factory Preset Musical Validation

Este documento registra a validação musical dos 8 presets de fábrica do **AUREQ** e as correções pontuais feitas para alinhar comportamento sonoro, intenção de uso e regra de Dynamic EQ.

---

## 1. Skills Consultadas
- **skill-index.md**: Para confirmar as skills internas aplicáveis ao monorepo.
- **safety-guardian.skill.md**: Para preservar escopo, evitar comandos destrutivos e proteger `Livros/` e PDFs.
- **juce-cmake-engineer.skill.md**: Para validar build CMake/JUCE e artefatos VST3/Standalone.
- **dsp-engineer.skill.md**: Para revisar coerência musical, ranges dinâmicos e estabilidade de filtros.
- **qa-build-validator.skill.md**: Para executar build e testes sem ocultar warnings ou falhas.

---

## 2. Objetivo da Fase
Validar os presets de fábrica embutidos do `PresetManager` e corrigir apenas valores incoerentes com a intenção musical declarada, sem implementar novas funcionalidades.

Regra confirmada para o Dynamic EQ:
- `dynamicRange` positivo = boost/expansão dinâmica.
- `dynamicRange` negativo = redução/atenuação dinâmica.
- `dynamicRange` zero = neutro.

---

## 3. Presets Revisados
1. **Default Flat**
2. **Vocal Presence**
3. **Drum Punch**
4. **Master Air**
5. **Low Cut Cleanup**
6. **Dynamic Vocal Control**
7. **Mid Focus**
8. **Side Air**

Foram revisados nomes, filtros, channel modes, `dynamicRange`, ganhos, slopes e estado seguro de bandas não usadas.

---

## 4. Alterações Feitas
- **APVTS estruturalmente preservado**: nenhum parâmetro novo foi criado, nenhum ID foi alterado e a contagem permanece em 108 parâmetros.
- **Exceção autorizada**: o range do parâmetro existente `bandNDynamicRange` foi corrigido de `0..24 dB` para `-24..24 dB`, permitindo a regra já documentada de atenuação dinâmica negativa.
- **Dynamic Vocal Control**:
  - Banda 4 em 2.8 kHz: `dynamicRange` alterado para `-3.0 dB`, threshold `-18.0 dBFS`, release `140 ms`.
  - Banda 5 em 6.5 kHz: `dynamicRange` alterado para `-5.0 dB`, threshold `-20.0 dBFS`, attack `5 ms`, release `90 ms`.
- **Drum Punch**:
  - Boost em 60 Hz reduzido de `+4.0 dB` para `+3.0 dB` e Q suavizado para `1.2`.
  - Ataque em agudos movido de 5.0 kHz para 4.8 kHz, reduzido de `+3.0 dB` para `+2.0 dB` e Q suavizado para `1.0`.
- **Master Air**:
  - High Shelf no canal Side reduzido de `+2.0 dB` para `+1.5 dB`.
- **Side Air**:
  - High Shelf no canal Side movido de 8.0 kHz para 10.0 kHz e reduzido de `+3.0 dB` para `+2.0 dB`.

---

## 5. Motivo Musical das Alterações
- **Dynamic Vocal Control** agora reduz aspereza e sibilância quando o sinal ultrapassa o threshold, em vez de expandir as regiões problemáticas.
- **Drum Punch** mantém impacto de bumbo e ataque de caixa/pratos, mas com menos risco de grave inchado e presença agressiva.
- **Master Air** permanece sutil para uso em mix/master, com abertura Side menos exagerada.
- **Side Air** fica mais alto em frequência e moderado em ganho, criando largura e brilho sem harshness evidente.

---

## 6. Confirmações de Escopo
- **APVTS**: estruturalmente não alterado; sem parâmetros novos, sem IDs alterados, sem mudança de contagem. Apenas o range de `Dynamic Range` foi corrigido para permitir valores negativos.
- **DSP**: não alterado.
- **UI**: não alterada.
- **Analyzer FFT**: não alterado.
- **A/B, undo/redo e dropdown**: não implementados.
- **Livros/PDFs**: não lidos, não movidos e não alterados.

---

## 7. Resultado do Build
Comando executado:
```bash
cmake --build build
```

Resultado:
```text
[ 23%] Built target shared_code
[ 59%] Built target AUREQ
[ 72%] Built target AUREQ_Standalone
[ 86%] Built target AUREQ_VST3
[100%] Built target AUREQ_FilterTests
```

Status: **aprovado**. O build gerou warnings de depreciação já existentes relacionados à assinatura antiga de `juce::AudioParameterFloat`, sem erros de compilação.

---

## 8. Resultado dos Testes
Comando executado:
```bash
./build/tests/AUREQ_FilterTests
```

Resultado:
```text
[PASS] Biquad Coefficient Tests completed successfully.
[PASS] EQBand Block Processing Tests completed successfully.
[PASS] Filter Stability and Safety Tests completed successfully.
SUCCESS: All AUREQ DSP Filter Tests Passed!
```

Status: **aprovado**.

---

## 9. Caminhos dos Artefatos
- **VST3**: `/Users/sidyziin/Documents/VST - Project/build/plugins/01_EQ/AUREQ_artefacts/VST3/AUREQ.vst3`
- **Standalone**: `/Users/sidyziin/Documents/VST - Project/build/plugins/01_EQ/AUREQ_artefacts/Standalone/AUREQ.app`
- **Testes**: `/Users/sidyziin/Documents/VST - Project/build/tests/AUREQ_FilterTests`

---

## 10. Próximo Passo Recomendado
Validar os presets em material real de voz, drums, mix bus e stereo bus dentro de uma DAW, ajustando somente valores musicais se algum preset soar excessivo em uso prático.
