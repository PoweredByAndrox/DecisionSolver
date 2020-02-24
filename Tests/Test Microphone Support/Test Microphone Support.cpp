#include "..\..\Engine\pch.h"

#include <iostream>
#pragma warning(push)
#pragma warning(disable : 4201)
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#pragma warning(pop)

#include <functiondiscoverykeys.h>
#include "..\..\Engine\WASAPICapture.h"

IMMDevice *         _Endpoint;
IAudioClient *      _AudioClient;
IAudioCaptureClient *_CaptureClient;
IMMDevice *device = NULL;
bool isDefaultDevice;
ERole deviceRole;
LPCWSTR OutputEndpoint;

wstring GetDeviceName(IMMDeviceCollection *DeviceCollection, UINT DeviceIndex)
{
	IMMDevice *device;
	LPWSTR deviceId;
	HRESULT hr;

	hr = DeviceCollection->Item(DeviceIndex, &device);
	if (FAILED(hr))
	{
		printf("Unable to get device %d: %x\n", DeviceIndex, hr);
		return NULL;
	}
	hr = device->GetId(&deviceId);
	if (FAILED(hr))
	{
		printf("Unable to get device %d id: %x\n", DeviceIndex, hr);
		return NULL;
	}

	IPropertyStore *propertyStore;
	hr = device->OpenPropertyStore(STGM_READ, &propertyStore);
	SAFE_RELEASE(device);
	if (FAILED(hr))
	{
		printf("Unable to open device %d property store: %x\n", DeviceIndex, hr);
		return NULL;
	}

	PROPVARIANT friendlyName;
	PropVariantInit(&friendlyName);
	hr = propertyStore->GetValue(PKEY_Device_FriendlyName, &friendlyName);
	SAFE_RELEASE(propertyStore);

	if (FAILED(hr))
	{
		printf("Unable to retrieve friendly name for device %d : %x\n", DeviceIndex, hr);
		return NULL;
	}

	wstring deviceName = wstring(wstring(friendlyName.vt != VT_LPWSTR ? L"Unknown" : friendlyName.pwszVal) +
		L"(" + wstring(deviceId) + L")");
	if (FAILED(hr))
	{
		printf("Unable to format friendly name for device %d : %x\n", DeviceIndex, hr);
		return NULL;
	}

	PropVariantClear(&friendlyName);
	CoTaskMemFree(deviceId);

	if (deviceName.empty())
	{
		printf("Unable to allocate buffer for return\n");
		return NULL;
	}
	return deviceName;
}


bool TestSection()
{
	USES_CONVERSION;
	HRESULT hr;
	bool retValue = true;
	IMMDeviceEnumerator *deviceEnumerator = NULL;
	IMMDeviceCollection *deviceCollection = NULL;

	isDefaultDevice = false;   // Assume we're not using the default device.

	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		printf("Unable to initialize COM: %x\n", hr);
		throw exception("Error HERE!");
	}

	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator));
	if (FAILED(hr))
	{
		printf("Unable to instantiate device enumerator: %x\n", hr);
		retValue = false;
		throw exception("Error HERE!");
	}

	//IMMDevice *device = NULL;

	//
	//  The user didn't specify an output device, prompt the user for a device and use that.
	//
	hr = deviceEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &deviceCollection);
	if (FAILED(hr))
	{
		printf("Unable to retrieve device collection: %x\n", hr);
		retValue = false;
		throw exception("Error HERE!");
	}

	UINT deviceCount;
	hr = deviceCollection->GetCount(&deviceCount);
	if (FAILED(hr))
	{
		printf("Unable to get device collection length: %x\n", hr);
		retValue = false;
		throw exception("Error HERE!");
	}
	for (UINT i = 0; i < deviceCount; i += 1)
	{
		wstring deviceName;

		deviceName = GetDeviceName(deviceCollection, i);
		if (deviceName.empty())
		{
			retValue = false;
			throw exception("Error HERE!");
		}
		MessageBoxA(0, (boost::format("    %d:  %s\n") % (i + 3) % W2A(deviceName.c_str())).str().c_str(), "", MB_OK);
	}
	const wchar_t *choice = L"2";

	long deviceIndex = 3;
	if (deviceIndex == 0)
	{
		printf("unrecognized device index: %S\n", choice);
		retValue = false;
		throw exception("Error HERE!");
	}
	hr = deviceCollection->Item(deviceIndex - 3, &device);
	if (FAILED(hr))
	{
		MessageBoxA(0, string("Unable to retrieve device %d: %x\n", deviceIndex - 3).c_str(), "Info", MB_OK);
		retValue = false;
		throw exception("Error HERE!");
	}
	if (OutputEndpoint != NULL)
	{
		hr = deviceEnumerator->GetDevice(OutputEndpoint, &device);
		if (FAILED(hr))
		{
			printf("Unable to get endpoint for endpoint %S: %x\n", OutputEndpoint, hr);
			retValue = false;
			throw exception("Error HERE!");
		}
	}
	//if (device == NULL)
	//{
	deviceRole = eCommunications;    // Assume we're using the console role.
	hr = deviceEnumerator->GetDefaultAudioEndpoint(eCapture, deviceRole, &device);
	if (FAILED(hr))
	{
		printf("Unable to get default device for role %d: %x\n", deviceRole, hr);
		retValue = false;
		throw exception("Error HERE!");
	}
	isDefaultDevice = true;
	retValue = true;
