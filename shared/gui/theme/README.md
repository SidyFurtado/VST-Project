# Arquitetura de Temas (shared/gui/theme)

Esta pasta abriga a especificação da arquitetura C++/JUCE para gerenciamento de temas (Light Mode e Dark Mode) baseada no **Design System** e nos **Theme Tokens** do projeto.

---

## Estrutura de Arquivos Futuros Planejados

Para implementar o sistema de temas nas fases seguintes de desenvolvimento do plugin, serão criados os seguintes arquivos de cabeçalho e implementação:

### 1. `ThemeTokens.h`
*   **Função**: Guarda os nomes semânticos dos tokens de design (ex: enums como `ColorId::colorBackgroundPrimary`, `ColorId::colorAccentPrimary`) e valores constantes globais que nunca mudam entre temas (como espaçamentos em pixels e raios de borda).
*   **Mapeamento**:
    ```cpp
    namespace ThemeTokens {
        enum class ColorId {
            colorBackgroundPrimary,
            colorBackgroundSecondary,
            colorPanelSurface,
            colorTextPrimary,
            colorAccentPrimary,
            // ...
        };
        constexpr float spacingMedium = 16.0f;
        constexpr float radiusMedium = 8.0f;
    }
    ```

### 2. `LightTheme.h`
*   **Função**: Define os valores concretos do **Light Mode** para cada token de cor mapeado em `ThemeTokens.h` (ex: mapear `colorBackgroundPrimary` para a cor `#EDF2F4`).
*   **Mapeamento**:
    ```cpp
    struct LightTheme {
        static juce::Colour getColor (ThemeTokens::ColorId id);
    };
    ```

### 3. `DarkTheme.h`
*   **Função**: Define os valores concretos do **Dark Mode** para cada token de cor (ex: mapear `colorBackgroundPrimary` para a cor `#0B0F12`).
*   **Mapeamento**:
    ```cpp
    struct DarkTheme {
        static juce::Colour getColor (ThemeTokens::ColorId id);
    };
    ```

### 4. `ThemeManager.h`
*   **Função**: O controlador centralizado de temas do monorepo (implementado como um Singleton ou objeto compartilhado). Ele herda de `juce::ChangeBroadcaster` para gerenciar a troca manual de tema pela UI e notificar todos os componentes a redesenharem-se (`repaint()`).
*   **Mapeamento**:
    ```cpp
    class ThemeManager : public juce::ChangeBroadcaster {
    public:
        enum class ThemeType { Light, Dark };
        static ThemeManager& getInstance();
        void setTheme (ThemeType theme);
        juce::Colour getColor (ThemeTokens::ColorId id) const;
    };
    ```

### 5. `ThemeAwareComponent.h`
*   **Função**: Classe base utilitária de componentes gráficos da qual nossos botões, knobs e painéis herdam. Ela herda de `juce::Component` e escuta o `ThemeManager` via `juce::ChangeListener`. Ao receber um aviso de mudança, chama automaticamente `repaint()`.
*   **Mapeamento**:
    ```cpp
    class ThemeAwareComponent : public juce::Component, 
                                private juce::ChangeListener {
    public:
        ThemeAwareComponent();
        void changeListenerCallback (juce::ChangeBroadcaster* source) override;
        virtual void themeChanged();
    };
    ```

---

## Fluxo de Trabalho de Renderização

1.  O componente desenha a si mesmo em `paint()` consultando o `ThemeManager`:
    ```cpp
    g.fillAll (ThemeManager::getInstance().getColor (ThemeTokens::ColorId::colorBackgroundPrimary));
    ```
2.  Quando o usuário clica no seletor de tema na barra superior, a UI chama `ThemeManager::getInstance().setTheme(newTheme)`.
3.  O `ThemeManager` altera o estado interno e dispara `sendChangeMessage()`.
4.  Todos os `ThemeAwareComponent` ativos recebem o callback, chamam `repaint()`, e o plugin redesenha a tela instantaneamente com a nova paleta de cores.
