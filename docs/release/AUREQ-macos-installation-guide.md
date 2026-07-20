# ASTRA Audio Suite — Guia de Instalação macOS (AU-First)

**Versão**: 0.9.0-rc1  
**Plataforma**: macOS 10.15 Catalina ou superior  
**Formato Principal**: Audio Unit (AU)

---

> [!WARNING]
> Este é um Release Candidate de beta privada não assinado e não notarizado.
> O macOS Gatekeeper exibirá um aviso de segurança ao abrir o instalador.
> Veja a seção [Aviso do Gatekeeper](#aviso-do-gatekeeper) para instruções de contorno.

---

## Pré-requisitos

- macOS 10.15 Catalina ou superior (compatível com Intel e Apple Silicon nativo)
- DAW compatível com o formato Audio Unit (AU):
  - Logic Pro, GarageBand, Final Cut, Ableton Live, FL Studio macOS, Reaper, etc.

---

## 1. Instalação Automática da Suite (Recomendado)

O pacote oficial da suite de plugins é o instalador unificado:

```text
https://github.com/SidyFurtado/VST-Project/releases/download/v1.0.0/ASTRA-Audio-Suite-0.9.0-rc1-macOS-AU.pkg
```

Este instalador coloca as Audio Units dos três plugins ativos no nível global do sistema:

```text
/Library/Audio/Plug-Ins/Components/AUREQ.component
/Library/Audio/Plug-Ins/Components/LUMINAR.component
/Library/Audio/Plug-Ins/Components/GRAVITY.component
```

### Passos:
1. Clique duas vezes em `ASTRA-Audio-Suite-0.9.0-rc1-macOS-AU.pkg` (consulte a seção do Gatekeeper se aparecer aviso de segurança).
2. Siga as instruções do assistente de instalação da ASTRA Audio.
3. Insira sua senha de administrador quando solicitado.
4. Abra sua DAW e force o escaneamento/validação das Audio Units.

---

## 2. Limpeza Preventiva de Duplicatas e Atualização do Cache

Se você testou versões beta antigas dos plugins que usavam o código de fabricante temporário `Manu` (em vez do código definitivo `AStr`), você deve executar a limpeza abaixo para evitar conflitos de cache na DAW:

### Passo 1 — Remover duplicatas de usuário:
No Terminal, execute:
```bash
rm -rf ~/Library/Audio/Plug-Ins/Components/AUREQ.component
rm -rf ~/Library/Audio/Plug-Ins/Components/LUMINAR.component
rm -rf ~/Library/Audio/Plug-Ins/Components/GRAVITY.component
```

### Passo 2 — Limpar o cache de Audio Units do macOS:
Para forçar o macOS a escanear a assinatura `AStr` correta, delete o cache do sistema e reinicie o serviço de registro:
```bash
rm -f ~/Library/Caches/AudioUnitCache/com.apple.audiounits.cache
killall -9 AudioComponentRegistrar
```

---

## 3. Aviso do Gatekeeper (Instalador Unsigned)

Como este pacote de beta privada não é assinado digitalmente, o macOS impedirá a abertura direta mostrando o aviso:
> *"ASTRA-Audio-Suite-0.9.0-rc1-macOS-AU.pkg" não pode ser aberto porque é de um desenvolvedor não identificado.*

### Como abrir:
1. Pressione **Control** e clique (ou clique com o botão direito) no arquivo `.pkg`.
2. Escolha **Abrir** no menu contextual.
3. Clique em **Abrir** (ou Abrir Mesmo Assim) na caixa de diálogo de aviso do macOS.
4. O instalador iniciará normalmente.

---

## 4. Escanear o plugin na DAW

### Logic Pro
* O Logic Pro valida os componentes automaticamente na inicialização.
* Se os plugins não aparecerem, acesse: `Logic Pro > Definições > Gestor de Plugins` (Preferences > Plug-in Manager).
* Localize os plugins da **ASTRA Audio** (fabricante `AStr`).
* Selecione os plugins e clique em **Reset & Rescan Selection** (Redefinir e Escanear Novamente a Seleção).

### Ableton Live macOS
* Vá em `Preferences > Plug-Ins`.
* Certifique-se de que a opção **"Use Audio Units"** (Usar Audio Units) está ativada.
* Se necessário, clique em **Rescan** (Re-escanear).

### FL Studio macOS
* Vá em `Options > Manage plugins`.
* Clique em **Find installed plugins** (Localizar plugins instalados).
* Certifique-se de que a opção **"Verify plugins"** está marcada para que o FL Studio reconheça o formato AU corretamente.

---

## 5. Formatos Legados (VST3 / Standalone)

* **macOS VST3:** O formato VST3 no macOS é mantido apenas como ferramenta de compatibilidade de desenvolvimento/QA. Não está incluído na distribuição pública da suite.
* **macOS Standalone:** Os aplicativos Standalone são binários internos de depuração. Não devem ser usados para mixagem/produção de áudio geral.

---

## 6. Desinstalação Completa

Para remover a suite de Audio Units do sistema e limpar o registro do instalador, execute os seguintes comandos no Terminal:

```bash
# Remover os plugins
sudo rm -rf /Library/Audio/Plug-Ins/Components/AUREQ.component
sudo rm -rf /Library/Audio/Plug-Ins/Components/LUMINAR.component
sudo rm -rf /Library/Audio/Plug-Ins/Components/GRAVITY.component

# Esquecer o registro do recibo da instalação
sudo pkgutil --forget br.com.sidyfurtado.astra-suite.au
```
