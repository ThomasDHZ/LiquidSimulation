#pragma once
#include <cuda_runtime.h>
#include <string>
#include <stdexcept>
#include <sstream>

class CudaError : public std::runtime_error
{
public:
    CudaError(cudaError_t result, const char* call, const char* file, int line, const char* func)
        : std::runtime_error(BuildMessage(result, call, file, line, func))
        , result(result)
    {
    }

    const cudaError_t result;

    static inline const char* GetCudaError(cudaError_t r)
    {
        switch (r) {
        case cudaSuccess:  return "The API call returned with no errors.";
        case cudaErrorInvalidValue: return "This indicates that one or more of the parameters passed to the API call is not within an acceptable range of values.";
        case cudaErrorMemoryAllocation: return "The API call failed because it was unable to allocate enough memory or other resources to perform the requested operation.";
        case cudaErrorInitializationError: return "The API call failed because the CUDA driver and runtime could not be initialized.";
        case cudaErrorCudartUnloading: return "This indicates that a CUDA Runtime API call cannot be executed because it is being called during process shut down, at a point in time after CUDA driver has been unloaded.";
        case cudaErrorProfilerDisabled: return "This indicates profiler is not initialized for this run. This can happen when the application is running with external profiling tools like visual profiler.";
        case cudaErrorProfilerNotInitialized: return "Deprecated as of CUDA 5.0. It is no longer an error to attempt to enable/disable the profiling via cudaProfilerStart or cudaProfilerStop without initialization.";
        case cudaErrorProfilerAlreadyStarted: return "Deprecated as of CUDA 5.0. It is no longer an error to call cudaProfilerStart() when profiling is already enabled.";
        case cudaErrorProfilerAlreadyStopped: return "Deprecated as of CUDA 5.0. It is no longer an error to call cudaProfilerStop() when profiling is already disabled.";
        case cudaErrorInvalidConfiguration: return "This indicates that a kernel launch is requesting resources that can never be satisfied by the current device. Requesting more shared memory per block than the device supports will trigger this error, as will requesting too many threads or blocks. See cudaDeviceProp for more device limitations.";
        case cudaErrorVersionTranslation: return "This indicates that the driver is newer than the runtime version and returned graph node parameter information that the runtime does not understand and is unable to translate.";
        case cudaErrorInvalidPitchValue: return "This indicates that one or more of the pitch-related parameters passed to the API call is not within the acceptable range for pitch.";
        case cudaErrorInvalidSymbol: return "This indicates that the symbol name/identifier passed to the API call is not a valid name or identifier.";
        case cudaErrorInvalidHostPointer: return "This indicates that at least one host pointer passed to the API call is not a valid host pointer. Deprecated as of CUDA 10.1.";
        case cudaErrorInvalidDevicePointer: return "This indicates that at least one device pointer passed to the API call is not a valid device pointer. Deprecated as of CUDA 10.1.";
        case cudaErrorInvalidTexture: return "This indicates that the texture passed to the API call is not a valid texture.";
        case cudaErrorInvalidTextureBinding: return "This indicates that the texture binding is not valid. This occurs if you call cudaGetTextureAlignmentOffset() with an unbound texture.";
        case cudaErrorInvalidChannelDescriptor: return "This indicates that the channel descriptor passed to the API call is not valid. This occurs if the format is not one of the formats specified by cudaChannelFormatKind, or if one of the dimensions is invalid.";
        case cudaErrorInvalidMemcpyDirection: return "This indicates that the direction of the memcpy passed to the API call is not one of the types specified by cudaMemcpyKind.";
        case cudaErrorAddressOfConstant: return "This indicated that the user has taken the address of a constant variable, which was forbidden up until the CUDA 3.1 release. Deprecated as of CUDA 3.1.";
        case cudaErrorTextureFetchFailed: return "This indicated that a texture fetch was not able to be performed. Deprecated as of CUDA 3.1.";
        case cudaErrorTextureNotBound: return "This indicated that a texture was not bound for access. Deprecated as of CUDA 3.1.";
        case cudaErrorSynchronizationError: return "This indicated that a synchronization operation had failed. Deprecated as of CUDA 3.1.";
        case cudaErrorInvalidFilterSetting: return "This indicates that a non-float texture was being accessed with linear filtering. This is not supported by CUDA.";
        case cudaErrorInvalidNormSetting: return "This indicates that an attempt was made to read an unsupported data type as a normalized float. This is not supported by CUDA.";
        case cudaErrorMixedDeviceExecution: return "Mixing of device and device emulation code was not allowed. Deprecated as of CUDA 3.1.";
        case cudaErrorNotYetImplemented: return "This indicates that the API call is not yet implemented. Production releases of CUDA will never return this error. Deprecated as of CUDA 4.1.";
        case cudaErrorMemoryValueTooLarge: return "This indicated that an emulated device pointer exceeded the 32-bit address range. Deprecated as of CUDA 3.1.";
        case cudaErrorStubLibrary: return "This indicates that the CUDA driver that the application has loaded is a stub library. Applications that run with the stub rather than a real driver loaded will result in CUDA API returning this error.";
        case cudaErrorInsufficientDriver: return "This indicates that the installed NVIDIA CUDA driver is older than the CUDA runtime library. This is not a supported configuration. Users should install an updated NVIDIA display driver to allow the application to run.";
        case cudaErrorCallRequiresNewerDriver: return "This indicates that the API call requires a newer CUDA driver than the one currently installed. Users should install an updated NVIDIA CUDA driver to allow the API call to succeed.";
        case cudaErrorInvalidSurface: return "This indicates that the surface passed to the API call is not a valid surface.";
        case cudaErrorDuplicateVariableName: return "This indicates that multiple global or constant variables (across separate CUDA source files in the application) share the same string name.";
        case cudaErrorDuplicateTextureName: return "This indicates that multiple textures (across separate CUDA source files in the application) share the same string name.";
        case cudaErrorDuplicateSurfaceName: return "This indicates that multiple surfaces (across separate CUDA source files in the application) share the same string name.";
        case cudaErrorDevicesUnavailable: return "This indicates that all CUDA devices are busy or unavailable at the current time. Devices are often busy/unavailable due to use of cudaComputeModeProhibited, cudaComputeModeExclusiveProcess, or when long running CUDA kernels have filled up the GPU and are blocking new work from starting.";
        case cudaErrorIncompatibleDriverContext: return "This indicates that the current context is not compatible with the CUDA Runtime. This can only occur if you are using CUDA Runtime/Driver interoperability and have created an existing Driver context using the driver API.";
        case cudaErrorMissingConfiguration: return "The device function being invoked (usually via cudaLaunchKernel()) was not previously configured via the cudaConfigureCall() function.";
        case cudaErrorPriorLaunchFailure: return "This indicated that a previous kernel launch failed. Deprecated as of CUDA 3.1.";
        case cudaErrorLaunchMaxDepthExceeded: return "This error indicates that a device runtime grid launch did not occur because the depth of the child grid would exceed the maximum supported number of nested grid launches.";
        case cudaErrorLaunchFileScopedTex: return "This error indicates that a grid launch did not occur because the kernel uses file-scoped textures which are unsupported by the device runtime.";
        case cudaErrorLaunchFileScopedSurf: return "This error indicates that a grid launch did not occur because the kernel uses file-scoped surfaces which are unsupported by the device runtime.";
        case cudaErrorSyncDepthExceeded: return "This error indicates that a call to cudaDeviceSynchronize made from the device runtime failed because the call was made at grid depth greater than the allowed limit.";
        case cudaErrorLaunchPendingCountExceeded: return "This error indicates that a device runtime grid launch failed because the launch would exceed the limit cudaLimitDevRuntimePendingLaunchCount.";
        case cudaErrorInvalidDeviceFunction: return "The requested device function does not exist or is not compiled for the proper device architecture.";
        case cudaErrorNoDevice: return "This indicates that no CUDA-capable devices were detected by the installed CUDA driver.";
        case cudaErrorInvalidDevice: return "This indicates that the device ordinal supplied by the user does not correspond to a valid CUDA device or that the action requested is invalid for the specified device.";
        case cudaErrorDeviceNotLicensed: return "This indicates that the device doesn't have a valid Grid License.";
        case cudaErrorSoftwareValidityNotEstablished: return "By default, the CUDA runtime may perform a minimal set of self-tests, as well as CUDA driver tests, to establish the validity of both. Introduced in CUDA 11.2, this error return indicates that at least one of these tests has failed.";
        case cudaErrorStartupFailure: return "This indicates an internal startup failure in the CUDA runtime.";
        case cudaErrorInvalidKernelImage: return "This indicates that the device kernel image is invalid.";
        case cudaErrorDeviceUninitialized: return "This most frequently indicates that there is no context bound to the current thread. This can also be returned if the context passed to an API call is not a valid handle.";
        case cudaErrorMapBufferObjectFailed: return "This indicates that the buffer object could not be mapped.";
        case cudaErrorUnmapBufferObjectFailed: return "This indicates that the buffer object could not be unmapped.";
        case cudaErrorArrayIsMapped: return "This indicates that the specified array is currently mapped and thus cannot be destroyed.";
        case cudaErrorAlreadyMapped: return "This indicates that the resource is already mapped.";
        case cudaErrorNoKernelImageForDevice: return "This indicates that there is no kernel image available that is suitable for the device.";
        case cudaErrorAlreadyAcquired: return "This indicates that a resource has already been acquired.";
        case cudaErrorNotMapped: return "This indicates that a resource is not mapped.";
        case cudaErrorNotMappedAsArray: return "This indicates that a mapped resource is not available for access as an array.";
        case cudaErrorNotMappedAsPointer: return "This indicates that a mapped resource is not available for access as a pointer.";
        case cudaErrorECCUncorrectable: return "This indicates that an uncorrectable ECC error was detected during execution.";
        case cudaErrorUnsupportedLimit: return "This indicates that the cudaLimit passed to the API call is not supported by the active device.";
        case cudaErrorDeviceAlreadyInUse: return "This indicates that a call tried to access an exclusive-thread device that is already in use by a different thread.";
        case cudaErrorPeerAccessUnsupported: return "This error indicates that P2P access is not supported across the given devices.";
        case cudaErrorInvalidPtx: return "A PTX compilation failed. The runtime may fall back to compiling PTX if an application does not contain a suitable binary for the current device.";
        case cudaErrorInvalidGraphicsContext: return "This indicates an error with the OpenGL or DirectX context.";
        case cudaErrorNvlinkUncorrectable: return "This indicates that an uncorrectable NVLink error was detected during the execution.";
        case cudaErrorJitCompilerNotFound: return "This indicates that the PTX JIT compiler library was not found.";
        case cudaErrorUnsupportedPtxVersion: return "This indicates that the provided PTX was compiled with an unsupported toolchain.";
        case cudaErrorJitCompilationDisabled: return "This indicates that the JIT compilation was disabled.";
        case cudaErrorUnsupportedExecAffinity: return "This indicates that the provided execution affinity is not supported by the device.";
        case cudaErrorUnsupportedDevSideSync: return "This indicates that the code to be compiled by the PTX JIT contains an unsupported call to cudaDeviceSynchronize.";
        case cudaErrorContained: return "This indicates that an exception occurred on the device that is now contained by the GPU's error containment capability. This leaves the process in an inconsistent state and any further CUDA work will return the same error. To continue using CUDA, the process must be terminated and relaunched.";
        case cudaErrorInsufficientLoaderVersion: return "This indicates that loader version is insufficient for Fatbin.";
        case cudaErrorInvalidSource: return "This indicates that the device kernel source is invalid.";
        case cudaErrorFileNotFound: return "This indicates that the file specified was not found.";
        case cudaErrorSharedObjectSymbolNotFound: return "This indicates that a link to a shared object failed to resolve.";
        case cudaErrorSharedObjectInitFailed: return "This indicates that initialization of a shared object failed.";
        case cudaErrorOperatingSystem: return "This error indicates that an OS call failed.";
        case cudaErrorInvalidResourceHandle: return "This indicates that a resource handle passed to the API call was not valid. Resource handles are opaque types like cudaStream_t and cudaEvent_t.";
        case cudaErrorIllegalState: return "This indicates that a resource required by the API call is not in a valid state to perform the requested operation.";
        case cudaErrorLossyQuery: return "This indicates an attempt was made to introspect an object in a way that would discard semantically important information.";
        case cudaErrorSymbolNotFound: return "This indicates that a named symbol was not found. Examples of symbols are global/constant variable names, driver function names, texture names, and surface names.";
        case cudaErrorNotReady: return "This indicates that asynchronous operations issued previously have not completed yet. This result is not actually an error, but must be indicated differently than cudaSuccess. Calls that may return this value include cudaEventQuery() and cudaStreamQuery().";
        case cudaErrorIllegalAddress: return "The device encountered a load or store instruction on an invalid memory address. This leaves the process in an inconsistent state and any further CUDA work will return the same error. To continue using CUDA, the process must be terminated and relaunched.";
        case cudaErrorLaunchOutOfResources: return "This indicates that a launch did not occur because it did not have appropriate resources. Although this error is similar to cudaErrorInvalidConfiguration, this error usually indicates that the user has attempted to pass too many arguments to the device kernel, or the kernel launch specifies too many threads for the kernel's register count.";
        case cudaErrorLaunchTimeout: return "This indicates that the device kernel took too long to execute. This can only occur if timeouts are enabled. This leaves the process in an inconsistent state and any further CUDA work will return the same error. To continue using CUDA, the process must be terminated and relaunched.";
        case cudaErrorLaunchIncompatibleTexturing: return "This error indicates a kernel launch that uses an incompatible texturing mode.";
        case cudaErrorPeerAccessAlreadyEnabled: return "This error indicates that a call to cudaDeviceEnablePeerAccess() is trying to re-enable peer addressing on from a context which has already had peer addressing enabled.";
        case cudaErrorPeerAccessNotEnabled: return "This error indicates that cudaDeviceDisablePeerAccess() is trying to disable peer addressing which has not been enabled yet via cudaDeviceEnablePeerAccess().";
        case cudaErrorSetOnActiveProcess: return "This indicates that the user has called cudaSetValidDevices(), cudaSetDeviceFlags(), or similar after initializing the CUDA runtime by calling non-device management operations.";
        case cudaErrorContextIsDestroyed: return "This error indicates that the context current to the calling thread has been destroyed using cuCtxDestroy, or is a primary context which has not yet been initialized.";
        case cudaErrorAssert: return "An assert triggered in device code during kernel execution. The device cannot be used again. All existing allocations are invalid. To continue using CUDA, the process must be terminated and relaunched.";
        case cudaErrorTooManyPeers: return "This error indicates that the hardware resources required to enable peer access have been exhausted for one or more of the devices passed to cudaEnablePeerAccess().";
        case cudaErrorHostMemoryAlreadyRegistered: return "This error indicates that the memory range passed to cudaHostRegister() has already been registered.";
        case cudaErrorHostMemoryNotRegistered: return "This error indicates that the pointer passed to cudaHostUnregister() does not correspond to any currently registered memory region.";
        case cudaErrorHardwareStackError: return "Device encountered an error in the call stack during kernel execution, possibly due to stack corruption or exceeding the stack size limit. This leaves the process in an inconsistent state and any further CUDA work will return the same error. To continue using CUDA, the process must be terminated and relaunched.";
        case cudaErrorIllegalInstruction: return "The device encountered an illegal instruction during kernel execution. This leaves the process in an inconsistent state and any further CUDA work will return the same error. To continue using CUDA, the process must be terminated and relaunched.";
        case cudaErrorMisalignedAddress: return "The device encountered a load or store instruction on a memory address which is not aligned. This leaves the process in an inconsistent state and any further CUDA work will return the same error. To continue using CUDA, the process must be terminated and relaunched.";
        case cudaErrorInvalidAddressSpace: return "While executing a kernel, the device encountered an instruction which can only operate on memory locations in certain address spaces (global, shared, or local), but was supplied a memory address not belonging to an allowed address space. This leaves the process in an inconsistent state and any further CUDA work will return the same error. To continue using CUDA, the process must be terminated and relaunched.";
        case cudaErrorInvalidPc: return "The device encountered an invalid program counter. This leaves the process in an inconsistent state and any further CUDA work will return the same error. To continue using CUDA, the process must be terminated and relaunched.";
        case cudaErrorLaunchFailure: return "An exception occurred on the device while executing a kernel. Common causes include dereferencing an invalid device pointer and accessing out of bounds shared memory. This leaves the process in an inconsistent state and any further CUDA work will return the same error. To continue using CUDA, the process must be terminated and relaunched.";
        case cudaErrorCooperativeLaunchTooLarge: return "This error indicates that the number of blocks launched per grid for a kernel that was launched via either cudaLaunchCooperativeKernel exceeds the maximum number of blocks allowed.";
        case cudaErrorTensorMemoryLeak: return "An exception occurred on the device while exiting a kernel using tensor memory: the tensor memory was not completely deallocated. This leaves the process in an inconsistent state and any further CUDA work will return the same error. To continue using CUDA, the process must be terminated and relaunched.";
        case cudaErrorNotPermitted: return "This error indicates the attempted operation is not permitted.";
        case cudaErrorNotSupported: return "This error indicates the attempted operation is not supported on the current system or device.";
        case cudaErrorSystemNotReady: return "This error indicates that the system is not yet ready to start any CUDA work. To continue using CUDA, verify the system configuration is in a valid state and all required driver daemons are actively running.";
        case cudaErrorSystemDriverMismatch: return "This error indicates that there is a mismatch between the versions of the display driver and the CUDA driver.";
        case cudaErrorCompatNotSupportedOnDevice: return "This error indicates that the system was upgraded to run with forward compatibility but the visible hardware detected by CUDA does not support this configuration.";
        case cudaErrorMpsConnectionFailed: return "This error indicates that the MPS client failed to connect to the MPS control daemon or the MPS server.";
        case cudaErrorMpsRpcFailure: return "This error indicates that the remote procedural call between the MPS server and the MPS client failed.";
        case cudaErrorMpsServerNotReady: return "This error indicates that the MPS server is not ready to accept new MPS client requests. This error can be returned when the MPS server is in the process of recovering from a fatal failure.";
        case cudaErrorMpsMaxClientsReached: return "This error indicates that the hardware resources required to create MPS client have been exhausted.";
        case cudaErrorMpsMaxConnectionsReached: return "This error indicates that the hardware resources required for device connections have been exhausted.";
        case cudaErrorMpsClientTerminated: return "This error indicates that the MPS client has been terminated by the server. To continue using CUDA, the process must be terminated and relaunched.";
        case cudaErrorCdpNotSupported: return "This error indicates that the program is using CUDA Dynamic Parallelism, but the current configuration (like MPS) does not support it.";
        case cudaErrorCdpVersionMismatch: return "This error indicates that the program contains an unsupported interaction between different versions of CUDA Dynamic Parallelism.";
        case cudaErrorStreamCaptureUnsupported: return "The operation is not permitted when the stream is capturing.";
        case cudaErrorStreamCaptureInvalidated: return "The current capture sequence on the stream has been invalidated due to a previous error.";
        case cudaErrorStreamCaptureMerge: return "The operation would have resulted in a merge of two independent capture sequences.";
        case cudaErrorStreamCaptureUnmatched: return "The capture was not initiated in this stream.";
        case cudaErrorStreamCaptureUnjoined: return "The capture sequence contains a fork that was not joined to the primary stream.";
        case cudaErrorStreamCaptureIsolation: return "A dependency would have been created which crosses the capture sequence boundary. Only implicit in-stream ordering dependencies are allowed to cross the boundary.";
        case cudaErrorStreamCaptureImplicit: return "The operation would have resulted in a disallowed implicit dependency on a current capture sequence from cudaStreamLegacy.";
        case cudaErrorCapturedEvent: return "The operation is not permitted on an event which was last recorded in a capturing stream.";
        case cudaErrorStreamCaptureWrongThread: return "A stream capture sequence not initiated with the cudaStreamCaptureModeRelaxed argument to cudaStreamBeginCapture was passed to cudaStreamEndCapture in a different thread.";
        case cudaErrorTimeout: return "This indicates that the wait operation has timed out.";
        case cudaErrorGraphExecUpdateFailure: return "This error indicates that the graph update was not performed because it included changes which violated constraints specific to instantiated graph update.";
        case cudaErrorExternalDevice: return "This indicates that an error has occurred in a device outside of GPU. This leaves the process in an inconsistent state and any further CUDA work will return the same error. To continue using CUDA, the process must be terminated and relaunched.";
        case cudaErrorInvalidClusterSize: return "This indicates that a kernel launch error has occurred due to cluster misconfiguration.";
        case cudaErrorFunctionNotLoaded: return "Indicates a function handle is not loaded when calling an API that requires a loaded function.";
        case cudaErrorInvalidResourceType: return "This error indicates one or more resources passed in are not valid resource types for the operation.";
        case cudaErrorInvalidResourceConfiguration: return "This error indicates one or more resources are insufficient or non-applicable for the operation.";
        case cudaErrorStreamDetached: return "This error indicates that the requested operation is not permitted because the stream is in a detached state. This can occur if the green context associated with the stream has been destroyed.";
        case cudaErrorGraphRecaptureFailure: return "This error indicates that a graph recapture failed and had to be terminated.";
        case cudaErrorFabricNotReady: return "This error indicates GPU fabric is not ready within the bounded wait while the fabric manager probe is still in progress. Applications may retry after a delay.";
        case cudaErrorUnknown: return "This indicates that an unknown internal error has occurred.";
        case cudaErrorApiFailureBase: return "Any unhandled CUDA driver error is added to this value and returned via the runtime. Production releases of CUDA should not return such errors. Deprecated as of CUDA 4.1.";
        default: return "UNKNOWN CUDA ERROR";
        }
    }

private:
    static std::string BuildMessage(cudaError_t r, const char* call, const char* file, int line, const char* func)
    {
        std::ostringstream oss;
        oss << "\n"
            << "CUDA FATAL ERROR\n"
            << "----------------------------------------\n"
            << " Call   : " << call << "\n"
            << " Result : " << cudaGetErrorName(r) << " (" << static_cast<int>(r) << ")\n"
            << " Error Type: " << cudaGetErrorString(r) << "\n"
            << " Message Description: " << GetCudaError(r) << "\n"
            << " File   : " << file << ":" << line << "\n"
            << " Func   : " << func << "\n"
            << "----------------------------------------\n";
        return oss.str();
    }
};

#define CUDA_THROW_IF_FAIL(call) \
    do { \
        cudaError_t cuda_res = (call); \
        if (cuda_res != cudaSuccess) [[unlikely]] { \
            throw CudaError(cuda_res, #call, __FILE__, __LINE__, __func__); \
        } \
    } while (0)