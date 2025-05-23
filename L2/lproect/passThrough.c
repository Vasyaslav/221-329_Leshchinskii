/*++

Copyright (c) 1999 - 2002  Microsoft Corporation

Module Name:

	passThrough.c

Abstract:

	This is the main module of the passThrough miniFilter driver.
	This filter hooks all IO operations for both pre and post operation
	callbacks.  The filter passes through the operations.

Environment:

	Kernel mode

--*/

#include <fltKernel.h>
#include <dontuse.h>
#include <suppress.h>
#include <ntddk.h>
#include "aes.h"

unsigned char key[] = { 0x04, 0x9a, 0x1d, 0x22, 0x0e, 0x28, 0xf0, 0x09, 0xf2, 0xa9, 0xfd, 0xdc, 0x5a, 0x0b, 0x7a, 0x42, 0x1c,
					0xb5, 0xb6, 0x42, 0xe2, 0xfb, 0x46, 0x4a, 0xd3, 0x86, 0x20, 0xa1, 0xc6, 0x20, 0x74, 0xc6 };
unsigned char iv[] = { 0x96, 0x38, 0x72, 0x5e, 0x2b, 0xfc, 0x8a, 0x49, 0xf0, 0x46, 0xc9, 0x85, 0x71, 0xf5, 0xae, 0x2b };

#pragma prefast(disable:__WARNING_ENCODE_MEMBER_FUNCTION_POINTER, "Not valid for kernel mode drivers")


PFLT_FILTER gFilterHandle;
ULONG_PTR OperationStatusCtx = 1;

#define PTDBG_TRACE_ROUTINES            0x00000001
#define PTDBG_TRACE_OPERATION_STATUS    0x00000002

ULONG gTraceFlags = 0;


#define PT_DBG_PRINT( _dbgLevel, _string )          \
    (FlagOn(gTraceFlags,(_dbgLevel)) ?              \
        DbgPrint _string :                          \
        ((int)0))

/*************************************************************************
	Prototypes
*************************************************************************/

DRIVER_INITIALIZE DriverEntry;
NTSTATUS
DriverEntry(
	_In_ PDRIVER_OBJECT DriverObject,
	_In_ PUNICODE_STRING RegistryPath
);

NTSTATUS
PtInstanceSetup(
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ FLT_INSTANCE_SETUP_FLAGS Flags,
	_In_ DEVICE_TYPE VolumeDeviceType,
	_In_ FLT_FILESYSTEM_TYPE VolumeFilesystemType
);

VOID
PtInstanceTeardownStart(
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags
);

VOID
PtInstanceTeardownComplete(
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags
);

NTSTATUS
PtUnload(
	_In_ FLT_FILTER_UNLOAD_FLAGS Flags
);

NTSTATUS
PtInstanceQueryTeardown(
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags
);

FLT_PREOP_CALLBACK_STATUS
PtPreOperationPassThrough(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID* CompletionContext
);

VOID
PtOperationStatusCallback(
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ PFLT_IO_PARAMETER_BLOCK ParameterSnapshot,
	_In_ NTSTATUS OperationStatus,
	_In_ PVOID RequesterContext
);

FLT_POSTOP_CALLBACK_STATUS
PtPostOperationPassThrough(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_opt_ PVOID CompletionContext,
	_In_ FLT_POST_OPERATION_FLAGS Flags
);

FLT_PREOP_CALLBACK_STATUS
PtPreOperationNoPostOperationPassThrough(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID* CompletionContext
);

BOOLEAN
PtDoRequestOperationStatus(
	_In_ PFLT_CALLBACK_DATA Data
);

//
//  Assign text sections for each routine.
//

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, PtUnload)
#pragma alloc_text(PAGE, PtInstanceQueryTeardown)
#pragma alloc_text(PAGE, PtInstanceSetup)
#pragma alloc_text(PAGE, PtInstanceTeardownStart)
#pragma alloc_text(PAGE, PtInstanceTeardownComplete)
#endif

//
//  operation registration
//

