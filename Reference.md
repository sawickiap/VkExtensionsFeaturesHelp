# VkExtensionsFeaturesHelp Reference

This document provides description of the entire API of the library. For step-by-step guide, see the other document: **[User Guide](UserGuide.md)**. See also main page: **[README](README.md)**.

All names of structures, extensions, layers passed to the library as strings are case-sensitive.

## Definition file

To use the library, you need to create *definition file* - file **"VkExtensionsFeatures.inl"** in the same directory as the library source code. It is `#include`-d by the library multiple times, for different definitions of the macros used in it.

It is actually a C++ source file where you should only use specific macros, documented below. Their purpose is to define list of extensions, layers, and feature structures that are required or optionally used by your program. Their order doesn't matter. They shall not have trailing semicolon `;`.

You can also use other preprocessor macros, like `#ifdef _WIN32` to enable some parts conditionally depending on platform or other project configuration.

**VKEFH_INSTANCE_EXTENSION(const char\* extensionName)**

Defines Vulkan instance extension used by your program.

- *extensionName* - Name of the extension. It must be a string. It may be a literal string, e.g. `"VK_KHR_surface"`, or appropriate constant as defined in Vulkan headers, e.g. `VK_KHR_SURFACE_EXTENSION_NAME`.

**VKEFH_INSTANCE_LAYER(const char\* layerName)**

Defines Vulkan instance layer used by your program.

- *layerName* - Name of the layer. It must be a string, e.g. `"VK_LAYER_KHRONOS_validation"`.

**VKEFH_INSTANCE_FEATURE_STRUCT(SYMBOL structName, VkStructureType sType)**

Defines Vulkan instance feature structure used by your program.

- *structName* - Name of the structure, e.g. `VkValidationFeaturesEXT`. It must be a C++ symbol, not a string.
- *sType* - Enum value that identfies type of this structure, to be passed in its `sType` member. It must match the structure as defined in Vulkan specification.

**VKEFH_DEVICE_EXTENSION(const char\* extensionName)**

Defines Vulkan device extension used by your program.

- *extensionName* - Name of the extension. It must be a string. It may be a literal string, e.g. `"VK_KHR_swapchain"`, or appropriate constant as defined in Vulkan headers, e.g. `VK_KHR_SWAPCHAIN_EXTENSION_NAME`.

**VKEFH_DEVICE_FEATURE_STRUCT(SYMBOL structName, VkStructureType sType)**

Defines Vulkan device feature structure used by your program.

- *structName* - Name of the structure, e.g. `VkPhysicalDeviceMemoryPriorityFeaturesEXT`. It must be a C++ symbol, not a string.
- *sType* - Enum value that identfies type of this structure, to be passed in its `sType` member, e.g. `VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PRIORITY_FEATURES_EXT`. It must match the structure as defined in Vulkan specification.

## Library API

All members of the library are defined inside namespace `VKEFH`, assumed implicitly from now on. There are 2 classes provided for direct use. They use inheritance from a base class, but this is not important and so not described here.

Members described below are sorted roughly by the order in which you should call them, not alphabetically.

### class InstanceInitHelp

Provides help in creation of `VkInstance` object. You should create only one object of this type and use it for filling structure `VkInstanceCreateInfo`.

**InstanceInitHelp()**

Default constructor. Initializes the object to an empty state.

**Reset()**

Resets the object back to its empty state, like after creation.

**VkResult EnumerateLayers()**

Uses `vkEnumerateInstanceLayerProperties` to inspect the list of available instance layers. You must call it once at the beginning.

**VkResult EnumerateExtensions()**

Uses `vkEnumerateInstanceExtensionProperties` to inspect the list of available instance extensions. You must call it once at the beginning.

**bool IsExtensionSupported(const char\* extensionName) const**

Returns `true` if given extension was found available in the current system. You can call it only after `EnumerateExtensions`. You can call it only for instance extensions specified in the definition file.

**bool IsExtensionEnabled(const char\* extensionName) const**

Returns `true` if given extension is enabled. Extensions are enabled by default when they are supported. You can call it only after `EnumerateExtensions`. You can call it only for instance extensions specified in the definition file.

**bool EnableExtension(const char\* extensionName, bool enabled)**

