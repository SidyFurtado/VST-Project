# AUREQ — Guia de Instalação Manual — macOS

**Versão**: 0.9.0-rc1  
**Plataforma**: macOS 10.13 High Sierra ou superior

---

> [!WARNING]
> Este é um Release Candidate não assinado e não notarizado.
> O macOS Gatekeeper pode exibir um aviso de segurança ao abrir o aplicativo pela primeira vez.
> Veja a seção [Aviso do Gatekeeper](#aviso-do-gatekeeper) para instruções.

---

## Pré-requisitos

- macOS 10.13 High Sierra ou superior (recomendado: macOS 12 Monterey+)
- Para uso do VST3: uma DAW compatível com o formato VST3 instalada
  - Reaper, Ableton Live, Bitwig Studio, Cakewalk, Studio One, etc.
- Para uso do Standalone: nenhuma DAW necessária

---

## 1. Instalação do Plugin VST3

O arquivo do plugin é `AUREQ.vst3` (um bundle — pasta com estrutura interna).

> [!IMPORTANT]
> **Instalador Automático (Fase 15.2B)**: O instalador `.pkg` plugin-only instala o VST3 exclusivamente no nível do sistema (`/Library/Audio/Plug-Ins/VST3/AUREQ.vst3`) para garantir máxima compatibilidade com todas as DAWs (Ableton Live, Logic Pro, Reaper, etc.). Instalações customizadas de caminho de plugin não são permitidas no instalador.

### Opção recomendada — DMG visual plugin-only

Abra o arquivo:

```text
AUREQ-0.9.0-rc1-macOS-plugin-only.dmg
```

Depois, clique duas vezes em:

```text
AUREQ Installer.pkg
```

O DMG e apenas a embalagem visual de distribuicao. O PKG dentro dele continua sendo o instalador real e coloca o VST3 em:

```text
/Library/Audio/Plug-Ins/VST3/AUREQ.vst3
```

> [!NOTE]
> Este RC ainda nao esta assinado nem notarizado. O macOS pode avisar que o instalador nao e de um desenvolvedor identificado.

### Opção Terminal — Instalador PKG plugin-only

```bash
sudo installer -pkg "AUREQ-0.9.0-rc1-macOS-plugin-only.pkg" -target /
```

Verificar instalação:

```bash
ls -ld "/Library/Audio/Plug-Ins/VST3/AUREQ.vst3"
pkgutil --pkg-info br.com.sidyfurtado.aureq.vst3
pkgutil --files br.com.sidyfurtado.aureq.vst3
```

### Opção A — Instalação manual para todos os usuários do sistema

```bash
sudo cp -r AUREQ.vst3 /Library/Audio/Plug-Ins/VST3/
```

> Requer senha de administrador. Todos os usuários da máquina terão acesso ao plugin.

### Opção B — Instalação apenas para o usuário atual (recomendada)

```bash
cp -r AUREQ.vst3 ~/Library/Audio/Plug-Ins/VST3/
```

> Não requer senha. O plugin fica disponível apenas para o usuário atual.  
> A pasta `~/Library/Audio/Plug-Ins/VST3/` será criada automaticamente se não existir.

### Verificar instalação

Após copiar, confirme que o arquivo está no lugar:

```bash
ls ~/Library/Audio/Plug-Ins/VST3/ | grep AUREQ
```

Saída esperada:
```
AUREQ.vst3
```

### Escanear o plugin na DAW

Abra sua DAW e force um scan de plugins:
- **FL Studio macOS**: `Plugin Manager > Find installed plugins`
  - Confirme que `/Library/Audio/Plug-Ins/VST3` esta habilitado.
  - Use `Rescan previously verified plugins` e `Verify plugins` se o AUREQ nao aparecer ou aparecer duplicado.
  - Se aparecer `AUREQ_2` sem duplicata fisica no Finder/Terminal, trate primeiro como possivel cache interno da DAW.
- **Reaper**: `Options > Preferences > Plug-ins > VST > Re-scan`
- **Ableton**: `Preferences > Plug-Ins > Rescan`
- **Bitwig**: `Settings > Plug-ins > Rescan Plug-ins`

O AUREQ deverá aparecer como **"AUREQ"** fabricado por **"ASTRA Audio"**.

> [!NOTE]
> Logic Pro nao carrega VST3 nativamente. A validacao em Logic deve ser tratada em fase futura via AU ou wrapper, nao como falha do VST3.

---

## 2. Instalação do Standalone

O arquivo é `AUREQ.app` (aplicativo macOS nativo).

### Opção A — Instalação global

Arraste `AUREQ.app` para `/Applications/`:

```bash
cp -r AUREQ.app /Applications/
```

### Opção B — Instalação para o usuário atual

```bash
cp -r AUREQ.app ~/Applications/
```

> Se a pasta `~/Applications/` não existir, crie-a:
> ```bash
> mkdir -p ~/Applications/
> ```

### Abrir o Standalone

```bash
open /Applications/AUREQ.app
```

Ou simplesmente clique duas vezes no ícone no Finder.

---

## 3. Aviso do Gatekeeper

Como este RC não está assinado nem notarizado, o macOS pode exibir:

> *"AUREQ.app" não pode ser aberto porque é de um desenvolvedor não identificado.*

### Como contornar (apenas para este RC interno)

**Método 1 — Clique com o botão direito:**
1. Clique com o botão direito (ou Ctrl+clique) em `AUREQ.app`
2. Selecione **Abrir**
3. Clique **Abrir** na caixa de diálogo de segurança

**Método 2 — Preferências do sistema:**
1. Abra **Preferências do Sistema > Segurança e Privacidade**
2. Na aba **Geral**, clique em **"Abrir mesmo assim"** ao lado da mensagem sobre AUREQ

**Método 3 — Terminal (remover quarentena):**
```bash
xattr -dr com.apple.quarantine /Applications/AUREQ.app
```

> [!IMPORTANT]
> `xattr` e apenas um workaround local para beta/RC interno. Nao e solucao de produto final.
> A release publica deve usar Apple Developer ID, assinatura e notarizacao para remover alertas do Gatekeeper.

---

## 4. Localização dos User Presets

Os presets de usuário são salvos em:

```
~/Library/Application Support/AUREQ/Presets/
```

Extensão: `.aureqpreset` (XML APVTS)

Para revelar a pasta no Finder, dentro do Standalone ou plugin:
- Abra o preset browser
- Selecione `User Presets > Reveal Presets Folder`

---

## 5. Desinstalação Manual

### Remover o VST3

```bash
rm -rf ~/Library/Audio/Plug-Ins/VST3/AUREQ.vst3
# ou, se instalado globalmente:
sudo rm -rf /Library/Audio/Plug-Ins/VST3/AUREQ.vst3
```

Se instalado via PKG plugin-only, remover tambem o recibo do instalador:

```bash
sudo pkgutil --forget br.com.sidyfurtado.aureq.vst3
```

### Remover o Standalone

```bash
rm -rf /Applications/AUREQ.app
# ou, se instalado no diretório do usuário:
rm -rf ~/Applications/AUREQ.app
```

### Remover preferências e user presets (opcional)

```bash
# User presets
rm -rf ~/Library/Application\ Support/AUREQ/

# Preferências de idioma (PropertiesFile JUCE)
rm -f ~/Library/Preferences/br.com.sidyfurtado.aureq.settings
```

---

## 6. Solução de Problemas

| Problema | Solução |
|----------|---------|
| Plugin não aparece na DAW | Force rescan na DAW; confirme que o `.vst3` está em `~/Library/Audio/Plug-Ins/VST3/` |
| "Developer not identified" | Use clique direito > Abrir ou remova quarentena com `xattr` |
| Standalone não abre | Verifique macOS 10.13+; tente abrir pelo Terminal com `open AUREQ.app` |
| User presets não aparecem | Use `Refresh User Presets` no preset browser |
| Acentos incorretos na UI | Não esperado neste RC; reportar ao desenvolvedor |

---

## 7. Suporte

- **Fabricante**: ASTRA Audio
- **Plugin**: AUREQ v0.9.0-rc1
- **Bundle ID**: `br.com.sidyfurtado.aureq`
