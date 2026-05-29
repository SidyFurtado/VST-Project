# AUREQ Preset Engineering Reference

Referência interna para orientar a criação futura de factory presets do AUREQ em navegação:

```text
Gênero Musical >
    Instrumento / Fonte >
        Presets específicos
```

Este documento sintetiza princípios de equalização, dinâmica, filtros IIR, slopes, Dynamic EQ e Channel Mode a partir da documentação técnica interna do projeto e das referências em `Livros/`, em palavras próprias. Ele não cria presets, não altera APVTS, não altera DSP e não muda a UI.

---

## 1. Princípios gerais de EQ musical

### Filosofia de preset

- Um preset de fábrica deve ser ponto de partida, não decisão final de mix.
- Prefira movimentos conservadores, especialmente em mix bus e master.
- Corte problemas antes de adicionar brilho, peso ou presença.
- Em fontes isoladas, boosts de `+1.0` a `+3.0 dB` tendem a ser suficientes.
- Em mix bus/master, boosts e cortes devem ficar normalmente entre `0.5` e `1.5 dB`; acima disso só quando o nome do preset indicar correção forte.
- A combinação de várias bandas pode somar energia. Um preset com muitos boosts pequenos pode soar mais agressivo do que um único boost claro.
- Sempre preservar headroom: presets não devem induzir clipping interno ou sensação de loudness artificial exagerada.

### Low cut / High cut

- **Low cut (High-pass)** remove rumble, vazamento de palco, plosivas e graves sem função musical.
- Use low cut com cuidado em fontes que carregam fundamental grave: kick, 808, baixo, piano grave, drum bus, mix bus e master.
- **High cut (Low-pass)** remove hiss, aspereza alta, sujeira de pratos, ruído de synth ou excesso de ar.
- Em master, high cut deve ser raro e suave; se usado, tende a ser uma correção de top end agressivo, não uma regra geral.

### Bell boosts/cuts

- Bell é o filtro principal para shaping musical e correção localizada.
- Use **boost largo** para corpo, presença, clareza ou ataque.
- Use **cut moderado** para mud, boxiness, nasalidade e harshness.
- Use **cut estreito** apenas quando a região problemática é ressonante e bem localizada.

### Shelves

- **Low shelf** serve para peso, warmth ou controle geral de grave sem eliminar a região.
- **High shelf** serve para air, brilho, abertura e polimento.
- Shelves são mais musicais quando o Q é baixo/médio e o ganho é pequeno.
- Em master, high shelf no Side pode abrir a imagem estéreo, mas deve ser muito sutil.

### Notch

- Notch é ferramenta cirúrgica para ressonâncias, apitos, ringing, room tone e frequências fixas muito ofensivas.
- Evite notch em presets genéricos de gênero, porque ele pode remover caráter musical quando aplicado em material diferente.
- Se houver preset de notch, o nome deve deixar claro que é corretivo.

### Band pass

- Band pass é mais útil para efeito, foco de comunicação, telefone/radio, isolamento temporário ou análise.
- Evite band pass em presets básicos de mix/master, pois ele altera demais a fonte.

### Q largo vs Q estreito

| Uso | Q recomendado | Observação |
|---|---:|---|
| Shaping musical | `0.5` a `1.2` | Natural, menos fase audível, bom para presets gerais. |
| Correção moderada | `1.2` a `2.0` | Bom para mud, boxiness, harshness e sibilância ampla. |
| Cirúrgico/notch | `3.0+` | Use só em problemas claros e com cortes pequenos/moderados. |

### Slopes seguros

| Slope | Uso recomendado |
|---|---|
| `12 dB/oct` | Limpeza leve, fontes delicadas, master/mix bus quando a intenção é musical. |
| `24 dB/oct` | Padrão seguro para low cut/high cut em vocal, instrumentos, drums e cleanup. |
| `48 dB/oct` | Limpeza forte de rumble, side low cleanup, efeitos ou isolamento; evitar como padrão em master. |

