# AUREQ Genre-First Preset Candidate Blueprint

Blueprint documental da Fase 8.3B para a futura expansão da biblioteca de factory presets do AUREQ.

Formato alvo:

```text
Gênero >
    Instrumento / Fonte >
        Presets candidatos
```

Este documento não implementa presets, não altera `PresetManager`, não cria arquivos externos de preset e não muda APVTS, DSP, analyzer FFT, UI, CMake, PDFs ou `Livros/`.

## Regras musicais aplicadas

- Presets são pontos de partida; boosts permanecem conservadores.
- Mix bus/master usam movimentos mais sutis que fontes individuais.
- Dynamic EQ negativo é usado para controlar mud, harshness, sibilância e low end instável.
- Dynamic EQ positivo aparece apenas como realce responsivo intencional e pequeno.
- Side é reservado para ar, largura e limpeza lateral; grave em Side deve ser evitado.
- `48 dB/oct` aparece apenas em rumble/isolamento forte ou limpeza lateral crítica.
- Q largo é padrão para shaping musical; Q estreito só para ressonância/notch.
- Nomes são únicos nesta lista para evitar duplicação direta na futura biblioteca.

Quantidade total proposta: **111 presets candidatos**.

---

## Forro

### Vocal

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Forro Vocal Front Clarity | Presence | Colocar vocal na frente sem afinar. | Low Cut 75 Hz, 24 dB/oct; Bell -1.2 dB em 320 Hz, Q 1.0; Bell +1.8 dB em 2.6 kHz, Q 1.0. | Low | Evitar presence acima de +2 dB se a voz ja estiver brilhante. |
| Forro Vocal Soft De-Esser | De-Esser | Controlar sibilancia em voz clara. | Bell dinamico 6.5 kHz, range -4 dB, Q 1.7; High Shelf +0.8 dB em 11 kHz, Q 0.7. | Medium | Dynamic EQ negativo; nao compensar com air alto. |
| Forro Vocal Warm Lead | Warmth | Dar corpo ao lead vocal sem embolar. | Low Cut 70 Hz, 12 dB/oct; Low Shelf +1.2 dB em 170 Hz, Q 0.8; Bell -1 dB em 350 Hz, Q 1.0. | Medium | Checar conflito com zabumba e baixo. |

### Backing Vocal

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Forro Backing Vocal Pocket | Cleanup | Recuar backing vocals mantendo inteligibilidade. | Low Cut 120 Hz, 24 dB/oct; Bell -1.5 dB em 500 Hz, Q 1.0; Bell -1 dB em 3 kHz, Q 1.0. | Low | Nao roubar foco do lead. |

### Sanfona / Accordion

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Forro Accordion Nasal Tame | De-Harsh | Reduzir nasalidade da sanfona. | Low Cut 100 Hz, 24 dB/oct; Bell -2 dB em 950 Hz, Q 1.4; Bell -1.2 dB em 3.2 kHz, Q 1.2. | Medium | Validar para nao apagar o timbre regional. |
| Forro Accordion Presence Lift | Presence | Trazer sanfona para a frente com brilho controlado. | Low Cut 90 Hz, 24 dB/oct; Bell +1.4 dB em 1.4 kHz, Q 1.0; Bell +1 dB em 3.5 kHz, Q 1.0. | Medium | Manter boosts abaixo de +1.5 dB. |

### Zabumba

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Forro Zabumba Low Punch | Punch | Reforcar impacto grave sem sub exagerado. | Low Cut 32 Hz, 24 dB/oct; Bell +1.8 dB em 80 Hz, Q 1.0; Bell -1.5 dB em 280 Hz, Q 1.0. | Medium | Nao somar com bass boost. |
| Forro Zabumba Mud Cleanup | Cleanup | Tirar caixa/embolamento da zabumba. | Low Cut 35 Hz, 24 dB/oct; Bell -2 dB em 240 Hz, Q 1.1; Bell +1 dB em 2.5 kHz, Q 1.0. | Low | Ataque deve continuar natural. |

### Percussion

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Forro Percussion Soft Top | De-Harsh | Suavizar triangulo, shaker e transientes agudos. | Low Cut 180 Hz, 24 dB/oct; Bell dinamico 6.8 kHz, range -3 dB, Q 1.5; High Shelf -0.8 dB em 12 kHz. | Medium | Controlar sem escurecer a percussao. |
| Forro Percussion Dance Bite | Presence | Aumentar definicao ritmica. | Low Cut 130 Hz, 24 dB/oct; Bell +1.2 dB em 3.2 kHz, Q 1.0; High Shelf +0.8 dB em 10 kHz. | Low | Evitar dureza em PA pequeno. |

### Bass

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Forro Bass Warm Lock | Low Control | Firmar baixo com calor. | Low Cut 30 Hz, 12 dB/oct; Bell +1.2 dB em 95 Hz, Q 0.9; Bell -1.5 dB em 260 Hz, Q 1.0. | Medium | Conferir relacao com zabumba. |

