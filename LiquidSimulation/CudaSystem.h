#pragma once
#include <Platform.h>
#include <TypeDef.h>
#include <MemorySystem.h>
#include <cuda_runtime.h>
#include "CudaError.h"

class CudaSystem
{
public:
    static CudaSystem& Get();

private:
    CudaSystem() = default;
    ~CudaSystem() = default;
    CudaSystem(const CudaSystem&) = delete;
    CudaSystem& operator=(const CudaSystem&) = delete;
    CudaSystem(CudaSystem&&) = delete;
    CudaSystem& operator=(CudaSystem&&) = delete;

    std::mutex                               m_mutex;
    std::unordered_map<void*, MemoryLeakPtr> m_ptrAddressMap;

    void TrackAllocation(void* ptr, size_t count, bool isArray, const char* file, int line, const char* typeName, const char* func, const char* notes)
    {
        if (!ptr) return;

        MemoryLeakPtr memoryLeakPtr{
            .PtrAddress = ptr,
            .Line = line,
            .IsArray = isArray,
            .ElementCount = count,
            .File = file ? file : "unknown",
            .TypeName = typeName ? typeName : "unknown",
            .Function = func ? func : "unknown",
            .Notes = notes ? notes : ""
        };

        m_ptrAddressMap[ptr] = std::move(memoryLeakPtr);
    }

public:

    template <typename T>
    T* AddCudaPtrBuffer(size_t count, const char* file, int line, const char* func)
    {
        if (count == 0) return nullptr;

        T* cpuData = nullptr;
        T* gpuData = nullptr;
        CUDA_THROW_IF_FAIL(cudaMalloc(&gpuData, count * sizeof(T)));

        cpuData = memorySystem.AddPtrBuffer<T>(count, file, line, func);
        CUDA_THROW_IF_FAIL(cudaMemcpy(gpuData, cpuData, count * sizeof(T), cudaMemcpyHostToDevice));
        memorySystem.DeletePtr(cpuData);

        TrackAllocation(gpuData, count * sizeof(T), count > 1, file, line, typeid(T).name(), func, "Cuda Ptr");
        return gpuData;
    }

    template <typename T>
    T* AddCudaPtrBuffer(const T* cpuData, size_t count, const char* file, int line, const char* func)
    {
        T* gpuData = AddCudaPtrBuffer<T>(count, file, line, func);
        if (gpuData && cpuData) cudaMemcpy(gpuData, cpuData, count * sizeof(T), cudaMemcpyHostToDevice);
        return gpuData;
    }

    template <typename T>
    Vector<T> GetCudaData(T* cudaPtr, uint32 instanceCount)
    {
        const uint32 size = instanceCount * sizeof(T);
        T* cpuListPtr = memorySystem.AddPtrBuffer<T>(instanceCount, __FILE__, __LINE__, __func__);
        cudaMemcpy(cpuListPtr, cudaPtr, size, cudaMemcpyDeviceToHost);

        Vector<T> cpuListCopy(cpuListPtr, cpuListPtr + instanceCount);
        memorySystem.DeletePtr(cpuListPtr);
        return cpuListCopy;
    }

    template<typename T>
    void DeletePtr(T* ptr)
    {
        if (!ptr) return;

        ptr->~T();
        DeletePtr(static_cast<void*>(ptr));
    }

    void DeletePtr(void* ptr);
    void ReportLeaks();
};
extern CudaSystem& cudaSystem;
inline CudaSystem& CudaSystem::Get()
{
    static CudaSystem instance;
    return instance;
}