Enables or disabled given extension. Returns `true` if `enabled == true` and the extension is supported so it can be enabled. For `enabled == false` always returns `false`. You can call it only after `EnumerateExtensions`. You can call it only for instance extensions specified in the definition file.

**void EnableAllExtensions(bool enabled)**

Enables or disables all supported extensions. You can call it only after `EnumerateExtensions`.

**bool IsLayerSupported(const char\* layerName) const**

Returns `true` if given layer was found available in the current system. You can call it only after `EnumerateLayers`. You can call it only for instance layers specified in the definition file.

**bool IsLayerEnabled(const char\* layerName) const**

Returns `true` if given layer is enabled. Layers are enabled by default when they are supported. You can call it only after `EnumerateLayers`. You can call it only for instance layers specified in the definition file.

**bool EnableLayer(const char\* layerName, bool enabled)**

Enables or disabled given layer. Returns `true` if `enabled == true` and the layer is supported so it can be enabled. For `enabled == false` always returns `false`. You can call it only after `EnumerateLayers`. You can call it only for instance layers specified in the definition file.

**void EnableAllLayers(bool enabled)**

Enables or disables all supported layers. You can call it only after `EnumerateLayers`.

**bool IsFeatureStructEnabled(const char\* structName) const**

Returns `true` if given feature structure, identified by its name passed as a string, is enabled. All feature structures are enabled by default. You can call it only for instance feature structures specified in the definition file.

**void EnableFeatureStruct(const char\* structName, bool enabled)**

Enables or disabled given feature structure, identified by its name passed as a string, to control whether it should be added to `VkInstanceCreateInfo::pNext` chain. You can call it only for instance feature structures specified in the definition file.

**bool IsFeatureStructEnabled(VkStructureType sType) const**

Returns `true` if given feature structure, identified by Vulkan structure type enum, is enabled. All feature structures are enabled by default. You can call it only for instance feature structures specified in the definition file.

**void EnableFeatureStruct(VkStructureType sType, bool enabled)**

Enables or disabled given feature structure, identified by Vulkan structure type enum, to control whether it should be added to `VkInstanceCreateInfo::pNext` chain. You can call it only for instance feature structures specified in the definition file.

**void EnableAllFeatureStructs(bool enabled)**

Enables or disables all instance feature structures specified in the definition file.

**STRUCT_NAME& Get##STRUCT_NAME()**<br>
**const STRUCT_NAME& Get##STRUCT_NAME() const**

Returns reference to a given feature structure, so it can be altered before it is passed as `VkInstanceCreateInfo::pNext`. You shall only disable features, not enable those that are `VK_FALSE` already and so not supported.

**void PrepareCreation()**

Prepares internal data needed for creation of `VkInstance` object. You can call it only after `EnumerateExtensions` and `EnumerateLayers`. You should enable/disable extensions, layers, and feature structures as required before this call.

**const void\* GetFeaturesChain() const**

Returns parameter to be passed as `VkInstanceCreateInfo::pNext`. It must be called after `PrepareCreation`. Returned pointer is valid only as long as this object remains alive and unchanged.

**uint32_t GetEnabledExtensionCount() const**

Returns parameter to be passed as `VkInstanceCreateInfo::enabledExtensionsCount`. It must be called after `PrepareCreation`.

**const char\* const\* GetEnabledExtensionNames() const**

Returns parameter to be passed as `VkInstanceCreateInfo::enabledExtensionNames`. It must be called after `PrepareCreation`. Returned pointer is valid only as long as this object remains alive and unchanged.

**uint32_t GetEnabledLayerCount() const**

Returns parameter to be passed as `VkInstanceCreateInfo::enabledLayerCount`. It must be called after `PrepareCreation`.

**const char\* const\* GetEnabledLayerNames() const**

Returns parameter to be passed as `VkInstanceCreateInfo::enabledLayerNames`. It must be called after `PrepareCreation`. Returned pointer is valid only as long as this object remains alive and unchanged.

### class DeviceInitHelp

Provides help in creation of `VkDevice` object. You should use it for filling structure `VkDeviceCreateInfo`.

**DeviceInitHelp()**

Default constructor. Initializes the object to an empty state.

**Reset()**

Resets the object back to its empty state, like after creation.

**void GetPhysicalDeviceFeatures(VkPhysicalDevice physicalDevice)**

