#define WIN32_LEAN_AND_MEAN
#include <filesystem>
#include <ShlObj_core.h>
#include <tlhelp32.h>
#include <shellapi.h>
#include <urlmon.h>
#include "resource.h"
#include <SDKDDKVer.h>
#define WINVER 0x0A00
#define _WIN32_WINNT 0x0A00
#pragma comment (lib, "urlmon.lib")
constexpr auto MAX_LOADSTRING = 100;
wchar_t b[83][MAX_PATH + 1];
SHELLEXECUTEINFOW sei;
int cb;
HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING] = L"LoLUpdater";
WCHAR szWindowClass[MAX_LOADSTRING];
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

std::wstring apimswin[] = {
	L"api-ms-win-core-console-l1-1-0.dll",
	L"api-ms-win-core-datetime-l1-1-0.dll",
	L"api-ms-win-core-debug-l1-1-0.dll",
	L"api-ms-win-core-errorhandling-l1-1-0.dll",
	L"api-ms-win-core-file-l1-1-0.dll",
	L"api-ms-win-core-file-l1-2-0.dll",
	L"api-ms-win-core-file-l2-1-0.dll",
	L"api-ms-win-core-handle-l1-1-0.dll",
	L"api-ms-win-core-heap-l1-1-0.dll",
	L"api-ms-win-core-interlocked-l1-1-0.dll",
	L"api-ms-win-core-libraryloader-l1-1-0.dll",
	L"api-ms-win-core-localization-l1-2-0.dll",
	L"api-ms-win-core-memory-l1-1-0.dll",
	L"api-ms-win-core-namedpipe-l1-1-0.dll",
	L"api-ms-win-core-processenvironment-l1-1-0.dll",
	L"api-ms-win-core-processthreads-l1-1-0.dll",
	L"api-ms-win-core-processthreads-l1-1-1.dll",
	L"api-ms-win-core-profile-l1-1-0.dll",
	L"api-ms-win-core-rtlsupport-l1-1-0.dll",
	L"api-ms-win-core-string-l1-1-0.dll",
	L"api-ms-win-core-synch-l1-1-0.dll",
	L"api-ms-win-core-synch-l1-2-0.dll",
	L"api-ms-win-core-sysinfo-l1-1-0.dll",
	L"api-ms-win-core-timezone-l1-1-0.dll",
	L"api-ms-win-core-util-l1-1-0.dll",
	L"api-ms-win-crt-conio-l1-1-0.dll",
	L"api-ms-win-crt-convert-l1-1-0.dll",
	L"api-ms-win-crt-environment-l1-1-0.dll",
	L"api-ms-win-crt-filesystem-l1-1-0.dll",
	L"api-ms-win-crt-heap-l1-1-0.dll",
	L"api-ms-win-crt-locale-l1-1-0.dll",
	L"api-ms-win-crt-math-l1-1-0.dll",
	L"api-ms-win-crt-multibyte-l1-1-0.dll",
	L"api-ms-win-crt-private-l1-1-0.dll",
	L"api-ms-win-crt-process-l1-1-0.dll",
	L"api-ms-win-crt-runtime-l1-1-0.dll",
	L"api-ms-win-crt-stdio-l1-1-0.dll",
	L"api-ms-win-crt-string-l1-1-0.dll",
	L"api-ms-win-crt-time-l1-1-0.dll",
	L"api-ms-win-crt-utility-l1-1-0.dll"
}; 

const wchar_t* box[18] = {
	L"League of Legends", L"SMITE", L"DOTA2", L"Minecraft Java",
	L"Black Desert Online", L"Paladins", L"World of Tanks", L"World of Warships", L"Lineage II",
	L"Elder Scrolls Online", L"GameLoop", L"TrackMania : Next", L"Mesen2", L"FinalBurn Neo", L"HBMAME", L"MAME", L"VCRedist AIO", L"DirectX"
};

std::wstring JoinPath(const int j, const std::wstring& add)
{
	const std::filesystem::path p = b[j];
	return (p / add).c_str();
}

void AppendFile(const int j, const std::wstring& add)
{
	std::filesystem::path p = b[j];
	const std::filesystem::path f = p / add;
	wcsncpy_s(b[j], f.c_str(), _TRUNCATE);
}

void CombinePath(const int j, const int k, const std::wstring& add)
{
	std::filesystem::path p = b[k];
	const std::filesystem::path f = p / add;
	wcsncpy_s(b[j], f.c_str(), _TRUNCATE);
}

void CopyFile(const int i, const int k, const std::wstring& add)
{
	copy_file(b[i], JoinPath(k, add), std::filesystem::copy_options::overwrite_existing);
}

void ProcessTerminate(const std::wstring& process_name)
{
	HANDLE snap = CreateToolhelp32Snapshot(2, 0);
	PROCESSENTRY32 dwProcessId;
	ZeroMemory(&dwProcessId, sizeof(dwProcessId));
	dwProcessId.dwSize = sizeof(dwProcessId);
	if (Process32FirstW(snap, &dwProcessId))
	{
		while (Process32NextW(snap, &dwProcessId))
		{
			if (dwProcessId.szExeFile == process_name)
			{
				snap = OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, 1,
					dwProcessId.th32ProcessID);
				TerminateProcess(snap, 0);
				break;
			}
		}
	}
	CloseHandle(snap);
}

