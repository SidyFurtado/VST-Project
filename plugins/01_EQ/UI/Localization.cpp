#include "Localization.h"

namespace AUREQ
{
    namespace
    {
        static juce::String utf8 (const char* text)
        {
            return juce::String (juce::CharPointer_UTF8 (text));
        }

        juce::String pt (TextKey key)
        {
            switch (key)
            {
                case TextKey::Settings: return "Config.";
                case TextKey::Theme: return "Tema";
                case TextKey::Bypass: return "Bypass";
                case TextKey::Reset: return "Reset";
                case TextKey::AddBand: return "+ Banda";
                case TextKey::Remove: return "Remover";
                case TextKey::SaveCurrentPreset: return "Salvar Preset Atual...";
                case TextKey::Genres: return utf8 ("Gêneros");
                case TextKey::BySource: return "Por Fonte";
                case TextKey::UserPresets: return utf8 ("Presets do Usuário");
                case TextKey::NoUserPresets: return utf8 ("Sem Presets do Usuário");
                case TextKey::RefreshUserPresets: return utf8 ("Atualizar Presets do Usuário");
                case TextKey::RevealPresetsFolder: return "Mostrar Pasta de Presets";
                case TextKey::DeleteUserPreset: return utf8 ("Apagar Preset do Usuário...");
                case TextKey::RevealPresetsFolderFailed: return "Falha ao Mostrar Pasta";
                case TextKey::RevealPresetsFolderFailedMessage: return utf8 ("A pasta de presets do usuário não pode ser aberta.");
                case TextKey::SavePresetFailed: return "Falha ao Salvar Preset";
                case TextKey::SavePresetFailedMessage: return utf8 ("O preset não pode ser salvo. Tente outro nome.");
                case TextKey::SaveCurrentPresetTitle: return "Salvar Preset Atual";
                case TextKey::SaveCurrentPresetMessage: return utf8 ("Digite um nome para este preset do usuário.");
                case TextKey::Name: return "Nome:";
                case TextKey::Save: return "Salvar";
                case TextKey::Cancel: return "Cancelar";
                case TextKey::DeleteUserPresetTitle: return utf8 ("Apagar Preset do Usuário");
                case TextKey::DeleteUserPresetMessagePrefix: return "Mover \"";
                case TextKey::DeleteUserPresetMessageSuffix: return "\" para a Lixeira?";
                case TextKey::Delete: return "Apagar";
                case TextKey::DeletePresetFailed: return "Falha ao Apagar Preset";
                case TextKey::DeletePresetFailedMessage: return utf8 ("O preset do usuário não pode ser movido para a Lixeira.");
                case TextKey::Ok: return "OK";
                case TextKey::Undo: return "Desfazer";
                case TextKey::Redo: return "Refazer";
                case TextKey::Language: return "Idioma";
                case TextKey::PortugueseBR: return utf8 ("Português (Brasil)");
                case TextKey::English: return "English";
                case TextKey::CopyAToB: return "Copiar A para B";
                case TextKey::CopyBToA: return "Copiar B para A";
                case TextKey::CopyCurrentToOther: return "Copiar Atual para o Outro";
                case TextKey::ResetBToCurrent: return "Resetar B com o Atual";
                case TextKey::InputGain: return "ENTRADA";
                case TextKey::OutputGain: return utf8 ("SAÍDA");
                case TextKey::Band: return "BANDA";
                case TextKey::Frequency: return "FREQ";
                case TextKey::Freq: return "FREQ";
                case TextKey::Gain: return "GANHO";
                case TextKey::QFactor: return "Q";
                case TextKey::Range: return "ALCANCE";
                case TextKey::Threshold: return "LIMIAR";
                case TextKey::Thresh: return "LIMIAR";
                case TextKey::Attack: return "ATAQUE";
                case TextKey::Release: return "LIBER.";
                case TextKey::Slope: return "INCLIN.";
                case TextKey::FilterType: return "TIPO DE FILTRO";
                case TextKey::ChannelMode: return "MODO CANAL";
                case TextKey::DynamicEQ: return "EQ DIN.";
                case TextKey::SelectBandToEdit: return utf8 ("SELECIONE UMA BANDA NO GRÁFICO");
                case TextKey::FooterProduct: return "AUREQ Equalizador";
                case TextKey::Type: return "Tipo";
                case TextKey::Active: return "ATIVA";
                case TextKey::On: return "ON";
                case TextKey::Off: return "OFF";
                case TextKey::NotAvailable: return "N/A";
                case TextKey::ChannelShort: return "CH:";
                case TextKey::Bell: return "Bell";
                case TextKey::LowCut: return "Corte de Graves";
                case TextKey::HighCut: return "Corte de Agudos";
                case TextKey::LowShelf: return "Shelf Grave";
                case TextKey::HighShelf: return "Shelf Agudo";
                case TextKey::Notch: return "Notch";
                case TextKey::BandPass: return "Passa-Banda";
                case TextKey::Stereo: return utf8 ("Estéreo");
                case TextKey::Mid: return "Mid";
                case TextKey::Side: return "Side";
                case TextKey::Left: return "Esquerda";
                case TextKey::Right: return "Direita";
                case TextKey::Vocals: return "Vocais";
                case TextKey::Drums: return "Bateria";
                case TextKey::Bass808: return "Bass / 808";
                case TextKey::Instruments: return "Instrumentos";
                case TextKey::MixBusMaster: return "Mix Bus / Master";
                case TextKey::UtilityCleanup: return "Utilidade / Limpeza";
                case TextKey::Vocal: return "Vocal";
                case TextKey::RapVocal: return "Rap Vocal";
                case TextKey::LeadVocal: return "Vocal Principal";
                case TextKey::BackingVocal: return "Backing Vocal";
                case TextKey::Choir: return "Coro";
                case TextKey::Adlibs: return "Adlibs";
                case TextKey::Accordion: return "Sanfona";
                case TextKey::Zabumba: return "Zabumba";
                case TextKey::Bass: return "Baixo";
                case TextKey::Kick: return "Kick";
                case TextKey::Snare: return "Caixa";
                case TextKey::SnareClap: return "Caixa / Clap";
                case TextKey::HiHat: return "Hi-Hat";
                case TextKey::Percussion: return utf8 ("Percussão");
                case TextKey::Beat: return "Beat";
                case TextKey::BeatBus: return "Beat Bus";
                case TextKey::DrumBus: return "Drum Bus";
                case TextKey::Sample: return "Sample";
                case TextKey::ElectricGuitar: return "Guitarra";
                case TextKey::AcousticGuitar: return utf8 ("Violão");
                case TextKey::Guitar: return "Guitarra";
                case TextKey::Viola: return "Viola";
                case TextKey::PianoKeys: return "Piano / Teclas";
                case TextKey::Synth: return "Synth";
                case TextKey::Lead: return "Lead";
                case TextKey::LeadSynth: return "Lead / Synth";
                case TextKey::Pad: return "Pad";
                case TextKey::MixBus: return "Mix Bus";
                case TextKey::Master: return "Master";
            }

            return {};
        }

