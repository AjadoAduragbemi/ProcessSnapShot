
#include "stdafx.h"
#include "ProcessSnapShot.hpp"
#include <string>

namespace snapprocess {

	std::string getProcess(PROCESSENTRY32 procEntry, ProcInfo<std::string> procInfo){
		CHAR szExeFile[MAX_PATH];
		WideCharToMultiByte(0, NULL, procEntry.szExeFile, MAX_PATH, szExeFile, MAX_PATH, NULL, NULL);
		return szExeFile;
	}

	std::wstring getProcess(PROCESSENTRY32 procEntry, ProcInfo<std::wstring> procInfo){
		return procEntry.szExeFile;
	}

	template<typename Ty>
	void ProcessSnapShot<Ty>::getAllProcId(){
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(PROCESSENTRY32);
		HANDLE snapProc = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		procInfo.clear();
		if(snapProc != INVALID_HANDLE_VALUE){
			if(Process32First(snapProc, &procEntry)) {
				//Process32Next(snapProc, &procEntry);
				do{
					procInfo.allProcesses.push_back(getProcess(procEntry, procInfo));
					procInfo.pIds.push_back(procEntry.th32ProcessID);
					procInfo.ppIds.push_back(procEntry.th32ParentProcessID);
				}while(Process32Next(snapProc, &procEntry));
			}

			CloseHandle(snapProc);
		}
	}

	template<typename Ty>
	DWORD ProcessSnapShot<Ty>::getProcessID(const Ty* file_name){
		int index = 0;
		DWORD pId = 0;
		
		index = traverse(file_name);
		index = (index == PINFO_EMPTY || index == NOT_FOUND) ? INVALID_ID : index;
		
		return (index != INVALID_ID) ? procInfo.pIds[index] : INVALID_ID;
	}

	template<typename Ty>
	DWORD ProcessSnapShot<Ty>::getParentProcessID(const Ty* file_name){
		int index = 0;
		DWORD pId = 0;
		
		index = traverse(file_name);
		index = (index == PINFO_EMPTY || index == NOT_FOUND) ? INVALID_ID : index;
		
		return (index != INVALID_ID) ? procInfo.ppIds[index] : INVALID_ID;
	}

	template<typename Ty>
	DWORD ProcessSnapShot<Ty>::traverse(const Ty* to_find){
		int index = 0;
		if(procInfo.isEmpty())
			return PINFO_EMPTY;
		else{
			std::vector<std::basic_string<Ty>>::iterator iter = procInfo.allProcesses.begin();
			while(iter != procInfo.allProcesses.end() && (*iter).compare(to_find) != 0){
				index++;
				iter++;
			}

			if(iter == procInfo.allProcesses.end()) return NOT_FOUND;
		}

		return index;
	}

	template<typename Ty>
	void ProcessSnapShot<Ty>::getProcessModules(DWORD pid, PFUNC_GET_MODULE foo, LPVOID arg){
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

	template<typename CharT>
	std::basic_ostream<CharT>& psOutput(std::basic_ostream<CharT>& out, ProcInfo<std::basic_string<CharT>>& pInfo){
		int max_proc_name_len = 0;
		if(pInfo.isEmpty()) out << "Empty" << std::endl;
		else{
			std::vector<std::basic_string<CharT>>::iterator procName;
			std::vector<DWORD>::iterator pId;
			std::vector<DWORD>::iterator ppId;
			
			out <<  "ProcessName" << std::basic_string<CharT>(25, ' ') << "PID      PPID" << std::endl; 

			for(procName = pInfo.allProcesses.begin(), pId = pInfo.pIds.begin(), ppId = pInfo.ppIds.begin();
				procName != pInfo.allProcesses.end() /*&& pId != pInfo.pIds.end() && ppId != pInfo.ppIds.end()*/;
				++procName, ++pId, ++ppId){
				std::basic_string<CharT> strProcName = (*procName).substr(0, 32);
				std::basic_string<CharT> strSpaceCount0((32-strProcName.length()) + 4, L' ');
				int cbSpaceCount1 = 0, denom = 1, expr = *pId/denom;
				while(expr > 0){
					cbSpaceCount1++;
					denom *= 10;
					expr = *pId/denom;
				}
				out <<  strProcName <<  strSpaceCount0
					<< *pId <<  std::basic_string<CharT>(10-cbSpaceCount1, ' ')
					<< *ppId << std::endl;
				//Sleep(100);
			}
		}

		return out;
	}

	std::wostream& operator << (std::wostream& out, ProcessSnapShot<wchar_t>& procs){
		ProcInfo<std::wstring> pInfo = procs.procInfo;
		return psOutput(out, pInfo);
	}

	std::ostream& operator << (std::ostream& out, ProcessSnapShot<char>& procs){
		ProcInfo<std::string> pInfo = procs.procInfo;
		return psOutput(out, pInfo);
	}
}
