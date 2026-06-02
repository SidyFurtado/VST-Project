# GRAVITY — ASTRA Audio Suite

**Tipo:** Compressor Dinâmico  
**Versão:** 0.1.0 (Skeleton)  
**Código do plugin:** `Grvt`  
**Bundle ID:** `br.com.sidyfurtado.gravity`

---

## Visão Geral

GRAVITY é o compressor dinâmico de alta performance da ASTRA Audio Suite. Inspirado na fidelidade de circuito do Waves R-Comp, mas construído com DSP moderno e sem artefatos de processamento.

## Parâmetros

| ID                | Nome       | Range             | Default  | Unidade |
|-------------------|------------|-------------------|----------|---------|
| `gravity_thresh`  | Threshold  | -60.0 → 0.0       | -18.0    | dB      |
| `gravity_ratio`   | Ratio      | 1.0 → 50.0        | 4.0      | :1      |
| `gravity_attack`  | Attack     | 0.1 → 500.0       | 10.0     | ms      |
| `gravity_release` | Release    | 5.0 → 3000.0      | 150.0    | ms      |
| `gravity_gain`    | Makeup Gain| 0.0 → 30.0        | 0.0      | dB      |
| `gravity_mode`    | Mode       | Electro / Opto    | Electro  | —       |
| `gravity_arc`     | ARC        | Off / On          | Off      | —       |

## Algoritmos DSP

### Detector Path
- **Electro (Peak):** seguidor de pico absoluto de bloco com decay suave.
- **Opto (RMS):** janela deslizante RMS de 30ms — resposta mais natural e musical.

### Curva de Transferência
Curva logarítmica com **Soft-Knee fixo de 6 dB** (±3 dB ao redor do threshold):
- Abaixo do knee: unity gain.
- Zona de knee: blend quadrático suave.
- Acima do knee: slope de ratio completo.

### ARC — Auto Release Control
O release se ajusta dinamicamente:
- Transiente detectado → release reduzido em **50%**.
- Sinal estável → release no valor configurado pelo usuário.

## Estrutura de Arquivos

```
plugins/03_GRAVITY/
├── CMakeLists.txt
├── README.md
├── DSP/
│   ├── GravityProcessor.h      # Engine DSP principal
│   └── GravityProcessor.cpp
├── Source/
│   ├── PluginProcessor.h       # AudioProcessor + APVTS
│   ├── PluginProcessor.cpp
│   └── ParameterIDs.h          # IDs dos parâmetros (prefixo gravity_)
├── UI/
│   ├── GravityTheme.h          # Paleta crimson / dark mode
│   ├── GravityTheme.cpp
│   ├── PluginEditor.h          # Editor + LookAndFeel + Meters
│   └── PluginEditor.cpp
├── Presets/
│   └── .gitkeep
└── Tests/
    └── .gitkeep
```

## Qualidade de Buffer

Segue os padrões estabelecidos pelo AUREQ e LUMINAR:
- `ScopedNoDenormals` em toda fronteira de processo.
- Guards `std::isfinite` em todas as operações de nível de amostra.
- `SmoothedValue` no makeup gain (ramp de 20ms).
- Estado por canal para evitar bleed inter-canal no cálculo de gain.
- Hard clip em ±4.0 (headroom compatível com LUMINAR).