        juce::String en (TextKey key)
        {
            switch (key)
            {
                case TextKey::Settings: return "Settings";
                case TextKey::Theme: return "Theme";
                case TextKey::Bypass: return "Bypass";
                case TextKey::Reset: return "Reset";
                case TextKey::AddBand: return "+ Band";
                case TextKey::Remove: return "Remove";
                case TextKey::SaveCurrentPreset: return "Save Current Preset...";
                case TextKey::Genres: return "Genres";
                case TextKey::BySource: return "By Source";
                case TextKey::UserPresets: return "User Presets";
                case TextKey::NoUserPresets: return "No User Presets";
                case TextKey::RefreshUserPresets: return "Refresh User Presets";
                case TextKey::RevealPresetsFolder: return "Reveal Presets Folder";
                case TextKey::DeleteUserPreset: return "Delete User Preset...";
                case TextKey::RevealPresetsFolderFailed: return "Reveal Presets Folder Failed";
                case TextKey::RevealPresetsFolderFailedMessage: return "The user presets folder could not be opened.";
                case TextKey::SavePresetFailed: return "Save Preset Failed";
                case TextKey::SavePresetFailedMessage: return "The preset could not be saved. Please try a different name.";
                case TextKey::SaveCurrentPresetTitle: return "Save Current Preset";
                case TextKey::SaveCurrentPresetMessage: return "Enter a name for this user preset.";
                case TextKey::Name: return "Name:";
                case TextKey::Save: return "Save";
                case TextKey::Cancel: return "Cancel";
                case TextKey::DeleteUserPresetTitle: return "Delete User Preset";
                case TextKey::DeleteUserPresetMessagePrefix: return "Move \"";
                case TextKey::DeleteUserPresetMessageSuffix: return "\" to the Trash?";
                case TextKey::Delete: return "Delete";
                case TextKey::DeletePresetFailed: return "Delete Preset Failed";
                case TextKey::DeletePresetFailedMessage: return "The user preset could not be moved to the Trash.";
                case TextKey::Ok: return "OK";
                case TextKey::Undo: return "Undo";
                case TextKey::Redo: return "Redo";
                case TextKey::Language: return "Language";
                case TextKey::PortugueseBR: return "Portuguese (Brazil)";
                case TextKey::English: return "English";
                case TextKey::CopyAToB: return "Copy A to B";
                case TextKey::CopyBToA: return "Copy B to A";
                case TextKey::CopyCurrentToOther: return "Copy Current to Other";
                case TextKey::ResetBToCurrent: return "Reset B to Current";
                case TextKey::InputGain: return "INPUT GAIN";
                case TextKey::OutputGain: return "OUTPUT GAIN";
                case TextKey::Band: return "BAND";
                case TextKey::Frequency: return "FREQUENCY";
                case TextKey::Freq: return "FREQ";
                case TextKey::Gain: return "GAIN";
                case TextKey::QFactor: return "Q FACTOR";
                case TextKey::Range: return "RANGE";
                case TextKey::Threshold: return "THRESHOLD";
                case TextKey::Thresh: return "THRESH";
                case TextKey::Attack: return "ATTACK";
                case TextKey::Release: return "RELEASE";
                case TextKey::Slope: return "SLOPE";
                case TextKey::FilterType: return "FILTER TYPE";
                case TextKey::ChannelMode: return "CHANNEL MODE";
                case TextKey::DynamicEQ: return "DYNAMIC EQ";
                case TextKey::SelectBandToEdit: return "SELECT A BAND ON THE GRAPH TO EDIT";
                case TextKey::FooterProduct: return "AUREQ Flagship Equalizer";
                case TextKey::Type: return "Type";
                case TextKey::Active: return "ACTIVE";
                case TextKey::On: return "ON";
                case TextKey::Off: return "OFF";
                case TextKey::NotAvailable: return "N/A";
                case TextKey::ChannelShort: return "CH:";
                case TextKey::Bell: return "Bell";
                case TextKey::LowCut: return "Low Cut";
                case TextKey::HighCut: return "High Cut";
                case TextKey::LowShelf: return "Low Shelf";
                case TextKey::HighShelf: return "High Shelf";
                case TextKey::Notch: return "Notch";
                case TextKey::BandPass: return "Band Pass";
                case TextKey::Stereo: return "Stereo";
                case TextKey::Mid: return "Mid";
                case TextKey::Side: return "Side";
                case TextKey::Left: return "Left";
                case TextKey::Right: return "Right";
                case TextKey::Vocals: return "Vocals";
                case TextKey::Drums: return "Drums";
                case TextKey::Bass808: return "Bass / 808";
                case TextKey::Instruments: return "Instruments";
                case TextKey::MixBusMaster: return "Mix Bus / Master";
                case TextKey::UtilityCleanup: return "Utility / Cleanup";
                case TextKey::Vocal: return "Vocal";
                case TextKey::RapVocal: return "Rap Vocal";
                case TextKey::LeadVocal: return "Lead Vocal";
                case TextKey::BackingVocal: return "Backing Vocal";
                case TextKey::Choir: return "Choir";
                case TextKey::Adlibs: return "Adlibs";
                case TextKey::Accordion: return "Accordion";
                case TextKey::Zabumba: return "Zabumba";
                case TextKey::Bass: return "Bass";
                case TextKey::Kick: return "Kick";
                case TextKey::Snare: return "Snare";
                case TextKey::SnareClap: return "Snare / Clap";
                case TextKey::HiHat: return "Hi-Hat";
                case TextKey::Percussion: return "Percussion";
                case TextKey::Beat: return "Beat";
                case TextKey::BeatBus: return "Beat Bus";
                case TextKey::DrumBus: return "Drum Bus";
                case TextKey::Sample: return "Sample";
                case TextKey::ElectricGuitar: return "Electric Guitar";
                case TextKey::AcousticGuitar: return "Acoustic Guitar";
                case TextKey::Guitar: return "Guitar";
                case TextKey::Viola: return "Viola";
                case TextKey::PianoKeys: return "Piano / Keys";
                case TextKey::Synth: return "Synth";
                case TextKey::Lead: return "Lead";
                case TextKey::LeadSynth: return "Lead / Synth";
                case TextKey::Pad: return "Pad";
                case TextKey::MixBus: return "Mix Bus";
                case TextKey::Master: return "Master";
            }

            return {};
        }
    }

