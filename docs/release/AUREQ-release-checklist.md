# AUREQ — Release Checklist Master

Checklist de release para todas as etapas de empacotamento e distribuição do AUREQ.

Status:
- `[DONE]` — concluído
- `[PENDING]` — pendente, sem bloqueio
- `[BLOCKED]` — bloqueado por dependência externa
- `[FUTURE]` — fase futura, fora do escopo atual

---

## Bloco 1 — Pré-Release: Build & QA

- [DONE] `cmake --build build` passa sem erros
- [DONE] `./build/tests/AUREQ_FilterTests` — 100% PASS (3 suites, 0 falhas)
- [DONE] Artefato VST3 gerado: `build/.../VST3/AUREQ.vst3`
- [DONE] Artefato Standalone gerado: `build/.../Standalone/AUREQ.app`
- [DONE] Bundle ID sem espaços: `br.com.sidyfurtado.aureq` — verificado nos dois binários
- [DONE] Versão `0.9.0` nos binários — verificada via plist
- [DONE] Standalone abre sem crash — PID confirmado
- [DONE] QA manual do Standalone (Fases 14.1–14.2) — aprovado pelo usuário
- [PENDING] DAW Validation (Bloco K) — aguarda DAW disponível no ambiente
- [DONE] FL Studio macOS validation parcial — AUREQ principal detectado como VST3, UI abre, vendor `ASTRA Audio`; `AUREQ_2` tratado como cache antigo
- [PENDING] FL Studio macOS Musical QA — checklist criado; aguarda passe musical humano

---

## Bloco 2 — Metadados & Identidade

- [DONE] `PRODUCT_NAME "AUREQ"` — correto no CMakeLists.txt
- [DONE] `VERSION "0.9.0"` — correto
- [DONE] `BUNDLE_ID "br.com.sidyfurtado.aureq"` — correto, sem espaços
- [DONE] `COMPANY_NAME "ASTRA Audio"` — correto
- [DONE] `PLUGIN_CODE Aurq` — correto
- [DONE] `MANUFACTURER_CODE AStr` — correto
- [FUTURE] Ícone do plugin `.icns` (macOS) — Fase 15.1
- [FUTURE] Ícone do plugin `.ico` (Windows) — Fase 15.1
- [FUTURE] Integração do ícone nos bundles — Fase 15.1

---

## Bloco 3 — Staging / Distribution Prep (macOS)

- [DONE] Estrutura de staging criada: `release/AUREQ-0.9.0-rc1/`
- [DONE] VST3 copiado para `release/AUREQ-0.9.0-rc1/macOS/VST3/AUREQ.vst3`
- [DONE] Standalone copiado para `release/AUREQ-0.9.0-rc1/macOS/Standalone/AUREQ.app`
- [DONE] Placeholder Windows criado: `release/AUREQ-0.9.0-rc1/Windows/VST3/.gitkeep`
- [DONE] Pasta `release/AUREQ-0.9.0-rc1/Docs/` criada
- [DONE] Release notes criadas: `docs/release/AUREQ-0.9.0-rc1-release-notes.md`
- [DONE] Guia macOS criado: `docs/release/AUREQ-macos-installation-guide.md`
- [DONE] Guia Windows (draft): `docs/release/AUREQ-windows-installation-guide-draft.md`
- [DONE] Kit de beta privada criado: instrucoes, feedback form e known limitations
- [DONE] Binários de staging não commitados no git (`.gitignore` preservado)

---

## Bloco 4 — macOS Code Signing & Notarização

- [BLOCKED] Certificado Apple Developer ID Application disponível
- [BLOCKED] `codesign --deep --sign "Developer ID Application: ..."` no VST3
- [BLOCKED] `codesign --deep --sign "Developer ID Application: ..."` no Standalone
- [BLOCKED] `xcrun notarytool submit` — submissão para notarização Apple
- [BLOCKED] `xcrun stapler staple` — grampear ticket de notarização
- [BLOCKED] Gatekeeper validado sem aviso de segurança

> Todos os itens deste bloco são bloqueados por ausência de certificado Apple Developer ID.  
> Não executar sem certificado explicitamente fornecido pelo usuário.

---

## Bloco 5 — macOS Installer (.pkg / .dmg) — Fase 15.2

