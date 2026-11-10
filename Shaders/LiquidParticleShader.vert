#version 460
#extension GL_ARB_gpu_shader_int64 : require
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

#include "Lights.glsl"
#include "Constants.glsl"
#include "MeshPropertiesBuffer.glsl"
#include "MaterialPropertiesBuffer.glsl" 
layout(std430, binding = 0) buffer SceneDataBuffer
{
   uint HDRMapInputIndex;
    uint EnvironmentMapIndex;
    uint BRDFMapId;
    uint CubeMapId;
    uint IrradianceMapId;
    uint PrefilterMapId;
	uint DirectionalLightShadowMapId;
	uint DirectionalLightShadowTintMapId;
	uint PointLightShadowMapId;
	uint PointLightShadowTintMapId;
    uint _pad0;
    uint _pad1;

    mat4 OrthoProjection;
    mat4 OrthoView;
    mat4 InverseOrthoProjection;
    mat4 InverseOrthoView;
    mat4 InversePerspectiveProjection;
    mat4 InversePerspectiveView;

    vec3  PerspectiveCameraPosition;
    float Time;
    vec3  PerspectiveViewDirection;
    uint  FrameIndex;
    vec2  InvertResolution;
    vec2  _padEnd;
} sceneDataBuffer;

layout(binding = 1) buffer BindlessBuffer
{
    uint64_t MeshOffset;
    uint MeshCount;
    uint MeshSize;
    uint64_t MaterialOffset;
    uint MaterialCount;
    uint MaterialSize;
    uint64_t DirectionalLightOffset;
    uint DirectionalLightCount;
    uint DirectionalLightSize;
    uint64_t PointLightOffset;
    uint PointLightCount;
    uint PointLightSize;
    uint64_t Texture2DOffset;
    uint Texture2DCount;
    uint Texture2DSize;
    uint64_t Texture3DOffset;
    uint Texture3DCount;
    uint Texture3DSize;
    uint64_t TextureCubeMapOffset;
    uint TextureCubeMapCount;
    uint TextureCubeMapSize;
    uint64_t SpriteInstanceOffset;
    uint SpriteInstanceCount;
    uint SpriteInstanceSize;
    uint Data[];
} bindlessBuffer;

layout(binding = 2) uniform samplerCube CubeMap[];
layout(binding = 3) uniform sampler2D TextureMap[];
layout(binding = 4) uniform sampler3D Texture3DMap[];

layout (location = 0)  in vec2  VS_SpriteSize;
layout (location = 1)  in vec3  VS_SpriteColor;
layout (location = 2)  in mat4  VS_InstanceTransform;

layout(location = 0) out vec3 WorldPos;
layout(location = 1) out vec3 PS_SpriteColor;

struct Vertex2D
{
	vec2 Position;
};

void main() 
{
    Vertex2D vertex = Vertex2D(vec2(0.0f));
    switch(gl_VertexIndex) 
	{
        case 0: vertex = Vertex2D(vec2(0.0f           , VS_SpriteSize.y)); break; 
        case 1: vertex = Vertex2D(vec2(VS_SpriteSize.x, VS_SpriteSize.y)); break;
        case 2: vertex = Vertex2D(vec2(VS_SpriteSize.x, 0.0f           )); break;
        case 3: vertex = Vertex2D(vec2(0.0f           , 0.0f           )); break;
    }

    vec4 worldPos = VS_InstanceTransform * vec4(vertex.Position.xy, 0.0f, 1.0);
    WorldPos = worldPos.xyz;
    PS_SpriteColor = VS_SpriteColor;

    gl_Position = worldPos;
    gl_PointSize = 16.0f;
}