    juce::String getText (TextKey key, AureqLanguage language)
    {
        return language == AureqLanguage::PortugueseBR ? pt (key) : en (key);
    }

    juce::String getFilterTypeText (int type, AureqLanguage language)
    {
        switch (type)
        {
            case 0: return getText (TextKey::Bell, language);
            case 1: return getText (TextKey::LowCut, language);
            case 2: return getText (TextKey::HighCut, language);
            case 3: return getText (TextKey::LowShelf, language);
            case 4: return getText (TextKey::HighShelf, language);
            case 5: return getText (TextKey::Notch, language);
            case 6: return getText (TextKey::BandPass, language);
            default: break;
        }

        return getText (TextKey::Bell, language);
    }

    juce::String getChannelModeText (int mode, AureqLanguage language)
    {
        switch (mode)
        {
            case 1: return getText (TextKey::Mid, language);
            case 2: return getText (TextKey::Side, language);
            case 3: return getText (TextKey::Left, language);
            case 4: return getText (TextKey::Right, language);
            default: break;
        }

        return getText (TextKey::Stereo, language);
    }

    juce::String getCategoryText (const juce::String& category, AureqLanguage language)
    {
        if (category.equalsIgnoreCase ("Vocals")) return getText (TextKey::Vocals, language);
        if (category.equalsIgnoreCase ("Drums")) return getText (TextKey::Drums, language);
        if (category.equalsIgnoreCase ("Bass / 808")) return getText (TextKey::Bass808, language);
        if (category.equalsIgnoreCase ("Instruments")) return getText (TextKey::Instruments, language);
        if (category.equalsIgnoreCase ("Mix Bus / Master")) return getText (TextKey::MixBusMaster, language);
        if (category.equalsIgnoreCase ("Dynamic EQ")) return getText (TextKey::DynamicEQ, language);
        if (category.equalsIgnoreCase ("Utility / Cleanup")) return getText (TextKey::UtilityCleanup, language);
        if (category.equalsIgnoreCase ("Genres")) return getText (TextKey::Genres, language);

        return category;
    }