CONST FLT_OPERATION_REGISTRATION Callbacks[] = {
	{ IRP_MJ_CREATE,
	  0,
	  PtPreOperationPassThrough,
	  PtPostOperationPassThrough },

	{ IRP_MJ_CREATE_NAMED_PIPE,
	  0,
	  PtPreOperationPassThrough,
	  PtPostOperationPassThrough },

	{ IRP_MJ_CLOSE,
	  0,
	  PtPreOperationPassThrough,
	  PtPostOperationPassThrough },

	{ IRP_MJ_READ,
	  0,
	  PtPreOperationPassThrough,
	  PtPostOperationPassThrough },

	{ IRP_MJ_WRITE,
	  0,
	  PtPreOperationPassThrough,
	  PtPostOperationPassThrough },

	{ IRP_MJ_QUERY_INFORMATION,
	  0,
	  PtPreOperationPassThrough,
	  PtPostOperationPassThrough },

	{ IRP_MJ_SET_INFORMATION,
	  0,
	  PtPreOperationPassThrough,
	  PtPostOperationPassThrough },

	{ IRP_MJ_QUERY_EA,
	  0,
	  PtPreOperationPassThrough,
	  PtPostOperationPassThrough },

	{ IRP_MJ_SET_EA,
	  0,
	  PtPreOperationPassThrough,
	  PtPostOperationPassThrough },

	{ IRP_MJ_FLUSH_BUFFERS,
	  0,
	  PtPreOperationPassThrough,
	  PtPostOperationPassThrough },

	{ IRP_MJ_QUERY_VOLUME_INFORMATION,
	  0,
	  PtPreOperationPassThrough,
	  PtPostOperationPassThrough },

	{ IRP_MJ_SET_VOLUME_INFORMATION,
	  0,
	  PtPreOperationPassThrough,
	  PtPostOperationPassThrough },

	{ IRP_MJ_DIRECTORY_CONTROL,
	  0,
	  PtPreOperationPassThrough,
	  PtPostOperationPassThrough },

	{ IRP_MJ_FILE_SYSTEM_CONTROL,
	  0,
	  PtPreOperationPassThrough,
	  PtPostOperationPassThrough },

	{ IRP_MJ_DEVICE_CONTROL,
	  0,
	  PtPreOperationPassThrough,
	  PtPostOperationPassThrough },

	{ IRP_MJ_INTERNAL_DEVICE_CONTROL,
	  0,
	  PtPreOperationPassThrough,
	  PtPostOperationPassThrough },

	{ IRP_MJ_SHUTDOWN,
	  0,
	  PtPreOperationNoPostOperationPassThrough,
	  NULL },                               //post operations not supported

	{ IRP_MJ_LOCK_CONTROL,
	  0,
	  PtPreOperationPassThrough,
	  PtPostOperationPassThrough },

	{ IRP_MJ_CLEANUP,
	  0,
	  PtPreOperationPassThrough,
	  PtPostOperationPassThrough },

	{ IRP_MJ_CREATE_MAILSLOT,
	  0,
	  PtPreOperationPassThrough,
	  PtPostOperationPassThrough },

	{ IRP_MJ_QUERY_SECURITY,
	  0,
	  PtPreOperationPassThrough,
	  PtPostOperationPassThrough },

	{ IRP_MJ_SET_SECURITY,
	  0,
	  PtPreOperationPassThrough,
	  PtPostOperationPassThrough },

	{ IRP_MJ_QUERY_QUOTA,
	  0,
	  PtPreOperationPassThrough,
	  PtPostOperationPassThrough },

	{ IRP_MJ_SET_QUOTA,
	  0,
	  PtPreOperationPassThrough,
	  PtPostOperationPassThrough },

	{ IRP_MJ_PNP,
	  0,
	  PtPreOperationPassThrough,
	  PtPostOperationPassThrough },

	{ IRP_MJ_ACQUIRE_FOR_SECTION_SYNCHRONIZATION,
	  0,
	  PtPreOperationPassThrough,
	  PtPostOperationPassThrough },

	{ IRP_MJ_RELEASE_FOR_SECTION_SYNCHRONIZATION,
	  0,
	  PtPreOperationPassThrough,
	  PtPostOperationPassThrough },

	{ IRP_MJ_ACQUIRE_FOR_MOD_WRITE,
	  0,
	  PtPreOperationPassThrough,
	  PtPostOperationPassThrough },

	{ IRP_MJ_RELEASE_FOR_MOD_WRITE,
	  0,
	  PtPreOperationPassThrough,
	  PtPostOperationPassThrough },

	{ IRP_MJ_ACQUIRE_FOR_CC_FLUSH,
	  0,
	  PtPreOperationPassThrough,
	  PtPostOperationPassThrough },

	{ IRP_MJ_RELEASE_FOR_CC_FLUSH,
	  0,
	  PtPreOperationPassThrough,
	  PtPostOperationPassThrough },

	{ IRP_MJ_FAST_IO_CHECK_IF_POSSIBLE,
	  0,
	  PtPreOperationPassThrough,
	  PtPostOperationPassThrough },

	{ IRP_MJ_NETWORK_QUERY_OPEN,
	  0,
	  PtPreOperationPassThrough,
	  PtPostOperationPassThrough },

	{ IRP_MJ_MDL_READ,
	  0,
	  PtPreOperationPassThrough,
	  PtPostOperationPassThrough },

	{ IRP_MJ_MDL_READ_COMPLETE,
	  0,
	  PtPreOperationPassThrough,
	  PtPostOperationPassThrough },

	{ IRP_MJ_PREPARE_MDL_WRITE,
	  0,
	  PtPreOperationPassThrough,
	  PtPostOperationPassThrough },

	{ IRP_MJ_MDL_WRITE_COMPLETE,
	  0,
	  PtPreOperationPassThrough,
	  PtPostOperationPassThrough },

	{ IRP_MJ_VOLUME_MOUNT,
	  0,
	  PtPreOperationPassThrough,
	  PtPostOperationPassThrough },

	{ IRP_MJ_VOLUME_DISMOUNT,
	  0,
	  PtPreOperationPassThrough,
	  PtPostOperationPassThrough },

	{ IRP_MJ_OPERATION_END }
};