### Gain staging

- Presets devem evitar somar muitos boosts na mesma região.
- Quando um preset adiciona grave e brilho, considerar um corte leve no low-mid para manter headroom perceptivo.
- Presets de master não devem funcionar como limiter, maximizer ou loudness enhancer.
- Evite boosts fortes abaixo de `80 Hz` e acima de `10 kHz` em presets genéricos, porque esses extremos mudam muito entre sistemas.

### Quando usar Dynamic EQ

Use Dynamic EQ quando o problema não é constante:

- Sibilância que aparece apenas em algumas palavras.
- Harshness em vocal/guitarra quando o cantor ou instrumento fica mais forte.
- Low-mid que infla em certas notas.
- Grave de 808/baixo que varia demais entre notas.
- Drum bus ou mix bus que fica agressivo em hits específicos.

Regras:

- `dynamicRange` negativo = controle/atenuação quando o sinal cruza o threshold.
- `dynamicRange` positivo = realce responsivo intencional; usar raramente e com valores pequenos.
- Para de-esser, de-harsh e low-mid control, usar range negativo.
- Evitar Dynamic EQ em filtros Low Cut, High Cut, Notch e Band Pass; no AUREQ o uso musical principal é Bell, Low Shelf e High Shelf.

### Quando usar Mid/Side

- **Mid**: foco central, vocal principal, kick, snare, bass mono, low-mid central e presença do lead.
- **Side**: ar, largura, ambiência, pratos, pads e limpeza lateral.
- Evite grave largo no Side. Abaixo de `120 Hz`, Side deve geralmente ser limpo ou preservado de forma muito sutil.
- Em master, boosts no Side devem ser menores que em fontes individuais.

### Quando usar Left/Right

- Left/Right é corretivo para assimetria, microfone desigual, imagem torta ou fonte estéreo com problema em um lado.
- Não deve ser base de presets genéricos, porque pode deslocar a imagem sem o usuário perceber.
- Se usado, o nome do preset deve indicar `Left` ou `Right`.

---

## 2. Regiões de frequência por problema

| Região / intenção | Faixa típica | Ação comum | Notas de cuidado |
|---|---:|---|---|
| Sub rumble | `20-40 Hz` | Low cut | Em 808/kick/master, cortar só se houver ruído real. |
| Peso sub | `40-80 Hz` | Boost/cut largo | Fundamental de kick/808; risco de headroom. |
| Punch | `60-120 Hz` | Bell/low shelf leve | Kick e baixo; evitar conflito com 808. |
| Body | `120-250 Hz` | Boost/cut largo | Corpo de vocal, snare, guitarra, piano; excesso vira embolo. |
| Warmth | `150-350 Hz` | Low shelf/bell leve | Musical em vocal/instrumentos; perigoso em mix densa. |
| Mud | `200-500 Hz` | Cut largo/moderado | Região clássica de lama em vocal, guitar, keys, mix bus. |
| Boxiness | `350-800 Hz` | Cut médio | Caixa, sala pequena, vocal fechado, snare oca. |
| Nasal | `700 Hz-1.5 kHz` | Cut moderado | Voz, guitarra e sanfona podem ficar irritantes. |
| Presence | `2-5 kHz` | Boost/cut musical | Clareza e inteligibilidade; excesso vira harshness. |
| Harshness | `2.5-5.5 kHz` | Cut ou Dynamic EQ negativo | Guitarras, vocal, snare, synth lead. |
| Sibilance | `5-9 kHz` | Dynamic EQ negativo | Voz e hi-hat; faixa varia por cantor/microfone. |
| Clarity | `3-7 kHz` | Boost leve ou corte de masking | Ajuda ataque e definição; testar com contexto. |
| Air | `10-16 kHz` | High shelf leve | Brilho premium; pode revelar ruído, hiss e aspereza. |
| Hiss/top noise | `12-20 kHz` | High cut/shelf negativo | Cuidado para não escurecer demais. |

---

