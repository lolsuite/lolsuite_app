#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <filesystem>
#include <ShlObj_core.h>
#include <tlhelp32.h>
#include <shellapi.h>
#include <urlmon.h>
#include "resource.h"
#include <SDKDDKVer.h>
#pragma comment (lib, "urlmon.lib")
constexpr auto MAX_LOADSTRING = 100;
wchar_t b[83][MAX_PATH + 1];
SHELLEXECUTEINFOW sei;
int cb;
HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING] = L"LoLSuite";
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

const wchar_t* box[12] = {
	L"League of Legends", L"DOTA2", L"Minecraft : Java", L"NES Emulator", L"Xenia (Xbox360 Emu)", L"FinalBurn Neo", L"HBMAME", L"MAME", L"Visual Redistributable", L"DirectX", L"7-Zip", L"Activate Windows"
};

std::wstring JoinPath(const int j, const std::wstring& add)
{
	const std::filesystem::path p = b[j];
	return (p / add).c_str();
}

bool CheckOneInstance()
{

	HANDLE  m_hStartEvent = CreateEventW(NULL, FALSE, FALSE, L"Global\\CSAPP");

	if (m_hStartEvent == NULL)
	{
		CloseHandle(m_hStartEvent);
		return false;
	}


	if (GetLastError() == ERROR_ALREADY_EXISTS) {

		CloseHandle(m_hStartEvent);
		m_hStartEvent = NULL;
		MessageBox(nullptr, L"You can only run one instance of LoLSuite", L"LoLSuite", MB_OK);
		exit(0);
	}
	// the only instance, start in a usual way
	return true;
}

void AppendPath(const int j, const std::wstring& add)
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
	URLDownloadToFile(nullptr, std::wstring(L"https://lolsuite.org/f/" + url).c_str(), b[j], 0, nullptr);
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
	AppendPath(82, std::filesystem::current_path());
	AppendPath(82, L"LoLSuite.ini");
	GetPrivateProfileString(L"Path", key.c_str(), nullptr, b[0], 261, b[82]);
	if (std::wstring(b[0]).empty())
	{
		BROWSEINFO i{};
		i.ulFlags = BIF_USENEWUI | BIF_NONEWFOLDERBUTTON;
		if (key == L"ll")
		{
			i.lpszTitle = L"X:\Riot Games";
		}
		if (key == L"d2")
		{
			i.lpszTitle = L"X:\Program Files (x86)\\Steam\\steamapps\\common\\dota 2 beta\\game\\bin\\win64";
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
		URL(L"r/dota2/d3dcompiler_47.dlll", 1);
		URL(L"r/dota2/embree3.dll", 2);
	}
	else
	{
		URL(L"6/D3DCompiler_47.dll", 1);
		URL(L"6/embree.dll", 2);

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
	AppendPath(54, L"UX");
	AppendPath(0, L"League of Legends");
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
		URL(L"r/lol/concrt140.dll", 42);
		URL(L"r/lol/d3dcompiler_47.dll", 43);
		URL(L"r/lol/msvcp140.dll", 44);
		URL(L"r/lol/msvcp140_1.dll", 45);
		URL(L"r/lol/msvcp140_2.dll", 46);
		URL(L"r/lol/msvcp140_codecvt_ids.dll", 47);
		URL(L"r/lol/ucrtbase.dll", 48);
		URL(L"r/lol/vcruntime140.dll", 49);
		URL(L"r/lol/vcruntime140_1.dll", 50);
		URL(L"r/lol/D3DCompiler_47.dll", 52);
		URL(L"r/lol/D3dx9_43.dll", 53);
		URL(L"r/lol/xinput1_3.dll", 54);
		URL(L"r/lol/tbb12.dll", 55);
	}
	else
	{
		URL(L"concrt140.dll", 42);
		URL(L"d3dcompiler_47.dll", 43);
		URL(L"msvcp140.dll", 44);
		URL(L"msvcp140_1.dll", 45);
		URL(L"msvcp140_2.dll", 46);
		URL(L"msvcp140_codecvt_ids.dll", 47);
		URL(L"ucrtbase.dll", 48);
		URL(L"vcruntime140.dll", 49);
		URL(L"vcruntime140_1.dll", 50);
		URL(L"D3dx9_43.dll", 53);
		URL(L"xinput1_3.dll", 54);
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
	}

	sei = {};
	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.fMask = 64;
	sei.nShow = 5;
	sei.lpFile = JoinPath(0, L"LeagueClient.exe").c_str();
	ShellExecuteExW(&sei);
	exit(0);
}

