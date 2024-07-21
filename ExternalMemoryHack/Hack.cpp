#include <iostream>
#include <Windows.h>
#include<TlHelp32.h>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <tchar.h> //_tcsmp()

using namespace std;


DWORD GetModuleBaseAddress(TCHAR* lpszModuleName, DWORD pID) {
    DWORD dwModuleBaseAddress = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID); // make snapshot of all modules within process
    MODULEENTRY32 ModuleEntry32 = { 0 };
    ModuleEntry32.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(hSnapshot, &ModuleEntry32)) //store first Module in ModuleEntry32
    {
        do {
            if (_tcscmp(ModuleEntry32.szModule, lpszModuleName) == 0) // if Found Module matches Module we look for -> done!
            {
                dwModuleBaseAddress = (DWORD)ModuleEntry32.modBaseAddr;
                break;
            }
        } while (Module32Next(hSnapshot, &ModuleEntry32)); // go through Module entries in Snapshot and store in ModuleEntry32


    }
    CloseHandle(hSnapshot);
    return dwModuleBaseAddress;
}


DWORD GetPointerAddress(HWND hwnd, DWORD gameBaseAddr, DWORD address, vector<DWORD> offsets)
{
    DWORD pID = NULL; // Game process ID
    GetWindowThreadProcessId(hwnd, &pID);
    HANDLE phandle = NULL;
    phandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);
    if (phandle == INVALID_HANDLE_VALUE || phandle == NULL);

    DWORD offset_null = NULL;
    ReadProcessMemory(phandle, (LPVOID*)(gameBaseAddr + address), &offset_null, sizeof(offset_null), 0);
    DWORD pointeraddress = offset_null; // the address we need
    for (int i = 0; i < offsets.size() - 1; i++) // we dont want to change the last offset value so we do -1
    {
        ReadProcessMemory(phandle, (LPVOID*)(pointeraddress + offsets.at(i)), &pointeraddress, sizeof(pointeraddress), 0);
    }
    return pointeraddress += offsets.at(offsets.size() - 1); // adding the last offset
}


int main(int argc, char** argv) {

	HWND RE4_window = FindWindow(nullptr,"Resident Evil 4");

	DWORD process_id = 0;
	GetWindowThreadProcessId(RE4_window, &process_id);

	HANDLE RE4_process = OpenProcess(PROCESS_ALL_ACCESS, true, process_id);

	
	//DWORD bytes_read = 0;



    char gamemodule[] = "re4.exe";
    DWORD gamebaseaddress1 = GetModuleBaseAddress(_T(gamemodule),process_id);

    DWORD assult_ammo = 0x0DBB3CB8;
    vector<DWORD> assult_ammo_offsets{ 0x20 , 0x48 , 0x308 , 0xA0 , 0x170, 0x20, 0x84};

    DWORD ammoPtrAddress = GetPointerAddress(RE4_window , gamebaseaddress1, assult_ammo, assult_ammo_offsets);

    while (true) {
        int new_assult_ammo = 69;
        WriteProcessMemory(RE4_process, (void*)ammoPtrAddress, &new_assult_ammo, 4, 0);
    }


//	ReadProcessMemory(RE4_process,(void*)0x14DBB3CB8,&assult_ammo, 4 ,0);


    
    //DWORD bytes_written = 0;

  //  WriteProcessMemory(RE4_process, (void*)assult_ammo, &new_assult_ammo, 4, 0);


	return 0;
}

/*

#include <Windows.h>

int main() {
    HWND RE4_window = FindWindow(NULL, L"Resident Evil 4");

    if (RE4_window == NULL) {
        std::cerr << "Error: Could not find Resident Evil 4 window." << std::endl;
        return 1;
    }

    DWORD process_id = 0;
    GetWindowThreadProcessId(RE4_window, &process_id);

    if (process_id == 0) {
        std::cerr << "Error: Could not retrieve process ID." << std::endl;
        return 1;
    }

    HANDLE RE4_process = OpenProcess(PROCESS_ALL_ACCESS, true, process_id);

    if (RE4_process == NULL) {
        std::cerr << "Error: Could not open process." << std::endl;
        return 1;
    }

    DWORD assault_ammo = 0;
    DWORD bytes_read = 0;

    if (!ReadProcessMemory(RE4_process, (void*)0x14DBB3CB8, &assault_ammo, sizeof(DWORD), reinterpret_cast<SIZE_T*>(&bytes_read))) {
        std::cerr << "Error: ReadProcessMemory failed." << std::endl;
        CloseHandle(RE4_process);
        return 1;
    }

    std::cout << "Assault Ammo: " << assault_ammo << std::endl;

    CloseHandle(RE4_process);
    return 0;
}
*/

/*

// FindWindow, GetWindowThreadProcessId, OpenProcess, ReadProcessMemory, and
WriteProcessMemory are all contained inside windows.h
#include <Windows.h>
int main(int argc, char** argv) {
 /*
 To use ReadProcessMemory and WriteProcessMemory, we require a handle
to the Wesnoth process.

 To get this handle, we require a process id. The quickest way to get
a process id for a particular
 process is to use GetWindowThreadProcessId.

 GetWindowThreadProcessId requires a window handle (different than a
process handle). To get this
 window handle, we use FindWindow.
 

 // Find our Wesnoth window. Depending on your language settings, this
might be different.
HWND wesnoth_window = FindWindow(NULL, L"The Battle for Wesnoth -
	1.14.9");
	// Get the process id for the Wesnoth process. GetWindowThreadProcessId
	does not return a process id, but
	// rather fills a provided variable with its value, hence the &.
	DWORD process_id = 0;
GetWindowThreadProcessId(wesnoth_window, &process_id);
// Open our Wesnoth process. PROCESS_ALL_ACCESS means we can both read
and write to the process.However,
// it also means that this program must be executed as an administrator
to work.
HANDLE wesnoth_process = OpenProcess(PROCESS_ALL_ACCESS, true,
	process_id);
// Read the value at 0x017EED18 and place its value into the variable
gold_value.
DWORD gold_value = 0;
DWORD bytes_read = 0;
ReadProcessMemory(wesnoth_process, (void*)0x017EED18, &gold_value, 4,
	&bytes_read);
// Add 0xA90 to the value read from the last step and then read the value
at that new address.These
// offsets are covered in https://gamehacking.academy/lesson/13
gold_value += 0xA90;
ReadProcessMemory(wesnoth_process, (void*)gold_value, &gold_value, 4,
	&bytes_read);
// Add 4 to the gold_value, which will then be pointing at the player's
current gold address.
// Write the value of new_gold_value (555) into this address
gold_value += 4;
DWORD new_gold_value = 555;
DWORD bytes_written = 0;
WriteProcessMemory(wesnoth_process, (void*)gold_value, &new_gold_value,
	4, &bytes_written);
return 0;
}

*/ 