## 3. Guia por fonte / instrumento

### Vocal

- Cortes úteis: low cut `70-100 Hz`; mud `200-450 Hz`; nasal `800 Hz-1.3 kHz`; harshness `2.5-4.5 kHz`; sibilance `5-8 kHz`.
- Boosts úteis: body `120-220 Hz`; presence `2-4 kHz`; air `10-14 kHz`.
- Regiões perigosas: `250-500 Hz` fechado, `3-5 kHz` agressivo, `6-8 kHz` sibilante.
- Q: largo `0.7-1.2` para tom; `1.5-2.5` para problemas localizados.
- Dynamic EQ: usar negativo em sibilância e harshness.
- Mid/Side: vocal lead geralmente Mid/Stereo; evitar Side em vocal mono.
- Cuidado: não transformar todo vocal em bright; preservar corpo.

### Backing Vocal

- Cortes úteis: low cut `100-160 Hz`; low-mid `250-500 Hz`; presença excessiva `2-4 kHz`.
- Boosts úteis: air leve `9-13 kHz` se precisar abrir sem competir.
- Regiões perigosas: presence demais rouba foco do lead.
- Q: largo para tuck; cortes médios para masking.
- Dynamic EQ: sibilância coletiva em `5-8 kHz`.
- Mid/Side: em backing estéreo, Side air pode abrir; Mid cut leve pode abrir espaço para lead.
- Cuidado: backing deve apoiar, não virar vocal principal.

### Rap Vocal

- Cortes úteis: low cut `80-120 Hz`; mud `180-400 Hz`; harshness `2.5-5 kHz`; sibilance `5.5-8.5 kHz`.
- Boosts úteis: intelligibility `1.8-3.5 kHz`; air `9-12 kHz`.
- Regiões perigosas: `2-4 kHz` pode ficar pontudo em flows rápidos.
- Q: médio para presença, dinâmico para controle.
- Dynamic EQ: essencial para de-esser e de-harsh.
- Mid/Side: foco normalmente no Mid.
- Cuidado: clareza sem magreza; manter peito e proximidade.

### Kick

- Cortes úteis: rumble abaixo de `25-35 Hz`; mud `200-450 Hz`; click áspero `3-6 kHz`.
- Boosts úteis: sub/body `45-80 Hz`; punch `80-120 Hz`; attack `2-5 kHz`.
- Regiões perigosas: boost sub forte em mix/master; low-mid sem controle.
- Q: largo para peso, médio para click, estreito só para ressonância.
- Dynamic EQ: controlar boom em notas/hits inconsistentes.
- Mid/Side: kick deve ficar Mid/Stereo central; evitar Side grave.
- Cuidado: não somar sub com 808 sem estratégia.

### Snare

- Cortes úteis: rumble abaixo de `80-120 Hz`; boxiness `400-800 Hz`; harsh ring `1-3 kHz`.
- Boosts úteis: body `150-250 Hz`; crack `2-5 kHz`; air/snap `8-12 kHz`.
- Regiões perigosas: `2-5 kHz` cansa rápido.
- Q: médio para ring, largo para corpo.
- Dynamic EQ: tame em crack/harsh quando hits fortes pulam.
- Mid/Side: Mid para impacto; Side só em snare room/ambience.
- Cuidado: corpo sem cartonado.

### Drum Bus

- Cortes úteis: rumble `20-35 Hz`; mud `200-450 Hz`; harsh cymbals `3-7 kHz`.
- Boosts úteis: punch `60-110 Hz`; snap `2-4 kHz`; air `10-14 kHz`.
- Regiões perigosas: boosts amplos em graves e highs podem destruir headroom.
- Q: largo e sutil.
- Dynamic EQ: controle de cymbal harshness e low-mid em hits cheios.
- Mid/Side: Mid para punch, Side para brilho/ambiente com cautela.
- Cuidado: drum bus precisa preservar transientes.

### Hi-Hat

