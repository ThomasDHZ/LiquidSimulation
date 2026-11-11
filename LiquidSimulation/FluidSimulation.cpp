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
	FluidParticle* fluidParticleList = memorySystem.AddPtrBuffer<FluidParticle>(instanceCount, __FILE__, __LINE__, __func__);
	cuda_particleList = cudaSystem.AddCudaPtrBuffer<FluidParticle>(fluidParticleList, m_instanceCount, __FILE__, __LINE__, __func__);
	cuda_particleInstanceList = cudaSystem.AddCudaPtrBuffer<FluidParticleInstance>(m_instanceCount, __FILE__, __LINE__, __func__);

	cuda_particleList = CreateFluidData(fluidParticleList);
}

void FluidSimulation::Update(float deltaTime)
{
	UpdateFluidInstanceData(cuda_particleList, cuda_particleInstanceList);
	Vector<FluidParticleInstance> instanceData = cudaSystem.GetCudaData<FluidParticleInstance>(cuda_particleInstanceList, m_instanceCount);
	int a = 34;
}

void FluidSimulation::Destroy()
{
	cudaFree(cuda_particleList);
	cudaFree(cuda_particleInstanceList);
}