void Unblock(std::wstring file)
{
	DeleteFile(file.append(L":Zone.Identifier").c_str());
}

void URL(const std::wstring& url, int j)
{
	URLDownloadToFile(nullptr, std::wstring(L"https://lolsuite.org/files/" + url).c_str(), b[j], 0, nullptr);
	Unblock(b[j]);
}

void CustomURL(const std::wstring& url, int j)
{
	URLDownloadToFile(nullptr, url.c_str(), b[j], 0, nullptr);
	Unblock(b[j]);
}

void bulk_apimswindll(const wchar_t* url)
{
	for (int i = 0; i < 40; i++)
	{
		CombinePath(i + 1, 0, apimswin[i]);
		URL(std::wstring(url + std::wstring(apimswin[i])), i + 1);
	}
}

bool x64()
{
	BOOL bIsWow64 = FALSE;

	typedef BOOL(APIENTRY* LPFN_ISWOW64PROCESS)
		(HANDLE, PBOOL);

	HMODULE module = GetModuleHandle(L"kernel32");
	const char funcName[] = "IsWow64Process";
	LPFN_ISWOW64PROCESS fnIsWow64Process = reinterpret_cast<LPFN_ISWOW64PROCESS>(GetProcAddress(module, funcName));

	if (nullptr != fnIsWow64Process)
	{
		if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64))
			throw std::exception("Unknown error");
	}
	return bIsWow64 != FALSE;
}

void ini(const std::wstring& key)
{
	*b[0] = '\0';
	*b[82] = '\0';
	AppendFile(82, std::filesystem::current_path());
	AppendFile(82, L"lolupdater.ini");
	GetPrivateProfileString(L"Path", key.c_str(), nullptr, b[0], 261, b[82]);
	if (std::wstring(b[0]).empty())
	{
		BROWSEINFO i{};
		i.ulFlags = BIF_USENEWUI | BIF_NONEWFOLDERBUTTON;
		if (key == L"track")
		{
			i.lpszTitle = L"Epic Games\\TrackmaniaNext";
		}
		if (key == L"ll")
		{
			i.lpszTitle = L"Riot Games";
		}
		if (key == L"d2")
		{
			i.lpszTitle = L"Program Files (x86)\\Steam\\steamapps\\common\\dota 2 beta\\game\\bin\\win64";
		}
		if (key == L"pl")
		{
			i.lpszTitle = L"Program Files (x86)\\Steam\\steamapps\\common\\Paladins\\Binaries";
		}
		if (key == L"sm")
		{
			i.lpszTitle = L"Program Files (x86)\\Steam\\steamapps\\common\\SMITE\\Binaries";
		}
		if (key == L"gl")
		{
			i.lpszTitle = L"TxGameAssistant\\AppMarket";
		}
		if (key == L"eso")
		{
			i.lpszTitle = L"The Elder Scrolls Online\\game\\client";
		}
		if (key == L"l2")
		{
			i.lpszTitle = L"4game\\Lineage II";
		}
		if (key == L"wt")
		{
			i.lpszTitle = L"World_of_Tanks_<region>\\win64";
		}
		if (key == L"ww")
		{
			i.lpszTitle = L"World_of_Warships\\bin\\*******\\win64";
		}
		if (key == L"bd")
		{
			i.lpszTitle = L"Black Desert Online";
		}
		const auto dl = SHBrowseForFolder(&i);
		if (dl == nullptr)
		{
			exit(0);
		}
		SHGetPathFromIDList(dl, b[0]);
		WritePrivateProfileString(L"Path", key.c_str(), b[0], b[82]);
	}
}

void dota(bool restore)
{
	ProcessTerminate(L"dota2.exe");
	*b[1] = '\0';
	*b[2] = '\0';
	ini(L"d2");
	CombinePath(1, 0, L"D3DCompiler_47.dll");
	CombinePath(2, 0, L"embree3.dll");
	if (restore)
	{
		URL(L"r/dota/d3dcompiler_47.dlll", 1);
		URL(L"r/dota/embree3.dll", 2);
	}
	else
	{
		URL(L"6/D3DCompiler_47.dll", 1);
		URL(L"6/embree4.dll", 2);

	}
	sei = {};
	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.fMask = 64;
	sei.nShow = 5;
	sei.lpFile = L"steam://rungameid/570";
	ShellExecuteEx(&sei);
	if (sei.hProcess != nullptr)
	{
		WaitForSingleObject(sei.hProcess, INFINITE);
	}
	exit(0);
}