### Mix Bus

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Forro Mix Dance Cleanup | Mix Bus | Limpar low-mid e manter energia de danca. | Low Cut 30 Hz, 12 dB/oct; Bell -1 dB em 300 Hz, Q 0.8; High Shelf +0.7 dB em 11 kHz. | Medium | Mix bus: manter todos os boosts abaixo de +1 dB. |
| Forro Mix Gentle Air | Mix Bus | Abrir mix sem deixar estridente. | Bell -0.8 dB em 350 Hz, Q 0.8; High Shelf +0.8 dB em 12 kHz; Side High Shelf +0.5 dB em 11 kHz. | Medium | Side sutil; sem grave em Side. |

---

## Trap

### Vocal

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Trap Lead Vocal Shine Control | Air | Vocal moderno com air controlado. | Low Cut 100 Hz, 24 dB/oct; Bell +1.4 dB em 3 kHz, Q 1.0; High Shelf +1.5 dB em 11 kHz, Q 0.7; Bell dinamico 7 kHz, range -3 dB. | Medium | Air condicionado por de-esser. |
| Trap Lead Vocal Dark Clarity | Presence | Clareza para vocal escuro sem exagerar brilho. | Low Cut 90 Hz, 24 dB/oct; Bell -1.3 dB em 280 Hz, Q 1.0; Bell +1.8 dB em 2.4 kHz, Q 1.0. | Low | Evitar high shelf se beat ja for brilhante. |

### Adlibs

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Trap Adlibs Side Air | Width | Abrir adlibs sem competir com lead. | Low Cut 140 Hz, 24 dB/oct; Side High Shelf +1.4 dB em 10 kHz, Q 0.7; Mid Bell -1 dB em 2.5 kHz. | Medium | Side somente para air; sem grave lateral. |
| Trap Adlibs Sibilance Tuck | De-Esser | Domar adlibs agudos e repetitivos. | Low Cut 130 Hz, 24 dB/oct; Bell dinamico 6.5 kHz, range -5 dB, Q 1.7; High Shelf -0.5 dB em 12 kHz. | Medium | Dynamic EQ negativo obrigatorio. |

### 808

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Trap 808 Tight Mono Focus | Low Control | Centralizar e limpar 808. | Low Cut 24 Hz, 24 dB/oct; Bell -1.5 dB em 190 Hz, Q 1.0; Side Low Cut 140 Hz, 24 dB/oct. | High | Validar em monitores e fones; evitar Side grave. |
| Trap 808 Note Tamer | Dynamic Control | Controlar notas que explodem. | Bell dinamico 55 Hz, range -3 dB, Q 1.0; Bell -1.2 dB em 220 Hz, Q 1.0; Low Cut 22 Hz, 12 dB/oct. | High | Ajustar threshold por material antes de virar factory. |
| Trap 808 Small Speaker Harmonics | Presence | Ajudar traducao em caixas pequenas. | Bell +1 dB em 900 Hz, Q 1.0; Bell +0.8 dB em 1.6 kHz, Q 1.1; Low Cut 25 Hz, 24 dB/oct. | Medium | Nao transformar harmonico em distorcao dura. |

### Kick

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Trap Kick Sub Boundary | Punch | Separar kick do 808. | Low Cut 28 Hz, 24 dB/oct; Bell +1.5 dB em 70 Hz, Q 1.0; Bell -1.5 dB em 180 Hz, Q 1.0; Bell +1 dB em 3 kHz. | Medium | Conferir fase/percepcao com 808. |

### Snare / Clap

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Trap Clap Snap Tame | De-Harsh | Clap definido sem agressividade. | Low Cut 160 Hz, 24 dB/oct; Bell +1.2 dB em 2.2 kHz, Q 1.0; Bell dinamico 5.5 kHz, range -3 dB, Q 1.4. | Medium | Controlar topo antes de adicionar presence. |

### Hi-Hat

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Trap Hi-Hat Silk Tame | De-Harsh | Amaciar hats rapidos. | Low Cut 350 Hz, 24 dB/oct; Bell -2 dB em 7.5 kHz, Q 1.4; High Shelf -0.7 dB em 13 kHz. | Medium | Nao remover groove nem brilho necessario. |

### Synth

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Trap Synth Lead De-Harsh | De-Harsh | Controlar lead ressonante. | Low Cut 120 Hz, 24 dB/oct; Bell dinamico 3.5 kHz, range -3 dB, Q 1.3; High Cut 14 kHz, 12 dB/oct. | Medium | Q moderado; evitar notch generico. |

### Mix Bus

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Trap Mix Low Focus | Mix Bus | Controlar grave mantendo peso. | Low Cut 24 Hz, 12 dB/oct; Bell dinamico 70 Hz, range -1.5 dB, Q 0.9; Bell -0.8 dB em 300 Hz, Q 0.8. | High | Mix bus sub precisa de validacao auditiva. |
| Trap Mix Gloss Guard | Mix Bus | Dar polimento sem sibilancia global. | Bell dinamico 7 kHz, range -1.5 dB, Q 1.2; High Shelf +0.7 dB em 12 kHz; Side High Shelf +0.5 dB em 11 kHz. | Medium | Master/mix bus: boosts abaixo de +1 dB. |

---

## Hip-Hop