//
//  This defines what we want to filter with FltMgr
//

CONST FLT_REGISTRATION FilterRegistration = {

	sizeof(FLT_REGISTRATION),         //  Size
	FLT_REGISTRATION_VERSION,           //  Version
	0,                                  //  Flags

	NULL,                               //  Context
	Callbacks,                          //  Operation callbacks

	PtUnload,                           //  MiniFilterUnload

	PtInstanceSetup,                    //  InstanceSetup
	PtInstanceQueryTeardown,            //  InstanceQueryTeardown
	PtInstanceTeardownStart,            //  InstanceTeardownStart
	PtInstanceTeardownComplete,         //  InstanceTeardownComplete

	NULL,                               //  GenerateFileName
	NULL,                               //  GenerateDestinationFileName
	NULL                                //  NormalizeNameComponent

};



NTSTATUS
PtInstanceSetup(
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ FLT_INSTANCE_SETUP_FLAGS Flags,
	_In_ DEVICE_TYPE VolumeDeviceType,
	_In_ FLT_FILESYSTEM_TYPE VolumeFilesystemType
)
/*++

Routine Description:

	This routine is called whenever a new instance is created on a volume. This
	gives us a chance to decide if we need to attach to this volume or not.

	If this routine is not defined in the registration structure, automatic
	instances are alwasys created.

Arguments:

	FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
		opaque handles to this filter, instance and its associated volume.

	Flags - Flags describing the reason for this attach request.

Return Value:

	STATUS_SUCCESS - attach
	STATUS_FLT_DO_NOT_ATTACH - do not attach

--*/
{
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(Flags);
	UNREFERENCED_PARAMETER(VolumeDeviceType);
	UNREFERENCED_PARAMETER(VolumeFilesystemType);

	PAGED_CODE();

	PT_DBG_PRINT(PTDBG_TRACE_ROUTINES,
		("PassThrough!PtInstanceSetup: Entered\n"));

	return STATUS_SUCCESS;
}


