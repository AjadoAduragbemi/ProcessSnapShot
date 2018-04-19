#include<windows.h>
#include<vector>
#include<iostream>
#include<tlhelp32.h>

using namespace std;

#define PINFO_EMPTY		-1
#define NOT_FOUND		-2
#define INVALID_ID		-1

typedef VOID (__stdcall *PFUNC_GET_MODULE)(MODULEENTRY32, LPVOID);

#pragma once

namespace snapprocess{
	
	struct ProcInfo{
		vector<wstring> allProcesses;
		vector<unsigned long> pIds;
		vector<unsigned long> ppIds;

		bool isEmpty(){
			return (allProcesses.empty() && pIds.empty() && ppIds.empty());
		}

		void clear(){
			allProcesses.shrink_to_fit();	//better than vector::clear() i think.
			pIds.shrink_to_fit();
			ppIds.shrink_to_fit();
		}
	};


	class ProcessSnapShot{
		ProcInfo procInfo;
		
	public:
		friend wostream& operator << (wostream& out, ProcessSnapShot& pInfo);

		ProcessSnapShot(){
			getAllProcId();
		}

		void getAllProcId();
		DWORD traverse(wstring);
		DWORD getProcessID(wstring);
		DWORD getParentProcessID(wstring);

		ProcInfo getProcInfo(){ return procInfo; }
		void getProcessModules(DWORD, PFUNC_GET_MODULE, LPVOID);
	};
}