### Rap Vocal

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Hip-Hop Rap Vocal Forward | Presence | Voz na frente do beat. | Low Cut 90 Hz, 24 dB/oct; Bell -1.2 dB em 280 Hz, Q 1.0; Bell +1.8 dB em 2.2 kHz, Q 1.0. | Low | Evitar agressividade em 3-5 kHz. |
| Hip-Hop Rap Vocal Warm Cut | Warmth | Rap encorpado com diccao. | Low Cut 80 Hz, 12 dB/oct; Low Shelf +1 dB em 180 Hz, Q 0.8; Bell +1.2 dB em 2 kHz, Q 1.0. | Medium | Conferir se nao embola com sample. |
| Hip-Hop Rap Vocal Dynamic Bite | Dynamic Control | Manter ataque sem harshness. | Bell +1 dB em 2.4 kHz, Q 1.0; Bell dinamico 4 kHz, range -3 dB, Q 1.3; Bell dinamico 6.5 kHz, range -4 dB. | Medium | Dynamic EQ negativo para picos. |

### Beat Bus

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Hip-Hop Beat Bus Low-Mid Clean | Cleanup | Abrir espaco para rap. | Low Cut 30 Hz, 12 dB/oct; Bell -1.5 dB em 320 Hz, Q 0.9; Bell -0.8 dB em 700 Hz, Q 1.0. | Medium | Nao deixar sample fino. |
| Hip-Hop Beat Bus Sample Air | Air | Dar brilho sutil ao sample/beat. | High Shelf +1 dB em 10 kHz, Q 0.7; Bell -1 dB em 250 Hz, Q 0.9; Side High Shelf +0.6 dB em 11 kHz. | Medium | Side sutil e sem low boost. |

### Kick

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Hip-Hop Kick Knock Control | Punch | Kick com knock e menos boxiness. | Low Cut 28 Hz, 24 dB/oct; Bell +1.8 dB em 85 Hz, Q 1.0; Bell -1.5 dB em 300 Hz; Bell +1 dB em 2.8 kHz. | Medium | Ajustar com baixo ligado. |

### Snare

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Hip-Hop Snare Body Crack | Punch | Corpo e crack sem dureza. | Low Cut 90 Hz, 12 dB/oct; Bell +1.5 dB em 200 Hz, Q 1.0; Bell +1.2 dB em 2.5 kHz; Bell dinamico 5 kHz, range -2 dB. | Medium | Evitar crack excessivo. |

### Bass

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Hip-Hop Bass Groove Definition | Low Control | Baixo definido em groove. | Low Cut 30 Hz, 12 dB/oct; Bell +1.2 dB em 90 Hz, Q 0.9; Bell -1.3 dB em 260 Hz; Bell +0.8 dB em 900 Hz. | Medium | Nao brigar com kick. |
| Hip-Hop Bass Note Leveler | Dynamic Control | Reduzir notas graves que saltam. | Bell dinamico 80 Hz, range -2.5 dB, Q 1.0; Bell -1 dB em 250 Hz, Q 1.0. | High | Threshold precisa de teste por material. |

### Sample

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Hip-Hop Sample Dust Cleanup | Cleanup | Limpar sample antigo sem matar textura. | Low Cut 70 Hz, 12 dB/oct; Bell -1.5 dB em 350 Hz, Q 1.0; High Cut 14 kHz, 12 dB/oct. | Medium | Manter carater do sample. |

### Mix Bus

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Hip-Hop Mix Punch Preserve | Mix Bus | Preservar impacto com limpeza sutil. | Low Cut 28 Hz, 12 dB/oct; Bell -0.8 dB em 300 Hz, Q 0.8; High Shelf +0.6 dB em 11 kHz. | Medium | Mix bus com movimentos pequenos. |
| Hip-Hop Mix Vocal Pocket | Mix Bus | Abrir janela para vocal no beat. | Mid Bell -0.8 dB em 2.4 kHz, Q 1.0; Bell -0.8 dB em 350 Hz, Q 0.8; Side High Shelf +0.5 dB em 10 kHz. | Medium | Verificar se nao recua beat demais. |

---

## Funk BR

### Vocal

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Funk BR Vocal Bite Control | Presence | Voz agressiva mas controlada. | Low Cut 100 Hz, 24 dB/oct; Bell +1.5 dB em 2.6 kHz, Q 1.0; Bell dinamico 4.2 kHz, range -3 dB. | Medium | Presence com de-harsh dinamico. |
| Funk BR Vocal De-Esser Tight | De-Esser | Controlar sibilancia em vocal frontal. | Bell dinamico 6.8 kHz, range -5 dB, Q 1.7; High Shelf +0.6 dB em 11 kHz. | Medium | Nao repor sibilancia com air. |

### Beat

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Funk BR Beat Dry Cleanup | Cleanup | Limpar beat seco para vocal. | Low Cut 30 Hz, 12 dB/oct; Bell -1.2 dB em 300 Hz; Bell -1 dB em 700 Hz, Q 1.0. | Low | Nao afinar beat. |
| Funk BR Beat Top Tame | De-Harsh | Reduzir top agressivo em beat. | Bell dinamico 6 kHz, range -2.5 dB, Q 1.3; High Shelf -0.6 dB em 13 kHz. | Medium | Manter energia de pista. |

### Kick

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Funk BR Kick Front Punch | Punch | Kick presente em celular e PA. | Low Cut 28 Hz, 24 dB/oct; Bell +1.5 dB em 75 Hz; Bell +1 dB em 2.5 kHz; Bell -1 dB em 240 Hz. | Medium | Conferir contra 808. |

