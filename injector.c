#include <windows.h>
#include <stdio.h>
#include <wchar.h>
#include <Python.h>

char notepad[] = "C:\\Windows\\system32\\notepad.exe";

static PyObject* _inject(PyObject* self, PyObject* args) {
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	HANDLE hProcess, hThread;
	HMODULE hKernel32;
	DWORD PID, TID;
	LPVOID rBuffer;

	wchar_t dllPath[MAX_PATH];
	size_t dllPathSize;
	char* dllPathStr;

	if (!PyArg_ParseTuple(args, "s", &dllPathStr)) {
		return Py_None;
	}
	
	mbstowcs(dllPath, dllPathStr, MAX_PATH);
	dllPathSize = wcslen(dllPath) * sizeof(wchar_t);

	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	si.cb = sizeof(si);

	// 1. Create a suspended process
	printf("[+] Creating process: %hs\n", notepad);
	if (!CreateProcess(NULL, notepad, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi)) {
		printf("[!] Failed to create process: %d\n", GetLastError());
		return Py_None;
	}

	printf("[+] Created process: %d\n", pi.dwProcessId);
	PID = pi.dwProcessId;
	
	// 2. Open the process and get a handle
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	if (hProcess == NULL) {
		printf("[!] Failed to open process: %d\n", GetLastError());
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		return Py_None;
	}

	printf("[+] Got process handle: 0x%p\n", hProcess);

	// 3. Allocate memory in the target process for the DLL path
	rBuffer = VirtualAllocEx(hProcess, NULL, dllPathSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (rBuffer == NULL) {
		printf("[!] Failed to allocate memory: %d\n", GetLastError());
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		return Py_None;
	}

	printf("[+] Allocated memory at: 0x%p\n", rBuffer);

	// 4. Write the DLL path to the allocated memory
	if (!WriteProcessMemory(hProcess, rBuffer, dllPath, dllPathSize, NULL)) {
		printf("[!] Failed to write memory: %d\n", GetLastError());
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		return Py_None;
	}

	printf("[+] Wrote memory: %ls\n", dllPath);
	
	// 5. Get the handle of kernel32.dll
	hKernel32 = GetModuleHandleW(L"Kernel32.dll");
	if (hKernel32 == NULL) {
		printf("[!] Failed to get kernel32 handle: %d\n", GetLastError());
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		CloseHandle(hProcess);
		return Py_None;
	}

	printf("[+] Got kernel32 handle: 0x%p\n", hKernel32);
	
	// 6. Get the address of LoadLibraryW function
	LPTHREAD_START_ROUTINE loadLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "LoadLibraryW");
	if (loadLibrary == NULL) {
		printf("Failed to get LoadLibraryW address: %d\n", GetLastError());
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		CloseHandle(hProcess);
		return Py_None;
	}

	printf("[+] Got LoadLibraryW address: 0x%p\n", loadLibrary);

	// 7. Create a remote thread in the target process to execute LoadLibraryW
	hThread = CreateRemoteThread(hProcess, NULL, 0, loadLibrary, rBuffer, 0, NULL);
	if (hThread == NULL) {
		printf("[!] Failed to create remote thread: %d\n", GetLastError());
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		CloseHandle(hProcess);
		return Py_None;
	}

	printf("[+] Created remote thread: 0x%p\n", hThread);
	
	// 8. Wait for the thread to finish and get the exit code
	WaitForSingleObject(hThread, INFINITE);
	if (GetExitCodeThread(hThread, &TID) == 0) {
		printf("[!] Failed to get exit code: %d\n", GetLastError());
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		CloseHandle(hProcess);
		CloseHandle(hThread);
		return Py_None;
	}

	printf("[+] Thread exit code: %d\n", TID);

	// 9. Check if the DLL was injected successfully
	if (TID == 0) {
		printf("[!] Failed to inject DLL: %d\n", GetLastError());
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		CloseHandle(hProcess);
		CloseHandle(hThread);
		return Py_None;
	}

	// 10. Clean up
	if (!VirtualFreeEx(hProcess, rBuffer, 0, MEM_RELEASE)) {
		printf("[!] Failed to free memory: %d\n", GetLastError());
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		CloseHandle(hProcess);
		CloseHandle(hThread);
		return Py_None;
	}

	printf("[+] Freed memory: 0x%p\n", rBuffer);

	CloseHandle(hThread);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	CloseHandle(hProcess);

	printf("[*] Injection completed successfully.\n");
	return Py_None; 
}

static PyMethodDef InjectorMethods[] = {
    {"inject", _inject, METH_VARARGS, "Injects a provided DLL."},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef injectormodule = {
    PyModuleDef_HEAD_INIT,
    "injector",
    NULL,
    -1,
    InjectorMethods
};

PyMODINIT_FUNC PyInit_injector(void) {
    return PyModule_Create(&injectormodule);
}