NTSTATUS
PtInstanceQueryTeardown(
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags
)
/*++

Routine Description:

	This is called when an instance is being manually deleted by a
	call to FltDetachVolume or FilterDetach thereby giving us a
	chance to fail that detach request.

	If this routine is not defined in the registration structure, explicit
	detach requests via FltDetachVolume or FilterDetach will always be
	failed.

Arguments:

	FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
		opaque handles to this filter, instance and its associated volume.

	Flags - Indicating where this detach request came from.

Return Value:

	Returns the status of this operation.

--*/
{
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(Flags);

	PAGED_CODE();

	PT_DBG_PRINT(PTDBG_TRACE_ROUTINES,
		("PassThrough!PtInstanceQueryTeardown: Entered\n"));

	return STATUS_SUCCESS;
}


VOID
PtInstanceTeardownStart(
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags
)
/*++

Routine Description:

	This routine is called at the start of instance teardown.

Arguments:

	FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
		opaque handles to this filter, instance and its associated volume.

	Flags - Reason why this instance is been deleted.

Return Value:

	None.

--*/
{
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(Flags);

	PAGED_CODE();

	PT_DBG_PRINT(PTDBG_TRACE_ROUTINES,
		("PassThrough!PtInstanceTeardownStart: Entered\n"));
}


VOID
PtInstanceTeardownComplete(
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags
)
/*++

Routine Description:

	This routine is called at the end of instance teardown.

Arguments:

	FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
		opaque handles to this filter, instance and its associated volume.

	Flags - Reason why this instance is been deleted.

Return Value:

	None.

--*/
{
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(Flags);

	PAGED_CODE();

	PT_DBG_PRINT(PTDBG_TRACE_ROUTINES,
		("PassThrough!PtInstanceTeardownComplete: Entered\n"));
}


/*************************************************************************
	MiniFilter initialization and unload routines.
*************************************************************************/

NTSTATUS
DriverEntry(
	_In_ PDRIVER_OBJECT DriverObject,
	_In_ PUNICODE_STRING RegistryPath
)
/*++

Routine Description:

	This is the initialization routine for this miniFilter driver.  This
	registers with FltMgr and initializes all global data structures.

Arguments:

	DriverObject - Pointer to driver object created by the system to
		represent this driver.

	RegistryPath - Unicode string identifying where the parameters for this
		driver are located in the registry.

Return Value:

	Returns STATUS_SUCCESS.

--*/
{
	NTSTATUS status;

	UNREFERENCED_PARAMETER(RegistryPath);

	PT_DBG_PRINT(PTDBG_TRACE_ROUTINES,
		("PassThrough!DriverEntry: Entered\n"));
	//
	//  Register with FltMgr to tell it our callback routines
	//

	status = FltRegisterFilter(DriverObject,
		&FilterRegistration,
		&gFilterHandle);

	FLT_ASSERT(NT_SUCCESS(status));

	if (NT_SUCCESS(status)) {

		//
		//  Start filtering i/o
		//

		status = FltStartFiltering(gFilterHandle);

		if (!NT_SUCCESS(status)) {

			FltUnregisterFilter(gFilterHandle);
		}
	}

	return status;
}

NTSTATUS
PtUnload(
	_In_ FLT_FILTER_UNLOAD_FLAGS Flags
)
/*++

Routine Description:

	This is the unload routine for this miniFilter driver. This is called
	when the minifilter is about to be unloaded. We can fail this unload
	request if this is not a mandatory unloaded indicated by the Flags
	parameter.

Arguments:

	Flags - Indicating if this is a mandatory unload.

Return Value:

	Returns the final status of this operation.

--*/
{
	UNREFERENCED_PARAMETER(Flags);

	PAGED_CODE();

	PT_DBG_PRINT(PTDBG_TRACE_ROUTINES,
		("PassThrough!PtUnload: Entered\n"));

	FltUnregisterFilter(gFilterHandle);

	return STATUS_SUCCESS;
}


