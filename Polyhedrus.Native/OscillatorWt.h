#pragma once

#include "Default.h"
#include "WavetableManager.h"
#include "OscillatorBase.h"

namespace Polyhedrus
{
	class OscillatorWt : public OscillatorBase
	{
	private:
		float* buffer;
		std::shared_ptr<Wavetable> wavetable;
		int samplerate;
		int modulationUpdateRate;
		uint32_t increment;
		uint32_t iterator;
		int tableSize;
		float iteratorScaler;
		float waveMix;
		float* waveA;
		float* waveB;

		float glideRate;
		float currentPitch;

	public:
		OscillatorWt();
		~OscillatorWt();

		OscillatorType GetType() const override;
		void Initialize(int samplerate, int bufferSize, int modulationUpdateRate) override;
		void UpdateGlide() override;
		void Reset() override;
		void Process(int count) override;
		float* GetOutput() const override;
		std::vector<uint8_t> GetVisual() const override;
		std::string GetShapeString() const override;

		void SetWavetable(std::shared_ptr<Wavetable> wavetable);
		std::shared_ptr<Wavetable> GetWavetable() const;
		
	private:
		void Update();
	};
}

