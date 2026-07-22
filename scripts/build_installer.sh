#!/usr/bin/env bash
# ==============================================================================
# build_installer.sh — VOID (ASTRA Audio Suite)
# Automated Release Build, Codesigning & macOS .pkg Installer Generator
#
# Usage:
#   ./scripts/build_installer.sh                    # Build + package (default)
#   ./scripts/build_installer.sh --no-codesign      # Skip codesigning
#   ./scripts/build_installer.sh --no-notarize      # Build + sign, skip notarization
#   ./scripts/build_installer.sh --skip-build       # Re-package existing build only
#
# Environment variables (optional):
#   DEVELOPER_ID   - Apple Developer ID for codesigning (e.g. "Developer ID Application: Name (TEAM)")
#   APPLE_ID       - Apple ID for notarization
#   APPLE_PASSWORD - App-specific password for notarization
#   APPLE_TEAM_ID  - Team ID for notarization
# ==============================================================================

set -euo pipefail
ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build"
INSTALLER_DIR="${BUILD_DIR}/installer"
PRODUCT_NAME="VOID"
VERSION="1.0.0"
BUNDLE_ID="br.com.sidyfurtado.void"

# ---- Parse arguments ---------------------------------------------------------
DO_CODESIGN=true
DO_NOTARIZE=true
DO_BUILD=true

for arg in "$@"; do
    case "$arg" in
        --no-codesign) DO_CODESIGN=false; DO_NOTARIZE=false ;;
        --no-notarize) DO_NOTARIZE=false ;;
        --skip-build)  DO_BUILD=false ;;
    esac
done

# ---- Colors for output -------------------------------------------------------
RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'; NC='\033[0m'
info()  { echo -e "${GREEN}[INFO]${NC}  $*"; }
warn()  { echo -e "${YELLOW}[WARN]${NC}  $*"; }
error() { echo -e "${RED}[ERROR]${NC} $*"; exit 1; }

# ---- Step 0: Check prerequisites --------------------------------------------
check_prereqs() {
    command -v cmake >/dev/null 2>&1 || error "cmake is required but not installed."
    command -v pkgbuild >/dev/null 2>&1 || error "pkgbuild not found (Xcode command line tools required)."
    command -v productbuild >/dev/null 2>&1 || error "productbuild not found."

    if $DO_CODESIGN && [ -z "${DEVELOPER_ID:-}" ]; then
        warn "DEVELOPER_ID not set. Falling back to ad-hoc signing."
        DEVELOPER_ID="-"
    fi
}
check_prereqs

# ---- Step 1: Clean Build (Release) ------------------------------------------
if $DO_BUILD; then
    info "Cleaning previous build..."
    rm -rf "${BUILD_DIR}"
    mkdir -p "${BUILD_DIR}"

    info "Configuring CMake (Release + Universal Binary)..."
    cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}" \
        -G "Unix Makefiles" \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" \
        -DCMAKE_CXX_FLAGS_RELEASE="-O3 -flto=auto -fvisibility=hidden"

    info "Building VOID VST3 + AU + Standalone..."
    cmake --build "${BUILD_DIR}" --target VOID_All -- -j"$(sysctl -n hw.logicalcpu)"
    info "Build complete."
else
    info "Skipping build (--skip-build). Using existing build in ${BUILD_DIR}"
fi

# ---- Step 2: Locate build artifacts -----------------------------------------
VST3_SRC=$(find "${BUILD_DIR}" -type d -name "VOID.vst3"        | head -n 1)
AU_SRC=$(  find "${BUILD_DIR}" -type d -name "VOID.component"   | head -n 1)
STANDALONE_SRC=$(find "${BUILD_DIR}" -type d -name "VOID.app"   | head -n 1)

[ -n "$VST3_SRC" ] || error "VOID.vst3 not found in build directory!"
[ -n "$AU_SRC" ]   || error "VOID.component not found in build directory!"

info "Found VST3:      ${VST3_SRC}"
info "Found AU:        ${AU_SRC}"
[ -n "$STANDALONE_SRC" ] && info "Found Standalone: ${STANDALONE_SRC}"

