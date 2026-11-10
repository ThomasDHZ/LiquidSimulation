#include "MemorySystem.h"
#include <cmath>
#include <cuda_runtime.h>

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

__global__ void CreateFluidDataCuda(FluidParticle* fluidCellList, uint width, uint height);
__global__ void UpdateFluidDataCuda(const FluidParticle* fluidParticleList, FluidParticleInstance* fluidParticleInstanceList, uint width, uint height);
Vector<FluidParticle> CreateFluidData();
Vector<FluidParticleInstance> CreateFluidInstanceData(const FluidParticle* fluidParticalListPtr);