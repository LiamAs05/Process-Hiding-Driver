#include <ntddk.h>

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


	DbgPrint("Hello from the kernel :)\nWith great power comes great responsibility!");
	PEPROCESS s = PsGetCurrentProcess();
	
	DbgPrint();
	

	return STATUS_SUCCESS;
}