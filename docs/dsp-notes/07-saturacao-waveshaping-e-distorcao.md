# Saturação, Waveshaping e Distorção

Diferente de filtros (EQ) e do ganho linear, que só mudam o volume e fase sem injetar frequências novas, processos de distorção funcionam ao mudar ativamente a geometria microscópica da forma da onda que os atravessa, forçando a criação de novos harmônicos musicais (Distorção Não-Linear).

## Funções de Waveshaping (Moldagem de Onda)
A base matemática de grande parte das saturações digitais no **Plugin 03_Saturation** são equações "Shapes". Mapeia-se cada amostra de entrada X numa amostra de saída Y através de uma equação matemática curva: `Y = f(X)`.
Isso simula o que amplificadores, válvulas, transformadores ou fita magnética fariam num ambiente saturado em picos.

* Se X é pequeno (volume baixo), a fórmula se comporta linearmente e passa ilesa (`Y ≈ X`).
* Conforme o X se aproxima dos polos extremos (volumes altíssimos e drives fortes), a fórmula o esmaga não linearmente ("curvando" ou espremendo a extremidade da onda senoidal de áudio original). 

## Tipos de Clipping
* **Soft Clipping (Ceifamento Suave)**: O limite onde o esmagamento arredonda os picos progressivamente. É a sonoridade que remete à saturação valvulada e calor analógico (Fitas e Vacuum Tubes). Tipicamente feito usando a função Tangente Hiperbólica `tanh()`. A onda ganha calor sem quebrar.
* **Hard Clipping (Ceifamento Duro)**: Ao exceder um patamar 1.0, todos os valores são brutalmente cortados num teto seco. Cria pontas agudas quadradas que induzem harmônicos inarmônicos e pares potentes. Soará como distorção digital estridente ou circuitos solid-state/pedais distorcidos levados ao limite máximo.

## A Geração de Harmônicos
O "esmagamento" da onda pelas equações acima acarreta um efeito físico em frequência: uma série de parciais e ondas extras que não existiam emergem no espectro.
* **Harmônicos Pares (Even Harmonics, 2ª, 4ª ordem, etc.)**: Gerados por equações de waveshaping Assimétricas. Produzem uma distorção musicalmente calorosa e amigável (Válvulas). 
* **Harmônicos Ímpares (Odd Harmonics, 3ª, 5ª ordem, etc.)**: Gerados por distorções Simétricas puras (ex: `tanh()`). Eles produzem um tipo de saturação agressiva, incisiva e perfurante (Fitas).

## O Fantasma Agressivo e as Necessidades do Oversampling
Conforme anotado no documento `02`, todo esse processo gerador "espreme" novas frequências altíssimas para fora da onda. Como o waveshaper é cego para as frequências (só age na amostra base), harmônicos criados na faixa de altas frequências podem ultrapassar Nyquist sem parar.
Sem Over-sampling drástico de 4x ou 8x, o saturador digital fatalmente apresentará uma textura irritante (brittle) cheia de ecos agudos fantasmas. Saturação profissional exige processamento em altas resoluções.
