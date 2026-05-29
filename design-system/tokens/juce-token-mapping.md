# JUCE Token Mapping Guide

Este documento orienta a futura equipe de desenvolvimento C++ sobre como traduzir os conceitos de **Design Tokens** definidos no sistema visual do **VST - Project** para código de interface real utilizando o framework **JUCE**. 

---

## 1. Mapeamento de Tipos de Tokens para JUCE

### A. Cores (`juce::Colour`)
Os tokens de cores hexadecimais (ex: `#0891B2` ou `#22D3EE`) serão mapeados para classes `juce::Colour`. 
*   **Declaração**: `juce::Colour (0xFF0891B2)` ou usando o construtor ARGB de bytes: `juce::Colour::fromRGB (8, 145, 178)`.
*   **Opacidade (Alpha)**: Estados e efeitos que alteram opacidade utilizarão `.withAlpha (float newAlpha)`.

### B. Constantes Numéricas (`float` e `int`)
*   **Espaçamento**: Espaçamentos como `spacingSmall` (8px), `spacingMedium` (16px) e `spacingLarge` (24px) devem ser definidos como constantes estáticas (`constexpr float` ou `constexpr int`) e consumidos no método `resized()` dos componentes para calcular as áreas e limites (`juce::Rectangle<int>`).
*   **Raios (Radius)**: Os tokens de raio `radiusSmall` (4px), `radiusMedium` (8px) e `radiusLarge` (12px) servirão de parâmetro em métodos de desenho gráfico como:
    ```cpp
    g.fillRoundedRectangle (getLocalBounds().toFloat(), ThemeTokens::radiusMedium);
    ```

### C. Tipografia (`juce::Font`)
As especificações de fontes (famílias, pesos e tamanhos) serão encapsuladas em objetos `juce::Font`.
*   Fontes técnicas e limpas (como `Inter` e `Roboto Mono`) devem ser empacotadas como recursos binários do plugin (usando a ferramenta Projucer ou CMake binary resources) e carregadas na inicialização:
    ```cpp
    static const juce::Font getLabelFont() {
        return juce::Font (juce::Typeface::createSystemTypefaceFor (BinaryData::InterRegular_ttf, BinaryData::InterRegular_ttfSize))
               .withHeight (ThemeTokens::fontLabelSmallSize);
    }
    ```

### D. Sombras e Glows em JUCE
O JUCE não suporta CSS box-shadows de forma nativa. Para simular esses efeitos, utilizaremos:
*   **Sombras de Painel / Knobs**:
    1.  Criação de caminhos (`juce::Path`) do controle ligeiramente menores do que o bounds real.
    2.  Aplicação da classe `juce::DropShadow` ou `juce::DropShadowEffect`.
    3.  Desenho de um gradiente linear ou radial suave simulando o sombreamento físico nas bordas do botão/knob.
*   **Glows Ativos**:
    *   Desenho de círculos concêntricos com opacidade decrescente (gradiente radial de cor para transparente) atrás do indicador ativo:
        ```cpp
        juce::Graphics::GradientType gradient (center, ThemeTokens::colorAccentPrimary, center, glowRadius, juce::Colours::transparentBlack, true);
        ```

---

## 2. Arquitetura de Código Sugerida

Abaixo está a estrutura de diretórios e arquivos conceitual sugerida para organizar o gerenciamento de temas no projeto (`shared/gui/theme/`):

```text
shared/gui/theme/
├── ThemeTokens.h             // Declarações constantes e nomes semânticos dos tokens
├── LightTheme.h              // Valores concretos das cores, raios e sombras do modo claro
├── DarkTheme.h               // Valores concretos das cores, raios e sombras do modo escuro
├── ThemeManager.h            // Singleton / Gerenciador global de temas e preferências
└── ThemeAwareComponent.h     // Classe base para componentes de UI que escutam mudanças de tema
```

### Detalhamento Conceitual das Classes