/*************************************************************************
	MiniFilter callback routines.
*************************************************************************/
FLT_PREOP_CALLBACK_STATUS
PtPreOperationPassThrough(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID* CompletionContext
)
/*++

Routine Description:

	This routine is the main pre-operation dispatch routine for this
	miniFilter. Since this is just a simple passThrough miniFilter it
	does not do anything with the callbackData but rather return
	FLT_PREOP_SUCCESS_WITH_CALLBACK thereby passing it down to the next
	miniFilter in the chain.

	This is non-pageable because it could be called on the paging path

Arguments:

	Data - Pointer to the filter callbackData that is passed to us.

	FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
		opaque handles to this filter, instance, its associated volume and
		file object.

	CompletionContext - The context for the completion routine for this
		operation.

Return Value:

	The return value is the status of the operation.

--*/
{
	NTSTATUS status;

	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(CompletionContext);

	PT_DBG_PRINT(PTDBG_TRACE_ROUTINES,
		("PassThrough!PtPreOperationPassThrough: Entered\n"));

	//
	//  See if this is an operation we would like the operation status
	//  for.  If so request it.
	//
	//  NOTE: most filters do NOT need to do this.  You only need to make
	//        this call if, for example, you need to know if the oplock was
	//        actually granted.
	//
	PFLT_FILE_NAME_INFORMATION NameInfo = NULL;
	status = FltGetFileNameInformation(
		Data,
		FLT_FILE_NAME_NORMALIZED |
		FLT_FILE_NAME_QUERY_DEFAULT,
		&NameInfo);
	UNICODE_STRING required_extension = RTL_CONSTANT_STRING(L".crypt_lab2");
	if (!NT_SUCCESS(status))
	{
	}
	else {
		// ������� 2022, ������
		PWSTR extensionStart = wcsrchr(NameInfo->Name.Buffer, L'.');
		if (extensionStart != NULL)
		{
			UNICODE_STRING extension;

			RtlInitUnicodeString(&extension, extensionStart);

			if (RtlEqualUnicodeString(&required_extension, &extension, FALSE)) {

				DbgPrint("");
				if (Data->Iopb->MajorFunction == IRP_MJ_WRITE) {
					// ����� - ���������� 
					// unsigned char* write_data[];
					unsigned char* hexarray[1024];
					memset(hexarray, 0, 1024);
					memcpy(hexarray, (unsigned char*)Data->Iopb->Parameters.Write.WriteBuffer, Data->Iopb->Parameters.Write.Length);

					// ����������
					struct AES_ctx ctx;
					AES_init_ctx_iv(&ctx, key, iv);
					AES_CBC_encrypt_buffer(&ctx, hexarray, 1024);

					// memcpy(hexarray, (unsigned char[]) { '1', '2', '3' }, sizeof((unsigned char[]) { '1', '2', '3' })); �������� ������ ��������

					DbgPrint("");
					memcpy((unsigned char*)Data->Iopb->Parameters.Write.WriteBuffer, hexarray, Data->Iopb->Parameters.Write.Length);
				}
				else if (Data->Iopb->MajorFunction == IRP_MJ_READ) { /*
					// ����� - �����������
					// unsigned char* read_data = (char*)Data->Iopb->Parameters.Read.ReadBuffer;
					unsigned char* hexarray[1024];
					memset(hexarray, 0, 1024);
					memcpy(hexarray, (unsigned char*)Data->Iopb->Parameters.Read.ReadBuffer, strlen(Data->Iopb->Parameters.Read.ReadBuffer));

					// ������������
					unsigned char key[] = { 0x04, 0x9a, 0x1d, 0x22, 0x0e, 0x28, 0xf0, 0x09, 0xf2, 0xa9, 0xfd, 0xdc, 0x5a, 0x0b, 0x7a, 0x42, 0x1c,
					0xb5, 0xb6, 0x42, 0xe2, 0xfb, 0x46, 0x4a, 0xd3, 0x86, 0x20, 0xa1, 0xc6, 0x20, 0x74, 0xc6 };
					unsigned char iv[] = { 0x96, 0x38, 0x72, 0x5e, 0x2b, 0xfc, 0x8a, 0x49, 0xf0, 0x46, 0xc9, 0x85, 0x71, 0xf5, 0xae, 0x2b };
					struct AES_ctx ctx;
					AES_init_ctx_iv(&ctx, key, iv);
					AES_CBC_decrypt_buffer(&ctx, hexarray, 64);

					DbgPrint("");
					memcpy((unsigned char*)Data->Iopb->Parameters.Read.ReadBuffer, hexarray, strlen(Data->Iopb->Parameters.Read.ReadBuffer));
				*/} 
			}
		}
	}

	if (PtDoRequestOperationStatus(Data)) {

		status = FltRequestOperationStatusCallback(Data,
			PtOperationStatusCallback,
			(PVOID)(++OperationStatusCtx));
		if (!NT_SUCCESS(status)) {

			PT_DBG_PRINT(PTDBG_TRACE_OPERATION_STATUS,
				("PassThrough!PtPreOperationPassThrough: FltRequestOperationStatusCallback Failed, status=%08x\n",
					status));
		}
	}

	return FLT_PREOP_SUCCESS_WITH_CALLBACK;
}