- Cortes úteis: low cut `250-600 Hz`; harshness `4-8 kHz`; hiss `12-18 kHz`.
- Boosts úteis: air `9-14 kHz` com pouco ganho.
- Regiões perigosas: `6-10 kHz` pode furar o ouvido.
- Q: médio para harshness, largo para high shelf.
- Dynamic EQ: negativo para picos agudos.
- Mid/Side: hats estéreo podem receber Side control/air.
- Cuidado: brilho não é volume.

### Percussion

- Cortes úteis: low cut conforme fonte `80-300 Hz`; boxiness `400-900 Hz`; harshness `3-7 kHz`.
- Boosts úteis: body `150-300 Hz`; attack `2-5 kHz`; air `9-13 kHz`.
- Regiões perigosas: transientes duros no presence.
- Q: médio para ressonâncias; largo para tone.
- Dynamic EQ: controlar hits inconsistentes.
- Mid/Side: Side para ambiência; Mid para elementos rítmicos centrais.
- Cuidado: não padronizar todas as percussões; cada uma tem fundamental diferente.

### 808

- Cortes úteis: sub rumble abaixo de `20-30 Hz`; mud/harmônicos baixos `180-350 Hz`; harsh harmonic `1-3 kHz` se distorcida.
- Boosts úteis: fundamental `35-70 Hz`; audibilidade `700 Hz-2 kHz` se precisa aparecer em caixas pequenas.
- Regiões perigosas: boosts sub fortes e Side grave.
- Q: largo para fundamental; médio para harmônicos.
- Dynamic EQ: negativo para notas que explodem; positivo só para harmonics controlados.
- Mid/Side: manter sub no Mid; limpar Side abaixo de `100-150 Hz`.
- Cuidado: preset de 808 deve respeitar tom da música.

### Bass

- Cortes úteis: rumble `20-35 Hz`; mud `180-400 Hz`; clank agressivo `1.5-4 kHz`.
- Boosts úteis: weight `60-120 Hz`; warmth `120-250 Hz`; definition `700 Hz-1.5 kHz`.
- Regiões perigosas: low-mid embolado.
- Q: largo para corpo; médio para definição.
- Dynamic EQ: controlar notas graves desiguais.
- Mid/Side: baixo principal no Mid; Side cleanup quando fonte estéreo tem grave aberto.
- Cuidado: não brigar com kick.

### Acoustic Guitar

- Cortes úteis: low cut `70-120 Hz`; boom `120-250 Hz`; mud `250-500 Hz`; harsh pick `2.5-5 kHz`.
- Boosts úteis: body `150-250 Hz`; clarity `3-6 kHz`; air `10-14 kHz`.
- Regiões perigosas: `200-500 Hz` em arranjos cheios; `3-5 kHz` palhetada dura.
- Q: largo para tone; médio para ressonâncias de corpo.
- Dynamic EQ: controlar pick harshness.
- Mid/Side: em par estéreo, Side air pode abrir; limpar low Side.
- Cuidado: solo pede corpo, base pede espaço.

### Electric Guitar

- Cortes úteis: low cut `70-120 Hz`; mud `200-450 Hz`; fizz `5-10 kHz`; harshness `2.5-5 kHz`.
- Boosts úteis: body `120-250 Hz`; bite `1.5-3.5 kHz`; air moderado se limpa.
- Regiões perigosas: `3-5 kHz` e fizz alto.
- Q: médio para harsh/fizz; largo para corpo.
- Dynamic EQ: de-harsh em partes fortes.
- Mid/Side: guitarras duplas podem usar Side shaping; centro deve abrir espaço para vocal.
- Cuidado: brilho de guitarra distorcida raramente precisa de muito shelf alto.

### Piano/Keys

