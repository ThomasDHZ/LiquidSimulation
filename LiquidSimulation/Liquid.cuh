#pragma once
#include "MemorySystem.h"
#include <cmath>
#include <cuda_runtime.h>
#include "CudaError.h"

struct FluidParticle
{
	vec3  ParticlePosition = vec3(0.0f);
	vec3  ParticleRotation = vec3(0.0f);
	vec3  ParticleScale = vec3(1.0f);
	vec2  ParticleSize = vec2(5.0f);
	vec4  ParticleColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	vec2  Velocity = vec2(0.0f);
	float Density = 0.0f;
};

struct FluidParticleInstance
{
	vec2  ParticleSize = vec2(5.0f);
	vec4  ParticleColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	mat4  InstanceTransform = mat4(1.0f);
	vec2  Velocity = vec2(0.0f);
	float Density = 0.0f;
};

__global__ void Cuda_CreateFluidData(FluidParticle* fluidCellList, uint width, uint height);
__global__ void Cuda_UpdateFluidData(const FluidParticle* fluidParticleList, FluidParticleInstance* fluidParticleInstanceList, uint width, uint height);
FluidParticle* CreateFluidData(FluidParticle* fluidParticleGpuList);
void UpdateFluidInstanceData(const FluidParticle* fluidParticaGpulListPtr, FluidParticleInstance* fluidParticleInstanceListPtr);