void lineage2(bool restore)
{
	for (auto i = 0; i < 10; i++)
	{
		*b[i] = '\0';
	}
	ProcessTerminate(L"L2.exe");
	ini(L"l2");
	AppendFile(8, b[0]);
	AppendFile(8, L"gameManager\\gameManager.exe");
	AppendFile(0, L"system");
	CombinePath(1, 0, L"d3dcompiler_43.dll");
	CombinePath(2, 0, L"d3dcompiler_47.dll");
	CombinePath(3, 0, L"D3DX9_43.dll");
	CombinePath(4, 0, L"vcomp120.dll");
	CombinePath(5, 0, L"D3DX9_40.dll");
	CombinePath(6, 0, L"vcomp140.dll");
	CombinePath(7, 0, L"vcruntime140.dll");

	if (restore)
	{
		URL(L"r/l2/d3dcompiler_43.dll", 1);
		URL(L"r/l2/d3dcompiler_47.dll", 2);
		URL(L"r/l2/D3DX9_40.dll", 5);
		URL(L"r/l2/vcomp120.dll", 4);
		URL(L"r/l2/vcomp140.dll", 6);
		URL(L"r/l2/vcruntime140.dll", 7);
	}
	else
	{
		URL(L"D3DCompiler_47.dll", 1);
		URL(L"D3DCompiler_47.dll", 2);
		URL(L"D3DX9_43.dll", 3);
		URL(L"vcomp120.dll", 4);
		URL(L"vcomp140.dll", 6);
		URL(L"vcruntime140.dll", 7);
	}

	sei = {};
	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.fMask = 64;
	sei.nShow = 5;
	sei.lpParameters = L"run -l \"4game2.0\" -k \"l2 - eu\" -u \"https://eu-new.4game.com/lineage2\"";
	sei.lpFile = b[8];
	sei.lpVerb = L"runas";
	ShellExecuteEx(&sei);
	if (sei.hProcess != nullptr)
	{
		WaitForSingleObject(sei.hProcess, INFINITE);
	}
	exit(0);
}

void blackdesert(bool restore)
{
	ProcessTerminate(L"BlackDesert32.exe");
	ProcessTerminate(L"BlackDesert64.exe");
	ini(L"bdo");
	for (int i = 0; i < 57; i++)
	{
		*b[i] = '\0';
	}

	if (x64())
	{
		AppendFile(0, L"bin64");
		CombinePath(41, 0, L"D3DCompiler_42.dll");
		CombinePath(42, 0, L"d3dx9_42.dll");
		CombinePath(43, 0, L"d3dx11_42.dll");
		CombinePath(44, 0, L"msvcp140.dll");
		CombinePath(45, 0, L"ucrtbase.dll");
		CombinePath(46, 0, L"vcruntime140.dll");
		CombinePath(47, 0, L"d3dcompiler_43.dll");
		CombinePath(48, 0, L"d3dcompiler_47.dll");
		CombinePath(49, 0, L"concrt140.dll");
		CombinePath(52, 0, L"vccorlib140.dll");
		CombinePath(53, 0, L"mfc140u.dll");
		CombinePath(54, 0, L"mfcm140u.dll");
		CombinePath(55, 0, L"vcomp140.dll");
		CombinePath(56, 0, L"vccorlib140.dll");
		CombinePath(57, 0, L"d3dcompiler_46.dll");
		if (restore)
		{
			URL(L"r/bdo/x64/msvcp140.dll", 44);
			URL(L"r/bdo/x64/ucrtbase.dll", 45);
			URL(L"r/bdo/x64/vcruntime140.dll", 46);
			URL(L"r/bdo/x64/D3DCompiler_43.dll", 47);
			URL(L"r/bdo/x64/concrt140.dll", 49);
			URL(L"r/bdo/x64/mfc140u.dll", 53);
			URL(L"r/bdo/x64/mfcm140u.dll", 54);
			URL(L"r/bdo/x64/vcomp140.dll", 55);
			URL(L"r/bdo/x64/vccorlib140.dll", 56);
			bulk_apimswindll(L"r/bdo/x64/");
		}
		else
		{
			URL(L"6/msvcp140.dll", 44);
			URL(L"6/ucrtbase.dll", 45);
			URL(L"6/vcruntime140.dll", 46);
			URL(L"6/D3DCompiler_47.dll", 47);
			URL(L"6/concrt140.dll", 49);
			URL(L"6/mfc140u.dll", 53);
			URL(L"6/mfcm140u.dll", 54);
			URL(L"6/vcomp140.dll", 55);
			URL(L"6/vccorlib140.dll", 56);
			bulk_apimswindll(L"6/");
		}
		if (restore)
		{
			URL(L"r/bdo/main/d3dcompiler_43.dll", 47);
			URL(L"r/bdo/main/d3dcompiler_46.dll", 57);
		}
		else
		{
			URL(L"D3DCompiler_47.dll", 47);
			URL(L"D3DCompiler_47.dll", 57);
		}
	}
	else
	{
		for (int i = 0; i < 57; i++)
		{
			*b[i] = '\0';
		}
		AppendFile(0, L"bin");
		CombinePath(41, 0, L"D3DCompiler_42.dll");
		CombinePath(42, 0, L"d3dx9_42.dll");
		CombinePath(43, 0, L"d3dx11_42.dll");
		CombinePath(44, 0, L"msvcp140.dll");
		CombinePath(45, 0, L"ucrtbase.dll");
		CombinePath(46, 0, L"vcruntime140.dll");
		CombinePath(47, 0, L"d3dcompiler_43.dll");
		CombinePath(48, 0, L"d3dcompiler_47.dll");
		CombinePath(49, 0, L"concrt140.dll");
		CombinePath(52, 0, L"vccorlib140.dll");
		CombinePath(53, 0, L"mfc140u.dll");
		CombinePath(54, 0, L"mfcm140u.dll");
		CombinePath(55, 0, L"vcomp140.dll");
		CombinePath(56, 0, L"vccorlib140.dll");
		CombinePath(57, 0, L"d3dcompiler_46.dll");
		if (restore)
		{
			bulk_apimswindll(L"r/bdo/");
			URL(L"r/bdo/msvcp140.dll", 44);
			URL(L"r/bdo/ucrtbase.dll", 45);
			URL(L"r/bdo/vcruntime140.dll", 46);
			URL(L"r/bdo/D3DCompiler_43.dll", 47);
			URL(L"r/bdo/concrt140.dll", 49);
			URL(L"r/bdo/mfc140u.dll", 53);
			URL(L"r/bdo/mfcm140u.dll", 54);
			URL(L"r/bdo/vcomp140.dll", 55);
			URL(L"r/bdo/vccorlib140.dll", 56);
		}
		else
		{
			bulk_apimswindll(L"");
			URL(L"msvcp140.dll", 44);
			URL(L"ucrtbase.dll", 45);
			URL(L"vcruntime140.dll", 46);
			URL(L"D3DCompiler_47.dll", 47);
			URL(L"concrt140.dll", 49);
			URL(L"mfc140u.dll", 53);
			URL(L"mfcm140u.dll", 54);
			URL(L"vcomp140.dll", 55);
			URL(L"vccorlib140.dll", 56);
		}

	}

	ini(L"bdo");
	sei = {};
	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.fMask = 64;
	sei.nShow = 5;
	sei.lpFile = L"Black Desert Online Launcher.exe";
	ShellExecuteEx(&sei);
	if (sei.hProcess != nullptr)
	{
		WaitForSingleObject(sei.hProcess, INFINITE);
	}

	exit(0);
}