- Cortes úteis: rumble `30-60 Hz`; mud `180-450 Hz`; boxiness `400-800 Hz`; harsh attack `2-5 kHz`.
- Boosts úteis: body `120-250 Hz`; presence `2-4 kHz`; air `8-12 kHz`.
- Regiões perigosas: conflito com vocal e baixo.
- Q: largo em piano; médio em keys/synths ressonantes.
- Dynamic EQ: notas graves que pulam ou ataque duro.
- Mid/Side: Side air para piano estéreo; Mid cleanup para vocal.
- Cuidado: preservar naturalidade do instrumento.

### Synth/Lead

- Cortes úteis: low cut conforme papel `80-250 Hz`; harshness `2.5-6 kHz`; fizz `8-14 kHz`.
- Boosts úteis: presence `1.5-4 kHz`; air `8-12 kHz`; body `150-350 Hz` se fino.
- Regiões perigosas: leads podem mascarar vocal.
- Q: médio para ressonâncias; largo para shaping.
- Dynamic EQ: controlar picos de filtro/resonance.
- Mid/Side: lead central no Mid; efeitos estéreo no Side.
- Cuidado: preset deve deixar espaço para voz quando gênero for vocal-driven.

### Pad

- Cortes úteis: low cut `100-250 Hz`; mud `250-600 Hz`; harshness `2-5 kHz`; hiss `10-16 kHz`.
- Boosts úteis: width/air `8-14 kHz`; warmth `200-350 Hz` em arranjos vazios.
- Regiões perigosas: low-mid acumulado e Side grave.
- Q: largo.
- Dynamic EQ: raramente, exceto para ressonância de filtro.
- Mid/Side: Side air funciona bem; Mid cut pode abrir espaço para vocal.
- Cuidado: pad deve preencher sem encobrir ritmo/lead.

### Mix Bus

- Cortes úteis: rumble `20-30 Hz`; mud `200-350 Hz`; harshness `2.5-5 kHz`; sibilance coletiva `6-9 kHz`.
- Boosts úteis: low shelf muito leve `60-100 Hz`; high shelf `10-16 kHz`; presence sutil `2-4 kHz`.
- Regiões perigosas: qualquer boost maior que `1.5 dB`.
- Q: largo, natural.
- Dynamic EQ: negativo em low-mid e harshness quando a mix abre demais.
- Mid/Side: Mid foco; Side air/cleanup com ganho pequeno; limpar Side abaixo de `120 Hz`.
- Cuidado: mix bus não deve consertar problema de faixa individual.

### Master

- Cortes úteis: sub rumble `20-25 Hz`; low-mid `180-300 Hz`; harshness `2.5-5 kHz`; top excess `12-18 kHz`.
- Boosts úteis: air `12-16 kHz`, presence muito sutil, low shelf mínimo.
- Regiões perigosas: grave, brilho e Side.
- Q: sempre largo, salvo ressonância muito clara.
- Dynamic EQ: negativo e sutil para harshness/low-mid; positivo quase nunca.
- Mid/Side: Mid para foco, Side para ar e limpeza lateral; nada de grave largo.
- Cuidado: presets de master devem ser os mais conservadores da biblioteca.

---

## 4. Guia por gênero

### Forro

- Prioridades sonoras: vocal claro, sanfona/acordeon presente, zabumba/baixo controlados, percussão viva.
- Fontes importantes: Vocal, Backing Vocal, Percussion, Kick/Zabumba, Bass, Acoustic Guitar, Accordion como instrumento futuro.
- Problemas comuns: embolo em `200-500 Hz`, nasalidade de sanfona/vocal, agudos duros em percussão.
- Sugestões: Vocal clarity com low cut moderado e presença `2-3 kHz`; Percussion brightness com controle de harshness; Mix cleanup com corte leve de mud.
- Mix/master: preservar calor e dança; evitar master bright demais.

### Trap

- Prioridades sonoras: 808 dominante e estável, vocal moderno/brilhante, hats detalhados, espaço estéreo limpo.
- Fontes importantes: 808, Rap Vocal, Hi-Hat, Synth/Lead, Pad, Mix Bus.
- Problemas comuns: sub excessivo, hats sibilantes, vocal fino, Side grave.
- Sugestões: 808 control com low Side cleanup; Trap Vocal Shine com de-esser dinâmico; Hi-Hat Softener para `6-10 kHz`.
- Mix/master: sub mono, master com air sutil e controle dinâmico de low-mid.