### Bass / 808

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Funk BR 808 Street Control | Low Control | 808 forte com sub controlado. | Low Cut 24 Hz, 24 dB/oct; Bell dinamico 55 Hz, range -3 dB, Q 1.0; Bell -1.5 dB em 210 Hz. | High | Validacao auditiva obrigatoria. |
| Funk BR Bass Mono Lock | Low Control | Manter grave central e limpo. | Side Low Cut 140 Hz, 24 dB/oct; Mid Bell +1 dB em 90 Hz, Q 0.9; Bell -1 dB em 260 Hz. | High | Evitar qualquer boost em Side grave. |

### Percussion

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Funk BR Percussion Cut Through | Presence | Percussao ritmica sem aspereza. | Low Cut 150 Hz, 24 dB/oct; Bell +1.2 dB em 3.5 kHz; Bell dinamico 6.5 kHz, range -2.5 dB. | Medium | Checar fadiga em volumes altos. |
| Funk BR Percussion Room Clean | Cleanup | Reduzir sala/boxiness de percussao. | Bell -1.8 dB em 550 Hz, Q 1.2; Low Cut 120 Hz, 24 dB/oct; High Shelf +0.5 dB em 10 kHz. | Low | Evitar som pequeno. |

### Lead / Synth

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Funk BR Lead Thin Space | Cleanup | Abrir espaco para vocal no lead. | Low Cut 180 Hz, 24 dB/oct; Bell -1.5 dB em 2.5 kHz, Q 1.0; Side High Shelf +0.6 dB em 10 kHz. | Medium | Nao deslocar centro do arranjo. |

### Mix Bus

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Funk BR Mix Low Guard | Mix Bus | Segurar low end sem perder peso. | Low Cut 24 Hz, 12 dB/oct; Bell dinamico 70 Hz, range -1.5 dB; Bell -0.8 dB em 280 Hz. | High | Mix bus low control exige teste em PA/fones. |
| Funk BR Mix Harsh Tame | Mix Bus | Domar agressividade geral. | Bell dinamico 4 kHz, range -1.5 dB, Q 1.1; High Shelf -0.4 dB em 13 kHz. | Medium | Nao escurecer demais o beat. |

---

## Rock

### Vocal

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Rock Vocal Edge Control | De-Harsh | Vocal atravessando guitarras sem dor. | Low Cut 85 Hz, 24 dB/oct; Bell +1.3 dB em 2.4 kHz; Bell dinamico 4 kHz, range -3 dB. | Medium | De-harsh antes de boost extra. |
| Rock Vocal Chest Focus | Warmth | Recuperar corpo de vocal em mix densa. | Low Cut 75 Hz, 12 dB/oct; Low Shelf +1 dB em 180 Hz; Bell -1 dB em 350 Hz; Bell +1 dB em 2 kHz. | Medium | Evitar embolo com guitarras. |

### Electric Guitar

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Rock Electric Guitar Fizz Tame | De-Harsh | Remover fizz de distorcao. | Low Cut 90 Hz, 24 dB/oct; Bell -1.5 dB em 3.8 kHz, Q 1.2; High Cut 10 kHz, 12 dB/oct. | Medium | Nao escurecer guitarra solo. |
| Rock Electric Guitar Mid Bite | Presence | Dar presenca sem agredir. | Low Cut 85 Hz, 24 dB/oct; Bell +1.3 dB em 1.6 kHz, Q 1.0; Bell -1 dB em 300 Hz. | Medium | Evitar competir com vocal. |
| Rock Electric Guitar Double Width | Width | Abrir guitarras duplas. | Low Cut 90 Hz, 24 dB/oct; Side High Shelf +0.8 dB em 8 kHz; Side Bell -1 dB em 250 Hz. | Medium | Side sem grave; testar mono. |

### Acoustic Guitar

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Rock Acoustic Guitar Strum Clean | Cleanup | Limpar violao em arranjo cheio. | Low Cut 110 Hz, 24 dB/oct; Bell -1.8 dB em 280 Hz; Bell +1 dB em 4 kHz. | Low | Preservar naturalidade da palhetada. |

### Bass

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Rock Bass Pick Definition | Presence | Destacar baixo com palheta. | Low Cut 30 Hz, 12 dB/oct; Bell +1 dB em 900 Hz; Bell -1.2 dB em 250 Hz; Bell -0.8 dB em 3 kHz se agressivo. | Medium | Nao deixar clank dominar. |

### Kick

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Rock Kick Beater Punch | Punch | Kick audivel em parede de guitarras. | Low Cut 30 Hz, 24 dB/oct; Bell +1.5 dB em 70 Hz; Bell -1.5 dB em 300 Hz; Bell +1.2 dB em 4 kHz. | Medium | Ataque sem click artificial. |

### Snare

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Rock Snare Ring Tamer | Dynamic Control | Controlar ring sem matar caixa. | Low Cut 90 Hz, 12 dB/oct; Bell dinamico 1.2 kHz, range -3 dB, Q 1.5; Bell +1 dB em 220 Hz. | Medium | Validar frequencia de ring por kit. |

