#include "FluidSimulation.h"

FluidSimulation::FluidSimulation()
{
}

FluidSimulation::~FluidSimulation()
{
}

void FluidSimulation::StartUp(uint instanceCount)
{
	m_instanceCount = instanceCount;
	cuda_particleInstanceList = cudaSystem.AddCudaPtrBuffer<FluidParticleInstance>(m_instanceCount, __FILE__, __LINE__, __func__);
	cuda_particleList = CreateFluidData(m_instanceCount);
}

void FluidSimulation::Update(float deltaTime)
{
	UpdateFluidInstanceData(cuda_particleList, cuda_particleInstanceList);
}

void FluidSimulation::Destroy()
{
	cudaSystem.DeletePtr(cuda_particleList);
	cudaSystem.DeletePtr(cuda_particleInstanceList);
}
