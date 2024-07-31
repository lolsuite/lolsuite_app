#define WIN32_LEAN_AND_MEAN
#include <ShlObj_core.h>
#include <tlhelp32.h>
#include <shellapi.h>
#include <urlmon.h>
#include "resource.h"
#include <SDKDDKVer.h>
import std.filesystem;

constexpr auto MAX_LOADSTRING = 100;
wchar_t b[83][MAX_PATH + 1];
SHELLEXECUTEINFOW sei;
int cb;
HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
LPFN_ISWOW64PROCESS fnIsWow64Process;

const wchar_t* box[7] = {
	L"League of Legends", L"DOTA2", L"Diablo IV", L"Minecraft : Java Edition", L"Mesen2", L"Xenia (Unlocked)", L"Arcade Emu Pack"
};

std::wstring JoinPath(const int j, const std::wstring& add)
{
	const std::filesystem::path p = b[j];
	return (p / add).c_str();
}

bool SingleInstance()
{

	HANDLE  m_hStartEvent = CreateEvent(nullptr, FALSE, FALSE, L"Global\\CSAPP");

	if (m_hStartEvent == nullptr)
	{
		CloseHandle(m_hStartEvent);
		return false;
	}


	if (GetLastError() == ERROR_ALREADY_EXISTS) {

		CloseHandle(m_hStartEvent);
		m_hStartEvent = nullptr;
		MessageBox(nullptr, L"One instance is allowed", L"LoLSuite", MB_OK);
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

void TerminateProcess(const std::wstring& process_name)
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

void Ublock(std::wstring file)
{
	DeleteFile(file.append(L":Zone.Identifier").c_str());
}

void serv(const std::wstring& url, int j)
{
	URLDownloadToFile(nullptr, std::wstring(L"https://lolsuite.org/f/" + url).c_str(), b[j], 0, nullptr);
	Ublock(b[j]);
}

void download(const std::wstring& url, int j)
{
	URLDownloadToFile(nullptr, url.c_str(), b[j], 0, nullptr);
	Ublock(b[j]);
}


BOOL x64()
{
	BOOL bIsWow64 = FALSE;

    fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "IsWow64Process");

	if (NULL != fnIsWow64Process)
	{
		if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64))
		{
			//handle error
		}
	}
	return bIsWow64;
}


