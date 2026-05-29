# AUREQ — Guia de Instalação — Windows (Draft)

**Versão**: 0.9.0-rc1  
**Plataforma**: Windows 10/11 (64-bit)  
**Status**: DRAFT — Compilação e instalador do Windows integrados via GitHub Actions (unsigned beta)

---

> [!WARNING]
> Este guia é preliminar para testadores beta. O build do Windows e o instalador `.exe` correspondente são gerados automaticamente no pipeline de CI do GitHub Actions em ambiente Windows real.
> O instalador executável gerado não está assinado digitalmente, o que pode acionar o aviso do Windows SmartScreen durante a instalação.
> Não usar o `AUREQ.vst3` macOS como payload Windows.

---

## Status do Build Windows

| Item | Status |
|------|--------|
| Ambiente de build Windows | ✅ Disponível via GitHub Actions (`windows-latest`) |
| Arquivo `AUREQ.vst3` para Windows | ✅ Gerado automaticamente no pipeline CI |
| Instalador plugin-only `.exe` | ✅ Compilado automaticamente no pipeline via Inno Setup |
| Script Inno Setup plugin-only | ✅ Preparado e integrado no workflow CI |
| Code signing Windows | 🔄 Pendente (Fase futura de assinatura de código) |

---

## Pré-requisitos (para execução do instalador)

- Windows 10 ou Windows 11 (64-bit)
- Uma DAW compatível com VST3 instalada (para uso do plugin):
  - Reaper, Ableton Live, Bitwig, Studio One, Cubase, FL Studio, Pro Tools, etc.
- Visual C++ Redistributable 2019 ou superior (pode ser requerido).

---

## 1. Instalação do Plugin VST3

O arquivo do plugin será `AUREQ.vst3` (uma pasta com estrutura interna).

### Caminho de destino padrão

```
C:\Program Files\Common Files\VST3\
```

> [!IMPORTANT]
> **Instalador Plugin-Only (Fase 17.0)**: Quando o instalador oficial for gerado em ambiente Windows real, o caminho de instalação padrão e obrigatório do VST3 será exclusivamente em `C:\Program Files\Common Files\VST3\` para garantir máxima compatibilidade com DAWs (Ableton Live, FL Studio, etc.). Instalações em pastas customizadas (como `C:\VSTPlugins`) não serão oferecidas pelo wizard para minimizar erros de varredura.

### Procedimento de instalação manual (quando o arquivo estiver disponível)

1. Copie a pasta `AUREQ.vst3` para:
   ```
   C:\Program Files\Common Files\VST3\
   ```
2. O instalador plugin-only usara o caminho global padrao. Caminhos de usuario nao serao oferecidos nesta fase para reduzir erros de scan em DAWs.

### Escanear o plugin na DAW

- **Reaper**: `Options > Preferences > Plug-ins > VST > Re-scan`
- **Ableton Live**: `Preferences > Plug-Ins > Rescan`
- **FL Studio**: `Options > Manage plugins > Find more plugins`
- **Cubase**: `Studio > Studio Setup > VST Plug-in Manager > Update`

O AUREQ deverá aparecer como **"AUREQ"** fabricado por **"ASTRA Audio"**.

---

## 2. Instalador Plugin-Only Inno Setup

Estrutura planejada:

```text
installer/Windows/plugin-only/
  scripts/AUREQ-plugin-only.iss
  resources/README-Windows.txt
  resources/AUREQ.ico
  staging/VST3/AUREQ.vst3
  output/AUREQ-0.9.0-rc1-Windows-plugin-only-setup.exe
```

O script Inno Setup usa:

```text
DefaultDirName={commoncf}\VST3
DisableDirPage=yes
Source=..\staging\VST3\AUREQ.vst3
DestDir={commoncf}\VST3
```

No Windows, `{commoncf}\VST3` deve resolver para:

```text
C:\Program Files\Common Files\VST3
```

O uninstaller padrao do Inno Setup removera o VST3 instalado. User presets nao devem ser removidos automaticamente nesta fase.

---

## 3. Aviso sobre Assinatura de Código

O AUREQ 0.9.0-rc1 **não está assinado** com certificado EV Windows Code Signing.

O Windows SmartScreen pode exibir:
> *"O Windows protegeu seu computador"* ou *"Aplicativo desconhecido"*

### Como contornar (apenas para RC interno)

1. Clique em **"Mais informações"** na tela do SmartScreen
2. Clique em **"Executar assim mesmo"**

> Na versão final com assinatura de código (Fase futura), este aviso não ocorrerá.

---

## 4. Localização dos User Presets (Windows)

Quando o build Windows estiver disponível, os user presets serão salvos em:

```
%APPDATA%\AUREQ\Presets\
```

Exemplo de caminho completo:
```
C:\Users\<SeuUsuário>\AppData\Roaming\AUREQ\Presets\
```

---

## 5. Desinstalação Manual (quando disponível)

### Remover o VST3

```
Excluir a pasta: C:\Program Files\Common Files\VST3\AUREQ.vst3
```

### Remover user presets (opcional)

```
Excluir a pasta: %APPDATA%\AUREQ\
```

---

## 6. Instalador Automático — Fase 17.1

Na **Fase 17.1**, o pipeline de CI do GitHub Actions foi configurado para compilar e gerar o instalador executável do Windows automaticamente a partir do build real do VST3.

- **Arquivo gerado**: `AUREQ-0.9.0-rc1-Windows-plugin-only-setup.exe` (disponível como artefato do workflow).
- **Destino obrigatório**: Copia o VST3 real para `C:\Program Files\Common Files\VST3\AUREQ.vst3`.
- **Registro do Windows**: Adiciona entrada no Painel de Controle > Programas e Recursos (Adicionar/Remover Programas).
- **Desinstalador**: Desinstalador integrado nativo do Inno Setup incluso para remoção limpa do plugin do sistema.

O instalador é executado de forma unsigned na fase beta de testes, necessitando de aprovação na caixa de diálogo do Windows SmartScreen.

---

## 7. Suporte

- **Fabricante**: ASTRA Audio
- **Plugin**: AUREQ v0.9.0-rc1
- **Bundle ID**: `br.com.sidyfurtado.aureq`