    juce::String getSourceText (const juce::String& source, AureqLanguage language)
    {
        if (source.equalsIgnoreCase ("vocal")) return getText (TextKey::Vocal, language);
        if (source.equalsIgnoreCase ("rap vocal")) return getText (TextKey::RapVocal, language);
        if (source.equalsIgnoreCase ("lead vocal")) return getText (TextKey::LeadVocal, language);
        if (source.equalsIgnoreCase ("backing vocal")) return getText (TextKey::BackingVocal, language);
        if (source.equalsIgnoreCase ("choir")) return getText (TextKey::Choir, language);
        if (source.equalsIgnoreCase ("adlibs")) return getText (TextKey::Adlibs, language);
        if (source.equalsIgnoreCase ("accordion")) return getText (TextKey::Accordion, language);
        if (source.equalsIgnoreCase ("zabumba")) return getText (TextKey::Zabumba, language);
        if (source.equalsIgnoreCase ("808")) return "808";
        if (source.equalsIgnoreCase ("bass / 808")) return getText (TextKey::Bass808, language);
        if (source.equalsIgnoreCase ("bass")) return getText (TextKey::Bass, language);
        if (source.equalsIgnoreCase ("kick")) return getText (TextKey::Kick, language);
        if (source.equalsIgnoreCase ("snare")) return getText (TextKey::Snare, language);
        if (source.equalsIgnoreCase ("snare / clap")) return getText (TextKey::SnareClap, language);
        if (source.equalsIgnoreCase ("hi-hat")) return getText (TextKey::HiHat, language);
        if (source.equalsIgnoreCase ("percussion")) return getText (TextKey::Percussion, language);
        if (source.equalsIgnoreCase ("beat")) return getText (TextKey::Beat, language);
        if (source.equalsIgnoreCase ("beat bus")) return getText (TextKey::BeatBus, language);
        if (source.equalsIgnoreCase ("drum bus")) return getText (TextKey::DrumBus, language);
        if (source.equalsIgnoreCase ("drums")) return getText (TextKey::Drums, language);
        if (source.equalsIgnoreCase ("sample")) return getText (TextKey::Sample, language);
        if (source.equalsIgnoreCase ("electric guitar")) return getText (TextKey::ElectricGuitar, language);
        if (source.equalsIgnoreCase ("acoustic guitar")) return getText (TextKey::AcousticGuitar, language);
        if (source.equalsIgnoreCase ("guitar")) return getText (TextKey::Guitar, language);
        if (source.equalsIgnoreCase ("viola")) return getText (TextKey::Viola, language);
        if (source.equalsIgnoreCase ("piano / keys")) return getText (TextKey::PianoKeys, language);
        if (source.equalsIgnoreCase ("synth")) return getText (TextKey::Synth, language);
        if (source.equalsIgnoreCase ("lead")) return getText (TextKey::Lead, language);
        if (source.equalsIgnoreCase ("lead / synth")) return getText (TextKey::LeadSynth, language);
        if (source.equalsIgnoreCase ("pad")) return getText (TextKey::Pad, language);
        if (source.equalsIgnoreCase ("mix bus")) return getText (TextKey::MixBus, language);
        if (source.equalsIgnoreCase ("master")) return getText (TextKey::Master, language);

        return source;
    }

    juce::String languageToPreferenceValue (AureqLanguage language)
    {
        return language == AureqLanguage::PortugueseBR ? "pt-BR" : "en";
    }

    AureqLanguage languageFromPreferenceValue (const juce::String& value)
    {
        if (value.equalsIgnoreCase ("en") || value.equalsIgnoreCase ("english"))
            return AureqLanguage::English;

        return AureqLanguage::PortugueseBR;
    }
}
