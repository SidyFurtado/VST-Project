# Regras de Boas Práticas de Código DSP (VST)

Com base nas pesquisas teóricas, aqui estão estipulados os mandamentos de programação de código de áudio para o Agente e toda a estrutura deste Monorepo. Qualquer desvio destas regras pode causar travamentos da DAW ou gerar áudio digital danificado.

### 1. Evitar Zipper Noise (Suavização de Parâmetros)
**O Problema**: Se o usuário arrastar rapidamente um Knob na UI de `Ganho` de -10dB para +10dB, ele não varrerá os dBs intermediários (será um salto descontínuo numérico do bloco anterior para o novo). O som vai "pipocar" estalando ("zipper noise").
**A Regra**: **Todos** os parâmetros que afetam volume ou filtros e podem ser arrastados devem obrigatoriamente possuir um "Smoother" (um pequeno filtro LPF 1st order iterativo de parâmetro). Suavize os "saltos" com interpolação linear durante a iteração pelo bloco!

### 2. Proteger Contra NaNs (Not a Number) e Infinitos
**O Problema**: Divisão por zero, ou valores de logaritmos que explodem e geram Infinity ou NaN, causam uma catástrofe que pode corromper toda a thread de áudio e desativar o plugin (Mute permanente em DAWs rígidas como Logic e Ableton).
**A Regra**: Toda equação instável de EQ ou filtro, toda divisão com denominador possivelmente igual a zero e todo cálculo exponencial instável deve ser protegido, inspecionado por limite (assert e checks). Mantenha as equações restritas!

### 3. Cuidar de Denormals (Números Subnormais)
**O Problema**: Quando reverbs ou filtros IIR estão no fim do decaimento e chegam em variações menores que `1e-38`, a FPU das CPUs Intel entra em loop infinito e penaliza processamento, destruindo a performance (Denormal slow down).
**A Regra**: Forçar ativação constante de *Flush to Zero* (FTZ) e *Denormals Are Zero* (DAZ) em cada thread de áudio, além de adicionar um dither de `1e-15` em loops IIR.

### 4. Isolamento Completo DSP/UI
**A Regra**: **Nunca aloque memória (`new`/`malloc`) ou realize travas em mutex (bloqueios lentos de Thread) dentro do `processBlock`**. Nunca acesse métodos gráficos da tela pelo loop de DSP. As threads precisam se comunicar sem usar "locks" pesados (Lock-free FIFOs ou atômicos).

### 5. Controle de Headroom e Clipping Oculto
**A Regra**: Não permita "clip" acidental internamente. Controle o ganho da função se gerar energia forte, e certifique-se que saturações possuam ganhos automáticos de redução para evitar saídas altíssimas estouradas nos conversores.

### 6. Independência de Sample Rate
**A Regra**: Se mudar o sample rate da DAW (44k vs 96k), o som do EQ e do Compressor devem responder exatamente igual no tempo e frequência! Re-calcule parâmetros de envelope/filtros toda vez que o Sample Rate mudar (usando os "callbacks" tipo `prepareToPlay`).

### 7. Validação Criteriosa
**A Regra**: Em todo release/fase, execute verificação técnica do *Bypass* em processamento neutro, confirmando e documentando decisões críticas tomadas contra aliasing e aliasing indesejado.