### Hip-Hop

- Prioridades sonoras: voz na frente, kick/bass com groove, sample/keys limpos, transiente sem harsh.
- Fontes importantes: Rap Vocal, Kick, Bass/808, Drum Bus, Piano/Keys, Synth/Lead.
- Problemas comuns: vocal mascarado por sample, low-mid denso, kick sem definição.
- Sugestões: Rap Vocal Cut Through; Kick Punch; Bass Tight Low; Dynamic Low-Mid Control em sample ou bus.
- Mix/master: preservar punch, não superabrir o Side.

### Funk BR

- Prioridades sonoras: low end forte, kick/808 articulado, vocal/percussão agressivos mas controlados.
- Fontes importantes: 808, Kick, Percussion, Rap Vocal, Drum Bus.
- Problemas comuns: sub descontrolado, médios ásperos, percussão acumulada.
- Sugestões: Funk BR Low Control; Dynamic 808 notes; Percussion cleanup; Vocal de-harsh.
- Mix/master: low end central, 48 dB/oct apenas para rumble ou Side low cleanup.

### Rock

- Prioridades sonoras: guitarras com corpo e bite, vocal inteligível, snare/kick presentes, mix densa mas clara.
- Fontes importantes: Electric Guitar, Vocal, Snare, Kick, Drum Bus, Bass.
- Problemas comuns: `200-500 Hz` embolado, `3-5 kHz` agressivo, pratos duros.
- Sugestões: Rock Guitar Edge com cuidado em `2-4 kHz`; Vocal De-Harsh; Drum Bus Punch; Bass definition.
- Mix/master: evitar high shelf forte; controlar harshness dinâmico.

### Sertanejo

- Prioridades sonoras: vocal polido, violões claros, baixo/kick suaves, brilho comercial.
- Fontes importantes: Vocal, Backing Vocal, Acoustic Guitar, Bass, Piano/Keys, Master.
- Problemas comuns: violão embolado, vocal sibilante, master brilhante demais.
- Sugestões: Sertanejo Vocal Air; Acoustic Guitar Clarity; Backing Vocal Tuck.
- Mix/master: high shelf sutil; de-esser antes de adicionar air.

### Pop

- Prioridades sonoras: vocal grande, low end controlado, brilho limpo, largura moderna.
- Fontes importantes: Vocal, Backing Vocal, Kick, Bass/808, Synth/Lead, Pad, Mix Bus, Master.
- Problemas comuns: excesso de air/sibilância, graves concorrendo com vocal, Side cheio.
- Sugestões: Pop Airy Vocal com dynamic sibilance; Side Air leve; Mix Bus Gentle Smile.
- Mix/master: brilho premium, mas com `dynamicRange` negativo em `5-8 kHz` se necessário.

### Gospel

- Prioridades sonoras: vocal lead claro, backing/choir amplo, piano/keys cheios, mix emocional sem embolo.
- Fontes importantes: Vocal, Backing Vocal, Piano/Keys, Pad, Drum Bus, Bass, Mix Bus.
- Problemas comuns: low-mid acumulado por keys/pads/vozes, sibilância em muitas vozes, master cansativo.
- Sugestões: Gospel Vocal Lift; Backing Vocal Tuck; Piano Presence; Pad Side Air com low Side cleanup.
- Mix/master: preservar corpo e dinâmica; master muito brilhante pode cansar.

### Electronic

- Prioridades sonoras: sub limpo, top end controlado, synths definidos, largura grande sem fase problemática.
- Fontes importantes: Kick, Bass/808, Synth/Lead, Pad, Hi-Hat, Drum Bus, Master.
- Problemas comuns: sub/headroom, hats agressivos, Side grave, synth resonance.
- Sugestões: Electronic Top End Control; Bass Side Cleanup; Synth Lead De-Harsh; Pad Width Cleanup.
- Mix/master: grave central, Side air cuidadoso, high cut/shelf negativo se o top estiver áspero.

