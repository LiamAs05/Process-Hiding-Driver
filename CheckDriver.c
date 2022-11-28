#include <winternl.h>
#include <ntddk.h>

void print_to_debugger()
{
	DbgPrint("Hello from the kernel :)\nWith great power comes great responsibility!\n");
}

void list_all_procs()
{
	SYSTEM_PROCESS_INFORMATION s[100];
	NTSTATUS status = NtQuerySystemInformation(SystemProcessInformation, &s, sizeof(SYSTEM_PROCESS_INFORMATION) * 100, NULL);
	
	if (status != STATUS_SUCCESS)
	{
		DbgPrint("This whole operation is a failue, status: (0x%08X)\n", status);
		return;
	}

	for (int i = 0; i < 100; i++)
	{
		if (!s->NextEntryOffset)
			break;

		DbgPrint("Process %d with name %s", s->UniqueProcessId, s->ImageName);
	}
}

VOID DriverUnload(PDRIVER_OBJECT DriverObject) {
	// delete device object
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

	print_to_debugger();

	return STATUS_SUCCESS;
}