#include "Liquid.cuh"

__global__ void Cuda_CreateFluidData(FluidParticle* fluidCellList, uint width, uint height)
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

__global__ void Cuda_UpdateFluidData(const FluidParticle* fluidParticleList, FluidParticleInstance* fluidParticleInstanceList, uint width, uint height)
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

FluidParticle* CreateFluidData(FluidParticle* fluidParticleCpuList)
{
	dim3 blockSize(16, 16, 1);
	dim3 gridSize((64 + blockSize.x - 1) / blockSize.x, (64 + blockSize.y - 1) / blockSize.y, 1);
	Cuda_CreateFluidData << <gridSize, blockSize >> > (fluidParticleCpuList, 64, 64);

	CUDA_THROW_IF_FAIL(cudaGetLastError());
	cudaDeviceSynchronize();
	return fluidParticleCpuList;
}

void UpdateFluidInstanceData(const FluidParticle* fluidParticaGpulListPtr, FluidParticleInstance* fluidParticleInstanceListPtr)
{
	dim3 blockSize(16, 16, 1);
	dim3 gridSize((64 + blockSize.x - 1) / blockSize.x, (64 + blockSize.y - 1) / blockSize.y, 1);

	Cuda_UpdateFluidData << <gridSize, blockSize >> > (fluidParticaGpulListPtr, fluidParticleInstanceListPtr, 64, 64);
	CUDA_THROW_IF_FAIL(cudaGetLastError()); 
	cudaDeviceSynchronize();
}