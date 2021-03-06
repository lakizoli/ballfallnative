#ifdef _WIN32

	// Including SDKDDKVer.h defines the highest available Windows platform.

	// If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
	// set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.

	#include <SDKDDKVer.h>

	#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
	#endif

	#include <windows.h>
#elif __ANDROID__

#	include <jni.h>
#	include <errno.h>

#	include <string.h>
#	include <sys/resource.h>

#	include <android/log.h>

#	include <GLES/gl.h>
#	include <GLES/glext.h>

#	include <SLES/OpenSLES.h>
#	include <SLES/OpenSLES_Platform.h>
#	include <SLES/OpenSLES_Android.h>
#	include <SLES/OpenSLES_AndroidConfiguration.h>
#	include <SLES/OpenSLES_AndroidMetadata.h>

#elif __APPLE__

#	include <OpenGLES/ES1/gl.h>
#	include <OpenGLES/ES1/glext.h>

#endif

//Cross platform includes
#include <vector>
#include <map>
#include <set>
#include <deque>
#include <array>
#include <stack>
#include <tuple>
#include <memory>
#include <functional>
#include <algorithm>
#include <string>
#include <utility>
#include <cmath>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
