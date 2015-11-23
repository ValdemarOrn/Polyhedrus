#include <string>
#include <sstream>
#include <vector>
#include "Parameters.h"
#include "Utils.h"
#include "ParameterFormatters.h"

namespace Leiftur
{
	std::map<Module, std::map<int, ParameterInfo>> Parameters::ParamInfo;

	void Parameters::Init()
	{
		// --------------- Osc ----------------------

		auto modMap = std::map<int, ParameterInfo>();

		modMap[(int)OscParameters::Cent] = ParameterInfo((int)OscParameters::Cent, "Cent", nullptr, 0, -50, 50, 
			ParameterFormatters::FormatIntFloor);
		modMap[(int)OscParameters::Keytrack] = ParameterInfo((int)OscParameters::Keytrack, "Keytrack", nullptr, 1, 0, 1, 
			ParameterFormatters::FormatOnOff);
		modMap[(int)OscParameters::Linear] = ParameterInfo((int)OscParameters::Linear, "Linear", nullptr, 0, -1, 1, 
			ParameterFormatters::FormatDecimal2);
		modMap[(int)OscParameters::Octave] = ParameterInfo((int)OscParameters::Octave, "Octave", nullptr, 0, -3, 3, 
			ParameterFormatters::FormatIntFloor);
		modMap[(int)OscParameters::Pan] = ParameterInfo((int)OscParameters::Pan, "Pan", nullptr, 0, -1, 1, 
			ParameterFormatters::FormatPercent);
		modMap[(int)OscParameters::Phase] = ParameterInfo((int)OscParameters::Phase, "Phase", nullptr, 1, 0, 1, 
			ParameterFormatters::FormatPhase);
		modMap[(int)OscParameters::Routing] = ParameterInfo((int)OscParameters::Routing, "Routing", nullptr, (int)RoutingStage::Character, 0, (int)RoutingStage::Direct,
			ParameterFormatters::FormatRouting);
		modMap[(int)OscParameters::Semi] = ParameterInfo((int)OscParameters::Semi, "Semi", nullptr, 0, -12, 12, 
			ParameterFormatters::FormatIntFloor);
		modMap[(int)OscParameters::Shape] = ParameterInfo((int)OscParameters::Shape, "Shape", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatPercentPrecise);
		modMap[(int)OscParameters::Slop] = ParameterInfo((int)OscParameters::Slop, "Slop", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatIntFloor);
		modMap[(int)OscParameters::Volume] = ParameterInfo((int)OscParameters::Volume, "Volume", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatPercent);
		modMap[(int)OscParameters::Waveform] = ParameterInfo((int)OscParameters::Waveform, "Waveform", nullptr, 0, 0, 999999, 
			ParameterFormatters::FormatIntFloor);

		ParamInfo[Module::Osc1] = modMap;
		ParamInfo[Module::Osc2] = modMap;
		ParamInfo[Module::Osc3] = modMap;

		// --------------- Mixer ----------------------

		modMap = std::map<int, ParameterInfo>();

		modMap[(int)MixerParameters::Am12] = ParameterInfo((int)MixerParameters::Am12, "Am12", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatDecimal2);
		modMap[(int)MixerParameters::Am23] = ParameterInfo((int)MixerParameters::Am23, "Am23", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatDecimal2);
		modMap[(int)MixerParameters::Fm12] = ParameterInfo((int)MixerParameters::Fm12, "Fm12", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatDecimal2);
		modMap[(int)MixerParameters::Fm13] = ParameterInfo((int)MixerParameters::Fm13, "Fm13", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatDecimal2);
		modMap[(int)MixerParameters::Fm23] = ParameterInfo((int)MixerParameters::Fm23, "Fm23", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatDecimal2);
		modMap[(int)MixerParameters::Color] = ParameterInfo((int)MixerParameters::Color, "Color", nullptr, 0, 0, 2, 
			ParameterFormatters::FormatNoiseType);
		modMap[(int)MixerParameters::CharacterOut] = ParameterInfo((int)MixerParameters::CharacterOut, "CharacterOut", nullptr, 1, 0, 1,
			ParameterFormatters::FormatPercent);
		modMap[(int)MixerParameters::FilterHpOut] = ParameterInfo((int)MixerParameters::FilterHpOut, "FilterHpOut", nullptr, 1, 0, 1, 
			ParameterFormatters::FormatPercent);
		modMap[(int)MixerParameters::FilterMainOut] = ParameterInfo((int)MixerParameters::FilterMainOut, "FilterMainOut", nullptr, 1, 0, 1,
			ParameterFormatters::FormatPercent);
		modMap[(int)MixerParameters::NoiseRouting] = ParameterInfo((int)MixerParameters::NoiseRouting, "NoiseRouting", nullptr, (int)RoutingStage::Character, 0, (int)RoutingStage::Direct, 
			ParameterFormatters::FormatRouting);
		modMap[(int)MixerParameters::Noise] = ParameterInfo((int)MixerParameters::Noise, "Noise", nullptr, 0, 0, 1,
			ParameterFormatters::FormatPercent);

		ParamInfo[Module::Mixer] = modMap;

		// --------------- Switches ----------------------

		modMap = std::map<int, ParameterInfo>();

		modMap[(int)ModuleSwitchParameters::Osc1On] = ParameterInfo((int)ModuleSwitchParameters::Osc1On, "Osc1On", nullptr, 1, 0, 1, 
			ParameterFormatters::FormatOnOff);
		modMap[(int)ModuleSwitchParameters::Osc2On] = ParameterInfo((int)ModuleSwitchParameters::Osc2On, "Osc2On", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatOnOff);
		modMap[(int)ModuleSwitchParameters::Osc3On] = ParameterInfo((int)ModuleSwitchParameters::Osc3On, "Osc3On", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatOnOff);
		modMap[(int)ModuleSwitchParameters::CharacterOn] = ParameterInfo((int)ModuleSwitchParameters::CharacterOn, "CharacterOn", nullptr, 1, 0, 1, 
			ParameterFormatters::FormatOnOff);
		modMap[(int)ModuleSwitchParameters::FilterHpOn] = ParameterInfo((int)ModuleSwitchParameters::FilterHpOn, "FilterHpOn", nullptr, 1, 0, 1, 
			ParameterFormatters::FormatOnOff);
		modMap[(int)ModuleSwitchParameters::FilterMainOn] = ParameterInfo((int)ModuleSwitchParameters::FilterMainOn, "FilterMainOn", nullptr, 1, 0, 1, 
			ParameterFormatters::FormatOnOff);
		modMap[(int)ModuleSwitchParameters::DriveOn] = ParameterInfo((int)ModuleSwitchParameters::DriveOn, "DriveOn", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatOnOff);
		modMap[(int)ModuleSwitchParameters::Mod1On] = ParameterInfo((int)ModuleSwitchParameters::Mod1On, "Mod1On", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatOnOff);
		modMap[(int)ModuleSwitchParameters::Mod2On] = ParameterInfo((int)ModuleSwitchParameters::Mod2On, "Mod2On", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatOnOff);
		modMap[(int)ModuleSwitchParameters::Mod3On] = ParameterInfo((int)ModuleSwitchParameters::Mod3On, "Mod3On", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatOnOff);
		modMap[(int)ModuleSwitchParameters::ArpOn] = ParameterInfo((int)ModuleSwitchParameters::ArpOn, "ArpOn", nullptr, 0, 0, 1,
			ParameterFormatters::FormatOnOff);
		modMap[(int)ModuleSwitchParameters::ChorusOn] = ParameterInfo((int)ModuleSwitchParameters::ChorusOn, "ChorusOn", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatOnOff);
		modMap[(int)ModuleSwitchParameters::DelayOn] = ParameterInfo((int)ModuleSwitchParameters::DelayOn, "DelayOn", nullptr, 0, 0, 1,
			ParameterFormatters::FormatOnOff);

		ParamInfo[Module::ModuleSwitches] = modMap;

		// ----------------- Character -----------------------

		modMap = std::map<int, ParameterInfo>();

		modMap[(int)CharacterParameters::Bottom] = ParameterInfo((int)CharacterParameters::Bottom, "Bottom", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatPercent);
		modMap[(int)CharacterParameters::Top] = ParameterInfo((int)CharacterParameters::Top, "Top", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatPercent);
		modMap[(int)CharacterParameters::Decimate] = ParameterInfo((int)CharacterParameters::Decimate, "Decimate", nullptr, 0, 0, 1, 
			[](double value) -> std::string { return ParameterFormatters::FormatExp2(value, 4); });
		modMap[(int)CharacterParameters::Reduce] = ParameterInfo((int)CharacterParameters::Reduce, "Reduce", nullptr, 0, 0, 1, 
			[](double value) -> std::string { return ParameterFormatters::FormatExp2(1.0 - value, 4); });
		modMap[(int)CharacterParameters::Clip] = ParameterInfo((int)CharacterParameters::Clip, "Clip", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatPercent);

		ParamInfo[Module::Character] = modMap;

		// --------------- Filter HP ---------------------

		modMap = std::map<int, ParameterInfo>();

		modMap[(int)FilterHpParameters::Cutoff] = ParameterInfo((int)FilterHpParameters::Cutoff, "Cutoff", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatFilterCutoff);
		modMap[(int)FilterHpParameters::Resonance] = ParameterInfo((int)FilterHpParameters::Resonance, "Resonance", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatPercent);
		modMap[(int)FilterHpParameters::Keytrack] = ParameterInfo((int)FilterHpParameters::Keytrack, "Keytrack", nullptr, 0, -1, 1, 
			ParameterFormatters::FormatPercent);
		modMap[(int)FilterHpParameters::Env] = ParameterInfo((int)FilterHpParameters::Env, "Env", nullptr, 0, -1, 1,
			ParameterFormatters::FormatPercent);

		ParamInfo[Module::FilterHp] = modMap;

		// --------------- Filter Main -------------------------

		modMap = std::map<int, ParameterInfo>();

		modMap[(int)FilterMainParameters::Drive] = ParameterInfo((int)FilterMainParameters::Drive, "Drive", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatPercent);
		modMap[(int)FilterMainParameters::Cutoff] = ParameterInfo((int)FilterMainParameters::Cutoff, "Cutoff", nullptr, 1, 0, 1, 
			ParameterFormatters::FormatFilterCutoff);
		modMap[(int)FilterMainParameters::Resonance] = ParameterInfo((int)FilterMainParameters::Resonance, "Resonance", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatPercent);
		modMap[(int)FilterMainParameters::Keytrack] = ParameterInfo((int)FilterMainParameters::Keytrack, "Keytrack", nullptr, 0, -1, 1, 
			ParameterFormatters::FormatPercent);
		modMap[(int)FilterMainParameters::Env] = ParameterInfo((int)FilterMainParameters::Env, "Env", nullptr, 0, -1, 1,
			ParameterFormatters::FormatPercent);
		modMap[(int)FilterMainParameters::Type] = ParameterInfo((int)FilterMainParameters::Type, "Type", nullptr, 0, 0, 6,
			ParameterFormatters::FormatIntFloor);

		ParamInfo[Module::FilterMain] = modMap;

		// ------------------- Drive --------------------------------

		modMap = std::map<int, ParameterInfo>();

		modMap[(int)DriveParameters::Gain] = ParameterInfo((int)DriveParameters::Gain, "Gain", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatPercent);
		modMap[(int)DriveParameters::Bias] = ParameterInfo((int)DriveParameters::Bias, "Bias", nullptr, 0, 0, 1,
			ParameterFormatters::FormatDecimal2);
		modMap[(int)DriveParameters::Post] = ParameterInfo((int)DriveParameters::Post, "Post", nullptr, 1, 0, 1,
			[](double value) -> std::string { return value >= 0.5 ? "Post" : "Pre"; });
		modMap[(int)DriveParameters::Type] = ParameterInfo((int)DriveParameters::Type, "Type", nullptr, 0, 0, 1,
			ParameterFormatters::FormatIntFloor);
		modMap[(int)DriveParameters::Mellow] = ParameterInfo((int)DriveParameters::Mellow, "Mellow", nullptr, 0, 0, 1,
			ParameterFormatters::FormatPercent);

		ParamInfo[Module::Drive] = modMap;

		// ------------------------- Env -------------------------------

		modMap = std::map<int, ParameterInfo>();

		modMap[(int)EnvParameters::Attack] = ParameterInfo((int)EnvParameters::Attack, "Attack", nullptr, 0.1, 0, 1,
			ParameterFormatters::FormatEnvTime);
		modMap[(int)EnvParameters::Hold] = ParameterInfo((int)EnvParameters::Hold, "Hold", nullptr, 0, 0, 1,
			ParameterFormatters::FormatEnvTime);
		modMap[(int)EnvParameters::Decay] = ParameterInfo((int)EnvParameters::Decay, "Decay", nullptr, 0.5, 0, 1,
			ParameterFormatters::FormatEnvTime);
		modMap[(int)EnvParameters::Sustain] = ParameterInfo((int)EnvParameters::Sustain, "Sustain", nullptr, 0.8, 0, 1, 
			ParameterFormatters::FormatPercent);
		modMap[(int)EnvParameters::Release] = ParameterInfo((int)EnvParameters::Release, "Release", nullptr, 0.2, 0, 1,
			ParameterFormatters::FormatEnvTime);
		modMap[(int)EnvParameters::Velocity] = ParameterInfo((int)EnvParameters::Velocity, "Velocity", nullptr, 0, 0, 1,
			ParameterFormatters::FormatPercent);
		modMap[(int)EnvParameters::AttackCurve] = ParameterInfo((int)EnvParameters::AttackCurve, "AttackCurve", nullptr, 0, -1, 1, 
			ParameterFormatters::FormatDecimal2);
		modMap[(int)EnvParameters::DecayCurve] = ParameterInfo((int)EnvParameters::DecayCurve, "DecayCurve", nullptr, -0.5, -1, 1, 
			ParameterFormatters::FormatDecimal2);
		modMap[(int)EnvParameters::ReleaseCurve] = ParameterInfo((int)EnvParameters::ReleaseCurve, "ReleaseCurve", nullptr, -0.5, -1, 1, 
			ParameterFormatters::FormatDecimal2);
		modMap[(int)EnvParameters::Retrigger] = ParameterInfo((int)EnvParameters::Retrigger, "Retrigger", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatOnOff);

		ParamInfo[Module::EnvAmp] = modMap;
		ParamInfo[Module::EnvFilter] = modMap;

		// ------------------------- Mod -------------------------------------

		modMap = std::map<int, ParameterInfo>();

		modMap[(int)ModParameters::Delay] = ParameterInfo((int)ModParameters::Delay, "Delay", nullptr, 0, 0, 1,
			ParameterFormatters::FormatEnvTime);
		modMap[(int)ModParameters::Attack] = ParameterInfo((int)ModParameters::Attack, "Attack", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatEnvTime);
		modMap[(int)ModParameters::Hold] = ParameterInfo((int)ModParameters::Hold, "Hold", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatEnvTime);
		modMap[(int)ModParameters::Decay] = ParameterInfo((int)ModParameters::Decay, "Decay", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatEnvTime);
		modMap[(int)ModParameters::Sustain] = ParameterInfo((int)ModParameters::Sustain, "Sustain", nullptr, 1, 0, 1, 
			ParameterFormatters::FormatPercent);
		modMap[(int)ModParameters::Release] = ParameterInfo((int)ModParameters::Release, "Release", nullptr, 1, 0, 1,
			ParameterFormatters::FormatEnvTime);
		modMap[(int)ModParameters::Phase] = ParameterInfo((int)ModParameters::Phase, "Phase", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatPhase);
		modMap[(int)ModParameters::Freq] = ParameterInfo((int)ModParameters::Freq, "Freq", nullptr, 0.5, 0, 1,
			ParameterFormatters::FormatLfoFreq);
		modMap[(int)ModParameters::Shape] = ParameterInfo((int)ModParameters::Shape, "Shape", nullptr, (int)LfoShape::Triangle, 0, (int)LfoShape::Count - 0.0001,
			ParameterFormatters::FormatIntFloor);
		modMap[(int)ModParameters::Slew] = ParameterInfo((int)ModParameters::Slew, "Slew", nullptr, 0, 0, 1,
			ParameterFormatters::FormatPercent);
		modMap[(int)ModParameters::Steps] = ParameterInfo((int)ModParameters::Steps, "Steps", nullptr, 1, 0, 1,
			ParameterFormatters::FormatPercent);
		modMap[(int)ModParameters::Sync] = ParameterInfo((int)ModParameters::Sync, "Sync", nullptr, 0, 0, 1,
			ParameterFormatters::FormatOnOff);
		modMap[(int)ModParameters::AttackCurve] = ParameterInfo((int)ModParameters::AttackCurve, "AttackCurve", nullptr, 0, -1, 1, 
			ParameterFormatters::FormatDecimal2);
		modMap[(int)ModParameters::DecayCurve] = ParameterInfo((int)ModParameters::DecayCurve, "DecayCurve", nullptr, 0, -1, 1,
			ParameterFormatters::FormatDecimal2);
		modMap[(int)ModParameters::ReleaseCurve] = ParameterInfo((int)ModParameters::ReleaseCurve, "ReleaseCurve", nullptr, 0, -1, 1,
			ParameterFormatters::FormatDecimal2);
		modMap[(int)ModParameters::Retrigger] = ParameterInfo((int)ModParameters::Retrigger, "Retrigger", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatOnOff);

		ParamInfo[Module::Mod1] = modMap;
		ParamInfo[Module::Mod2] = modMap;
		ParamInfo[Module::Mod3] = modMap;

		// ---------------------- Arp ---------------------------

		modMap = std::map<int, ParameterInfo>();

		modMap[(int)ArpParameters::Range] = ParameterInfo((int)ArpParameters::Range, "Range", nullptr, 1, 1, 4.999,
			ParameterFormatters::FormatIntFloor);
		modMap[(int)ArpParameters::NotePtn] = ParameterInfo((int)ArpParameters::NotePtn, "NotePtn", nullptr, 0, 0, 4.999,
			ParameterFormatters::FormatIntFloor);
		modMap[(int)ArpParameters::OctavePtn] = ParameterInfo((int)ArpParameters::OctavePtn, "OctavePtn", nullptr, 0, 0, 4.999,
			ParameterFormatters::FormatIntFloor);
		modMap[(int)ArpParameters::Gate] = ParameterInfo((int)ArpParameters::Gate, "Gate", nullptr, 0, 0, 1,
			ParameterFormatters::FormatPercent);
		modMap[(int)ArpParameters::Divide] = ParameterInfo((int)ArpParameters::Divide, "Divide", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatDecimal2);
		modMap[(int)ArpParameters::Bpm] = ParameterInfo((int)ArpParameters::Bpm, "Bpm", nullptr, 120, 10, 250, 
			ParameterFormatters::FormatIntRounded);
		modMap[(int)ArpParameters::Sync] = ParameterInfo((int)ArpParameters::Sync, "Sync", nullptr, 0, 0, 1,
			ParameterFormatters::FormatOnOff);

		ParamInfo[Module::Arp] = modMap;


	}

