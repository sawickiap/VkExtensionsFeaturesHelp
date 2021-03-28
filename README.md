**Vulkan Extensions & Features Help**, or **VkExtensionsFeaturesHelp**, is a small, header-only, C++ library for developers who use Vulkan API. It helps to avoid boilerplate code while creating `VkInstance` and `VkDevice` object by providing a convenient way to query and then enable:

- instance layers
- instance extensions
- instance feature structures
- device features
- device extensions
- device feature structures

The library provides a domain-specific language to describe the list of required or supported extensions, features, and layers. The language is fully defined in terms of preprocessor macros, so no custom build step is needed.

Author: Adam Sawicki - https://asawicki.info 
Version: 1.0.0, 2021-03-28 
License: MIT (see file: [LICENSE](LICENSE))

## Documentation

- **[User Guide](UserGuide.md)**
- **[Reference](Reference.md)**

## Quick example

- Copy file "VkExtensionsFeaturesHelp.hpp" into your project and `#include` it.
- Create file "VkExtensionsFeatures.inl" and put appropriate macros there to define device extensions and features your program supports, e.g.:

```cpp
VKEFH_DEVICE_EXTENSION(VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME)
VKEFH_DEVICE_FEATURE_STRUCT(VkPhysicalDeviceMemoryPriorityFeaturesEXT,
    VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PRIORITY_FEATURES_EXT)
```

- Use class `VKEFH::DeviceInitHelp` to conveniently create Vulkan device object:

```cpp
VKEFH::DeviceInitHelp devInitHelp;
devInitHelp.GetPhysicalDeviceFeatures(physicalDevice);
devInitHelp.EnumerateExtensions(physicalDevice);

bool memoryPrioritySupported =
    devInitHelp.IsExtensionSupported(VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME) &&
    devInitHelp.GetVkPhysicalDeviceMemoryPriorityFeaturesEXT().memoryPriority;

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

## Example integration

To see example integration of this library and check how much it simplifies the code, visit following forked repositories:

- [Wicked Engine](https://github.com/sawickiap/WickedEngine/tree/experiment-VkExtensionsFeaturesHelp-integration) ([see code comparison](https://github.com/sawickiap/WickedEngine/compare/16fe939..a98e136))
- [Vulkan Memory Allocator](https://github.com/sawickiap/VulkanMemoryAllocator/tree/experiment-VkExtensionsFeaturesIntegration) library and its sample app from AMD ([see code comparison](https://github.com/sawickiap/VulkanMemoryAllocator/compare/d1851f0..f944c4d))
- [Cauldron](https://github.com/sawickiap/Cauldron/tree/experiment-VkExtensionsFeaturesHelp-integration) framework with CACAO effect sample by AMD ([see code comparison](https://github.com/sawickiap/Cauldron/compare/e850540..69f9cdf))