---

## 5. Regras para criação de presets AUREQ

### Faixa segura de ganhos

| Tipo de preset | Ganho recomendado |
|---|---:|
| Master | `-1.5` a `+1.5 dB` por banda, preferir menos. |
| Mix Bus | `-2.0` a `+2.0 dB`, com Q largo. |
| Vocal/instrumentos | `-3.0` a `+3.0 dB`; boosts fortes só se o nome prometer efeito. |
| Cleanup/corretivo | cortes até `-4.0 dB` em casos claros; notch pode ser mais forte se explícito. |
| Dynamic EQ negativo | `-2.0` a `-6.0 dB` conforme problema. |
| Dynamic EQ positivo | `+0.5` a `+2.0 dB`, raro e intencional. |

### Limites recomendados por tipo

- **Vocal presets**: low cut `70-120 Hz`; presença até `+2.5 dB`; air até `+2.5 dB`; de-esser dinâmico preferido a cut estático grande.
- **Drum presets**: punch com ganho moderado; controlar harsh cymbals dinamicamente.
- **Bass/808 presets**: evitar boost sub genérico grande; priorizar controle, mono focus e definição.
- **Instrument presets**: cortes de mud e boosts de presença com Q largo/médio.
- **Mix/Master presets**: movimentos largos, pequenos e com nomes honestos.
- **Utility/Cleanup presets**: podem ser mais corretivos, mas devem indicar função claramente.

### Dynamic Range

- `dynamicRange < 0`: controle de sibilância, harshness, boom, low-mid, cymbal peaks e notas inconsistentes.
- `dynamicRange > 0`: realce responsivo de presença/air/ataque quando a intenção é trazer vida em trechos fortes; usar pouco.
- `dynamicRange = 0`: banda estática ou dinâmica desligada.

### Channel Mode

- **Stereo**: padrão para a maioria dos presets.
- **Mid**: vocal, kick, snare, bass, foco central, limpeza de low-mid central.
- **Side**: air, width, ambience, limpeza lateral e remoção de grave estéreo.
- **Left/Right**: evitar em factory presets genéricos; reservar para utilitários explícitos.

### Slopes

- `12 dB/oct`: musical, leve, bom para fontes delicadas e master.
- `24 dB/oct`: padrão de limpeza para vocal/instrumentos e low cut comum.
- `48 dB/oct`: rumble forte, side low cleanup, isolamento de efeito; não usar como default musical.

### Como evitar presets agressivos demais

- Não somar boost de presence e high shelf alto sem checar sibilância.
- Não usar high shelf grande em master.
- Não abrir Side enquanto o grave lateral está sujo.
- Não cortar low-mid demais em vocal, senão perde corpo.
- Não usar Q estreito para shaping genérico.
- Não usar Dynamic EQ positivo como maquiagem de loudness.

### Como nomear presets

Formato recomendado:

```text
[Fonte] [Ação Musical]
[Gênero] [Fonte] [Ação Musical]
[Utility] [Problema] [Ação]
```

Exemplos de nomes futuros:

- `Forro Vocal Clarity`
- `Trap 808 Tight Mono`
- `Rock Guitar De-Harsh`
- `Gospel Backing Vocal Tuck`
- `Electronic Side Low Cleanup`
- `Master Gentle Air`
- `Utility Sub Rumble Clean`

Evitar nomes que prometam masterização final, loudness ou correção universal.

---

## 6. Proposta de estrutura final da biblioteca

