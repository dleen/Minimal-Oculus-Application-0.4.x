##Introduction

This tutorial will walk you through creating a simple console application that will read out 
some values from the Oculus Rift developer kit. The idea is to show the absolute minimal amount 
of code necessary to interface with the hardware. Although it is not very useful on its 
own (no 3d graphics, etc.) it will demonstrate how easy it is to work with the Oculus Rift 
SDK and get up and running.

Before starting, make sure you have already downloaded the Oculus Rift SDK. You can get that 
here:

https://developer.oculusvr.com/?action=dl

You will also need a compiler/IDE. For this tutorial we will be using Microsoft Visual 
Studio Express 2013.

Once you have everything installed and ready we can continue.

##Project Creation

Click *File* -> *New* -> *New Project*. In the pop-up window choose *Win32 Console Application*. 

Give the project a name, like MinimalOculus. Then press*OK*.

In the Application Wizard popup, click on *Application Settings*. Under *Application Type*, 
choose *Console Application*. Under *Additional Settings*, choose *Empty Project*. Click *Finish*.

In the *Solution Explorer*, right-click on the *Source Files* folder and choose 
*Add" -> "New Item*. In the pop-up window click on *C++ File (.cpp)*, give it a name 
like *Main.cpp*, then click *Add*.

##Project Configuration

Click *Project* in the top menu, choose *Properties*. Click *Configuration Properties*. In the 
drop-down menu next to *Configuration*, choose *All Configurations*. 

Now click on *C/C++" -> "General*. In the *Additional Include Directories* box add 
`[OCULUS_SDK_LOCATION]/LibOVR/Include` and `[OCULUS_SDK_LOCATION]/LibOVR/Src`, of course 
replacing with the actual directory where the Oculus SDK lives on your machine.

Click *Linker" -> "General*. In the *Additional Library Directories* add 
`[OCULUS_SDK_LOCATION]/LibOVR/Lib/[PLATFORM]/[IDE_VERSION]`, for example: 
`[OCULUS_SDK_LOCATION]/LibOVR/Lib/Win32/VS2010`. 

On the top left drop-down menu next to *Configuration* choose *Debug*. Click 
*Linker* -> *Input*. Under *Additional Dependencies* add `libovrd.lib`, `Winmm.lib` and 
`ws2_32.lib`.

On the top left drop-down menu next to *Configuration* choose *Release*. Click 
*Linker* -> *Input*. Under *Additional Dependencies* add `libovr.lib`, `Winmm.lib` and 
`ws2_32.lib`.


Its possible that you will encounter an error while compiling:
```error LNK1104: cannot open file 'atls.lib'```

1. Download the Windows driver kit from: http://www.microsoft.com/en-us/download/details.aspx?id=11800
2. Open `LibOVR\Projects\Win32\VS2013\LibOVR.vcxproj in VS2013`.
3. Click *Project* in the top menu, choose *Properties*. Click *Configuration Properties*. In the drop-down menu next to `Configuration*, choose *All Configurations*. 
4. Now click on *VC++ Directories*. Under *Include Directories* add the path to the atl include directory of the Windows driver kit that you just installed. It will look like: `C:\WinDDK\7600.16385.1\inc\atl71`.
5. Add the library path under *Library Directories*. It should look like: `C:\WinDDK\7600.16385.1\lib\ATL\i386`.
6. Build the LibOVR project using these settings to generate new libovr.lib and libovrd.lib libraries.
7. Update your MinimalOculus project to use these new libraries.
8. Update your MinimalOculus project to use the atl includes and libraries by following steps 3, 4, and 5.

Your project should now be able to build successfully.

##Source Code

Double-click on *Main.cpp* if it is not already open. Type at the top of the file:

```
#include "OVR_CAPI.h"
#include "Kernel\OVR_Math.h"
```

This will pull in the header file needed to interface with the Oculus Rift SDK.


Next we want to start using the OVR namespace, to make the code more compact. We can do that with the following line:

```
using namespace OVR;
```

Then we can add some global variables to hold the device handles and other parameters. In a real application these would likely be members of a class, but we are using globals here just for demonstration:

```
ovrHmd hmd;
ovrFrameTiming frameTiming;
```


Now lets initialize the SDK:

```
void Init()
{
	ovr_Initialize();
	
	hmd = ovrHmd_Create(0);

	if(!hmd) return;

	ovrHmd_ConfigureTracking(hmd, 1, 0);
}
```



Next lets add a function to clean things up (this could be called in a destructor):

```
void Clear()
{
	ovrHmd_Destroy(hmd);

	ovr_Shutdown();
}
```


We also need a main function, so here it is:

```
int main()
{
	Init();

	Clear();

	return 0;
}
```


At this point you should be able to compile the application. It won't do anything yet, but if you get any errors now is a good time to double-check and make sure you followed every step. If it does compile: great! You've written the bare minimal amount of code to work with the Oculus Rift SDK. Not very exciting right now, so lets print out some parameters we get from the headset so we can confirm its working.


At the top of the file (under the OVR include) add:

```
#include <iostream>
#include <conio.h>
#include <windows.h>
```


Near the top under the OVR using directive, add:
```
using namespace std;
```


Now underneath the Clear function add this new Output function:

```
void Output()
{
    // Optional: we can overwrite the previous console to more
    // easily see changes in values
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
	GetConsoleScreenBufferInfo(h, &bufferInfo);

	while(hmd){
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
```

Finally, lets make sure the Output function gets called so we can see the values. 
We can put this in the main function, in-between the `Init();` and `Clear();` function calls:

```
Output();
```

OK. That's it! Now is the time to compile and run the application (`Control + F5`) and see 
what happens. If your Oculus Rift developer kit is plugged in, you should start seeing some 
values. Hopefully this all made sense and shows you how simple it is to work with the 
Oculus Rift SDK. You should be up and running in your own application in no time, and we 
can't wait to see what you come up with.