### Drum Bus

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Rock Drum Bus Cymbal Guard | Dynamic Control | Domar pratos duros no bus. | Bell dinamico 5.5 kHz, range -2 dB, Q 1.1; High Shelf -0.5 dB em 12 kHz; Low Cut 28 Hz, 12 dB/oct. | Medium | Nao abafar overheads. |
| Rock Drum Bus Weight Glue | Punch | Peso e cola em drums. | Low Shelf +0.8 dB em 85 Hz; Bell -1 dB em 320 Hz; Bell +0.8 dB em 3 kHz. | Medium | Bus com boosts pequenos. |

### Mix Bus

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Rock Mix Harshness Guard | Mix Bus | Controlar aspereza geral. | Bell dinamico 3.5 kHz, range -1.5 dB, Q 1.0; Bell -0.7 dB em 300 Hz; High Shelf +0.4 dB em 11 kHz. | Medium | Master/mix bus: nada agressivo. |

---

## Sertanejo

### Vocal

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Sertanejo Vocal Silk Air | Air | Polimento vocal suave. | Low Cut 75 Hz, 24 dB/oct; Bell +1.2 dB em 2.8 kHz; High Shelf +1.4 dB em 12 kHz; Bell dinamico 6.8 kHz, range -3 dB. | Medium | De-esser antes do air. |
| Sertanejo Vocal Intimacy Warm | Warmth | Proximidade e corpo no lead. | Low Cut 70 Hz, 12 dB/oct; Low Shelf +1 dB em 170 Hz; Bell -1 dB em 350 Hz; Bell +0.8 dB em 2.5 kHz. | Medium | Checar low-mid com violao. |
| Sertanejo Vocal Dynamic Polish | Dynamic Control | Controlar sibilancia e harsh em voz polida. | Bell dinamico 3.2 kHz, range -2.5 dB; Bell dinamico 7 kHz, range -4 dB; High Shelf +0.7 dB em 12 kHz. | Medium | Dynamic negativo, boosts discretos. |

### Backing Vocal

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Sertanejo Backing Vocal Blend | Cleanup | Backings encaixados atras do lead. | Low Cut 120 Hz, 24 dB/oct; Bell -1.2 dB em 2.8 kHz; High Shelf +0.6 dB em 10 kHz. | Low | Lead continua no centro. |

### Acoustic Guitar

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Sertanejo Acoustic Guitar Shine | Air | Violao aberto sem aspereza. | Low Cut 95 Hz, 24 dB/oct; Bell -1.5 dB em 260 Hz; High Shelf +1.2 dB em 10 kHz. | Low | Se houver chiado, reduzir shelf. |
| Sertanejo Acoustic Guitar Pick Tame | De-Harsh | Controlar palhetada dura. | Bell dinamico 3.8 kHz, range -3 dB, Q 1.3; Low Cut 90 Hz, 24 dB/oct; Bell -1 dB em 300 Hz. | Medium | Nao apagar ataque ritmico. |

### Viola

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Sertanejo Viola Presence Clean | Presence | Dar destaque a viola sem nasalidade. | Low Cut 100 Hz, 24 dB/oct; Bell -1.2 dB em 800 Hz; Bell +1.3 dB em 3 kHz. | Medium | Evitar nasal em 1 kHz. |

### Bass

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Sertanejo Bass Soft Body | Warmth | Baixo redondo e discreto. | Low Cut 30 Hz, 12 dB/oct; Bell +1 dB em 100 Hz; Bell -1.2 dB em 250 Hz; Bell +0.6 dB em 800 Hz. | Low | Nao dominar vocal/violao. |

### Percussion

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Sertanejo Percussion Gentle Top | Air | Topo leve em percussao. | Low Cut 150 Hz, 24 dB/oct; Bell -1 dB em 600 Hz; High Shelf +0.8 dB em 10 kHz. | Low | Evitar excesso de brilho acumulado. |
| Sertanejo Percussion Harsh Guard | De-Harsh | Controlar transientes agudos. | Bell dinamico 5.5 kHz, range -2.5 dB, Q 1.3; High Shelf -0.4 dB em 13 kHz. | Medium | Manter naturalidade. |

### Mix Bus

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Sertanejo Mix Polished Smile | Mix Bus | Smile discreto comercial. | Low Shelf +0.5 dB em 90 Hz; Bell -0.8 dB em 300 Hz; High Shelf +0.8 dB em 12 kHz. | Medium | Mix bus: max +0.8 dB. |
| Sertanejo Mix Vocal Space | Mix Bus | Abrir espaco para vocal. | Mid Bell -0.7 dB em 320 Hz; Mid Bell +0.5 dB em 2.5 kHz; Side High Shelf +0.4 dB em 11 kHz. | Medium | Validar se nao afeta arranjo. |

---

## Pop

### Vocal

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Pop Vocal Gloss Control | Air | Vocal brilhante sem sibilancia. | Low Cut 90 Hz, 24 dB/oct; Bell +1.2 dB em 3.2 kHz; High Shelf +1.5 dB em 12 kHz; Bell dinamico 7 kHz, range -4 dB. | Medium | Air sempre com de-esser dinamico. |
| Pop Vocal Intimate Focus | Presence | Vocal proximo e inteligivel. | Low Cut 80 Hz, 24 dB/oct; Bell -1 dB em 300 Hz; Bell +1.5 dB em 2.5 kHz; High Shelf +0.8 dB. | Low | Evitar voz fina. |
| Pop Vocal Dynamic De-Harsh | Dynamic Control | Controlar picos de presence. | Bell dinamico 3.5 kHz, range -3 dB, Q 1.2; Bell dinamico 6.5 kHz, range -3.5 dB. | Medium | Nao usar range positivo aqui. |

