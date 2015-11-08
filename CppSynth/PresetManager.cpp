#include "PresetManager.h"
#include "Parameters.h"
#include "Synth.h"
#include "Lfo.h"
#include "ModMatrix.h"
#include "ModSourceDest.h"
#include "Utils.h"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <fstream>
#include <map>
#include <iostream>

using namespace boost::filesystem;
using namespace std;

namespace Leiftur
{
	std::string Preset::Serialize()
	{
		stringstream ss;
		ss << "BankName=" << this->BankName << ";";
		ss << "PresetName=" << this->PresetName << ";";
		ss << "FilePath=" << this->FilePath << ";";

		for (auto kvp : Metadata)
		{
			ss << kvp.first << "=" << kvp.second << ";";
		}

		return ss.str();
	}

	const char* PresetManager::Osc1Waveform = "_osc1.waveform";
	const char* PresetManager::Osc2Waveform = "_osc2.waveform";
	const char* PresetManager::Osc3Waveform = "_osc3.waveform";

	PresetManager::PresetManager()
	{
	}

	void PresetManager::Initialize(std::string baseDirectory)
	{
		path path = baseDirectory;
		path = path / "Presets";
		BaseDirectory = path.string();
		create_directories(path);
		LoadBanks();
		
		for (auto bank : PresetBanks)
			LoadPresetsByBank(bank.first);
	}

