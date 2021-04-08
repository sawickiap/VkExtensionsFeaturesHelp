# VkExtensionsFeaturesHelp User Guide

## Introduction

[Vulkan](https://www.khronos.org/vulkan/) is a cross-platform, modern, graphics API used by many games and other interactive applications. Its API, defined in C, with the distinctive idea of supporting extensions and chaining structures using `pNext` pointer, is very smart and clean, but requires lots of boilerplate code. Every Vulkan application needs to create two main objects at the beginning: `VkInstance` and `VkDevice`. Nowadays, years after Vulkan first release, most applications have a list of additional extensions and features they need to enable. VkExtensionsFeaturesHelp library provides help with this task.

This document provides step-by-step guide needed to setup and use the library. A more systematic description of its entire API can be found in the other document: **[Reference](Reference.md)**. See also main page: **[README](README.md)**.

## Prerequisites

- Written in C++, using some basic features of C++11.
- Depends only on standard C/C++ headers and Vulkan.
    - Standard headers like `<cstdint>` are included internally.
    - Vulkan header needs to be included by the user.
- Should be compatible with various platforms, compilers, 32-bit as well as 64-bit (although it was developed and tested just on Windows using Visual Studio 2019, x64).
- Errors are reported by returning `VkResult` from functions, like in Vulkan.
- Correct use of the API is validated using `assert` (can be configured using a macro).
- C++ exceptions and RTTI are not used.

## Setup

The library is single-header - not in "STB-style", where implementation needs to be extracted in one CPP file using a special macro, but it is truly header-only, with all functions inline. To use the library:

1. Copy file **"VkExtensionsFeaturesHelp.hpp"** to your project.
2. In the same directory, create file **"VkExtensionsFeatures.inl"**. We will put macros with the list of extensions and features there. It is included by the library multiple times. From now on, we will call it *definition file*.
3. `#include "VkExtensionsFeaturesHelp.hpp"` in every CPP file where you need it, especially where you create your `VkInstance` and `VkDevice` objects.

## Instance creation

Imagine we need to enable following instance extensions "VK_KHR_surface", "VK_KHR_win32_surface". We can additionally use "VK_EXT_debug_utils" if available. We also want to use instance layer "VK_LAYER_KHRONOS_validation" if available.

Without help of this library, you would need to: call `vkEnumerateInstanceExtensionProperties` to get the list of supported extensions, call `vkEnumerateInstanceLayerProperties` to get the list of supported layers, then inspect these lists, decide what to enable, create your own list of extensions and layers to enable, and finally call `vkCreateInstance` with right parameters.

### Step 1

Using this library, we start by defining required or supported extensions and layers in the definition file "VkExtensionsFeatures.inl" using appropriate macros:

```
VKEFH_INSTANCE_EXTENSION(VK_KHR_SURFACE_EXTENSION_NAME)
VKEFH_INSTANCE_EXTENSION(VK_KHR_WIN32_SURFACE_EXTENSION_NAME)
VKEFH_INSTANCE_EXTENSION(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)
VKEFH_INSTANCE_LAYER("VK_LAYER_KHRONOS_validation")
```

You can also use other preprocessor macros there, like `#ifdef _WIN32` to enable some parts of the code depending on platform or project configuration.

### Step 2

Next, inside C++ code, we need to create an object of type `InstanceInitHelp` and call appropriate methods to initialize it - to load data about supported extensions and layers from Vulkan:

```cpp
VKEFH::InstanceInitHelp instInitHelp;
VkResult res = instInitHelp.EnumerateExtensions();
// Handle error if res != VK_SUCCESS...
res = instInitHelp.EnumerateLayers();
// Handle error if res != VK_SUCCESS...
```

### Step 3

By default, every extension and layer we listed will be enabled if available. If this is the expected behavior, you can skip this step.

If some of them are required for your program to work, you can check their support like this:

```cpp
bool requiredExtensionsSupported =
    instInitHelp.IsExtensionSupported(VK_KHR_SURFACE_EXTENSION_NAME) &&
    instInitHelp.IsExtensionSupported(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
if(!requiredExtensionsSupported)
    // Handle error...
```

You can disable some of them so they won't be enabled while creating Vulkan instance despite they are supported. This may be useful e.g. if you have a set of extensions or layers that you want to enable as "all or nothing". Example:

```cpp
bool debuggingSupported =
    instInitHelp.IsExtensionSupported(VK_EXT_DEBUG_UTILS_EXTENSION_NAME) &&
    instInitHelp.IsLayerSupported("VK_LAYER_KHRONOS_validation");
if(!debuggingSupported)
{
    instInitHelp.EnableExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME, false);
    instInitHelp.EnableLayer("VK_LAYER_KHRONOS_validation", false);
}
```

You can also use opt-in approach when you don't want to enable anything unless explicitly requested. To do this:

```cpp
instInitHelp.EnableAllExtensions(false);
instInitHelp.EnableAllLayers(false);
bool debugUtilsEnabled = instInitHelp.EnableExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME, true);
```

Function `EnableExtension` enables the extension and returns `true` only if the extenion is supported. Function `EnableLayer` behaves the same way. Please note that you can use `EnableExtension`, `EnableLayer` only with the extensions/layers specified previously in the definition file.

### Step 4

You need to call `PrepareCreation` method to proceed with next step. Then you can fetch data from the library object to help fill in the structure used to create Vulkan instance.

```cpp
instInitHelp.PrepareCreation();

VkApplicationInfo appInfo = /* ... */;
VkInstanceCreateInfo instCreateInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
instCreateInfo.pNext = instInitHelp.GetFeaturesChain();
instCreateInfo.pApplicationInfo = &appInfo;
instCreateInfo.enabledExtensionCount = instInitHelp.GetEnabledExtensionCount();
instCreateInfo.ppEnabledExtensionNames = instInitHelp.GetEnabledExtensionNames();
instCreateInfo.enabledLayerCount = instInitHelp.GetEnabledLayerCount();
instCreateInfo.ppEnabledLayerNames = instInitHelp.GetEnabledLayerNames();

VkInstance instance = nullptr;
res = vkCreateInstance(&instCreateInfo, nullptr, &instance);
```

## Device creation

Second task this library can help with is the creation of `VkDevice` object. Usage of this feature is similar to what was described above. Imagine we need to enable device extension "VK_KHR_swapchain". We can also make use of extensions "VK_EXT_memory_budget" and "VK_EXT_memory_priority" if available.

Without this library, we would need to call `vkEnumerateDeviceExtensionProperties` to get the list of supported extensions and `vkGetPhysicalDeviceFeatures2` to query for supported device features. Some extensions also come with a dedicated structure that describes parameters of the support for this particular feature, like `VkPhysicalDeviceMemoryPriorityFeaturesEXT`. Such structure needs to be chained to `VkPhysicalDeviceFeatures2::pNext`. We would then inspect the list of supported extensions and the values returned in the structures, decide which extensions and features to enable, assemble a new list of extensions, and pass them, together with the chain of custom feature structures, to `VkDeviceCreateInfo` to create the device object. By the way, there is also `VkPhysicalDeviceFeatures` structure with a long list of toggles to enable/disable standard Vulkan features. Handling this all would require a lot of boilerplate code.

### Step 1

Using this library, we start by defining required or supported extensions and related structures in the definition file "VkExtensionsFeatures.inl" using appropriate macros:

```
VKEFH_DEVICE_EXTENSION(VK_KHR_SWAPCHAIN_EXTENSION_NAME)
VKEFH_DEVICE_EXTENSION(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME)
VKEFH_DEVICE_EXTENSION(VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME)
VKEFH_DEVICE_FEATURE_STRUCT(VkPhysicalDeviceMemoryPriorityFeaturesEXT,
    VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PRIORITY_FEATURES_EXT)
```

Structures describing standard features of new Vulkan versions: `VkPhysicalDeviceVulkan11Features` and `VkPhysicalDeviceVulkan12Features` can also be added this way, not only ones provided by extensions.

### Step 2

Next, inside C++ code, we need to create an object of type `DeviceInitHelp` and call appropriate methods to initialize it - to load data about supported extensions and features from given `VkPhysicalDevice`:

```cpp
VKEFH::DeviceInitHelp devInitHelp;
devInitHelp.GetPhysicalDeviceFeatures(physicalDevice);
VkResult res = instInitHelp.EnumerateExtensions(physicalDevice);
// Handle error if res != VK_SUCCESS...
```

### Step 3

By default, every extension and feature we listed in the definition file will be enabled if supported. If this is the expected behavior, you can skip this step.

If some of them are required for your program to work, you can check their support like this:

```cpp
bool requiredExtensionsSupported =
    devInitHelp.IsExtensionSupported(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
if(!requiredExtensionsSupported)
    // Handle error...
```

You can inspect the content of `VkPhysicalDeviceFeatures` structure and alter it, e.g. disable some standard Vulkan features even if supported. This is especially important with `robustBufferAccess` - a feature that is usually not required while it may slow down your program.

```cpp
VkPhysicalDeviceFeatures& devFeatures = devInitHelp.GetFeatures();
devFeatures.robustBufferAccess = VK_FALSE;
```

You can inspect the content of additional feature structures specified in the definition file in a similar way - get references to them by calling auto-generated methods `Get` + structure name, e.g.:

```cpp
bool memoryPrioritySupported =
    devInitHelp.IsExtensionSupported(VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME) &&
    devInitHelp.GetVkPhysicalDeviceMemoryPriorityFeaturesEXT().memoryPriority;
```

You can then enable and disable extensions, as well as entire structures. Disabled structures won't be chanined in `pNext` while creating device object.

```cpp
devInitHelp.EnableExtension(VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME, memoryPrioritySupported);
devInitHelp.EnableFeatureStruct("VkPhysicalDeviceMemoryPriorityFeaturesEXT", memoryPrioritySupported);
```

You can also alter the structures. For example, if you specify `VkPhysicalDeviceBufferDeviceAddressFeatures` structure in the definition file, you can use only 1 out of 3 features offered there and disable the other 2 even if supported:

```cpp
VkPhysicalDeviceBufferDeviceAddressFeatures& bufDevAddressFeatures =
    devInitHelp.GetVkPhysicalDeviceBufferDeviceAddressFeatures();
bool bufDevAddressSupported = bufDevAddressFeatures.bufferDeviceAddress != VK_FALSE;
bufDevAddressFeatures.bufferDeviceAddressCaptureReplay = VK_FALSE;
bufDevAddressFeatures.bufferDeviceAddressMultiDevice = VK_FALSE;
```

Just like with instance creation, there is a possibility to opt-in to only the extensions and structures you explicitly enable:

```cpp
devInitHelp.EnableAllExtensions(false);
devInitHelp.EnableAllFeatureStructs(false);
if(memoryPrioritySupported)
{
    devInitHelp.EnableExtension(VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME, true);
    devInitHelp.EnableFeatureStruct("VkPhysicalDeviceMemoryPriorityFeaturesEXT", true);
}
```

### Step 4

You need to call `PrepareCreation` method to proceed with next step. Then you can fetch data from the library object to help fill in the structure used to create Vulkan device.

```cpp
devInitHelp.PrepareCreation();

VkDeviceCreateInfo devCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
devCreateInfo.pNext = devInitHelp.GetFeaturesChain();
devCreateInfo.enabledExtensionCount = devInitHelp.GetEnabledExtensionCount();
devCreateInfo.ppEnabledExtensionNames = devInitHelp.GetEnabledExtensionNames();
devCreateInfo.queueCreateInfoCount = /* ... */;
devCreateInfo.pQueueCreateInfos = /* ... */;

VkDevice device = nullptr;
VkResult res = vkCreateDevice(physicalDevice, &devCreateInfo, nullptr, &device);
```

Note we don't assign `devCreateInfo.pEnabledFeatures`. Instead, `VkPhysicalDeviceFeatures` structure is automatically passed in form of `VkPhysicalDeviceFeatures2` as part of the `devCreateInfo.pNext` chain - a method recommended in modern Vulkan versions.

Instance creation supports feature structures (e.g. `VkValidationFeaturesEXT`) just like device creation, despite not shown in this document. To use them, put macro `VKEFH_INSTANCE_FEATURE_STRUCT` in the definition file and call related methods on class `InstanceInitHelp` same way as on class `DeviceInitHelp`, e.g. `IsFeatureStructEnabled`, `EnableFeatureStruct`, `EnableAllFeatureStructs`, and `Get` + structure name to get reference to its content.

## Features not supported

Following features are left out of scope of this library, at least for now:

- Calling `vkEnumerateInstanceExtensionProperties` and `vkEnumerateDeviceExtensionProperties` with `layerName != null`. Is is useful?
- Inspecting `VkLayerProperties` or `VkExtensionProperties` to check supported version of a layer/extension. Is it useful?
- Device layers. These are deprecated and ignored in modern Vulkan versions. Only instance object supports layers, not device object.
