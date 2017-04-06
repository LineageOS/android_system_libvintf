/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef ANDROID_VINTF_HAL_MANIFEST_H
#define ANDROID_VINTF_HAL_MANIFEST_H

#include <map>
#include <string>
#include <utils/Errors.h>
#include <vector>

#include "ManifestHal.h"
#include "MapValueIterator.h"
#include "SchemaType.h"
#include "Version.h"

namespace android {
namespace vintf {

struct CompatibilityMatrix;

// A HalManifest is reported by the hardware and query-able from
// framework code. This is the API for the framework.
struct HalManifest {
public:

    // manifest.version
    constexpr static Version kVersion{1, 0};

    // Construct a device HAL manifest.
    HalManifest() : mType(SchemaType::DEVICE) {}

    // Given a component name (e.g. "android.hardware.camera"),
    // return getHal(name)->transport if the component exist and v exactly matches
    // one of the versions in that component, else EMPTY
    Transport getTransport(const std::string &name, const Version &v,
            const std::string &interfaceName, const std::string &instanceName) const;

    // Given a component name (e.g. "android.hardware.camera"),
    // return a list of version numbers that are supported by the hardware.
    // If the component is not found, empty list is returned.
    const std::vector<Version> &getSupportedVersions(const std::string &name) const;

    // Given a component name (e.g. "android.hardware.camera") and an interface
    // name, return all instance names for that interface.
    // * If the component ("android.hardware.camera") does not exist, return empty list
    // * If the component ("android.hardware.camera") does exist,
    //    * If the interface (ICamera) does not exist, return {"default"}
    //    * Else return the list hal.interface.instance
    const std::set<std::string> &getInstances(
            const std::string &halName, const std::string &interfaceName) const;

    // Convenience method for checking if instanceName is in getInstances(halName, interfaceName)
    bool hasInstance(const std::string &halName,
            const std::string &interfaceName, const std::string &instanceName) const;

    // Return a list of component names that does NOT conform to
    // the given compatibility matrix. It may contain components that are optional
    // for the framework.
    std::vector<std::string> checkIncompatiblity(const CompatibilityMatrix &mat) const;

    // Add an hal to this manifest so that a HalManifest can be constructed programatically.
    bool add(ManifestHal &&hal);

    // Get an HAL component based on the component name. Return nullptr
    // if the component does not exist. The component name looks like:
    // android.hardware.foo
    const ManifestHal *getHal(const std::string &name) const;
    ManifestHal *getHal(const std::string &name);

    // Returns all component names.
    std::set<std::string> getHalNames() const;

    // Given a component name (e.g. "android.hardware.camera"),
    // return a list of interface names of that component.
    // If the component is not found, empty list is returned.
    std::set<std::string> getInterfaceNames(const std::string &name) const;

private:
    friend struct HalManifestConverter;
    friend class VintfObject;
    friend class AssembleVintf;
    friend struct LibVintfTest;
    friend std::string dump(const HalManifest &vm);

    // clear this manifest.
    inline void clear() { mHals.clear(); }

    // Return an iterable to all ManifestHal objects. Call it as follows:
    // for (const ManifestHal &e : vm.getHals()) { }
    ConstMapValueIterable<std::string, ManifestHal> getHals() const;

    status_t fetchAllInformation(const std::string &path);

    SchemaType mType;

    // sorted map from component name to the component.
    // The component name looks like: android.hardware.foo
    std::map<std::string, ManifestHal> mHals;

    // entries for device hal manifest only
    struct {
        Version mSepolicyVersion;
    } device;
};


} // namespace vintf
} // namespace android

#endif // ANDROID_VINTF_HAL_MANIFEST_H
