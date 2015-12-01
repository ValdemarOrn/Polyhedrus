#include <string>
#include <sstream>
#include <vector>
#include "Parameters.h"
#include "Utils.h"
#include "ParameterFormatters.h"
#include "SynthDefines.h"
#include "ModSourceDest.h"

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
		modMap[(int)MixerParameters::Color] = ParameterInfo((int)MixerParameters::Color, "Color", nullptr, 0, 0, 2.999, 
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

		modMap[(int)FilterHpParameters::Cutoff] = ParameterInfo((int)FilterHpParameters::Cutoff, "Cutoff", nullptr, 0, 0, 9.5, 
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
		modMap[(int)FilterMainParameters::Cutoff] = ParameterInfo((int)FilterMainParameters::Cutoff, "Cutoff", nullptr, 10.3, 0, 10.3, 
			ParameterFormatters::FormatFilterCutoff);
		modMap[(int)FilterMainParameters::Resonance] = ParameterInfo((int)FilterMainParameters::Resonance, "Resonance", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatPercent);
		modMap[(int)FilterMainParameters::Keytrack] = ParameterInfo((int)FilterMainParameters::Keytrack, "Keytrack", nullptr, 0, -1, 1, 
			ParameterFormatters::FormatPercent);
		modMap[(int)FilterMainParameters::Env] = ParameterInfo((int)FilterMainParameters::Env, "Env", nullptr, 0, -1, 1,
			ParameterFormatters::FormatPercent);
		modMap[(int)FilterMainParameters::Mode] = ParameterInfo((int)FilterMainParameters::Mode, "Mode", nullptr, 0, 0, 14,
			ParameterFormatters::FormatIntFloor);
		modMap[(int)FilterMainParameters::Type] = ParameterInfo((int)FilterMainParameters::Type, "Type", nullptr, 0, 0, 6,
			ParameterFormatters::FormatIntFloor);

		ParamInfo[Module::FilterMain] = modMap;

		// ------------------- Drive --------------------------------

		modMap = std::map<int, ParameterInfo>();

		modMap[(int)DriveParameters::Gain] = ParameterInfo((int)DriveParameters::Gain, "Gain", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatPercent);
		modMap[(int)DriveParameters::Bias] = ParameterInfo((int)DriveParameters::Bias, "Bias", nullptr, 0, -1, 1,
			ParameterFormatters::FormatDecimal2);
		modMap[(int)DriveParameters::Volume] = ParameterInfo((int)DriveParameters::Volume, "Volume", nullptr, 1, 0, 1,
			ParameterFormatters::FormatPercent);
		modMap[(int)DriveParameters::Post] = ParameterInfo((int)DriveParameters::Post, "Post", nullptr, 1, 0, 1,
			[](double value) -> std::string { return value >= 0.5 ? "Post" : "Pre"; });
		modMap[(int)DriveParameters::Type] = ParameterInfo((int)DriveParameters::Type, "Type", nullptr, 1, 0, 4,
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
			ParameterFormatters::FormatModulatorSteps);
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

		// -------------------- Voices ---------------------------

		modMap = std::map<int, ParameterInfo>();

		modMap[(int)VoiceParameters::Detune] = ParameterInfo((int)VoiceParameters::Detune, "Detune", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatPercent);
		modMap[(int)VoiceParameters::Spread] = ParameterInfo((int)VoiceParameters::Spread, "Spread", nullptr, 0, 0, 1,
			ParameterFormatters::FormatPercent);
		modMap[(int)VoiceParameters::Glide] = ParameterInfo((int)VoiceParameters::Glide, "Glide", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatPercent);
		modMap[(int)VoiceParameters::Bend] = ParameterInfo((int)VoiceParameters::Bend, "Bend", nullptr, 2, 0, 24, 
			ParameterFormatters::FormatIntFloor);
		modMap[(int)VoiceParameters::Master] = ParameterInfo((int)VoiceParameters::Master, "Master", nullptr, 0.5, 0, 1,
			ParameterFormatters::FormatDecimal3);
		modMap[(int)VoiceParameters::HiQuality] = ParameterInfo((int)VoiceParameters::HiQuality, "HiQuality", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatPercent);
		modMap[(int)VoiceParameters::Polyphony] = ParameterInfo((int)VoiceParameters::Polyphony, "Polyphony", nullptr, 1, 1, 32,
			ParameterFormatters::FormatIntFloor);
		modMap[(int)VoiceParameters::Unison] = ParameterInfo((int)VoiceParameters::Unison, "Unison", nullptr, 1, 1, 32,
			ParameterFormatters::FormatIntFloor);
		modMap[(int)VoiceParameters::VoiceMode] = ParameterInfo((int)VoiceParameters::VoiceMode, "VoiceMode", nullptr, (int)VoiceMode::PolyRoundRobin, 0, (int)VoiceMode::PolyRoundRobin,
			ParameterFormatters::FormatIntFloor);

		ParamInfo[Module::Voices] = modMap;

		// ------------------- Chorus ------------------------

		modMap = std::map<int, ParameterInfo>();

		modMap[(int)ChorusParameters::Enable1] = ParameterInfo((int)ChorusParameters::Enable1, "Enable1", nullptr, 1, 0, 1, 
			ParameterFormatters::FormatOnOff);
		modMap[(int)ChorusParameters::Enable2] = ParameterInfo((int)ChorusParameters::Enable2, "Enable2", nullptr, 1, 0, 1, 
			ParameterFormatters::FormatOnOff);
		modMap[(int)ChorusParameters::Rate1] = ParameterInfo((int)ChorusParameters::Rate1, "Rate1", nullptr, 0.3123, 0, 1,
			ParameterFormatters::FormatLfoFreq);
		modMap[(int)ChorusParameters::Rate2] = ParameterInfo((int)ChorusParameters::Rate2, "Rate2", nullptr, 0.6687, 0, 1,
			ParameterFormatters::FormatLfoFreq);
		modMap[(int)ChorusParameters::Depth1] = ParameterInfo((int)ChorusParameters::Depth1, "Depth1", nullptr, 0.4, 0, 1, 
			ParameterFormatters::FormatPercent);
		modMap[(int)ChorusParameters::Depth2] = ParameterInfo((int)ChorusParameters::Depth2, "Depth2", nullptr, 0.6, 0, 1, 
			ParameterFormatters::FormatPercent);
		modMap[(int)ChorusParameters::Width] = ParameterInfo((int)ChorusParameters::Width, "Width", nullptr, 1, 0, 1, 
			ParameterFormatters::FormatPercent);
		modMap[(int)ChorusParameters::Quality] = ParameterInfo((int)ChorusParameters::Quality, "Quality", nullptr, 0.5, 0, 1,
			ParameterFormatters::FormatPercent);
		modMap[(int)ChorusParameters::Wet] = ParameterInfo((int)ChorusParameters::Wet, "Wet", nullptr, 0.5, 0, 1,
			ParameterFormatters::FormatPercent);

		ParamInfo[Module::Chorus] = modMap;

		// -------------------- Delay ---------------------------

		modMap = std::map<int, ParameterInfo>();

		modMap[(int)DelayParameters::DelayL] = ParameterInfo((int)DelayParameters::DelayL, "DelayL", nullptr, 0.8, 0, 1, 
			ParameterFormatters::FormatPercent);
		modMap[(int)DelayParameters::DelayR] = ParameterInfo((int)DelayParameters::DelayR, "DelayR", nullptr, 0.8, 0, 1, 
			ParameterFormatters::FormatPercent);
		modMap[(int)DelayParameters::FeedbackL] = ParameterInfo((int)DelayParameters::FeedbackL, "FeedbackL", nullptr, 0.5, 0, 1, 
			ParameterFormatters::FormatPercent);
		modMap[(int)DelayParameters::FeedbackR] = ParameterInfo((int)DelayParameters::FeedbackR, "FeedbackR", nullptr, 0.5, 0, 1, 
			ParameterFormatters::FormatPercent);
		modMap[(int)DelayParameters::Lowpass] = ParameterInfo((int)DelayParameters::Lowpass, "Lowpass", nullptr, 1, 0, 1, 
			ParameterFormatters::FormatPercent);
		modMap[(int)DelayParameters::Highpass] = ParameterInfo((int)DelayParameters::Highpass, "Highpass", nullptr, 0, 0, 1,
			ParameterFormatters::FormatPercent);
		modMap[(int)DelayParameters::Saturate] = ParameterInfo((int)DelayParameters::Saturate, "Saturate", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatPercent);
		modMap[(int)DelayParameters::DiffuseAmount] = ParameterInfo((int)DelayParameters::DiffuseAmount, "DiffuseAmount", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatPercent);
		modMap[(int)DelayParameters::DiffuseSize] = ParameterInfo((int)DelayParameters::DiffuseSize, "DiffuseSize", nullptr, 0.5, 0, 1, 
			ParameterFormatters::FormatPercent);
		modMap[(int)DelayParameters::Wet] = ParameterInfo((int)DelayParameters::Wet, "Wet", nullptr, 0.3, 0, 1, 
			ParameterFormatters::FormatPercent);
		modMap[(int)DelayParameters::Sync] = ParameterInfo((int)DelayParameters::Sync, "Sync", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatOnOff);

		ParamInfo[Module::Delay] = modMap;

		// --------------- Macros ---------------------

		modMap = std::map<int, ParameterInfo>();

		modMap[(int)MacroParameters::Macro1] = ParameterInfo((int)MacroParameters::Macro1, "Macro1", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatDecimal2);
		modMap[(int)MacroParameters::Macro2] = ParameterInfo((int)MacroParameters::Macro2, "Macro2", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatDecimal2);
		modMap[(int)MacroParameters::Macro3] = ParameterInfo((int)MacroParameters::Macro3, "Macro3", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatDecimal2);
		modMap[(int)MacroParameters::Macro4] = ParameterInfo((int)MacroParameters::Macro4, "Macro4", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatDecimal2);
		modMap[(int)MacroParameters::Macro5] = ParameterInfo((int)MacroParameters::Macro5, "Macro5", nullptr, 0, 0, 1,
			ParameterFormatters::FormatDecimal2);
		modMap[(int)MacroParameters::Macro6] = ParameterInfo((int)MacroParameters::Macro6, "Macro6", nullptr, 0, 0, 1,
			ParameterFormatters::FormatDecimal2);
		modMap[(int)MacroParameters::Macro7] = ParameterInfo((int)MacroParameters::Macro7, "Macro7", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatDecimal2);
		modMap[(int)MacroParameters::Macro8] = ParameterInfo((int)MacroParameters::Macro8, "Macro8", nullptr, 0, 0, 1, 
			ParameterFormatters::FormatDecimal2);

		ParamInfo[Module::Macros] = modMap;

		// --------------------- Matrix ---------------------------

		modMap = std::map<int, ParameterInfo>();

		modMap[(int)ModMatrixParameters::Source_1] = ParameterInfo((int)ModMatrixParameters::Source_1, "Source_1", nullptr, 0, 0, (int)ModSource::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Dest_1] = ParameterInfo((int)ModMatrixParameters::Dest_1, "Dest_1", nullptr, 0, 0, (int)ModDest::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Via_1] = ParameterInfo((int)ModMatrixParameters::Via_1, "Via_1", nullptr, 0, 0, (int)ModSource::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Amount_1] = ParameterInfo((int)ModMatrixParameters::Amount_1, "Amount_1", nullptr, 0, -1, 1, ParameterFormatters::FormatDecimal3);
		modMap[(int)ModMatrixParameters::ViaAmount_1] = ParameterInfo((int)ModMatrixParameters::ViaAmount_1, "ViaAmount_1", nullptr, 0, -1, 1, ParameterFormatters::FormatDecimal3);
		
		modMap[(int)ModMatrixParameters::Source_2] = ParameterInfo((int)ModMatrixParameters::Source_2, "Source_2", nullptr, 0, 0, (int)ModSource::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Dest_2] = ParameterInfo((int)ModMatrixParameters::Dest_2, "Dest_2", nullptr, 0, 0, (int)ModDest::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Via_2] = ParameterInfo((int)ModMatrixParameters::Via_2, "Via_2", nullptr, 0, 0, (int)ModSource::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Amount_2] = ParameterInfo((int)ModMatrixParameters::Amount_2, "Amount_2", nullptr, 0, -1, 1, ParameterFormatters::FormatDecimal3);
		modMap[(int)ModMatrixParameters::ViaAmount_2] = ParameterInfo((int)ModMatrixParameters::ViaAmount_2, "ViaAmount_2", nullptr, 0, -1, 1, ParameterFormatters::FormatDecimal3);
		
		modMap[(int)ModMatrixParameters::Source_3] = ParameterInfo((int)ModMatrixParameters::Source_3, "Source_3", nullptr, 0, 0, (int)ModSource::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Dest_3] = ParameterInfo((int)ModMatrixParameters::Dest_3, "Dest_3", nullptr, 0, 0, (int)ModDest::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Via_3] = ParameterInfo((int)ModMatrixParameters::Via_3, "Via_3", nullptr, 0, 0, (int)ModSource::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Amount_3] = ParameterInfo((int)ModMatrixParameters::Amount_3, "Amount_3", nullptr, 0, -1, 1, ParameterFormatters::FormatDecimal3);
		modMap[(int)ModMatrixParameters::ViaAmount_3] = ParameterInfo((int)ModMatrixParameters::ViaAmount_3, "ViaAmount_3", nullptr, 0, -1, 1, ParameterFormatters::FormatDecimal3);
		
		modMap[(int)ModMatrixParameters::Source_4] = ParameterInfo((int)ModMatrixParameters::Source_4, "Source_4", nullptr, 0, 0, (int)ModSource::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Dest_4] = ParameterInfo((int)ModMatrixParameters::Dest_4, "Dest_4", nullptr, 0, 0, (int)ModDest::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Via_4] = ParameterInfo((int)ModMatrixParameters::Via_4, "Via_4", nullptr, 0, 0, (int)ModSource::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Amount_4] = ParameterInfo((int)ModMatrixParameters::Amount_4, "Amount_4", nullptr, 0, -1, 1, ParameterFormatters::FormatDecimal3);
		modMap[(int)ModMatrixParameters::ViaAmount_4] = ParameterInfo((int)ModMatrixParameters::ViaAmount_4, "ViaAmount_4", nullptr, 0, -1, 1, ParameterFormatters::FormatDecimal3);
		
		modMap[(int)ModMatrixParameters::Source_5] = ParameterInfo((int)ModMatrixParameters::Source_5, "Source_5", nullptr, 0, 0, (int)ModSource::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Dest_5] = ParameterInfo((int)ModMatrixParameters::Dest_5, "Dest_5", nullptr, 0, 0, (int)ModDest::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Via_5] = ParameterInfo((int)ModMatrixParameters::Via_5, "Via_5", nullptr, 0, 0, (int)ModSource::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Amount_5] = ParameterInfo((int)ModMatrixParameters::Amount_5, "Amount_5", nullptr, 0, -1, 1, ParameterFormatters::FormatDecimal3);
		modMap[(int)ModMatrixParameters::ViaAmount_5] = ParameterInfo((int)ModMatrixParameters::ViaAmount_5, "ViaAmount_5", nullptr, 0, -1, 1, ParameterFormatters::FormatDecimal3);
		
		modMap[(int)ModMatrixParameters::Source_6] = ParameterInfo((int)ModMatrixParameters::Source_6, "Source_6", nullptr, 0, 0, (int)ModSource::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Dest_6] = ParameterInfo((int)ModMatrixParameters::Dest_6, "Dest_6", nullptr, 0, 0, (int)ModDest::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Via_6] = ParameterInfo((int)ModMatrixParameters::Via_6, "Via_6", nullptr, 0, 0, (int)ModSource::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Amount_6] = ParameterInfo((int)ModMatrixParameters::Amount_6, "Amount_6", nullptr, 0, -1, 1, ParameterFormatters::FormatDecimal3);
		modMap[(int)ModMatrixParameters::ViaAmount_6] = ParameterInfo((int)ModMatrixParameters::ViaAmount_6, "ViaAmount_6", nullptr, 0, -1, 1, ParameterFormatters::FormatDecimal3);
		
		modMap[(int)ModMatrixParameters::Source_7] = ParameterInfo((int)ModMatrixParameters::Source_7, "Source_7", nullptr, 0, 0, (int)ModSource::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Dest_7] = ParameterInfo((int)ModMatrixParameters::Dest_7, "Dest_7", nullptr, 0, 0, (int)ModDest::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Via_7] = ParameterInfo((int)ModMatrixParameters::Via_7, "Via_7", nullptr, 0, 0, (int)ModSource::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Amount_7] = ParameterInfo((int)ModMatrixParameters::Amount_7, "Amount_7", nullptr, 0, -1, 1, ParameterFormatters::FormatDecimal3);
		modMap[(int)ModMatrixParameters::ViaAmount_7] = ParameterInfo((int)ModMatrixParameters::ViaAmount_7, "ViaAmount_7", nullptr, 0, -1, 1, ParameterFormatters::FormatDecimal3);
		
		modMap[(int)ModMatrixParameters::Source_8] = ParameterInfo((int)ModMatrixParameters::Source_8, "Source_8", nullptr, 0, 0, (int)ModSource::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Dest_8] = ParameterInfo((int)ModMatrixParameters::Dest_8, "Dest_8", nullptr, 0, 0, (int)ModDest::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Via_8] = ParameterInfo((int)ModMatrixParameters::Via_8, "Via_8", nullptr, 0, 0, (int)ModSource::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Amount_8] = ParameterInfo((int)ModMatrixParameters::Amount_8, "Amount_8", nullptr, 0, -1, 1, ParameterFormatters::FormatDecimal3);
		modMap[(int)ModMatrixParameters::ViaAmount_8] = ParameterInfo((int)ModMatrixParameters::ViaAmount_8, "ViaAmount_8", nullptr, 0, -1, 1, ParameterFormatters::FormatDecimal3);
		
		modMap[(int)ModMatrixParameters::Source_9] = ParameterInfo((int)ModMatrixParameters::Source_9, "Source_9", nullptr, 0, 0, (int)ModSource::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Dest_9] = ParameterInfo((int)ModMatrixParameters::Dest_9, "Dest_9", nullptr, 0, 0, (int)ModDest::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Via_9] = ParameterInfo((int)ModMatrixParameters::Via_9, "Via_9", nullptr, 0, 0, (int)ModSource::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Amount_9] = ParameterInfo((int)ModMatrixParameters::Amount_9, "Amount_9", nullptr, 0, -1, 1, ParameterFormatters::FormatDecimal3);
		modMap[(int)ModMatrixParameters::ViaAmount_9] = ParameterInfo((int)ModMatrixParameters::ViaAmount_9, "ViaAmount_9", nullptr, 0, -1, 1, ParameterFormatters::FormatDecimal3);
		
		modMap[(int)ModMatrixParameters::Source_10] = ParameterInfo((int)ModMatrixParameters::Source_10, "Source_10", nullptr, 0, 0, (int)ModSource::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Dest_10] = ParameterInfo((int)ModMatrixParameters::Dest_10, "Dest_10", nullptr, 0, 0, (int)ModDest::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Via_10] = ParameterInfo((int)ModMatrixParameters::Via_10, "Via_10", nullptr, 0, 0, (int)ModSource::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Amount_10] = ParameterInfo((int)ModMatrixParameters::Amount_10, "Amount_10", nullptr, 0, -1, 1, ParameterFormatters::FormatDecimal3);
		modMap[(int)ModMatrixParameters::ViaAmount_10] = ParameterInfo((int)ModMatrixParameters::ViaAmount_10, "ViaAmount_10", nullptr, 0, -1, 1, ParameterFormatters::FormatDecimal3);
		
		modMap[(int)ModMatrixParameters::Source_11] = ParameterInfo((int)ModMatrixParameters::Source_11, "Source_11", nullptr, 0, 0, (int)ModSource::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Dest_11] = ParameterInfo((int)ModMatrixParameters::Dest_11, "Dest_11", nullptr, 0, 0, (int)ModDest::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Via_11] = ParameterInfo((int)ModMatrixParameters::Via_11, "Via_11", nullptr, 0, 0, (int)ModSource::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Amount_11] = ParameterInfo((int)ModMatrixParameters::Amount_11, "Amount_11", nullptr, 0, -1, 1, ParameterFormatters::FormatDecimal3);
		modMap[(int)ModMatrixParameters::ViaAmount_11] = ParameterInfo((int)ModMatrixParameters::ViaAmount_11, "ViaAmount_11", nullptr, 0, -1, 1, ParameterFormatters::FormatDecimal3);
		
		modMap[(int)ModMatrixParameters::Source_12] = ParameterInfo((int)ModMatrixParameters::Source_12, "Source_12", nullptr, 0, 0, (int)ModSource::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Dest_12] = ParameterInfo((int)ModMatrixParameters::Dest_12, "Dest_12", nullptr, 0, 0, (int)ModDest::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Via_12] = ParameterInfo((int)ModMatrixParameters::Via_12, "Via_12", nullptr, 0, 0, (int)ModSource::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Amount_12] = ParameterInfo((int)ModMatrixParameters::Amount_12, "Amount_12", nullptr, 0, -1, 1, ParameterFormatters::FormatDecimal3);
		modMap[(int)ModMatrixParameters::ViaAmount_12] = ParameterInfo((int)ModMatrixParameters::ViaAmount_12, "ViaAmount_12", nullptr, 0, -1, 1, ParameterFormatters::FormatDecimal3);
		
		modMap[(int)ModMatrixParameters::Source_13] = ParameterInfo((int)ModMatrixParameters::Source_13, "Source_13", nullptr, 0, 0, (int)ModSource::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Dest_13] = ParameterInfo((int)ModMatrixParameters::Dest_13, "Dest_13", nullptr, 0, 0, (int)ModDest::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Via_13] = ParameterInfo((int)ModMatrixParameters::Via_13, "Via_13", nullptr, 0, 0, (int)ModSource::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Amount_13] = ParameterInfo((int)ModMatrixParameters::Amount_13, "Amount_13", nullptr, 0, -1, 1, ParameterFormatters::FormatDecimal3);
		modMap[(int)ModMatrixParameters::ViaAmount_13] = ParameterInfo((int)ModMatrixParameters::ViaAmount_13, "ViaAmount_13", nullptr, 0, -1, 1, ParameterFormatters::FormatDecimal3);
		
		modMap[(int)ModMatrixParameters::Source_14] = ParameterInfo((int)ModMatrixParameters::Source_14, "Source_14", nullptr, 0, 0, (int)ModSource::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Dest_14] = ParameterInfo((int)ModMatrixParameters::Dest_14, "Dest_14", nullptr, 0, 0, (int)ModDest::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Via_14] = ParameterInfo((int)ModMatrixParameters::Via_14, "Via_14", nullptr, 0, 0, (int)ModSource::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Amount_14] = ParameterInfo((int)ModMatrixParameters::Amount_14, "Amount_14", nullptr, 0, -1, 1, ParameterFormatters::FormatDecimal3);
		modMap[(int)ModMatrixParameters::ViaAmount_14] = ParameterInfo((int)ModMatrixParameters::ViaAmount_14, "ViaAmount_14", nullptr, 0, -1, 1, ParameterFormatters::FormatDecimal3);
		
		modMap[(int)ModMatrixParameters::Source_15] = ParameterInfo((int)ModMatrixParameters::Source_15, "Source_15", nullptr, 0, 0, (int)ModSource::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Dest_15] = ParameterInfo((int)ModMatrixParameters::Dest_15, "Dest_15", nullptr, 0, 0, (int)ModDest::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Via_15] = ParameterInfo((int)ModMatrixParameters::Via_15, "Via_15", nullptr, 0, 0, (int)ModSource::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Amount_15] = ParameterInfo((int)ModMatrixParameters::Amount_15, "Amount_15", nullptr, 0, -1, 1, ParameterFormatters::FormatDecimal3);
		modMap[(int)ModMatrixParameters::ViaAmount_15] = ParameterInfo((int)ModMatrixParameters::ViaAmount_15, "ViaAmount_15", nullptr, 0, -1, 1, ParameterFormatters::FormatDecimal3);
		
		modMap[(int)ModMatrixParameters::Source_16] = ParameterInfo((int)ModMatrixParameters::Source_16, "Source_16", nullptr, 0, 0, (int)ModSource::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Dest_16] = ParameterInfo((int)ModMatrixParameters::Dest_16, "Dest_16", nullptr, 0, 0, (int)ModDest::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Via_16] = ParameterInfo((int)ModMatrixParameters::Via_16, "Via_16", nullptr, 0, 0, (int)ModSource::Count - 0.0001, ParameterFormatters::FormatIntFloor);
		modMap[(int)ModMatrixParameters::Amount_16] = ParameterInfo((int)ModMatrixParameters::Amount_16, "Amount_16", nullptr, 0, -1, 1, ParameterFormatters::FormatDecimal3);
		modMap[(int)ModMatrixParameters::ViaAmount_16] = ParameterInfo((int)ModMatrixParameters::ViaAmount_16, "ViaAmount_16", nullptr, 0, -1, 1, ParameterFormatters::FormatDecimal3);

		ParamInfo[Module::ModMatrix] = modMap;

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
		for (auto param : ParamInfo[module])
		{
			if (param.second.ParameterName == parameterString)
				return param.second.ParameterIndex;
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
