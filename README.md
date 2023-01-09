
#  Process Hiding Driver

This is a driver for the windows kernel built with WDM.
Its sole intention is to receive a Process Identifier
and hide the corresponding process from the User Mode.

The only library used is `ntddk.h`, which made this a bit of a challenge to write,
since there are some parts of the Windows kernel, such as the `EPROCESS` struct,
that are not included and therefore need to be accessed with memory offsets.

For that same reason, the driver might not work when you run it.
While it does work as of January 2023, the struct `EPROCESS` might change as the kernel
continues to evolve. I assume the only severe modification that needs to be made,
independent of the build date, is setting the offsets to their correct value.

The offset values for your kernel version can be found using Windbg,
with the `dt nt!_eprocess` command.



## Side Note: Driver Signatures

Do note you need to disable driver signature verification.

This is scary, change this at your own risk, and certainly from a VM.

I ran this driver on a Win10 machine
configured for stuff like this, with vmmon and Windbg.
## Documentation

The code contains five functions.

* VOID PrintToDebugger()  : a basic function that prints to Windbg. Useful in case of VM Debugging.

* VOID PrintAllProcesses() : iterating over all processes in the system and printing the name and PID to a debugger.

* VOID HideProcess(DWORD32 pid) :  receives a PID and hides the corresponding process.

* VOID DriverUnload(...) : Mandatory function that unloads the driver.

* NTSTATUS DriverEntry(...) : Acts as a main function for the driver.

To change the target pid, change the parameter passed to HideProcess in line 93.
    
    ...
    // PrintToDebugger();
    // PrintAllProcesses();
    HideProcess(PID);

    return STATUS_SUCCESS;
    ...

I hope to make this part easier in a future update, by creating a user mode runnable program.
## Acknowledgements

 - [PR0CESS/HideMyAss](https://github.com/aaaddress1/PR0CESS/tree/main/HideMyAss)
 - [Manipulating ActiveProcessLinks to Hide Processes in Userland](https://www.ired.team/miscellaneous-reversing-forensics/windows-kernel-internals/manipulating-activeprocesslinks-to-unlink-processes-in-userland)