VOID
PtOperationStatusCallback(
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ PFLT_IO_PARAMETER_BLOCK ParameterSnapshot,
	_In_ NTSTATUS OperationStatus,
	_In_ PVOID RequesterContext
)
/*++

Routine Description:

	This routine is called when the given operation returns from the call
	to IoCallDriver.  This is useful for operations where STATUS_PENDING
	means the operation was successfully queued.  This is useful for OpLocks
	and directory change notification operations.

	This callback is called in the context of the originating thread and will
	never be called at DPC level.  The file object has been correctly
	referenced so that you can access it.  It will be automatically
	dereferenced upon return.

	This is non-pageable because it could be called on the paging path

Arguments:

	FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
		opaque handles to this filter, instance, its associated volume and
		file object.

	RequesterContext - The context for the completion routine for this
		operation.

	OperationStatus -

Return Value:

	The return value is the status of the operation.

--*/
{
	UNREFERENCED_PARAMETER(FltObjects);

	PT_DBG_PRINT(PTDBG_TRACE_ROUTINES,
		("PassThrough!PtOperationStatusCallback: Entered\n"));

	PT_DBG_PRINT(PTDBG_TRACE_OPERATION_STATUS,
		("PassThrough!PtOperationStatusCallback: Status=%08x ctx=%p IrpMj=%02x.%02x \"%s\"\n",
			OperationStatus,
			RequesterContext,
			ParameterSnapshot->MajorFunction,
			ParameterSnapshot->MinorFunction,
			FltGetIrpName(ParameterSnapshot->MajorFunction)));
}