	Module Parameters::GetModule(std::string moduleString)
	{
		if (moduleString == "Control") return Module::Control;
		if (moduleString == "Osc1") return Module::Osc1;
		if (moduleString == "Osc2") return Module::Osc2;
		if (moduleString == "Osc3") return Module::Osc3;
		if (moduleString == "Mixer") return Module::Mixer;
		if (moduleString == "ModuleSwitches") return Module::ModuleSwitches;
		if (moduleString == "Character") return Module::Character;
		if (moduleString == "FilterHp") return Module::FilterHp;
		if (moduleString == "FilterMain") return Module::FilterMain;
		if (moduleString == "Drive") return Module::Drive;
		if (moduleString == "EnvAmp") return Module::EnvAmp;
		if (moduleString == "EnvFilter") return Module::EnvFilter;
		if (moduleString == "Mod1") return Module::Mod1;
		if (moduleString == "Mod2") return Module::Mod2;
		if (moduleString == "Mod3") return Module::Mod3;
		if (moduleString == "Arp") return Module::Arp;
		if (moduleString == "Voices") return Module::Voices;
		if (moduleString == "Chorus") return Module::Chorus;
		if (moduleString == "Delay") return Module::Delay;
		if (moduleString == "Macros") return Module::Macros;
		if (moduleString == "ModMatrix") return Module::ModMatrix;

		return Module::Control;
	}