#### `ThemeTokens.h`
Define enums, structs ou identificadores semânticos de tokens.
```cpp
namespace ThemeTokens {
    enum class ColorId {
        colorBackgroundPrimary,
        colorBackgroundSecondary,
        colorPanelSurface,
        colorTextPrimary,
        colorTextSecondary,
        colorAccentPrimary,
        colorAccentSecondary,
        colorMeterSafe,
        colorMeterWarning,
        colorMeterClip
    };
    
    constexpr float spacingSmall = 8.0f;
    constexpr float spacingMedium = 16.0f;
    constexpr float spacingLarge = 24.0f;
    
    constexpr float radiusSmall = 4.0f;
    constexpr float radiusMedium = 8.0f;
    constexpr float radiusLarge = 12.0f;
}
```

#### `ThemeManager.h`
Gerencia a troca de tema ativa (Light vs Dark). Ele notifica todos os componentes registrados para que eles chamem `repaint()` quando o tema for alterado. Ele é o responsável por centralizar o mapeamento do `ColorId` ativo para a classe `juce::Colour` correspondente.
```cpp
class ThemeManager : public juce::ChangeBroadcaster {
public:
    enum class ThemeType {
        Light,
        Dark
    };

    static ThemeManager& getInstance();
    
    void setTheme (ThemeType newTheme) {
        currentTheme = newTheme;
        sendChangeMessage(); // Notifica a GUI para se redesenhar
    }

    ThemeType getTheme() const { return currentTheme; }
    
    juce::Colour getColor (ThemeTokens::ColorId id) const {
        if (currentTheme == ThemeType::Light) {
            return LightTheme::getColor (id);
        }
        return DarkTheme::getColor (id);
    }
    
private:
    ThemeType currentTheme = ThemeType::Dark; // Padrão
};
```

#### `ThemeAwareComponent.h`
Classe base utilitária da qual todos os nossos knobs, botões e painéis herdam. Ela se registra automaticamente como um `juce::ChangeListener` no `ThemeManager`.
```cpp
class ThemeAwareComponent : public juce::Component,
                            private juce::ChangeListener {
public:
    ThemeAwareComponent() {
        ThemeManager::getInstance().addChangeListener (this);
    }
    
    ~ThemeAwareComponent() override {
        ThemeManager::getInstance().removeChangeListener (this);
    }
    
    void changeListenerCallback (juce::ChangeBroadcaster* source) override {
        if (source == &ThemeManager::getInstance()) {
            themeChanged();
        }
    }
    
    virtual void themeChanged() {
        repaint(); // Força o redesenho do componente com as novas cores
    }
};
```

---

## 3. Persistência de Preferência e Estado do Plugin

Para garantir que a escolha de tema do usuário (Light ou Dark) seja preservada quando o projeto da DAW for fechado e reaberto:
1.  **Estado Global**: A propriedade `theme` (inteiro ou string) deve ser gravada na classe `juce::AudioProcessorValueTreeState` (APVTS) como um parâmetro não automatizável ou em uma seção de metadados dedicada dentro do ValueTree XML do plugin.
2.  **Salvamento do Estado (`getStateInformation`)**: Quando a DAW solicita o salvamento do estado do plugin, o ValueTree converte a preferência do `ThemeManager` ativa em XML/Binário.
3.  **Carregamento do Estado (`setStateInformation`)**: Ao carregar, o plugin lê o valor salvo e chama `ThemeManager::setTheme()`, atualizando a interface gráfica instantaneamente no modo escolhido.

---

## 4. Desacoplamento Estrito entre DSP e UI

Conforme as boas práticas do framework JUCE:
*   A classe de áudio principal (`AudioProcessor`) gerencia apenas dados do DSP, amostras, buffers, cálculos de filtros e o estado dos parâmetros. Ela **não sabe** qual tema está ativo.
*   A classe gráfica principal (`AudioProcessorEditor`) cria o `ThemeManager` e possui o controle do seletor visual de tema.
*   O DSP não sofre nenhuma alteração de latência, buffer size ou alocação de memória ao alternar entre Light e Dark Mode na tela, garantindo estabilidade e integridade total do áudio em tempo real.
