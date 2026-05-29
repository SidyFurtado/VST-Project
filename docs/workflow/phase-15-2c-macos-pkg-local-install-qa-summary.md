# Fase 15.2C — macOS PKG Local Install QA

Data: 2026-05-28  
Pacote alvo: `installer/macOS/plugin-only/output/AUREQ-0.9.0-rc1-macOS-plugin-only.pkg`

## Resultado

Status desta execução: **bloqueada por autenticação sudo indisponível na sessão**.

O VST3 já existia antes desta execução em:

```bash
/Library/Audio/Plug-Ins/VST3/AUREQ.vst3
```

Foi criado backup local antes de qualquer tentativa de instalação:

```bash
backups/local-install-qa/AUREQ.vst3.backup-20260528-093811
```

O comando de instalação solicitado foi iniciado:

```bash
sudo installer -pkg "installer/macOS/plugin-only/output/AUREQ-0.9.0-rc1-macOS-plugin-only.pkg" -target /
```

Resultado: o `sudo` solicitou senha, mas a autenticação não foi concluída nesta sessão. O processo foi interrompido antes de instalar.

## Estado Observado Após a Tentativa

`ls -ld "/Library/Audio/Plug-Ins/VST3/AUREQ.vst3"`:

```text
drwxr-xr-x@ 3 root  wheel  96 May 27 16:21 /Library/Audio/Plug-Ins/VST3/AUREQ.vst3
```

`pkgutil --pkg-info br.com.sidyfurtado.aureq.vst3`:

```text
package-id: br.com.sidyfurtado.aureq.vst3
version: 0.9.0
volume: /
location:
install-time: 1779918130
```

O `install-time` corresponde a `2026-05-27 18:42:10 -03`, portanto o recibo observado parece ser de uma instalação anterior, não desta execução bloqueada.

`pkgutil --files br.com.sidyfurtado.aureq.vst3`:

```text
._Library
Library
Library/._Audio
Library/Audio
Library/Audio/._Plug-Ins
Library/Audio/Plug-Ins
Library/Audio/Plug-Ins/._VST3
Library/Audio/Plug-Ins/VST3
Library/Audio/Plug-Ins/VST3/._AUREQ.vst3
Library/Audio/Plug-Ins/VST3/AUREQ.vst3
Library/Audio/Plug-Ins/VST3/AUREQ.vst3/._Contents
Library/Audio/Plug-Ins/VST3/AUREQ.vst3/Contents
Library/Audio/Plug-Ins/VST3/AUREQ.vst3/Contents/._Info.plist
Library/Audio/Plug-Ins/VST3/AUREQ.vst3/Contents/._MacOS
Library/Audio/Plug-Ins/VST3/AUREQ.vst3/Contents/._PkgInfo
Library/Audio/Plug-Ins/VST3/AUREQ.vst3/Contents/._Resources
Library/Audio/Plug-Ins/VST3/AUREQ.vst3/Contents/.__CodeSignature
Library/Audio/Plug-Ins/VST3/AUREQ.vst3/Contents/Info.plist
Library/Audio/Plug-Ins/VST3/AUREQ.vst3/Contents/MacOS
Library/Audio/Plug-Ins/VST3/AUREQ.vst3/Contents/MacOS/._AUREQ
Library/Audio/Plug-Ins/VST3/AUREQ.vst3/Contents/MacOS/AUREQ
Library/Audio/Plug-Ins/VST3/AUREQ.vst3/Contents/PkgInfo
Library/Audio/Plug-Ins/VST3/AUREQ.vst3/Contents/Resources
Library/Audio/Plug-Ins/VST3/AUREQ.vst3/Contents/Resources/._AppIcon.icns
Library/Audio/Plug-Ins/VST3/AUREQ.vst3/Contents/Resources/._moduleinfo.json
Library/Audio/Plug-Ins/VST3/AUREQ.vst3/Contents/Resources/AppIcon.icns
Library/Audio/Plug-Ins/VST3/AUREQ.vst3/Contents/Resources/moduleinfo.json
Library/Audio/Plug-Ins/VST3/AUREQ.vst3/Contents/_CodeSignature
Library/Audio/Plug-Ins/VST3/AUREQ.vst3/Contents/_CodeSignature/._CodeResources
Library/Audio/Plug-Ins/VST3/AUREQ.vst3/Contents/_CodeSignature/CodeResources
```

## Manutencao

O VST3 observado foi mantido instalado para validacao manual em DAW:

```bash
/Library/Audio/Plug-Ins/VST3/AUREQ.vst3
```

Comandos de uninstall documentados, mas nao executados:

```bash
sudo rm -rf "/Library/Audio/Plug-Ins/VST3/AUREQ.vst3"
sudo pkgutil --forget br.com.sidyfurtado.aureq.vst3
```

Comando de restore do backup documentado, mas nao executado:

```bash
sudo cp -R "backups/local-install-qa/AUREQ.vst3.backup-20260528-093811" "/Library/Audio/Plug-Ins/VST3/AUREQ.vst3"
```

## Integridade de Escopo

Nao foram alterados C++, CMake, APVTS, DSP, analyzer FFT, presets, UI interna, PKG gerado, livros ou PDFs.

Proximo passo: repetir a instalacao local com autenticacao administrativa disponivel e, depois, executar validacao manual em DAW.
