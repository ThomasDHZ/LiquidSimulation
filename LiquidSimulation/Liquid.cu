#include "CudaSystem.h"
#include "Liquid.cuh"
#define GLM_FORCE_CUDA

mat4 makeTRS(const vec3& pos, const vec3& rot, const vec3& scale)
{
	float cx = cosf(rot.x), sx = sinf(rot.x);
	float cy = cosf(rot.y), sy = sinf(rot.y);
	float cz = cosf(rot.z), sz = sinf(rot.z);

	mat4 m(1.0f);

	// Rotation (adjust order if your convention differs)
	m[0][0] = cy * cz * scale.x;
	m[0][1] = cy * sz * scale.x;
	m[0][2] = -sy * scale.x;

	m[1][0] = (sx * sy * cz - cx * sz) * scale.y;
	m[1][1] = (sx * sy * sz + cx * cz) * scale.y;
	m[1][2] = sx * cy * scale.y;

	m[2][0] = (cx * sy * cz + sx * sz) * scale.z;
	m[2][1] = (cx * sy * sz - sx * cz) * scale.z;
	m[2][2] = cx * cy * scale.z;

	// Translation
	m[3] = vec4(pos, 1.0f);

	return m;
}

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
		//
		//mat4 spriteMatrix = mat4(1.0f);
		//spriteMatrix = glm::translate(spriteMatrix, vec3(position.x, position.y, 0.0f));
		//spriteMatrix = glm::rotate(spriteMatrix, glm::radians(rotation.x), vec3(1.0f, 0.0f, 0.0f));
		//spriteMatrix = glm::rotate(spriteMatrix, glm::radians(rotation.y), vec3(0.0f, 1.0f, 0.0f));
		//spriteMatrix = glm::rotate(spriteMatrix, glm::radians(0.0f), vec3(0.0f, 0.0f, 1.0f));
		//spriteMatrix = glm::scale(spriteMatrix, vec3(scale.x, scale.y, 1.0f));

		fluidParticleInstanceList[idx] = FluidParticleInstance
		{
			.ParticleSize = vec2((float)x, (float)y),
			.ParticleColor = fluidParticleList[idx].ParticleColor,
			.InstanceTransform = makeTRS(position, rotation, scale),
			.Velocity = fluidParticleList[idx].Velocity,
			.Density = fluidParticleList[idx].Density
		};
	}
}

FluidParticle* CreateFluidData(uint32 instanceCount)
{
	dim3 blockSize(16, 16, 1);
	dim3 gridSize((64 + blockSize.x - 1) / blockSize.x, (64 + blockSize.y - 1) / blockSize.y, 1);
	FluidParticle* fluidParticleGpuList = cudaSystem.AddCudaPtrBuffer<FluidParticle>(instanceCount, __FILE__, __LINE__, __func__);
	Cuda_CreateFluidData << <gridSize, blockSize >> > (fluidParticleGpuList, 64, 64);
	CUDA_THROW_IF_FAIL(cudaDeviceSynchronize());
	CUDA_THROW_IF_FAIL(cudaGetLastError());

	return fluidParticleGpuList;
}

void UpdateFluidInstanceData( FluidParticle* fluidParticaGpulListPtr, FluidParticleInstance* fluidParticleInstanceListPtr)
{
	uint32 size = 64 * 64;
	Vector<FluidParticle> instanceData = cudaSystem.GetCudaData<FluidParticle>(fluidParticaGpulListPtr, size);
	Vector<FluidParticleInstance> instanceData2 = cudaSystem.GetCudaData<FluidParticleInstance>(fluidParticleInstanceListPtr, size);

	dim3 blockSize(16, 16, 1);
	dim3 gridSize((64 + blockSize.x - 1) / blockSize.x, (64 + blockSize.y - 1) / blockSize.y, 1);
	Cuda_UpdateFluidData << <gridSize, blockSize >> > (fluidParticaGpulListPtr, fluidParticleInstanceListPtr, 64, 64);
	CUDA_THROW_IF_FAIL(cudaDeviceSynchronize()); //thrown here
	CUDA_THROW_IF_FAIL(cudaGetLastError()); 

	Vector<FluidParticleInstance> instanceData3 = cudaSystem.GetCudaData<FluidParticleInstance>(fluidParticleInstanceListPtr, size);
	int a = 234;
}