	int Parameters::GetParameter(std::string parameterString, Module module)
	{
		if (module == Module::Osc1 || module == Module::Osc2 || module == Module::Osc3 || module == Module::Mixer
			|| module == Module::ModuleSwitches || module == Module::Character || module == Module::FilterHp 
			|| module == Module::FilterMain || module == Module::Drive || module == Module::EnvAmp || module == Module::EnvFilter
			|| module == Module::Mod1 || module == Module::Mod2 || module == Module::Mod3 || module == Module::Arp)
		{
			for (auto param : ParamInfo[module])
			{
				if (param.second.ParameterName == parameterString)
					return param.second.ParameterIndex;
			}
		}
		else if (module == Module::Voices)
		{
			if (parameterString == "Detune") return (int)VoiceParameters::Detune;
			if (parameterString == "Spread") return (int)VoiceParameters::Spread;
			if (parameterString == "Glide") return (int)VoiceParameters::Glide;
			if (parameterString == "Bend") return (int)VoiceParameters::Bend;
			if (parameterString == "Master") return (int)VoiceParameters::Master;
			if (parameterString == "HiQuality") return (int)VoiceParameters::HiQuality;
			if (parameterString == "Polyphony") return (int)VoiceParameters::Polyphony;
			if (parameterString == "Unison") return (int)VoiceParameters::Unison;
			if (parameterString == "VoiceMode") return (int)VoiceParameters::VoiceMode;
		}
		else if (module == Module::Chorus)
		{
			if (parameterString == "Depth1") return (int)ChorusParameters::Depth1;
			if (parameterString == "Depth2") return (int)ChorusParameters::Depth2;
			if (parameterString == "Enable1") return (int)ChorusParameters::Enable1;
			if (parameterString == "Enable2") return (int)ChorusParameters::Enable2;
			if (parameterString == "Quality") return (int)ChorusParameters::Quality;
			if (parameterString == "Rate1") return (int)ChorusParameters::Rate1;
			if (parameterString == "Rate2") return (int)ChorusParameters::Rate2;
			if (parameterString == "Wet") return (int)ChorusParameters::Wet;
			if (parameterString == "Width") return (int)ChorusParameters::Width;
		}
		else if (module == Module::Delay)
		{
			if (parameterString == "DelayL") return (int)DelayParameters::DelayL;
			if (parameterString == "DelayR") return (int)DelayParameters::DelayR;
			if (parameterString == "FeedbackL") return (int)DelayParameters::FeedbackL;
			if (parameterString == "FeedbackR") return (int)DelayParameters::FeedbackR;
			if (parameterString == "Lowpass") return (int)DelayParameters::Lowpass;
			if (parameterString == "Highpass") return (int)DelayParameters::Highpass;
			if (parameterString == "Saturate") return (int)DelayParameters::Saturate;
			if (parameterString == "DiffuseAmount") return (int)DelayParameters::DiffuseAmount;
			if (parameterString == "DiffuseSize") return (int)DelayParameters::DiffuseSize;
			if (parameterString == "Wet") return (int)DelayParameters::Wet;
			if (parameterString == "Sync") return (int)DelayParameters::Sync;
		}
		else if (module == Module::Macros)
		{
			if (parameterString == "Macro1") return (int)MacroParameters::Macro1;
			if (parameterString == "Macro2") return (int)MacroParameters::Macro2;
			if (parameterString == "Macro3") return (int)MacroParameters::Macro3;
			if (parameterString == "Macro4") return (int)MacroParameters::Macro4;
			if (parameterString == "Macro5") return (int)MacroParameters::Macro5;
			if (parameterString == "Macro6") return (int)MacroParameters::Macro6;
			if (parameterString == "Macro7") return (int)MacroParameters::Macro7;
			if (parameterString == "Macro8") return (int)MacroParameters::Macro8;
		}
		else if (module == Module::ModMatrix)
		{
			if (parameterString == "Source_1") return (int)ModMatrixParameters::Source_1;
			if (parameterString == "Dest_1") return (int)ModMatrixParameters::Dest_1;
			if (parameterString == "Via_1") return (int)ModMatrixParameters::Via_1;
			if (parameterString == "Amount_1") return (int)ModMatrixParameters::Amount_1;
			if (parameterString == "ViaAmount_1") return (int)ModMatrixParameters::ViaAmount_1;
			if (parameterString == "Source_2") return (int)ModMatrixParameters::Source_2;
			if (parameterString == "Dest_2") return (int)ModMatrixParameters::Dest_2;
			if (parameterString == "Via_2") return (int)ModMatrixParameters::Via_2;
			if (parameterString == "Amount_2") return (int)ModMatrixParameters::Amount_2;
			if (parameterString == "ViaAmount_2") return (int)ModMatrixParameters::ViaAmount_2;
			if (parameterString == "Source_3") return (int)ModMatrixParameters::Source_3;
			if (parameterString == "Dest_3") return (int)ModMatrixParameters::Dest_3;
			if (parameterString == "Via_3") return (int)ModMatrixParameters::Via_3;
			if (parameterString == "Amount_3") return (int)ModMatrixParameters::Amount_3;
			if (parameterString == "ViaAmount_3") return (int)ModMatrixParameters::ViaAmount_3;
			if (parameterString == "Source_4") return (int)ModMatrixParameters::Source_4;
			if (parameterString == "Dest_4") return (int)ModMatrixParameters::Dest_4;
			if (parameterString == "Via_4") return (int)ModMatrixParameters::Via_4;
			if (parameterString == "Amount_4") return (int)ModMatrixParameters::Amount_4;
			if (parameterString == "ViaAmount_4") return (int)ModMatrixParameters::ViaAmount_4;
			if (parameterString == "Source_5") return (int)ModMatrixParameters::Source_5;
			if (parameterString == "Dest_5") return (int)ModMatrixParameters::Dest_5;
			if (parameterString == "Via_5") return (int)ModMatrixParameters::Via_5;
			if (parameterString == "Amount_5") return (int)ModMatrixParameters::Amount_5;
			if (parameterString == "ViaAmount_5") return (int)ModMatrixParameters::ViaAmount_5;
			if (parameterString == "Source_6") return (int)ModMatrixParameters::Source_6;
			if (parameterString == "Dest_6") return (int)ModMatrixParameters::Dest_6;
			if (parameterString == "Via_6") return (int)ModMatrixParameters::Via_6;
			if (parameterString == "Amount_6") return (int)ModMatrixParameters::Amount_6;
			if (parameterString == "ViaAmount_6") return (int)ModMatrixParameters::ViaAmount_6;
			if (parameterString == "Source_7") return (int)ModMatrixParameters::Source_7;
			if (parameterString == "Dest_7") return (int)ModMatrixParameters::Dest_7;
			if (parameterString == "Via_7") return (int)ModMatrixParameters::Via_7;
			if (parameterString == "Amount_7") return (int)ModMatrixParameters::Amount_7;
			if (parameterString == "ViaAmount_7") return (int)ModMatrixParameters::ViaAmount_7;
			if (parameterString == "Source_8") return (int)ModMatrixParameters::Source_8;
			if (parameterString == "Dest_8") return (int)ModMatrixParameters::Dest_8;
			if (parameterString == "Via_8") return (int)ModMatrixParameters::Via_8;
			if (parameterString == "Amount_8") return (int)ModMatrixParameters::Amount_8;
			if (parameterString == "ViaAmount_8") return (int)ModMatrixParameters::ViaAmount_8;
			if (parameterString == "Source_9") return (int)ModMatrixParameters::Source_9;
			if (parameterString == "Dest_9") return (int)ModMatrixParameters::Dest_9;
			if (parameterString == "Via_9") return (int)ModMatrixParameters::Via_9;
			if (parameterString == "Amount_9") return (int)ModMatrixParameters::Amount_9;
			if (parameterString == "ViaAmount_9") return (int)ModMatrixParameters::ViaAmount_9;
			if (parameterString == "Source_10") return (int)ModMatrixParameters::Source_10;
			if (parameterString == "Dest_10") return (int)ModMatrixParameters::Dest_10;
			if (parameterString == "Via_10") return (int)ModMatrixParameters::Via_10;
			if (parameterString == "Amount_10") return (int)ModMatrixParameters::Amount_10;
			if (parameterString == "ViaAmount_10") return (int)ModMatrixParameters::ViaAmount_10;
			if (parameterString == "Source_11") return (int)ModMatrixParameters::Source_11;
			if (parameterString == "Dest_11") return (int)ModMatrixParameters::Dest_11;
			if (parameterString == "Via_11") return (int)ModMatrixParameters::Via_11;
			if (parameterString == "Amount_11") return (int)ModMatrixParameters::Amount_11;
			if (parameterString == "ViaAmount_11") return (int)ModMatrixParameters::ViaAmount_11;
			if (parameterString == "Source_12") return (int)ModMatrixParameters::Source_12;
			if (parameterString == "Dest_12") return (int)ModMatrixParameters::Dest_12;
			if (parameterString == "Via_12") return (int)ModMatrixParameters::Via_12;
			if (parameterString == "Amount_12") return (int)ModMatrixParameters::Amount_12;
			if (parameterString == "ViaAmount_12") return (int)ModMatrixParameters::ViaAmount_12;
			if (parameterString == "Source_13") return (int)ModMatrixParameters::Source_13;
			if (parameterString == "Dest_13") return (int)ModMatrixParameters::Dest_13;
			if (parameterString == "Via_13") return (int)ModMatrixParameters::Via_13;
			if (parameterString == "Amount_13") return (int)ModMatrixParameters::Amount_13;
			if (parameterString == "ViaAmount_13") return (int)ModMatrixParameters::ViaAmount_13;
			if (parameterString == "Source_14") return (int)ModMatrixParameters::Source_14;
			if (parameterString == "Dest_14") return (int)ModMatrixParameters::Dest_14;
			if (parameterString == "Via_14") return (int)ModMatrixParameters::Via_14;
			if (parameterString == "Amount_14") return (int)ModMatrixParameters::Amount_14;
			if (parameterString == "ViaAmount_14") return (int)ModMatrixParameters::ViaAmount_14;
			if (parameterString == "Source_15") return (int)ModMatrixParameters::Source_15;
			if (parameterString == "Dest_15") return (int)ModMatrixParameters::Dest_15;
			if (parameterString == "Via_15") return (int)ModMatrixParameters::Via_15;
			if (parameterString == "Amount_15") return (int)ModMatrixParameters::Amount_15;
			if (parameterString == "ViaAmount_15") return (int)ModMatrixParameters::ViaAmount_15;
			if (parameterString == "Source_16") return (int)ModMatrixParameters::Source_16;
			if (parameterString == "Dest_16") return (int)ModMatrixParameters::Dest_16;
			if (parameterString == "Via_16") return (int)ModMatrixParameters::Via_16;
			if (parameterString == "Amount_16") return (int)ModMatrixParameters::Amount_16;
			if (parameterString == "ViaAmount_16") return (int)ModMatrixParameters::ViaAmount_16;
		}

		return 0;
	}

	void Parameters::ParseAddress(std::string address, Module* module, int* parameter)
	{
		address = std::string(&address[1]);

		auto parts = SplitString(address, '/');
		if (parts.size() != 2)
		{
			*module = Module::Control;
			*parameter = 0;
			return;
		}

		auto moduleString = parts[0];
		auto parameterString = parts[1];

		*module = GetModule(moduleString);
		*parameter = GetParameter(parameterString, *module);
	}
}