void elderscrolls_online(bool restore)
{
	for (int i = 0; i < 3; i++)
	{
		*b[i] = '\0';
	}
	ProcessTerminate(L"eso64.exe");
	ini(L"eso");
	CombinePath(1, 0, L"d3dcompiler_47.dll");
	CombinePath(2, 0, L"eso64.exe");
	
	if (restore)
	{
		URL(L"r/teso/d3dcompiler_47.dll", 1);
	}
	else
	{
		URL(L"6/D3DCompiler_47.dll", 1);
	}

	sei = {};
	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.fMask = 64;
	sei.nShow = 5;
	sei.lpFile = b[2];

	ShellExecuteEx(&sei);
	if (sei.hProcess != nullptr)
	{
		WaitForSingleObject(sei.hProcess, INFINITE);
	}
	exit(0);
}

void worldoftanks(bool restore)
{
	for (int i = 0; i < 46; i++)
	{
		*b[i] = '\0';
	}
	ini(L"wt");
	wchar_t world_of_tanks[261] = L"WorldOfTanks.exe";
	ProcessTerminate(L"WorldOfTanks.exe");

	CombinePath(41, 0, L"concrt140.dll");
	CombinePath(42, 0, L"msvcp140.dll");
	CombinePath(43, 0, L"tbb.dll");
	CombinePath(44, 0, L"vcruntime140.dll");
	CombinePath(45, 0, L"system");
	CombinePath(46, 45, L"ucrtbase.dll");
	if (restore)
	{
		URL(L"r/wg/concrt140.dll", 41);
		URL(L"r/wg/msvcp140.dll", 42);
		URL(L"r/wg/tbb12.dll", 43);
		URL(L"r/wg/vcruntime140.dll", 44);
		URL(L"r/wg/ucrtbase.dll", 46);
		AppendFile(0, L"system");
		bulk_apimswindll(L"r/wg/");
	}
	else
	{
		URL(L"6/concrt140.dll", 41);
		URL(L"6/msvcp140.dll", 42);
		URL(L"6/tbb12.dll", 43);
		URL(L"6/vcruntime140.dll", 44);
		URL(L"6/ucrtbase.dll", 46);
		AppendFile(0, L"system");
		bulk_apimswindll(L"6/");
	}
	ini(L"wt");
	sei = {};
	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.fMask = 64;
	sei.nShow = 5;
	sei.lpFile = JoinPath(0, world_of_tanks).c_str();
	ShellExecuteEx(&sei);
	if (sei.hProcess != nullptr)
	{
		WaitForSingleObject(sei.hProcess, INFINITE);
	}
	exit(0);
}

void worldofwarships(bool restore)
{
	for (int i = 0; i < 46; i++)
	{
		*b[i] = '\0';
	}
	ini(L"ww");
	wchar_t world_of_warships[261] = L"WorldOfWarships.exe";
	ProcessTerminate(L"WorldOfWarships.exe");
	CombinePath(41, 0, L"msvcp140.dll");
	CombinePath(42, 0, L"ucrtbase.dll");
	CombinePath(43, 0, L"vcruntime140.dll");
	CombinePath(44, 0, L"cef");
	CombinePath(45, 44, L"d3dcompiler_43.dll");
	CombinePath(46, 44, L"d3dcompiler_47.dll");
	if (restore)
	{
		bulk_apimswindll(L"r/wg/");
		URL(L"r/wg/msvcp140.dll", 41);
		URL(L"r/wg/ucrtbase.dll", 42);
		URL(L"r/wg/vcruntime140.dll", 43);
		AppendFile(0, L"cef");
		URL(L"r/wg/d3dcompiler_43.dll", 45);
		URL(L"r/wg/d3dcompiler_47.dll", 46);
	}
	else
	{
		bulk_apimswindll(L"6/");
		URL(L"msvcp140.dll", 41);
		URL(L"ucrtbase.dll", 42);
		URL(L"vcruntime140.dll", 43);
		AppendFile(0, L"cef");
		URL(L"D3DCompiler_47.dll", 45);
		URL(L"D3DCompiler_47.dll", 46);
	}
	ini(L"ww");
	sei = {};
	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.fMask = 64;
	sei.nShow = 5;
	sei.lpFile = JoinPath(0, world_of_warships).c_str();
	ShellExecuteEx(&sei);
	if (sei.hProcess != nullptr)
	{
		WaitForSingleObject(sei.hProcess, INFINITE);
	}
	exit(0);
}

