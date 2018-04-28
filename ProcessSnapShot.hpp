#include<windows.h>
#include<vector>
#include<iostream>
//#undef UNICODE
#include<tlhelp32.h>
#pragma warning(disable:4661)

#define PINFO_EMPTY		-1
#define NOT_FOUND		-2
#define INVALID_ID		-1

typedef VOID (__stdcall *PFUNC_GET_MODULE)(MODULEENTRY32, LPVOID);

#pragma once

namespace snapprocess{
	template<typename StrT>
	struct ProcInfo{
		std::vector<StrT> allProcesses;
		std::vector<unsigned long> pIds;
		std::vector<unsigned long> ppIds;

		bool isEmpty(){
			return (allProcesses.empty() && pIds.empty() && ppIds.empty());
		}

		void clear(){
			allProcesses.shrink_to_fit();	//better than std::vector::clear() i think.
			pIds.shrink_to_fit();
			ppIds.shrink_to_fit();
		}
	};

	template<typename CharT>
	class ProcessSnapShot{
		ProcInfo<std::basic_string<CharT>> procInfo;
		
	public:
		friend std::basic_ostream<CharT>& operator << (std::basic_ostream<CharT>& out, ProcessSnapShot<CharT>& pInfo);

		ProcessSnapShot(){
			getAllProcId();
		}

		void getAllProcId();
		DWORD traverse(const CharT*);
		DWORD getProcessID(const CharT*);
		DWORD getParentProcessID(const CharT*);

		ProcInfo<std::basic_string<CharT>> getProcInfo(){ return procInfo; }
		void getProcessModules(DWORD, PFUNC_GET_MODULE, LPVOID);
	};

	template void ProcessSnapShot<char>::getAllProcId(void);
	template DWORD ProcessSnapShot<char>::traverse(const char*);
	template DWORD ProcessSnapShot<char>::getProcessID(const char*);
	template DWORD ProcessSnapShot<char>::getParentProcessID(const char*);
	template ProcInfo<std::string> ProcessSnapShot<char>::getProcInfo(void);

	template void ProcessSnapShot<wchar_t>::getAllProcId(void);
	template DWORD ProcessSnapShot<wchar_t>::traverse(const wchar_t*);
	template DWORD ProcessSnapShot<wchar_t>::getProcessID(const wchar_t*);
	template DWORD ProcessSnapShot<wchar_t>::getParentProcessID(const wchar_t*);
	template ProcInfo<std::wstring> ProcessSnapShot<wchar_t>::getProcInfo(void);

	std::ostream& operator << (std::ostream& out, ProcessSnapShot<char>& pInfo);
	std::wostream& operator << (std::wostream& out, ProcessSnapShot<wchar_t>& pInfo);
}
