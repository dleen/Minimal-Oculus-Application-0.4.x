#include <conio.h>
#include <iostream>
#include <Windows.h>

#include "OVR_CAPI.h"
#include "Kernel\OVR_Math.h"

using namespace OVR;
using namespace std;

void Init(ovrHmd &hmd)
{
	ovr_Initialize();
	hmd = ovrHmd_Create(0);
	if (!hmd) return;

	ovrHmd_ConfigureTracking(hmd, 1, 0);
}

void Clear(const ovrHmd &hmd)
{
	ovrHmd_Destroy(hmd);
	ovr_Shutdown();
}

void Output(const ovrHmd &hmd)
{
	// Optional: we can overwrite the previous console to more
	// easily see the values change
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
	GetConsoleScreenBufferInfo(h, &bufferInfo);

	ovrFrameTiming frameTiming;

	while (hmd) {
		frameTiming = ovrHmd_BeginFrameTiming(hmd, 0);
		ovrTrackingState ts = ovrHmd_GetTrackingState(hmd, frameTiming.ScanoutMidpointSeconds);

		if (ts.StatusFlags & (ovrStatus_OrientationTracked | ovrStatus_PositionTracked)) {
			// The cpp compatibility layer is used to convert ovrPosef to Posef (see OVR_Math.h)
			Posef pose = ts.HeadPose.ThePose;
			float yaw, pitch, roll;
			pose.Rotation.GetEulerAngles<Axis_Y, Axis_X, Axis_Z>(&yaw, &pitch, &roll);

			// Optional: move cursor back to starting position and print values
			SetConsoleCursorPosition(h, bufferInfo.dwCursorPosition);
			cout << "yaw: "   << RadToDegree(yaw)   << endl;
			cout << "pitch: " << RadToDegree(pitch) << endl;
			cout << "roll: "  << RadToDegree(roll)  << endl;

			Sleep(50);

			ovrHmd_EndFrameTiming(hmd);

			if (_kbhit()) exit(0);
		}
	}
}

int main()
{
	ovrHmd hmd;
	Init(hmd);
	Output(hmd);
	Clear(hmd);
	return 0;
}