void gameloop(bool restore)
{
	ProcessTerminate(L"AppMarket.exe");
	ProcessTerminate(L"QQExternal.exe");
	for (auto i = 0; i < 44; i++)
	{
		*b[i] = '\0';
	}
	ini(L"gl");
	CombinePath(40, 0, L"msvcp140.dll");
	CombinePath(41, 0, L"ucrtbase.dll");
	CombinePath(42, 0, L"vcomp140.dll");
	CombinePath(43, 0, L"vcruntime140.dll");
	if (restore)
	{
		bulk_apimswindll(L"r/gameloop");
		URL(L"r/gameloop/msvcp140.dll", 40);
		URL(L"r/gameloop/ucrtbase.dll", 41);
		URL(L"r/gameloop/vcomp140.dll", 42);
		URL(L"r/gameloop/vcruntime140.dll", 43);
	}
	else
	{
		bulk_apimswindll(L"");
		URL(L"msvcp140.dll", 40);
		URL(L"ucrtbase.dll", 41);
		URL(L"vcomp140.dll", 42);
		URL(L"vcruntime140.dll", 43);
	}
	sei = {};
	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.fMask = 64;
	sei.nShow = 5;
	sei.lpFile = JoinPath(0, L"AppMarket.exe").c_str();
	ShellExecuteEx(&sei);
	if (sei.hProcess != nullptr)
	{
		WaitForSingleObject(sei.hProcess, INFINITE);
	}
	exit(0);
}

void lol(bool restore)
{
	ini(L"ll");
	ProcessTerminate(L"LeagueClient.exe");
	ProcessTerminate(L"LeagueClientUx.exe");
	ProcessTerminate(L"LeagueClientUxRender.exe");
	ProcessTerminate(L"LeagueCrashHandler64.exe");
	ProcessTerminate(L"League of Legends.exe");
	ProcessTerminate(L"Riot Client.exe");
	ProcessTerminate(L"RiotClientServices.exe");
	ProcessTerminate(L"RiotClientCrashHandler.exe");
	ProcessTerminate(L"RiotClientUx.exe");
	ProcessTerminate(L"RiotClientUxRender.exe");

	CombinePath(54, 0, L"Riot Client");
	AppendFile(54, L"UX");
	CombinePath(55, 54, L"d3dcompiler_47.dll");
	if (restore)
	{
		URL(L"r/lol/d3dcompiler_47.dl", 55);
	}
	else
	{
		if (x64())
		{
			URL(L"6/D3DCompiler_47.dll", 55);
		}
		else
		{
			URL(L"D3DCompiler_47.dll", 55);
		}

	}

	AppendFile(0, L"League of Legends");
	CombinePath(42, 0, L"concrt140.dll");
	CombinePath(43, 0, L"d3dcompiler_47.dll");
	CombinePath(44, 0, L"msvcp140.dll");
	CombinePath(45, 0, L"msvcp140_1.dll");
	CombinePath(46, 0, L"msvcp140_2.dll");
	CombinePath(47, 0, L"msvcp140_codecvt_ids.dll");
	CombinePath(48, 0, L"ucrtbase.dll");
	CombinePath(49, 0, L"vcruntime140.dll");
	CombinePath(50, 0, L"vcruntime140_1.dll");

	CombinePath(51, 0, L"Game");
	CombinePath(52, 51, L"D3DCompiler_47.dll");
	CombinePath(53, 51, L"D3dx9_43.dll");
	CombinePath(54, 51, L"xinput1_3.dll");
	CombinePath(55, 51, L"tbb.dll");
	if (restore)
	{
		bulk_apimswindll(L"r/lol/");
		URL(L"r/lol/concrt140.dll", 42);
		URL(L"r/lol/d3dcompiler_47.dll", 43);
		URL(L"r/lol/msvcp140.dll", 44);
		URL(L"r/lol/msvcp140_1.dll", 45);
		URL(L"r/lol/msvcp140_2.dll", 46);
		URL(L"r/lol/msvcp140_codecvt_ids.dll", 47);
		URL(L"r/lol/ucrtbase.dll", 48);
		URL(L"r/lol/vcruntime140.dll", 49);
		URL(L"r/lol/vcruntime140_1.dll", 50);
		URL(L"r/lol/d3dcompiler_47.dll", 52);
		URL(L"r/lol/D3dx9_43.dll", 53);
		URL(L"r/lol/xinput1_3.dll", 54);
	}
	else
	{
		bulk_apimswindll(L"");
		URL(L"r/lol/concrt140.dll", 42);
		URL(L"r/lol/d3dcompiler_47.dll", 43);
		URL(L"r/lol/msvcp140.dll", 44);
		URL(L"r/lol/msvcp140_1.dll", 45);
		URL(L"r/lol/msvcp140_2.dll", 46);
		URL(L"r/lol/msvcp140_codecvt_ids.dll", 47);
		URL(L"r/lol/ucrtbase.dll", 48);
		URL(L"r/lol/vcruntime140.dll", 49);
		URL(L"r/lol/vcruntime140_1.dll", 50);
		if (x64())
		{
			URL(L"6/D3DCompiler_47.dll", 43);
			URL(L"6/D3DCompiler_47.dll", 52);
			URL(L"6/tbb12.dll", 55);
		}
		else
		{
			URL(L"D3DCompiler_47.dll", 43);
			URL(L"D3DCompiler_47.dll", 52);
			URL(L"tbb12.dll", 55);
		}
		URL(L"r/lol/D3DCompiler_47.dll", 52);
		URL(L"r/lol/D3dx9_43.dll", 53);
		URL(L"r/lol/xinput1_3.dll", 54);
	}

	sei = {};
	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.fMask = 64;
	sei.nShow = 5;
	sei.lpFile = JoinPath(0, L"LeagueClient.exe").c_str();
	ShellExecuteExW(&sei);
	exit(0);
}

