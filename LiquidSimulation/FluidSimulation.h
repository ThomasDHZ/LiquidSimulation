#pragma once
#include "CudaSystem.h"
#include "Liquid.cuh"

class FluidSimulation
{
private:
	uint32 m_instanceCount = UINT32_MAX;
	FluidParticle* cuda_particleList = nullptr;
	FluidParticleInstance* cuda_particleInstanceList = nullptr;

public:
	FluidSimulation();
	~FluidSimulation();

	void StartUp(uint instanceCount);
	void Update(float deltaTime);
	void Destroy();
};

