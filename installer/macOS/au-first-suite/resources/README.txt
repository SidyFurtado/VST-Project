ASTRA Audio Suite 0.9.0-rc1 - macOS AU Installer

Official public download:

https://github.com/SidyFurtado/VST-Project/releases/download/v1.0.0/ASTRA-Audio-Suite-0.9.0-rc1-macOS-AU.pkg

This package installs only the Audio Unit versions of:

- AUREQ.component
- LUMINAR.component
- GRAVITY.component

Install destination:

/Library/Audio/Plug-Ins/Components/

This AU-first beta package does not install VST3 plugins and does not install Standalone applications.

After installation, AU validation can be performed with:

auval -v aufx Aurq AStr
auval -v aufx Lmnr AStr
auval -v aufx Grvt AStr

This beta is unsigned and not notarized.

---
IMPORTANT NOTE FOR UPGRADING USERS:
If you have previously installed testing versions of these plugins (which used the old developer signature 'Manu'), you MUST clean up old user-level components to avoid AU validation errors:

1. Delete the user-level duplicates:
   rm -rf ~/Library/Audio/Plug-Ins/Components/AUREQ.component
   rm -rf ~/Library/Audio/Plug-Ins/Components/LUMINAR.component
   rm -rf ~/Library/Audio/Plug-Ins/Components/GRAVITY.component

2. Clear the AudioUnit Cache and restart the registration daemon:
   rm -f ~/Library/Caches/AudioUnitCache/com.apple.audiounits.cache
   killall -9 AudioComponentRegistrar