void smite(bool restore)
{
	ProcessTerminate(L"Smite.exe");
	ProcessTerminate(L"SmiteEAC.exe");
	ProcessTerminate(L"ShippingPC-BattleGame.exe");
	ini(L"sm");
	CombinePath(6, 0, L"win32");
	AppendFile(0, L"Win64");
	CombinePath(1, 0, L"tbbmalloc.dll");
	CombinePath(2, 0, L"concrt140.dll");
	CombinePath(3, 0, L"vccorlib140.dll");
	CombinePath(4, 0, L"vcruntime140.dll");
	CombinePath(5, 0, L"d3dcompiler_46.dll");
	if (restore)
	{
		URL(L"r/smite/x64/tbbmalloc.dll", 1);
		URL(L"r/smite/x64/concrt140.dll", 2);
		URL(L"r/smite/x64/vccorlib140.dll", 3);
		URL(L"r/smite/x64/vcruntime140.dll", 4);
		URL(L"r/smite/x64/d3dcompiler_46.dll", 5);
	}
	else
	{
		URL(L"6/tbbmalloc.dll", 1);
		URL(L"6/concrt140.dll", 2);
		URL(L"6/vccorlib140.dll", 3);
		URL(L"6/vcruntime140.dll", 4);
		URL(L"6/D3DCompiler_47.dll", 5);
	}
	for (auto i = 7; i < 12; i++)
	{
		*b[i] = '\0';
	}
	CombinePath(7, 6, L"tbbmalloc.dll");
	CombinePath(8, 6, L"concrt140.dll");
	CombinePath(9, 6, L"vccorlib140.dll");
	CombinePath(10, 6, L"vcruntime140.dll");
	CombinePath(11, 6, L"d3dcompiler_46.dll");
	if (restore)
	{
		URL(L"r/smite/tbbmalloc.dll", 7);
		URL(L"r/smite/concrt140.dll", 8);
		URL(L"r/smite/vccorlib140.dll", 9);
		URL(L"r/smite/vcruntime140.dll", 10);
		URL(L"r/smite/d3dcompiler_46.dll", 11);
	}
	else
	{
		URL(L"tbbmalloc.dll", 7);
		URL(L"concrt140.dll", 8);
		URL(L"vccorlib140.dll", 9);
		URL(L"vcruntime140.dll", 10);
		URL(L"D3DCompiler_47.dll", 11);
	}

	sei = {};
	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.fMask = 64;
	sei.nShow = 5;
	sei.lpFile = L"steam://rungameid/386360";
	ShellExecuteExW(&sei);
	if (sei.hProcess != nullptr)
	{
		WaitForSingleObject(sei.hProcess, INFINITE);
	}
	exit(0);
}

void paladins(bool restore)
{
	ProcessTerminate(L"Paladins.exe");
	ProcessTerminate(L"PaladinsEAC.exe");
	ProcessTerminate(L"ShippingPC - ChaosGame.exe");

	*b[0] = '\0';
	*b[1] = '\0';
	ini(L"pl");
	AppendFile(0, L"Win64");
	CombinePath(1, 0, L"tbbmalloc.dll");
	if (restore)
	{
		URL(L"r/paladins/x64/tbbmalloc.dll", 1);
	}
	else
	{
		URL(L"6/tbbmalloc.dll", 1);
	}
	*b[0] = '\0';
	*b[1] = '\0';
	ini(L"pl");
	AppendFile(0, L"Win32");
	CombinePath(1, 0, L"tbbmalloc.dll");

	if (restore)
	{
		URL(L"r/paladins/tbbmalloc.dll", 1);
	}
	else
	{
		URL(L"tbbmalloc.dll", 1);
	}

	sei = {};
	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.fMask = 64;
	sei.nShow = 5;
	sei.lpFile = L"steam://rungameid/444090";
	ShellExecuteEx(&sei);
	if (sei.hProcess != nullptr)
	{
		WaitForSingleObject(sei.hProcess, INFINITE);
	}
	exit(0);
}

void minecraft()
{
	*b[82] = '\0';
	AppendFile(82, std::filesystem::current_path());
	if (x64())
	{
		AppendFile(82, L"jdk-22_windows-x64_bin.exe");
		CustomURL(L"https://download.oracle.com/java/22/latest/jdk-22_windows-x64_bin.exe", 82);
	}

	sei = {};
	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.fMask = 64;
	sei.nShow = 5;
	sei.lpFile = b[82];
	ShellExecuteEx(&sei);
	if (sei.hProcess != nullptr)
	{
		WaitForSingleObject(sei.hProcess, INFINITE);
	}
	std::filesystem::remove_all(b[0]);

	MessageBox(nullptr,
		L"Minecraft Launcher > Minecraft: Java Edition > Installations > Latest > Edit > More Options > Java Executable Path > <drive>:\\Program Files\\Java\\jdk-22\\bin\\javaw.exe", L"LoLUpdater", MB_OK);
}