```text
Forro
    Vocal
        Forro Vocal Clarity
        Forro Vocal De-Harsh
        Forro Vocal Air
    Backing Vocal
        Forro Backing Vocal Tuck
    Percussion
        Forro Percussion Brightness
        Forro Percussion Soft Top
    Bass
        Forro Bass Warm Control
    Mix Bus
        Forro Mix Cleanup
        Forro Mix Gentle Air

Trap
    Rap Vocal
        Trap Vocal Shine
        Trap Vocal De-Esser
        Trap Vocal Cut Through
    808
        Trap 808 Control
        Trap 808 Tight Mono
        Trap 808 Small Speaker
    Hi-Hat
        Trap Hi-Hat Softener
    Synth/Lead
        Trap Lead De-Harsh
    Master
        Trap Master Sub Control

Hip-Hop
    Rap Vocal
        Hip-Hop Vocal Focus
        Hip-Hop Vocal Warm Presence
    Kick
        Hip-Hop Kick Punch
    Bass
        Hip-Hop Bass Definition
    Drum Bus
        Hip-Hop Drum Bus Punch
    Piano/Keys
        Hip-Hop Keys Cleanup
    Mix Bus
        Hip-Hop Mix Low-Mid Control

Funk BR
    Vocal
        Funk BR Vocal Bite Control
    Kick
        Funk BR Kick Punch
    808
        Funk BR Low Control
        Funk BR 808 Note Tamer
    Percussion
        Funk BR Percussion Cut Through
    Master
        Funk BR Master Low Cleanup

Rock
    Vocal
        Rock Vocal De-Harsh
        Rock Vocal Presence
    Electric Guitar
        Rock Guitar Edge
        Rock Guitar Fizz Control
    Snare
        Rock Snare Crack
    Drum Bus
        Rock Drum Bus Glue
    Bass
        Rock Bass Definition
    Mix Bus
        Rock Mix Harshness Control

Sertanejo
    Vocal
        Sertanejo Vocal Air
        Sertanejo Vocal Polish
    Backing Vocal
        Sertanejo Backing Vocal Tuck
    Acoustic Guitar
        Sertanejo Acoustic Guitar Clarity
    Bass
        Sertanejo Bass Warm Body
    Master
        Sertanejo Master Gentle Shine

Pop
    Vocal
        Pop Airy Vocal
        Pop Vocal Dynamic De-Esser
        Pop Vocal Intimacy
    Backing Vocal
        Pop Backing Vocal Wide Air
    Kick
        Pop Kick Tight Punch
    Bass/808
        Pop Low End Control
    Synth/Lead
        Pop Lead Presence
    Mix Bus
        Pop Mix Gentle Smile
    Master
        Pop Master Air Control

Gospel
    Vocal
        Gospel Vocal Lift
        Gospel Vocal Warmth
    Backing Vocal
        Gospel Backing Vocal Tuck
        Gospel Choir Air
    Piano/Keys
        Gospel Piano Presence
        Gospel Keys Low-Mid Cleanup
    Pad
        Gospel Pad Side Air
    Mix Bus
        Gospel Mix Clarity

Electronic
    Kick
        Electronic Kick Sub Focus
    Bass/808
        Electronic Bass Tight Low
        Electronic Side Low Cleanup
    Synth/Lead
        Electronic Lead De-Harsh
    Pad
        Electronic Pad Width Cleanup
    Hi-Hat
        Electronic Top End Control
    Drum Bus
        Electronic Drum Bus Punch
    Master
        Electronic Master Top Tame

Utility / Cleanup
    Vocal
        Dynamic Sibilance 6k
        Dynamic De-Harsh 3k
    Low End
        Sub Rumble Clean
        Bass Side Cleanup
    Mix Bus / Master
        Dynamic Mix Bus Tame
        Stereo Width Cleanup
    General
        Mud Remover
        Boxiness Cut
        Harshness Control
```

---

## 7. Confirmações de escopo desta fase

- Este documento é pesquisa e orientação técnica.
- Nenhum preset foi criado.
- Nenhuma estrutura de browser foi implementada.
- Nenhum código, APVTS, DSP, analyzer FFT ou UI foi alterado por este guia.
- PDFs e arquivos em `Livros/` devem permanecer intactos.
