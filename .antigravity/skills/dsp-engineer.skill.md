# Skill: Engenharia de DSP (DSP Engineer)

Esta skill é voltada para a criação, modelagem, otimização e validação de algoritmos de processamento de áudio digital (DSP) na suíte de efeitos **ASTRA**.

---

## 1. Responsabilidades Principais
- **Fundamentação Matemática**: Consultar os resumos teóricos do repositório em `docs/dsp-notes/` antes de codificar qualquer bloco de áudio.
- **Consulta Literária**: Utilizar os livros de referência presentes na pasta `Livros/` (sobre processamento de sinais, filtros e dinâmica) sempre que precisar expandir ou validar formulações.
- **Qualidade Acústica**: Priorizar a transparência, resposta ao impulso correta, ausência de distorções não intencionais e consistência da fase.
- **Segurança Numérica**: Evitar estouros de CPU, travamentos, realimentação destrutiva e ruídos digitais.
- **Separação Arquitetural**: Manter o código matemático isolado de qualquer elemento de UI ou manipulação de widgets.

---

## 2. Regras Operacionais
- **Sem DSP Sem Plano**: Nunca escreva blocos de processamento real ou novos filtros sem uma especificação matemática no plano de implementação da fase correspondente.
- **Mitigação de Zipper Noise**: Sempre utilize suavizadores de parâmetros (`juce::LinearSmoothedValue` ou equivalentes baseados em tempo constante) para parâmetros continuamente alterados pelo usuário (ganhos, frequências de corte, Q).
- **Proteção Numérica contra Artefatos**:
  - Implementar detectores de NaNs e infinitos nos buffers de saída.
  - Prevenir comportamento "denormal" utilizando a classe de controle nativa de hardware do JUCE (`juce::ScopedNoDenormals`) em todas as threads de processamento em tempo real.
  - Limpar apropriadamente os buffers e atrasos internos (`history buffers`, `feedback registers`) em `prepareToPlay` e sempre que a taxa de amostragem mudar.
- **Multi-Sample Rate Compliance**: Certificar-se de que os cálculos de coeficientes de filtros dependam estritamente da taxa de amostragem dinâmica informada (`sampleRate`), testando a compatibilidade desde 44.1 kHz até 192 kHz.
- **Bypass Seguro**: Garantir que a ativação ou desativação de bypass não introduza estalos no sinal de saída (clique acústico) e mantenha a fase do sinal original.
- **Progresso Incremental**: Não implemente recursos complexos (ex: oversampling de fase linear, equalização dinâmica) enquanto o esqueleto básico do motor IIR principal não estiver compilável e com testes de estabilidade básica aprovados.
