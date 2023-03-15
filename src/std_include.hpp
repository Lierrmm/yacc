#pragma once

//#define BINARY_PAYLOAD_SIZE 0x0A000000
#define BASE_ADDRESS 0x400000

#define BINARY_PAYLOAD_SIZE 0xD92C000
#define TLS_PAYLOAD_SIZE 0x2000

// Decide whether to load the game as lib or to inject it
//#define INJECT_HOST_AS_LIB

#pragma warning(push)
#pragma warning(disable: 4100)
#pragma warning(disable: 4127)
#pragma warning(disable: 4244)
#pragma warning(disable: 4458)
#pragma warning(disable: 4702)
#pragma warning(disable: 4996)
#pragma warning(disable: 5054)
#pragma warning(disable: 6011)
#pragma warning(disable: 6297)
#pragma warning(disable: 6385)
#pragma warning(disable: 6386)
#pragma warning(disable: 6387)
#pragma warning(disable: 26110)
#pragma warning(disable: 26451)
#pragma warning(disable: 26444)
#pragma warning(disable: 26451)
#pragma warning(disable: 26489)
#pragma warning(disable: 26495)
#pragma warning(disable: 26498)
#pragma warning(disable: 26812)
#pragma warning(disable: 28020)

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <MsHTML.h>
#include <MsHtmHst.h>
#include <ExDisp.h>
#include <WinSock2.h>
#include <d3d9.h>
#include <WS2tcpip.h>
#include <corecrt_io.h>
#include <fcntl.h>
#include <shellapi.h>
#include <csetjmp>
#include <VersionHelpers.h>

// min and max is required by gdi, therefore NOMINMAX won't work
#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

#include <atomic>
#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <mutex>
#include <queue>
#include <regex>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include <zlib.h>
#include <zstd.h>
#include <diff.h>
#include <patch.h>
#include <tomcrypt.h>

#include <gsl/gsl>

#include <udis86.h>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "ws2_32.lib")

#pragma warning(pop)

#include <d3dx9tex.h>
#pragma comment(lib, "D3dx9.lib")

#pragma warning(disable: 4100)
#pragma warning(disable: 26812)

#include "resource.hpp"

using namespace std::literals;

extern __declspec(thread) char tls_data[TLS_PAYLOAD_SIZE];
