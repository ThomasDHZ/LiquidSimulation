#include "Liquid.cuh"

__global__ void CreateFluidDataCuda(FluidParticle* fluidCellList, uint width, uint height)
{
	const uint x = blockIdx.x * blockDim.x + threadIdx.x;
	const uint y = blockIdx.y * blockDim.y + threadIdx.y;

	if (x < width && 
		y < height)
	{
		uint idx = y * width + x;
		fluidCellList[idx] = FluidParticle
		{
			.ParticlePosition = vec3((float)x, (float)y, 0.0f),
			.ParticleRotation = vec3(0.0f),
			.ParticleScale = vec3(1.0f),
			.ParticleSize = vec2(5.0f),
			.ParticleColor = vec4(0.0f, 0.0f, 1.0f, 1.0f),
			.Velocity = vec2(0.0f),
			.Density = 32.0f
		};
	}
}

__global__ void UpdateFluidDataCuda(const FluidParticle* fluidParticleList, FluidParticleInstance* fluidParticleInstanceList, uint width, uint height)
{
	const uint x = blockIdx.x * blockDim.x + threadIdx.x;
	const uint y = blockIdx.y * blockDim.y + threadIdx.y;

	if (x < width &&
		y < height)
	{
		uint idx = y * width + x;
		const vec3 position = fluidParticleList[idx].ParticlePosition;
		const vec3 rotation = fluidParticleList[idx].ParticleRotation;
		const vec3 scale	= fluidParticleList[idx].ParticleScale;
		
		mat4 spriteMatrix = mat4(1.0f);
		spriteMatrix = glm::translate(spriteMatrix, vec3(position.x, position.y, 0.0f));
		spriteMatrix = glm::rotate(spriteMatrix, glm::radians(rotation.x), vec3(1.0f, 0.0f, 0.0f));
		spriteMatrix = glm::rotate(spriteMatrix, glm::radians(rotation.y), vec3(0.0f, 1.0f, 0.0f));
		spriteMatrix = glm::rotate(spriteMatrix, glm::radians(0.0f), vec3(0.0f, 0.0f, 1.0f));
		spriteMatrix = glm::scale(spriteMatrix, vec3(scale.x, scale.y, 1.0f));

		fluidParticleInstanceList[idx] = FluidParticleInstance
		{
			.ParticleSize = fluidParticleList[idx].ParticleSize,
			.ParticleColor = fluidParticleList[idx].ParticleColor,
			.InstanceTransform = spriteMatrix,
			.Velocity = fluidParticleList[idx].Velocity,
			.Density = fluidParticleList[idx].Density
		};
	}
}

Vector<FluidParticle> CreateFluidData()
{
	const int count = 64 * 64;
	const uint32 size = count * sizeof(FluidParticle);

	FluidParticle* fluidCellCpu = memorySystem.AddPtrBuffer<FluidParticle>(count, __FILE__, __LINE__, __func__);

	FluidParticle* fluidCellGpu = nullptr;
	cudaMalloc(&fluidCellGpu, count * sizeof(FluidParticle));
	cudaMemcpy(fluidCellGpu, fluidCellCpu, size, cudaMemcpyHostToDevice);

	dim3 blockSize(16, 16, 1);
	dim3 gridSize((64 + blockSize.x - 1) / blockSize.x, (64 + blockSize.y - 1) / blockSize.y, 1);
	CreateFluidDataCuda << <gridSize, blockSize >> > (fluidCellGpu, 64, 64);
	cudaError_t error = cudaGetLastError();
	cudaDeviceSynchronize();

	cudaMemcpy(fluidCellCpu, fluidCellGpu, size, cudaMemcpyDeviceToHost);
	Vector<FluidParticle> fluidParticleCopy(fluidCellCpu, fluidCellCpu + count);
	
	cudaFree(fluidCellGpu);
	memorySystem.DeletePtr(fluidCellCpu);
	return fluidParticleCopy;
}

Vector<FluidParticleInstance> CreateFluidInstanceData(const FluidParticle* fluidParticalListPtr)
{
	const int count = 64 * 64;
	const uint32 size = count * sizeof(FluidParticleInstance);

	FluidParticleInstance* fluidCellCpu = memorySystem.AddPtrBuffer<FluidParticleInstance>(count, __FILE__, __LINE__, __func__);

	FluidParticleInstance* fluidCellGpu = nullptr;
	cudaMalloc(&fluidCellGpu, count * sizeof(FluidParticleInstance));
	cudaMemcpy(fluidCellGpu, fluidCellCpu, size, cudaMemcpyHostToDevice);

	dim3 blockSize(16, 16, 1);
	dim3 gridSize((64 + blockSize.x - 1) / blockSize.x, (64 + blockSize.y - 1) / blockSize.y, 1);
	UpdateFluidDataCuda << <gridSize, blockSize >> > (fluidParticalListPtr, fluidCellGpu, 64, 64);
	cudaError_t error = cudaGetLastError();
	cudaDeviceSynchronize();

	cudaMemcpy(fluidCellCpu, fluidCellGpu, size, cudaMemcpyDeviceToHost);
	Vector<FluidParticleInstance> fluidParticleInstanceCopy(fluidCellCpu, fluidCellCpu + count);

	cudaFree(fluidCellGpu);
	memorySystem.DeletePtr(fluidCellCpu);
	return Vector<FluidParticleInstance>(fluidCellCpu, fluidCellCpu + count);
}