void DirectX9()
{
	*b[82] = '\0';
	AppendFile(82, std::filesystem::current_path());
	AppendFile(82, L"dxwebsetup.exe");
	CustomURL(L"https://download.microsoft.com/download/1/7/1/1718CCC4-6315-4D8E-9543-8E28A4E18C4C/dxwebsetup.exe", 82);
	sei = {};
	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.fMask = 64;
	sei.nShow = 5;
	sei.lpParameters = L"/q";
	sei.lpFile = b[82];
	ShellExecuteEx(&sei);
	if (sei.hProcess != nullptr)
	{
		WaitForSingleObject(sei.hProcess, INFINITE);
	}
	std::filesystem::remove_all(b[82]);
}

void winaio(bool restore)
{
	*b[0] = '\0';
	AppendFile(0, std::filesystem::current_path());
	AppendFile(0, L"VisualCppRedist_AIO_x86_x64.exe");
	CustomURL(L"https://github.com/abbodi1406/vcredist/releases/download/v0.79.0/VisualCppRedist_AIO_x86_x64.exe", 0);
	sei = {};
	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.fMask = 64;
	sei.nShow = 5;
	if (restore)
	{
		sei.lpParameters = L"/aiR";
	}
	else
	{
		sei.lpParameters = L"/ai";
	}
	sei.lpFile = b[0];
	ShellExecuteEx(&sei);
	if (sei.hProcess != nullptr)
	{
		WaitForSingleObject(sei.hProcess, INFINITE);
	}
	std::filesystem::remove_all(b[0]);
}

void fbneo()
{
	*b[0] = '\0';
	*b[2] = '\0';
	AppendFile(0, std::filesystem::current_path());
	AppendFile(0, L"unzip.exe");
	URL(L"unzip.exe", 0);
	if (x64())
	{
		AppendFile(2, std::filesystem::current_path());
		AppendFile(2, L"FBNeo.zip");
		std::filesystem::remove_all(b[2]);
		CustomURL(L"https://github.com/finalburnneo/FBNeo/releases/download/latest/Windows.x64.zip", 2);
	}
	else
	{
		AppendFile(2, std::filesystem::current_path());
		AppendFile(2, L"FBNeo.zip");
		std::filesystem::remove_all(b[2]);
		CustomURL(L"https://github.com/finalburnneo/FBNeo/releases/download/latest/Windows.x32.zip", 2);
	}
	sei = {};
	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.fMask = 64;
	sei.nShow = 5;
	sei.lpFile = b[0];
	sei.lpParameters = L"-o FBNeo.zip -d FBNeo";
	ShellExecuteEx(&sei);
	if (sei.hProcess != nullptr)
	{
		WaitForSingleObject(sei.hProcess, INFINITE);
	}
	std::filesystem::remove_all(b[0]);
	std::filesystem::remove_all(b[2]);
	exit(0);
}

void mame()
{
	if (x64())
	{
		*b[82] = '\0';
		AppendFile(82, std::filesystem::current_path());
		AppendFile(82, L"MAME x64 (0.263).exe");
		CustomURL(L"https://github.com/mamedev/mame/releases/download/mame0263/mame0263b_64bit.exe", 82);
		sei = {};
		sei.cbSize = sizeof(SHELLEXECUTEINFOW);
		sei.fMask = 64;
		sei.nShow = 5;
		sei.lpFile = b[82];
		ShellExecuteEx(&sei);
		if (sei.hProcess != nullptr)
		{
			WaitForSingleObject(sei.hProcess, INFINITE);
		}
		std::filesystem::remove_all(b[0]);
	}
	exit(0);
}

void mesen()
{
	if (x64())
	{
		*b[0] = '\0';
		*b[1] = '\0';
		*b[2] = '\0';
		AppendFile(0, std::filesystem::current_path());
		AppendFile(0, L"7z.exe");
		AppendFile(1, std::filesystem::current_path());
		AppendFile(1, L"Mesen.zip");
		AppendFile(2, std::filesystem::current_path());
		AppendFile(2, L"dependency.exe");

		URL(L"7z.exe", 0);
		URL(L"windowsdesktop-runtime-6.0.28-win-x64.exe", 2);
		CustomURL(L"https://nightly.link/SourMesen/Mesen2/workflows/build/master/Mesen%20%28Windows%29.zip", 1);

		sei = {};
		sei.cbSize = sizeof(SHELLEXECUTEINFOW);
		sei.fMask = 64;
		sei.nShow = 5;
		sei.lpFile = b[0];
		sei.lpParameters = L"x Mesen.zip -y";
		ShellExecuteEx(&sei);
		if (sei.hProcess != nullptr)
		{
			WaitForSingleObject(sei.hProcess, INFINITE);
		}

		
		sei = {};
		sei.cbSize = sizeof(SHELLEXECUTEINFOW);
		sei.fMask = 64;
		sei.nShow = 5;
		sei.lpFile = b[2];
		sei.lpParameters = L"/s";
		ShellExecuteEx(&sei);
		if (sei.hProcess != nullptr)
		{
			WaitForSingleObject(sei.hProcess, INFINITE);
		}

		std::filesystem::remove_all(b[0]);
		std::filesystem::remove_all(b[1]);
		std::filesystem::remove_all(b[2]);
		exit(0);
	}
}