void lol(bool restore)
{
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

	// reset variables
	for (int i = 0; i < 83; i++)
	{
		*b[i] = '\0';
	}

	// Client Files
	AppendPath(82, std::filesystem::current_path());
	AppendPath(82, L"LoLSuite.ini");
	GetPrivateProfileString(L"Path", L"League of Legends", nullptr, b[0], 261, b[82]);
	if (std::wstring(b[0]).empty())
	{
		BROWSEINFO i{};
		i.ulFlags = BIF_USENEWUI | BIF_NONEWFOLDERBUTTON;
		i.lpszTitle = L"<drive>:\\Riot Games";
		const auto dl = SHBrowseForFolder(&i);
		if (dl == nullptr)
		{
			exit(0);
		}
		SHGetPathFromIDList(dl, b[0]);
		WritePrivateProfileString(L"Path", L"League of Legends", b[0], b[82]);
	}

	// Clear Running Processes
	TerminateProcess(L"LeagueClient.exe");
	TerminateProcess(L"LeagueClientUx.exe");
	TerminateProcess(L"LeagueClientUxRender.exe");
	TerminateProcess(L"League of Legends.exe");
	TerminateProcess(L"Riot Client.exe");
	TerminateProcess(L"RiotClientServices.exe");

	// Set Riot Client Executable Path
	CombinePath(54, 0, L"Riot Client\\RiotClientElectron");

	// Base Dir
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

	// Game Directory
	CombinePath(51, 0, L"Game");
	Ublock(JoinPath(51, L"League of Legends.exe"));


	CombinePath(52, 51, L"D3DCompiler_47.dll");
	CombinePath(53, 51, L"D3dx9_43.dll");
	CombinePath(55, 51, L"xinput1_3.dll");

	// Experimental
	CombinePath(56, 51, L"tbb.dll");

	// Riot Client Replace DLL
	CombinePath(57, 54, L"d3dcompiler_47.dll");
	if (restore)
	{


		// Client Files
		for (int i = 0; i < 40; i++)
		{
			CombinePath(i + 1, 0, apimswin[i]);
			serv(&std::wstring(L"r/lol/" + std::wstring(apimswin[i]))[0], i + 1);
		}
		serv(L"r/lol/concrt140.dll", 42);
		serv(L"r/lol/d3dcompiler_47.dll", 43);
		serv(L"r/lol/msvcp140.dll", 44);
		serv(L"r/lol/msvcp140_1.dll", 45);
		serv(L"r/lol/msvcp140_2.dll", 46);
		serv(L"r/lol/msvcp140_codecvt_ids.dll", 47);
		serv(L"r/lol/ucrtbase.dll", 48);
		serv(L"r/lol/vcruntime140.dll", 49);
		serv(L"r/lol/vcruntime140_1.dll", 50);

		// Game Files
		serv(L"r/lol/D3DCompiler_47.dll", 52);
		serv(L"r/lol/D3dx9_43.dll", 53);
		serv(L"r/lol/xinput1_3.dll", 55);
		DeleteFile(b[55]);

		serv(L"r/lol/d3dcompiler_47.dll", 57);
	}
	else
	{
		// Client Files
		for (int i = 0; i < 40; i++)
		{
			CombinePath(i + 1, 0, apimswin[i]);
			serv(&std::wstring(L"" + std::wstring(apimswin[i]))[0], i + 1);
		}
		serv(L"concrt140.dll", 42);
		serv(L"msvcp140.dll", 44);
		serv(L"D3DCompiler_47.dll", 43);
		serv(L"msvcp140_1.dll", 45);
		serv(L"msvcp140_2.dll", 46);
		serv(L"msvcp140_codecvt_ids.dll", 47);
		serv(L"ucrtbase.dll", 48);
		serv(L"vcruntime140.dll", 49);
		serv(L"vcruntime140_1.dll", 50);

		// Game Files
		serv(L"D3dx9_43.dll", 53);
		serv(L"xinput1_3.dll", 55);
		if (x64())
		{
			serv(L"6/D3DCompiler_47.dll", 52);
			serv(L"6/tbb12.dll", 56);
		}
		else
		{
			serv(L"D3DCompiler_47.dll", 52);
			serv(L"tbb12.dll", 56);
		}


		if (x64())
		{
			serv(L"6/D3DCompiler_47.dll", 57);
		}
		else
		{
			serv(L"D3DCompiler_47.dll", 57);
		}

		// Auto Start Client
		sei = {};
		sei.cbSize = sizeof(SHELLEXECUTEINFOW);
		sei.fMask = 64;
		sei.nShow = 5;
		sei.lpFile = JoinPath(54, L"Riot Client.exe").c_str();
		ShellExecuteExW(&sei);
	}
}