	Preset PresetManager::GetDefaultPreset()
	{
		Preset preset;
		preset.BankName = "Default";
		preset.FilePath = "";
		preset.Metadata["Author"] = "Default";
		preset.PresetName = "Init Preset";

		auto setOsc = [&](Module module)
		{
			preset.Values[Synth::PackParameter(module, (int)OscParameters::Cent)] = 0;
			preset.Values[Synth::PackParameter(module, (int)OscParameters::Octave)] = 0;
			preset.Values[Synth::PackParameter(module, (int)OscParameters::Pan)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)OscParameters::Phase)] = 1.0;
			preset.Values[Synth::PackParameter(module, (int)OscParameters::Routing)] = (int)RoutingStage::Character;
			preset.Values[Synth::PackParameter(module, (int)OscParameters::Semi)] = 0;
			preset.Values[Synth::PackParameter(module, (int)OscParameters::Shape)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)OscParameters::Slop)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)OscParameters::Volume)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)OscParameters::Waveform)] = 0;
		};

		auto setMixer = [&](Module module)
		{
			preset.Values[Synth::PackParameter(module, (int)MixerParameters::Am12)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)MixerParameters::Am23)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)MixerParameters::Color)] = 0;
			preset.Values[Synth::PackParameter(module, (int)MixerParameters::Fm12)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)MixerParameters::Fm13)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)MixerParameters::Fm23)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)MixerParameters::Noise)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)MixerParameters::Output)] = 1.0;
			preset.Values[Synth::PackParameter(module, (int)MixerParameters::NoiseRouting)] = (int)RoutingStage::Character;
		};

		auto setSwitches = [&](Module module)
		{
			preset.Values[Synth::PackParameter(module, (int)ModuleSwitchParameters::ArpOn)] = 0;
			preset.Values[Synth::PackParameter(module, (int)ModuleSwitchParameters::CharacterOn)] = 1;
			preset.Values[Synth::PackParameter(module, (int)ModuleSwitchParameters::ChorusOn)] = 0;
			preset.Values[Synth::PackParameter(module, (int)ModuleSwitchParameters::DelayOn)] = 0;
			preset.Values[Synth::PackParameter(module, (int)ModuleSwitchParameters::DriveOn)] = 0;
			preset.Values[Synth::PackParameter(module, (int)ModuleSwitchParameters::FilterHpOn)] = 1;
			preset.Values[Synth::PackParameter(module, (int)ModuleSwitchParameters::FilterMainOn)] = 1;
			preset.Values[Synth::PackParameter(module, (int)ModuleSwitchParameters::Mod1On)] = 1;
			preset.Values[Synth::PackParameter(module, (int)ModuleSwitchParameters::Mod2On)] = 0;
			preset.Values[Synth::PackParameter(module, (int)ModuleSwitchParameters::Mod3On)] = 0;
			preset.Values[Synth::PackParameter(module, (int)ModuleSwitchParameters::Osc1On)] = 1;
			preset.Values[Synth::PackParameter(module, (int)ModuleSwitchParameters::Osc2On)] = 0;
			preset.Values[Synth::PackParameter(module, (int)ModuleSwitchParameters::Osc3On)] = 0;
		};

		auto setCharacter = [&](Module module)
		{
			preset.Values[Synth::PackParameter(module, (int)CharacterParameters::Bottom)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)CharacterParameters::Clip)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)CharacterParameters::Decimate)] = 0;
			preset.Values[Synth::PackParameter(module, (int)CharacterParameters::Reduce)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)CharacterParameters::Top)] = 0.0;
		};

		auto setFilterHp = [&](Module module)
		{
			preset.Values[Synth::PackParameter(module, (int)FilterHpParameters::Cutoff)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)FilterHpParameters::Env)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)FilterHpParameters::Keytrack)] = 0;
			preset.Values[Synth::PackParameter(module, (int)FilterHpParameters::Resonance)] = 0.0;
		};

		auto setFilterMain = [&](Module module)
		{
			preset.Values[Synth::PackParameter(module, (int)FilterMainParameters::Drive)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)FilterMainParameters::Cutoff)] = 1.0;
			preset.Values[Synth::PackParameter(module, (int)FilterMainParameters::Env)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)FilterMainParameters::Keytrack)] = 0;
			preset.Values[Synth::PackParameter(module, (int)FilterMainParameters::Resonance)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)FilterMainParameters::Type)] = 0;
		};

		auto setDrive = [&](Module module)
		{
			preset.Values[Synth::PackParameter(module, (int)DriveParameters::Bias)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)DriveParameters::Gain)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)DriveParameters::Mellow)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)DriveParameters::Post)] = 1;
			preset.Values[Synth::PackParameter(module, (int)DriveParameters::Type)] = 0;
		};

		auto setEnv = [&](Module module)
		{
			preset.Values[Synth::PackParameter(module, (int)EnvParameters::Attack)] = 0.1;
			preset.Values[Synth::PackParameter(module, (int)EnvParameters::AttackCurve)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)EnvParameters::Decay)] = 0.5;
			preset.Values[Synth::PackParameter(module, (int)EnvParameters::DecayCurve)] = -0.5;
			preset.Values[Synth::PackParameter(module, (int)EnvParameters::Hold)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)EnvParameters::Release)] = 0.2;
			preset.Values[Synth::PackParameter(module, (int)EnvParameters::ReleaseCurve)] = -0.5;
			preset.Values[Synth::PackParameter(module, (int)EnvParameters::Retrigger)] = 0;
			preset.Values[Synth::PackParameter(module, (int)EnvParameters::Sustain)] = 0.8;
			preset.Values[Synth::PackParameter(module, (int)EnvParameters::Velocity)] = 0.0;
		};

		auto setMod = [&](Module module)
		{
			preset.Values[Synth::PackParameter(module, (int)ModParameters::Delay)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)ModParameters::Attack)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)ModParameters::Hold)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)ModParameters::Decay)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)ModParameters::Sustain)] = 1.0;
			preset.Values[Synth::PackParameter(module, (int)ModParameters::Release)] = 1.0;

			preset.Values[Synth::PackParameter(module, (int)ModParameters::Phase)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)ModParameters::Freq)] = 0.5;
			preset.Values[Synth::PackParameter(module, (int)ModParameters::Shape)] = (int)LfoShape::Triangle;
			preset.Values[Synth::PackParameter(module, (int)ModParameters::Slew)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)ModParameters::Steps)] = 1.0;
			preset.Values[Synth::PackParameter(module, (int)ModParameters::Sync)] = 0.0;

			preset.Values[Synth::PackParameter(module, (int)ModParameters::AttackCurve)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)ModParameters::DecayCurve)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)ModParameters::ReleaseCurve)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)ModParameters::Retrigger)] = 0;
		};

		auto setArp = [&](Module module)
		{
			preset.Values[Synth::PackParameter(module, (int)ArpParameters::Bpm)] = 120.0;
			preset.Values[Synth::PackParameter(module, (int)ArpParameters::Divide)] = 16.0;
			preset.Values[Synth::PackParameter(module, (int)ArpParameters::Gate)] = 0.99;
			preset.Values[Synth::PackParameter(module, (int)ArpParameters::NotePtn)] = 0;
			preset.Values[Synth::PackParameter(module, (int)ArpParameters::OctavePtn)] = 0;
			preset.Values[Synth::PackParameter(module, (int)ArpParameters::Range)] = 3;
			preset.Values[Synth::PackParameter(module, (int)ArpParameters::Sync)] = 0;
		};

		auto setChorus = [&](Module module)
		{
			preset.Values[Synth::PackParameter(module, (int)ChorusParameters::Enable1)] = 1;
			preset.Values[Synth::PackParameter(module, (int)ChorusParameters::Enable2)] = 1;
			preset.Values[Synth::PackParameter(module, (int)ChorusParameters::Rate1)] = 0.3123;
			preset.Values[Synth::PackParameter(module, (int)ChorusParameters::Rate2)] = 0.6687;
			preset.Values[Synth::PackParameter(module, (int)ChorusParameters::Depth1)] = 0.4;
			preset.Values[Synth::PackParameter(module, (int)ChorusParameters::Depth2)] = 0.6;
			preset.Values[Synth::PackParameter(module, (int)ChorusParameters::Width)] = 1.0;
			preset.Values[Synth::PackParameter(module, (int)ChorusParameters::Quality)] = 0.5;
			preset.Values[Synth::PackParameter(module, (int)ChorusParameters::Wet)] = 0.5;
		};

		auto setDelay = [&](Module module)
		{
			preset.Values[Synth::PackParameter(module, (int)DelayParameters::DelayL)] = 0.5;
			preset.Values[Synth::PackParameter(module, (int)DelayParameters::DelayR)] = 0.5;
			preset.Values[Synth::PackParameter(module, (int)DelayParameters::FeedbackL)] = 0.5;
			preset.Values[Synth::PackParameter(module, (int)DelayParameters::FeedbackR)] = 0.5;
			preset.Values[Synth::PackParameter(module, (int)DelayParameters::Lowpass)] = 1.0;
			preset.Values[Synth::PackParameter(module, (int)DelayParameters::Highpass)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)DelayParameters::Saturate)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)DelayParameters::Crossfeed)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)DelayParameters::Diffuse)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)DelayParameters::Wet)] = 0.5;
			preset.Values[Synth::PackParameter(module, (int)DelayParameters::Sync)] = 0;
		};

		auto setMacros = [&](Module module)
		{
			preset.Values[Synth::PackParameter(module, (int)MacroParameters::Macro1)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)MacroParameters::Macro2)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)MacroParameters::Macro3)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)MacroParameters::Macro4)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)MacroParameters::Macro5)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)MacroParameters::Macro6)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)MacroParameters::Macro7)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)MacroParameters::Macro8)] = 0.0;
		};

		auto setVoices = [&](Module module)
		{
			preset.Values[Synth::PackParameter(module, (int)VoiceParameters::Bend)] = 2.0;
			preset.Values[Synth::PackParameter(module, (int)VoiceParameters::Detune)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)VoiceParameters::Glide)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)VoiceParameters::Master)] = 1.0;
			preset.Values[Synth::PackParameter(module, (int)VoiceParameters::Polyphony)] = 6;
			preset.Values[Synth::PackParameter(module, (int)VoiceParameters::Spread)] = 0.0;
			preset.Values[Synth::PackParameter(module, (int)VoiceParameters::Unison)] = 1;
			preset.Values[Synth::PackParameter(module, (int)VoiceParameters::VoiceMode)] = (int)VoiceMode::PolyRoundRobin;
		};

		auto setModMatrix = [&](Module module)
		{
			for (int i = 0; i < ModMatrix::MatrixCount; i++)
			{
				preset.Values[Synth::PackParameter(module, 10 * (1 + i) + 0)] = (int)ModSource::Off; // Source
				preset.Values[Synth::PackParameter(module, 10 * (1 + i) + 1)] = (int)ModDest::Off; // Dest
				preset.Values[Synth::PackParameter(module, 10 * (1 + i) + 2)] = (int)ModDest::Off; // Via
				preset.Values[Synth::PackParameter(module, 10 * (1 + i) + 3)] = 0.0; // Amount
				preset.Values[Synth::PackParameter(module, 10 * (1 + i) + 4)] = 0.0; // Via Amount
			}
		};

		setOsc(Module::Osc1);
		setOsc(Module::Osc2);
		setOsc(Module::Osc3);
		setMixer(Module::Mixer);
		setSwitches(Module::ModuleSwitches);
		setCharacter(Module::Character);
		setFilterHp(Module::FilterHp);
		setFilterMain(Module::FilterMain);
		setDrive(Module::Drive);
		setEnv(Module::EnvAmp);
		setEnv(Module::EnvFilter);
		setMod(Module::Mod1);
		setMod(Module::Mod2);
		setMod(Module::Mod3);
		setArp(Module::Arp);
		setChorus(Module::Chorus);
		setDelay(Module::Delay);
		setMacros(Module::Macros);
		setVoices(Module::Voices);
		setModMatrix(Module::ModMatrix);

		preset.Values[Synth::PackParameter(Module::Osc1, (int)OscParameters::Volume)] = 1.0;

		return preset;
	}

	Preset PresetManager::GetPreset(std::string bankName, std::string presetName)
	{
		if (PresetBanks.find(bankName) == PresetBanks.end())
			return GetDefaultPreset();

		for (auto preset : PresetBanks[bankName])
		{
			if (preset.PresetName == presetName)
				return preset;
		}
		
		return GetDefaultPreset();
	}

	std::string PresetManager::GetBankString()
	{
		stringstream ss;

		for (auto bank : PresetBanks)
		{
			if (bank.second.size() == 0)
				continue;

			ss << bank.first << ";";
		}
		
		return ss.str();
	}

	std::string PresetManager::GetPresetString(std::string bankName)
	{
		if (PresetBanks.find(bankName) == PresetBanks.end())
			return "";

		stringstream ss;
		for (auto preset : PresetBanks[bankName])
		{
			ss << preset.PresetName << ";";
		}

		return ss.str();
	}

	void PresetManager::LoadBanks()
	{
		PresetBanks.clear();
		directory_iterator end_itr;

		for (directory_iterator itr(BaseDirectory); itr != end_itr; ++itr)
		{
			if (is_directory(itr->status()))
			{
				auto bank = itr->path();
				auto file = bank.filename().string();
				PresetBanks[file];
			}
		}
	}
	
	void PresetManager::LoadPresetsByBank(std::string bankName)
	{
		path bankDir = BaseDirectory;
		bankDir = bankDir / bankName;
		directory_iterator end_itr;
		PresetBanks[bankName].clear();

		for (directory_iterator itr(bankDir); itr != end_itr; ++itr)
		{
			auto ext = itr->path().extension();
			if (ext == ".preset")
			{
				auto preset = ReadPresetFile(bankName, itr->path().string());
				PresetBanks[bankName].push_back(preset);
			}
		}
	}

	Preset PresetManager::ReadPresetFile(std::string bankName, std::string presetFilepath)
	{
		try
		{
			Preset preset;
			preset.BankName = bankName;
			preset.FilePath = presetFilepath;
			
			map<string, string> metadata;
			ifstream infile(presetFilepath);
			bool headerComplete = false;

			std::string line;
			while (std::getline(infile, line))
			{
				if (line == "")
				{
					headerComplete = true;
					continue;
				}

				auto kvp = SplitString(line, ':');
				auto key = kvp.at(0);
				auto value = kvp.at(1);

				if (!headerComplete)
				{
					if (key == "PresetName")
						preset.PresetName = value;
					else
						preset.Metadata[key] = value;
				}
				else
				{
					int intKey = stoi(key);
					double dVal = stod(value);
					preset.Values[intKey] = dVal;
				}
			}

			return preset;
		}
		catch (exception ex)
		{
			cout << "Failed to parse preset file: " << presetFilepath << endl;
			cout << ex.what() << endl;
			return GetDefaultPreset();
		}
	}

	void PresetManager::SavePreset(Preset* preset)
	{
		// Create the contents
		stringstream ss;

		ss << "PresetName:" << preset->PresetName << std::endl;
		for (auto kvp : preset->Metadata)
			ss << kvp.first << ":" << kvp.second << std::endl;
		
		ss << std::endl;

		for (auto kvp : preset->Values)
			ss << kvp.first << ":" << kvp.second << std::endl;

		string data = ss.str();

		// Write the file
		path dir(BaseDirectory);
		dir = dir / preset->BankName;
		auto filepath = dir / (preset->PresetName + ".preset");
		create_directories(dir);

		fstream fs;
		fs.open(filepath.string(), ios::out);
		fs << data;
		fs.flush();
		fs.close();

		preset->FilePath = filepath.string();

		// reload the list of presets in the bank
		LoadPresetsByBank(preset->BankName);
	}
}