Uses `vkGetPhysicalDeviceFeatures2` to inspect the list of available device features. You must call it once at the beginning.

**VkResult EnumerateExtensions(VkPhysicalDevice physicalDevice)**

Uses `vkEnumerateDeviceExtensionProperties` to inspect the list of available device extensions. You must call it once at the beginning.

**bool IsExtensionSupported(const char\* extensionName) const**

Returns `true` if given extension was found available in the current system. You can call it only after `EnumerateExtensions`. You can call it only for device extensions specified in the definition file.

**bool IsExtensionEnabled(const char\* extensionName) const**

Returns `true` if given extension is enabled. Extensions are enabled by default when they are supported. You can call it only after `EnumerateExtensions`. You can call it only for device extensions specified in the definition file.

**bool EnableExtension(const char\* extensionName, bool enabled)**

Enables or disabled given extension. Returns `true` if `enabled == true` and the extension is supported so it can be enabled. For `enabled == false` always returns `false`. You can call it only after `EnumerateExtensions`. You can call it only for device extensions specified in the definition file.

**void EnableAllExtensions(bool enabled)**

Enables or disables all supported extensions. You can call it only after `EnumerateExtensions`.

**bool IsFeatureStructEnabled(const char\* structName) const**

Returns `true` if given feature structure, identified by its name passed as a string, is enabled. All feature structures are enabled by default. You can call it only for device feature structures specified in the definition file.

**bool IsFeatureStructEnabled(VkStructureType sType) const**

Returns `true` if given feature structure, identified by Vulkan structure type enum, is enabled. All feature structures are enabled by default. You can call it only for device feature structures specified in the definition file.

**void EnableFeatureStruct(const char\* structName, bool enabled)**

Enables or disabled given feature structure, identified by its name passed as a string, to control whether it should be added to `vkGetPhysicalDeviceFeatures2` (if called before `GetPhysicalDeviceFeatures`) or `VkDeviceCreateInfo::pNext` chain (if called before `PrepareCreation`). You can call it only for device feature structures specified in the definition file.

**void EnableFeatureStruct(VkStructureType sType, bool enabled)**

Enables or disabled given feature structure, identified by Vulkan structure type enum, to control whether it should be added to `vkGetPhysicalDeviceFeatures2` (if called before `GetPhysicalDeviceFeatures`) or `VkDeviceCreateInfo::pNext` chain (if called before `PrepareCreation`). You can call it only for device feature structures specified in the definition file.

**void EnableAllFeatureStructs(bool enabled)**

Enables or disables all device feature structures specified in the definition file.

**STRUCT_NAME& Get##STRUCT_NAME()**<br>
**const STRUCT_NAME& Get##STRUCT_NAME() const**

Returns reference to a given feature structure, so you can inspect it to see which specific features are supported, as well as alter it before it is passed as `VkDeviceCreateInfo::pNext`. You can call it only after `GetPhysicalDeviceFeatures`. You shall only disable features, not enable those that are `VK_FALSE` already and so not supported.

**VkPhysicalDeviceFeatures& GetFeatures()**<br>
**const VkPhysicalDeviceFeatures& GetFeatures() const**

Returns reference to the structure with switches for standard Vulkan features, so you can inspect it to see which features are supported, as well as alter it before it is passed to device creation. You can call it only after `GetPhysicalDeviceFeatures`. You shall only disable features, not enable those that are `VK_FALSE` already and so not supported.

**void PrepareCreation()**

Prepares internal data needed for creation of `VkDevice` object. You can call it only after `EnumerateExtensions` and `GetPhysicalDeviceFeatures`. You should enable/disable extensions, feature structures, and specific features as required before this call.

**const void\* GetFeaturesChain() const**

Returns parameter to be passed as `VkDeviceCreateInfo::pNext`. It must be called after `PrepareCreation`. Returned pointer is valid only as long as this object remains alive and unchanged.

**uint32_t GetEnabledExtensionCount() const**

Returns parameter to be passed as `VkDeviceCreateInfo::enabledExtensionsCount`. It must be called after `PrepareCreation`.

**const char\* const\* GetEnabledExtensionNames() const**

Returns parameter to be passed as `VkDeviceCreateInfo::enabledExtensionNames`. It must be called after `PrepareCreation`. Returned pointer is valid only as long as this object remains alive and unchanged.