FLT_POSTOP_CALLBACK_STATUS
PtPostOperationPassThrough(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_opt_ PVOID CompletionContext,
	_In_ FLT_POST_OPERATION_FLAGS Flags
)
/*++

Routine Description:

	This routine is the post-operation completion routine for this
	miniFilter.

	This is non-pageable because it may be called at DPC level.

Arguments:

	Data - Pointer to the filter callbackData that is passed to us.

	FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
		opaque handles to this filter, instance, its associated volume and
		file object.

	CompletionContext - The completion context set in the pre-operation routine.

	Flags - Denotes whether the completion is successful or is being drained.

Return Value:

	The return value is the status of the operation.

--*/
{
	UNREFERENCED_PARAMETER(Data);
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(CompletionContext);
	UNREFERENCED_PARAMETER(Flags);

	PT_DBG_PRINT(PTDBG_TRACE_ROUTINES,
		("PassThrough!PtPostOperationPassThrough: Entered\n"));
	
	NTSTATUS status;
	PFLT_FILE_NAME_INFORMATION NameInfo = NULL;
	status = FltGetFileNameInformation(
		Data,
		FLT_FILE_NAME_NORMALIZED |
		FLT_FILE_NAME_QUERY_DEFAULT,
		&NameInfo);
	UNICODE_STRING required_extension = RTL_CONSTANT_STRING(L".crypt_lab2");
	if (!NT_SUCCESS(status))
	{
	}
	else {
		// ������� 2022, ������
		PWSTR extensionStart = wcsrchr(NameInfo->Name.Buffer, L'.');
		if (extensionStart != NULL)
		{
			UNICODE_STRING extension;

			RtlInitUnicodeString(&extension, extensionStart);

			if (RtlEqualUnicodeString(&required_extension, &extension, FALSE)) {

				DbgPrint("");
				if (Data->Iopb->MajorFunction == IRP_MJ_WRITE) 
				{
				}
				else if (Data->Iopb->MajorFunction == IRP_MJ_READ) {
					// ����� - �����������
					// unsigned char* read_data = (char*)Data->Iopb->Parameters.Read.ReadBuffer;
					unsigned char* hexarray[1024];
					memset(hexarray, 0, 1024);
					memcpy(hexarray, (unsigned char*)Data->Iopb->Parameters.Read.ReadBuffer, Data->Iopb->Parameters.Read.Length);

					// ������������
					struct AES_ctx ctx;
					AES_init_ctx_iv(&ctx, key, iv);
					AES_CBC_decrypt_buffer(&ctx, hexarray, 1024);

					DbgPrint("");
					memcpy((unsigned char*)Data->Iopb->Parameters.Read.ReadBuffer, hexarray, Data->Iopb->Parameters.Read.Length);
				}
			}
		}
	}

	return FLT_POSTOP_FINISHED_PROCESSING;
}


FLT_PREOP_CALLBACK_STATUS
PtPreOperationNoPostOperationPassThrough(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID* CompletionContext
)
/*++

Routine Description:

	This routine is the main pre-operation dispatch routine for this
	miniFilter. Since this is just a simple passThrough miniFilter it
	does not do anything with the callbackData but rather return
	FLT_PREOP_SUCCESS_WITH_CALLBACK thereby passing it down to the next
	miniFilter in the chain.

	This is non-pageable because it could be called on the paging path

Arguments:

	Data - Pointer to the filter callbackData that is passed to us.

	FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
		opaque handles to this filter, instance, its associated volume and
		file object.

	CompletionContext - The context for the completion routine for this
		operation.

Return Value:

	The return value is the status of the operation.

--*/
{
	UNREFERENCED_PARAMETER(Data);
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(CompletionContext);

	PT_DBG_PRINT(PTDBG_TRACE_ROUTINES,
		("PassThrough!PtPreOperationNoPostOperationPassThrough: Entered\n"));

	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}


BOOLEAN
PtDoRequestOperationStatus(
	_In_ PFLT_CALLBACK_DATA Data
)
/*++

Routine Description:

	This identifies those operations we want the operation status for.  These
	are typically operations that return STATUS_PENDING as a normal completion
	status.

Arguments:

Return Value:

	TRUE - If we want the operation status
	FALSE - If we don't

--*/
{
	PFLT_IO_PARAMETER_BLOCK iopb = Data->Iopb;

	//
	//  return boolean state based on which operations we are interested in
	//

	return (BOOLEAN)

		//
		//  Check for oplock operations
		//

		(((iopb->MajorFunction == IRP_MJ_FILE_SYSTEM_CONTROL) &&
			((iopb->Parameters.FileSystemControl.Common.FsControlCode == FSCTL_REQUEST_FILTER_OPLOCK) ||
				(iopb->Parameters.FileSystemControl.Common.FsControlCode == FSCTL_REQUEST_BATCH_OPLOCK) ||
				(iopb->Parameters.FileSystemControl.Common.FsControlCode == FSCTL_REQUEST_OPLOCK_LEVEL_1) ||
				(iopb->Parameters.FileSystemControl.Common.FsControlCode == FSCTL_REQUEST_OPLOCK_LEVEL_2)))

			||

			//
			//    Check for directy change notification
			//

			((iopb->MajorFunction == IRP_MJ_DIRECTORY_CONTROL) &&
				(iopb->MinorFunction == IRP_MN_NOTIFY_CHANGE_DIRECTORY))
			);
}

