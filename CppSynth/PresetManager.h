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
		std::string Filename;

		bool ValuesLoaded;
		std::map<std::string, std::string> Metadata;
		std::map<int, double> Values;
	};

	class PresetManager
	{
	public:
		std::map<std::string, std::map<std::string, Preset>> PresetBanks;

		PresetManager();
		void Initialize(std::string baseDirectory);
		Preset GetDefaultPreset();
		void LoadPresetValues(Preset* preset);
		void SavePreset(Preset* preset);
	};
}

#endif

