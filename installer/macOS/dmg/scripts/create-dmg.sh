#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../../.." && pwd)"
DMG_DIR="${ROOT_DIR}/installer/macOS/dmg"
STAGING_DIR="${DMG_DIR}/staging"
OUTPUT_DIR="${DMG_DIR}/output"
VOL_NAME="AUREQ 0.9.0-rc1 Installer"
TEMP_DMG="${OUTPUT_DIR}/AUREQ-0.9.0-rc1-macOS-plugin-only.temp.dmg"
FINAL_DMG="${OUTPUT_DIR}/AUREQ-0.9.0-rc1-macOS-plugin-only.dmg"
MOUNT_DIR="/Volumes/${VOL_NAME}"

PKG_SOURCE="${ROOT_DIR}/installer/macOS/plugin-only/output/AUREQ-0.9.0-rc1-macOS-plugin-only.pkg"
PKG_STAGED="${STAGING_DIR}/AUREQ Installer.pkg"
README_STAGED="${STAGING_DIR}/README - Install AUREQ.txt"
ICON_SOURCE="${ROOT_DIR}/assets/brand/macOS/AUREQ.icns"
ICON_STAGED="${STAGING_DIR}/AUREQ.icns"
BACKGROUND_SOURCE="${ROOT_DIR}/assets/installer/previews/aureq-installer-background-preview.png"
BACKGROUND_STAGED="${STAGING_DIR}/.background/aureq-dmg-background.png"

echo "Preparing AUREQ DMG staging..."
mkdir -p "${STAGING_DIR}/.background" "${OUTPUT_DIR}"
cp -f "${PKG_SOURCE}" "${PKG_STAGED}"
cp -f "${ICON_SOURCE}" "${ICON_STAGED}"
cp -f "${BACKGROUND_SOURCE}" "${BACKGROUND_STAGED}"
test -f "${README_STAGED}"

echo "Creating read-write DMG..."
hdiutil create \
  -volname "${VOL_NAME}" \
  -srcfolder "${STAGING_DIR}" \
  -ov \
  -format UDRW \
  "${TEMP_DMG}"

echo "Mounting read-write DMG for optional Finder polish..."
ATTACH_OUTPUT="$(hdiutil attach "${TEMP_DMG}" -readwrite -noverify -noautoopen)"
echo "${ATTACH_OUTPUT}"

if [ ! -d "${MOUNT_DIR}" ]; then
  echo "Expected mount point not found: ${MOUNT_DIR}" >&2
  exit 1
fi

set +e
osascript <<APPLESCRIPT
tell application "Finder"
  tell disk "${VOL_NAME}"
    open
    set current view of container window to icon view
    set toolbar visible of container window to false
    set statusbar visible of container window to false
    set the bounds of container window to {120, 120, 920, 660}
    set theViewOptions to the icon view options of container window
    set arrangement of theViewOptions to not arranged
    set icon size of theViewOptions to 96
    set background picture of theViewOptions to file ".background:aureq-dmg-background.png"
    set position of item "AUREQ Installer.pkg" of container window to {245, 245}
    set position of item "README - Install AUREQ.txt" of container window to {555, 245}
    set position of item "AUREQ.icns" of container window to {555, 405}
    update without registering applications
    delay 1
    close
  end tell
end tell
APPLESCRIPT
POLISH_STATUS=$?
set -e

if [ "${POLISH_STATUS}" -eq 0 ]; then
  echo "Finder polish completed."
else
  echo "Finder polish failed; continuing with functional DMG."
fi

echo "Detaching read-write DMG..."
hdiutil detach "${MOUNT_DIR}"

echo "Converting to compressed read-only DMG..."
hdiutil convert "${TEMP_DMG}" \
  -format UDZO \
  -imagekey zlib-level=9 \
  -ov \
  -o "${FINAL_DMG}"

echo "Removing temporary DMG..."
rm -f "${TEMP_DMG}"

echo "Done: ${FINAL_DMG}"