# ---- Step 3: Sanitize metadata (xattr) --------------------------------------
info "Removing quarantine and extended attributes from bundles..."
xattr -cr "${VST3_SRC}" 2>/dev/null || true
xattr -cr "${AU_SRC}"   2>/dev/null || true
[ -n "$STANDALONE_SRC" ] && xattr -cr "${STANDALONE_SRC}" 2>/dev/null || true

# ---- Step 4: Codesign (if available) ----------------------------------------
if $DO_CODESIGN; then
    info "Codesigning with Developer ID: ${DEVELOPER_ID}"
    CODESIGN_OPTS="--force --verify --verbose --timestamp --options=runtime"

    codesign ${CODESIGN_OPTS} --sign "${DEVELOPER_ID}" "${VST3_SRC}"
    codesign ${CODESIGN_OPTS} --sign "${DEVELOPER_ID}" "${AU_SRC}"
    [ -n "$STANDALONE_SRC" ] && codesign ${CODESIGN_OPTS} --sign "${DEVELOPER_ID}" "${STANDALONE_SRC}"

    info "Verifying signatures..."
    codesign --verify --deep --strict "${VST3_SRC}"
    codesign --verify --deep --strict "${AU_SRC}"
    info "Codesigning complete."
else
    warn "Skipping codesign. Using ad-hoc signature from build."
fi

# ---- Step 5: Stage for packaging --------------------------------------------
STAGING="${INSTALLER_DIR}/staging"
rm -rf "${STAGING}"
mkdir -p "${STAGING}/VST3" "${STAGING}/AU" "${STAGING}/Standalone"

cp -R "${VST3_SRC}"        "${STAGING}/VST3/"
cp -R "${AU_SRC}"          "${STAGING}/AU/"
[ -n "$STANDALONE_SRC" ] && cp -R "${STANDALONE_SRC}" "${STAGING}/Standalone/"

# ---- Step 6: Build component .pkg files -------------------------------------
COMPONENT_DIR="${INSTALLER_DIR}/components"
OUTPUT_DIR="${INSTALLER_DIR}/output"
rm -rf "${COMPONENT_DIR}" "${OUTPUT_DIR}"
mkdir -p "${COMPONENT_DIR}" "${OUTPUT_DIR}"

info "Building component packages..."

pkgbuild --root "${STAGING}/VST3" \
    --identifier "${BUNDLE_ID}.vst3" \
    --version "${VERSION}" \
    --install-location "/Library/Audio/Plug-Ins/VST3" \
    "${COMPONENT_DIR}/VOID-VST3.pkg"

pkgbuild --root "${STAGING}/AU" \
    --identifier "${BUNDLE_ID}.au" \
    --version "${VERSION}" \
    --install-location "/Library/Audio/Plug-Ins/Components" \
    "${COMPONENT_DIR}/VOID-AU.pkg"

if [ -n "$STANDALONE_SRC" ]; then
    pkgbuild --root "${STAGING}/Standalone" \
        --identifier "${BUNDLE_ID}.standalone" \
        --version "${VERSION}" \
        --install-location "/Applications" \
        "${COMPONENT_DIR}/VOID-Standalone.pkg"
fi

# ---- Step 7: Create Distribution XML ----------------------------------------
info "Creating distribution XML..."
DIST_XML="${OUTPUT_DIR}/Distribution.xml"

