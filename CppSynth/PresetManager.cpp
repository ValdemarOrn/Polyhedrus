#include "PresetManager.h"
#include "Parameters.h"
#include "Synth.h"
#include "Modulator.h"
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

		for (auto kvp : Parameters::ParamInfo)
		{
			auto module = kvp.first;

			for (auto param : Parameters::ParamInfo[module])
			{
				preset.Values[Synth::PackParameter(module, param.first)] = param.second.DefaultValue;
			}
		}

		preset.Values[Synth::PackParameter(Module::Osc1, (int)OscParameters::Volume)] = 1.0;
		preset.Values[Synth::PackParameter(Module::EnvAmp, (int)EnvParameters::VelocityCurve)] = -0.65;
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