- [DONE] Component package `pkgbuild` para gerar `.pkg` do VST3
- [DONE] `Distribution.xml` para `productbuild`
- [DONE] `.dmg` via `hdiutil` com assets visuais premium; polish avançado da janela Finder ficou como fase futura
- [DONE] Payload embutido nativo (copiar VST3 para `/Library/Audio/Plug-Ins/VST3/`)
- [BLOCKED] Fase 15.2C reexecutada em 2026-05-28 — backup criado, mas `sudo installer` bloqueado por autenticação indisponível na sessão
- [DONE] Fase 15.2H hardening macOS — duplicatas, metadata, Gatekeeper e DMG UX documentados
- [DONE] Fase 16.3 refresh beta macOS — PKG/DMG regenerados com build mais recente, sem instalar
- [FUTURE] Uninstaller script
- [DONE] Background e ícone do instalador (Fases 15.1 e 15.2B)
- [FUTURE] Assinatura do `.pkg` / `.dmg` (depende do Bloco 4)

---

## Bloco 6 — Windows Build & Staging — Fase 17.1 (GitHub Actions CI)

- [DONE] Ambiente Windows (runner GitHub Actions `windows-latest`) disponível
- [DONE] Estrutura Windows plugin-only preparada em `installer/Windows/plugin-only/`
- [PARTIAL] `cmake --build build` em ambiente Windows via CI (Aguardando execução do pipeline)
- [PARTIAL] Artefato VST3 Windows gerado (Aguardando execução do pipeline)
- [PARTIAL] VST3 Windows copiado para `installer/Windows/plugin-only/staging/VST3/AUREQ.vst3` (Cópia em staging no runner)
- [FUTURE] VST3 copiado localmente para `release/AUREQ-0.9.0-rc1/Windows/VST3/` (Fase 17.2, pós-download do artifact)

---

## Bloco 7 — Windows Installer — Fase 17.1 (Inno Setup no Actions)

- [DONE] Script Inno Setup plugin-only `.iss` criado e adaptado para CI
- [DONE] Destino VST3 definido: `C:\Program Files\Common Files\VST3\AUREQ.vst3`
- [DONE] `{commoncf}\VST3` documentado como destino Inno Setup
- [DONE] `DisableDirPage=yes` para evitar caminho customizado no wizard
- [DONE] README Windows plugin-only criado
- [DONE] `AUREQ.ico` Windows gerado localmente e validado
- [PARTIAL] Gerar `.exe` final — compilado automaticamente no runner (Aguardando execução do pipeline)
- [PARTIAL] Payload Windows `AUREQ.vst3` disponível no staging do runner (Aguardando execução do pipeline)
- [FUTURE] Wizard branding BMP validado (Fase 17.2)
- [FUTURE] Windows Code Signing (certificado EV/OV) (Fase futura)

---

## Bloco 8 — Distribuição Pública

- [FUTURE] Repositório público de release (GitHub Releases, site, etc.)
- [FUTURE] Arquivo ZIP de distribuição com guia de instalação
- [FUTURE] Comunicação de release (changelog público, anúncio)
- [FUTURE] Site da ASTRA Audio com página do AUREQ
- [FUTURE] Documentação pública do produto

---

## Bloco 9 — Suite Completa — Fase Futura

- [FUTURE] Instalador da ASTRA Audio Suite (plugin + standalone + assets)
- [FUTURE] Ícone do pacote/suite ASTRA Audio
- [FUTURE] Tela de splash do instalador da suite
- [FUTURE] Versionamento da suite separado do versionamento do plugin

---

## Resumo de Status

| Bloco | DONE | PENDING | BLOCKED | FUTURE |
|-------|------|---------|---------|--------|
| 1 — Build & QA | 9 | 2 | 0 | 0 |
| 2 — Metadados & Identidade | 6 | 0 | 0 | 3 |
| 3 — Staging macOS | 10 | 0 | 0 | 0 |
| 4 — Signing macOS | 0 | 0 | 6 | 0 |
| 5 — macOS Installer | 7 | 0 | 1 | 2 |
| 6 — Windows Build | 1 | 0 | 1 | 4 |
| 7 — Windows Installer | 5 | 0 | 2 | 3 |
| 8 — Distribuição Pública | 0 | 0 | 0 | 5 |
| 9 — Suite Completa | 0 | 0 | 0 | 5 |
| **Total** | **38** | **2** | **10** | **22** |
