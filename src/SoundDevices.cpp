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


#include "OgreStringConverter.h"
#include "SoundDevices.h"
#include "SoundSystem.h"

//----------------------------------------------------------------------------//
SoundDevices::SoundDevices()
{
	ALDEVICEINFO	ALDeviceInfo;
	char *devices;
	int index;
	const char *defaultDeviceName;
	const char *actualDeviceName;

	// DeviceInfo vector stores, for each enumerated device, it's device name, selection status, spec version #, and extension support
	vDeviceInfo.empty();
	vDeviceInfo.reserve(10);

	defaultDeviceIndex = 0;

	// grab function pointers for 1.0-API functions, and if successful proceed to enumerate all devices

	if (alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT")) 
	{
		devices = (char *)alcGetString(NULL, ALC_DEVICE_SPECIFIER);
		defaultDeviceName = (char *)alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
		index = 0;
		// go through device list (each device terminated with a single NULL, list terminated with double NULL)
		while (*devices != NULL) 
		{
			if (strcmp(defaultDeviceName, devices) == 0) 
			{
				defaultDeviceIndex = index;
			}
			ALCdevice *device = alcOpenDevice(devices);
			if (device) {
				ALCcontext *context = alcCreateContext(device, NULL);
				if (context) {
					alcMakeContextCurrent(context);
					// if new actual device name isn't already in the list, then add it...
					actualDeviceName = alcGetString(device, ALC_DEVICE_SPECIFIER);
					bool bNewName = (getDeviceIndex(actualDeviceName) < 0);
					if ((bNewName) && (actualDeviceName != NULL) && (strlen(actualDeviceName) > 0)) {
						memset(&ALDeviceInfo, 0, sizeof(ALDEVICEINFO));
						ALDeviceInfo.bSelected = true;
						ALDeviceInfo.strDeviceName = actualDeviceName;
						alcGetIntegerv(device, ALC_MAJOR_VERSION, sizeof(int), &ALDeviceInfo.iMajorVersion);
						alcGetIntegerv(device, ALC_MINOR_VERSION, sizeof(int), &ALDeviceInfo.iMinorVersion);

						ALDeviceInfo.pvstrExtensions = new std::vector<Ogre::String>;

						// Check for ALC Extensions
						if (alcIsExtensionPresent(device, "ALC_EXT_CAPTURE") == AL_TRUE)
							ALDeviceInfo.pvstrExtensions->push_back("ALC_EXT_CAPTURE");
						if (alcIsExtensionPresent(device, "ALC_EXT_EFX") == AL_TRUE)
							ALDeviceInfo.pvstrExtensions->push_back("ALC_EXT_EFX");

						// Check for AL Extensions
						if (alIsExtensionPresent("AL_EXT_OFFSET") == AL_TRUE)
							ALDeviceInfo.pvstrExtensions->push_back("AL_EXT_OFFSET");

						if (alIsExtensionPresent("AL_EXT_LINEAR_DISTANCE") == AL_TRUE)
							ALDeviceInfo.pvstrExtensions->push_back("AL_EXT_LINEAR_DISTANCE");
						if (alIsExtensionPresent("AL_EXT_EXPONENT_DISTANCE") == AL_TRUE)
							ALDeviceInfo.pvstrExtensions->push_back("AL_EXT_EXPONENT_DISTANCE");
						
						if (alIsExtensionPresent("EAX2.0") == AL_TRUE)
							ALDeviceInfo.pvstrExtensions->push_back("EAX2.0");
						if (alIsExtensionPresent("EAX3.0") == AL_TRUE)
							ALDeviceInfo.pvstrExtensions->push_back("EAX3.0");
						if (alIsExtensionPresent("EAX4.0") == AL_TRUE)
							ALDeviceInfo.pvstrExtensions->push_back("EAX4.0");
						if (alIsExtensionPresent("EAX5.0") == AL_TRUE)
							ALDeviceInfo.pvstrExtensions->push_back("EAX5.0");

						if (alIsExtensionPresent("EAX-RAM") == AL_TRUE)
							ALDeviceInfo.pvstrExtensions->push_back("EAX-RAM");

						// Get Source Count
						ALDeviceInfo.uiSourceCount = getMaxNumSources();

						vDeviceInfo.push_back(ALDeviceInfo);
					}
					alcMakeContextCurrent(NULL);
					alcDestroyContext(context);
				}
				alcCloseDevice(device);
			}
			devices += strlen(devices) + 1;
			index += 1;
		}
	}

	resetFilters();
}

//----------------------------------------------------------------------------//
SoundDevices::~SoundDevices()
{
	for (unsigned int i = 0; i < vDeviceInfo.size(); i++) {
		if (vDeviceInfo[i].pvstrExtensions) {
			vDeviceInfo[i].pvstrExtensions->empty();
			delete vDeviceInfo[i].pvstrExtensions;
		}
	}

	vDeviceInfo.empty();
}

//----------------------------------------------------------------------------//
int SoundDevices::getNumDevices() const
{
	return (int)vDeviceInfo.size();	
}

//----------------------------------------------------------------------------//
char * SoundDevices::getDeviceName(int index) const
{
	if (index < getNumDevices())
		return (char *)vDeviceInfo[index].strDeviceName.c_str();
	else
		return NULL;
}

//----------------------------------------------------------------------------//
void SoundDevices::getDeviceVersion(int index, int *major, int *minor) const
{
	if (index < getNumDevices()) {
		if (major)
			*major = vDeviceInfo[index].iMajorVersion;
		if (minor)
			*minor = vDeviceInfo[index].iMinorVersion;
	}
	return;
}

//----------------------------------------------------------------------------//
unsigned int SoundDevices::getMaxNumSources(int index) const
{
	if (index < getNumDevices())
		return vDeviceInfo[index].uiSourceCount;
	else
		return 0;
}

//----------------------------------------------------------------------------//
bool SoundDevices::isExtensionSupported(int index, char *szExtName) const
{
	bool bReturn = false;

	if (index < getNumDevices()) {
		for (unsigned int i = 0; i < vDeviceInfo[index].pvstrExtensions->size(); i++) {
			if (!_stricmp(vDeviceInfo[index].pvstrExtensions->at(i).c_str(), szExtName)) {
				bReturn = true;
				break;
			}				
		}
	}

	return bReturn;
}

//----------------------------------------------------------------------------//
int SoundDevices::getDefaultDevice() const
{
	return defaultDeviceIndex;
}

//----------------------------------------------------------------------------//
void SoundDevices::filterDevicesMinVer(int major, int minor)
{
	int dMajor, dMinor;
	for (unsigned int i = 0; i < vDeviceInfo.size(); i++) {
		getDeviceVersion(i, &dMajor, &dMinor);
		if ((dMajor < major) || ((dMajor == major) && (dMinor < minor))) {
			vDeviceInfo[i].bSelected = false;
		}
	}
}

//----------------------------------------------------------------------------//
void SoundDevices::filterDevicesMaxVer(int major, int minor)
{
	int dMajor, dMinor;
	for (unsigned int i = 0; i < vDeviceInfo.size(); i++) {
		getDeviceVersion(i, &dMajor, &dMinor);
		if ((dMajor > major) || ((dMajor == major) && (dMinor > minor))) {
			vDeviceInfo[i].bSelected = false;
		}
	}
}

//----------------------------------------------------------------------------//
void SoundDevices::filterDevicesExtension(char *szExtName)
{
	bool bFound;

	for (unsigned int i = 0; i < vDeviceInfo.size(); i++) {
		bFound = false;
		for (unsigned int j = 0; j < vDeviceInfo[i].pvstrExtensions->size(); j++) {
			if (!_stricmp(vDeviceInfo[i].pvstrExtensions->at(j).c_str(), szExtName)) {
				bFound = true;
				break;
			}
		}
		if (!bFound)
			vDeviceInfo[i].bSelected = false;
	}
}

//----------------------------------------------------------------------------//
void SoundDevices::resetFilters()
{
	for (int i = 0; i < getNumDevices(); i++) {
		vDeviceInfo[i].bSelected = true;
	}
	filterIndex = 0;
}

//----------------------------------------------------------------------------//
int SoundDevices::getFirstFilteredDevice()
{
	int i;

	for (i = 0; i < getNumDevices(); i++) {
		if (vDeviceInfo[i].bSelected == true) {
			break;
		}
	}
	filterIndex = i + 1;
	return i;
}

//----------------------------------------------------------------------------//
int SoundDevices::getNextFilteredDevice()
{
	int i;

	for (i = filterIndex; i < getNumDevices(); i++) {
		if (vDeviceInfo[i].bSelected == true) {
			break;
		}
	}
	filterIndex = i + 1;
	return i;
}

//----------------------------------------------------------------------------//
unsigned int SoundDevices::getMaxNumSources() const
{
	ALuint uiSources[256];
	unsigned int iSourceCount = 0;

	// Clear AL Error Code
	alGetError();

	// Generate up to 256 Sources, checking for any errors
	for (iSourceCount = 0; iSourceCount < 256; iSourceCount++)
	{
		alGenSources(1, &uiSources[iSourceCount]);
		if (alGetError() != AL_NO_ERROR)
			break;
	}

	// Release the Sources
	alDeleteSources(iSourceCount, uiSources);
	if (alGetError() != AL_NO_ERROR)
	{
		for (unsigned int i = 0; i < 256; i++)
		{
			alDeleteSources(1, &uiSources[i]);
		}
	}

	return iSourceCount;
}
//----------------------------------------------------------------------------//
void SoundDevices::logDevices() const
{
	SoundSystem& sys = SoundSystem::getSingleton();

	sys.logMessage("Sound Devices");
	sys.logMessage("-------------");
	for(size_t i = 0; i < vDeviceInfo.size(); i++)
	{
		sys.logMessage(Ogre::String("# ") + vDeviceInfo[i].strDeviceName);
		sys.logMessage(Ogre::String(" * Version: ") + Ogre::StringConverter::toString(vDeviceInfo[i].iMajorVersion) + "." + Ogre::StringConverter::toString(vDeviceInfo[i].iMinorVersion));
		sys.logMessage(Ogre::String(" * Max. Sources: ") + Ogre::StringConverter::toString(vDeviceInfo[i].uiSourceCount));
		sys.logMessage(" * Extensions: ");
		std::vector<Ogre::String> &ext = *vDeviceInfo[i].pvstrExtensions;
		for(size_t j = 0; j < ext.size(); j++)
			sys.logMessage(Ogre::String("  + ") + ext[j]);
	}
}
//----------------------------------------------------------------------------//
int SoundDevices::getDeviceIndex(const char* DeviceName) const
{
	int index = -1;
	for (int i = 0; i < getNumDevices(); i++) 
	{
		if (strcmp(getDeviceName(i), DeviceName) == 0) 
		{
			index = i;
			break;
		}
	}
	return index;
}
//----------------------------------------------------------------------------//