Exit:
	SAFE_RELEASE(deviceCollection);
	SAFE_RELEASE(deviceEnumerator);

	return retValue;
	//}
}
#include <xaudio2.h>
#pragma comment(lib,"xaudio2.lib")
#pragma comment(lib, "winmm.lib")
IXAudio2*					m_XAudio;
IXAudio2MasteringVoice*		m_MasteringVoice;
int TargetDurationInSec = 0;

void StartCapture()
{
	shared_ptr<CWASAPICapture> capturer = make_shared<CWASAPICapture>(device, isDefaultDevice, deviceRole);
	if (!capturer.operator bool())
	{
		MessageBoxA(0, "Unable to allocate capturer", "", MB_OK);
		//return -1;
	}

	if (capturer->Initialize(10))
	{
		//
		//  We've initialized the capturer.  Once we've done that, we know some information about the
		//  mix format and we can allocate the buffer that we're going to capture.
		//
		//
		//  The buffer is going to contain "TargetDuration" seconds worth of PCM data.  That means 
		//  we're going to have TargetDuration*samples/second frames multiplied by the frame size.
		//
		size_t captureBufferSize = capturer->SamplesPerSecond() * 10 * capturer->FrameSize();
		BYTE *captureBuffer = new (std::nothrow) BYTE[captureBufferSize];

		if (captureBuffer == NULL)
		{
			MessageBoxA(0, "Unable to allocate capturer buffer", "", MB_OK);
		}

		if (capturer->Start(captureBuffer, captureBufferSize))
		{
			do
			{
				if (TargetDurationInSec < 0) break;
				printf(".");
				Sleep(1000);
			} while (--TargetDurationInSec);
			printf("\n");
			capturer->Stop();

			HRESULT hr = S_OK;
			if (hr = XAudio2Create(&m_XAudio, 0)) throw exception("HERE");
			if (hr = m_XAudio->CreateMasteringVoice(&m_MasteringVoice)) throw exception("HERE");

			//Play
			IXAudio2SourceVoice* sorucevoice = nullptr;

			// create source voice
			m_XAudio->CreateSourceVoice(&sorucevoice, capturer->MixFormat());

			// submit source voice
			XAUDIO2_BUFFER submit = { 0 };
			submit.AudioBytes = captureBufferSize;
			submit.pAudioData = &captureBuffer[0];
			submit.Flags = XAUDIO2_END_OF_STREAM;
			sorucevoice->SubmitSourceBuffer(&submit);

			sorucevoice->Start();

			XAUDIO2_VOICE_STATE state;
			sorucevoice->GetState(&state);
			
			do
			{
				if (TargetDurationInSec < 0) break;
				printf(".");
				Sleep(1000);
			} while (--TargetDurationInSec);

			//
			//  Now shut down the capturer and release it we're done.
			//
			capturer->Shutdown();
		}

		if (m_MasteringVoice) {
			m_MasteringVoice->DestroyVoice();
			m_MasteringVoice = nullptr;
		}
		if (m_XAudio) {
			m_XAudio->Release();
			m_XAudio = nullptr;
		}
		delete[]captureBuffer;
	}
}

int main()
{
	std::cout << "Hello World!\n";
	std::cout << "This is the test micro!\n";
	std::cout << "\n";
	std::cout << "No need to do anything just input the time that would you want to start capture and say something\n";
	std::cout << "Afte tat xD you can listen to your beautiful voice) (same time to do this)!\n";

	std::cout << "\n";
	std::cout << "\n";
	std::cout << "Input your time (in sec): "; std::cin >> TargetDurationInSec;
	if (TestSection())
		StartCapture();

}