### Backing Vocal

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Pop Backing Vocal Wide Halo | Width | Abrir camadas de backing. | Low Cut 140 Hz, 24 dB/oct; Side High Shelf +1.2 dB em 10 kHz; Mid Bell -1 dB em 2.8 kHz. | Medium | Side sem corpo grave. |
| Pop Backing Vocal Sibilance Net | De-Esser | Controlar sibilancia acumulada. | Bell dinamico 6.8 kHz, range -4 dB, Q 1.6; High Shelf +0.5 dB em 11 kHz. | Medium | Evitar chorus vocal agressivo. |

### Synth

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Pop Synth Lead Polish | Presence | Synth lead presente e limpo. | Low Cut 140 Hz, 24 dB/oct; Bell -1 dB em 350 Hz; Bell +1.2 dB em 2.5 kHz; High Shelf +0.7 dB. | Medium | Nao mascarar vocal. |
| Pop Synth Pad Side Air | Width | Pad largo e leve. | Low Cut 180 Hz, 24 dB/oct; Side High Shelf +1 dB em 10 kHz; Mid Bell -0.8 dB em 400 Hz. | Medium | Grave lateral proibido. |

### Drums

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Pop Drum Bus Snap Glue | Punch | Drums pop com snap coeso. | Low Cut 28 Hz, 12 dB/oct; Bell +0.8 dB em 90 Hz; Bell +1 dB em 3 kHz; Bell dinamico 6 kHz, range -1.5 dB. | Medium | Bus com ganhos pequenos. |

### Bass

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Pop Bass Tight Foundation | Low Control | Low end firme para pop moderno. | Low Cut 28 Hz, 12 dB/oct; Bell +1 dB em 90 Hz; Bell -1.3 dB em 250 Hz; Bell +0.7 dB em 900 Hz. | Medium | Ajustar com kick. |

### Guitar

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Pop Guitar Sparkle Pocket | Air | Guitarra brilhante em segundo plano. | Low Cut 120 Hz, 24 dB/oct; Bell -1 dB em 300 Hz; High Shelf +1 dB em 9 kHz. | Low | Nao competir com vocal air. |

### Mix Bus

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Pop Mix Gentle Width | Mix Bus | Largura sutil e limpa. | Side Low Cut 130 Hz, 24 dB/oct; Side High Shelf +0.6 dB em 11 kHz; Bell -0.6 dB em 300 Hz. | Medium | Mono check recomendado. |
| Pop Mix Air Control | Mix Bus | Polir topo sem sibilancia global. | Bell dinamico 7 kHz, range -1.5 dB; High Shelf +0.7 dB em 13 kHz. | Medium | Boost abaixo de +1 dB no bus. |

---

## Gospel

### Lead Vocal

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Gospel Lead Vocal Lift Safe | Presence | Lead vocal claro acima da banda. | Low Cut 80 Hz, 24 dB/oct; Bell -1 dB em 300 Hz; Bell +1.5 dB em 2.6 kHz; High Shelf +0.8 dB. | Low | Preservar corpo emocional. |
| Gospel Lead Vocal Warm Air | Warmth | Corpo e ar sem sibilancia. | Low Cut 75 Hz, 12 dB/oct; Low Shelf +0.8 dB em 180 Hz; High Shelf +1 dB em 11 kHz; Bell dinamico 6.8 kHz, range -3 dB. | Medium | Conferir com choir. |

### Choir

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Gospel Choir Blend Wide | Width | Coro largo e encaixado. | Low Cut 140 Hz, 24 dB/oct; Mid Bell -1 dB em 2.5 kHz; Side High Shelf +0.8 dB em 10 kHz. | Medium | Nao tirar foco do lead. |
| Gospel Choir Sibilance Cloud | De-Esser | Controlar sibilancia acumulada do coro. | Bell dinamico 6.5 kHz, range -4 dB, Q 1.5; High Shelf -0.3 dB em 13 kHz. | Medium | Dynamic negativo para vozes multiplas. |

### Piano / Keys

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Gospel Piano Presence Seat | Presence | Piano definido sem mascarar voz. | Low Cut 60 Hz, 12 dB/oct; Bell -1.3 dB em 300 Hz; Bell +1 dB em 2.8 kHz; Side High Shelf +0.5 dB. | Medium | Cuidar para nao competir com lead. |
| Gospel Keys Low-Mid Cleanup | Cleanup | Reduzir acumulacao de keys/pads. | Low Cut 100 Hz, 24 dB/oct; Bell -1.8 dB em 350 Hz, Q 0.9; Bell -0.8 dB em 700 Hz. | Low | Manter sustentacao harmonica. |
| Gospel Organ Soft Air | Air | Abrir orgao/keys sem aspereza. | Low Cut 90 Hz, 12 dB/oct; Bell -1 dB em 500 Hz; High Shelf +0.7 dB em 10 kHz. | Low | Evitar hiss e harsh. |

