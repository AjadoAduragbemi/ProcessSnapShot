
#include "stdafx.h"
#include "ProcessSnapShot.hpp"

namespace snapprocess {
	
	void ProcessSnapShot::getAllProcId(){
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(PROCESSENTRY32);
		HANDLE snapProc = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		procInfo.clear();
		if(snapProc != INVALID_HANDLE_VALUE){
			if(Process32First(snapProc, &procEntry)) {
				//Process32Next(snapProc, &procEntry);
				do{
					procInfo.allProcesses.push_back(static_cast<wstring>(procEntry.szExeFile));
					procInfo.pIds.push_back(procEntry.th32ProcessID);
					procInfo.ppIds.push_back(procEntry.th32ParentProcessID);
				}while(Process32Next(snapProc, &procEntry));
			}

			CloseHandle(snapProc);
		}
	}

	DWORD ProcessSnapShot::getProcessID(wstring file_name){
		int index = 0;
		DWORD pId = 0;
		
		index = traverse(file_name);
		index = (index == PINFO_EMPTY || index == NOT_FOUND) ? INVALID_ID : index;
		
		return (index != INVALID_ID) ? procInfo.pIds[index] : INVALID_ID;
	}
	
	DWORD ProcessSnapShot::getParentProcessID(wstring file_name){
		int index = 0;
		DWORD pId = 0;
		
		index = traverse(file_name);
		index = (index == PINFO_EMPTY || index == NOT_FOUND) ? INVALID_ID : index;
		
		return (index != INVALID_ID) ? procInfo.ppIds[index] : INVALID_ID;
	}

	wostream& operator << (wostream& out, ProcessSnapShot& procs){
		int max_proc_name_len = 0;
		ProcInfo pInfo = procs.procInfo;
		if(pInfo.isEmpty()) out << "Empty\n";
		else{
			vector<wstring>::iterator procName;
			vector<DWORD>::iterator pId;
			vector<DWORD>::iterator ppId;
			wchar_t *wstr = new wchar_t[40];

			swprintf_s(wstr, 40, L"%16.16s\t%6s\t%6s", L"procFile", L"PID", L"PPID");
			out << wstr << endl;
			for(procName = pInfo.allProcesses.begin(), pId = pInfo.pIds.begin(), ppId = pInfo.ppIds.begin();
				procName != pInfo.allProcesses.end() && pId != pInfo.pIds.end() && ppId != pInfo.ppIds.end();
				++procName, ++pId, ++ppId){

				swprintf_s(wstr, 40, L"%16.16s\t%6.0d\t%6.0d", (*procName).c_str(), *pId, *ppId);
				out << wstr << endl;
			}

			delete[] wstr;
		}

		return out;
	}

	DWORD ProcessSnapShot::traverse(wstring to_find){
		int index = 0;
		if(procInfo.isEmpty())
			return PINFO_EMPTY;
		else{
			vector<wstring>::iterator iter = procInfo.allProcesses.begin();
			while(iter != procInfo.allProcesses.end() && (*iter).compare(to_find) != 0){
				index++;
				iter++;
			}

			if(iter == procInfo.allProcesses.end()) return NOT_FOUND;
		}

		return index;
	}

	void ProcessSnapShot::getProcessModules(DWORD pid, PFUNC_GET_MODULE foo, LPVOID arg){
		MODULEENTRY32 modEntry = {0};
		modEntry.dwSize = sizeof(MODULEENTRY32);
		HANDLE snapProc = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	
		if(snapProc != INVALID_HANDLE_VALUE){
			if(Module32First(snapProc, &modEntry)) {
				//Module32Next(snapProc, &modEntry);
				do{
					foo(modEntry, arg);
				}while(Module32Next(snapProc, &modEntry));
			}

			CloseHandle(snapProc);
		}
	}
}