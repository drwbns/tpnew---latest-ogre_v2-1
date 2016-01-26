/*
Copyright (c) 2010 Yunus Kara

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef _SOUNDDEVICES_H
#define _SOUNDDEVICES_H

#include <vector>

#include "OgreMemoryAllocatorConfig.h"

typedef struct AlDeviceInfo
{
	std::string					strDeviceName;
	int							iMajorVersion;
	int							iMinorVersion;
	unsigned int				uiSourceCount;
	std::vector<std::string>	*pvstrExtensions;
	bool						bSelected;
} ALDEVICEINFO, *LPALDEVICEINFO;

class SoundDevices : public Ogre::GeneralAllocatedObject
{
	std::vector<ALDEVICEINFO> vDeviceInfo;
	int defaultDeviceIndex;
	int filterIndex;

public:
	SoundDevices();
	~SoundDevices();
	int getNumDevices() const;
	char* getDeviceName(int index) const;
	void getDeviceVersion(int index, int *major, int *minor) const;
	unsigned int getMaxNumSources(int index) const;
	bool isExtensionSupported(int index, char *szExtName) const;
	int getDefaultDevice() const;
	int getDeviceIndex(const char* DeviceName) const;
	
	void filterDevicesMinVer(int major, int minor);
	void filterDevicesMaxVer(int major, int minor);
	void filterDevicesExtension(char *szExtName);
	void resetFilters();
	int getFirstFilteredDevice();
	int getNextFilteredDevice();
	void logDevices() const;

private:
	static unsigned int getMaxNumSources();
};

#endif