### Guitar

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Gospel Guitar Clean Strum | Cleanup | Limpar guitarra base. | Low Cut 110 Hz, 24 dB/oct; Bell -1.5 dB em 280 Hz; Bell +0.8 dB em 4 kHz. | Low | Preservar suavidade. |

### Bass

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Gospel Bass Smooth Foundation | Low Control | Baixo redondo e sustentado. | Low Cut 30 Hz, 12 dB/oct; Bell +0.8 dB em 90 Hz; Bell -1.2 dB em 260 Hz; Bell +0.5 dB em 800 Hz. | Medium | Nao embolar com piano. |

### Drums

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Gospel Drum Bus Gentle Punch | Punch | Drums firmes sem agressividade. | Low Cut 28 Hz, 12 dB/oct; Bell +0.8 dB em 80 Hz; Bell -1 dB em 320 Hz; Bell +0.7 dB em 3 kHz. | Medium | Bus deve soar natural. |

### Mix Bus

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Gospel Mix Clarity Lift | Mix Bus | Clarear mix vocal/instrumental. | Bell -0.8 dB em 320 Hz, Q 0.8; High Shelf +0.7 dB em 11 kHz; Bell dinamico 6.5 kHz, range -1.2 dB. | Medium | Brilho sutil para evitar fadiga. |
| Gospel Mix Choir Space | Mix Bus | Controlar low-mid de arranjos grandes. | Bell dinamico 300 Hz, range -1.5 dB, Q 0.8; Side Low Cut 120 Hz, 24 dB/oct. | Medium | Validar em trechos de coro cheio. |

---

## Electronic

### Kick

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Electronic Kick Sub Focus | Punch | Kick sub firme e definido. | Low Cut 25 Hz, 24 dB/oct; Bell +1.5 dB em 60 Hz; Bell -1.2 dB em 220 Hz; Bell +0.8 dB em 3 kHz. | Medium | Ajustar com bass sidechain/arranjo. |
| Electronic Kick Click Tame | De-Harsh | Reduzir click agressivo. | Bell dinamico 4.5 kHz, range -2.5 dB, Q 1.3; Low Cut 28 Hz, 24 dB/oct. | Medium | Nao perder traducao em caixas pequenas. |

### Bass

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Electronic Bass Tight Low | Low Control | Baixo eletronico limpo. | Low Cut 25 Hz, 12 dB/oct; Bell -1.5 dB em 220 Hz; Bell +0.8 dB em 90 Hz; Side Low Cut 140 Hz, 24 dB/oct. | High | Grave lateral precisa ficar limpo. |
| Electronic Bass Harmonic Read | Presence | Traducao em caixas pequenas. | Bell +0.8 dB em 900 Hz; Bell +0.6 dB em 1.7 kHz; Bell dinamico 70 Hz, range -2 dB. | Medium | Harmonics pequenos; sem harsh. |

### Synth

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Electronic Synth Resonance Guard | Dynamic Control | Controlar filtros ressonantes. | Bell dinamico 2.8 kHz, range -3 dB, Q 1.4; Bell dinamico 6 kHz, range -2 dB; Low Cut 100 Hz. | Medium | Frequencias podem variar por synth. |
| Electronic Synth Body Clean | Cleanup | Remover low-mid de synth denso. | Low Cut 120 Hz, 24 dB/oct; Bell -1.5 dB em 350 Hz; Bell +0.8 dB em 2 kHz. | Low | Manter corpo se synth for principal. |

### Lead

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Electronic Lead Bright Safe | Air | Lead aberto sem perfurar. | Low Cut 140 Hz, 24 dB/oct; Bell +1 dB em 3 kHz; Bell dinamico 5.5 kHz, range -2.5 dB; High Shelf +0.6 dB. | Medium | Bright com controle dinamico. |

### Pad

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Electronic Pad Width Cleanup | Width | Pad largo e limpo. | Low Cut 180 Hz, 24 dB/oct; Side Low Cut 150 Hz, 24 dB/oct; Side High Shelf +0.8 dB em 10 kHz; Mid Bell -0.8 dB em 350 Hz. | Medium | Mono check recomendado. |
| Electronic Pad Air Wash | Air | Ar etereo sem hiss. | High Shelf +0.8 dB em 12 kHz; Bell -1 dB em 500 Hz; High Cut 17 kHz, 12 dB/oct se houver hiss. | Low | High cut opcional e suave. |

### Percussion

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Electronic Percussion Top Guard | De-Harsh | Controlar hats/percussao aguda. | Low Cut 250 Hz, 24 dB/oct; Bell dinamico 7 kHz, range -3 dB, Q 1.4; High Shelf -0.4 dB em 14 kHz. | Medium | Manter energia de pista. |

### Mix Bus

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Electronic Mix Side Low Clean | Mix Bus | Limpar grave lateral da mix. | Side Low Cut 130 Hz, 24 dB/oct; Bell -0.6 dB em 300 Hz; Side High Shelf +0.4 dB em 11 kHz. | High | Validar imagem stereo e mono. |
| Electronic Mix Top End Control | Mix Bus | Controlar top agressivo. | Bell dinamico 7 kHz, range -1.5 dB, Q 1.1; High Shelf -0.3 dB em 14 kHz. | Medium | Nao escurecer drop. |