void minecraft()
{
	*b[82] = '\0';
	AppendPath(82, std::filesystem::current_path());
	if (x64())
	{
		AppendPath(82, L"jdk-22_windows-x64_bin.exe");
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

	MessageBox(nullptr, L"Minecraft Launcher > Minecraft: Java Edition > Installations > Latest > Edit > More Options > Java Executable Path > <drive>:\\Program Files\\Java\\jdk-22\\bin\\javaw.exe", L"LoLSuite", MB_OK);
}

void DirectX9()
{
	*b[82] = '\0';
	AppendPath(82, std::filesystem::current_path());
	AppendPath(82, L"dxwebsetup.exe");
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

void zip()
{
	*b[0] = '\0';
	AppendPath(0, std::filesystem::current_path());
	AppendPath(0, L"7z.exe");

	if (x64())
	{
		CustomURL(L"https://7-zip.org/a/7z2407-x64.exe", 0);
	}
	else
	{
		CustomURL(L"https://7-zip.org/a/7z2407.exe", 0);
	}


	sei = {};
	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.fMask = 64;
	sei.nShow = 5;
	sei.lpParameters = L"/S";
	sei.lpFile = b[0];
	ShellExecuteEx(&sei);
	if (sei.hProcess != nullptr)
	{
		WaitForSingleObject(sei.hProcess, INFINITE);
	}
	std::filesystem::remove_all(b[0]);
}

void activate()
{
	system("start powershell.exe -command \"irm https://get.activated.win | iex\"");
	exit(0);
}

void winaio()
{
	*b[0] = '\0';
	AppendPath(0, std::filesystem::current_path());
	AppendPath(0, L"VisualCppRedist_AIO_x86_x64.exe");
	CustomURL(L"https://github.com/abbodi1406/vcredist/releases/download/v0.82.0/VisualCppRedist_AIO_x86_x64.exe", 0);
	sei = {};
	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.fMask = 64;
	sei.nShow = 5;
	sei.lpParameters = L"/aiR /gm2";
	sei.lpFile = b[0];
	ShellExecuteEx(&sei);
	if (sei.hProcess != nullptr)
	{
		WaitForSingleObject(sei.hProcess, INFINITE);
	}
	sei = {};
	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.fMask = 64;
	sei.nShow = 5;
	sei.lpParameters = L"/y /gm2";
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
	AppendPath(0, std::filesystem::current_path());
	AppendPath(0, L"7z.exe");
	URL(L"7z.exe", 0);
	if (x64())
	{
		AppendPath(2, std::filesystem::current_path());
		AppendPath(2, L"FBNeo.zip");
		std::filesystem::remove_all(b[2]);
		CustomURL(L"https://github.com/finalburnneo/FBNeo/releases/download/latest/Windows.x64.zip", 2);
	}
	else
	{
		AppendPath(2, std::filesystem::current_path());
		AppendPath(2, L"FBNeo.zip");
		std::filesystem::remove_all(b[2]);
		CustomURL(L"https://github.com/finalburnneo/FBNeo/releases/download/latest/Windows.x32.zip", 2);
	}
	sei = {};
	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.fMask = 64;
	sei.nShow = 5;
	sei.lpFile = b[0];
	sei.lpParameters = L"x FBNeo.zip -y";
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
		AppendPath(82, std::filesystem::current_path());
		AppendPath(82, L"MAME266.exe");
		CustomURL(L"https://github.com/mamedev/mame/releases/download/mame0266/mame0266b_64bit.exe", 82);
		*b[1] = '\0';
		AppendPath(1, std::filesystem::current_path());
		AppendPath(1, L"7z.exe");
		URL(L"7z.exe", 1);
		sei = {};
		sei.cbSize = sizeof(SHELLEXECUTEINFOW);
		sei.fMask = 64;
		sei.nShow = 5;
		sei.lpFile = b[1];
		sei.lpParameters = L"x MAME256.exe -y";
		ShellExecuteEx(&sei);
		if (sei.hProcess != nullptr)
		{
			WaitForSingleObject(sei.hProcess, INFINITE);
		}
		std::filesystem::remove_all(b[1]);
		std::filesystem::remove_all(b[82]);
	}
	exit(0);
}

void mesen()
{
		*b[0] = '\0';
		*b[1] = '\0';
		*b[2] = '\0';
		*b[3] = '\0';
		AppendPath(0, std::filesystem::current_path());
		AppendPath(0, L"7z.exe");
		AppendPath(1, std::filesystem::current_path());
		AppendPath(1, L"Mesen.zip");
		AppendPath(2, std::filesystem::current_path());
		AppendPath(2, L"dotnet.exe");

		if (x64)
		{
			CustomURL(L"https://download.visualstudio.microsoft.com/download/pr/76e5dbb2-6ae3-4629-9a84-527f8feb709c/09002599b32d5d01dc3aa5dcdffcc984/windowsdesktop-runtime-8.0.6-win-x64.exe", 2);
			CustomURL(L"https://nightly.link/SourMesen/Mesen2/workflows/build/master/Mesen%20%28Windows%20-%20net8.0%20-%20AoT%29.zip", 1);
			URL(L"7z.exe", 0);
		}
		else
		{
			MessageBox(nullptr, L"Only Available for x64 CPUs", L"LoLSuite", MB_OK);
		}

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

		AppendPath(3, L"Mesen.exe");
		sei = {};
		sei.cbSize = sizeof(SHELLEXECUTEINFOW);
		sei.fMask = 64;
		sei.nShow = 5;
		sei.lpFile = b[3];
		ShellExecuteEx(&sei);
		if (sei.hProcess != nullptr)
		{
			WaitForSingleObject(sei.hProcess, INFINITE);
		}


		exit(0);
}

void xenia()
{
	*b[0] = '\0';
	*b[1] = '\0';
	*b[2] = '\0';
	*b[3] = '\0';
	*b[4] = '\0';
	AppendPath(0, std::filesystem::current_path());
	AppendPath(0, L"7z.exe");
	AppendPath(1, std::filesystem::current_path());
	AppendPath(1, L"xenia_master.zip");

	if (x64)
	{
		URL(L"7z.exe", 0);
		CustomURL(L"https://github.com/xenia-project/release-builds-windows/releases/latest/download/xenia_master.zip", 1);
	}
	else
	{
		MessageBox(nullptr, L"Only Available for x64 CPUs", L"LoLSuite", MB_OK);
	}

	sei = {};
	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.fMask = 64;
	sei.nShow = 5;
	sei.lpFile = b[0];
	sei.lpParameters = L"x xenia_master.zip -y";
	ShellExecuteEx(&sei);
	if (sei.hProcess != nullptr)
	{
		WaitForSingleObject(sei.hProcess, INFINITE);
	}

	AppendPath(3, std::filesystem::current_path());
	AppendPath(3, L"LICENSE");
	AppendPath(2, std::filesystem::current_path());
	AppendPath(2, L"xenia.pdb");
	std::filesystem::remove_all(b[0]);
	std::filesystem::remove_all(b[1]);
	std::filesystem::remove_all(b[2]);
	std::filesystem::remove_all(b[3]);

	AppendPath(4, std::filesystem::current_path());
	AppendPath(4, L"xenia.exe");
	sei = {};
	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.fMask = 64;
	sei.nShow = 5;
	sei.lpParameters = L"--license_mask -1";
	sei.lpFile = b[4];
	ShellExecuteEx(&sei);


	if (sei.hProcess != nullptr)
	{
		WaitForSingleObject(sei.hProcess, INFINITE);
	}

	exit(0);
}

void hbmame()
{
	if (x64())
	{
		*b[0] = '\0';
		*b[1] = '\0';
		AppendPath(0, std::filesystem::current_path());
		AppendPath(0, L"7z.exe");
		URL(L"7z.exe", 0);
		AppendPath(1, std::filesystem::current_path());
		AppendPath(1, L"HBMAME.7z");
		CustomURL(L"https://hbmame.1emulation.com/hbmameui19.7z", 1);
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

	CheckOneInstance();

	HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME, CW_USEDEFAULT, CW_USEDEFAULT, 390, 130, nullptr, nullptr, hInstance, nullptr);

	CreateWindow(L"BUTTON", L"Patch", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 10, 10, 75, 75, hWnd, reinterpret_cast<HMENU>(1), hInstance, nullptr);

	CreateWindow(L"BUTTON", L"Restore", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 290, 10, 75, 75, hWnd, reinterpret_cast<HMENU>(2), hInstance, nullptr);

	HWND ComboBox = CreateWindow(WC_COMBOBOX, L"", CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 90, 10, 190, 365, hWnd, NULL, hInstance, NULL);

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
				dota(false);
				break;
			case 2:
				minecraft();
				break;
			case 3:
				mesen();
				break;
			case 4:
				xenia();
				break;
			case 5:
				fbneo();
				break;
			case 6:
				hbmame();
				break;
			case 7:
				mame();
				break;
			case 8:
				winaio();
				break;
			case 9:
				DirectX9();
				break;
			case 10:
				zip();
				break;
			case 11:
				activate();
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
				dota(true);
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
