# Phase 5.19 — APVTS Advanced Band Parameters

**Status:** ✅ Concluída  
**Data:** 2026-05-24

---

## Objetivo

Adicionar 56 novos parâmetros ao APVTS (7 por banda × 8 bandas), tornando o slope persistente via APVTS e preparando o terreno para Dynamic EQ e Channel Mode em fases futuras.

**Contagem de parâmetros: 52 → 108**

---

## Parâmetros Adicionados (por banda)

| ID Pattern        | Tipo              | Valores              | Padrão   | Status DSP           |
|-------------------|-------------------|----------------------|----------|----------------------|
| `bandNSlope`      | Choice (3 opções) | 12 / 24 / 48 dB/oct  | 12       | ✅ Ativo             |
| `bandNDynamicEnabled`  | Bool         | false / true         | false    | 🔒 Reservado (5.20+) |
| `bandNDynamicRange`    | Float (dB)   | 0 – 24 dB            | 6.0 dB   | 🔒 Reservado         |
| `bandNDynamicThreshold` | Float (dBFS) | -60 – 0 dBFS        | -24 dBFS | 🔒 Reservado         |
| `bandNDynamicAttack`   | Float (ms)   | 0.1 – 200 ms (skew)  | 10 ms    | 🔒 Reservado         |
| `bandNDynamicRelease`  | Float (ms)   | 10 – 2000 ms (skew)  | 100 ms   | 🔒 Reservado         |
| `bandNChannelMode`     | Choice (5)   | Stereo/Mid/Side/L/R  | Stereo   | 🔒 Reservado         |

---

## Mudanças Arquiteturais

### Estado temporário removido
- `std::array<std::atomic<int>, 8> bandSlopes` removido do `PluginProcessor`
- `setBandSlope()` / `getBandSlope()` removidos do `PluginProcessor`
- `EQProcessorCore::setBandSlope()` removido

### Slope agora persistente via APVTS
- `EQProcessorCore::updateFromAPVTS()` lê `bandNSlope` diretamente do APVTS
- Choice index `0=12`, `1=24`, `2=48` dB/oct convertido para int antes de passar ao `EQBand`
- `PluginEditor` slope buttons agora leem/escrevem o parâmetro APVTS (DAW automation + preset save)

---

## Arquivos Modificados

| Arquivo | Mudança |
|---------|---------|
| `Source/ParameterIDs.h` | +7 declarações de ID helpers |
| `Source/ParameterIDs.cpp` | +7 implementações de ID helpers |
| `Source/PluginProcessor.h` | Remove `bandSlopes`, `setBandSlope`, `getBandSlope` |
| `Source/PluginProcessor.cpp` | Remove init `bandSlopes`, remove `setBandSlope/getBandSlope`, add 56 params ao layout |
| `DSP/EQProcessorCore.h` | Remove `setBandSlope` declaration |
| `DSP/EQProcessorCore.cpp` | Remove `setBandSlope`, read `bandNSlope` em `updateFromAPVTS` |
| `UI/PluginEditor.cpp` | Slope buttons usam APVTS, `timerCallback` lê APVTS slope |

---

## Verificação de Build

```
[100%] Built target AUREQ_VST3
```

- ✅ 0 erros de compilação  
- ✅ Warnings: apenas deprecations pré-existentes (sem novos)  
- ✅ 108 parâmetros totais confirmados (4 globais + 13 × 8 bandas)

---

## Notas

> **Slope é agora persistente**: O valor é salvo em presets e projetos de DAW automaticamente via APVTS XML. O comportamento sonoro do Cut slope (cascaded biquads) permanece idêntico ao implementado na Fase 5.18.

> **Dynamic EQ e Channel Mode**: Parâmetros registrados no APVTS para garantir compatibilidade futura de preset, mas o DSP ainda não está implementado. A ativação do DSP ocorrerá em fase dedicada.