### Master

| Preset candidato | Processamento | Intenção musical | Bandas sugeridas | Risco | Segurança |
|---|---|---|---|---|---|
| Electronic Master Air Guard | Mix Bus | Polimento de master com topo controlado. | Low Cut 22 Hz, 12 dB/oct; Bell dinamico 6.5 kHz, range -1 dB; Side High Shelf +0.4 dB em 12 kHz. | High | Master: max +0.5 dB no Side. |

---

## Presets prioritarios para implementar primeiro

Priorizar candidatos com maior utilidade e menor risco de exagero:

1. Forro Vocal Front Clarity
2. Forro Accordion Nasal Tame
3. Forro Mix Dance Cleanup
4. Trap Lead Vocal Shine Control
5. Trap 808 Tight Mono Focus
6. Trap Hi-Hat Silk Tame
7. Hip-Hop Rap Vocal Forward
8. Hip-Hop Beat Bus Low-Mid Clean
9. Hip-Hop Kick Knock Control
10. Funk BR Vocal Bite Control
11. Funk BR Bass Mono Lock
12. Rock Electric Guitar Fizz Tame
13. Rock Drum Bus Cymbal Guard
14. Sertanejo Vocal Silk Air
15. Sertanejo Acoustic Guitar Pick Tame
16. Pop Vocal Gloss Control
17. Pop Backing Vocal Wide Halo
18. Gospel Lead Vocal Lift Safe
19. Gospel Keys Low-Mid Cleanup
20. Electronic Bass Tight Low
21. Electronic Pad Width Cleanup
22. Electronic Mix Top End Control

## Presets arriscados que exigem validacao auditiva

- Trap 808 Tight Mono Focus: pode reduzir largura percebida ou mudar peso do 808.
- Trap 808 Note Tamer: depende muito do threshold e da tonalidade da linha.
- Trap Mix Low Focus: low end no mix bus pode afetar impacto do beat inteiro.
- Funk BR 808 Street Control: sub forte exige validacao em fones, monitores e PA.
- Funk BR Bass Mono Lock: limpeza Side pode estreitar beats ja prontos.
- Funk BR Mix Low Guard: low control no bus pode tirar energia de pista.
- Hip-Hop Bass Note Leveler: notas variam muito por sample/baixo.
- Rock Snare Ring Tamer: frequencia de ring muda por kit.
- Pop Mix Gentle Width: precisa mono check.
- Gospel Mix Choir Space: pode mexer demais em arranjos densos.
- Electronic Bass Tight Low: grave e Side em musica eletronica exigem cuidado.
- Electronic Mix Side Low Clean: altera imagem stereo.
- Electronic Master Air Guard: qualquer master preset precisa validacao auditiva humana.

## Nomes finais recomendados

Manter estes nomes como candidatos de primeira leva por clareza e baixa ambiguidade:

- Forro Vocal Front Clarity
- Forro Accordion Nasal Tame
- Forro Mix Dance Cleanup
- Trap Lead Vocal Shine Control
- Trap 808 Tight Mono Focus
- Trap Hi-Hat Silk Tame
- Hip-Hop Rap Vocal Forward
- Hip-Hop Beat Bus Low-Mid Clean
- Funk BR Bass Mono Lock
- Rock Electric Guitar Fizz Tame
- Rock Drum Bus Cymbal Guard
- Sertanejo Vocal Silk Air
- Pop Vocal Gloss Control
- Gospel Lead Vocal Lift Safe
- Electronic Bass Tight Low
- Electronic Mix Side Low Clean

## Como mapear para PresetManager na proxima fase

- Cada candidato deve virar um `makePreset()` somente na fase de implementacao, nunca nesta fase documental.
- Usar `category` como o genero: `Forro`, `Trap`, `Hip-Hop`, `Funk BR`, `Rock`, `Sertanejo`, `Pop`, `Gospel`, `Electronic`.
- Usar `subcategory` como a fonte/instrumento: por exemplo `Vocal`, `808`, `Mix Bus`, `Piano / Keys`.
- Usar `tags` com genero, fonte e funcao: exemplo `{ "trap", "808", "low control" }`.
- Preservar o reset completo de bandas antes de aplicar parametros especificos.
- Manter bandas nao usadas desativadas.
- Converter as bandas sugeridas para parametros APVTS existentes: `bandNType`, `bandNFrequency`, `bandNGain`, `bandNQ`, `bandNSlope`, `bandNDynamic*` e `bandNChannelMode`.
- Nao criar novos IDs, parametros, DSP ou UI para acomodar estes presets.
- Evitar adicionar todos os 108 de uma vez se a revisao musical ainda nao foi feita; implementar primeiro a lista prioritaria e validar em material real.
- Se um nome ja existir na biblioteca atual, decidir na fase seguinte entre substituir, manter o existente ou adicionar sufixo claro. Nao duplicar nomes no vetor final.

## Confirmacao de escopo

- Documento criado apenas para planejamento.
- Nenhum preset foi implementado.
- Nenhum arquivo de codigo foi alterado.
- APVTS, DSP, analyzer FFT, UI, CMake, PDFs e `Livros/` permanecem fora de escopo.
