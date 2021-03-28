/*
VkExtensionsFeaturesHelp - Small header-only C++ library that helps to initialize Vulkan instance and device object

Author:  Adam Sawicki - https://asawicki.info - adam__DELETE__@asawicki.info
Version: 1.0.0, 2021-03-28
License: MIT

Documentation: see README.md and other .md files in the repository or online on GitHub:
https://github.com/sawickiap/VkExtensionsFeaturesHelp

# Version history

Version 1.0.0, 2021-03-28

    First version.

# License

Copyright 2021 Adam Sawicki

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#pragma once

#include <vector>
#include <cstdint>
#include <cassert>
#include <cstring>

namespace VKEFH
{

class EnabledItemVector
{
public:
    struct EnabledItem
    {
        const char* m_Name;
        bool m_Supported, m_Enabled;
    };
    std::vector<EnabledItem> m_Items;
    std::vector<const char*> m_EnabledItemNames;

	void Reset()
	{
		for(size_t i = 0, count = m_Items.size(); i < count; ++i)
		{
			m_Items[i].m_Supported = false;
			m_Items[i].m_Enabled = false;
		}
		m_EnabledItemNames.clear();
	}

    bool IsSupported(const char* name) const
    {
        size_t index = Find(name);
        if(index != SIZE_MAX)
            return m_Items[index].m_Supported;
        assert(0 && "You can query only for items specified in VkExtensionsFeatures.inl.");
        return false;
    }
    bool IsEnabled(const char* name) const
    {
        size_t index = Find(name);
        if(index != SIZE_MAX)
            return m_Items[index].m_Enabled;
        assert(0 && "You can query only for items specified in VkExtensionsFeatures.inl.");
        return false;
    }
    bool Enable(const char* name, bool enabled)
    {
        size_t index = Find(name);
        if(index != SIZE_MAX)
        {
            if(enabled && m_Items[index].m_Supported)
            {
                m_Items[index].m_Enabled = true;
                return true;
            }
            m_Items[index].m_Enabled = false;
            return false;
        }
        assert(0 && "You can enable only for items specified in VkExtensionsFeatures.inl.");
        return false;
    }
    void EnableAll(bool enabled)
    {
        for(size_t i = 0, count = m_Items.size(); i < count; ++i)
        {
            m_Items[i].m_Enabled = enabled && m_Items[i].m_Supported;
        }
    }
    void PrepareEnabled()
    {
        m_EnabledItemNames.clear();
        for(size_t i = 0, count = m_Items.size(); i < count; ++i)
        {
            if(m_Items[i].m_Enabled)
            {
                assert(m_Items[i].m_Supported);
                m_EnabledItemNames.push_back(m_Items[i].m_Name);
            }
        }
    }

private:
    size_t Find(const char* name) const
    {
        for(size_t i = 0, count = m_Items.size(); i < count; ++i)
        {
            if(strcmp(name, m_Items[i].m_Name) == 0)
            {
                return i;
            }
        }
        return SIZE_MAX;
    }
};

class InitHelpBase
{
public:
	InitHelpBase(const InitHelpBase&) = delete;
	InitHelpBase(InitHelpBase&&) = delete;
	InitHelpBase& operator=(const InitHelpBase&) = delete;
	InitHelpBase& operator=(InitHelpBase&&) = delete;

    bool IsExtensionSupported(const char* extensionName) const
    {
        assert(m_ExtensionsEnumerated && "You should call EnumerateExtensions first.");
        return m_Extensions.IsSupported(extensionName);
    }
    bool IsExtensionEnabled(const char* extensionName) const
    {
        assert(m_ExtensionsEnumerated && "You should call EnumerateExtensions first.");
        return m_Extensions.IsEnabled(extensionName);
    }
    bool EnableExtension(const char* extensionName, bool enabled)
    {
        assert(m_ExtensionsEnumerated && "You should call EnumerateExtensions first.");
        return m_Extensions.Enable(extensionName, enabled);
    }
    void EnableAllExtensions(bool enabled)
    {
        assert(m_ExtensionsEnumerated && "You should call EnumerateExtensions first.");
        m_Extensions.EnableAll(enabled);
    }

    uint32_t GetEnabledExtensionCount() const
    {
        assert(m_CreationPrepared && "You need to call PrepareCreation first.");
        return (uint32_t)m_Extensions.m_EnabledItemNames.size();
    }
    const char* const* GetEnabledExtensionNames() const
    {
        assert(m_CreationPrepared && "You need to call PrepareCreation first.");
        return !m_Extensions.m_EnabledItemNames.empty() ? m_Extensions.m_EnabledItemNames.data() : nullptr;
    }

    bool IsFeatureStructEnabled(const char* structName) const
    {
        const size_t index = FindFeatureStruct(structName);
        if(index != SIZE_MAX)
        {
            return m_FeatureStructs[index].m_Enabled;
        }
        assert(0 && "You can query only for feature structs specified in VkExtensionsFeatures.inl.");
        return false;
    }
    void EnableFeatureStruct(const char* structName, bool enabled)
    {
        const size_t index = FindFeatureStruct(structName);
        if(index != SIZE_MAX)
        {
            m_FeatureStructs[index].m_Enabled = enabled;
            return;
        }
        assert(0 && "You can enable only feature structs specified in VkExtensionsFeatures.inl.");
    }

    bool IsFeatureStructEnabled(VkStructureType sType) const
    {
        const size_t index = FindFeatureStruct(sType);
        if(index != SIZE_MAX)
        {
            return m_FeatureStructs[index].m_Enabled;
        }
        assert(0 && "You can query only for feature structs specified in VkExtensionsFeatures.inl.");
        return false;
    }
    void EnableFeatureStruct(VkStructureType sType, bool enabled)
    {
        const size_t index = FindFeatureStruct(sType);
        if(index != SIZE_MAX)
        {
            m_FeatureStructs[index].m_Enabled = enabled;
            return;
        }
        assert(0 && "You can enable only feature structs specified in VkExtensionsFeatures.inl.");
    }

    void EnableAllFeatureStructs(bool enabled)
    {
        for(size_t i = 0, count = m_FeatureStructs.size(); i < count; ++i)
        {
            m_FeatureStructs[i].m_Enabled = enabled;
        }
    }

protected:
    bool m_ExtensionsEnumerated = false;
    bool m_CreationPrepared = false;
    EnabledItemVector m_Extensions;

    struct FeatureStruct
    {
        const char* m_Name;
        VkStructureType m_sType;
        VkBaseInStructure* m_StructPtr;
        bool m_Enabled;
    };
    std::vector<FeatureStruct> m_FeatureStructs;

	InitHelpBase() { }

	void Reset()
	{
		m_ExtensionsEnumerated = false;
		m_CreationPrepared = false;
		m_Extensions.Reset();
		for(size_t i = 0, count = m_FeatureStructs.size(); i < count; ++i)
			m_FeatureStructs[i].m_Enabled = true;
	}

	void LoadExtensions(const VkExtensionProperties* extProps, size_t extPropCount)
    {
        assert(!m_ExtensionsEnumerated && "You should call EnumerateExtensions only once.");
        for(size_t extPropIndex = 0; extPropIndex < extPropCount; ++extPropIndex)
        {
            for(size_t extIndex = 0, extCount = m_Extensions.m_Items.size(); extIndex < extCount; ++extIndex)
            {
                if(strcmp(extProps[extPropIndex].extensionName, m_Extensions.m_Items[extIndex].m_Name) == 0)
                {
                    m_Extensions.m_Items[extIndex].m_Supported = true;
                    m_Extensions.m_Items[extIndex].m_Enabled = true;
                }
            }
        }
        m_ExtensionsEnumerated = true;
    }

    void PrepareEnabledExtensionNames()
    {
        assert(m_ExtensionsEnumerated && "You should call EnumerateExtensions first.");
        m_Extensions.PrepareEnabled();
    }

private:
    size_t FindFeatureStruct(const char* name) const
    {
        for(size_t i = 0, count = m_FeatureStructs.size(); i < count; ++i)
        {
            if(strcmp(m_FeatureStructs[i].m_Name, name) == 0)
            {
                return i;
            }
        }
        return SIZE_MAX;
    }
    size_t FindFeatureStruct(VkStructureType sType) const
    {
        for(size_t i = 0, count = m_FeatureStructs.size(); i < count; ++i)
        {
            if(m_FeatureStructs[i].m_sType == sType)
            {
                return i;
            }
        }
        return SIZE_MAX;
    }
};

class InstanceInitHelp : public InitHelpBase
{
	InstanceInitHelp(const InstanceInitHelp&) = delete;
	InstanceInitHelp(InstanceInitHelp&&) = delete;
	InstanceInitHelp& operator=(const InstanceInitHelp&) = delete;
	InstanceInitHelp& operator=(InstanceInitHelp&&) = delete;

#define VKEFH_INSTANCE_EXTENSION(extensionName)
#define VKEFH_INSTANCE_LAYER(layerName)
#define VKEFH_INSTANCE_FEATURE_STRUCT(structName, sType) \
    private: structName m_##structName = { (sType) }; \
    public: structName& Get##structName() { return m_##structName; } \
    public: const structName& Get##structName() const { return m_##structName; }
#define VKEFH_DEVICE_EXTENSION(extensionName)
#define VKEFH_DEVICE_FEATURE_STRUCT(structName, sType)

#include "VkExtensionsFeatures.inl"

#undef VKEFH_INSTANCE_EXTENSION
#undef VKEFH_INSTANCE_LAYER
#undef VKEFH_INSTANCE_FEATURE_STRUCT
#undef VKEFH_DEVICE_EXTENSION
#undef VKEFH_DEVICE_FEATURE_STRUCT

public:
    InstanceInitHelp()
    {
#define VKEFH_INSTANCE_EXTENSION(extensionName)   m_Extensions.m_Items.push_back({(extensionName), false, false});
#define VKEFH_INSTANCE_LAYER(layerName)   m_Layers.m_Items.push_back({(layerName), false, false});
#define VKEFH_INSTANCE_FEATURE_STRUCT(structName, sType)   m_FeatureStructs.push_back({(#structName), (sType), (VkBaseInStructure*)(&m_##structName), true});
#define VKEFH_DEVICE_EXTENSION(extensionName)
#define VKEFH_DEVICE_FEATURE_STRUCT(structName, sType)

#include "VkExtensionsFeatures.inl"

#undef VKEFH_INSTANCE_EXTENSION
#undef VKEFH_INSTANCE_LAYER
#undef VKEFH_INSTANCE_FEATURE_STRUCT
#undef VKEFH_DEVICE_EXTENSION
#undef VKEFH_DEVICE_FEATURE_STRUCT
    }

	void Reset()
	{
		InitHelpBase::Reset();
		m_LayersEnumerated = false;
		m_Layers.Reset();
		m_FeaturesChain = nullptr;
	}

    VkResult EnumerateExtensions()
    {
        uint32_t extPropCount = 0;
        VkResult res = vkEnumerateInstanceExtensionProperties(nullptr, &extPropCount, nullptr);
        if(res != VK_SUCCESS)
            return res;
        if(extPropCount)
        {
            std::vector<VkExtensionProperties> extProps(extPropCount);
            res = vkEnumerateInstanceExtensionProperties(nullptr, &extPropCount, extProps.data());
            if(res != VK_SUCCESS)
                return res;
            LoadExtensions(extProps.data(), extPropCount);
        }
        return VK_SUCCESS;
    }

    VkResult EnumerateLayers()
    {
        assert(!m_LayersEnumerated && "You should call EnumerateLayers only once.");
        uint32_t layerPropCount = 0;
        VkResult res = vkEnumerateInstanceLayerProperties(&layerPropCount, nullptr);
        if(res != VK_SUCCESS)
            return res;
        if(layerPropCount)
        {
            std::vector<VkLayerProperties> layerProps(layerPropCount);
            res = vkEnumerateInstanceLayerProperties(&layerPropCount, layerProps.data());
            if(res != VK_SUCCESS)
                return res;
            LoadLayers(layerProps.data(), layerPropCount);
        }
        m_LayersEnumerated = true;
        return VK_SUCCESS;
    }

    bool IsLayerSupported(const char* layerName) const
    {
        assert(m_LayersEnumerated && "You should call EnumerateLayers first.");
        return m_Layers.IsSupported(layerName);
    }
    bool IsLayerEnabled(const char* layerName) const
    {
        assert(m_LayersEnumerated && "You should call EnumerateLayers first.");
        return m_Layers.IsEnabled(layerName);
    }
    bool EnableLayer(const char* layerName, bool enabled)
    {
        assert(m_LayersEnumerated && "You should call EnumerateLayers first.");
        return m_Layers.Enable(layerName, enabled);
    }
    void EnableAllLayers(bool enabled)
    {
        assert(m_LayersEnumerated && "You should call EnumerateLayers first.");
        m_Layers.EnableAll(enabled);
    }

    void PrepareCreation()
    {
        assert(m_LayersEnumerated && "You should call EnumerateLayers first.");
        
        PrepareEnabledExtensionNames();
        
        m_Layers.PrepareEnabled();
        
        m_FeaturesChain = nullptr;
        for(size_t structIndex = 0, structCount = m_FeatureStructs.size(); structIndex < structCount; ++structIndex)
        {
            if(m_FeatureStructs[structIndex].m_Enabled)
            {
                assert(m_FeatureStructs[structIndex].m_StructPtr->sType == m_FeatureStructs[structIndex].m_sType);
                m_FeatureStructs[structIndex].m_StructPtr->pNext = m_FeaturesChain;
                m_FeaturesChain = m_FeatureStructs[structIndex].m_StructPtr;
            }
        }

        m_CreationPrepared = true;
    }

    uint32_t GetEnabledLayerCount() const
    {
        assert(m_CreationPrepared && "You need to call PrepareCreation first.");
        return (uint32_t)m_Layers.m_EnabledItemNames.size();
    }
    const char* const* GetEnabledLayerNames() const
    {
        assert(m_CreationPrepared && "You need to call PrepareCreation first.");
        return !m_Layers.m_EnabledItemNames.empty() ? m_Layers.m_EnabledItemNames.data() : nullptr;
    }
    const void* GetFeaturesChain() const
    {
        assert(m_CreationPrepared && "You need to call PrepareCreation first.");
        return m_FeaturesChain;
    }

private:
    bool m_LayersEnumerated = false;
    EnabledItemVector m_Layers;
    VkBaseInStructure* m_FeaturesChain = nullptr;

    void LoadLayers(const VkLayerProperties* layerProps, size_t layerPropCount)
    {
        for(size_t layerPropIndex = 0; layerPropIndex < layerPropCount; ++layerPropIndex)
        {
            for(size_t layerIndex = 0, layerCount = m_Layers.m_Items.size(); layerIndex < layerCount; ++layerIndex)
            {
                if(strcmp(layerProps[layerPropIndex].layerName, m_Layers.m_Items[layerIndex].m_Name) == 0)
                {
                    m_Layers.m_Items[layerIndex].m_Supported = true;
                    m_Layers.m_Items[layerIndex].m_Enabled = true;
                }
            }
        }
    }
};

class DeviceInitHelp : public InitHelpBase
{
	DeviceInitHelp(const DeviceInitHelp&) = delete;
	DeviceInitHelp(DeviceInitHelp&&) = delete;
	DeviceInitHelp& operator=(const DeviceInitHelp&) = delete;
	DeviceInitHelp& operator=(DeviceInitHelp&&) = delete;

#define VKEFH_INSTANCE_EXTENSION(extensionName)
#define VKEFH_INSTANCE_LAYER(layerName)
#define VKEFH_INSTANCE_FEATURE_STRUCT(structName, sType)
#define VKEFH_DEVICE_EXTENSION(extensionName)
#define VKEFH_DEVICE_FEATURE_STRUCT(structName, sType) \
    private: structName m_##structName = { (sType) }; \
    public: structName& Get##structName() \
    { \
        assert(m_PhysicalDeviceFeaturesQueried && "You need to call GetPhysicalDeviceFeatures first."); \
        return m_##structName; \
    } \
    public: const structName& Get##structName() const \
    { \
        assert(m_PhysicalDeviceFeaturesQueried && "You need to call GetPhysicalDeviceFeatures first."); \
        return m_##structName; \
    }

#include "VkExtensionsFeatures.inl"

#undef VKEFH_INSTANCE_EXTENSION
#undef VKEFH_INSTANCE_LAYER
#undef VKEFH_INSTANCE_FEATURE_STRUCT
#undef VKEFH_DEVICE_EXTENSION
#undef VKEFH_DEVICE_FEATURE_STRUCT

public:
    DeviceInitHelp()
    {
#define VKEFH_INSTANCE_EXTENSION(extensionName)
#define VKEFH_INSTANCE_LAYER(layerName)
#define VKEFH_INSTANCE_FEATURE_STRUCT(structName, sType)
#define VKEFH_DEVICE_EXTENSION(extensionName)   m_Extensions.m_Items.push_back({(extensionName), false, false});
#define VKEFH_DEVICE_FEATURE_STRUCT(structName, sType)   m_FeatureStructs.push_back({(#structName), (sType), (VkBaseInStructure*)(&m_##structName), true});

#include "VkExtensionsFeatures.inl"

#undef VKEFH_INSTANCE_EXTENSION
#undef VKEFH_INSTANCE_LAYER
#undef VKEFH_INSTANCE_FEATURE_STRUCT
#undef VKEFH_DEVICE_EXTENSION
#undef VKEFH_DEVICE_FEATURE_STRUCT
    }

	void Reset()
	{
		InitHelpBase::Reset();
		m_PhysicalDeviceFeaturesQueried = false;
		m_Features2 = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
	}

    VkResult EnumerateExtensions(VkPhysicalDevice physicalDevice)
    {
        assert(physicalDevice);
        uint32_t extPropCount = 0;
        VkResult res = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extPropCount, nullptr);
        if(res != VK_SUCCESS)
        {
            return res;
        }
        std::vector<VkExtensionProperties> extProps{extPropCount};
        if(extPropCount)
        {
            res = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extPropCount, extProps.data());
            if(res != VK_SUCCESS)
            {
                return res;
            }
            LoadExtensions(extProps.data(), extPropCount);
        }
        return VK_SUCCESS;
    }

    void GetPhysicalDeviceFeatures(VkPhysicalDevice physicalDevice)
    {
        assert(physicalDevice);
        assert(!m_PhysicalDeviceFeaturesQueried && "You should call GetPhysicalDeviceFeatures only once.");

        assert(m_Features2.sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2);
        m_Features2.pNext = nullptr;

        for(size_t structIndex = 0, structCount = m_FeatureStructs.size(); structIndex < structCount; ++structIndex)
        {
            if(m_FeatureStructs[structIndex].m_Enabled)
            {
                assert(m_FeatureStructs[structIndex].m_StructPtr->sType == m_FeatureStructs[structIndex].m_sType);
                m_FeatureStructs[structIndex].m_StructPtr->pNext = (VkBaseInStructure*)m_Features2.pNext;
                m_Features2.pNext = m_FeatureStructs[structIndex].m_StructPtr;
            }
        }

        vkGetPhysicalDeviceFeatures2(physicalDevice, &m_Features2);

        m_PhysicalDeviceFeaturesQueried = true;
    }

    VkPhysicalDeviceFeatures& GetFeatures()
    {
        assert(m_PhysicalDeviceFeaturesQueried && "You need to call GetPhysicalDeviceFeatures first.");
        return m_Features2.features;
    }
	const VkPhysicalDeviceFeatures& GetFeatures() const
	{
		assert(m_PhysicalDeviceFeaturesQueried && "You need to call GetPhysicalDeviceFeatures first.");
		return m_Features2.features;
	}

    void PrepareCreation()
    {
        assert(m_ExtensionsEnumerated && "You need to call EnumerateExtensions first.");
        assert(m_PhysicalDeviceFeaturesQueried && "You need to call GetPhysicalDeviceFeatures first.");

        PrepareEnabledExtensionNames();

        assert(m_Features2.sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2);
        m_Features2.pNext = nullptr;
        for(size_t structIndex = 0, structCount = m_FeatureStructs.size(); structIndex < structCount; ++structIndex)
        {
            if(m_FeatureStructs[structIndex].m_Enabled)
            {
                assert(m_FeatureStructs[structIndex].m_StructPtr->sType == m_FeatureStructs[structIndex].m_sType);
                m_FeatureStructs[structIndex].m_StructPtr->pNext = (VkBaseInStructure*)m_Features2.pNext;
                m_Features2.pNext = m_FeatureStructs[structIndex].m_StructPtr;
            }
        }
        
        m_CreationPrepared = true;
    }

    const void* GetFeaturesChain() const
    {
        assert(m_CreationPrepared && "You need to call PrepareCreation first.");
        return &m_Features2;
    }

private:
    bool m_PhysicalDeviceFeaturesQueried = false;
    VkPhysicalDeviceFeatures2 m_Features2 = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
};

} // namespace VKEFH