cat > "${DIST_XML}" << EOF
<?xml version="1.0" encoding="utf-8"?>
<installer-gui-script minSpecVersion="2">
    <title>VOID - AI Noise Vacuum</title>
    <organization>${BUNDLE_ID}</organization>
    <version>${VERSION}</version>
    <welcome file="welcome.html" />
    <conclusion file="conclusion.html" />
    <pkg-ref id="${BUNDLE_ID}.vst3"/>
    <pkg-ref id="${BUNDLE_ID}.au"/>
    <pkg-ref id="${BUNDLE_ID}.standalone"/>
    <options customize="allow" require-scripts="false" hostArchitectures="arm64,x86_64"/>
    <domains enable_anywhere="false" enable_localSystem="true" enable_currentUserHome="false"/>
    <choices-outline>
        <line choice="default">
            <line choice="${BUNDLE_ID}.vst3"/>
            <line choice="${BUNDLE_ID}.au"/>
            <line choice="${BUNDLE_ID}.standalone"/>
        </line>
    </choices-outline>
    <choice id="default" title="VOID Plugin Bundle" description="VOID Plugin Suite Installation"/>
    <choice id="${BUNDLE_ID}.vst3" visible="true" title="VST3 Plugin" description="Install VOID as a VST3 plugin for compatible DAWs (Reaper, FL Studio, Cubase, etc.)" start_selected="true">
        <pkg-ref id="${BUNDLE_ID}.vst3"/>
    </choice>
    <choice id="${BUNDLE_ID}.au" visible="true" title="Audio Unit (AU) Plugin" description="Install VOID as an Audio Unit plugin for Logic Pro, GarageBand, etc." start_selected="true">
        <pkg-ref id="${BUNDLE_ID}.au"/>
    </choice>
    <choice id="${BUNDLE_ID}.standalone" visible="true" title="Standalone App" description="Install the standalone VOID application for testing without a DAW." start_selected="true">
        <pkg-ref id="${BUNDLE_ID}.standalone"/>
    </choice>
    <pkg-ref id="${BUNDLE_ID}.vst3"    version="${VERSION}" onConclusion="none">VOID-VST3.pkg</pkg-ref>
    <pkg-ref id="${BUNDLE_ID}.au"      version="${VERSION}" onConclusion="none">VOID-AU.pkg</pkg-ref>
    <pkg-ref id="${BUNDLE_ID}.standalone" version="${VERSION}" onConclusion="none">VOID-Standalone.pkg</pkg-ref>
</installer-gui-script>
EOF

# Create minimal HTML resources
mkdir -p "${OUTPUT_DIR}/resources"
cat > "${OUTPUT_DIR}/resources/welcome.html" << 'HTML'
<!DOCTYPE html><html><body style="font-family: -apple-system; font-size: 11pt;">
<h2>Welcome to VOID</h2>
<p>AI-powered noise reduction plugin by <b>ASTRA Audio</b>.</p>
<p>This installer will place VOID in your system audio plugin directories.</p>
</body></html>
HTML

cat > "${OUTPUT_DIR}/resources/conclusion.html" << 'HTML'
<!DOCTYPE html><html><body style="font-family: -apple-system; font-size: 11pt;">
<h2>Installation Complete</h2>
<p>VOID has been installed successfully.</p>
<p>Open your DAW, rescan plugins, and look for <b>VOID</b> under ASTRA Audio.</p>
</body></html>
HTML

# ---- Step 8: Build final .pkg -----------------------------------------------
FINAL_PKG="${INSTALLER_DIR}/VOID_macOS_Installer.pkg"
info "Building final installer package: ${FINAL_PKG}"

productbuild --distribution "${DIST_XML}" \
    --package-path "${COMPONENT_DIR}" \
    --resources "${OUTPUT_DIR}/resources" \
    "${FINAL_PKG}"

# ---- Step 9: Notarize (if available) ----------------------------------------
if $DO_NOTARIZE && [ -n "${APPLE_ID:-}" ] && [ -n "${APPLE_PASSWORD:-}" ] && [ -n "${APPLE_TEAM_ID:-}" ]; then
    info "Submitting for notarization..."
    xcrun notarytool submit "${FINAL_PKG}" \
        --apple-id "${APPLE_ID}" \
        --password "${APPLE_PASSWORD}" \
        --team-id "${APPLE_TEAM_ID}" \
        --wait

    info "Stapling notarization ticket..."
    xcrun stapler staple "${FINAL_PKG}"
    info "Notarization complete."
else
    warn "Skipping notarization. Set APPLE_ID, APPLE_PASSWORD, and APPLE_TEAM_ID to enable."
fi

# ---- Done -------------------------------------------------------------------
FILESIZE=$(du -h "${FINAL_PKG}" | cut -f1)
info "=========================================="
info "  VOID macOS Installer generated!"
info "  Path:  ${FINAL_PKG}"
info "  Size:  ${FILESIZE}"
info "  Build: Release / Universal Binary"
echo ""  info "  To install manually from build:"
    info "    sudo cp -R '${VST3_SRC}' '/Library/Audio/Plug-Ins/VST3/'"
    info "    sudo cp -R '${AU_SRC}' '/Library/Audio/Plug-Ins/Components/'"
info "=========================================="