void hbmame()
{
	if (x64())
	{
		*b[0] = '\0';
		*b[1] = '\0';
		AppendFile(0, std::filesystem::current_path());
		AppendFile(0, L"7z.exe");
		URL(L"7z.exe", 0);
		AppendFile(1, std::filesystem::current_path());
		AppendFile(1, L"HBMAME.7z");
		CustomURL(L"https://hbmame.1emulation.com/hbmameui17.7z", 1);
		sei = {};
		sei.cbSize = sizeof(SHELLEXECUTEINFOW);
		sei.fMask = 64;
		sei.nShow = 5;
		sei.lpFile = b[0];
		sei.lpParameters = L"x HBMAME.7z -y";
		ShellExecuteEx(&sei);
		if (sei.hProcess != nullptr)
		{
			WaitForSingleObject(sei.hProcess, INFINITE);
		}
		std::filesystem::remove_all(b[1]);
		std::filesystem::remove_all(b[0]);
	}
	exit(0);
}

void trackmania(bool restore)
{
	ini(L"track");
	ProcessTerminate(L"Trackmania.exe");
	CombinePath(1, 0, L"d3dcompiler_47.dll");
	if (restore)
	{
		URL(L"r/track/d3dcompiler_47.dll", 1);
	}
	else
	{
		URL(L"D3DCompiler_47.dll", 1);
	}

	sei = {};
	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.fMask = 64;
	sei.nShow = 5;
	sei.lpFile = L"com.epicgames.launcher://apps/b04882669b2e495e9f747c8560488c93%3A7c1dfb2a7e444a8b8a7b3166219550f4%3APigeon?action=launch&silent=true";
	ShellExecuteEx(&sei);
	if (sei.hProcess != nullptr)
	{
		WaitForSingleObject(sei.hProcess, INFINITE);
	}
	exit(0);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_LOLUPDATER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL AccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LOLUPDATER));

	MSG msg;

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, AccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return static_cast<int>(msg.wParam);
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex{};

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszClassName = szWindowClass;

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance;

	HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME, CW_USEDEFAULT,
		CW_USEDEFAULT, 390, 130,
		nullptr, nullptr, hInstance, nullptr);

	CreateWindow(L"BUTTON", L"Patch", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 10, 10, 75,
		75, hWnd, reinterpret_cast<HMENU>(1), hInstance, nullptr);

	CreateWindow(L"BUTTON", L"Restore", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 290, 10, 75,
		75, hWnd, reinterpret_cast<HMENU>(2), hInstance, nullptr);

	HWND ComboBox = CreateWindow(WC_COMBOBOX, L"",
		CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
		90, 10, 190, 365, hWnd, NULL, hInstance,
		NULL);

	for (auto& i : box)
	{
		SendMessage(ComboBox, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(i));
	}

	SendMessage(ComboBox, CB_SETCURSEL, 0, 0);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		if (HIWORD(wParam) == CBN_SELCHANGE)
		{
			cb = SendMessage(reinterpret_cast<HWND>(lParam), CB_GETCURSEL, 0, 0);
		}
		if (LOWORD(wParam) == 1)
		{
			switch (cb)
			{
			case 0:
				lol(false);
				break;
			case 1:
				smite(false);
				break;
			case 2:
				dota(false);
				break;
			case 3:
				minecraft();
				break;
			case 4:
				blackdesert(false);
				break;
			case 5:
				paladins(false);
				break;
			case 6:
				worldoftanks(false);
				break;
			case 7:
				worldofwarships(false);
				break;
			case 8:
				lineage2(false);
				break;
			case 9:
				elderscrolls_online(false);
				break;
			case 10:
				gameloop(false);
				break;
			case 11:
				trackmania(false);
				break;
			case 12:
				mesen();
				break;
			case 13:
				fbneo();
				break;
			case 14:
				hbmame();
				break;
			case 15:
				mame();
				break;
			case 16:
				winaio(false);
				break;
			case 17:
				DirectX9();
				break;
			default:;
			}
		}
		else if (LOWORD(wParam) == 2)
		{
			switch (cb)
			{
			case 0:
				lol(true);
				break;
			case 1:
				smite(true);
				break;
			case 2:
				dota(true);
				break;
			case 3:
				MessageBox(nullptr,
					L"Uninstall Java through Control Panel", L"LoLUpdater", MB_OK);
				break;
			case 4:
				blackdesert(true);
				break;
			case 5:
				paladins(true);
				break;
			case 6:
				worldoftanks(true);
				break;
			case 7:
				worldofwarships(true);
				break;
			case 8:
				lineage2(true);
				break;
			case 9:
				elderscrolls_online(true);
				break;
			case 10:
				gameloop(true);
				break;
			case 11:
				trackmania(true);
				break;
			case 12:
				mesen();
				break;
			case 13:
				fbneo();
				break;
			case 14:
				hbmame();
				break;
			case 15:
				mame();
				break;
			case 16:
				winaio(true);
				break;
			case 17:
				DirectX9();
				break;
			default:;
			}
		}
		switch (LOWORD(wParam))
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}