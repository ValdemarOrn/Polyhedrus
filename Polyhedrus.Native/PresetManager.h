#ifndef POLYHEDRUS_PRESET_MANAGER
#define POLYHEDRUS_PRESET_MANAGER

#include <string>
#include <map>
#include <vector>

namespace Polyhedrus
{
	class Preset
	{
	public:
		std::string BankName;
		std::string PresetName;
		std::string FilePath;

		std::map<std::string, std::string> Metadata;
		std::map<int, double> Values;

		std::string Serialize();
	};

	class PresetManager
	{
	public:
		const static char* Osc1Waveform;
		const static char* Osc2Waveform;
		const static char* Osc3Waveform;

		std::string BaseDirectory;

		std::map<std::string, std::vector<Preset>> PresetBanks;

		PresetManager();
		void Initialize(std::string baseDirectory);
		Preset GetDefaultPreset();
		Preset GetPreset(std::string bankName, std::string presetName);
		std::string GetBankString();
		std::string GetPresetString(std::string bankName);
		void LoadBanks();
		void LoadPresetsByBank(std::string bankName);
		Preset ReadPresetFile(std::string bankName, std::string presetFilepath);
		void SavePreset(Preset* preset);
	};
}

#endif

