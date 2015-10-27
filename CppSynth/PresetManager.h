#ifndef LEIFTUR_PRESET_MANAGER
#define LEIFTUR_PRESET_MANAGER

#include <string>
#include <map>
#include <vector>

namespace Leiftur
{
	class Preset
	{
	public:
		std::string BankName;
		std::string PresetName;
		std::string FilePath;

		std::map<std::string, std::string> Metadata;
		std::map<int, double> Values;
	};

	class PresetManager
	{
	public:
		std::string BaseDirectory;

		std::map<std::string, std::vector<Preset>> PresetBanks;

		PresetManager();
		void Initialize(std::string baseDirectory);
		Preset GetDefaultPreset();
		void LoadBanks();
		void LoadPresetsByBank(std::string bank);
		Preset ReadPresetFile(std::string bank, std::string presetFilepath);
		void SavePreset(Preset* preset);
	};
}

#endif