void dota2(bool restore)
{
	TerminateProcess(L"dota2.exe");
	for (int i = 0; i < 83; i++)
	{
		*b[i] = '\0';
	}

	AppendPath(82, std::filesystem::current_path());
	AppendPath(82, L"LoLSuite.ini");
	GetPrivateProfileString(L"Path", L"DOTA 2", nullptr, b[0], 261, b[82]);
	if (std::wstring(b[0]).empty())
	{
		BROWSEINFO i{};
		i.ulFlags = BIF_USENEWUI | BIF_NONEWFOLDERBUTTON;
		i.lpszTitle = L"<drive>:\\Steam\\steamapps\\common\\dota 2 beta";
		const auto dl = SHBrowseForFolder(&i);
		if (dl == nullptr)
		{
			exit(0);
		}
		SHGetPathFromIDList(dl, b[0]);
		WritePrivateProfileString(L"Path", L"DOTA 2", b[0], b[82]);
	}
	AppendPath(0, L"game\\bin\\win64");
	CombinePath(1, 0, L"embree3.dll");
	Ublock(JoinPath(0, L"dota2.exe"));
	if (restore)
	{
		serv(L"r/dota2/embree3.dll", 1);
	}
	else
	{
		serv(L"6/embree4.dll", 1);

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

void diabloiv(bool restore)
{
	TerminateProcess(L"Diablo IV.exe");
	for (int i = 0; i < 83; i++)
	{
		*b[i] = '\0';
	}

	AppendPath(82, std::filesystem::current_path());
	AppendPath(82, L"LoLSuite.ini");
	GetPrivateProfileString(L"Path", L"DiabloIV", nullptr, b[0], 261, b[82]);
	if (std::wstring(b[0]).empty())
	{
		BROWSEINFO i{};
		i.ulFlags = BIF_USENEWUI | BIF_NONEWFOLDERBUTTON;
		i.lpszTitle = L"<drive>:\\Battle.net";
		const auto dl = SHBrowseForFolder(&i);
		if (dl == nullptr)
		{
			exit(0);
		}
		SHGetPathFromIDList(dl, b[0]);
		WritePrivateProfileString(L"Path", L"DiabloIV", b[0], b[82]);
	}
	AppendPath(0, L"Diablo IV");
	CombinePath(1, 0, L"msvcp140_1.dll");
	CombinePath(2, 0, L"msvcp140_2.dll");
	CombinePath(3, 0, L"msvcp140.dll");
	CombinePath(4, 0, L"msvcp140_codecvt_ids.dll");
	CombinePath(5, 0, L"vcruntime140.dll");
	CombinePath(6, 0, L"vcruntime140_1.dll");
	Ublock(JoinPath(0, L"Diablo IV.exe"));
	if (restore)
	{
		serv(L"r/diabloiv/msvcp140_1.dll", 1);
		serv(L"r/diabloiv/msvcp140_2.dll", 2);
		serv(L"r/diabloiv/msvcp140.dll", 3);
		serv(L"r/diabloiv/msvcp140_codecvt_ids.dll", 4);
		serv(L"r/diabloiv/vcruntime140.dll", 5);
		serv(L"r/diabloiv/vcruntime140_1.dll", 6);

	}
	else
	{
		serv(L"msvcp140_1.dll", 1);
		serv(L"msvcp140_2.dll", 2);
		serv(L"msvcp140.dll", 3);
		serv(L"msvcp140_codecvt_ids.dll", 4);
		serv(L"vcruntime140.dll", 5);
		serv(L"vcruntime140_1.dll", 6);

	}
	sei = {};
	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.fMask = 64;
	sei.nShow = 5;
	sei.lpFile = JoinPath(0, L"Diablo IV Launcher.exe").c_str();
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
	AppendPath(82, std::filesystem::current_path());
	if (x64())
	{
		AppendPath(82, L"jdk-22_windows-x64_bin.exe");
		download(L"https://download.oracle.com/java/22/latest/jdk-22_windows-x64_bin.exe", 82);
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
	std::filesystem::remove_all(b[82]);

	MessageBox(nullptr, L"Minecraft Launcher > Minecraft: Java Edition > Installations > Latest > Edit > More Options > Java Executable Path > <drive>:\\Program Files\\Java\\jdk-22\\bin\\javaw.exe", L"LoLSuite", MB_OK);
}

void DirectX9()
{
	*b[82] = '\0';
	AppendPath(82, std::filesystem::current_path());
	AppendPath(82, L"dxwebsetup.exe");
	download(L"https://download.microsoft.com/download/1/7/1/1718CCC4-6315-4D8E-9543-8E28A4E18C4C/dxwebsetup.exe", 82);
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

void redistaio()
{
	*b[0] = '\0';
	AppendPath(0, std::filesystem::current_path());
	AppendPath(0, L"VisualCppRedist_AIO_x86_x64.exe");
	download(L"https://github.com/abbodi1406/vcredist/releases/download/v0.82.0/VisualCppRedist_AIO_x86_x64.exe", 0);
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
	sei.lpParameters = L"/ai /gm2";
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
	*b[1] = '\0';
	AppendPath(0, std::filesystem::current_path());
	AppendPath(0, L"7z.exe");
	serv(L"7z.exe", 0);
	AppendPath(1, std::filesystem::current_path());
	AppendPath(1, L"FBNeo.zip");

	if (x64())
	{
		download(L"https://github.com/finalburnneo/FBNeo/releases/download/latest/Windows.x64.zip", 1);
	}
	else
	{
		download(L"https://github.com/finalburnneo/FBNeo/releases/download/latest/Windows.x32.zip", 1);
	}
	*b[2] = '\0';
	AppendPath(2, std::filesystem::current_path());
	AppendPath(2, L"FBNeo");

	CreateDirectoryW(b[2], NULL);
	sei = {};
	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.fMask = 64;
	sei.nShow = 5;
	sei.lpFile = b[0];
	sei.lpParameters = L"x FBNeo.zip -oFBNeo -y";
	ShellExecuteEx(&sei);
	if (sei.hProcess != nullptr)
	{
		WaitForSingleObject(sei.hProcess, INFINITE);
	}

	std::filesystem::remove_all(b[0]);
	std::filesystem::remove_all(b[1]);
}

void mame()
{
	if (x64())
	{
		*b[0] = '\0';
		AppendPath(0, std::filesystem::current_path());
		AppendPath(0, L"MAME.exe");
		download(L"https://github.com/mamedev/mame/releases/download/mame0268/mame0268b_64bit.exe", 0);
		*b[1] = '\0';
		AppendPath(1, std::filesystem::current_path());
		AppendPath(1, L"7z.exe");
		serv(L"7z.exe", 1);

		*b[2] = '\0';
		AppendPath(2, std::filesystem::current_path());
		AppendPath(2, L"MAME");

		CreateDirectoryW(b[2], NULL);
		sei = {};
		sei.cbSize = sizeof(SHELLEXECUTEINFOW);
		sei.fMask = 64;
		sei.nShow = 5;
		sei.lpFile = b[1];
		sei.lpParameters = L"x MAME.exe -oMAME -y";
		ShellExecuteEx(&sei);
		if (sei.hProcess != nullptr)
		{
			WaitForSingleObject(sei.hProcess, INFINITE);
		}

	}
	std::filesystem::remove_all(b[0]);
	std::filesystem::remove_all(b[1]);
}

void hbmame()
{
	if (x64())
	{
		*b[0] = '\0';
		*b[1] = '\0';
		AppendPath(0, std::filesystem::current_path());
		AppendPath(0, L"7z.exe");
		serv(L"7z.exe", 0);
		AppendPath(1, std::filesystem::current_path());
		AppendPath(1, L"HBMAME.7z");
		download(L"https://hbmame.1emulation.com/hbmameui20.7z", 1);

		*b[2] = '\0';
		AppendPath(2, std::filesystem::current_path());
		AppendPath(2, L"HBMAME");

		CreateDirectoryW(b[2], NULL);


		sei = {};
		sei.cbSize = sizeof(SHELLEXECUTEINFOW);
		sei.fMask = 64;
		sei.nShow = 5;
		sei.lpFile = b[0];
		sei.lpParameters = L"x HBMAME.7z -oHBMAME -y";
		ShellExecuteEx(&sei);
		if (sei.hProcess != nullptr)
		{
			WaitForSingleObject(sei.hProcess, INFINITE);
		}

		std::filesystem::remove_all(b[0]);
		std::filesystem::remove_all(b[1]);

	}
}

void mesen2()
{
	*b[0] = '\0';
	*b[1] = '\0';
	*b[2] = '\0';
	AppendPath(0, std::filesystem::current_path());
	AppendPath(0, L"7z.exe");
	AppendPath(1, std::filesystem::current_path());
	AppendPath(1, L"Mesen.zip");
	AppendPath(2, std::filesystem::current_path());
	AppendPath(2, L"dotnet.exe");

	download(L"https://download.visualstudio.microsoft.com/download/pr/bb581716-4cca-466e-9857-512e2371734b/5fe261422a7305171866fd7812d0976f/windowsdesktop-runtime-8.0.7-win-x64.exe", 2);
	download(L"https://nightly.link/SourMesen/Mesen2/workflows/build/master/Mesen%20%28Windows%20-%20net8.0%20-%20AoT%29.zip", 1);
	serv(L"7z.exe", 0);

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

	serv(L"7z.exe", 0);
	download(L"https://github.com/xenia-project/release-builds-windows/releases/latest/download/xenia_master.zip", 1);

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

	SingleInstance();

	HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME, CW_USEDEFAULT, CW_USEDEFAULT, 390, 130, nullptr, nullptr, hInstance, nullptr);

	CreateWindow(L"BUTTON", L"Patch", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 10, 75, 75, hWnd, reinterpret_cast<HMENU>(1), hInstance, nullptr);

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
				DirectX9();
				break;
			case 1:
				dota2(false);
				DirectX9();
				break;
			case 2:
				diabloiv(false);
				DirectX9();
				break;
			case 3:
				minecraft();
				break;
			case 4:
				mesen2();
				break;
			case 5:
				xenia();
				break;
			case 6:
				DirectX9();
				fbneo();
				hbmame();
				mame();
				exit(0);
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
				DirectX9();
				break;
			case 1:
				dota2(true);
				DirectX9();
				break;
			case 2:
				diabloiv(true);
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
