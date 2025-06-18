//WARNING: NEVER INCLUDE THIS FILE DIRECTLY EXCEPT IN THE CORRESPONDING .c FILE.
//<<START>>	IMPORTANT: FIRST INCLUDE THE CORRESPONDING ".h" FILE
//			INCLUDE ".h" OF C NON CRXed CODE THAT THIS HEADER NEEDS.
//			INCLUDE ".h", NOT ".c.h", OF C CRXed CODE.
#include "CrxOs/H/crx/c/os/fileSystem.h"
#include "CrxOs/H/crx/c/os/info.h"
//<<END>>

CRX__LIB__C_CODE_BEGIN()

//CLASS: Iterator

/*
	WARNING: KEEP SYNCHRONIZED WITH THE IMPLEMENTATION OF
			crx_c_os_fileSystem_private_posix_isStatOfFileLikelySameAsOneFromDirent() AND
			crx_c_os_fileSystem_private_win32_areWin32FindDataAndByHandleFileInformatinLikelyForSameEntry()
*/
CRX__LIB__PUBLIC_C_FUNCTION() bool crx_c_os_fileSystem_iterator_verifyRecordAgainst(
		Crx_C_Os_FileSystem_Iterator_Record * pRecord,
		Crx_C_Os_FileSystem_OsFileHandle pOsFileHandle)
{
	if(pOsFileHandle == CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE)
		{return false;}
	else
	{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD))
		Crx_NonCrxed_Os_Posix_Dll_Libc_Stat tStat /*= ?*/;
		
		if(crx_c_os_fileSystem_internal_posix_fstat(pOsFileHandle, &tStat))
			{return (pRecord->gPrivate_entry->gPrivate_dInoOrFileno == pStat->st_ino);}
		else
			{return false;}
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
		BY_HANDLE_FILE_INFORMATION tByHandleFileInformation /*= ?*/;
	
		crx_c_os_fileSystem_internal_win32_getFileInformationByHandle(pOsFileHandle, 
				&tByHandleFileInformation);

		if((pRecord->gPrivate_entry->gPrivate_dwFileAttributes == 
						tByHandleFileInformation.dwFileAttributes) &&
				(pRecord->gPrivate_entry->gPrivate_ftLastWriteTime == 
						tByHandleFileInformation.ftLastWriteTime))
	{
		if(pRecord->gPrivate_entry->gPrivate_dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{return true;}
		else
		{
			return ((pRecord->gPrivate_entry->gPrivate_nFileSizeHigh == 
							tByHandleFileInformation.nFileSizeLow) &&
					(pRecord->gPrivate_entry->gPrivate_nFileSizeHigh == 
							tByHandleFileInformation.nFileSizeLow));
		}
	}
	else
		{return false;}
#endif
	}
}
#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
/*
	WARNING: KEEP SYNCHRONIZED WITH THE IMPLEMENTATION OF
			crx_c_os_fileSystem_private_win32_areWin32FindDataAndByHandleFileInformatinLikelyForSameEntry()
*/
CRX__LIB__PUBLIC_C_FUNCTION() bool crx_c_os_fileSystem_iterator_verifyRecordAgainst2(
		Crx_C_Os_FileSystem_Iterator_Record * pRecord,
		Crx_C_Os_FileSystem_Internal_Win32_Win32FindData const * pWin32FindData)
{
	return ((pRecord->gPrivate_dwFileAttributes == pWin32FindData->dwFileAttributes) &&
			(pRecord->gPrivate_ftLastWriteTime == pWin32FindData->ftLastAccessTime) &&
			((pWin32FindData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ||
					((pRecord->gPrivate_nFileSizeHigh == pWin32FindData->nFileSizeHigh) &&
					(pRecord->gPrivate_nFileSizeLow == pWin32FindData->nFileSizeLow))));
}
#endif

CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_iterator_construct(
		Crx_C_Os_FileSystem_Iterator * pThis,
		Crx_C_Os_FileSystem_Contract const * CRX_NOT_NULL pContract)
	{crx_c_os_fileSystem_iterator_internal_construct(pThis, pContract, false, false);}
/*
	IF pIsToForceScanForOccupiedSlots IS SET TO TRUE, THE RUNTIME FILE SYSTEM PRELIMINIARY 
			CHECKS REQUIRED BY THE CONTRACT WHEN THE RESOLUTION MODEL IS "PRECISE AND SLOT AWARE", 
			OR ONLY "SLOT AWARE", ARE DONE NO MATTER THE CONTRACT. HOWEVER, INVALID ENTRIES
			DISCOVERRED ARE ONLY SHOWN TO THE USER IF THEY DO NOT VIOLATE THE CONTRACT.
			
			REMEMBER, AN EXAMPLE OF INVALID ENTRIES WOULD BE, "/cat.txt" AND "/Cat.txt". REMEMBER,
			THIS IS NOT ABOUT CASING ONLY BUT ANY FILE NAMES THAT EVALUATE EQUAL BY 
			::crx::c::os::osString::fileSystem::compareStrings().
	IF pAreContractErrosFatal IS SET TO TRUE, THEN WHEN A CONTRACT ERROR IS ENCOUNTERED, THE
			ITERATION TREATS IT AS A FATAL ERROR, MEANING IT STOPS. NOTE THAT IF THE CONTRACT IS
			SUCH AS THE RUNTIME FILE SYSTEM PRELIMINIARY CHECKS WOULD NOT BE DONE, BUT BECAUSE
			pIsToForceScanForOccupiedSlots IS SET TO TRUE, THEY ARE DONE, THE NEHATIVE RESULTS
			OF THE SAID CHECK WOULD NOT CAUSE A FATAL ERROR, BECAUSE pIsToForceScanForOccupiedSlots
			DOES NOT CHANGE THE UNDERLYING CONTRACT.
*/
CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_iterator_internal_construct(
		Crx_C_Os_FileSystem_Iterator * pThis,
		Crx_C_Os_FileSystem_Contract const * CRX_NOT_NULL pContract, 
		bool pAreContractErrosFatal, bool pIsToForceScanForOccupiedSlots)
{
	pThis->gPrivate_isSet = false;
	pThis->gPrivate_isNoError = true;
	pThis->gPrivate_areContractualErrorsFatal = pAreContractErrosFatal;
	memcpy(&(pThis->gPrivate_contract), pContract, sizeof(Crx_C_Os_FileSystem_Contract));
	pThis->gPrivate_isToShowInvalidEntries = pIsToForceScanForOccupiedSlots &&
			!(((pThis->gPrivate_contract->gResolutionModel == 
			CRX__C__OS__FILE_SYSTEM__RESOLUTION_MODEL__PRECISE_AND_SLOT_AWARE) ||
			(pThis->gPrivate_contract->gResolutionModel == 
			CRX__C__OS__FILE_SYSTEM__RESOLUTION_MODEL__SLOT_AWARE)) &&
			(pThis->gPrivate_contract->gFileSystemRuntimeGuaranteeLevel == 2));
	pThis->gPrivate_depthLimitOfReparsePoints = 0;
	crx_c_string_construct(&(pThis->gPrivate_directoryFullPath));
	crx_c_string_construct(&(pThis->gPrivate_internalDirectoryFullPath));
	pThis->gPrivate_byteSizeOfPathPrefixInDirectoryFullPath = 0;
	pThis->gPrivate_byteSizeOfPathPrefixInInternalDirectoryFullPath = 0;
	pThis->gPrivate_lengthResidueWhenDirectory = 0;
	pThis->gPrivate_lengthResidue = 0;
	pThis->gPrivate_getOrderOfObjects = NULL;
	crx_c_queue_construct(&(pThis->gPrivate_stackEntries),
			crx_c_os_fileSystem_iterator_private_stackEntry_getTypeBluePrint(), 6);
	/*pThis->gPrivate_osFileHandle = -1;
	pThis->gPrivate_osFileHandle__current = -1;
	pThis->gPrivate_dir.gDir = NULL;*/
}

CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_iterator_copyConstruct(
		Crx_C_Os_FileSystem_Iterator * pThis, 
		Crx_C_Os_FileSystem_Iterator * CRX_NOT_NULL pIterator)
	{exit();}

CRX__LIB__PUBLIC_C_FUNCTION() Crx_C_Os_FileSystem_Iterator * crx_c_os_fileSystem_iterator_new()	
{
	Crx_C_Os_FileSystem_Iterator * vReturn = 
			((Crx_C_Os_FileSystem_Iterator *)malloc(sizeof(Crx_C_Os_FileSystem_Iterator)));

	if(vReturn != NULL)
		{crx_c_os_fileSystem_iterator_construct(vReturn);}

	return vReturn;
}
CRX__LIB__PUBLIC_C_FUNCTION() Crx_C_Os_FileSystem_Iterator * crx_c_os_fileSystem_iterator_copyNew(
		Crx_C_Os_FileSystem_Iterator const * CRX_NOT_NULL pIterator)
	{exit();}
CRX__LIB__PUBLIC_C_FUNCTION() Crx_C_Os_FileSystem_Iterator * crx_c_os_fileSystem_iterator_moveNew(
		Crx_C_Os_FileSystem_Iterator * CRX_NOT_NULL pIterator)
{
	Crx_C_Os_FileSystem_Iterator * vReturn = 
			((Crx_C_Os_FileSystem_Iterator *)malloc(sizeof(Crx_C_Os_FileSystem_Iterator)));

	if(vReturn != NULL)
		{memcpy(vReturn, pIterator, sizeof(Crx_C_Os_FileSystem_Iterator));}

	return vReturn;
}

CRX__C__TYPE_BLUE_PRINT__DEFINE_GET_BLUE_PRINT(
		Crx_C_Os_FileSystem_Iterator, crx_c_os_fileSystem_iterator_,
		CRXM__TRUE, CRXM__TRUE,
		CRXM__FALSE, CRXM__TRUE,
		CRXM__FALSE, CRXM__FALSE)

CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_iterator_destruct(
		Crx_C_Os_FileSystem_Iterator * pThis)
{
	crx_c_string_destruct(&(pThis->gPrivate_directoryFullPath));
	crx_c_string_destruct(&(pThis->gPrivate_internalDirectoryFullPath));
	crx_c_queue_destruct(&(pThis->gPrivate_stackEntries));

	if(pThis->gPrivate_isSet)
		{Crx_NonCrxed_Os_Posix_Dll_Libc_Closedir(pThis->gPrivate_dir.gDir);}
}
CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_iterator_free(
		Crx_C_Os_FileSystem_Iterator * pThis)
	{free(pThis);}
	
CRX__LIB__PRIVATE_C_FUNCTION() int32_t crx_c_os_fileSystem_iterator_private_areEntriesEqual(
		void const * pEntry, void const * pEntry__2)
{
	return crx_c_string_isEqualTo(
			&(((Crx_C_Os_FileSystem_Iterator_Entry const * )pEntry)->gPrivate_name),
			&(((Crx_C_Os_FileSystem_Iterator_Entry const * )pEntry__2)->gPrivate_name));
}
CRX__LIB__PRIVATE_C_FUNCTION() size_t crx_c_os_fileSystem_iterator_private_getHashOfEntry(
		size_t pSeed, void const * CRX_NOT_NULL pEntry)
{
	return crx_c_string_getHash(pSeed, 
			&(((Crx_C_Os_FileSystem_Iterator_Entry const * )pEntry)->gPrivate_name));
}

CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_iterator_private_clear(
		Crx_C_Os_FileSystem_Iterator * pThis)
{
	pThis->gPrivate_isNoError = true;
	pThis->gPrivate_depthLimitOfReparsePoints = 0;
	crx_c_string_empty(&(pThis->gPrivate_directoryFullPath));
	crx_c_string_empty(&(pThis->gPrivate_internalDirectoryFullPath));
	pThis->gPrivate_byteSizeOfPathPrefixInDirectoryFullPath = 0;
	pThis->gPrivate_byteSizeOfPathPrefixInInternalDirectoryFullPath = 0;
	pThis->gPrivate_lengthResidueWhenDirectory = 0;
	pThis->gPrivate_lengthResidue = 0;
	pThis->gPrivate_getOrderOfObjects = NULL;
	crx_c_queue_empty(&(pThis->gPrivate_stackEntries));
}

CRX__LIB__PUBLIC_C_FUNCTION() bool crx_c_os_fileSystem_iterator_set(
		Crx_C_Os_FileSystem_Iterator * pThis, 
		Crx_C_String const * CRX_NOT_NULL pDirectoryFullPath, uint32_t pDepthLimitOfReparsePoints,
		Crx_C_TypeBluePrint_GetOrderOfObjects pFunc_getOrderOfObjects)
{
	Crx_C_String vVerifiedPathPrefix /*= ?*/;
	Crx_C_String vVerifiedRoute /*= ?*/;
	bool vIsNoError = true;

	crx_c_string_construct(&vVerifiedPathPrefix);
	crx_c_string_construct(&vVerifiedRoute);
	
	if(pThis->gPrivate_isSet)
	{
		crx_c_os_fileSystem_iterator_private_clear(pThis);
		pThis->gPrivate_isSet = false;
	}
	
	if(crx_c_os_fileSystem_verifyFileSystemPath(
			&(pThis->gPrivate_contract), CRX__C__OS__FILE_SYSTEM__OPERATION__READ
			pDirectoryFullPath, pDepthLimitOfReparsePoints, false,
			&vVerifiedPathPrefix, &vVerifiedRoute,
			&(pThis->gPrivate_directoryFullPath), &(pThis->gPrivate_lengthResidueWhenDirectory), 
			&(pThis->gPrivate_lengthResidue), NULL, NULL))
	{
		pThis->gPrivate_depthLimitOfReparsePoints = pDepthLimitOfReparsePoints;
		
		if(vIsNoError && !crx_c_string_isEqualTo2(&vVerifiedRoute, 
				CRX__OS__FILE_SYSTEM__SEPERATOR_STRING, false))
		{
			crx_c_os_fileSystem_enforcePathAsDirectoryPath(&vVerifiedRoute, true);

			if(!crx_c_string_appendString(&(pThis->gPrivate_directoryFullPath), 
					&vVerifiedRoute))
				{vIsNoError = false;}
		}

		pThis->gPrivate_byteSizeOfPathPrefixInDirectoryFullPath = 
				crx_c_string_getSize(&(pThis->gPrivate_directoryFullPath));
		pThis->gPrivate_byteSizeOfPathPrefixInInternalDirectoryFullPath = 
				crx_c_string_getSize(&vVerifiedRoute);		
		pThis->gPrivate_getOrderOfObjects = pFunc_getOrderOfObjects;

		if(vIsNoError)
		{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
			Crx_C_Os_FileSystem_Internal_Posix_FileHandle tFileHandle /*= ?*/;
			Crx_C_Os_FileSystem_Existance tExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__NULL;
			
			crx_c_os_fileSystem_internal_posix_fileHandle_construct(&tFileHandle);
			
			if(crx_c_os_fileSystem_internal_posix_verifyFileSystemPathToOs2AndOpen(&tFileHandle,
					&(pThis->gPrivate_internalDirectoryFullPath), &(pThis->gPrivate_contract), 
					CRX__C__OS__FILE_SYSTEM__OPERATION__READ, pDepthLimitOfReparsePoints, 
					&tExistance, &vVerifiedPathPrefix, &vVerifiedRoute, 
					O_RDONLY /*| O_DIRECTORY DOES NOT EXIST ON MACOS 10.7*/, 0))
			{
				if(tExistance == CRX__C__OS__FILE_SYSTEM__EXISTANCE__EXISTS)
				{
					Crx_C_String tDirectoryPartialPath /*= ?*/;

					crx_c_string_construct(&tDirectoryPartialPath);

					crx_c_string_appendCString(&tDirectoryPartialPath, 
							CRX__OS__FILE_SYSTEM__SEPERATOR_STRING);
					vIsNoError = crx_c_os_fileSystem_iterator_private_pushStackEntry(pThis,
							&tDirectoryPartialPath, pThis->gPrivate_lengthResidueWhenDirectory,
							pThis->gPrivate_lengthResidue, NULL);

					crx_c_string_destruct(&tDirectoryPartialPath);
				}
				else
					{vIsNoError = false;}
			}
			else
				{vIsNoError = false;}
			
			crx_c_os_fileSystem_internal_posix_close(&tFileHandle);

			crx_c_os_fileSystem_internal_posix_fileHandle_destruct(&tFileHandle);
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
			HANDLE tHandle;

			if(crx_c_os_fileSystem_verifyFileSystemPath2(&(pThis->gPrivate_contract), 
					CRX__C__OS__FILE_SYSTEM__OPERATION__READ, NULL, pDepthLimitOfReparsePoints, 
					&tExistance, &vVerifiedPathPrefix, &vVerifiedRoute, NULL))
			{
				if(tExistance == CRX__C__OS__FILE_SYSTEM__EXISTANCE__EXISTS)
				{
					if(!(crx_c_string_appendString(&(pThis->gPrivate_internalDirectoryFullPath), 
							&vVerifiedPathPrefix) && (crx_c_string_isEqualTo2(&vVerifiedRoute, 
							CRX__OS__FILE_SYSTEM__SEPERATOR_STRING, false) ? true :
							crx_c_string_appendString(
							&(pThis->gPrivate_internalDirectoryFullPath), &vVerifiedRoute))))
						{vIsNoError = false;}

					if(vIsNoError) 
					{
						Crx_C_String tDirectoryPartialPath /*= ?*/;

						crx_c_string_construct(&tDirectoryPartialPath);

						crx_c_string_appendCString(&tDirectoryPartialPath, 
								CRX__OS__FILE_SYSTEM__SEPERATOR_STRING);
						vIsNoError = crx_c_os_fileSystem_iterator_private_pushStackEntry(pThis,
								&tDirectoryPartialPath, pThis->gPrivate_lengthResidueWhenDirectory,
								pThis->gPrivate_lengthResidue, NULL);

						crx_c_string_destruct(&tDirectoryPartialPath);
					}
				}
				else
					{vIsNoError = false;}
			}
			else
				{vIsNoError = false;}
#endif
		}

		if(vIsNoError)
			{pThis->gPrivate_isSet = true;}
		else
		{
			crx_c_os_fileSystem_iterator_private_clear(pThis);
			pThis->gPrivate_isNoError = false;
		}
	}
	
	crx_c_string_destruct(&vVerifiedPathPrefix);
	crx_c_string_destruct(&vVerifiedRoute);

	return vIsNoError;
}

/*
	NOTE: VERIFICATION WITH pDirectoryName MUST BE DONE BEFORE CALLING THIS. HENCE 
			pLengthResidueWhenDirectory AND pLengthResidue ARE THE ALREADY UPDATED VALUES.
		
	RETURNS:
		-2: ERROR: SERIOUS ERROR. A FATAL ERROR THAT SHOULD END ALL ENUMERATION.
		-1: ERROR: ACCESS DENIED. ANY ERROR SUCH AS THE USER IS NOT ABLE TO SEE WHAT HE WOULD 
				OTHERWISE SHOULD BE ABLE TO SEE GIVEN THE CONTRACTS THAT HE PASSED.
		 0: NOTHING TO REPORT
		 1: FINISHED
*/
CRX__LIB__PRIVATE_C_FUNCTION() int32_t crx_c_os_fileSystem_iterator_private_pushStackEntry(
		Crx_C_Os_FileSystem_Iterator * pThis, 
		Crx_C_String const * CRX_NOT_NULL pParentDirectoryPartialPath,
		size_t pLengthResidueWhenDirectory, size_t pLengthResidue,
		char const * pDirectoryName)
{
	bool vIsNoError = true;
	Crx_C_Os_FileSystem_Iterator_Private_StackEntry tStackEntry;
	Crx_C_Os_FileSystem_Iterator_Private_StackEntry * tStackEntry_previous = 
			((Crx_C_Os_FileSystem_Iterator_Private_StackEntry *)crx_c_queue_get(
			&(pThis->gPrivate_stackEntries)));
	Crx_C_String vInternalDirectoryFullPath /*= ?*/;
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	Crx_C_String vRemainingPath /*= ?*/;
	int32_t vErrorCode = 0;
#endif
	int32_t vReturn = 0;

	crx_c_os_fileSystem_iterator_private_stackEntry_construct(&tStackEntry);
	crx_c_string_construct(&vInternalDirectoryFullPath);
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	crx_c_string_construct(&vRemainingPath);
#endif	

	if(crx_c_queue_push(&(pThis->gPrivate_stackEntries), &tStackEntry))
	{
		Crx_C_Os_FileSystem_Iterator_Private_StackEntry * tStackEntry2 = 
				crx_c_queue_get(&(pThis->gPrivate_stackEntries));
		tIsNoError = true;

		tStackEntry2->gMode = 1;
		tStackEntry2->gCurrentStep = 1;
		tStackEntry2->gLengthResidueWhenDirectory = pLengthResidueWhenDirectory;
		tStackEntry2->gLengthResidue = pLengthResidue;
		if(!crx_c_string_isEqualTo2(pParentDirectoryPartialPath, 
						CRX__OS__FILE_SYSTEM__SEPERATOR_STRING) &&
				!crx_c_string_appendString(&(tStackEntry2->gDirectoryPartialPath), 
						pParentDirectoryPartialPath))
			{tIsNoError = false;}
		if(tIsNoError && ((((pThis->gPrivate_contract->gResolutionModel == 
				CRX__C__OS__FILE_SYSTEM__RESOLUTION_MODEL__PRECISE_AND_SLOT_AWARE) ||
				(pThis->gPrivate_contract->gResolutionModel == 
				CRX__C__OS__FILE_SYSTEM__RESOLUTION_MODEL__SLOT_AWARE)) &&
				(pThis->gPrivate_contract->gFileSystemRuntimeGuaranteeLevel == 2)) ||
				pThis->gPrivate_isToShowInvalidEntries))
		{
			tStackEntry2->gAllEntries = crx_c_orderedHashTable_new(
					crx_c_string_getTypeBluePrint(),
					crx_c_os_fileSystem_iterator_entry_getTypeBluePrint(),
					crx_c_os_fileSystem_iterator_private_areEntriesEqual,
					crx_c_os_fileSystem_iterator_private_getHashOfEntry);

			tIsNoError = (tStackEntry2->gAllEntries != NULL);
		}
		if(tIsNoError)
		{
			if(pDirectoryName != NULL))
			{
				if(!crx_c_string_appendCString(&(tStackEntry2->gDirectoryPartialPath), 
						pDirectoryName))
					{tIsNoError = false;}
				if(tIsNoError && !crx_c_string_appendCString(&(tStackEntry2->gDirectoryPartialPath), 
						CRX__OS__FILE_SYSTEM__SEPERATOR_STRING))
					{tIsNoError = false;}
			}
			else
			{
				if(!crx_c_string_appendCString(&(tStackEntry2->gDirectoryPartialPath), 
						CRX__OS__FILE_SYSTEM__SEPERATOR_STRING))
					{tIsNoError = false;}
			}
			
			if(tIsNoError && !crx_c_string_appendString(&vInternalDirectoryFullPath,
					&(pThis->gPrivate_internalDirectoryFullPath)))
				{tIsNoError = false;}
			if(tIsNoError && 
					!crx_c_string_isEqualTo2(&(tStackEntry2->gDirectoryPartialPath),
							CRX__OS__FILE_SYSTEM__SEPERATOR_STRING)
					!crx_c_string_appendString(&vInternalDirectoryFullPath,
							&(tStackEntry2->gDirectoryPartialPath)))
				{tIsNoError = false;}
		}

		if(!tIsNoError)
		{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
			vErrorCode = ENOMEM;
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
			vErrorCode = ERROR_NOT_ENOUGH_MEMORY;
#endif
			vReturn = -2;
		}

		/*
			IF WE HAPPEN TO RECURSE FIRST BECAUSE OF THE RESOLUTION MODEL, ON LINUX NOT CALLING 
			stat() AND CACHING THE RESULT AND WAITING THE SECOND TIME WE ITERATE TO CALL stat()
			SHOULD NOT BE A PROBLEM IN CASES WHERE THERE CAN BE AMBIGUITIY SUCH AS A FILE "cat.txt"
			AND "Cat.txt" ON A CASE INSENSITIVE SYSTEM, AS SYSTEM IN DEFINED IN THE DESIGN, BECAUSE
			WE GENERALLY ALWAYS ITERATE WITH REFERENCE TO THE HANDLE OF THE PARENT DIRECTORY.
			ON OLDER OPERATING SYSTEMS, AND IN PARTICULAR OLDER MACOS, THIS IS NOT TRUE. IN ANY 
			CASE, ON WINDOWS, WE CAN NOT DO THE SAME, WHETHER OLDER VERSION OF WINDOWS, OR NOT,
			BECAUSE ON WINDOWS TO GET THE INFORMATION WE GET FROM THE SYSTEM ITERATORS BUT NOT BY
			THEM, WE HAVE TO CALL GetFileInformationByHandle() AND THAT REQUIRES OPENING WHICH
			REQUIRES PASSING THE ENTIRE PATH AGAIN. THIS SECOND TIME, IF THERE IS AN AMBIGUITY,
			OR SLOT OCCUPIED SITUATION, WE MIGHT END UP READING THE WRONG ENTITY COMPARED TO WHEN
			WE DID OUR FIRST ITERATION WHICH HAPPENS IN THE CODE BELOW.
			
			REMEMBER THE ABIGUITY CAN NOT ACTUALLY HAPPEN ON THE CURRENT LEVEL BECAUSE THE CODE 
			BELOW WOULD ALREADY SOLVE THAT, BUT IT CAN HAPPEN BECAUSE AMBIGUITY HIGHER UP THE
			PATH, BUT LINUX THERE IS NOT ISSUE BECAUSE WE HAVE A HANDLE TO THE PARENT INSTEAD. AGAIN
			THIS IS STILL A PROBLEM ON OLDER POSIX OPERATING SYSTEMS WHERE WE DO NOT HAVE THE "at"
			FUNCTIONS.
		*/
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
		if(tIsNoError)
		{
			if(tStackEntry_previous == NULL)
			{
				if(crx_c_os_fileSystem_internal_posix_lookAt2(&(tStackEntry2->gFileHandle),
						&vInternalDirectoryFullPath, &vRemainingPath))
				{
					if(!crx_c_os_fileSystem_internal_posix_replaceOpenat(&(tStackEntry2->gFileHandle),
							crx_c_string_getCString(&vRemainingPath), 
							O_RDONLY /*| O_DIRECTORY DOES NOT EXIST ON MACOS 10.7*/, 0))
					{
						tErrorCode = errno;
						vIsNoError = false;
					}
				}
				else
				{
					tErrorCode = errno;
					vIsNoError = false;
				}
			}
			else
			{
				if(!crx_c_os_fileSystem_internal_posix_openat(&(tStackEntry2->gFileHandle),
						&(tStackEntry_previous->gFileHandle), pDirectoryName, 
						O_RDONLY /*| O_DIRECTORY DOES NOT EXIST ON MACOS 10.7*/, 0))
				{
					tErrorCode = errno;
					vIsNoError = false;
				}
			}
		}

		if(vIsNoError)
		{
			//MEANING WE NEED TO DO PRELMINIARY CHECKS PERTAINING TO FILESYSTEM PATH RESOLUTION
			if(tStackEntry2->gAllEntries != NULL)
			{
				if(crx_c_os_fileSystem_internal_posix_fdopendir(&(tStackEntry2->gDir), 
						&(tStackEntry2->gFileHandle)))
				{
					Crx_C_Os_FileSystem_Iterator_Entry tEntry /*= ?*/;
					Crx_NonCrxed_Os_Posix_Dll_Libc_Dirent * tDirent = 
							crx_c_os_fileSystem_internal_posix_readdir(tStackEntry2->gDir);
					Crx_C_String tNormalizedName /*= ?*/;
					Crx_C_String tTemp /*= ?*/;

					crx_c_os_fileSystem_iterator_entry_construct(&tEntry);
					crx_c_string_construct(&tNormalizedName);
					crx_c_string_construct(&tTemp);

					while((tDirent != NULL) && vIsNoError)
					{
						bool tIsNotToSkipCurrent = 
								((crx_c_fileSystem_strcmp(tDirent->d_name, '.') != 0) && 
								(crx_c_fileSystem_strcmp(tDirent->d_name, '..') != 0));

						if(tIsNotToSkipCurrent)
						{
							crx_c_string_empty(&tNormalizedName);
							crx_c_string_empty(&tTemp);

							if(vIsNoError && !crx_c_string_appendCString(&tNormalizedName, 
									tStackEntry2->gDirent->d_name))
								{vIsNoError = false;}
							if(vIsNoError && !crx_c_os_osString_fileSystem_normalizeToUnicodeNfd(
									&tTemp, &tNormalizedName, true))
								{vIsNoError = false;}
							crx_c_string_empty(&tNormalizedName);
							if(vIsNoError && 
									!crx_c_os_osString_fileSystem_getByteComparableStringFrom(
									&tNormalizedName, &tTemp, true))
								{vIsNoError = false;}
						
							if(vIsNoError)
							{
								if(crx_c_orderedHashTable_hasKey(tStackEntry2->gAllEntries,
										&tNormalizedName))
								{
									crx_c_os_fileSystem_iterator_entry_private_lowerFlags(&tEntry,
											CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY__FLAG__IS_VALID);
								}
								else
								{
									Crx_C_String * tNameOfEntry = 
											crx_c_os_fileSystem_iterator_entry_private_getName(
											&tEntry);

									crx_c_os_fileSystem_iterator_entry_private_raiseFlags(&tEntry,
											CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY__FLAG__IS_VALID);
									crx_c_string_empty(tNameOfEntry);
									vIsNoError = crx_c_string_appendCString(tNameOfEntry, 
											tStackEntry2->gDirent->d_name);
											
									crx_c_os_fileSystem_iterator_entry_private_setPosixData(&tEntry,
											tStackEntry2->gDirent);

									if(vIsNoError)
									{
										if(crx_c_orderedHashTable_tryMoveKeyAndElementAndSet(
												tStackEntry2->gAllEntries, &tNormalizedName, 
												&tEntry))
										{
											crx_c_os_fileSystem_iterator_entry_construct(&tEntry);
											crx_c_string_construct(&tNormalizedName);
										}
										else
											{vIsNoError = false;}
									}
								}
							}

							if(!vIsNoError)
							{
								//IF SO, WE HAVE A MEMORY ERROR. THE ABOVE CODE MUST GUARANTEE IT.
								vErrorCode = ENOMEM;
								vReturn = -2;
							}
							else
							{
								if(pThis->gPrivate_areContractualErrorsFatal &&
										!(tEntry.gPrivate_flags & 
												CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY__FLAG__IS_VALID))
								{
									vErrorCode = EINVAL;
									vReturn = -2;
								}
							}
						}
						
						if(vIsNoError)
						{
							tDirent = crx_c_os_fileSystem_internal_posix_readdir(
									tStackEntry2->gDir);
						}
					}
					
					if(vIsNoError && (errno != 0))
					{
						tErrorCode = errno;
						vIsNoError = false;
						
						/*crx_c_os_fileSystem_internal_posix_closedir(tStackEntry2->gDir);
						tStackEntry2->gDir = NULL;*/
					}
					/*else
						{rewinddir(tStackEntry2->gDir);}*/
					
					crx_c_os_fileSystem_internal_posix_closedir(tStackEntry2->gDir);
					tStackEntry2->gDir = NULL;

					crx_c_os_fileSystem_iterator_entry_destruct(&tEntry);
					crx_c_string_destruct(&tNormalizedName);
					crx_c_string_destruct(&tTemp);
				}
				else
				{
					tErrorCode = errno;
					vIsNoError = false;
				}
			}
			else
			{
				tStackEntry2->gMode = 2;

				if(!crx_c_os_fileSystem_internal_posix_fdopendir(&(tStackEntry2->gDir),
						&(tStackEntry2->gFileHandle)))
				{
					tErrorCode = errno;
					vIsNoError = false;
				}
			}
			
			if(!vIsNoError)
				{crx_c_os_fileSystem_internal_posix_close(&(tStackEntry2->gFileHandle));}

			if(vIsNoError)
			{
				vReturn = crx_c_os_fileSystem_iterator_private_next(pThis, 1); 

				if(vReturn == -2)
				{
					tErrorCode = errno;
					vIsNoError = false;
				}
			}
		}

		if(!vIsNoError && (vReturn == 0))
		{
			/*
				ERRORS CONSIDERED:

				EACCES			EBADF			ELOOP			EMFILE
				ENAMETOOLONG	ENOENT			ENFILE			ENOTDIR
			*/
			if((tErrorCode == ELOOP) || (tErrorCode == ENOENT))
				{vReturn = 1;}
			else if(tErrorCode == EACCES)
				{vReturn = -1;}
			else
				{vReturn = -2;}
		}
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
		if(vIsNoError)
		{
			//MEANING WE NEED TO DO PRELMINIARY CHECKS PERTAINING TO FILESYSTEM PATH RESOLUTION
			if(tStackEntry2->gAllEntries != NULL)
			{
				if(!crx_c_string_appendCString(&vInternalDirectoryFullPath, "*"))
				{
					vIsNoError = false;
					vReturn = -2;
				}

				if(vIsNoError)
				{
					tStackEntry2->gHandle = crx_c_os_fileSystem_internal_win32_findFirstFile(false,
							&vInternalDirectoryFullPath, &(tStackEntry2->gWin32FindData));

					crx_c_string_removeChars(&vInternalDirectoryFullPath, crx_c_string_getSize(
							&vInternalDirectoryFullPath) - 1, 1);

					if(tStackEntry2->gHandle == INVALID_HANDLE_VALUE)
					{
						tErrorCode = GetLastError();
						vIsNoError = false;
					}
				}
				
				if(vIsNoError)
				{
					Crx_C_Os_FileSystem_Iterator_Entry tEntry /*= ?*/;
					Crx_C_String tNormalizedName /*= ?*/;
					Crx_C_String tTemp /*= ?*/;
					bool tIsToContinue = true;

					crx_c_os_fileSystem_iterator_entry_construct(&tEntry);
					crx_c_string_construct(&tNormalizedName);
					crx_c_string_construct(&tTemp);

					while(tIsToContinue && vIsNoError)
					{
						bool tIsNotToSkipCurrent = (!crx_c_string_isEqualTo2(
										&(tStackEntry2->gWin32FindData.cFileName), '.', false)  && 
								!crx_c_string_isEqualTo2(
										&(tStackEntry2->gWin32FindData.cFileName), '..', false));

						if(tIsNotToSkipCurrent)
						{
							crx_c_string_empty(&tNormalizedName);
							crx_c_string_empty(&tTemp);

							if(vIsNoError && !crx_c_string_appendCString(&tNormalizedName, 
									&(tStackEntry2->gWin32FindData.cFileName)))
								{vIsNoError = false;}
							if(vIsNoError && !crx_c_os_osString_fileSystem_normalizeToUnicodeNfd(
									&tTemp, &tNormalizedName, true))
								{vIsNoError = false;}
							crx_c_string_empty(&tNormalizedName);
							if(vIsNoError && 
									!crx_c_os_osString_fileSystem_getByteComparableStringFrom(
									&tNormalizedName, &tTemp, true))
								{vIsNoError = false;}
							
							if(vIsNoError)
							{
								if(crx_c_orderedHashTable_hasKey(tStackEntry2->gAllEntries,
										&tNormalizedName))
								{
									crx_c_os_fileSystem_iterator_entry_private_lowerFlags(&tEntry,
											CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY__FLAG__IS_VALID);
								}
								else
								{
									Crx_C_String * tNameOfEntry = 
											crx_c_os_fileSystem_iterator_entry_private_getName(
											&tEntry);

									crx_c_os_fileSystem_iterator_entry_private_raiseFlags(&tEntry,
											CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY__FLAG__IS_VALID);
									crx_c_string_empty(tNameOfEntry);
									vIsNoError = crx_c_string_appendCString(tNameOfEntry, 
											&(tStackEntry2->gWin32FindData.cFileName));

									crx_c_os_fileSystem_iterator_entry_private_setWin32Data(&tEntry,
											&(tStackEntry2->gWin32FindData));

									if(vIsNoError && 
											crx_c_orderedHashTable_tryMoveKeyAndElementAndSet(
											tStackEntry2->gAllEntries, &tNormalizedName, &tEntry))
									{
										crx_c_os_fileSystem_iterator_entry_construct(&tEntry);
										crx_c_string_construct(&tNormalizedName);
									}
									else
										{vIsNoError = false;}
								}
							}

							if(!vIsNoError)
							{
								//IF SO, WE HAVE A MEMORY ERROR. THE ABOVE CODE MUST GUARANTEE IT.
								vErrorCode = ERROR_NOT_ENOUGH_MEMORY;
								vReturn = -2;
							}
							else
							{
								if(pThis->gPrivate_areContractualErrorsFatal &&
										!(tEntry.gPrivate_flags &
												CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY__FLAG__IS_VALID))
								{
									vErrorCode = ERROR_BAD_PATHNAME;
									vReturn = -2;
								}
							}
						}
						
						if(vIsNoError)
						{
							if(!crx_c_os_fileSystem_internal_win32_findNextFile(
									tStackEntry2->gHandle, &(tStackEntry2->gWin32FindData)))
							{
								int32_t tErrorCode2 = GetLastError();
								
								if(tErrorCode2 != ERROR_NO_MORE_FILES)
								{
									vIsNoError = false;
									tErrorCode = tErrorCode2;
								}

								tIsToContinue = false;
							}
						}
						else
							{tIsToContinue = false;}
					}
					
					crx_c_os_fileSystem_internal_win32_findClose(tStackEntry2->gHandle);
					tStackEntry2->gHandle = INVALID_HANDLE_VALUE;
					
					crx_c_os_fileSystem_iterator_entry_destruct(&tEntry);
					crx_c_string_destruct(&tNormalizedName);
					crx_c_string_destruct(&tTemp);
				}
			}
		
			if(vIsNoError)
			{
				vReturn = crx_c_os_fileSystem_iterator_private_next(pThis, 1);

				if(vReturn == -2)
				{
					tErrorCode = GetLastError();
					vIsNoError = false;
				}
			}
		}

		if(!vIsNoError && (vReturn == 0))
		{
			/*
				ERROR_ACCESS_DENIED				ERROR_BAD_PATHNAME			
				ERROR_BUFFER_OVERFLOW			ERROR_FILE_NOT_FOUND
				ERROR_FILENAME_EXCED_RANGE		ERROR_INVALID_FUNCTION
				ERROR_INVALID_HANDLE			ERROR_LABEL_TOO_LONG		
				ERROR_NOT_READY					ERROR_PATH_NOT_FOUND		
				ERROR_TOO_MANY_OPEN_FILES
			*/
			if(tErrorCode == ERROR_FILE_NOT_FOUND)
				{vReturn = 1;}
			else if(tErrorCode == ERROR_ACCESS_DENIED)
				{vReturn = -1;}
			else
				{vReturn = -2;}
		}
#endif

		/*
			REMEMBER: COMING OUT OF THE DIRECTORY WOULD HAPPEN IN 
					crx_c_os_fileSystem_iterator_private_next(), NOT HERE. FURTHERMORE THIS MEANS
					THAT OUR CALL TO crx_c_os_fileSystem_iterator_private_next() MIGHT BRING US BACK
					UP THE DIRECTORY LEVEL OF OUR CALLER, WHO COULD BE 
					crx_c_os_fileSystem_iterator_private_next()
		*/
	}

	crx_c_os_fileSystem_iterator_private_stackEntry_destruct(&tStackEntry);
	crx_c_string_destruct(&vInternalDirectoryFullPath);
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	crx_c_string_destruct(&vRemainingPath);
#endif

	return vReturn;
}

CRX__LIB__PRIVATE_C_FUNCTION() void crx_c_os_fileSystem_iterator_next(
		Crx_C_Os_FileSystem_Iterator * pThis)
	{crx_c_os_fileSystem_iterator_private_next(pThis, 1);}
CRX__LIB__PRIVATE_C_FUNCTION() void crx_c_os_fileSystem_iterator_skipIfDirectoryThenNext(
		Crx_C_Os_FileSystem_Iterator * pThis)
	{crx_c_os_fileSystem_iterator_private_next(pThis, 2);}
CRX__LIB__PRIVATE_C_FUNCTION() void crx_c_os_fileSystem_iterator_queueThenNext(
		Crx_C_Os_FileSystem_Iterator * pThis)
	{crx_c_os_fileSystem_iterator_private_next(pThis, 3);}
CRX__LIB__PRIVATE_C_FUNCTION() void crx_c_os_fileSystem_iterator_nextThenAddress(
		Crx_C_Os_FileSystem_Iterator * pThis)
	{crx_c_os_fileSystem_iterator_private_next(pThis, 4);}
CRX__LIB__PRIVATE_C_FUNCTION() void crx_c_os_fileSystem_iterator_upThenNext(
		Crx_C_Os_FileSystem_Iterator * pThis)
	{crx_c_os_fileSystem_iterator_private_next(pThis, 5);}
/*
	pOperation: 1:next	2:skipThenNext		3:stackThenNext		4:NextThenAddress
			5: upThenNext
	
	Return is same as that of iterator::pushStackEntry() but -1 is never returned.
*/
CRX__LIB__PRIVATE_C_FUNCTION() int32_t crx_c_os_fileSystem_iterator_private_next(
		Crx_C_Os_FileSystem_Iterator * pThis, uint32_t pOperation)
{
	if(!(pThis->gPrivate_isSet))
		{return -2;}
	else 
	{
		int32_t tReturn = 0;

		if(crx_c_queue_getLength(&(pThis->gPrivate_stackEntries)) > 0)
		{
			Crx_C_Os_FileSystem_Iterator_Private_StackEntry * tStackEntry = 
					crx_c_queue_get(&(pThis->gPrivate_stackEntries));
			Crx_C_Os_FileSystem_Iterator_Entry * tEntry__current = NULL;
			int32_t tErrorCode = 0; //WARNING: IF THIS IS NOT 0, IT MUST BE A FATAL ERROR
			bool tIsFinished = false;
			
			if(tStackEntry->gMode == 2)
			{
				if(tStackEntry->gAllEntries != NULL)
				{
					tEntry__current = ((Crx_C_Os_FileSystem_Iterator_Entry const *)
							crx_c_orderedHashTable_getElementFromIndex(tStackEntry->gAllEntries, 
							tStackEntry->gAllEntries_index));
				}
				else
					{tEntry__current = &(tStackEntry->gEntry__current);}
			}
			else if(tStackEntry->gMode == 3)
			{
				if(pThis->gPrivate_getOrderOfObjects == NULL)
					{tEntry__current = crx_c_queue_getFront(&(pThis->gEntries1));}
				else
					{tEntry__current = crx_c_sortedQueue_getFront(&(pThis->gSortedEntries1));}
			}
			else if(tStackEntry->gMode == 4)
			{
				if(p2This->gPrivate_getOrderOfObjects == NULL)
					{tEntry__current = crx_c_queue_getFront(&(pThis->gEntries2));}
				else
					{tEntry__current = crx_c_sortedQueue_getFront(&(pThis->gSortedEntries2));}
			}

			/*
				REMEMBER: IF MODE IS 1, THE OPERATION CAN NOT BE BUT 1, AND THE CALLER IS
						NOT BUT iterator::pushStackEntry()
			*/
			if((tStackEntry->gMode == 2) || (tStackEntry->gMode == 3) || (tStackEntry->gMode == 4))
			{
				if(!(tEntry__current->gPrivate_flags & 
						(CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY__FLAG__BARRED_ENTRY |
						CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY__FLAG__MOVING_OUT)))
				{
					if(pOperation == 3)
					{
						bool tIsNoError = true;

						if(pThis->gPrivate_getOrderOfObjects == NULL)
						{
							Crx_C_Queue/*<Crx_C_Os_FileSystem_Iterator_Entry>*/ * tEntries = NULL;

							if((tStackEntry->gMode == 2) || (tStackEntry->gMode == 4))
							{
								if(tStackEntry->gEntries1 == NULL)
								{
									pThis->gEntries1 = crx_c_queue_new(
											crx_c_os_fileSystem_iterator_entry_getTypeBluePrint(), 16);
								}
								
								tEntries = pThis->gEntries1;
							}
							else //tStackEntry->gMode IS 3
							{
								if(tStackEntry->gEntries2 == NULL)
								{
									pThis->gEntries2 = crx_c_queue_new(
											crx_c_os_fileSystem_iterator_entry_getTypeBluePrint(), 16);
								}
								
								tEntries = pThis->gEntries2;
							}
							
							if(tEntries != NULL)
								{tIsNoError = crx_c_queue_push(tEntries, tEntry__current);}
							else
								{tIsNoError = false;}
						}
						else
						{
							Crx_C_SortedQueue/*<Crx_C_Os_FileSystem_Iterator_Entry>*/ * tSortedEntries = 
									NULL;

							if((tStackEntry->gMode == 2) || (tStackEntry->gMode == 4))
							{
								if(tStackEntry->gSortedEntries1 == NULL)
								{
									pThis->gSortedEntries1 = crx_c_sortedQueue_new(
											crx_c_os_fileSystem_iterator_entry_getTypeBluePrint(), 
											pThis->gPrivate_getOrderOfObjects, false, 16);
								}

								tSortedEntries = pThis->gSortedEntries1;
							}
							else //tStackEntry->gMode IS 3
							{
								if(tStackEntry->gSortedEntries2 == NULL)
								{
									pThis->gSortedEntries2 = crx_c_sortedQueue_new(
											crx_c_os_fileSystem_iterator_entry_getTypeBluePrint(), 
											pThis->gPrivate_getOrderOfObjects, false, 16);
								}
								
								tSortedEntries = pThis->gSortedEntries2;
							}
							
							if(tSortedEntries != NULL)
								{tIsNoError = crx_c_sortedQueue_push(tSortedEntries, tEntry__current);}
							else
								{tIsNoError = false;}
						}
						
						if(!tIsNoError)
						{
							//MEMORY ERROR. ABOVE CODE MUST GUARANTEE THIS
							tReturn = -2;
							tIsFinished = true;
						}
					}
					else if(pOperation == 4)
					{
						if((tEntry__current->gPrivate_entryType == 
								CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY_TYPE__DIRECTORY) /* &&
								!(tEntry__current->gPrivate_flags & 
								CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY__FLAG__BARRED_ENTRY) &&
								(tEntry__current != &(tStackEntry->gEntry__current))*/)
						{
							/*if(crx_c_os_fileSystem_iterator_entry_copyAssignFrom(
									&(tStackEntry->gEntry__current), tEntry__current))
								{tStackEntry->gAdvancingStep = 1;}
							else
							{
								tReturn = -2;
								tIsFinished = true;
							}*/
							/*
								ORIGINALLY I WANTED TO AVOID MODIFYING ENTRIES WHICH INCLUDES 
										SETTING THEIR FLAGS, BECAUSE THE USER MIGHT QUEUE THEM 
										AGAIN. AFTER DECIDING THAT NO OTHER THAN THE 'NEXT' 
										OPERATION IS ALLOWED ON A DIRECTORY ENTRY RETURNED AFTER 
										MOVING OUT OF IT, THIS WAS NO LONGER A CALL FOR WORRY.
							*/
							tStackEntry->gAdvancingStep = 1;
						}
					}

					/*if((pOperation == 2) || (pOperation == 3))
					{
						if((tStackEntry->gMode != 2) || (tStackEntry->gAllEntries == NULL))
						{
							crx_c_os_fileSystem_iterator_entry_private_setEntryType(
									&(tStackEntry->gEntry__current), 
									CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY_TYPE__NULL);
							crx_c_string_empty(crx_c_os_fileSystem_iterator_entry_private_getName(
												&(tStackEntry->gEntry__current)));
						}
					}*/
					
					if(!tIsFinished && ((pOperation == 1) || (pOperation == 4)) &&
							(tEntry__current->gPrivate_entryType == 
							CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY_TYPE__DIRECTORY) /*&&
							!(tEntry__current->gPrivate_flags & 
							CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY__FLAG__BARRED_ENTRY)*/)
					{
						tReturn = crx_c_os_fileSystem_iterator_private_pushStackEntry(pThis, 
								&(tStackEntry->gDirectoryPartialPath),
								crx_c_os_fileSystem_iterator_entry_private_getLengthResidueWhenDirectory(
										tEntry__current),
								crx_c_os_fileSystem_iterator_entry_private_getLengthResidue(
										tEntry__current),
								crx_c_string_getCString(&(tEntry__current->gPrivate_name)));

						if((tReturn == 0) || (tReturn == -2))
							{tIsFinished = true;}
						else
						{
							if(tReturn == -1)
							{
								crx_c_os_fileSystem_iterator_entry_private_raiseFlags(
										tEntry__current,
										CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY__FLAG__BARRED_ENTRY |
										CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY__FLAG__MOVING_OUT);
								tStackEntry->gAdvancingStep = 2;
								
								tIsFinished = true;
							}
							else
							{
								assert(tReturn == 1);
								assert((tStackEntry->gAdvancingStep == 0) || 
										(tStackEntry->gAdvancingStep == 1));

								if(tStackEntry->gAdvancingStep == 1)
								{
									tStackEntry->gAdvancingStep = 2;
									crx_c_os_fileSystem_iterator_entry_private_raiseFlags(
											&(tStackEntry->gEntry__current),
											CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY__FLAG__MOVING_OUT);
									tIsFinished = true;
								}
							}

							tReturn = 0;
						}
					}
				}
				else
				{
					if(pOperation == 5)
					{
						crx_c_queue_pop(&(pThis->gPrivate_stackEntries));

						tStackEntry = crx_c_queue_get(&(pThis->gPrivate_stackEntries));

						if(tStackEntry == NULL)
							{tIsFinished = true;}
						else if(tStackEntry->gAdvancingStep == 1)
						{
							tStackEntry->gAdvancingStep = 2;
							crx_c_os_fileSystem_iterator_entry_private_raiseFlags(
									&(tStackEntry->gEntry__current),
									CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY__FLAG__MOVING_OUT |
									CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY__FLAG__FORCED_MOVE_OUT);
							tIsFinished = true;
						}
					}
					else
					{
						if(tStackEntry->gAdvancingStep == 2)
							{tStackEntry->gAdvancingStep = 0;}
					}
				}
			}

			if(!tIsFinished)
			{
				/*
					WHEN WE GO INTO A DIRECTORY, WE PUSH INTO THE PROGRAM STACK, MEANING, WE MAKE AN 
					EVENTUAL RECURSIVE CALL TO THIS FUNCTION, WHICH HAPPENS VIA 
					crx_c_os_fileSystem_iterator_private_pushStackEntry(). WHEN WE GO OUT OF A 
					DIRECTORY, UNLESS IT WAS EMPTY, WE DO NOT POP THE PROGRAM STACK, EVEN IF OUR CALLER 
					WAS crx_c_os_fileSystem_iterator_private_pushStackEntry(). IN OTHER WORDS, WHILE ON 
					THE SAME DIRECTORY LEVEL, THE MODE CAN NOT GO BACK TO 1 OR 2, AFTER GOING PAST 2,
					THIS CAN STILL HAPPEN IF WE GO BACK UP FROM A DIRECTORY.
				*/
				while(!tIsFinished)
				{
					Crx_C_Os_FileSystem_Iterator_Entry * tEntry = NULL;

					if((tStackEntry->gMode == 1) || (tStackEntry->gMode == 2))
					{
						bool tIsToContinue = true;

						while(tIsToContinue)
						{
							char const * tName = NULL;
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
							Crx_NonCrxed_Os_Posix_Dll_Libc_Stat tStat /*= ?*/;
#endif

							tIsToContinue = false;

							if(tStackEntry->gAllEntries != NULL)
							{
								if(tStackEntry->gMode == 1)
								{
									tStackEntry->gAllEntries_index = 
											crx_c_orderedHashTable_getStartIndex(
											tStackEntry->gAllEntries);
									tStackEntry->gMode = 2;
								}
								else
								{
									tStackEntry->gAllEntries_index = crx_c_orderedHashTable_getNextIndex(
											tStackEntry->gAllEntries);
								}

								while(tStackEntry->gAllEntries_index != 
										crx_c_orderedHashTable_getEndIndex(tStackEntry->gAllEntries))
								{
									tEntry = ((Crx_C_Os_FileSystem_Iterator_Entry *)
											crx_c_orderedHashTable_getElementFromIndex(
											tStackEntry->gAllEntries, tStackEntry->gAllEntries_index));

									if((tEntry->gPrivate_flags &
											CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY__FLAG__IS_VALID) || 
											pThis->gPrivate_isToShowInvalidEntries)
									{
										tName = crx_c_string_getCString(&(tEntry->gPrivate_name));
										tIsToContinue = true;
										
										break;
									}
									else
									{
										tStackEntry->gAllEntries_index = 
												crx_c_orderedHashTable_getNextIndex(
												tStackEntry->gAllEntries);
									}
								}
#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
								//ON POSIX, tErrorCode WOULD HAVE TO BE SET TO 0, BUT IT IS ALREADY IS.
								if(!tIsToContinue)
									{tErrorCode == ERROR_NO_MORE_FILES;}
#endif
							}
							else
							{
								crx_c_os_fileSystem_iterator_entry_private_setEntryType(
										&(tStackEntry->gEntry__current),
										CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY_TYPE__NULL);
								crx_c_string_empty(
										crx_c_os_fileSystem_iterator_entry_private_getName(
										&(tStackEntry->gEntry__current)));

								tEntry = &(tStackEntry->gEntry__current);

#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
								if(tStackEntry->gMode == 1)
									{tStackEntry->gMode = 2;}//SHOULD NOT HAPPEN

								tStackEntry->gDirent = crx_c_os_fileSystem_internal_posix_readdir(
										tStackEntry->gDir);

								if(tStackEntry->gDirent != NULL)
								{
									tName = tStackEntry->gDirent->d_name;
									tIsToContinue = true;
								}
								else
								{
									tErrorCode = errno;
									crx_c_os_fileSystem_internal_posix_closedir(tStackEntry->gDir);
									tStackEntry->gDir = NULL;
									
									crx_c_os_fileSystem_internal_posix_close(&(tStackEntry->gFileHandle));
								}
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
								if(tStackEntry->gMode == 1)
								{
									Crx_C_String tInternalDirectoryFullPath /*= ?*/;

									crx_c_string_construct(&tInternalDirectoryFullPath);

									if(tIsNoError && !crx_c_string_appendString(
											&tInternalDirectoryFullPath,
											&(pThis->gPrivate_internalDirectoryFullPath)))
										{tIsNoError = false;}
									if(tIsNoError && !crx_c_string_isEqualTo2(
											&(tStackEntry->gDirectoryPartialPath),
											CRX__OS__FILE_SYSTEM__SEPERATOR_STRING) &&
											!crx_c_string_appendString(&tInternalDirectoryFullPath,
											&(tStackEntry->gDirectoryPartialPath)))
										{tIsNoError = false;}
									if(tIsNoError && !crx_c_string_appendCString(
											&tInternalDirectoryFullPath, "*"))
										{tIsNoError = false;}
											
									if(tIsNoError)
									{
										tStackEntry->gHandle = 
												crx_c_os_fileSystem_internal_win32_findFirstFile(
												false, &tInternalDirectoryFullPath, 
												&(tStackEntry->gWin32FindData));

										crx_c_string_removeChars(&tInternalDirectoryFullPath, 
												crx_c_string_getSize(&tInternalDirectoryFullPath) - 
												1, 1);

										if(tStackEntry->gHandle != INVALID_HANDLE_VALUE)
										{
											tName = crx_c_string_getCString(
													&(tStackEntry->gWin32FindData.cFileName));
											tIsToContinue = true;
										}
										else
										{
											tErrorCode = GetLastError();

											/*
												UNSURE WHAT I WOULD GET WHEN USING * AND THE DIRECTORY
														DOES EXISTS, MEANING IT EXISTS BUT IT IS EMPTY.
														WHETHER IT EXISTS, OR IS EMPTY, THE FOLLOWING
														ENSURES THAT WE GET ERROR_NO_MORE_FILES, MEANING
														BOTH CASES ARE TREATED AS AN EMPTY DIRECTORY.
											*/
											if(tErrorCode == ERROR_FILE_NOT_FOUND)
												{tErrorCode = ERROR_NO_MORE_FILES;}
										}
									}
									else
										{tErrorCode = ERROR_NOT_ENOUGH_MEMORY;}
									
									crx_c_string_destruct(&tInternalDirectoryFullPath);
								}
								else
								{
									if(crx_c_os_fileSystem_internal_win32_findNextFile(
											tStackEntry->gHandle, &(tStackEntry->gWin32FindData)))
									{
										tName = crx_c_string_getCString(
												&(tStackEntry->gWin32FindData.cFileName));
										tIsToContinue = true;
									}
									else
									{
										tErrorCode = GetLastError();

										if(tErrorCode == ERROR_FILE_NOT_FOUND)
										{
											/* 
												UNSURE IF THIS IS POSSIBLE, AND I AM ASSUMING THAT IF 
														THIS HAPPENED, IT MEANS THAT THE DIRECTORY WAS 
														REMOVED WHILE WE WERE STILL ITERATING OVER IT.
														I AM HOPING THAT THIS IS NOT POSSIBLE, BUT JUST
														IN CASE.
											*/
											tErrorCode = ERROR_NO_MORE_FILES;
										}

										crx_c_os_fileSystem_internal_win32_findClose(
												tStackEntry->gHandle);
										tStackEntry->gHandle = INVALID_HANDLE_VALUE;
									}
								}
#endif
							}

							if(tIsToContinue)
							{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
								if(crx_c_os_fileSystem_internal_posix_fstatat(
										&(tStackEntry->gFileHandle), tName, &tStat, true))
								{
									if(S_ISDIR(tStat))
									{
										crx_c_os_fileSystem_iterator_entry_private_setEntryType(
												tEntry,
												CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY_TYPE__DIRECTORY);
									}
									else if(S_ISREG(tStat))
									{
										crx_c_os_fileSystem_iterator_entry_private_setEntryType(
												tEntry,
												CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY_TYPE__FILE);
									}
									else if(S_ISLNK(tStat))
									{
										crx_c_os_fileSystem_iterator_entry_private_setEntryType(
												tEntry,
												CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY_TYPE__REPARSE_POINT);
									}

									/*
									if((tStackEntry->gAllEntries != NULL) && 
											(tEntry->gPrivate_dInoOrFileno == tStat.st_ino))
									{
										UNSURE WHAT ERROR TO USE. REMEMBER, ON WINDOWS WE STORE THE 
										INFORMATION ON THE FIRST ITERATION, BUT ON POSIX OPERATING
										SYSTEMS, WE DO NOT GET THAT INFORMATION IN THE FIRST PLACE.
										WHILE THIS IS UNLIKELY TO BE A PROBLEM BECAUSE OF THE USE OF THE
										"at" FUNCTIONS AND THE FACT THAT DIRECTORY WE ARE ITERATING IT
										IS ALWAYS REFERENCED BY THE HANDLE, AND THE FACT THAT FIRST
										ITERATION WOULD HAVE ALREADY COVERED SLOT OCCUPIED AND
										AMBIGUITY SITUATIONS, THIS IS UNLIKELY TO BE AN ISSUE, BUT STILL
										CAN BECAUSE THE ABOVE IS NOT ATOMIC.
										
										IF YOU DECIDE TO UNCOMMENT THIS CODE, AND ASSIGN AN ERROR, REFER
										TO crx_c_os_fileSystem_private_posix_isStatOfFileLikelySameAsOneFromDirent()
										FIRST. REMEMBER THE FREEBSD ISSUE OF SEITCHING TO 64 BITS 
										INTEGERS FOR THE VALUE CHECKED ABOVE.
									}*/
								}
								else
									{tErrorCode = errno;}
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
								DWORD tDwAttributes = 0;

								if(tStackEntry->gAllEntries != NULL)
									{tDwAttributes = tEntry->gPrivate_dwFileAttributes;}
								else
									{tDwAttributes = tStackEntry->gWin32FindData.dwFileAttributes;}

								if(tDwAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
								{
									crx_c_os_fileSystem_iterator_entry_private_setEntryType(tEntry,
											CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY_TYPE__REPARSE_POINT);
								}
								else
								{
									if(tDwAttributes & FILE_ATTRIBUTE_DIRECTORY)
									{
										crx_c_os_fileSystem_iterator_entry_private_setEntryType(
												tEntry,
												CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY_TYPE__DIRECTORY);
									}
									else
									{
										crx_c_os_fileSystem_iterator_entry_private_setEntryType(
												tEntry,
												CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY_TYPE__FILE);
									}
								}
#endif
								tIsToContinue = (tEntry->gPrivate_entryType != 
										CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY_TYPE__NULL);
							}

							if(tIsToContinue)
							{
								Crx_C_String tString /*= ?*/;
								Crx_C_String tVerifiedPathPrefix /*= ?*/;
								Crx_C_String tVerifiedRoute /*= ?*/;
								Crx_C_String tVerifiedOriginalPathPrefix /*= ?*/;
								bool tIsNoContractError = true;
								bool tIsNoError = true;

								crx_c_string_construct(&tString);
								crx_c_string_construct(&tVerifiedPathPrefix);
								crx_c_string_construct(&tVerifiedRoute);
								crx_c_string_construct(&tVerifiedOriginalPathPrefix);
								
								tIsNoError = crx_c_string_appendCString(&tString, tName);
								
								if(tIsNoError && (tEntry->gPrivate_entryType ==
										CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY_TYPE__DIRECTORY))
								{
									crx_c_string_appendCString(&tString,
											CRX__OS__FILE_SYSTEM__SEPERATOR_STRING);
								}

								*(crx_c_os_fileSystem_iterator_entry_private_getLengthResidueWhenDirectory(
										tEntry)) = tStackEntry->gLengthResidueWhenDirectory;
								*(crx_c_os_fileSystem_iterator_entry_private_getLengthResidue(
										tEntry)) = tStackEntry->gLengthResidue;

								if(tIsNoError)
								{
									if(crx_c_os_fileSystem_verifyFileSystemPath(
											&(pThis->gPrivate_contract), 
											CRX__C__OS__FILE_SYSTEM__OPERATION__PATH_RETREIVAL, 
											&tString, 0, true, &tVerifiedPathPrefix,
											&tVerifiedRoute, &tVerifiedOriginalPathPrefix, 
											crx_c_os_fileSystem_iterator_entry_private_getLengthResidueWhenDirectory(
													tEntry),
											crx_c_os_fileSystem_iterator_entry_private_getLengthResidue(
													tEntry), NULL, NULL))
									{
										if(crx_c_string_getSize(&tVerifiedPathPrefix) == 0)
										{
											if(tStackEntry->gAllEntries == NULL)
											{
												Crx_C_String * tNameOfEntry = 
														crx_c_os_fileSystem_iterator_entry_private_getName(
														tEntry);
												/*REMEMBER: IF WE ARE HERE, 
														tEntry == &(tStackEntry->gEntry__current)*/
												crx_c_string_empty(tNameOfEntry);

#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
												/*
												WARNING: WE WANT TO KEEP THE RAW DIRECTORY OR FILE 
														NAME AS WE RECIEVED FROM THE SYSTEM, AND 
														HENCE WE DO NOT USE THE EXTRACTED PREFIX 
														FROM 
														crx_c_os_fileSystem_verifyFileSystemPath()
														FOR THE CREATED Entry. CONSIDER IF THE FILE 
														NAME CONTAINS '\' ON LINUX, 
														crx_c_os_fileSystem_verifyFileSystemPath()
														WOULD CONVERT TO '\' TO '/' AND IF WE USED 
														THIS CHANGED NAME INSTEAD WE WOULD BECOME 
														BLIND TO THIS ENTRY, DEFEATING THE PRIMARY 
														PURPOSE OF THE ITERATOR.
														
												UPDATE(30-MAY-2025): THE ABOVE MIGHT NOT HAVE BEEN
														FULLY VALID AS AN APPROACH, BUT I AM NOT 
														SURE. 
														crx_c_os_fileSystem_verifyFileSystemPath()
														NOW ACCEPTS THE
														pIsToForceTreatAsSingleSegment PARAMETER TO 
														SOLVE THE ISSUE.
												*/
												tIsNoError = crx_c_string_appendCString(
														tNameOfEntry, tStackEntry->gDirent->d_name);

												crx_c_os_fileSystem_iterator_entry_private_setPosixData(
														tEntry, tStackEntry->gDirent);
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
												tIsNoError = crx_c_string_appendString(tNameOfEntry, 
														&(tStackEntry->gWin32FindData.cFileName));

												crx_c_os_fileSystem_iterator_entry_private_setWin32Data(
														tEntry, &(tStackEntry->gWin32FindData));
#endif
											}

											tIsFinished = true;
										}
									}
									else
										{tIsNoContractError = false;}
								}
								
								if(!tIsNoError)
								{
									//ABOVE CODE MUST GUARANTEE THAT THIS IS A MEMORY ERROR
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
									tErrorCode = ENOMEM;
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
									tErrorCode = ERROR_NOT_ENOUGH_MEMORY;
#endif
									tIsToContinue = false;
								}
								else
								{
									if(pThis->gPrivate_areContractualErrorsFatal && 
											!tIsNoContractError)
									{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
										tErrorCode = EINVAL;
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
										tErrorCode = ERROR_BAD_PATHNAME;
#endif
										tIsNoError = false;
										tIsToContinue = false;
									}
								}

								crx_c_string_destruct(&tString);
								crx_c_string_destruct(&tVerifiedPathPrefix);
								crx_c_string_destruct(&tVerifiedRoute);
								crx_c_string_destruct(&tVerifiedOriginalPathPrefix);
							}
							
							if(tIsToContinue)
							{
								//tErrorCode WOULD BE 0 HERE.
								if(tIsFinished)
									{tIsToContinue = false;}
							}
							else
							{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
								/*
									ERRORS CONSIDERED:

									EACCES			EBADF			ELOOP			EMFILE
									ENAMETOOLONG	ENOENT			ENFILE			ENOTDIR
									0				EINVAL
									
									EINVAL IS EXPECTED TO COME FROM US AND NOT THE SYSTEM.
								*/
								if(tErrorCode == ENOENT)
								{
									//I AM ASSUMING THAT IF POSIX'S readdir() WAS CALLED AND GAVE AN 
									//		ERROR, IT WOULD NOT BE ENOENT, AND IF ENOENT HAPPENED AT
									//		THIS POINT IT WOULD BE FROM POSIX'S 
									//		stat()/lstat()/fstat()/fstatat(). MEANING WE SAW THE ENTRY,
									//		BUT BY THE TIME WE CALLED stat() OR IT'S SISTERS, THE ENTRY
									//		WAS REMOVED.
									tIsToContinue = true;
									tErrorCode = 0;
								}
								//I AM ASSUMING THAT IF tErrorCode IS ALREADY 0 AND
								//		tStackEntry->gAllEntries WAS NULL, THEN THIS HAPPENED BECAUSE 
								//		readdir() REACHED THE END, OTHERWISE tStackEntry->gAllEntries 
								//		WAS EXCHAUSTED. IN BOTH CASES, tIsToContinue NEEDS TO BE false,
								//		AND THE DIRECTORY ENTRIES WERE EXCHAUSTED
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
								/*
									ERRORS CONSIDERED:
											ERROR_ACCESS_DENIED				ERROR_BAD_PATHNAME			
											ERROR_BUFFER_OVERFLOW			ERROR_FILE_NOT_FOUND		
											ERROR_FILENAME_EXCED_RANGE		ERROR_INVALID_FUNCTION
											ERROR_INVALID_HANDLE			ERROR_LABEL_TOO_LONG		
											ERROR_NOT_READY					ERROR_PATH_NOT_FOUND		
											ERROR_TOO_MANY_OPEN_FILES		ERROR_NO_MORE_FILES

									IF FindFirstFile() WAS USED ABOVE, I AM UNSURE WHAT WOULD I GET 
											WHEN USING * AND THE DIRECTORY IS MISSING OR EMPTY. UNLIKE 
											THE POSIX CASE, WHEN WE FIRST ENTER THIS FUNCTION, MEANING 
											WHEN tStackEntry->gMode IS 1, IT IS POSSIBLE FOR THE PARENT 
											DIRECTORY TO BE REMOVED BEFORE WE GET TO BEGIN ITERATING 
											OVER IT, AND THEREFORE FindFirstFile() WOULD ENCOUNTER A 
											FILE NOT FOUND SITUATION. 
											
											I AM ALSO HOPING THAT LATER WHILE ITERATING, FindNextFile() 
											WOULD NOT ENCOUNTER THIS SITUATION, MEANING THE DIRECTORY IT 
											IS ITERATING OVER WOULD NOT DISSAPPEAR FROM UNDER IT'S FEET. 
											HOWEVER, IT IS GOOD THAT THE IMPLEMENTATION IS STILL ABLE TO 
											HANDLE THAT, NOT THAT I SAW ANY INDICATION THAT THAT COULD 
											HAPPEN.
								*/
								if(tErrorCode == ERROR_NO_MORE_FILES)
								{
									//AT THIS POINT, IT IS GUARANTEED THAT WHETHER THE PARENT DIRECTORY
									//		NO LONGER EXISTS, OR WE FINISHED ITERATING ALL ITS ENTRIES,
									//		THE ERROR CODE WOULD BE ERROR_NO_MORE_FILES
									tErrorCode = 0;
								}
#endif
							}
						}
						
						if(tErrorCode != 0)
							{tIsFinished = true;}

						if(!tIsFinished)
						{
							CRX_SCOPE_META
							assert(tIsToContinue == false);

							CRX_SCOPE
							bool tIsNotFound = true;

							if(pThis->gPrivate_getOrderOfObjects == NULL)
							{
								if((pThis->gEntries1 != NULL) && 
										(crx_c_queue_getLength(&(pThis->gEntries1)) > 0))
								{
									tStackEntry->gMode == 3;
									tStackEntry->gCurrentStep = tStackEntry->gCurrentStep + 1;
									
									tIsNotFound = false;
								}
							}
							else
							{
								if((pThis->gSortedEntries1 != NULL) && 
										(crx_c_sortedQueue_getLength(&(pThis->gSortedEntries1)) > 0))
								{
									tStackEntry->gMode == 3;
									tStackEntry->gCurrentStep = tStackEntry->gCurrentStep + 1;

									tIsNotFound = false;
								}
							}
							
							if(tIsNotFound)
							{
								crx_c_queue_pop(&(pThis->gPrivate_stackEntries));
								
								tStackEntry = crx_c_queue_get(&(pThis->gPrivate_stackEntries));
								
								if(tStackEntry == NULL)
									{tIsFinished = true;}
								else if(tStackEntry->gAdvancingStep == 1)
								{
									tStackEntry->gAdvancingStep = 2;
									crx_c_os_fileSystem_iterator_entry_private_raiseFlags(
											&(tStackEntry->gEntry__current),
											CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY__FLAG__MOVING_OUT);
									tIsFinished = true;
								}
							}
							else
								{tIsFinished = true;}
							
							CRX_SCOPE_END
						}
					}
					
					if(!tIsFinished && ((tStackEntry->gMode == 3) || (tStackEntry->gMode == 4)))
					{
						CRX_FOR(size_t tI = 0, tI < 2, tI++)
						{
							if(pThis->gPrivate_getOrderOfObjects == NULL)
							{
								Crx_C_Queue/*<Crx_C_Os_FileSystem_Iterator_Entry>*/ * tEntries = NULL;
								Crx_C_Queue/*<Crx_C_Os_FileSystem_Iterator_Entry>*/ * tEntries2 = NULL;

								if(tStackEntry->gMode == 3)
								{
									tEntries = pThis->gEntries1;
									tEntries2 = pThis->gEntries2;
								}
								else
								{
									tEntries = pThis->gEntries2;
									tEntries2 = pThis->gEntries1;
								}
								
								if(tEntries != NULL)
								{
									if(crx_c_queue_getLength(tEntries) > 0)
									{
										crx_c_queue_popFromFront(tEntries);

										if(crx_c_queue_getLength(tEntries) > 0)
											{tIsFinished = true;}
									}
								}
							}
							else
							{
								Crx_C_SortedQueue/*<Crx_C_Os_FileSystem_Iterator_Entry>*/ * tSortedEntries = 
											NULL;

								if(tStackEntry->gMode == 3)
									{tSortedEntries = pThis->gSortedEntries1;}
								else
									{tSortedEntries = pThis->gSortedEntries2;}
								
								if(tSortedEntries != NULL)
								{
									if(crx_c_sortedQueue_getLength(tSortedEntries) > 0)
									{
										crx_c_sortedQueue_popFromFront(tSortedEntries);

										if(crx_c_sortedQueue_getLength(tSortedEntries) > 0)
											{tIsFinished = true;}
									}
								}
							}
							
							if(tIsFinished)
							{
								tStackEntry->gCurrentStep = tStackEntry->gCurrentStep + 1;
								break;
							}
							else
							{
								if(tStackEntry->gMode == 3)
									{tStackEntry->gMode = 4;}
								else
									{tStackEntry->gMode = 3;}
							}
						}
						CRX_ENDFOR

						if(!tIsFinished)
						{
							crx_c_queue_pop(&(pThis->gPrivate_stackEntries));

							tStackEntry = crx_c_queue_get(&(pThis->gPrivate_stackEntries));

							if(tStackEntry == NULL)
								{tIsFinished = true;}
							else if(tStackEntry->gAdvancingStep == 1)
							{
								tStackEntry->gAdvancingStep = 2;
								crx_c_os_fileSystem_iterator_entry_private_raiseFlags(
										&(tStackEntry->gEntry__current),
										CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY__FLAG__MOVING_OUT);
								tIsFinished = true;
							}
						}
					}
				}

				assert(tIsFinished);

				if(crx_c_queue_getLength(&(pThis->gPrivate_stackEntries)) == 0)
					{tReturn = 1;}
				else
				{
					if(tErrorCode == 0)
						{tReturn = 0;}
					else
					{
						tReturn = -2;
						crx_c_queue_empty(&(pThis->gPrivate_stackEntries));
						pThis->gPrivate_isNoError = false;
					}
				}
			}
		}

		return tReturn;
	}
}

CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_iterator_isValid(
		Crx_C_Os_FileSystem_Iterator * pThis)
	{return (crx_c_queue_getLength(&(pThis->gPrivate_stackEntries)) != 0);}
//ITERATION MIGHT FINISH BECAUSE OF A FATAL ERROR. CLIENT CODE SHOULD CHECK THAT THAT IS NOT THE
//		CASE WITH THE FOLLOWING FUNCTION.
CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_iterator_isNoError(
		Crx_C_Os_FileSystem_Iterator * pThis)
	{return pThis->gPrivate_isNoError;}
	
CRX__LIB__PRIVATE_C_FUNCTION() Crx_C_Os_FileSystem_Iterator_Entry * 
		crx_c_os_fileSystem_iterator_private_getEntry(Crx_C_Os_FileSystem_Iterator * pThis)
{
	CRX_SCOPE_META
	if(!(pThis->gPrivate_isSet))
		{return NULL;}
	else if(crx_c_queue_getLength(&(pThis->gPrivate_stackEntries)) == 0)
		{return NULL;}

	CRX_SCOPE
	Crx_C_Os_FileSystem_Iterator_Private_StackEntry * vStackEntry = crx_c_queue_get(
			&(pThis->gPrivate_stackEntries));

	/*if(vStackEntry->gAdvancingStep == 2)
		{return &(vStackEntry->gEntry__current);}
	else
	{*/
	if(vStackEntry->gMode == 2)
	{
		if(vStackEntry->gAllEntries != NULL)
		{
			return ((Crx_C_Os_FileSystem_Iterator_Entry *)
					crx_c_orderedHashTable_getElementFromIndex(vStackEntry->gAllEntries, 
					vStackEntry->gAllEntries_index));
		}
		else
			{return &(vStackEntry->gEntry__current);}
	}
	else if(vStackEntry->gMode == 3)
		{return crx_c_sortedQueue_get(pThis->gEntries1);}
	else if(vStackEntry->gMode == 4)
		{return crx_c_sortedQueue_get(pThis->gEntries2);}
	else
		{return NULL;}
	/*}*/
	
	CRX_SCOPE_END
}

CRX__LIB__PUBLIC_C_FUNCTION() bool crx_c_os_fileSystem_iterator_getRecord(
		Crx_C_Os_FileSystem_Iterator * pThis,
		Crx_C_Os_FileSystem_Iterator_Record * CRX_NOT_NULL pReturn)
{
	Crx_C_Os_FileSystem_Iterator_Entry * vEntry = crx_c_os_fileSystem_iterator_private_getEntry(
			pThis);
	Crx_C_Os_FileSystem_Iterator_Private_StackEntry * vStackEntry = crx_c_queue_get(
			&(pThis->gPrivate_stackEntries));
	bool vReturn = true;

	if(vEntry != NULL)
	{
		if(crx_c_os_fileSystem_iterator_entry_copyAssignFrom(&(pReturn->gPrivate_entry), vEntry))
		{
			pReturn->gPrivate_byteSizeOfPathPrefixInInternalFullPath = 
					pThis->gPrivate_byteSizeOfPathPrefixInInternalDirectoryFullPath;
			pReturn->gPrivate_byteSizeOfPathPrefixInFullPath = 
					pThis->gPrivate_byteSizeOfPathPrefixInDirectoryFullPath;

			if(!crx_c_string_copyAssignFrom(&(pReturn->gPrivate_internalFullPath), 
					&(pThis->gPrivate_internalDirectoryFullPath)))
				{vReturn = false;}
		}
		else
			{vReturn = false;}

		if(vReturn && !crx_c_string_isEqualTo2(&(vStackEntry->gDirectoryPartialPath),
				CRX__OS__FILE_SYSTEM__SEPERATOR_STRING) && !crx_c_string_appendString(
				&(pReturn->gPrivate_internalFullPath), &(vStackEntry->gDirectoryPartialPath)))
			{vReturn = false;}
		if(vReturn && !crx_c_string_appendString(&(pReturn->gPrivate_internalFullPath), 
				&(vEntry->gPrivate_name)))
			{vReturn = false;}

		if(vReturn && !crx_c_string_copyAssignFrom(&(pReturn->gPrivate_fullPath), 
				&(pThis->gPrivate_directoryFullPath)))
			{vReturn = false;}
		if(vReturn && !crx_c_string_isEqualTo2(&(vStackEntry->gDirectoryPartialPath), 
				CRX__OS__FILE_SYSTEM__SEPERATOR_STRING) && !crx_c_string_appendString(
				&(pReturn->gPrivate_fullPath), &(vStackEntry->gDirectoryPartialPath)))
			{vReturn = false;}
		if(vReturn && !crx_c_string_appendString(&(pReturn->gPrivate_fullPath), 
				&(vEntry->gPrivate_name)))
			{vReturn = false;}

		if(vReturn && (vEntry->gPrivate_entryType == 
						CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY_TYPE__DIRECTORY))
		{
			vReturn = crx_c_string_appendCString(&(pReturn->gPrivate_internalFullPath), 
					CRX__OS__FILE_SYSTEM__SEPERATOR_STRING);

			if(vReturn && !crx_c_string_appendCString(&(pReturn->gPrivate_fullPath), 
					CRX__OS__FILE_SYSTEM__SEPERATOR_STRING))
				{vReturn = false;}
		}
	}
	else
		{vReturn = false;}

	return vReturn;
}

CRX__LIB__PUBLIC_C_FUNCTION() bool crx_c_os_fileSystem_iterator_getInternalDirectoryFullPath(
		Crx_C_Os_FileSystem_Iterator * pThis, Crx_C_String * CRX_NOT_NULL pReturn)
	{return crx_c_string_copyAssignFrom(pReturn, &(pThis->gPrivate_internalDirectoryFullPath));}

CRX__LIB__PUBLIC_C_FUNCTION() Crx_C_String const * crx_c_os_fileSystem_iterator_getCurrentName(
		Crx_C_Os_FileSystem_Iterator * pThis)
{
	Crx_C_Os_FileSystem_Iterator_Entry * vEntry = crx_c_os_fileSystem_iterator_private_getEntry(
			pThis);

	if(vEntry != NULL)
		{return &(vEntry->gPrivate_name);}
	else
		{return NULL;}
}
CRX__LIB__PUBLIC_C_FUNCTION() Crx_C_Os_FileSystem_Iterator_EntryType
		crx_c_os_fileSystem_iterator_getCurrentEntryType(Crx_C_Os_FileSystem_Iterator * pThis)
{
	Crx_C_Os_FileSystem_Iterator_Entry * vEntry = crx_c_os_fileSystem_iterator_private_getEntry(
			pThis);

	if(vEntry != NULL)
		{return vEntry->gPrivate_entryType;}
	else
		{return CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY_TYPE__NULL;}
}
CRX__LIB__PUBLIC_C_FUNCTION() uint32_t crx_c_os_fileSystem_iterator_getCurrentDepth(
		Crx_C_Os_FileSystem_Iterator * pThis)
{
	if(pThis->gPrivate_isSet)
		{return crx_c_queue_getLength(&(pThis->gPrivate_stackEntries));}
	else
		{return 0;}
}
CRX__LIB__PUBLIC_C_FUNCTION() uint32_t crx_c_os_fileSystem_iterator_getCurrentRunCount(
		Crx_C_Os_FileSystem_Iterator * pThis)
{
	if(pThis->gPrivate_isSet && (crx_c_queue_getLength(&(pThis->gPrivate_stackEntries)) > 0))
	{
		Crx_C_Os_FileSystem_Iterator_Private_StackEntry * tStackEntry = 
				((Crx_C_Os_FileSystem_Iterator_Private_StackEntry *)crx_c_queue_get(
				&(pThis->gPrivate_stackEntries)));

		return tStackEntry->gCurrentStep;
	}
	else
		{return 0;}
}
CRX__LIB__PUBLIC_C_FUNCTION() bool crx_c_os_fileSystem_iterator_getCurrentFullPath(
		Crx_C_Os_FileSystem_Iterator * pThis, Crx_C_String * CRX_NOT_NULL pReturn)
{
	Crx_C_Os_FileSystem_Iterator_Entry * vEntry = crx_c_os_fileSystem_iterator_private_getEntry(
			pThis);
	Crx_C_Os_FileSystem_Iterator_Private_StackEntry * vStackEntry = crx_c_queue_get(
			&(pThis->gPrivate_stackEntries));
	bool vReturn = true;

	crx_c_string_empty(pReturn);

	if(vEntry != NULL)
	{
		if(!crx_c_string_copyAssignFrom(pReturn, &(pThis->gPrivate_directoryFullPath)))
			{vReturn = false;}
		if(vReturn && !crx_c_string_isEqualTo2(&(vStackEntry->gDirectoryPartialPath),
				CRX__OS__FILE_SYSTEM__SEPERATOR_STRING) && !crx_c_string_appendString(pReturn, 
				&(vStackEntry->gDirectoryPartialPath)))
			{vReturn = false;}
		if(vReturn && !crx_c_string_appendString(pReturn, &(vEntry->gPrivate_name)))
			{vReturn = false;}
		if(vReturn && (vEntry->gPrivate_entryType == 
						CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY_TYPE__DIRECTORY))
			{vReturn = crx_c_string_appendCString(pReturn, CRX__OS__FILE_SYSTEM__SEPERATOR_STRING);}
	}
	else
		{vReturn = false;}
	
	if(!vReturn)
		{crx_c_string_empty(pReturn);}

	return vReturn;
}
CRX__LIB__PUBLIC_C_FUNCTION() bool crx_c_os_fileSystem_iterator_getCurrentInternalFullPath(
		Crx_C_Os_FileSystem_Iterator * pThis, Crx_C_String * CRX_NOT_NULL pReturn)
{
	Crx_C_Os_FileSystem_Iterator_Entry * vEntry = crx_c_os_fileSystem_iterator_private_getEntry(
			pThis);
	Crx_C_Os_FileSystem_Iterator_Private_StackEntry * vStackEntry = crx_c_queue_get(
			&(pThis->gPrivate_stackEntries));
	bool vReturn = true;

	crx_c_string_empty(pReturn);

	if(vEntry != NULL)
	{
		if(!crx_c_string_appendString(pReturn, &(pThis->gPrivate_internalDirectoryFullPath)))
			{vReturn = false;}
		if(vReturn && !crx_c_string_isEqualTo2(&(vStackEntry->gDirectoryPartialPath),
				CRX__OS__FILE_SYSTEM__SEPERATOR_STRING) && !crx_c_string_appendString(pReturn, 
				&(vStackEntry->gDirectoryPartialPath)))
			{vReturn = false;}
		if(vReturn && !crx_c_string_appendString(pReturn, &(vEntry->gPrivate_name)))
			{vReturn = false;}
		if(vReturn && (vEntry->gPrivate_entryType == 
						CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY_TYPE__DIRECTORY))
			{vReturn = crx_c_string_appendCString(pReturn, CRX__OS__FILE_SYSTEM__SEPERATOR_STRING);}
	}
	else
		{vReturn = false;}

	if(!vReturn)
		{crx_c_string_empty(pReturn);}

	return vReturn;
}
CRX__LIB__PUBLIC_C_FUNCTION() bool crx_c_os_fileSystem_iterator_getCurrentPartialPath(
		Crx_C_Os_FileSystem_Iterator * pThis, Crx_C_String * CRX_NOT_NULL pReturn)
{
	Crx_C_Os_FileSystem_Iterator_Entry * vEntry = crx_c_os_fileSystem_iterator_private_getEntry(
			pThis);
	Crx_C_Os_FileSystem_Iterator_Private_StackEntry * vStackEntry = crx_c_queue_get(
			&(pThis->gPrivate_stackEntries));
	bool vReturn = true;

	crx_c_string_empty(pReturn);

	if(vEntry != NULL)
	{
		if(!crx_c_string_isEqualTo2(&(vStackEntry->gDirectoryPartialPath),
				CRX__OS__FILE_SYSTEM__SEPERATOR_STRING) && !crx_c_string_appendString(pReturn, 
				&(vStackEntry->gDirectoryPartialPath)))
			{vReturn = false;}
		if(vReturn && !crx_c_string_appendString(pReturn, &(vEntry->gPrivate_name)))
			{vReturn = false;}
		if(vReturn && (vEntry->gPrivate_entryType == 
						CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY_TYPE__DIRECTORY))
			{vReturn = crx_c_string_appendCString(pReturn, CRX__OS__FILE_SYSTEM__SEPERATOR_STRING);}
	}
	else
		{vReturn = false;}

	if(!vReturn)
		{crx_c_string_empty(pReturn);}

	return vReturn;
}
CRX__LIB__PUBLIC_C_FUNCTION() bool crx_c_os_fileSystem_iterator_internal_getCurrentIsValid(
		Crx_C_Os_FileSystem_Iterator * pThis)
{
	Crx_C_Os_FileSystem_Iterator_Entry * vEntry = crx_c_os_fileSystem_iterator_private_getEntry(
			pThis);

	if(vEntry != NULL)
	{
		return ((vEntry->gPrivate_flags & 
				CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY__FLAG__IS_VALID) != 0);
	}
	else
		{return false;}
}
CRX__LIB__PUBLIC_C_FUNCTION() bool crx_c_os_fileSystem_iterator_getCurrentIsBarredEntry(
		Crx_C_Os_FileSystem_Iterator * pThis)
{
	Crx_C_Os_FileSystem_Iterator_Entry * vEntry = crx_c_os_fileSystem_iterator_private_getEntry(
			pThis);

	if(vEntry != NULL)
	{
		return ((vEntry->gPrivate_flags & 
				CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY__FLAG__BARRED_ENTRY) != 0);
	}
	else
		{return false;}
}
CRX__LIB__PUBLIC_C_FUNCTION() bool crx_c_os_fileSystem_iterator_getCurrentIsAMoveOut(
		Crx_C_Os_FileSystem_Iterator * pThis)
{
	Crx_C_Os_FileSystem_Iterator_Entry * vEntry = crx_c_os_fileSystem_iterator_private_getEntry(
			pThis);

	if(vEntry != NULL)
	{
		return ((vEntry->gPrivate_flags & 
				CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY__FLAG__MOVING_OUT) != 0);
	}
	else
		{return false;}
}
CRX__LIB__PUBLIC_C_FUNCTION() bool crx_c_os_fileSystem_iterator_getCurrentIsAForcedMoveOut(
		Crx_C_Os_FileSystem_Iterator * pThis)
{
	Crx_C_Os_FileSystem_Iterator_Entry * vEntry = crx_c_os_fileSystem_iterator_private_getEntry(
			pThis);

	if(vEntry != NULL)
	{
		return ((vEntry->gPrivate_flags & 
				CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY__FLAG__FORCED_MOVE_OUT) != 0);
	}
	else
		{return false;}
}
/*
WARNING: UNFINISHED. REMEMBER, YOU STILL HAVE TO VERIFY pDirectoryPath. AVOID
CRX__LIB__PUBLIC_C_FUNCTION() bool crx_c_os_fileSystem_iterator_appendEntryPartialPathTo(
		Crx_C_Os_FileSystem_Iterator * pThis, Crx_C_String * CRX_NOT_NULL pDirectoryPath)
{
	Crx_C_Os_FileSystem_Iterator_Entry * vEntry = crx_c_os_fileSystem_iterator_private_getEntry(
			pThis);
	Crx_C_Os_FileSystem_Iterator_Private_StackEntry * vStackEntry = crx_c_queue_get(
			&(pThis->gPrivate_stackEntries));
	size_t vSize = crx_c_string_getSize(pDirectoryPath);
	bool vWasPathEmptyPartial = false;
	bool vReturn = true;
	
	if(vEntry != NULL)
	{
		vReturn = crx_c_os_fileSystem_enforcePathAsDirectoryPath(pDirectoryPath, false);

		if(crx_c_string_isEqualTo2(pDirectoryPath, CRX__OS__FILE_SYSTEM__SEPERATOR_STRING) ||
				!crx_c_string_isBeginningWith2(pDirectoryPath, 
						CRX__OS__FILE_SYSTEM__SEPERATOR_STRING, false))
		{//PARTIAL PATH. SEE CRX URL STANDARD
			if(crx_c_string_isEqualTo2(pDirectoryPath, CRX__OS__FILE_SYSTEM__SEPERATOR_STRING))
				{crx_c_string_empty(pDirectoryPath);}
		}

		//vEntry->gPrivate_name IS EXPECTED NEVER EMPTY
		if(!crx_c_string_isEqualTo2(&(vStackEntry->gDirectoryPartialPath),
						CRX__OS__FILE_SYSTEM__SEPERATOR_STRING) &&
				!crx_c_string_appendString(pDirectoryPath, &(vStackEntry->gDirectoryPartialPath)))
			{vReturn = false;}
		if(vReturn && !crx_c_string_appendString(pDirectoryPath, &(vEntry->gPrivate_name)))
			{vReturn = false;}
		if(vReturn && (vEntry->gPrivate_entryType == 
						CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY_TYPE__DIRECTORY))
		{
			vReturn = crx_c_string_appendCString(pDirectoryPath, 
					CRX__OS__FILE_SYSTEM__SEPERATOR_STRING);
		}
	}
	else
		{vReturn = false;}

	if(!vReturn)
	{
		crx_c_string_removeChars(pDirectoryPath, vSize, crx_c_string_getSize(&pDirectoryPath) - 
				vSize);
	}

	return vReturn;
}*/

//WARNING: DO NOT CLOSE THE HANDLE, OR CHANGE ITS TARGET
CRX__LIB__PUBLIC_C_FUNCTION() Crx_C_Os_FileSystem_Internal_Posix_FileHandle * 
		crx_c_os_fileSystem_iterator_internal_getParentDirectoryFileHandleOnPosix(
		Crx_C_Os_FileSystem_Iterator * pThis)
{
	Crx_C_Os_FileSystem_Iterator_Private_StackEntry * vStackEntry = crx_c_queue_get(
			&(pThis->gPrivate_stackEntries));

	if(vStackEntry != NULL)
		{return vStackEntry->gFileHandle;}
	else
		{return NULL;}
}

//CLASS: Iterator::Entry
CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_iterator_entry_construct(
		Crx_C_Os_FileSystem_Iterator_Entry * pThis)
{
	pThis->gPrivate_flags = CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY__FLAG__NULL;
	pThis->gPrivate_entryType = CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY_TYPE__NULL;
	crx_c_string_construct(pThis->gPrivate_name);
	pThis->gPrivate_lengthResidueWhenDirectory = 0;
	pThis->gPrivate_lengthResidue = 0;
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD))
	pThis->gPrivate_dInoOrFileno = 0;
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	pThis->gPrivate_dwFileAttributes = 0;
	CRX__SET_TO_ZERO(FILETIME, pThis->gPrivate_ftLastWriteTime);
	pThis->gPrivate_nFileSizeHigh = 0;
	pThis->gPrivate_nFileSizeLow = 0;
#endif
}
CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_iterator_entry_copyConstruct(
		Crx_C_Os_FileSystem_Iterator_Entry * pThis);
		Crx_C_Os_FileSystem_Iterator_Entry const * CRX_NOT_NULL pEntry)
{
	pThis->gPrivate_flags = pEntry->gPrivate_flags;
	pThis->gPrivate_entryType = pEntry->gPrivate_entryType;
	crx_c_string_copyConstruct(&(pThis->gPrivate_name), &(pEntry->gPrivate_name));
	pThis->gPrivate_lengthResidueWhenDirectory = pEntry->gPrivate_lengthResidueWhenDirectory;
	pThis->gPrivate_lengthResidue = pEntry->gPrivate_lengthResidue;
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	pThis->gPrivate_dInoOrFileno = pEntry->gPrivate_dInoOrFileno;
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	pThis->gPrivate_dwFileAttributes = pEntry->gPrivate_dwFileAttributes;
	memcpy(&(pThis->gPrivate_ftLastWriteTime), &(pEntry->gPrivate_ftLastWriteTime), 
			sizeof(FILETIME));
	pThis->gPrivate_nFileSizeHigh = pEntry->gPrivate_nFileSizeHigh;
	pThis->gPrivate_nFileSizeLow = pEntry->gPrivate_nFileSizeLow;
#endif
}

CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_iterator_entry_new(
		Crx_C_Os_FileSystem_Iterator_Entry * pThis)
{
	Crx_C_Os_FileSystem_Iterator_Entry * vReturn = 
			((Crx_C_Os_FileSystem_Iterator_Entry *)malloc(sizeof(Crx_C_Os_FileSystem_Iterator_Entry)));

	if(vReturn != NULL)
		{crx_c_os_fileSystem_iterator_entry_construct(vReturn);}

	return vReturn;
}
CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_iterator_entry_moveNew(
		Crx_C_Os_FileSystem_Iterator_Entry * pThis,
		Crx_C_Os_FileSystem_Iterator_Entry * CRX_NOT_NULL pEntry)
{
	Crx_C_Os_FileSystem_Iterator_Entry * vReturn = ((Crx_C_Os_FileSystem_Iterator_Entry *)malloc(
			sizeof(Crx_C_Os_FileSystem_Iterator_Entry)));

	if(vReturn != NULL)
		{memcpy(vReturn, pEntry, sizeof(Crx_C_Os_FileSystem_Iterator_Entry));}

	return vReturn;
}
CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_iterator_entry_copyNew(
		Crx_C_Os_FileSystem_Iterator_Entry * pThis,
		Crx_C_Os_FileSystem_Iterator_Entry const * CRX_NOT_NULL pEntry)
{
	Crx_C_Os_FileSystem_Iterator_Entry * vReturn = 
			((Crx_C_Os_FileSystem_Iterator_Entry *)malloc(sizeof(Crx_C_Os_FileSystem_Iterator_Entry)));

	if(vReturn != NULL)
		{crx_c_os_fileSystem_iterator_entry_copyConstruct(vReturn, pEntry);}

	return vReturn;
}

CRX__C__TYPE_BLUE_PRINT__DEFINE_GET_BLUE_PRINT(
		Crx_C_Os_FileSystem_Iterator_Entry, crx_c_os_fileSystem_iterator_entry_,
		CRXM__TRUE, CRXM__TRUE,
		CRXM__TRUE, CRXM__TRUE,
		CRXM__FALSE, CRXM__FALSE);

CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_iterator_entry_destruct(
		Crx_C_Os_FileSystem_Iterator_Entry * pThis)
	{crx_c_string_destruct(&(pThis->gPrivate_name));}
CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_iterator_entry_free(
		Crx_C_Os_FileSystem_Iterator_Entry * pThis);
	{free(pThis);}

CRX__LIB__PUBLIC_C_FUNCTION() bool crx_c_os_fileSystem_iterator_entry_copyAssignFrom(
		Crx_C_Os_FileSystem_Iterator_Entry * pThis,
		Crx_C_Os_FileSystem_Iterator_Entry const * CRX_NOT_NULL pEntry)
{
	pThis->gPrivate_flags = pEntry->gPrivate_flags;
	pThis->gPrivate_entryType = pEntry->gPrivate_entryType;
	crx_c_string_copyAssignFrom(&(pThis->gPrivate_name), &(pEntry->gPrivate_name));
	pThis->gPrivate_lengthResidueWhenDirectory = pEntry->gPrivate_lengthResidueWhenDirectory;
	pThis->gPrivate_lengthResidue = pEntry->gPrivate_lengthResidue;
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	pThis->gPrivate_dInoOrFileno = pEntry->gPrivate_dInoOrFileno;
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	pThis->gPrivate_dwFileAttributes = pEntry->gPrivate_dwFileAttributes;
	memcpy(&(pThis->gPrivate_ftLastWriteTime), &(pEntry->gPrivate_ftLastWriteTime), 
			sizeof(FILETIME));
	pThis->gPrivate_nFileSizeHigh = pEntry->gPrivate_nFileSizeHigh;
	pThis->gPrivate_nFileSizeLow = pEntry->gPrivate_nFileSizeLow;
#endif
}

CRX__LIB__PRIVATE_C_FUNCTION() Crx_C_Os_FileSystem_Iterator_EntryType 
		crx_c_os_fileSystem_iterator_entry_getEntryType(
		Crx_C_Os_FileSystem_Iterator_Entry const * pThis)
	{return pThis->gPrivate_entryType;}
CRX__LIB__PRIVATE_C_FUNCTION() Crx_C_String const * crx_c_os_fileSystem_iterator_entry_getName(
		Crx_C_Os_FileSystem_Iterator_Entry const * pThis)
	{return &(pThis->gPrivate_name);}

CRX__LIB__PRIVATE_C_FUNCTION() uint32_t * 
		crx_c_os_fileSystem_iterator_entry_private_getLengthResidueWhenDirectory(
		Crx_C_Os_FileSystem_Iterator_Entry * pThis)
	{return &(pThis->gPrivate_lengthResidueWhenDirectory);}
CRX__LIB__PRIVATE_C_FUNCTION() uint32_t * 
		crx_c_os_fileSystem_iterator_entry_private_getLengthResidue(
		Crx_C_Os_FileSystem_Iterator_Entry * pThis)
	{return &(pThis->gPrivate_lengthResidue);}
CRX__LIB__PRIVATE_C_FUNCTION() Crx_C_String * crx_c_os_fileSystem_iterator_entry_private_getName(
		Crx_C_Os_FileSystem_Iterator_Entry * pThis)
	{return &(pThis->gPrivate_name);}
CRX__LIB__PRIVATE_C_FUNCTION() Crx_C_String * 
		crx_c_os_fileSystem_iterator_entry_private_setEntryType(
		Crx_C_Os_FileSystem_Iterator_Entry * pThis, 
		Crx_C_Os_FileSystem_Iterator_EntryType pEntryType)
	{pThis->gPrivate_entryType = pEntryType;}
CRX__LIB__PRIVATE_C_FUNCTION() Crx_C_String * crx_c_os_fileSystem_iterator_entry_private_raiseFlags(
		Crx_C_Os_FileSystem_Iterator_Entry * pThis, uint32_t pFlags)
	{pThis->gPrivate_flags = pThis->gPrivate_flags | pFlags;}
CRX__LIB__PRIVATE_C_FUNCTION() Crx_C_String * crx_c_os_fileSystem_iterator_entry_private_lowerFlags(
		Crx_C_Os_FileSystem_Iterator_Entry * pThis, uint32_t pFlags)
	{pThis->gPrivate_flags = pThis->gPrivate_flags & ~pFlags;}

#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD))
CRX__LIB__PRIVATE_C_FUNCTION() void crx_c_os_fileSystem_iterator_entry_private_setPosixData(
		Crx_C_Os_FileSystem_Iterator_Entry * pThis, 
		Crx_NonCrxed_Os_Posix_Dll_Libc_Dirent * CRX_NOT_NULL pDirent)
{
	
	#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
			(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	pThis->gPrivate_dInoOrFileno = tStackEntry->gDirent->d_ino;
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD))
	pThis->gPrivate_dInoOrFileno = tStackEntry->gDirent->d_fileno;
	#endif
}
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
CRX__LIB__PRIVATE_C_FUNCTION() void crx_c_os_fileSystem_iterator_entry_private_setWin32Data(
		Crx_C_Os_FileSystem_Iterator_Entry * pThis, 
		Crx_C_Os_FileSystem_Internal_Win32_Win32FindData * CRX_NOT_NULL pWin32FindData)
{
	pThis->gPrivate_dwFileAttributes = gWin32FindData.dwFileAttributes;
	pThis->gPrivate_ftLastWriteTime = gWin32FindData.ftLastWriteTime;
	pThis->gPrivate_nFileSizeHigh = gWin32FindData.nFileSizeHigh;
	pThis->gPrivate_nFileSizeLow = gWin32FindData.nFileSizeLow;
}
#endif


//CLASS: Iterator::Record
CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_iterator_record_construct(
		Crx_C_Os_FileSystem_Iterator_Record * pThis)
{
	crx_c_os_fileSystem_iterator_entry_construct(&(pThis->gPrivate_entry));
	pThis->gPrivate_byteSizeOfPathPrefixInInternalFullPath = 0;
	pThis->gPrivate_byteSizeOfPathPrefixInFullPath = 0;
	crx_c_string_construct(&(pThis->gPrivate_internalFullPath));
	crx_c_string_construct(&(pThis->gPrivate_fullPath));
}
CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_iterator_record_copyConstruct(
		Crx_C_Os_FileSystem_Iterator_Record * pThis,
		Crx_C_Os_FileSystem_Iterator_Record const * CRX_NOT_NULL pRecord)
{
	crx_c_os_fileSystem_iterator_entry_copyConstruct(&(pThis->gPrivate_entry), 
			&(pThis->gPrivate_entry));
	pThis->gPrivate_byteSizeOfPathPrefixInInternalFullPath = 
			pRecord->gPrivate_byteSizeOfPathPrefixInInternalFullPath;
	pThis->gPrivate_byteSizeOfPathPrefixInFullPath = 
			pRecord->gPrivate_byteSizeOfPathPrefixInFullPath;
	crx_c_string_copyConstruct(&(pThis->gPrivate_internalFullPath), 
			&(pThis->gPrivate_internalFullPath));
	crx_c_string_copyConstruct(&(pThis->gPrivate_fullPath),
			&(pThis->gPrivate_fullPath));
}

CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_iterator_record_new()
{
	Crx_C_Os_FileSystem_Iterator_Record * vReturn = 
			((Crx_C_Os_FileSystem_Iterator_Record *)malloc(
					sizeof(Crx_C_Os_FileSystem_Iterator_Record)));

	if(vReturn != NULL)
		{crx_c_os_fileSystem_iterator_record_construct(vReturn);}

	return vReturn;
}
CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_iterator_record_moveNew(
		Crx_C_Os_FileSystem_Iterator_Record * pThis,
		Crx_C_Os_FileSystem_Iterator_Record * CRX_NOT_NULL pEntry)
{
	Crx_C_Os_FileSystem_Iterator_Record * vReturn = ((Crx_C_Os_FileSystem_Iterator_Record *)malloc(
			sizeof(Crx_C_Os_FileSystem_Iterator_Record)));

	if(vReturn != NULL)
		{memcpy(vReturn, pEntry, sizeof(Crx_C_Os_FileSystem_Iterator_Record));}

	return vReturn;
}
CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_iterator_record_copyNew(
		Crx_C_Os_FileSystem_Iterator_Record * pThis,
		Crx_C_Os_FileSystem_Iterator_Record const * CRX_NOT_NULL pEntry)
{
	Crx_C_Os_FileSystem_Iterator_Record * vReturn = 
			((Crx_C_Os_FileSystem_Iterator_Record *)malloc(sizeof(Crx_C_Os_FileSystem_Iterator_Record)));

	if(vReturn != NULL)
		{crx_c_os_fileSystem_iterator_record_copyConstruct(vReturn, pEntry);}

	return vReturn;
}
		
CRX__C__TYPE_BLUE_PRINT__DEFINE_GET_BLUE_PRINT(
		Crx_C_Os_FileSystem_Iterator_Record, crx_c_os_fileSystem_iterator_record_,
		CRXM__TRUE, CRXM__TRUE,
		CRXM__TRUE, CRXM__TRUE,
		CRXM__FALSE, CRXM__FALSE);
		
CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_iterator_record_destruct(
		Crx_C_Os_FileSystem_Iterator_Record * pThis)
{
	crx_c_os_fileSystem_iterator_entry_destruct(&(pThis->gPrivate_entry));
	crx_c_string_destruct(&(pThis->gPrivate_internalFullPath));
	crx_c_string_destruct(&(pThis->gPrivate_fullPath));
}
CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_iterator_record_free(
		Crx_C_Os_FileSystem_Iterator_Record * pThis)
	{free(pThis);}
	
CRX__LIB__PUBLIC_C_FUNCTION() Crx_C_String const * 
		crx_c_os_fileSystem_iterator_record_getInternalFullPath(
		Crx_C_Os_FileSystem_Iterator_Record const * pThis)
	{return &(pThis->gPrivate_internalFullPath);}
CRX__LIB__PUBLIC_C_FUNCTION() Crx_C_String const * 
		crx_c_os_fileSystem_iterator_record_getFullPath(
		Crx_C_Os_FileSystem_Iterator_Record const * pThis)
	{return &(pThis->gPrivate_fullPath);}
CRX__LIB__PUBLIC_C_FUNCTION() Crx_C_Os_FileSystem_Iterator_EntryType 
		crx_c_os_fileSystem_iterator_record_getEntryType(
		Crx_C_Os_FileSystem_Iterator_Record const * pThis)
	{return crx_c_os_fileSystem_iterator_entry_getEntryType(&(pThis->gPrivate_entry));}

//CLASS: Iterator::StackEntry
CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_iterator_private_stackEntry_construct(
		Crx_C_Os_FileSystem_Iterator_Private_StackEntry * pThis)
{
	pThis->gMode = 0;
	pThis->gAdvancingStep = 0;
	pThis->gCurrentStep = 0;
	crx_c_string_construct(&(pThis->gDirectoryPartialPath));
	crx_c_os_fileSystem_iterator_entry_construct(&(pThis->gEntry__current));
	pThis->gLengthResidueWhenDirectory = 0;
	pThis->gLengthResidue = 0;
	pThis->gAllEntries = NULL;
	pThis->gAllEntries_index = 0;
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	//pThis->gFileDescriptor = -1;
	crx_c_os_fileSystem_internal_posix_fileHandle_construct(&(pThis->gFileHandle));
	pThis->gDir = NULL;
	pThis->gDirent = NULL;
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	crx_c_os_fileSystem_internal_win32_win32FindData_construct(&(pThis->gWin32FindData));
	pThis->gHandle = INVALID_HANDLE_VALUE;
#endif
}
CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_iterator_private_stackEntry_copyConstruct(
		Crx_C_Os_FileSystem_Iterator_Private_StackEntry * pThis,
		Crx_C_Os_FileSystem_Iterator_Private_StackEntry const * CRX_NOT_NULL pStackEntry)
	{exit();}

CRX__C__TYPE_BLUE_PRINT__DEFINE_GET_BLUE_PRINT(
		Crx_C_Os_FileSystem_Iterator_Private_StackEntry, 
				crx_c_os_fileSystem_iterator_private_stackEntry_,
		CRXM__TRUE, CRXM__TRUE,
		CRXM__FALSE, CRXM__TRUE,
		CRXM__FALSE, CRXM__FALSE);
	
CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_iterator_private_stackEntry_destruct(
		Crx_C_Os_FileSystem_Iterator_Private_StackEntry * pThis)
{
	crx_c_string_destruct(&(pThis->gDirectoryPartialPath));
	crx_c_os_fileSystem_iterator_entry_destruct(&(pThis->gEntry__current));
	if(pThis->gAllEntries != NULL)
	{
		crx_c_orderedHashTable_destruct(pThis->gAllEntries);
		crx_c_orderedHashTable_free(pThis->gAllEntries);
	}
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	crx_c_os_fileSystem_internal_posix_close(pThis->gFileHandle);
	if(pThis->gDir != NULL)
		{crx_c_os_fileSystem_internal_posix_closedir(pThis->gDir);}
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	crx_c_os_fileSystem_internal_win32_win32FindData_destruct(&(pThis->gWin32FindData));
	crx_c_os_fileSystem_internal_win32_findClose(pThis->gHandle);
#endif
	if(pThis->gEntries1 != NULL)
	{
		crx_c_queue_destruct(pThis->gEntries1);
		crx_c_queue_free(pThis->gEntries1);
	}
	if(pThis->gEntries2 != NULL)
	{
		crx_c_queue_destruct(pThis->gEntries2);
		crx_c_queue_free(pThis->gEntries2);
	}
	if(pThis->gSortedEntries1 != NULL)
	{
		crx_c_sortedQueue_destruct(pThis->gSortedEntries1);
		crx_c_sortedQueue_free(pThis->gSortedEntries1);
	}
	if(pThis->gSortedEntries2 != NULL)
	{
		crx_c_sortedQueue_destruct(pThis->gSortedEntries2);
		crx_c_sortedQueue_free(pThis->gSortedEntries2);
	}
}
CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_iterator_private_stackEntry_free(
		Crx_C_Os_FileSystem_Iterator_Private_StackEntry * pThis)
	{free(pThis);}


//CLASS: MountInfo
CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_mountInfo_construct(
		Crx_C_Os_FileSystem_MountInfo * pThis,
		Crx_C_String const * CRX_NOT_NULL pFullPath, 
		Crx_C_String const * CRX_NOT_NULL pFileSystemName,
		bool pIsLikelyABlockDevice)
{
	crx_c_string_construct(&(pThis->gPrivate_fullPath));
	crx_c_string_construct(&(pThis->gPrivate_fileSystemName));
	pThis->gPrivate_isLikelyABlockDevice = pIsLikelyABlockDevice;
}
CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_mountInfo_copyConstruct(Crx_C_Os_FileSystem_MountInfo * pThis,
		Crx_C_Os_FileSystem_MountInfo const * CRX_NOT_NULL pMountInfo)
{
	crx_c_string_copyConstruct(pThis->gPrivate_fullPath, pMountInfo->gPrivate_fullPath);
	crx_c_string_copyConstruct(pThis->gPrivate_fileSystemName, pMountInfo->gPrivate_fileSystemName);
	pThis->gPrivate_isLikelyABlockDevice = pMountInfo->gPrivate_isLikelyABlockDevice;
}

CRX__LIB__PUBLIC_C_FUNCTION() Crx_C_Os_FileSystem_MountInfo * crx_c_os_fileSystem_mountInfo_new()
{
	Crx_C_Os_FileSystem_MountInfo * vReturn = 
			((Crx_C_Os_FileSystem_MountInfo *)malloc(sizeof(Crx_C_Os_FileSystem_MountInfo)));

	if(vReturn != NULL)
		{crx_c_os_fileSystem_mountInfo_construct(vReturn);}

	return vReturn;
}
CRX__LIB__PUBLIC_C_FUNCTION() Crx_C_Os_FileSystem_MountInfo * crx_c_os_fileSystem_mountInfo_copyNew(
		Crx_C_Os_FileSystem_MountInfo const * CRX_NOT_NULL pMountInfo)
{
	Crx_C_Os_FileSystem_MountInfo * vReturn = 
			((Crx_C_Os_FileSystem_MountInfo *)malloc(sizeof(Crx_C_Os_FileSystem_MountInfo)));

	if(vReturn != NULL)
		{crx_c_os_fileSystem_mountInfo_copyConstruct(vReturn, pMountInfo);}

	return vReturn;
}
CRX__LIB__PUBLIC_C_FUNCTION() Crx_C_Os_FileSystem_MountInfo * crx_c_os_fileSystem_mountInfo_moveNew(
		Crx_C_Os_FileSystem_MountInfo * CRX_NOT_NULL pMountInfo)
{
	Crx_C_Os_FileSystem_MountInfo * vReturn = 
			((Crx_C_Os_FileSystem_MountInfo *)malloc(sizeof(Crx_C_Os_FileSystem_MountInfo)));

	if(vReturn != NULL)
		{memcpy(vReturn, pMountInfo, sizeof(Crx_C_Os_FileSystem_MountInfo));}

	return vReturn;
}

CRX__C__TYPE_BLUE_PRINT__DEFINE_GET_BLUE_PRINT(
		Crx_C_Os_FileSystem_MountInfo, Crx_C_Os_FileSystem_MountInfo_,
		CRXM__TRUE, CRXM__TRUE,
		CRXM__TRUE, CRXM__TRUE,
		CRXM__FALSE, CRXM__FALSE)

CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_mountInfo_set(
		Crx_C_Os_FileSystem_MountInfo * pThis,
		Crx_C_String const * CRX_NOT_NULL pFullPath, 
		Crx_C_String const * CRX_NOT_NULL pFileSystemName,
		int32_t pOffsetToUtc,
		bool pIsLikelyABlockDevice)
{
	crx_c_string_empty(&(pThis->gPrivate_fullPath));
	crx_c_string_appendString(&(pThis->gPrivate_fullPath), pFullPath);
	
	if(!crx_c_string_isEndingWith3(&(pThis->gPrivate_fullPath), 
			CRX__OS__FILE_SYSTEM__SEPERATOR_STRING, 1, false))
	{
		crx_c_string_appendCString(&(pThis->gPrivate_fullPath), 
				CRX__OS__FILE_SYSTEM__SEPERATOR_STRING);
	}
	
	crx_c_string_empty(&(pThis->gPrivate_fileSystemName));
	crx_c_string_appendString(&(pThis->gPrivate_fullPath), pFileSystemName);
	
	crx_c_string_upperTheCase(&(pThis->gPrivate_fileSystemName));
	
	pThis->gPrivate_offsetToUtc = pOffsetToUtc;
	
	pThis->gPrivate_isLikelyABlockDevice = pIsLikelyABlockDevice;
}
CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_mountInfo_set2(
		Crx_C_Os_FileSystem_MountInfo * pThis,
		Crx_C_String const * CRX_NOT_NULL pFullPath, 
		Crx_C_String const * CRX_NOT_NULL pFileSystemName,
		int32_t pOffsetToUtc)
{
	crx_c_string_empty(&(pThis->gPrivate_fullPath));
	crx_c_string_appendString(&(pThis->gPrivate_fullPath), pFullPath);
	crx_c_string_empty(&(pThis->gPrivate_fileSystemName));
	crx_c_string_appendString(&(pThis->gPrivate_fullPath), pFileSystemName);

	crx_c_string_upperTheCase(&(pThis->gPrivate_fileSystemName));

	pThis->gPrivate_offsetToUtc = pOffsetToUtc;

	/*REUIQRES REVISION OF THE ACTUAL NAMES*/
	pThis->gPrivate_isLikelyABlockDevice = 
			(crx_c_string_isEqualTo2(&(pThis->gPrivate_fileSystemName), "EXT4", false) ||
			crx_c_string_isEqualTo2(&(pThis->gPrivate_fileSystemName), "EXT3", false) ||
			crx_c_string_isEqualTo2(&(pThis->gPrivate_fileSystemName), "EXT2", false) || 
			crx_c_string_isEqualTo2(&(pThis->gPrivate_fileSystemName), "FAT32", false) ||
			crx_c_string_isEqualTo2(&(pThis->gPrivate_fileSystemName), "FAT16", false) ||
			crx_c_string_isEqualTo2(&(pThis->gPrivate_fileSystemName), "EXFAT", false) ||
			crx_c_string_isEqualTo2(&(pThis->gPrivate_fileSystemName), "NTFS", false) ||
			crx_c_string_isEqualTo2(&(pThis->gPrivate_fileSystemName), "HFS", false) ||
			crx_c_string_isEqualTo2(&(pThis->gPrivate_fileSystemName), "APFS", false));
}

CRX__LIB__PUBLIC_C_FUNCTION() Crx_C_String const * 
		crx_c_os_fileSystem_mountInfo_getFullPathOfMountPoint(
		Crx_C_Os_FileSystem_MountInfo const * CRX_NOT_NULL pThis)
	{return &(pThis->gPrivate_fullPath);}
CRX__LIB__PUBLIC_C_FUNCTION() Crx_C_String const * 
		crx_c_os_fileSystem_mountInfo_getFileSystemName(
		Crx_C_Os_FileSystem_MountInfo const * CRX_NOT_NULL pThis)
	{return &(pThis->gPrivate_fileSystemName);}
CRX__LIB__PUBLIC_C_FUNCTION() bool crx_c_os_fileSystem_mountInfo_isLikelyABlockDevice(
		Crx_C_Os_FileSystem_MountInfo const * CRX_NOT_NULL pThis)
	{return pThis->gPrivate_isLikelyABlockDevice;}
CRX__LIB__PUBLIC_C_FUNCTION() int32_t crx_c_os_fileSystem_mountInfo_getOffsetToUtc(
		Crx_C_Os_FileSystem_MountInfo const * CRX_NOT_NULL pThis)
	{return pThis->gPrivate_offsetToUtc;}

CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_mountInfo_destruct(
		Crx_C_Os_FileSystem_MountInfo * pThis)
{
	crx_c_string_destruct(pThis->gPrivate_fullPath);
	crx_c_string_destruct(pThis->gPrivate_fileSystemName);
}
CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_mountInfo_free(
		Crx_C_Os_FileSystem_MountInfo * pThis)
	{free(pThis);}

CRX__LIB__PUBLIC_C_FUNCTION() bool crx_c_os_fileSystem_mountInfo_copyAssignFrom(
		Crx_C_Os_FileSystem_MountInfo * pThis,
		Crx_C_Os_FileSystem_MountInfo const * CRX_NOT_NULL pMountInfo)
{
	if(crx_c_string_copyAssignFrom(&(pThis->gPrivate_fullPath), 
					&(pMountInfo->gPrivate_fullPath)) &&
			crx_c_string_copyAssignFrom(&(pThis->gPrivate_fileSystemName), 
					&(pMountInfo->gPrivate_fileSystemName)))
	{					
		pThis->gPrivate_isLikelyABlockDevice = pMountInfo->gPrivate_isLikelyABlockDevice;
		pThis->gPrivate_offsetToUtc = pMountInfo->gPrivate_offsetToUtc;
		
		return true;
	}
	else
	{
		crx_c_string_empty(&(pThis->gPrivate_fullPath));
		crx_c_string_empty(&(pThis->gPrivate_fileSystemName));
		pThis->gPrivate_isLikelyABlockDevice = false;
		pThis->gPrivate_offsetToUtc = 0;

		return false;
	}
}




CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_io_construct(Crx_C_Os_FileSystem_Io * pThis)
{
#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	SYSTEM_INFO vSystemInfo /*= ?*/;
#endif
	
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	//FORMAL VIOLATION IN ASSIGNING CONSTANT.
	if(!crx_c_os_fileSystem_private_posix_sysconf(&(pThis->gPRIVATE__REGION_ALIGNMENT_VALUE),
			CRX__C__OS__FILE_SYSTEM__SC__PAGE_SIZE))
		{pThis->gPRIVATE__REGION_ALIGNMENT_VALUE = 4096;}
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	GetSystemInfo(&vSystemInfo);

	pThis->gPRIVATE__REGION_ALIGNMENT_VALUE = vSystemInfo.dwAllocationGranularity;
#endif
	pThis->gPrivate_isMemoryMappingPrefered = false;
	pThis->gPrivate_isRwLockInitialized = false;
	pThis->gAccessorCount = 0;
	pThis->gPrivate_fileAccessMode = CRX__C__OS__FILE_SYSTEM__FILE_ACCESS_MODE__NULL;
	pThis->gPrivate_fileShareMode = CRX__C__OS__FILE_SYSTEM__FILE_SHARE_MODE__NULL;
	pThis->gPrivate_osFileHandle = CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE;
	pThis->gPrivate_osFileHandle__2 = CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE;
#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	pThis->gPrivate_mappingHandle = NULL;
#endif
	crx_c_string_construct(&(pThis->gPrivate_internalFullPath));
	pThis->gPrivate_expectedByteSize = 0;
	pThis->gPrivate_hiddenByteSize = 0;
	pThis->gPrivate_maximumTraversedByteSize = 0;
	pThis->gPrivate_numberOfActiveSequentialRequests = 0;
	crx_c_hashTable_construct(&(pThis->gPrivate_infoOfSequentials), 
			crx_c_getTypeBluePrintForUint32(), 
			crx_c_os_fileSystem_io_infoOfSequential_getTypeBluePrintForPointer(), 
			&crx_c_hashTable_areUint32sEqual, &crx_c_hashTable_getHashKeyForUint32);
	//REMEMBER, "void *", "char *" "unsigned char *", AND "signed char *" HAVE THE SAME 
	//		REPRESENTATION AND ALIGNMENT REQUIREMENT IN C89 WHICH IS WHY WE CAN USE A VOID POINTER
	//		TYPE BLUE PRINT WITHOUT ANY SPECIAL TREATMENT.
	crx_c_tree_construct13(&(pThis->gPrivate_regionsByAddress), 
			crx_c_os_fileSystem_io_region_getTypeBluePrintForPointer(),
			crx_c_getTypeBluePrintForVoidPointer(), 
			CRX__C__TREE__FLAG__CONSERVATIVE_IN_GROWTH, 
			&crx_c_os_fileSystem_io_getOrderOfRegions1,
			&crx_c_os_fileSystem_io_getOrderOfUnsignedCharPointers,
			&crx_c_os_fileSystem_io_constructIndexFromRegion,
			2)
	crx_c_tree_construct13(&(pThis->gPrivate_regionsByOffset),
			crx_c_os_fileSystem_io_region_getTypeBluePrintForPointer(),
			crx_c_getTypeBluePrintForUint64(),
			CRX__C__TREE__FLAG__CONSERVATIVE_IN_GROWTH,
			&crx_c_os_fileSystem_io_getOrderOfRegions2,
			&crx_c_os_fileSystem_io_getOrderOfUint64,
			&crx_c_os_fileSystem_io_constructIndexFromRegion2,
			2);
	crx_c_tree_construct13(&(pThis->gPrivate_portions__locked),
			crx_c_os_fileSystem_io_portion_getTypeBluePrintForPointer(),
			crx_c_getTypeBluePrintForUint64(), 
			CRX__C__TREE__FLAG__CONSERVATIVE_IN_GROWTH,
			&crx_c_os_fileSystem_io_getOrderOfPortions,
			&crx_c_os_fileSystem_io_getOrderOfUint64,
			&crx_c_os_fileSystem_io_constructIndexFromPortion,
			2);

	if(crx_c_pthread_rwlock_crx_init(&(pThis->gPrivate_rwLock)) == 0)
	{
		if(crx_c_pthread_rwlock_crx_init(&(pThis->gPrivate_rwLock__lockedPortions)) == 0)
			{pThis->gPrivate_isRwLockInitialized = true;}
		else
			{crx_c_pthread_rwlock_destroy(&(pThis->gPrivate_rwLock));}
	}

	if(pThis->gPrivate_isRwLockInitialized)
		{CRX__SET_TO_ZERO(Crx_C_Pthread_RawId, pThis->gPrivate_rawId);}
	else
		{crx_c_pthread_crx_getRawThreadId(&(pThis->gPrivate_rawId));}
}
CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_io_copyConstruct(
		Crx_C_Os_FileSystem_Io * pThis, Crx_C_Os_FileSystem_Io const * CRX_NOT_NULL pIo)
	{exit();}

CRX__LIB__PUBLIC_C_FUNCTION() Crx_C_Os_FileSystem_Io * crx_c_os_fileSystem_io_new()
{
	Crx_C_Os_FileSystem_Io * vReturn = ((Crx_C_Os_FileSystem_Io *)malloc(
			sizeof(Crx_C_Os_FileSystem_Io)));

	if(vReturn != NULL)
		{crx_c_os_fileSystem_io_construct(vReturn);}

	return vReturn;
}
CRX__LIB__PUBLIC_C_FUNCTION() Crx_C_Os_FileSystem_Io * crx_c_os_fileSystem_io_copyNew(
		Crx_C_Os_FileSystem_Io const * CRX_NOT_NULL pIo)
	{exit();}
CRX__LIB__PUBLIC_C_FUNCTION() Crx_C_Os_FileSystem_Io * crx_c_os_fileSystem_io_moveNew(
		Crx_C_Os_FileSystem_Io * CRX_NOT_NULL pIo)
{
	Crx_C_Os_FileSystem_Io * vReturn = ((Crx_C_Os_FileSystem_Io *)malloc(
			sizeof(Crx_C_Os_FileSystem_Io)));

	if(vReturn != NULL)
		{memcpy(vReturn, pIo, sizeof(Crx_C_Os_FileSystem_Io));}

	return vReturn;
}

CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_io_destruct(
		Crx_C_Os_FileSystem_Io * pThis)
{
	size_t vIndex = crx_c_hashTable_getStartIndex(&(pThis->gPrivate_infoOfSequentials));
	Crx_C_Tree_Iterator vIterator /*= ?*/;

	crx_c_tree_iterator_construct(&vIterator, &(pThis->gPrivate_regionsByAddress));
	
	crx_c_string_destruct(&(pThis->gPrivate_internalFullPath));
	
	while(vIndex != HashTable::getEndIndex())
	{
		Crx_C_Os_FileSystem_Io_InfoOfSequential * const tInfoOfSequential = 
				*((Crx_C_Os_FileSystem_Io_InfoOfSequential * const *)
				crx_c_hashTable_getElementFromIndex(&(pThis->gPrivate_infoOfSequentials), vIndex));

		crx_c_os_fileSystem_io_infoOfSequential_destruct(tInfoOfSequential);
		crx_c_os_fileSystem_io_infoOfSequential_free(tInfoOfSequential);

		vIndex = HashTable::getNextIndex();
	}

	crx_c_tree_iterator_reset(&vIterator);
	while(crx_c_tree_iterator_isValid(&vIterator))
	{
		Crx_C_Os_FileSystem_Io_Region * const tRegion = 
				*((Crx_C_Os_FileSystem_Io_Region * const *)crx_c_tree_iterator_get(&vIterator));

		crx_c_os_fileSystem_io_region_free(tRegion);
	}
	crx_c_tree_destruct(&(pThis->gPrivate_regionsByOffset));
	crx_c_tree_destruct(&(pThis->gPrivate_regionsByAddress));
	
	
	//WE DO NOT NEED TO FREE SPACE WITH THIS BECAUSE THE PORTIONS ARE PART OF THE InfoOfSequential
	//		AND Regions INTANCES THAT Io IS HOLDING.
	crx_c_tree_destruct(&(pThis->gPrivate_portions__locked));
	
	if(pThis->gPrivate_isRwLockInitialized)
	{
		int32_t tResult = (crx_c_pthread_thread_crx_isSelfAPthread() ? 
				crx_c_pthread_rwlock_destroy(&(pThis->gPrivate_rwLock)) : -1);
		int32_t tResult2 = (crx_c_pthread_thread_crx_isSelfAPthread() ? 
				crx_c_pthread_rwlock_destroy(&(pThis->gPrivate_rwLock__lockedPortions)) : -1);

		assert((tResult == 0) && (tResult2 == 0));
	}
}

CRX__LIB__PUBLIC_C_FUNCTION() bool
		crx_c_os_fileSystem_io_private_unsafeCheckIfRegionsInRangeAndOptionallyMarkThem(
		Crx_C_Os_FileSystem_Io * pThis, uint64_t pStartOffset, uint64_t pExclusiveEndOffset, 
		bool * pIsNoError, Crx_C_Array/*<Crx_C_Os_FileSystem_Io_Region *>*/ pRegions)
{
	Crx_C_Tree_Iterator vIterator /*= ?*/;
	Crx_C_Os_FileSystem_Io_Region vRegion /*= ?*/;
	bool vIsNoError = true;
	bool vReturn = false;

	crx_c_tree_iterator_construct(&vIterator, &(pThis->gPrivate_regionsByOffset));
	CRX__SET_TO_ZERO(Crx_C_Os_FileSystem_Io_Region, &vRegion);
	
	/*
	OVERLLAPING REGIONS ARE NO LONGER ALLOWED PER THE ORIGINAL DESIGN.
	
	if(pStartOffset < CRX__OS__FILE_SYSTEM__MAXIMUM_REGION_SIZE)
		{crx_c_tree_iterator_reset(&vIterator);}
	else
	{
		Crx_C_Os_FileSystem_Io_Region tRegion /*= ?* /;
		
		CRX__SET_TO_ZERO(Crx_C_Os_FileSystem_Io_Region, &tRegion);

		tRegion.gBaseOffset = pStartOffset - CRX__OS__FILE_SYSTEM__MAXIMUM_REGION_SIZE;
		crx_c_tree_iterator_setToPositionOf(&vIterator, &tRegion, 
				CRX__C__TREE__EDGE__LEFT);

		/*while(crx_c_tree_iterator_isValid(&vIterator))
		{
			Crx_C_Os_FileSystem_Io_Region * tRegion2 = 
					*((Crx_C_Os_FileSystem_Io_Region * *)crx_c_tree_iterator_unsafeGet(&vIterator));
					
			if((tRegion2->gBaseOffset + CRX__OS__FILE_SYSTEM__MAXIMUM_REGION_SIZE) <= 
					pStartOffset)
				{crx_c_tree_iterator_prev(&vIterator);}
			else
				{break;}
		}* /
	}*/

	vRegion.gBaseOffset = pStartOffset;
	crx_c_tree_iterator_setToPositionOf(&vIterator, &vRegion, CRX__C__TREE__EDGE__LEFT);
	
	if(crx_c_tree_iterator_isValid(&vIterator))
		{crx_c_tree_iterator_next(&vIterator);}
	else
		{crx_c_tree_iterator_reset(&vIterator);}

	while(crx_c_tree_iterator_isValid(&vIterator))
	{
		Crx_C_Os_FileSystem_Io_Region * vRegion = 
				*((Crx_C_Os_FileSystem_Io_Region * *)crx_c_tree_iterator_unsafeGet(&vIterator));

		if(!(((vRegion->gBaseOffset < pStartOffset) && 
						((vRegion->gBaseOffset + vRegion->gLength) < 
						pStartOffset)) ||
				((vRegion->gBaseOffset > pExclusiveEndOffset) && 
						((vRegion->gBaseOffset + vRegion->gLength) > pExclusiveEndOffset))))
		{
			vReturn = true;

			if(pRegions__marked != NULL)
			{
				vIsNoError = crx_c_array_push(&pRegions__marked, &vRegion);

				if(vIsNoError && crx_c_pthread_rwlock_wrlock(
						&(pThis->gPrivate_rwLock__lockedPortions)) == 0)
				{
					vRegion->gNumberOfHeldPointers = vRegion->gNumberOfHeldPointers + 1;

					crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock__lockedPortions));
				}
				else
					{vIsNoError = false;}
			}
		}
		
		if((!vReturn || (pRegions__marked != NULL)) && vIsNoError && 
				(vRegion->gBaseOffset < pExclusiveEndOffset))
			{crx_c_tree_iterator_next(&vIterator);}
		else
			{break;}
	}
	
	if(pIsNoError != NULL)
		{*pIsNoError = vIsNoError;}

	crx_c_tree_iterator_destruct(&vIterator);
	
	return vReturn;
}
CRX__LIB__PRIVATE_C_FUNCTION() Crx_C_Os_FileSystem_Io_Region *
		crx_c_os_fileSystem_io_private_unsafeGetTheRegionThatFullyEncapsulatesTheSpan(
		Crx_C_Os_FileSystem_Io * pThis, uint64_t pStartOffset, uint64_t pExclusiveEndOffset)
{
	/*OVERLLAPING REGIONS ARE NO LONGER ALLOWED PER THE ORIGINAL DESIGN.*/
	Crx_C_Tree_Iterator vIterator /*= ?*/;
	Crx_C_Os_FileSystem_Io_Region vRegion /*= ?*/;
	Crx_C_Os_FileSystem_Io_Region * vReturn = NULL;

	crx_c_tree_iterator_construct(&vIterator, &(pThis->gPrivate_regionsByOffset));
	CRX__SET_TO_ZERO(Crx_C_Os_FileSystem_Io_Region, &vRegion);
	
	
	vRegion.gBaseOffset = pStartOffset;
	crx_c_tree_iterator_setToPositionOf(&vIterator, &vRegion, CRX__C__TREE__EDGE__LEFT);
	
	if(crx_c_tree_iterator_isValid(&vIterator))
	{
		Crx_C_Os_FileSystem_Io_Region * tRegion = 
				*((Crx_C_Os_FileSystem_Io_Region * *)crx_c_tree_iterator_unsafeGet(&vIterator));

		if((tRegion->gBaseOffset >= pStartOffset) && 
				((tRegion->gBaseOffset + tRegion->gLength) <= pExclusiveEndOffset))
			{vReturn = tRegion;}
	}

	crx_c_tree_iterator_destruct(&vIterator);
	
	return vReturn;
}
CRX__LIB__PRIVATE_C_FUNCTION() Crx_C_Os_FileSystem_Io_Region *
		crx_c_os_fileSystem_io_private_unsafeGetTheRegionThatContainsTheAddress(
		Crx_C_Os_FileSystem_Io * pThis, void * pAddress)
{
	/*OVERLLAPING REGIONS ARE NO LONGER ALLOWED PER THE ORIGINAL DESIGN.*/
	Crx_C_Tree_Iterator vIterator /*= ?*/;
	Crx_C_Os_FileSystem_Io_Region vRegion /*= ?*/;
	Crx_C_Os_FileSystem_Io_Region * vReturn = NULL;

	crx_c_tree_iterator_construct(&vIterator, &(pThis->gPrivate_regionsByAddress));
	CRX__SET_TO_ZERO(Crx_C_Os_FileSystem_Io_Region, &vRegion);
	
	
	vRegion.gBaseAddress = ((unsigned char *)pAddress);
	crx_c_tree_iterator_setToPositionOf(&vIterator, &vRegion, CRX__C__TREE__EDGE__LEFT);
	
	if(crx_c_tree_iterator_isValid(&vIterator))
	{
		Crx_C_Os_FileSystem_Io_Region * tRegion = 
				*((Crx_C_Os_FileSystem_Io_Region * *)crx_c_tree_iterator_unsafeGet(&vIterator));

		if((tRegion->gBaseAddress <= pStartOffset) && 
				(pStartOffset <= (tRegion->gBaseAddress + ((size_t)tRegion->gLength))))
			{vReturn = tRegion;}
	}

	crx_c_tree_iterator_destruct(&vIterator);
	
	return vReturn;
}


CRX__LIB__PUBLIC_C_FUNCTION() int32_t crx_c_os_fileSystem_io_private_patientlyLockForReadRwLock(
		Crx_C_Os_FileSystem_Io * pThis, Crx_C_Pthread_RwLock * CRX_NOT_NULL pRwLock)
{
	uint32_t vUnlockCount = 0;
	int32_t vReturn = crx_c_pthread_rwlock_rdlock(pRwLock);

	while((vReturn == EAGAIN) && (vUnlockCount < 4))
	{
		crx_c_pthread_sched_crx_runOtherThreadsInstead();
		vReturn = crx_c_pthread_rwlock_rdlock(pRwLock);
		vUnlockCount = vUnlockCount + 1;
	}
	
	return vReturn;
}
CRX__LIB__PUBLIC_C_FUNCTION() int32_t crx_c_os_fileSystem_io_private_patientlyLockForWriteRwLock(
		Crx_C_Os_FileSystem_Io * pThis, Crx_C_Pthread_RwLock * CRX_NOT_NULL pRwLock)
{
	uint32_t vUnlockCount = 0;
	int32_t vReturn = crx_c_pthread_rwlock_wrlock(pRwLock);

	while((vReturn == EAGAIN) && (vUnlockCount < 4))
	{
		crx_c_pthread_sched_crx_runOtherThreadsInstead();
		vReturn = crx_c_pthread_rwlock_wrlock(pRwLock);
		vUnlockCount = vUnlockCount + 1;
	}
	
	return vReturn;
}

CRX__LIB__PUBLIC_C_FUNCTION() bool crx_c_os_fileSystem_io_private_unsafeGetIsExtentLocked(
		Crx_C_Os_FileSystem_Io * pThis, uint64_t pStartOffset, uint64_t pExclusiveEndOffset,
		bool * pIsNoError)
{
	if(!(pThis->gPrivate_isRwLockInitialized) || 
			crx_c_pthread_rwlock_rdlock(&(pThis->gPrivate_rwLock__lockedPortions)) == 0)
	{
		Crx_C_Tree_Iterator tIterator /*= ?*/;
		bool tReturn = false;

		crx_c_tree_iterator_construct(&tIterator, &(pThis->gPrivate_portions__locked));

		/*
			WHILE REGIONS ARE NO LONGER ALLOWED TO OVERLAP PER THE ORIGINAL DESIGN, LOCKED 'REGIONS'
					ARE STILL ALLOWED TO OVERLAP BECAUSE THEY DO NOT AMOUNT TO ANY ACTUAL MEMORY
					MAPPING
		*/
		if(pStartOffset < CRX__OS__FILE_SYSTEM__MAXIMUM_REGION_SIZE)
			{crx_c_tree_iterator_reset(&tIterator);}
		else
		{
			Crx_C_Os_FileSystem_Io_Portion tPortion /*= ?*/;
			
			CRX__SET_TO_ZERO(Crx_C_Os_FileSystem_Io_Portion, &tPortion);

			tPortion.gBaseOffset = pStartOffset - CRX__OS__FILE_SYSTEM__MAXIMUM_REGION_SIZE;
			crx_c_tree_iterator_setToPositionOf(&tIterator, &tPortion, 
					CRX__C__TREE__EDGE__LEFT);
		}

		if(crx_c_tree_iterator_isValid(&tIterator))
			{crx_c_tree_iterator_next(&tIterator);}
		else
			{crx_c_tree_iterator_reset(&tIterator);}

		while(crx_c_tree_iterator_isValid(&tIterator))
		{
			Crx_C_Os_FileSystem_Io_Portion * tPortion = *((Crx_C_Os_FileSystem_Io_Portion * *)
					crx_c_tree_iterator_unsafeGet(&tIterator));

			if(!(((tPortion->gBaseOffset < pStartOffset) && 
							((tPortion->gBaseOffset + tPortion->gLength) < 
							pStartOffset)) ||
					((tPortion->gBaseOffset > pExclusiveEndOffset) && 
							((tPortion->gBaseOffset + tPortion->gLength) > pExclusiveEndOffset))))
				{tReturn = true;}
			
			if(!tReturn && (tPortion->gBaseOffset < pExclusiveEndOffset))
				{crx_c_tree_iterator_next(&tIterator);}
			else
				{break;}
		}

		crx_c_tree_iterator_destruct(&tIterator);
		
		return tReturn;
	}
	else
	{
		if(pIsNoError != NULL)
			{*pIsNoError = false;}

		return true;
	}
}

/*
	WARNING: 
		- THIS FUNCTION USES AS WRTIE LOCK, THE LOCK Io::gPrivate_rwLock.
		- THE HANDLE RETURNED IS NOT SAFE TO USE WITHOUT A LOCK. THIS MEANS THAT THE HANDLE MUST BE
				REFERED TO DIRECTLY USING pThis, WHILE IN THE LOCK.
*/
CRX__LIB__PRIVATE_C_FUNCTION() Crx_C_Os_FileSystem_OsFileHandle 
		crx_c_os_fileSystem_io_private_prepareAndGetFileHandleForSequentialAccess(
		Crx_C_Os_FileSystem_Io * pThis)
{
	if(pThis->gPrivate_osFileHandle != CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE)
		{return pThis->gPrivate_osFileHandle;}
	else if(pThis->gPrivate_osFileHandle__2 != CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE)
	{
		if(!(pThis->gPrivate_isRwLockInitialized) ? crx_c_pthread_crx_isSameThreadAs(
				&(pThis->gPrivate_rawId)) : 
				(crx_c_pthread_rwlock_wrlock(&(pThis->gPrivate_rwLock)) == 0))
		{
			if((pThis->gPrivate_osFileHandle == CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE) &&
					(pThis->gPrivate_osFileHandle__2 != 
					CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE))
			{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD))
				/*
					ARTIFICIALLY IMPOSING THE SAME RESTRICTION REQUIRED FOR WINDOWS 95.
					IF WE ARE HERE, EITHER ACCESS IS EXCLUSIVE, OR THE FILE WAS OPENED USING A
					RECORD
				*/
				if((crx_c_tree_getSize(&(pThis->gPrivate_regionsByAddress)) == 0) &&
						(pThis->gPrivate_numberOfActiveRegionCreations == 0))
				{
					pThis->gPrivate_osFileHandle = pThis->gPrivate_osFileHandle__2;
					pThis->gPrivate_osFileHandle__2 = CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE;
				}
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
				//FAVOR KEEPING CURRENT MAPPING OPEN.
				if((pThis->gPrivate_fileShareMode == 
						CRX__C__OS__FILE_SYSTEM__FILE_SHARE_MODE__EXCLUSIVE) || 
						crx_c_string_isEmpty(&(pThis->gPrivate_internalFullPath)))
				{
					if((crx_c_tree_getSize(&(pThis->gPrivate_regionsByAddress)) == 0) &&
							(pThis->gPrivate_numberOfActiveRegionCreations == 0))
					{
						if(pThis->gPrivate_mappingHandle != NULL)
						{
							CloseHandle(pThis->gPrivate_mappingHandle);
							pThis->gPrivate_mappingHandle = NULL;
						}

						pThis->gPrivate_osFileHandle = pThis->gPrivate_osFileHandle__2;
						pThis->gPrivate_osFileHandle__2 = 
								CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE;
					}
				}
				else
				{
					Crx_C_Os_FileSystem_Contract tContract /*= ?*/

					//FORMAL ERROR
					CRX__SET_TO_ZERO(Crx_C_Os_FileSystem_Contract, tContract);
					{
						tContract.gCharacterSet = CRX__C__OS__INFO__CHARACTER_SET__UNICODE;
						tContract.gGuaranteeLevel = 1;
						tContract.gFileSystemGuaranteeLevel = 1;
						tContract.gFileSystemRuntimeGuaranteeLevel = 1;
						tContract.gResolutionModel = CRX__C__OS__FILE_SYSTEM__RESOLUTION_MODEL__PRECISE;
					}

					pThis->gPrivate_osFileHandle = crx_c_os_fileSystem_internal_doOpen(
							&(pThis->gPrivate_internalFullPath), ((size_t)(-1)), NULL, 
							CRX__C__OS__FILE_SYSTEM__FILE_OPEN_MODE__OPEN, 
							pThis->gPrivate_fileAccessMode, pThis->gPrivate_fileShareMode,
							&tContract, NULL);
				}
#endif
			}

			if(pThis->gPrivate_isRwLockInitialized)
				{crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock));}
		}

		return pThis->gPrivate_osFileHandle;
	}
	else
		{return INVALID_HANDLE_VALUE;}
}
/*
	WARNING: 
		- THE HANDLE RETURNED IS NOT SAFE TO USE WITHOUT A LOCK. THIS MEANS THAT THE HANDLE MUST BE
				REFERED TO DIRECTLY USING pThis, WHILE IN THE LOCK.
		- THIS FUNCTION USES THE LOCK Io::gPrivate_rwLock.
*/
CRX__LIB__PRIVATE_C_FUNCTION() Crx_C_Os_FileSystem_OsFileHandle 
		crx_c_os_fileSystem_io_private_prepareAndGetFileHandleForRegionalAccess(
		Crx_C_Os_FileSystem_Io * pThis)
{
	if(pThis->gPrivate_fileAccessMode == CRX__C__OS__FILE_SYSTEM__FILE_ACCESS_MODE__WRITE_ONLY)
		{return INVALID_HANDLE_VALUE;}

	if(pThis->gPrivate_osFileHandle__2 != CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE)
		{return pThis->gPrivate_osFileHandle__2;}
	else if(pThis->gPrivate_osFileHandle != CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE)
	{
		if(!(pThis->gPrivate_isRwLockInitialized) ? crx_c_pthread_crx_isSameThreadAs(
				&(pThis->gPrivate_rawId)) : 
				(crx_c_pthread_rwlock_wrlock(&(pThis->gPrivate_rwLock)) == 0))
		{
			if((pThis->gPrivate_osFileHandle__2 == CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE) &&
					(pThis->gPrivate_osFileHandle != CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE))
			{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD))
				/*
					ARTIFICIALLY IMPOSING THE SAME RESTRICTION REQUIRED FOR WINDOWS 95.
					IF WE ARE HERE, EITHER ACCESS IS EXCLUSIVE, OR THE FILE WAS OPENED USING A
					RECORD
				*/
				if(crx_c_tree_getSize(&(pThis->gPrivate_portions__locked)) == 0)
				{
					if((crx_c_hashTable_getLength(&(pThis->gPrivate_infoOfSequentials)) == 0) ||
							(pThis->gPrivate_numberOfActiveSequentialRequests == 0))
					{
						pThis->gPrivate_osFileHandle__2 = pThis->gPrivate_osFileHandle;
						pThis->gPrivate_osFileHandle = CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE;
					}
				}
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
				/*if((crx_c_hashTable_getLength(&(pThis->gPrivate_infoOfSequentials)) == 0) ||
						(((pThis->gPrivate_fileShareMode == 
								CRX__C__OS__FILE_SYSTEM__FILE_SHARE_MODE__EXCLUSIVE) || 
								crx_c_string_isEmpty(&(pThis->gPrivate_internalFullPath)))) &&
								(pThis->gPrivate_numberOfActiveSequentialRequests == 0))*/
				if((pThis->gPrivate_fileShareMode == 
						CRX__C__OS__FILE_SYSTEM__FILE_SHARE_MODE__EXCLUSIVE) || 
						crx_c_string_isEmpty(&(pThis->gPrivate_internalFullPath)))
				{
					if((crx_c_hashTable_getLength(&(pThis->gPrivate_infoOfSequentials)) == 0) ||
							(pThis->gPrivate_numberOfActiveSequentialRequests == 0))
					{
						pThis->gPrivate_osFileHandle__2 = pThis->gPrivate_osFileHandle;
						pThis->gPrivate_osFileHandle = CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE;
					}
				}
				else
				{
					Crx_C_Os_FileSystem_Contract tContract /*= ?*/

					//FORMAL ERROR
					CRX__SET_TO_ZERO(Crx_C_Os_FileSystem_Contract, tContract);
					{
						tContract.gCharacterSet = CRX__C__OS__INFO__CHARACTER_SET__UNICODE;
						tContract.gGuaranteeLevel = 1;
						tContract.gFileSystemGuaranteeLevel = 1;
						tContract.gFileSystemRuntimeGuaranteeLevel = 1;
						tContract.gResolutionModel = 
								CRX__C__OS__FILE_SYSTEM__RESOLUTION_MODEL__PRECISE;
					}

					pThis->gPrivate_osFileHandle__2 = crx_c_os_fileSystem_internal_doOpen(
							&(pThis->gPrivate_internalFullPath), ((size_t)(-1)), NULL, 
							CRX__C__OS__FILE_SYSTEM__FILE_OPEN_MODE__OPEN, 
							pThis->gPrivate_fileAccessMode, pThis->gPrivate_fileShareMode,
							&tContract, NULL);
				}

				/*if(pThis->gPrivate_osFileHandle__2 != CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE)
				{
					pThis->gPrivate_mappingHandle = 
							crx_c_os_fileSystem_private_win32_doCreateFileMapping(
							&(pThis->gPrivate_osFileHandle__2), ((pThis->gPrivate_fileAccessMode == 
							CRX__C__OS__FILE_SYSTEM__FILE_ACCESS_MODE__READ_ONLY) ? PAGE_READONLY :
							PAGE_READWRITE), ((pThis->gPrivate_expectedByteSize >=
							pThis->gPrivate_maximumTraversedByteSize) ? 
							pThis->gPrivate_expectedByteSize : 
							pThis->gPrivate_maximumTraversedByteSize), true);

					if(pThis->gPrivate_mappingHandle == NULL)
					{
						if(pThis->gPrivate_osFileHandle == 
								CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE)
						{
							pThis->gPrivate_osFileHandle = pThis->gPrivate_osFileHandle__2;
							pThis->gPrivate_osFileHandle__2 = 
									CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE;
						}
						else
						{
							crx_c_os_fileSystem_close(pThis->gPrivate_osFileHandle__2);
							pThis->gPrivate_osFileHandle__2 = 
									CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE;
						}
					}
				}*/
#endif
			}

			if(pThis->gPrivate_isRwLockInitialized)
				{crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock));}
		}

		return pThis->gPrivate_osFileHandle__2;
	}
	else
		{return INVALID_HANDLE_VALUE;}
}

/*
	IF pExpectedFileByteSize  IS 0, THE SIZE IS ASSUMED THE CURRENT FILE SIZE.
*/
CRX__LIB__PUBLIC_C_FUNCTION() bool crx_c_os_fileSystem_io_open(Crx_C_Os_FileSystem_Io * pThis
		Crx_C_String const * CRX_NOT_NULL pFileFullPath, uint32_t pDepthLimitOfReparsePoints,
		Crx_C_Os_FileSystem_FileOpenMode pFileOpenMode,
		Crx_C_Os_FileSystem_FileAccessMode pFileAccessMode,
		Crx_C_Os_FileSystem_FileShareMode pFileShareMode,
		Crx_C_Os_FileSystem_Contract const * CRX_NOT_NULL pContract,
		size_t pExpectedFileByteSize, bool pIsToFavorMemoryMapping)
{
	bool vReturn = false;

	if((pThis->gPrivate_osFileHandle == CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE) &&
			(pThis->gPrivate_osFileHandle__2 == CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE))
	{
		if(!(pThis->gPrivate_isRwLockInitialized) ? crx_c_pthread_crx_isSameThreadAs(
				&(pThis->gPrivate_rawId)) :
				(crx_c_pthread_rwlock_trywrlock(&(pThis->gPrivate_rwLock)) == 0))
		{
			if((pThis->gPrivate_osFileHandle == CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE) &&
					(pThis->gPrivate_osFileHandle__2 == 
					CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE))
			{
				pThis->gPrivate_osFileHandle = crx_c_os_fileSystem_internal_doOpen(pFileFullPath, 
						pDepthLimitOfReparsePoints, NULL, pFileOpenMode, pFileAccessMode, 
						pFileShareMode, pContract, &(pThis->gPrivate_internalFullPath));
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD))
				/*
					BECAUSE WE HAVE TO SUPPORT EXCLUSIVE ACCESS, AND BECAUSE ON WINDOWS 95 WE
					CAN NOT USE A FILE HANDLE WHILE IT IS BEING USED WITH CreateFileMapping(), 
					AND BECAUSE OPENING A NEW FILE HANDLE WOULD NOT BE POSSIBLE UNDER EXCLUSIVE
					ACCESS, WE IMPOSE THE RESTRICTION THAT A FILE CAN NOT INTERNALLY HAVE BOTH 
					REGIONAL AND SEQUENTIAL ACCESS AT THE SAME TIME. REMEMBER, WITH WRITE ONLY, WE 
					CAN NOT HAVE REGIONAL ACCESS AT ALL.
				*/
				if((pFileAccessMode != CRX__C__OS__FILE_SYSTEM__FILE_ACCESS_MODE__WRITE_ONLY) &&
						(pFileShareMode != CRX__C__OS__FILE_SYSTEM__FILE_SHARE_MODE__EXCLUSIVE))
					{pThis->gPrivate_osFileHandle__2 = pThis->gPrivate_osFileHandle;}
#endif
				//ON WINDOWS WE WAIT UNTIL LATER TO OPEN A SECOND HANDLE IF NECESSARY

				if(pThis->gPrivate_osFileHandle != CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE)
				{
					pThis->gPrivate_isMemoryMappingPrefered = pIsToFavorMemoryMapping;
					pThis->gPrivate_expectedByteSize = pExpectedFileByteSize;
					pThis->gPrivate_fileAccessMode = pFileAccessMode;
					pThis->gPrivate_fileShareMode = pFileShareMode;
					
					if(crx_c_os_fileSystem_io_private_getFileSize(pThis,
							&(pThis->gPrivate_hiddenByteSize), pThis->gPrivate_osFileHandle))
					{
						pThis->gPrivate_maximumTraversedByteSize = pThis->gPrivate_hiddenByteSize;
						
						if((pThis->gPrivate_expectedByteSize == 0) || 
								(pThis->gPrivate_expectedByteSize < pThis->gPrivate_hiddenByteSize))
							{pThis->gPrivate_expectedByteSize = pThis->gPrivate_hiddenByteSize;}

						vReturn = true;
					}
					else
					{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD))
						crx_c_os_fileSystem_private_posix_rawClose(pThis->gPrivate_osFileHandle);
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
						CloseHandle(pThis->gPrivate_osFileHandle);
#endif				
						pThis->gPrivate_osFileHandle = CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE;
						pThis->gPrivate_osFileHandle__2 = 
								CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE;
					}
				}
			}

			if(pThis->gPrivate_isRwLockInitialized)
				{crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock));}
		}
	}

	return vReturn;
}
CRX__LIB__PUBLIC_C_FUNCTION() bool crx_c_os_fileSystem_io_open2(Crx_C_Os_FileSystem_Io * pThis
		Crx_C_Os_FileSystem_Iterator_Record * pRecord,
		Crx_C_Os_FileSystem_FileAccessMode pFileAccessMode,
		Crx_C_Os_FileSystem_FileShareMode pFileShareMode,
		size_t pExpectedFileByteSize, bool pIsToFavorMemoryMapping)
{
	bool vReturn = false;

	if((pThis->gPrivate_osFileHandle == CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE) &&
			(pThis->gPrivate_osFileHandle__2 == CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE))
	{
		if(!(pThis->gPrivate_isRwLockInitialized) ? crx_c_pthread_crx_isSameThreadAs(
				&(pThis->gPrivate_rawId)) :
				(crx_c_pthread_rwlock_trywrlock(&(pThis->gPrivate_rwLock)) == 0))
		{
			if((pThis->gPrivate_osFileHandle == CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE) &&
					(pThis->gPrivate_osFileHandle__2 == 
					CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE))
			{
				pThis->gPrivate_osFileHandle = crx_c_os_fileSystem_open2(pRecord, 
						pFileAccessMode, pFileShareMode);
				/*
					 ON WINDOWS 95 WE WOULD REQUIRE A SEPERATE HANDLE FOR MAPPING, BUT BECAUSE WE
					 CAN NOT AFFORD TO OPEN THE HANDLE AGAIN WHEN USING RECORDS, WE IMPOSE THE SAME
					 LIMITATION WHEN THE FILE IS OPENED FOR EXCLUSIVE ACCESS. WHILE OTHER SYSTEMS
					 DO NOT HAVE THE NEED FOR OPENING A NEW FILE HANDLE FOR THE MAPPING, WE HAVE
					 TO IMPOSE THE SAME LIMITATION ON THEM TO KEEP THE ABSTRACT VALID.
				*/
				pThis->gPrivate_osFileHandle__2 = INVALID_HANDLE_VALUE;

				crx_c_string_empty(&(pThis->gPrivate_internalFullPath));

				if(pThis->gPrivate_osFileHandle != CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE)
				{
					pThis->gPrivate_isMemoryMappingPrefered = pIsToFavorMemoryMapping;
					pThis->gPrivate_expectedByteSize = pExpectedFileByteSize;
					pThis->gPrivate_fileAccessMode = pFileAccessMode;
					pThis->gPrivate_fileShareMode = pFileShareMode;

					if(crx_c_os_fileSystem_io_private_getFileSize(pThis,
							&(pThis->gPrivate_hiddenByteSize), pThis->gPrivate_osFileHandle))
					{
						pThis->gPrivate_maximumTraversedByteSize = pThis->gPrivate_hiddenByteSize;
						
						if((pThis->gPrivate_expectedByteSize == 0) || 
								(pThis->gPrivate_expectedByteSize < pThis->gPrivate_hiddenByteSize))
							{pThis->gPrivate_expectedByteSize = pThis->gPrivate_hiddenByteSize;}

						vReturn = true;
					}
					else
					{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD))
						crx_c_os_fileSystem_private_posix_rawClose(pThis->gPrivate_osFileHandle);
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
						CloseHandle(pThis->gPrivate_osFileHandle);
#endif				
						pThis->gPrivate_osFileHandle = CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE;
					}
				}
			}

			if(pThis->gPrivate_isRwLockInitialized)
				{crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock));}
		}
	}
	else
		{return false;}
}

CRX__LIB__PUBLIC_C_FUNCTION() bool crx_c_os_fileSystem_io_private_getFileSize(
		Crx_C_Os_FileSystem_Io * pThis, uint64_t * CRX_NOT_NULL pReturn,
		Crx_C_Os_FileSystem_OsFileHandle pOsFileHandle)
{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	Crx_NonCrxed_Os_Posix_Dll_Libc_Stat vStat /*= ?*/;
	bool vIsNoError = true;

	CRX__SET_TO_ZERO(Crx_NonCrxed_Os_Posix_Dll_Libc_Stat, vStat);
	
	if(vIsNoError && (crx_c_os_fileSystem_private_posix_callFstat(pOsFileHandle, &vStat) != 0))
	{
		*pReturn = ((uint64_t)(vStat.st_size > 0 ? vStat.st_size : 0));

		return true;
	}
	else
	{
		*pReturn = 0;

		return false;
	}
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	BY_HANDLE_FILE_INFORMATION vByHandleFileInformation /*= ?*/;
	bool vIsNoError = true;
	uint64_t vFileSize = 0;

	if(crx_c_os_fileSystem_internal_win32_getFileInformationByHandle(
				pOsFileHandle, &vByHandleFileInformation) != 0)
	{
		ULARGE_INTEGER tULargeInteger;

		tULargeInteger.u.HighPart = vByHandleFileInformation.nFileSizeHigh;
		tULargeInteger.u.LowPart = vByHandleFileInformation.nFileSizeLow;
		
		*pReturn = tULargeInteger.QuadPart;

		return true;
	}
	else
	{
		*pReturn = 0;

		return false;
	}
#endif
}

CRX__LIB__PUBLIC_C_FUNCTION() bool crx_c_os_fileSystem_io_getFileSize(
		Crx_C_Os_FileSystem_Io * pThis, uint64_t * CRX_NOT_NULL pReturn,
		Crx_C_Os_FileSystem_OsFileHandle pOsFileHandle)
{
	bool vReturn = false;

	if((pThis->gPrivate_osFileHandle != CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE) ||
			(pThis->gPrivate_osFileHandle__2 != CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE))
	{
		if(!(pThis->gPrivate_isRwLockInitialized) ? crx_c_pthread_crx_isSameThreadAs(
				&(pThis->gPrivate_rawId)) :
				(crx_c_pthread_rwlock_rdlock(&(pThis->gPrivate_rwLock)) == 0))
		{
			if((pThis->gPrivate_osFileHandle != CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE) ||
					(pThis->gPrivate_osFileHandle__2 != CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE))
			{
				*pReturn = pThis->gPrivate_maximumTraversedByteSize;
				
				vReturn = true;
			}

			if(pThis->gPrivate_isRwLockInitialized)
				{crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock));}
		}
	}
	
	if(!vReturn)
		{*pReturn = false;}

	return vReturn;
}

CRX__LIB__PUBLIC_C_FUNCTION() bool crx_c_os_fileSystem_io_getSequentialAccessorId(
		Crx_C_Os_FileSystem_Io * pThis, uint32_t * CRX_NOT_NULL pReturn,
		uint32_t pPreferedMinimumNumberOfBytesToWrite, bool pIsToIgnoreRegions)
{
	if(!(pThis->gPrivate_isRwLockInitialized))
	{
		if(crx_c_pthread_crx_isSameThreadAs(&(pThis->gPrivate_rawId)))
		{
			uint16_t tId = 0;
			bool tReturn = true;

			if(!crx_c_hashTable_hasKey(&(pThis->gPrivate_infoOfSequentials), &tId))
			{
				Crx_C_Os_FileSystem_Io_InfoOfSequential * tInfoOfSequential = 
						crx_c_os_fileSystem_io_infoOfSequential_new(
						pPreferedMinimumNumberOfBytesToWrite, pIsToIgnoreRegions);

				if(crx_c_hashTable_set(&(pThis->gPrivate_infoOfSequentials), 
						&tId, &tInfoOfSequential))
					{tInfoOfSequential = NULL;}
				else
					{tReturn = false;}
			}
			
			*pReturn = tId;

			return tReturn;
		}
		else
			{return false;}
	}
	else
	{
		Crx_C_Os_FileSystem_Io_InfoOfSequential * tInfoOfSequential = 
				crx_c_os_fileSystem_io_infoOfSequential_new(pPreferedMinimumNumberOfBytesToWrite,
				pIsToIgnoreRegions);
		bool tReturn = (tInfoOfSequential != NULL);

		if(tReturn && (crx_c_pthread_rwlock_wrlock(&(pThis->gPrivate_rwLock)) == 0))
		{
			if(pThis->gPRIVATE__REGION_ALIGNMENT_VALUE > 
					CRX__C__OS__FILE_SYSTEM__IO__MAXIMUM_MEMORY_PAGE_SIZE_TO_CORRECT_BUFFER_WITH)
			{
				if(((crx_c_tree_getSize(&(pThis->gPrivate_regionsByOffset)) > 0) ||
						(pThis->gPrivate_numberOfActiveRegionCreations > 0)) &&
						(crx_c_hashTable_getSize(&(pThis->gPrivate_infoOfSequentials)) > 1))
					{tReturn = false;}
			}

			if(tReturn)
			{
				if(pThis->gAccessorCount < UINT32_MAX)
					{pThis->gAccessorCount = pThis->gAccessorCount + 1;}

				while(crx_c_hashTable_hasKey(&(pThis->gPrivate_infoOfSequentials), 
						&(pThis->gAccessorCount)))
				{
					if(pThis->gAccessorCount < UINT32_MAX)
						{pThis->gAccessorCount = pThis->gAccessorCount + 1;}
					else
					{
						tReturn = false;
						
						break;
					}
				}
			}

			if(tReturn)
			{
				if(crx_c_hashTable_set(&(pThis->gPrivate_infoOfSequentials), 
						&(pThis->gAccessorCount), &tInfoOfSequential))
					{tInfoOfSequential = NULL;}
				else
					{tReturn = false;}
			}
			
			crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock));
		}
		else
			{tReturn = false;}

		if(tReturn)
			{&pReturn = pThis->gAccessorCount;}
		else
			{&pReturn = 0;}
		
		if(tInfoOfSequential != NULL)
		{
			crx_c_os_fileSystem_io_infoOfSequential_destruct(tInfoOfSequential);
			crx_c_os_fileSystem_io_infoOfSequential_free(tInfoOfSequential);
			tInfoOfSequential = NULL;
		}
		
		return tReturn;
	}
}
CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_io_returnSequentialAccessorId(
		Crx_C_Os_FileSystem_Io * pThis, uint32_t pSequentialAccessorId)
{
	bool vReturn = true;

	if((pSequentialAccessorId != 0) && pThis->gPrivate_isRwLockInitialized)
	{
		if(crx_c_pthread_thread_crx_isSelfAPthread())
		{
			if(crx_c_pthread_rwlock_wrlock(&(pThis->gPrivate_rwLock)) == 0)
			{
				if(crx_c_hashTable_hasKey(&(pThis->gPrivate_infoOfSequentials), 
						&pSequentialAccessorId))
				{
					Crx_C_Os_FileSystem_Io_InfoOfSequential * const tInfoOfSequential = 
							*((Crx_C_Os_FileSystem_Io_InfoOfSequential * const *)crx_c_hashTable_get(
							&(pThis->gPrivate_infoOfSequentials), &pSequentialAccessorId));

					if(!tInfoOfSequential->gIsInUse)
					{
						crx_c_hashTable_remove(&(pThis->gPrivate_infoOfSequentials), 
								&pSequentialAccessorId);
					}
					else
						{vReturn = false;}
				}

				if(vReturn)
				{
					if(tInfoOfSequential->gWriteBufferLength > 0)
					{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
						crx_c_os_fileSystem_internal_posix_doPwrite(pThis->gPrivate_osFileHandle,
								tInfoOfSequential->gWriteBuffer, 
								tInfoOfSequential->gWriteBufferLength, tInfoOfSequential->gOffset);
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
						crx_c_os_fileSystem_private_win32_doWriteFile(pThis->gPrivate_osFileHandle,
								tInfoOfSequential->gWriteBuffer, 
								tInfoOfSequential->gWriteBufferLength, tInfoOfSequential->gOffset);
#endif
					}
				}
				
				crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock));

				if(vReturn)
				{
					crx_c_os_fileSystem_io_infoOfSequential_destruct(tInfoOfSequential);
					crx_c_os_fileSystem_io_infoOfSequential_free(tInfoOfSequential);
					tInfoOfSequential = NULL;
				}
			}
			else
				{vReturn = false;}
		}
		else
			{vReturn = false;}
	}

	return vReturn;
}
/*
	REMEMBER: IF THE FILE IS OPENED WITH CRX__C__OS__FILE_SYSTEM__FILE_SHARE_MODE__EXCLUSIVE SHARE 
			MODE, STRICTLY SPEAKING, FILE CAN ONLY BE USED FOR REGIONAL ACCESS OR SEQUENTIAL ACCESS 
			BUT NOT BOTH. IF ANY REGIONS ARE OPEN, SEQUENTIAL ACCESS HAS TO BE DONE USING REGIONS.
	REMEMBER: IF THE FILE IS OPENED USING A RECORD, STRICTLY SPEAKING, FILE CAN ONLY BE USED FOR 
			REGIONAL ACCESS OR SEQUENTIAL ACCESS BUT NOT BOTH. IF ANY REGIONS ARE OPEN, SEQUENTIAL 
			ACCESS HAS TO BE DONE USING REGIONS.
*/
CRX__LIB__PUBLIC_C_FUNCTION() bool crx_c_os_fileSystem_io_write(
		Crx_C_Os_FileSystem_Io * pThis, uint32_t pSequentialAccessorId,
		void * CRX_NOT_NULL pBytes, utin64_t pLength)
{
	CRX_SCOPE_META
	if(pThis->gPrivate_fileAccessMode == CRX__C__OS__FILE_SYSTEM__FILE_ACCESS_MODE__READ_ONLY)
		{return 0;}
	/*else if(crx_c_os_fileSystem_io_private_prepareAndGetFileHandleForSequentialAccess(pThis) == 
			CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE)
		{return false;}*/

	CRX_SCOPE
	bool vIsNoError = true;

	crx_c_os_fileSystem_io_private_prepareAndGetFileHandleForSequentialAccess(pThis);
	
	if(!(pThis->gPrivate_isRwLockInitialized) && !crx_c_pthread_crx_isSameThreadAs(
			&(pThis->gPrivate_rawId)))
		{vIsNoError = false;}

	if(vIsNoError && (!(pThis->gPrivate_isRwLockInitialized) ||
			(crx_c_pthread_rwlock_rdlock(&(pThis->gPrivate_rwLock)) == 0)))
	{
		Crx_C_Os_FileSystem_Io_InfoOfSequential * tInfoOfSequential = NULL;
		bool tIsLocked = pThis->gPrivate_isRwLockInitialized;

		if(crx_c_hashTable_hasKey(&(pThis->gPrivate_infoOfSequentials), &pSequentialAccessorId))
		{
			tInfoOfSequential = ((Crx_C_Os_FileSystem_Io_InfoOfSequential *)crx_c_hashTable_get(
					&(pThis->gPrivate_infoOfSequentials), &pSequentialAccessorId));

			if(!tInfoOfSequential->gIsInUse)
				{tInfoOfSequential->gIsInUse = true;}
			else
				{vIsNoError = false;}
		}
		else
			{vIsNoError = false;}

		if(vIsNoError && 
				(pThis->gPrivate_osFileHandle != CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE))
		{
			bool tIsToOnlyBuffer = false;
			bool tIsToExtendToASecondBufferAndOnly = false;
			bool tIsToAddressRegions = false;
			uint64_t tAnticipatedOffset = 0;
			bool tIsGrowing = false;

			if(pThis->gPrivate_isRwLockInitialized)
			{
				//AVOIDING ATOMICS FOR THE TIME BEING
				if(crx_c_pthread_rwlock_wrlock(&(pThis->gPrivate_rwLock__lockedPortions)) == 0)
				{
					pThis->gPrivate_numberOfActiveSequentialRequests = 
							pThis->gPrivate_numberOfActiveSequentialRequests + 1;

					crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock__lockedPortions));
				}
				else
					{vIsNoError = false;}
			}

			if(vIsNoError)
			{
				tIsToExtendToASecondBufferAndOnly = ((pLength >
						(tInfoOfSequential->gBufferCapacity - tInfoOfSequential->gIndex))  &&
						((pLength - (tInfoOfSequential->gBufferCapacity - 
						tInfoOfSequential->gIndex)) <= tInfoOfSequential->gBufferCapacity));
				tIsToOnlyBuffer = ((pLength <= (tInfoOfSequential->gBufferCapacity - 
								tInfoOfSequential->gIndex)) || tIsToExtendToASecondBufferAndOnly);
			}
			
			if(vIsNoError)
			{
				/*
					BECAUSE OF THIS, IF pThis->gPrivate_numberOfActiveSequentialRequests IS NOT 
							ZERO WE CAN NOT CREATE NEW REGIONS ON WINDOWS.
				*/
				tAnticipatedOffset = tInfoOfSequential->gOffset + 
						tInfoOfSequential->gIndex + pLength;

				if(tAnticipatedOffset > pThis->gPrivate_hiddenByteSize)
				{
					//THIS SHOULD PROTECT REGION CREATION FROM SIZE CHANGING DURING REGION CREATION.
					if((crx_c_tree_getSize(&(pThis->gPrivate_regionsByOffset)) == 0) &&
							(pThis->gPrivate_numberOfActiveRegionCreations == 0))
					{
						assert(pThis->gPrivate_osFileHandle != 
								CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE);

						/*Io::gPrivate_numberOfActiveSequentialRequests SHOULD PROTECT SIZE CHANGING
								FROM REGION CREATION DURING SIZE CHANGING EVEN AFTER LETTING GO
								OF pThis->gPrivate_rwLock.*/

						tIsToOnlyBuffer = false;
						tIsGrowing = true;
					}
					else
						{vIsNoError = false;}
				}
			}
			
			if(vIsNoError && pThis->gPrivate_isRwLockInitialized &&
					!(tInfoOfSequential->gIsToIgnoreRegions) && 
					(pThis->gPrivate_fileAccessMode != 
					CRX__C__OS__FILE_SYSTEM__FILE_ACCESS_MODE__WRITE_ONLY))
				{tIsToAddressRegions = true;}

			if(vIsNoError && tIsToOnlyBuffer && tIsToAddressRegions)
			{
				/*IF THE BUFFER IS TO BE USED, AND IS CURRENTLY EMPTY, MEANING A REGION LOCK 
						SPANNING THE BUFFER SEGMENT IS NOT SET YET, OR IT IS NOT EMPTY AND NEW BUFFER
						IS ENOUGH TO SATIFY THE REQUEST, WE MUST ENSURE THAT NO CURRENT
						REGIONS OVERLAP THE UNSET BUFFER, OTHERIWSE WE CAN NOT USE IT. FOR THE 
						BUFFER, NOT ONLY WE WANT TO FORBID REGIONS OVERLAPPING ITS SEGMENT TO BE 
						CREATED OR DELETED, WHICH IS WHAT A REGION LOCK IS FOR, BUT WE ALSO WANT NO 
						REGIONS AT ALL OVERLAPING ITS SEGMENT.
				*/
				if((tInfoOfSequential->gWriteBufferLength == 0) || 
						tIsToExtendToASecondBufferAndOnly)
				{
					size_t tStartOffset = tInfoOfSequential->gOffset + 
							((tInfoOfSequential->gWriteBufferLength == 0) ? 0 : 
							tInfoOfSequential->gBufferCapacity);
					size_t tExclusiveEndOffset = tStartOffset + 
							(tInfoOfSequential->gBufferCapacity * 
							(((tInfoOfSequential->gWriteBufferLength == 0) &&
							tIsToExtendToASecondBufferAndOnly) ? 2 : 1));
					
					//REMEMBER: WE NEED TO HAVE Io::gPrivate_rwLock LOCKED FOR THIS FUCNITON
					tIsToOnlyBuffer = 
						!crx_c_os_fileSystem_io_private_unsafeCheckIfRegionsInRangeAndOptionallyMarkThem(
						pThis, tStartOffset, tExclusiveEndOffset, &vIsNoError, NULL);
				}
			}
			
			if(vIsNoError && tIsToOnlyBuffer)
			{
				/*
					IN THIS CASE,
						- WE CAN NOT DEAL WITH REGIONS OVERLAPPING THE SEGMENT
						- WE MIGHT FLUSH THE CURRENT BUFFER, THEN WRITE TO A NEW BUFFER, BUT NO 
								MORE.
						- THE DATA TO BE WRITTEN MIGHT BE SMALL ENOUGH TO FIT THE CURRENT BUFFER,
								OR LARGE ENOUGH TO NEED ANOTHER BUFFER, BUT NOT LARGER.
						- THE DATA TO BE WRITTEN CAN NOT ENLARGE THE FILE.
				*/
				size_t tRemainingLength = pLength;

				if(tIsToAddressRegions && ((tInfoOfSequential->gWriteBufferLength == 0) ||
						tIsToExtendToASecondBufferAndOnly))
				{
					/*
						BECAUSE WE WANT TO UNLOCK AS SOON AS POSSIBLE, WE END UP REGION LOCKING
						THE CURRENT BUFFER SPAN WHICH IS YET TO BE FLUSHED IF NECESSARY OR READ, 
						AND THE NEXT BUFFER SPAN WHICH IS TO FILLED WITH NEW DATA.
					*/
					if(crx_c_pthread_rwlock_wrlock(&(pThis->gPrivate_rwLock__lockedPortions)) == 0)
					{
						crx_c_tree_removeElement(&(pThis->gPrivate_portions__locked),
								&(tInfoOfSequential->gPortion__lock));

						tInfoOfSequential->gPortion__lock.gBaseOffset = tInfoOfSequential->gOffset;
						tInfoOfSequential->gPortion__lock.gLength = 
								(tInfoOfSequential->gBufferCapacity * 
								(tIsToExtendToASecondBufferAndOnly ? 2 : 1));
						
						if(!crx_c_tree_insertElement(&(pThis->gPrivate_portions__locked), 
								&(tInfoOfSequential->gPortion__lock)))
							{vIsNoError = false;}

						crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock__lockedPortions));
					}
					else
						{vIsNoError = false;}
				}

				if(tIsLocked)//MEANING pThis->gPrivate_isRwLockInitialized
				{
					crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock));
					tIsLocked = false;
				}

				if(vIsNoError && tIsToExtendToASecondBufferAndOnly)
				{
					if(tInfoOfSequential->gIndex < tInfoOfSequential->gBufferCapacity)
					{
						memcpy(tInfoOfSequential->gWriteBuffer + tInfoOfSequential->gIndex, pBytes,
								tInfoOfSequential->gBufferCapacity - tInfoOfSequential->gIndex);
						tRemainingLength = tRemainingLength - 
								(tInfoOfSequential->gBufferCapacity - tInfoOfSequential->gIndex);

						tInfoOfSequential->gIsDirty = true;
					}

					if(tInfoOfSequential->gIsDirty)
					{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD))
						vIsNoError = crx_c_os_fileSystem_internal_posix_doPwrite(
								pThis->gPrivate_osFileHandle, 
								tInfoOfSequential->gWriteBuffer, 
								tInfoOfSequential->gIndex, 
								tInfoOfSequential->gOffset);
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
						vIsNoError = crx_c_os_fileSystem_private_win32_doWriteFile(
								pThis->gPrivate_osFileHandle,
								tInfoOfSequential->gWriteBuffer, 
								tInfoOfSequential->gIndex, 
								tInfoOfSequential->gOffset);
#endif
					}

					if(vIsNoError)
					{
						tInfoOfSequential->gOffset = tInfoOfSequential->gOffset + 
								tInfoOfSequential->gWriteBufferLength;
						tInfoOfSequential->gIndex = 0;
						tInfoOfSequential->gIsDirty = false;
						tInfoOfSequential->gWriteBufferLength = 0;
					}
					else
					{
						//WE HAVE NOT CHANGED tInfoOfSequential->gIndex NOR 
						//		tInfoOfSequential->gWriteBufferLength ABOVE FOR THIS VERY SITUATION.
						
						//POSSIBLY DISCARDING BUFFERED READ BYTES BUT THAT IS BETTER BECAUSE THEY
						//		MIGHT HAVE BECOME DIRTY FROM THE ABOVE.
						tInfoOfSequential->gWriteBufferLength = tInfoOfSequential->gIndex;
					}
				}

				if(vIsNoError && (tRemainingLength > 0))
				{
					memcpy(tInfoOfSequential->gWriteBuffer + tInfoOfSequential->gIndex, pBytes, 
							tRemainingLength);
					tInfoOfSequential->gIndex = tInfoOfSequential->gIndex + tRemainingLength;

					if(tInfoOfSequential->gIndex > tInfoOfSequential->gWriteBufferLength)
						{tInfoOfSequential->gWriteBufferLength = tInfoOfSequential->gIndex;}
					tInfoOfSequential->gIsDirty = true;
				}
			}
			else if(vIsNoError && !tIsToAddressRegions)
			{
				/*
					IN THIS CASE,
						- WE EXPECT NO REGIONS AT ALL.
						- IF DATA TO BE WRITTEN IS SMALL ENOUGH TO FIT THE CURRENT BUFFER, NO 
								FLUSHING IS DONE, AND IF LARGER BUT SMALL ENOUGH TO FIT A NEW 
								BUFFER, ONLY A SINGLE FLUSHING, THE FLUSHING OF THE OLD BUFFER,
								IS DONE. AND IF LARGER, ONLY A SINGLE FLUSHING IS DONE, AND THE 
								BUFFER IS NOT USED FURTHER.
						- THE DATA TO BE WRITTEN MIGHT BE SMALL ENOUGH TO FIT THE CURRENT BUFFER,
								OR LARGE ENOUGH TO NEED ANOTHER BUFFER, OR LARGE ENOUGH TO NEED 
								MORE.
						- THE DATA TO BE WRITTEN COULD ENLARGE THE FILE.
				*/
				size_t tRemainingLength = pLength;
				size_t tTemp = tRemainingLength;

				if(tIsLocked)//MEANING pThis->gPrivate_isRwLockInitialized
				{
					crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock));
					tIsLocked = false;
				}

				if(tRemainingLength > (tInfoOfSequential->gBufferCapacity - 
						tInfoOfSequential->gIndex))
					{tTemp = (tInfoOfSequential->gBufferCapacity - tInfoOfSequential->gIndex);}

				memcpy(tInfoOfSequential->gWriteBuffer + tInfoOfSequential->gIndex, pBytes, tTemp);
				tRemainingLength = tRemainingLength - tTemp;
				tInfoOfSequential->gIndex = tInfoOfSequential->gIndex + tTemp;
				tInfoOfSequential->gIsDirty = true;
				
				if(tInfoOfSequential->gIndex > tInfoOfSequential->gWriteBufferLength)
					{tInfoOfSequential->gWriteBufferLength = tInfoOfSequential->gIndex;}
				
				if(tRemainingLength > 0)
				{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD))
					vIsNoError = crx_c_os_fileSystem_internal_posix_doPwrite(
							pThis->gPrivate_osFileHandle, 
							tInfoOfSequential->gWriteBuffer, 
							tInfoOfSequential->gBufferCapacity, 
							tInfoOfSequential->gOffset);
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
					vIsNoError = crx_c_os_fileSystem_private_win32_doWriteFile(
							pThis->gPrivate_osFileHandle,
							tInfoOfSequential->gWriteBuffer, 
							tInfoOfSequential->gBufferCapacity, 
							tInfoOfSequential->gOffset);
#endif

					if(vIsNoError)
					{
						tInfoOfSequential->gOffset = tInfoOfSequential->gOffset + 
								tInfoOfSequential->gBufferCapacity;
						tInfoOfSequential->gIndex = 0;
						tInfoOfSequential->gIsDirty = false;
						tInfoOfSequential->gWriteBufferLength = 0;
					}
					else
					{
						tInfoOfSequential->gIndex = tInfoOfSequential->gIndex - tTemp;
						//POSSIBLY DISCARDING BUFFERED READ BYTES BUT THAT IS BETTER BECAUSE THEY
						//		MIGHT HAVE BECOME DIRTY FROM THE ABOVE.
						tInfoOfSequential->gWriteBufferLength = tInfoOfSequential->gIndex;
					}
				}

				if(vIsNoError && (tRemainingLength > 0))
				{
					if(tRemainingLength > tInfoOfSequential->gBufferCapacity)
					{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD))
						vIsNoError = crx_c_os_fileSystem_internal_posix_doPwrite(
								pThis->gPrivate_osFileHandle, 
								pBytes + (pLength - tRemainingLength), 
								tRemainingLength, 
								tInfoOfSequential->gOffset);
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
						vIsNoError = crx_c_os_fileSystem_private_win32_doWriteFile(
								pThis->gPrivate_osFileHandle,
								pBytes + (pLength - tRemainingLength), 
								tRemainingLength, 
								tInfoOfSequential->gOffset);
#endif

						if(vIsNoError)
						{
							tInfoOfSequential->gOffset = tInfoOfSequential->gOffset + 
									tRemainingLength;
						}
					}
					else
					{
						memcpy(tInfoOfSequential->gWriteBuffer, pBytes, tRemainingLength);
						tInfoOfSequential->gIndex = tRemainingLength;
						tInfoOfSequential->gIsDirty = true;
						tInfoOfSequential->gWriteBufferLength = tRemainingLength;
						
						/*
							WE DO NOT HAVE TO WORRY ABOUT IMMEDIATELY GROWING THE FILE WHEN IT NEEDS
							GROWING BECAUSE NO REGIONS CAN EXIST, OTHERWISE WE WOULD NOT BE HERE.
						*/
					}
				}

				if(vIsNoError && (tIsGrowing || 
						(tAnticipatedOffset > pThis->gPrivate_maximumTraversedByteSize)))
				{
					if(!(pThis->gPrivate_isRwLockInitialized) ||
							(crx_c_pthread_rwlock_wrlock(&(pThis->gPrivate_rwLock)) == 0))
					{
						if(tAnticipatedOffset > pThis->gPrivate_hiddenByteSize)
						{
							/*
								THE FILE MIGHT NOT YET BE THE SIZE BELOW, BUT THAT IS FINE IN THIS
								CASE BECAUSE NO REGIONS CAN EXIST.
							*/
							pThis->gPrivate_hiddenByteSize = tAnticipatedOffset;
							
							if(pThis->gPrivate_hiddenByteSize > 
									pThis->gPrivate_expectedByteSize)
							{
								pThis->gPrivate_expectedByteSize = 
										pThis->gPrivate_hiddenByteSize;
							}
						}
						
						if(tAnticipatedOffset > pThis->gPrivate_maximumTraversedByteSize)
							{pThis->gPrivate_maximumTraversedByteSize = tAnticipatedOffset;}

						if(pThis->gPrivate_isRwLockInitialized)
							{crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock));}
					}
					else
						{vIsNoError = false;}
				}
			}
			else if(vIsNoError)
			{
				/*
					IN THIS CASE,
						- WE CAN DEAL WITH REGIONS OVERLAPPING THE SEGMENT
						- WE FLUSH THE BUFFER, AND NOT USE IT AGAIN.
						- THE DATA TO BE WRITTEN MIGHT BE SMALL ENOUGH TO FIT THE CURRENT BUFFER,
								OR LARGE ENOUGH TO NEED ANOTHER BUFFER, OR LARHE ENOUGH TO NEED 
								MORE.
						- THE DATA TO BE WRITTEN COULD ENLARGE THE FILE.
						
					NOTE: CODE HERE WAS WRITTEN WITH OVERLAPPING REGIONS IN MIND. OVERLAPPING REGIONS
							ARE NO LONGER ALLOWED, BUT KEEP THE ALREADY EXISTING CODE THAT MIGHT
							BE ADDRESSING THIS CASE.
				*/
				Crx_C_Os_FileSystem_Io_Region * _tBuffer[8];
				Crx_C_Array/*<Crx_C_Os_FileSystem_Io_Region *>*/ tRegions /*= ?*/;
				uint64_t tOriginalOffset = tInfoOfSequential->gOffset + tInfoOfSequential->gIndex;
				size_t tCurrentOffset = tOriginalOffset;
				size_t tExclusiveEndOffset = tOriginalOffset + pLength;
				size_t tTemp = 0;
				int32_t tUnlockResult = 0;
				size_t tUnlockCount = 0;

				crx_c_array_construct2(&tRegions, 
						crx_c_os_fileSystem_io_region_getTypeBluePrintForPointer(), &(_tBuffer[0]), 
						8, 8);
				
				//REMEMBER: WE NEED TO HAVE Io::gPrivate_rwLock LOCKED FOR THIS FUCNITON
				crx_c_os_fileSystem_io_private_unsafeCheckIfRegionsInRangeAndOptionallyMarkThem(
						pThis, tOriginalOffset, tExclusiveEndOffset, &vIsNoError, tRegions);

				if(vIsNoError)
				{
					if(crx_c_pthread_rwlock_wrlock(&(pThis->gPrivate_rwLock__lockedPortions)) == 0)
					{
						crx_c_tree_removeElement(&(pThis->gPrivate_portions__locked), 
								&(tInfoOfSequential->gPortion__lock));

						/*
							WE MIGHT BE LOCKING MORE THAN THE REQUIRED REGION, BUT THAT IS FINE.
							THE ALGORITHM MUST CONSIDER FAILURES LATER ON.
						*/
						if(tInfoOfSequential->gWriteBufferLength > 0)
						{
							tInfoOfSequential->gPortion__lock.gBaseOffset = 
									tInfoOfSequential->gOffset;
							tInfoOfSequential->gPortion__lock.gLength = 
									tInfoOfSequential->gBufferCapacity + pLength;
						}
						else
						{
							//REMEMBER, WE WILL NOT USE THE BUFFER AT ALL IF IT IS EMPTY.
							tInfoOfSequential->gPortion__lock.gBaseOffset = tOriginalOffset;
							tInfoOfSequential->gPortion__lock.gLength = pLength;
						}

						if(!crx_c_tree_insertElement(&(pThis->gPrivate_portions__locked), 
								&(tInfoOfSequential->gPortion__lock)))
							{vIsNoError = false;}

						crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock__lockedPortions));
					}
					else
						{vIsNoError = false;}
				}

				crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock));
				tIsLocked = false;

				if(vIsNoError && tInfoOfSequential->gIsDirty &&
						(tInfoOfSequential->gIndex < tInfoOfSequential->gBufferCapacity))
				{
					tTemp = tInfoOfSequential->gBufferCapacity - tInfoOfSequential->gIndex;

					if(tTemp > pLength)
						{tTemp = pLength;}

					memcpy(tInfoOfSequential->gWriteBuffer + tInfoOfSequential->gIndex, pBytes,
							tTemp);

					tCurrentOffset = tCurrentOffset + tTemp;
					tInfoOfSequential->gIndex = tInfoOfSequential->gIndex + tTemp;
				}

				if(vIsNoError)
				{
					if(tInfoOfSequential->gIsDirty)
					{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD))
						vIsNoError = crx_c_os_fileSystem_internal_posix_doPwrite(
								pThis->gPrivate_osFileHandle, 
								tInfoOfSequential->gWriteBuffer, 
								tInfoOfSequential->gIndex, 
								tInfoOfSequential->gOffset);
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
						vIsNoError = crx_c_os_fileSystem_private_win32_doWriteFile(
								pThis->gPrivate_osFileHandle,
								tInfoOfSequential->gWriteBuffer, 
								tInfoOfSequential->gIndex, 
								tInfoOfSequential->gOffset);
#endif
						if(!vIsNoError)
							{tInfoOfSequential->gIndex = tInfoOfSequential->gIndex - tTemp;}
					}
					
					if(vIsNoError)
					{
						tInfoOfSequential->gOffset = tCurrentOffset;
						tInfoOfSequential->gIndex = 0;
						tInfoOfSequential->gIsDirty = false;
						tInfoOfSequential->gWriteBufferLength = 0;
					}
				}

				if(vIsNoError && (tCurrentOffset < tExclusiveEndOffset))
				{
					CRX_FOR(size_t tI = 0, tI < crx_c_array_getLength(&tRegions), tI++)
					{
						Crx_C_Os_FileSystem_Io_Region * tRegion2 = 
								*((Crx_C_Os_FileSystem_Io_Region * *)crx_c_array_get(&tRegions, 
								tI));

						if(tCurrentOffset < tRegion2->gBaseOffset)
						{
							size_t tLength2 = tRegion2->gBaseOffset - tCurrentOffset;
							
							if(tLength2 > (tExclusiveEndOffset - tCurrentOffset))
								{tLength2 = tExclusiveEndOffset - tCurrentOffset;}

#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD))
							//WE EXPECT NO FAILURE
							vIsNoError = crx_c_os_fileSystem_internal_posix_doPwrite(
									pThis->gPrivate_osFileHandle,
									pBytes + (tCurrentOffset - tOriginalOffset),
									tLength2, tCurrentOffset);	
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
							//WE EXPECT NO FAILURE
							vIsNoError = crx_c_os_fileSystem_private_win32_doWriteFile(
									pThis->gPrivate_osFileHandle,
									pBytes + (tCurrentOffset - tOriginalOffset), 
									tLength2, tCurrentOffset);
#endif

							if(vIsNoError)
								{tCurrentOffset = tCurrentOffset + tLength2;}
							else
								{tInfoOfSequential->gOffset = tCurrentOffset;}
						}

						if(vIsNoError && (tCurrentOffset < tExclusiveEndOffset) &&
								(tCurrentOffset >= tRegion2->gBaseOffset) &&
								(tCurrentOffset < (tRegion2->gBaseOffset + tRegion2->gLength)))
						{
							unsigned char * tPointerIntoRegion = tRegion2->gBaseAddress + 
									(tCurrentOffset - tRegion2->gBaseOffset);
							size_t tLength2 = (tRegion2->gBaseOffset + tRegion2->gLength) - 
									tCurrentOffset;

							if(tCurrentOffset + tLength2 > tExclusiveEndOffset)
								{tLength2 = tExclusiveEndOffset - tCurrentOffset;}

							memcpy(tPointerIntoRegion, pBytes + (tCurrentOffset - tOriginalOffset),
									tLength2);
							tCurrentOffset = tCurrentOffset + tLength2;
						}

						if(!vIsNoError || (tCurrentOffset >= tExclusiveEndOffset))
							{break;}
					}
					CRX_ENDFOR

					if(vIsNoError && (tCurrentOffset < tExclusiveEndOffset))
					{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD))
						vIsNoError = crx_c_os_fileSystem_internal_posix_doPwrite(
								pThis->gPrivate_osFileHandle,
								pBytes + (tCurrentOffset - tOriginalOffset),
								tExclusiveEndOffset - tCurrentOffset, 
								tCurrentOffset);
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
						vIsNoError = crx_c_os_fileSystem_private_win32_doWriteFile(
								pThis->gPrivate_osFileHandle,
								pBytes + (tCurrentOffset - tOriginalOffset), 
								tExclusiveEndOffset - tCurrentOffset, 
								tCurrentOffset);
#endif

						if(vIsNoError)
							{tCurrentOffset = tExclusiveEndOffset;}
						else
							{tInfoOfSequential->gOffset = tCurrentOffset;}
					}
					
					if(vIsNoError)
						{tInfoOfSequential->gOffset = tCurrentOffset;}
				}

				if(vIsNoError && tIsGrowing)
				{
					if(crx_c_pthread_rwlock_wrlock(&(pThis->gPrivate_rwLock)) == 0))
					{
						if(tAnticipatedOffset > pThis->gPrivate_maximumTraversedByteSize)
						{
							pThis->gPrivate_maximumTraversedByteSize = tAnticipatedOffset;

							if(pThis->gPrivate_maximumTraversedByteSize > 
									pThis->gPrivate_expectedByteSize)
							{
								pThis->gPrivate_expectedByteSize = 
										pThis->gPrivate_maximumTraversedByteSize;
							}
						}

						crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock));
					}
					else
						{vIsNoError = false;}
				}

				if(crx_c_os_fileSystem_io_private_patientlyLockForReadRwLock(pThis, 
						&(pThis->gPrivate_rwLock)) == 0)
				{
					tIsLocked = true;
					
					CRX_FOR(size_t tI = 0, tI < crx_c_array_getLength(&tRegions), tI++)
					{
						Crx_C_Os_FileSystem_Io_Region * tRegion2 = 
								*((Crx_C_Os_FileSystem_Io_Region * *)crx_c_array_get(&tRegions, 
								tI));

						if(crx_c_os_fileSystem_io_private_patientlyLockForWriteRwLock(pThis,
								&(pThis->gPrivate_rwLock__lockedPortions)) == 0)
						{//TRYING TO AVOID THE USE OF ATOMICS FOR THE TIME BEING
							tRegion2->gNumberOfHeldPointers = tRegion2->gNumberOfHeldPointers - 1;
							crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock__lockedPortions));
						}
					}
					CRX_ENDFOR

					if(crx_c_os_fileSystem_io_private_patientlyLockForWriteRwLock(pThis,
							&(pThis->gPrivate_rwLock__lockedPortions)) == 0)
					{
						crx_c_tree_removeElement(&(pThis->gPrivate_portions__locked), 
								&(tInfoOfSequential->gPortion__lock));

						if(tInfoOfSequential->gWriteBufferLength > 0) //WE FAILED BEFORE FLUSH
						{
							tInfoOfSequential->gPortion__lock.gBaseOffset = 
									tInfoOfSequential->gOffset;
							tInfoOfSequential->gPortion__lock.gLength = 
									tInfoOfSequential->gBufferCapacity;

							crx_c_tree_insertElement(&(pThis->gPrivate_portions__locked), 
									&(tInfoOfSequential->gPortion__lock));
						}

						crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock__lockedPortions));
					}

					crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock));
				}
				
				crx_c_array_destruct(&tRegions);
			}
			
			if(!tIsLocked && pThis->gPrivate_isRwLockInitialized)
			{
				tIsLocked = (crx_c_os_fileSystem_io_private_patientlyLockForReadRwLock(pThis, 
						&(pThis->gPrivate_rwLock)) == 0);
			}
			
			if(tIsLocked)
			{
				if(crx_c_os_fileSystem_io_private_patientlyLockForWriteRwLock(pThis,
						&(pThis->gPrivate_rwLock__lockedPortions)) == 0)
				{
					//TRYING TO AVOID THE USE OF ATOMICS FOR THE TIME BEING
					pThis->gPrivate_numberOfActiveSequentialRequests = 
							pThis->gPrivate_numberOfActiveSequentialRequests - 1;
					crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock__lockedPortions));
				}
			}
		}
		else if(vIsNoError && 
				(pThis->gPrivate_osFileHandle__2 != CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE))
		{
			Crx_C_Os_FileSystem_Io_Region * _tBuffer[8];
			Crx_C_Array/*<Crx_C_Os_FileSystem_Io_Region *>*/ tRegions /*= ?*/;
			uint64_t tOriginalOffset = tInfoOfSequential->gOffset + tInfoOfSequential->gIndex;
			size_t tCurrentOffset = tOriginalOffset;
			size_t tExclusiveEndOffset = tOriginalOffset + pLength;
			size_t tTemp = 0;
			int32_t tUnlockResult = 0;
			size_t tUnlockCount = 0;

			crx_c_array_construct2(&tRegions, 
					crx_c_os_fileSystem_io_region_getTypeBluePrintForPointer(), &(_tBuffer[0]), 
					8, 8);
			
			//REMEMBER: WE NEED TO HAVE Io::gPrivate_rwLock LOCKED FOR THIS FUCNITON
			crx_c_os_fileSystem_io_private_unsafeCheckIfRegionsInRangeAndOptionallyMarkThem(
					pThis, tOriginalOffset, tExclusiveEndOffset, &vIsNoError, tRegions);

			if(tIsLocked)//MEANING pThis->gPrivate_isRwLockInitialized
			{
				crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock));
				tIsLocked = false;
			}

			crx_c_os_fileSystem_io_getPointerToRegion
		}
	}
	
	return vIsNoError;
	CRX_SCOPE_END
}
CRX__LIB__PUBLIC_C_FUNCTION() int64_t crx_c_os_fileSystem_io_read(
		Crx_C_Os_FileSystem_Io * pThis, uint32_t pSequentialAccessorId,
		void * CRX_NOT_NULL pBytes, uint64_t pLength)
{
	CRX_SCOPE_META
	if(pThis->gPrivate_fileAccessMode == CRX__C__OS__FILE_SYSTEM__FILE_ACCESS_MODE__WRITE_ONLY)
		{return 0;}

	CRX_SCOPE
	bool vIsNoError = true;
	int64_t vReturn = -1;

	crx_c_os_fileSystem_io_private_prepareAndGetFileHandleForSequentialAccess(pThis);

	if(!(pThis->gPrivate_isRwLockInitialized) ||
			(crx_c_pthread_rwlock_rdlock(&(pThis->gPrivate_rwLock)) == 0))
	{
		Crx_C_Os_FileSystem_Io_InfoOfSequential * tInfoOfSequential = NULL
		bool tIsLocked = pThis->gPrivate_isRwLockInitialized;

		if(crx_c_hashTable_hasKey(&(pThis->gPrivate_infoOfSequentials), &pSequentialAccessorId))
		{
			tInfoOfSequential = ((Crx_C_Os_FileSystem_Io_InfoOfSequential *)crx_c_hashTable_get(
					&(pThis->gPrivate_infoOfSequentials), &pSequentialAccessorId));

			if(!tInfoOfSequential->gIsInUse)
				{tInfoOfSequential->gIsInUse = true;}
			else
				{vIsNoError = false;}
		}
		else
			{vIsNoError = false;}

		if(vIsNoError && 
				(pThis->gPrivate_osFileHandle != CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE))
		{
			bool tIsToOnlyBuffer = false;
			bool tIsToExtendToASecondBufferAndOnly = false;
			bool tIsToAddressRegions = false;
			uint64_t tOriginalOffset = 0;
			uint64_t tLength = 0;
			uint64_t tRemainingLength = 0;

			if(pThis->gPrivate_isRwLockInitialized)
			{
				//AVOIDING ATOMICS FOR THE TIME BEING
				if(crx_c_pthread_rwlock_wrlock(&(pThis->gPrivate_rwLock__lockedPortions)) == 0)
				{
					pThis->gPrivate_numberOfActiveSequentialRequests = 
							pThis->gPrivate_numberOfActiveSequentialRequests + 1;

					crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock__lockedPortions));
				}
				else
					{vIsNoError = false;}
			}

			if(vIsNoError)
			{
				tOriginalOffset = tInfoOfSequential->gOffset + tInfoOfSequential->gIndex;

				if((tOriginalOffset + pLength) > pThis->gPrivate_maximumTraversedByteSize)
				{
					utin64_t tDifference = ((tInfoOfSequential->gOffset + 
							tInfoOfSequential->gIndex + pLength) - 
							pThis->gPrivate_maximumTraversedByteSize);

					if(pLength >= tDifference)
						{tLength = pLength - tDifference;}
					else
						{tLength = 0;}

					tRemainingLength = tLength;
				}

				tIsToExtendToASecondBufferAndOnly = ((tLength >
						(tInfoOfSequential->gWriteBufferLength - tInfoOfSequential->gIndex))  &&
						((tLength - (tInfoOfSequential->gWriteBufferLength - 
						tInfoOfSequential->gIndex)) <= tInfoOfSequential->gBufferCapacity)) &&
						(tInfoOfSequential->gWriteBufferLength != 0);
				tIsToOnlyBuffer = ((tLength <= (tInfoOfSequential->gWriteBufferLength - 
						tInfoOfSequential->gIndex)) || tIsToExtendToASecondBufferAndOnly);
			}
			
			if(vIsNoError && pThis->gPrivate_isRwLockInitialized &&
					!(tInfoOfSequential->gIsToIgnoreRegions) && 
					(pThis->gPrivate_fileAccessMode != 
					CRX__C__OS__FILE_SYSTEM__FILE_ACCESS_MODE__WRITE_ONLY))
				{tIsToAddressRegions = true;}

			if(vIsNoError && tIsToOnlyBuffer && tIsToAddressRegions)
			{
				/*IF THE BUFFER IS TO BE USED, AND IS CURRENTLY EMPTY, A REGION LOCK HAS NOT BEEN
						SET YET, OR IF IT IS NOT EMPTY BUT IT DOES NOT HAVE ENOUGH REMAINING BITS 
						BUT READING AN ENTIRE NEW BUFFER CAN FULLFILL THE REQUEST, A REGION LOCK 
						SPANNING THE NEW BUFFER SEGMENT IS NOT SET YET. WE MUST ENSURE THAT NO 
						CURRENT REGIONS OVERLAP THE UNSET BUFFER, OTHERIWSE WE CAN NOT USE IT. FOR 
						THE BUFFER, NOT ONLY WE WANT TO FORBID REGIONS THAT ARE OVERLAPPING ITS 
						SEGMENT TO BE CREATED OR DELETED, WHICH IS WHAT A REGION LOCK IS FOR, BUT WE 
						ALSO WANT NO REGIONS AT ALL OVERLAPING ITS SEGMENT.
				*/		
				if((tInfoOfSequential->gWriteBufferLength == 0) ||
						tIsToExtendToASecondBufferAndOnly)
				{
					size_t tStartOffset = tInfoOfSequential->gOffset + 
							tInfoOfSequential->gWriteBufferLength;
					size_t tExclusiveEndOffset = tStartOffset + 
							tInfoOfSequential->gBufferCapacity;

					//REMEMBER: WE NEED TO HAVE Io::gPrivate_rwLock LOCKED FOR THIS FUCNITON
					tIsToOnlyBuffer = 
							!crx_c_os_fileSystem_io_private_unsafeCheckIfRegionsInRangeAndOptionallyMarkThem(
							pThis, tStartOffset, tExclusiveEndOffset, &vIsNoError, NULL);
				}
			}

			if(vIsNoError && tIsToOnlyBuffer)
			{
				if(tIsToAddressRegions && ((tInfoOfSequential->gWriteBufferLength == 0) ||
						tIsToExtendToASecondBufferAndOnly))
				{
					/*
						BECAUSE WE WANT TO UNLOCK AS SOON AS POSSIBLE, WE END UP REGION LOCKING
						THE CURRENT BUFFER SPAN WHICH IS YET TO BE FLUSHED IF NECESSARY OR READ, 
						AND THE NEXT BUFFER SPAN WHICH IS TO FILLED WITH NEW DATA.
					*/
					if(crx_c_pthread_rwlock_wrlock(&(pThis->gPrivate_rwLock__lockedPortions)) == 0)
					{
						crx_c_tree_removeElement(&(pThis->gPrivate_portions__locked),
								&(tInfoOfSequential->gPortion__lock));

						tInfoOfSequential->gPortion__lock.gBaseOffset = tInfoOfSequential->gOffset;
						tInfoOfSequential->gPortion__lock.gLength = 
								tInfoOfSequential->gWriteBufferLength + 
								tInfoOfSequential->gBufferCapacity;
						
						if(!crx_c_tree_insertElement(&(pThis->gPrivate_portions__locked), 
								&(tInfoOfSequential->gPortion__lock)))
							{vIsNoError = false;}

						crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock__lockedPortions));
					}
					else
						{vIsNoError = false;}
				}

				if(tIsLocked) //MEANING pThis->gPrivate_isRwLockInitialized
				{
					crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock));
					tIsLocked = false;
				}

				if(vIsNoError && tIsToExtendToASecondBufferAndOnly)
				{
					if(tInfoOfSequential->gIndex < tInfoOfSequential->gWriteBufferLength)
					{
						memcpy(pBytes, tInfoOfSequential->gWriteBuffer + tInfoOfSequential->gIndex,
								tInfoOfSequential->gWriteBufferLength - tInfoOfSequential->gIndex);
						tRemainingLength = tRemainingLength - 
								(tInfoOfSequential->gWriteBufferLength - tInfoOfSequential->gIndex);
					}

					if(tInfoOfSequential->gIsDirty)
					{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD))
						vIsNoError = crx_c_os_fileSystem_internal_posix_doPwrite(
								pThis->gPrivate_osFileHandle, 
								tInfoOfSequential->gWriteBuffer, 
								tInfoOfSequential->gIndex, 
								tInfoOfSequential->gOffset);
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
						vIsNoError = crx_c_os_fileSystem_private_win32_doWriteFile(
								pThis->gPrivate_osFileHandle,
								tInfoOfSequential->gWriteBuffer, 
								tInfoOfSequential->gIndex, 
								tInfoOfSequential->gOffset);
#endif
					}
					
					if(vIsNoError)
					{
						tInfoOfSequential->gOffset = tInfoOfSequential->gOffset + 
								tInfoOfSequential->gWriteBufferLength;
						tInfoOfSequential->gIndex = 0;
						tInfoOfSequential->gIsDirty = false;
						tInfoOfSequential->gWriteBufferLength = 0;
					}
					
					if(vIsNoError)
					{
						int64_t tNumberOfBytesRead = 0;

#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD))
						tNumberOfBytesRead = crx_c_os_fileSystem_internal_posix_doPread(
								pThis->gPrivate_osFileHandle,
								tInfoOfSequential->gWriteBuffer,
								tInfoOfSequential->gBufferCapacity, 
								tInfoOfSequential->gOffset);
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
						tNumberOfBytesRead = crx_c_os_fileSystem_private_win32_doReadFile(
								pThis->gPrivate_osFileHandle,
								tInfoOfSequential->gWriteBuffer,
								tInfoOfSequential->gBufferCapacity, 
								tInfoOfSequential->gOffset);
#endif

						if(tNumberOfBytesRead >= 0)
							{tInfoOfSequential->gWriteBufferLength = tNumberOfBytesRead;}
						else
							{vIsNoError = false;}
					}
				}

				if(vIsNoError && (tRemainingLength > 0))
				{
					size_t tNumberOfByteToRead = tRemainingLength;

					if(tRemainingLength > (tInfoOfSequential->gWriteBufferLength - 
							tInfoOfSequential->gIndex))
					{
						tNumberOfByteToRead = (tInfoOfSequential->gWriteBufferLength - 
								tInfoOfSequential->gIndex);
					}

					if(tNumberOfByteToRead > 0)
					{
						memcpy(pBytes, tInfoOfSequential->gWriteBuffer + tInfoOfSequential->gIndex,
								tNumberOfByteToRead);
						tInfoOfSequential->gIndex = tInfoOfSequential->gIndex + tNumberOfByteToRead;

						tRemainingLength = tRemainingLength - tNumberOfByteToRead;
					}
				}
			}
			else if(vIsNoError && !tIsToAddressRegions)
			{
				size_t tTemp = tRemainingLength;

				if(tLocked) //MEANING pThis->gPrivate_isRwLockInitialized
				{
					crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock));
					tIsLocked = false;
				}
				
				if(tRemainingLength > (tInfoOfSequential->gBufferCapacity - 
						tInfoOfSequential->gIndex))
					{tTemp = (tInfoOfSequential->gWriteBufferLength - tInfoOfSequential->gIndex);}

				memcpy(pBytes, tInfoOfSequential->gWriteBuffer + tInfoOfSequential->gIndex, tTemp);
				tRemainingLength = tRemainingLength - tTemp;
				tInfoOfSequential->gIndex = tInfoOfSequential->gIndex + tTemp;

				if(tInfoOfSequential->gIndex > tInfoOfSequential->gWriteBufferLength)
					{tInfoOfSequential->gWriteBufferLength = tInfoOfSequential->gIndex;}

				if(tInfoOfSequential->gIsDirty && (tRemainingLength > 0))
				{
					/*
						NO REASON TO FLUSH PAST tInfoOfSequential->gIndex ALL THE WAY TO 
								tInfoOfSequential->gWriteBufferLength BECAUSE THE BYTES THERE
								COULD NOT HAVE BEEN CHANGED
					*/
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD))
					vIsNoError = crx_c_os_fileSystem_internal_posix_doPwrite(
							pThis->gPrivate_osFileHandle, 
							tInfoOfSequential->gWriteBuffer, 
							tInfoOfSequential->gIndex, 
							tInfoOfSequential->gOffset);
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
					vIsNoError = crx_c_os_fileSystem_private_win32_doWriteFile(
							pThis->gPrivate_osFileHandle,
							tInfoOfSequential->gWriteBuffer, 
							tInfoOfSequential->gIndex, 
							tInfoOfSequential->gOffset);
#endif

					if(vIsNoError)
					{
						tInfoOfSequential->gOffset = tInfoOfSequential->gOffset + 
								tInfoOfSequential->gWriteBufferLength;
						tInfoOfSequential->gIndex = 0;
						tInfoOfSequential->gIsDirty = false;
						tInfoOfSequential->gWriteBufferLength = 0;
					}
					else
					{
						tInfoOfSequential->gIndex = tInfoOfSequential->gIndex - tTemp;
						//THIS MIGHT DISCARD SOME BUFFERED READ BYTES, BUT THAT IS FINE
						tInfoOfSequential->gWriteBufferLength = tInfoOfSequential->gIndex;
					}
				}

				if(vIsNoError && (tRemainingLength > 0))
				{
					if(tRemainingLength > tInfoOfSequential->gBufferCapacity)
					{
						int64_t tNumberOfBytesRead = 0;

#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD))
						tNumberOfBytesRead = crx_c_os_fileSystem_internal_posix_doPread(
								pThis->gPrivate_osFileHandle, 
								pBytes + (tLength - tRemainingLength), 
								tRemainingLength, 
								tInfoOfSequential->gOffset);
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
						tNumberOfBytesRead = crx_c_os_fileSystem_private_win32_doReadFile(
								pThis->gPrivate_osFileHandle, 
								pBytes + (tLength - tRemainingLength), 
								tRemainingLength, 
								tInfoOfSequential->gOffset);
#endif

						if(tNumberOfBytesRead >= 0)
						{
							tInfoOfSequential->gOffset = tInfoOfSequential->gOffset + 
									tNumberOfBytesRead;
							tRemainingLength = tRemainingLength - tNumberOfBytesRead;
						}
						else
							{vIsNoError = false;}
					}
					else
					{
						int64_t tNumberOfBytesRead = 0;

#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD))
						tNumberOfBytesRead = crx_c_os_fileSystem_internal_posix_doPread(
								pThis->gPrivate_osFileHandle,
								tInfoOfSequential->gWriteBuffer,
								tInfoOfSequential->gBufferCapacity, 
								tInfoOfSequential->gOffset);
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
						tNumberOfBytesRead = crx_c_os_fileSystem_private_win32_doReadFile(
								pThis->gPrivate_osFileHandle,
								tInfoOfSequential->gWriteBuffer,
								tInfoOfSequential->gBufferCapacity, 
								tInfoOfSequential->gOffset);
#endif

						if(tNumberOfBytesRead >= 0)
						{
							size_t tNumberOfBytesToRead = tRemainingLength;

							tInfoOfSequential->gWriteBufferLength = tNumberOfBytesRead;

							if(tRemainingLength > tNumberOfBytesRead)
								{tNumberOfBytesToRead = tNumberOfBytesRead;}

							memcpy(pBytes, tInfoOfSequential->gWriteBuffer, tNumberOfBytesToRead);
							tInfoOfSequential->gIndex = tNumberOfBytesToRead;
							tRemainingLength = tRemainingLength - tNumberOfBytesToRead;
						}
						else
							{vIsNoError = false;}
					}
				}
			}
			else if(vIsNoError)
			{
				/*NOTE: CODE HERE WAS WRITTEN WITH OVERLAPPING REGIONS IN MIND. OVERLAPPING REGIONS
							ARE NO LONGER ALLOWED, BUT KEEP THE ALREADY EXISTING CODE THAT MIGHT
							BE ADDRESSING THIS CASE.*/
				Crx_C_Os_FileSystem_Io_Region * _tBuffer[8];
				Crx_C_Array/*<Crx_C_Os_FileSystem_Io_Region *>*/ tRegions /*= ?*/;
				size_t tCurrentOffset = tOriginalOffset;
				size_t tExclusiveEndOffset = tOriginalOffset + tLength;
				int32_t tUnlockResult = 0;
				size_t tUnlockCount = 0;

				crx_c_array_construct2(&tRegions, 
						crx_c_os_fileSystem_io_region_getTypeBluePrintForPointer(), &(_tBuffer[0]), 
						8, 8);

				//REMEMBER: WE NEED TO HAVE Io::gPrivate_rwLock LOCKED FOR THIS FUCNITON
				crx_c_os_fileSystem_io_private_unsafeCheckIfRegionsInRangeAndOptionallyMarkThem(
						pThis, tOriginalOffset, tOriginalOffset + tLength, 
						&vIsNoError, Crx_C_Array/*<Crx_C_Os_FileSystem_Io_Region *>*/ tRegions);

				if(vIsNoError)
				{
					if(crx_c_pthread_rwlock_wrlock(&(pThis->gPrivate_rwLock__lockedPortions)) == 0)
					{
						crx_c_tree_removeElement(&(pThis->gPrivate_portions__locked), 
								&(tInfoOfSequential->gPortion__lock));

						/*
							WE MIGHT BE LOCKING MORE THAN THE REQUIRED REGION, BUT THAT IS FINE.
							THE ALGORITHM MUST CONSIDER FAILURES LATER ON.
						*/
						if(tInfoOfSequential->gWriteBufferLength > 0)
						{
							tInfoOfSequential->gPortion__lock.gBaseOffset = 
									tInfoOfSequential->gOffset;
							tInfoOfSequential->gPortion__lock.gLength = 
									tInfoOfSequential->gBufferCapacity + tLength;
						}
						else
						{
							//IN A READ, IF THE BUFFER IS EMPTY, IT WILL REMAIN EMPTY.
							tInfoOfSequential->gPortion__lock.gBaseOffset = tOriginalOffset;
							tInfoOfSequential->gPortion__lock.gLength = tLength;
						}
						
						if(!crx_c_tree_insertElement(&(pThis->gPrivate_portions__locked), 
								&(tInfoOfSequential->gPortion__lock)))
							{vIsNoError = false;}

						crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock__lockedPortions));
					}
					else
						{vIsNoError = false;}
				}

				crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock));
				tIsLocked = false;

				if(vIsNoError && 
						(tInfoOfSequential->gIndex < tInfoOfSequential->gWriteBufferLength))
				{
					size_t tLength2 = tInfoOfSequential->gWriteBufferLength - 
							tInfoOfSequential->gIndex;

					if(tLength2 > tLength)
						{tLength2 = tLength;}

					memcpy(pBytes, tInfoOfSequential->gWriteBuffer + tInfoOfSequential->gIndex,
							tLength2);

					tCurrentOffset = tCurrentOffset + tLength2;
				}

				if(vIsNoError)
				{
					if(tInfoOfSequential->gIsDirty/* && (tInfoOfSequential->gIndex > 0)*/)
					{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD))
						vIsNoError = crx_c_os_fileSystem_internal_posix_doPwrite(
								pThis->gPrivate_osFileHandle, 
								tInfoOfSequential->gWriteBuffer, 
								tInfoOfSequential->gIndex, 
								tInfoOfSequential->gOffset);
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
						vIsNoError = crx_c_os_fileSystem_private_win32_doWriteFile(
								pThis->gPrivate_osFileHandle,
								tInfoOfSequential->gWriteBuffer, 
								tInfoOfSequential->gIndex, 
								tInfoOfSequential->gOffset);
#endif
					}
					
					if(vIsNoError)
					{
						tInfoOfSequential->gOffset = tCurrentOffset;
						tInfoOfSequential->gIndex = 0;
						tInfoOfSequential->gIsDirty = false;
						tInfoOfSequential->gWriteBufferLength = 0;
					}
				}

				if(vIsNoError && (tCurrentOffset < tExclusiveEndOffset))
				{
					CRX_FOR(size_t tI = 0, tI < crx_c_array_getLength(&tRegions), tI++)
					{
						Crx_C_Os_FileSystem_Io_Region * tRegion2 = 
								*((Crx_C_Os_FileSystem_Io_Region * *)crx_c_array_get(&tRegions, 
								tI));

						if(tCurrentOffset < tRegion2->gBaseOffset)
						{
							size_t tLength2 = tRegion2->gBaseOffset - tCurrentOffset;
							
							if(tLength2 > (tExclusiveEndOffset - tCurrentOffset))
								{tLength2 = tExclusiveEndOffset - tCurrentOffset;}

#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD))
							//WE EXPECT NO FAILURE AND tLength2 BYTES TO BE READ
							if(crx_c_os_fileSystem_internal_posix_doPread(
									pThis->gPrivate_osFileHandle, 
									pBytes + (tCurrentOffset - tOriginalOffset), 
									tLength2, tCurrentOffset) < 0)
								{vIsNoError = false;}
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
							//WE EXPECT NO FAILURE AND tLength2 BYTES TO BE READ
							if(crx_c_os_fileSystem_private_win32_doReadFile(
									pThis->gPrivate_osFileHandle,
									pBytes + (tCurrentOffset - tOriginalOffset), 
									tLength2, tCurrentOffset) < 0)
								{vIsNoError = false;}
#endif

							if(vIsNoError)
								{tCurrentOffset = tCurrentOffset + tLength2;}
						}

						if(vIsNoError && (tCurrentOffset < tExclusiveEndOffset) &&
								(tCurrentOffset >= tRegion2->gBaseOffset) &&
								(tCurrentOffset < (tRegion2->gBaseOffset + tRegion2->gLength)))
						{
							unsigned char * tPointerIntoRegion = tRegion2->gBaseAddress + 
									(tCurrentOffset - tRegion2->gBaseOffset);
							size_t tLength2 = (tRegion2->gBaseOffset + tRegion2->gLength) - 
									tCurrentOffset;

							if(tCurrentOffset + tLength2 > tExclusiveEndOffset)
								{tLength2 = tExclusiveEndOffset - tCurrentOffset;}

							memcpy(pBytes + (tCurrentOffset - tOriginalOffset), tPointerIntoRegion,
									tLength2);
							tCurrentOffset = tCurrentOffset + tLength2;
						}

						if(!vIsNoError || (tCurrentOffset >= tExclusiveEndOffset))
							{break;}
					}
					CRX_ENDFOR

					if(vIsNoError && (tCurrentOffset < tExclusiveEndOffset))
					{
						int64_t tNumberOfBytesRead = 0;

#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD))
						tNumberOfBytesRead = crx_c_os_fileSystem_internal_posix_doPread(
								pThis->gPrivate_osFileHandle,
								pBytes + (tCurrentOffset - tOriginalOffset), 
								tExclusiveEndOffset - tCurrentOffset, 
								tCurrentOffset);
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
						tNumberOfBytesRead = crx_c_os_fileSystem_private_win32_doReadFile(
								pThis->gPrivate_osFileHandle,
								pBytes + (tCurrentOffset - tOriginalOffset),
								tExclusiveEndOffset - tCurrentOffset,
								tCurrentOffset);
#endif

						if(tNumberOfBytesRead >= 0)
							{tCurrentOffset = tCurrentOffset + tNumberOfBytesRead;}
						else
							{vIsNoError = false;}
					}

					if(vIsNoError)
						{tInfoOfSequential->gOffset = tCurrentOffset;}
				}
				
				if(vIsNoError)
					{tRemainingLength = tExclusiveEndOffset - tCurrentOffset;}

				if(crx_c_os_fileSystem_io_private_patientlyLockForReadRwLock(pThis, 
						&(pThis->gPrivate_rwLock)) == 0)
				{
					tIsLocked = true;

					CRX_FOR(size_t tI = 0, tI < crx_c_array_getLength(&tRegions), tI++)
					{
						Crx_C_Os_FileSystem_Io_Region * tRegion2 = 
								*((Crx_C_Os_FileSystem_Io_Region * *)crx_c_array_get(&tRegions, 
								tI));

						if(crx_c_os_fileSystem_io_private_patientlyLockForWriteRwLock(pThis,
								&(pThis->gPrivate_rwLock__lockedPortions)) == 0)
						{//TRYING TO AVOID THE USE OF ATOMICS FOR THE TIME BEING
							tRegion2->gNumberOfHeldPointers = tRegion2->gNumberOfHeldPointers - 1;
							crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock__lockedPortions));
						}
					}
					CRX_ENDFOR

					if(crx_c_os_fileSystem_io_private_patientlyLockForWriteRwLock(pThis,
							&(pThis->gPrivate_rwLock__lockedPortions)) == 0)
					{
						crx_c_tree_removeElement(&(pThis->gPrivate_portions__locked), 
								&(tInfoOfSequential->gPortion__lock));

						if(tInfoOfSequential->gWriteBufferLength > 0) //WE FAILED BEFORE FLUSH
						{
							tInfoOfSequential->gPortion__lock.gBaseOffset = 
									tInfoOfSequential->gOffset;
							tInfoOfSequential->gPortion__lock.gLength = 
									tInfoOfSequential->gBufferCapacity;

							crx_c_tree_insertElement(&(pThis->gPrivate_portions__locked), 
									&(tInfoOfSequential->gPortion__lock));
						}
								
						crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock__lockedPortions));
					}

					crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock));
				}
				
				crx_c_array_destruct(&tRegions);
			}
			
			if(!tIsLocked && pThis->gPrivate_isRwLockInitialized)
			{
				tIsLocked = (crx_c_os_fileSystem_io_private_patientlyLockForReadRwLock(pThis, 
						&(pThis->gPrivate_rwLock)) == 0);
			}
			
			if(tIsLocked)
			{
				if(crx_c_os_fileSystem_io_private_patientlyLockForWriteRwLock(pThis,
						&(pThis->gPrivate_rwLock__lockedPortions)) == 0)
				{
					//TRYING TO AVOID THE USE OF ATOMICS FOR THE TIME BEING
					pThis->gPrivate_numberOfActiveSequentialRequests = 
							pThis->gPrivate_numberOfActiveSequentialRequests - 1;
					crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock__lockedPortions));
				}
			}
	
			if(vIsNoError)
				{vReturn = tLength - tRemainingLength;}
		}
		else if(vIsNoError && 
				(pThis->gPrivate_osFileHandle__2 != CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE))
		{
		}
	}
	
	return vReturn;
	CRX_SCOPE_END
}
CRX__LIB__PUBLIC_C_FUNCTION() bool crx_c_os_fileSystem_io_seek(
		Crx_C_Os_FileSystem_Io * pThis, uint32_t pSequentialAccessorId,
		size_t pOffset)
{
	if(!(pThis->gPrivate_isRwLockInitialized) ||
			(crx_c_pthread_rwlock_rdlock(&(pThis->gPrivate_rwLock)) == 0))
	{
		Crx_C_Os_FileSystem_Io_InfoOfSequential * tInfoOfSequential = NULL

		if(crx_c_hashTable_hasKey(&(pThis->gPrivate_infoOfSequentials), &pSequentialAccessorId))
		{
			tInfoOfSequential = ((Crx_C_Os_FileSystem_Io_InfoOfSequential *)crx_c_hashTable_get(
					&(pThis->gPrivate_infoOfSequentials), &pSequentialAccessorId));

			if(!tInfoOfSequential->gIsInUse)
				{tInfoOfSequential->gIsInUse = true;}
			else
				{vReturn = false;}
		}
		else
			{vReturn = false;}

		if(vReturn && 
				(pThis->gPrivate_osFileHandle != CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE))
		{
			bool tIsToAddressRegions = false;
			size_t tCurrentSize = pThis->gPrivate_maximumTraversedByteSize;

			if(pThis->gPrivate_isRwLockInitialized)
			{
				//AVOIDING ATOMICS FOR THE TIME BEING
				if(crx_c_pthread_rwlock_wrlock(&(pThis->gPrivate_rwLock__lockedPortions)) == 0)
				{
					pThis->gPrivate_numberOfActiveSequentialRequests = 
							pThis->gPrivate_numberOfActiveSequentialRequests + 1;

					crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock__lockedPortions));
				}
				else
					{vReturn = false;}
			}

			if(pThis->gPrivate_isRwLockInitialized)
				{crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock));}

			if(vReturn)
			{
				size_t tLength = pLength;

				if(tInfoOfSequential->gIsDirty)
				{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD))
					vReturn = crx_c_os_fileSystem_internal_posix_doPwrite(
							pThis->gPrivate_osFileHandle, 
							tInfoOfSequential->gWriteBuffer, 
							tInfoOfSequential->gIndex, 
							tInfoOfSequential->gOffset);
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
					vReturn = crx_c_os_fileSystem_private_win32_doWriteFile(
							pThis->gPrivate_osFileHandle,
							tInfoOfSequential->gWriteBuffer, 
							tInfoOfSequential->gIndex, 
							tInfoOfSequential->gOffset);
#endif
				}

				if(vReturn)
				{
					tInfoOfSequential->gOffset = tInfoOfSequential->gOffset + 
							tInfoOfSequential->gIndex;
					tInfoOfSequential->gIndex = 0;
					tInfoOfSequential->gIsDirty = false;
					tInfoOfSequential->gWriteBufferLength = 0;

					if(pThis->gPrivate_isRwLockInitialized)
						{vReturn = (crx_c_pthread_rwlock_rdlock(&(pThis->gPrivate_rwLock)) != 0);}
				}
			}
			
			if(vReturn) 
			{
				if(pOffset < pThis->gPrivate_maximumTraversedByteSize)
					{tInfoOfSequential->gOffset = pOffset;}
				else
				{
					if((crx_c_tree_getSize(&(pThis->gPrivate_regionsByOffset)) == 0) ||
							(pThis->gPrivate_numberOfActiveRegionCreations == 0))
					{
						if(pThis->gPrivate_isRwLockInitialized)
							{crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock));}

#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD))
						vReturn = crx_c_os_fileSystem_internal_posix_resizeFile(
								pThis->gPrivate_osFileHandle, true, pOffset);
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
						vReturn = crx_c_os_fileSystem_internal_win32_resizeFile(
								pThis->gPrivate_osFileHandle, true, pOffset);
#endif

						if(vReturn)
						{
							pThis->gPrivate_maximumTraversedByteSize = pOffset;
							
							if(pThis->gPrivate_maximumTraversedByteSize > 
									pThis->gPrivate_expectedByteSize)
							{
								pThis->gPrivate_maximumTraversedByteSize = 
										pThis->gPrivate_expectedByteSize;
							}
						}
					}
					else
						{vReturn = false;}
					
					if(vReturn && (!(pThis->gPrivate_isRwLockInitialized) ||
							(crx_c_pthread_rwlock_wrlock(&(pThis->gPrivate_rwLock)) == 0)))
					{
						pThis->gPrivate_maximumTraversedByteSize = tNewSize;
						pThis->gPrivate_expectedByteSize = tNewSize;
					}
				}
			}

			if(pThis->gPrivate_isRwLockInitialized)
			{
				if(crx_c_os_fileSystem_io_private_patientlyLockForWriteRwLock(pThis,
						&(pThis->gPrivate_rwLock__lockedPortions)) == 0)
				{
					//TRYING TO AVOID THE USE OF ATOMICS FOR THE TIME BEING
					pThis->gPrivate_numberOfActiveSequentialRequests = 
							pThis->gPrivate_numberOfActiveSequentialRequests - 1;
					crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock__lockedPortions));
				}
			}
		}
		else if(vReturn && 
				(pThis->gPrivate_osFileHandle__2 != CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE))
		{
			
		}
	}
}

CRX__LIB__PUBLIC_C_FUNCTION() unsigned char * crx_c_os_fileSystem_io_getPointerToRegion(
		Crx_C_Os_FileSystem_Io * pThis, uint64_t pStartOffset, uint64_t pLength)
{
	CRX_SCOPE_META
	if((pLength > CRX__OS__FILE_SYSTEM__MAXIMUM_REGION_SIZE) || 
			(pThis->gPRIVATE__REGION_ALIGNMENT_VALUE > CRX__OS__FILE_SYSTEM__MAXIMUM_REGION_SIZE) ||
			(pThis->gPrivate_fileAccessMode == 
			CRX__C__OS__FILE_SYSTEM__FILE_ACCESS_MODE__WRITE_ONLY) ||
			(pStartOffset > INT64_MAX))
		{return NULL;}

	CRX_SCOPE
	uint64_t vStartOffset = pStartOffset & (~(pThis->gPRIVATE__REGION_ALIGNMENT_VALUE - 1));
	uint32_t vLength = ((uint32_t)((((uint64_t)pLength) + pThis->gPRIVATE__REGION_ALIGNMENT_VALUE - 
			1) & (~(pThis->gPRIVATE__REGION_ALIGNMENT_VALUE - 1))));
	bool vIsNoError = true;
	unsigned char * vReturn = NULL;

	crx_c_os_fileSystem_io_private_prepareAndGetFileHandleForRegionalAccess(pThis);

	if(!(pThis->gPrivate_isRwLockInitialized) && !crx_c_pthread_crx_isSameThreadAs(
			&(pThis->gPrivate_rawId)))
		{vIsNoError = false;}

	if(vIsNoError && (!(pThis->gPrivate_isRwLockInitialized) ||
			(crx_c_pthread_rwlock_wrlock(&(pThis->gPrivate_rwLock)) == 0)))
	{
		bool tIsLocked = pThis->gPrivate_isRwLockInitialized;

		if(pThis->gPrivate_osFileHandle__2 != CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE)
		{
			Crx_C_Os_FileSystem_Io_Region * tRegion = 
					crx_c_os_fileSystem_io_private_unsafeGetTheRegionThatFullyEncapsulatesTheSpan(
					pThis, vStartOffset, vStartOffset + vLength);

			if(tRegion != NULL)
			{
				//STRICTLY, Io::gPrivate_rwLock__lockedPortions IS NOT REQUIRED HERE BECAUSE WE HAVE
				//		A WRITE LOCK ON Io::gPrivate_rwLock, BUT THE SCHEMA REQUIRES IT, HOWEVER 
				//		IGNORING SCHEMA FOR NOW.
				tRegion->gNumberOfHeldPointers = tRegion->gNumberOfHeldPointers + 1;

				vReturn = tRegion->gBaseAddress + (vStartOffset - tRegion->gBaseOffset);

				if(tIsLocked)
				{
					crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock));
					tIsLocked = false;
				}
			}
		}
		else
			{vIsNoError = false;}
		
		if(vIsNoError && (vReturn == NULL))
		{
			if(pThis->gPRIVATE__REGION_ALIGNMENT_VALUE > 
					CRX__C__OS__FILE_SYSTEM__IO__MAXIMUM_MEMORY_PAGE_SIZE_TO_CORRECT_BUFFER_WITH)
			{
				if(crx_c_hashTable_getSize(&(pThis->gPrivate_infoOfSequentials)) > 1)
					{vIsNoError = false;}
			}
		}

		if(vIsNoError && (vReturn == NULL))
		{
			bool tIsGrowing = false;
			bool tWasGrown = false;
			Crx_C_Os_FileSystem_Io_Region * tRegion = NULL;

			if(pThis->gPrivate_numberOfActiveSequentialRequests != 0)
				{vIsNoError = false;}
			if(vIsNoError && crx_c_os_fileSystem_io_private_unsafeGetIsExtentLocked(pThis, 
					vStartOffset, vStartOffset + vLength, &vIsNoError))
				{vIsNoError = false;}
			if(vIsNoError &&
					!crx_c_os_fileSystem_io_private_unsafeCheckIfRegionsInRangeAndOptionallyMarkThem(
					pThis, vStartOffset, vStartOffset + vLength, &vIsNoError, NULL))
				{vIsNoError = false;}
			if(vIsNoError && (pThis->gPrivate_expectedByteSize > pThis->gPrivate_hiddenByteSize))
			{
				tIsGrowing = true;

				if((vStartOffset + vLength) > pThis->gPrivate_expectedByteSize)
					{vIsNoError = false;}
				else if(pThis->gPrivate_numberOfActiveRegionCreations != 0)
				{
					tIsGrowing = false;
					
					/* WE HAVE TO WAIT FOR THE ACTIVE REGION CREATION THAT IS EXPECTED TO BE 
							CURRENTLY GROWING THE FILE, AND BECAUSE ON WINDOWS WE HAVE TO KNOW
							THE SIZE OF THE FILE WE WANT BEFORE HAND, WE CAN STILL CAN NOT BYPASS
							THE WAIT EVEN IF (vStartOffset + vLength) IS SMALLER THAN
							pThis->gPrivate_hiddenByteSize.
					*/
					vIsNoError = false;
				}
			}
			if(vIsNoError && !tIsGrowing && (pThis->gPrivate_numberOfActiveRegionCreations == 0))
			{
				/*
					THE FIRST THREAD TO CREATE A REGION IS RESPONSIBLE FOR RESIZING THE FILE IF 
							NECESSARY, AND CREATING SECONDARY HANDLES RELATED TO MAPPING FILES.
							IN THIS CASE, WE ARE THE FIRST THREAD CREATING A REGION, AND WE ARE
							NOT REQUIRED TO GROW THE FILE BEYOND ITS CURRENT 'HIDDEN', MEANING
							ACTUAL, SIZE ON DISK.
				*/
#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
				pThis->gPrivate_mappingHandle = 
						crx_c_os_fileSystem_private_win32_doCreateFileMapping(
						&(pThis->gPrivate_osFileHandle__2), ((pThis->gPrivate_fileAccessMode == 
						CRX__C__OS__FILE_SYSTEM__FILE_ACCESS_MODE__READ_ONLY) ? PAGE_READONLY :
						PAGE_READWRITE), pThis->gPrivate_hiddenByteSize, true);

				if(pThis->gPrivate_mappingHandle == NULL)
					{vIsNoError = false;}
#endif
			}

			if(vIsNoError && pThis->gPrivate_isRwLockInitialized)
			{
				/*
					WE CAN NOT ADD THE Region INSTANCE TO THE Io INSTANCE, UNTIL THE UNDERLYING 
							MAPPING IS DONE, BECAUSE OTHERWISE, SEQUENTIAL ACCESSORS OF THE Io 
							INSTANCE MIGHT ACCESS IT. THE FOLLOWING HELPS UNLOCK Io::gPrivate_rwLock 
							QUICKLY WITHOUT HAVING TO CREATE A Region INSTANCE FIRST, WHILE STILL 
							SIGNALLING THAT REGIONS EXISTS EVEN THOUGH NOT ADDED TO THE Io INSTANCE 
							YET.
				*/
				pThis->gPrivate_numberOfActiveRegionCreations = 
						pThis->gPrivate_numberOfActiveRegionCreations + 1;
			}

			if(vIsNoError)
			{
				tRegion = crx_c_os_fileSystem_io_region_new();

				//STRICTLY, Io::gPrivate_rwLock__lockedPortions IS NOT REQUIRED HERE BECAUSE WE HAVE
				//		A WRITE LOCK ON Io::gPrivate_rwLock, BUT STICKING TO SCHEMA
				if(tRegion != NULL)
				{
					if(pThis->gPrivate_isRwLockInitialized)
					{
						if(crx_c_pthread_rwlock_wrlock(
								&(pThis->gPrivate_rwLock__lockedPortions)) == 0)
						{
							tRegion->gBaseOffset = vStartOffset;
							tRegion->gLength = vLength;
							tRegion->gNumberOfHeldPointers = 1;
							tRegion->gPortion__lock.gBaseOffset = vStartOffset;
							tRegion->gPortion__lock.gLength = vLength;

							crx_c_tree_insertElement(&(pThis->gPrivate_portions__locked),
									&(tRegion->gPortion__lock));

							crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock__lockedPortions));
						}
						else
						{
							crx_c_os_fileSystem_io_region_free(tRegion);
							tRegion = NULL;

							vIsNoError = false;
						}
					}
				}
				else
					{vIsNoError = false;}

				if(tIsLocked) //MEANING pThis->gPrivate_isRwLockInitialized IS true
				{
					crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock));
					tIsLocked = false;
				}

				
				if(vIsNoError && tIsGrowing)
				{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD))
					vIsNoError = crx_c_os_fileSystem_internal_posix_resizeFile(
							pThis->gPrivate_osFileHandle__2, true, 
							pThis->gPrivate_expectedByteSize);
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
					assert(pThis->gPrivate_mappingHandle == NULL);

					vIsNoError = crx_c_os_fileSystem_internal_win32_resizeFile(
							pThis->gPrivate_osFileHandle__2, true, 
							pThis->gPrivate_expectedByteSize);
#endif

					if(vIsNoError)
					{
						tWasGrown = true;

#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
						pThis->gPrivate_mappingHandle = 
								crx_c_os_fileSystem_private_win32_doCreateFileMapping(
								&(pThis->gPrivate_osFileHandle__2), 
								((pThis->gPrivate_fileAccessMode == 
										CRX__C__OS__FILE_SYSTEM__FILE_ACCESS_MODE__READ_ONLY) ? 
										PAGE_READONLY : PAGE_READWRITE), 
								pThis->gPrivate_expectedByteSize, true);

						if(pThis->gPrivate_mappingHandle == NULL)
							{vIsNoError = false;}
#endif
					}
				}

				if(!vIsNoError)
				{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD))
					/*
						EVEN IF READ ONLY, WE EXPECT CHANGES TO THE MAPPED REGION BY OTHER PROCESSES
						SHOULD BECOME VISIBLE, HENCE THE USE OF MAP_SHARED IN ALL CASES, CONTRARY TO THE
						ORIGINAL DESIGN. THE APPROACH IS SIMPLY PLAYING IT SAFE IN CASE IMPLEMENTATOIN
						DO NOT PROPOGATE CHANGES BY OTHER PROCESS IF MAP_PRIVATE IS USED BY THIS 
						PROCESS.
					*/
					vReturn = crx_c_os_fileSystem_internal_posix_mmap(NULL, vLength, 
							((pThis->gPrivate_fileAccessMode == 
							CRX__C__OS__FILE_SYSTEM__FILE_ACCESS_MODE__READ_ONLY) ? PROT_READ :
							PROT_READ | PROT_WRITE), MAP_SHARED, pThis->gPrivate_osFileHandle__2, 
							((int64_t)vStartOffset));
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
					vReturn = crx_c_os_fileSystem_private_win32_mapViewOfFile(
							pThis->gPrivate_mappingHandle, 
							((pThis->gPrivate_fileAccessMode == 
							CRX__C__OS__FILE_SYSTEM__FILE_ACCESS_MODE__READ_ONLY) ? FILE_MAP_READ :
							FILE_MAP_WRITE), vStartOffset, vLength);
#endif

					if(vReturn != NULL)
					{
						tRegion->gBaseAddress = vReturn;
						vReturn = tRegion->gBaseAddress + (vStartOffset - tRegion->gBaseOffset);
					}
					else
						{vIsNoError = false;}
				}
				else
					{vIsNoError = false;}
			
				if(!(pThis->gPrivate_isRwLockInitialized))
				{
					if(vIsNoError && (crx_c_os_fileSystem_io_private_patientlyLockForWriteRwLock(
							&(pThis->gPrivate_rwLock)) == 0))
						{tIsLocked = true;}
					else
						{vIsNoError = false;}
				}

				if(vIsNoError)
				{
					if(tWasGrown)
						{pThis->gPrivate_hiddenByteSize = pThis->gPrivate_expectedByteSize;}
					
					if((vStartOffset + vLength) > pThis->gPrivate_maximumTraversedByteSize)
						{pThis->gPrivate_maximumTraversedByteSize = vStartOffset + vLength;}

					if(!crx_c_tree_insertElement(&(pThis->gPrivate_regionsByAddress), &tRegion))
						{vIsNoError = false;}
					else
					{
						if(!crx_c_tree_insertElement(&(pThis->gPrivate_regionsByOffset), &tRegion))
						{
							crx_c_tree_removeElement(&(pThis->gPrivate_regionsByAddress), &tRegion);
							vIsNoError = false;
						}
					}
				}
				
				if(!vIsNoError && (tRegion != NULL) && (vReturn != NULL))
				{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD))
					crx_c_os_fileSystem_internal_posix_munmap(tRegion->gBaseAddress, 
							tRegion->gLength);
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
					crx_c_os_fileSystem_private_win32_unmapViewOfFile(
						tRegion->gBaseAddress);
#endif

					vReturn = NULL;
				}

				if(tRegion != NULL)
				{
					if(tIsLocked && (crx_c_os_fileSystem_io_private_patientlyLockForWriteRwLock(
							&(pThis->gPrivate_rwLock__lockedPortions)) == 0))
					{
						crx_c_tree_removeElement(&(pThis->gPrivate_portions__locked),
								&(tRegion->gPortion__lock));

						crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock__lockedPortions));
					}
					else
					{
						/*
							THIS WILL CAUSE A MEMORY LEAK, BUT THIS IS THE APPROACH CURRENTLY.
						*/
						tRegion = NULL;
						vIsNoError = false;
					}
				}

				if(tIsLocked)
				{
					pThis->gPrivate_numberOfActiveRegionCreations = 
							pThis->gPrivate_numberOfActiveRegionCreations - 1;
							
					crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock));
					tIsLocked = false;
				}

				if(!vIsNoError)
				{
					if(tRegion != NULL)
					{
						crx_c_os_fileSystem_io_region_free(tRegion);
						tRegion = NULL;
					}
				}
			}
		}
	}
	else
		{vIsNoError = false;}
	
	return vReturn;

	CRX_SCOPE_END
}
CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_io_returnPointerToRegion(
		Crx_C_Os_FileSystem_Io * pThis, void * CRX_NOT_NULL pPointer)
{
	CRX_SCOPE_META
	if(pPointer == NULL)
		{return true;}

	CRX_SCOPE
	bool vIsNoError = true;

	if(!(pThis->gPrivate_isRwLockInitialized) && !crx_c_pthread_crx_isSameThreadAs(
			&(pThis->gPrivate_rawId)))
		{vIsNoError = false;}

	if(vIsNoError && (!(pThis->gPrivate_isRwLockInitialized) ||
			(crx_c_pthread_rwlock_wrlock(&(pThis->gPrivate_rwLock)) == 0)))
	{
		bool tIsLocked = pThis->gPrivate_isRwLockInitialized;

		if(pThis->gPrivate_osFileHandle__2 != CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE)
		{
			Crx_C_Os_FileSystem_Io_Region * tRegion = 
					crx_c_os_fileSystem_io_private_unsafeGetTheRegionThatContainsTheAddress(pThis, 
					pPointer);

			if(tRegion != NULL)
			{
				//STRICTLY, Io::gPrivate_rwLock__lockedPortions IS NOT REQUIRED HERE BECAUSE WE HAVE
				//		A WRITE LOCK ON Io::gPrivate_rwLock, BUT THE SCHEMA REQUIRES IT, HOWEVER 
				//		IGNORING SCHEMA FOR NOW.
				tRegion->gNumberOfHeldPointers = tRegion->gNumberOfHeldPointers - 1;

				if(tRegion->gNumberOfHeldPointers == 0)
				{
					crx_c_tree_removeElement(&(pThis->gPrivate_regionsByAddress), &tRegion);
					crx_c_tree_removeElement(&(pThis->gPrivate_regionsByOffset), &tRegion);
				}
				else
					{tRegion = NULL;}

				if(tIsLocked)
				{
					crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock));
					tIsLocked = false;
				}

				if(tRegion != NULL)
				{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD))
					crx_c_os_fileSystem_internal_posix_munmap(tRegion->gBaseAddress, 
							tRegion->gLength);
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
					crx_c_os_fileSystem_private_win32_unmapViewOfFile(
							tRegion->gBaseAddress);
#endif

					crx_c_os_fileSystem_io_region_free(tRegion);
					tRegion = NULL;
				}
			}
		}
	}
	else
		{vIsNoError = false;}
	
	return vIsNoError;
	CRX_SCOPE_END
}

CRX__LIB__PUBLIC_C_FUNCTION() unsigned char * crx_c_os_fileSystem_io_private_unsafeCreateMemoryMapForSequentialAccess(
		Crx_C_Os_FileSystem_Io * pThis, uint64_t pStartOffset, uint64_t pLength)
{
CRX_SCOPE_META
	if((pLength > CRX__OS__FILE_SYSTEM__MAXIMUM_REGION_SIZE) || 
			(pThis->gPRIVATE__REGION_ALIGNMENT_VALUE > CRX__OS__FILE_SYSTEM__MAXIMUM_REGION_SIZE) ||
			(pThis->gPrivate_fileAccessMode == 
			CRX__C__OS__FILE_SYSTEM__FILE_ACCESS_MODE__WRITE_ONLY) ||
			(pStartOffset > INT64_MAX))
		{return NULL;}

	CRX_SCOPE
	uint64_t vStartOffset = pStartOffset & (~(pThis->gPRIVATE__REGION_ALIGNMENT_VALUE - 1));
	uint32_t vLength = ((uint32_t)((((uint64_t)pLength) + pThis->gPRIVATE__REGION_ALIGNMENT_VALUE - 
			1) & (~(pThis->gPRIVATE__REGION_ALIGNMENT_VALUE - 1))));
	bool vIsNoError = true;
	unsigned char * vReturn = NULL;

	if(vIsNoError)
	{
		if(pThis->gPrivate_osFileHandle__2 != CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE)
		{
			Crx_C_Os_FileSystem_Io_Region * tRegion = 
					crx_c_os_fileSystem_io_private_unsafeGetTheRegionThatFullyEncapsulatesTheSpan(
					pThis, vStartOffset, vStartOffset + vLength);

			/*
				THIS SHOULD NOT HAPPEN. THE ONE CALLING THIS FUNCTION IS THE SEQUENTIAL ACCESSOR
				THAT NEEDS TO DO ITS ACCESS USING REGIONAL ACCESS. IF THE ERROR BELOW HAPPENS,
				AND ASSUMING THIS LIBRARY IS CORRECT, THEN THE CLIENT CODE VIOLATED THE DISTANCE
				REQUIREMENT BETWEEN TWO SEQUENTIAL ACCESS.
				
				PERHAPS THIS SOULD BE A FATAL ERROR.
			*/
			if(tRegion != NULL)
			{
				assert(false);
				vIsNoError = false;
			}
		}
		else
			{vIsNoError = false;}
		
		if(vIsNoError && (vReturn == NULL))
		{
			if(pThis->gPRIVATE__REGION_ALIGNMENT_VALUE > 
					CRX__C__OS__FILE_SYSTEM__IO__MAXIMUM_MEMORY_PAGE_SIZE_TO_CORRECT_BUFFER_WITH)
			{
				if(crx_c_hashTable_getSize(&(pThis->gPrivate_infoOfSequentials)) > 1)
					{vIsNoError = false;}
			}
		}

		TODO: WHAT HAPPENS IF THE USER FAVORED REGIONAL ACCESS AND THE SEQUENTIAL ACCESSER NEEDS TO
		GROW THE FILE, AND CURRENTLY THERE ARE NO REGIONS EXISTING.
		if(vIsNoError && (vReturn == NULL))
		{
			bool tIsGrowing = false;
			bool tWasGrown = false;
			Crx_C_Os_FileSystem_Io_Region * tRegion = NULL;

			if(pThis->gPrivate_numberOfActiveSequentialRequests != 0)
				{vIsNoError = false;}
			if(vIsNoError && crx_c_os_fileSystem_io_private_unsafeGetIsExtentLocked(pThis, 
					vStartOffset, vStartOffset + vLength, &vIsNoError))
				{vIsNoError = false;}
			if(vIsNoError &&
					!crx_c_os_fileSystem_io_private_unsafeCheckIfRegionsInRangeAndOptionallyMarkThem(
					pThis, vStartOffset, vStartOffset + vLength, &vIsNoError, NULL))
				{vIsNoError = false;}
			if(vIsNoError && (pThis->gPrivate_expectedByteSize > pThis->gPrivate_hiddenByteSize))
			{
				tIsGrowing = true;

				if((vStartOffset + vLength) > pThis->gPrivate_expectedByteSize)
					{vIsNoError = false;}
				else if(pThis->gPrivate_numberOfActiveRegionCreations != 0)
				{
					tIsGrowing = false;
					
					/* WE HAVE TO WAIT FOR THE ACTIVE REGION CREATION THAT IS EXPECTED TO BE 
							CURRENTLY GROWING THE FILE, AND BECAUSE ON WINDOWS WE HAVE TO KNOW
							THE SIZE OF THE FILE WE WANT BEFORE HAND, WE CAN STILL CAN NOT BYPASS
							THE WAIT EVEN IF (vStartOffset + vLength) IS SMALLER THAN
							pThis->gPrivate_hiddenByteSize.
					*/
					vIsNoError = false;
				}
			}
			if(vIsNoError && !tIsGrowing && (pThis->gPrivate_numberOfActiveRegionCreations == 0))
			{
				/*
					THE FIRST THREAD TO CREATE A REGION IS RESPONSIBLE FOR RESIZING THE FILE IF 
							NECESSARY, AND CREATING SECONDARY HANDLES RELATED TO MAPPING FILES.
							IN THIS CASE, WE ARE THE FIRST THREAD CREATING A REGION, AND WE ARE
							NOT REQUIRED TO GROW THE FILE BEYOND ITS CURRENT 'HIDDEN', MEANING
							ACTUAL, SIZE ON DISK.
				*/
#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
				pThis->gPrivate_mappingHandle = 
						crx_c_os_fileSystem_private_win32_doCreateFileMapping(
						&(pThis->gPrivate_osFileHandle__2), ((pThis->gPrivate_fileAccessMode == 
						CRX__C__OS__FILE_SYSTEM__FILE_ACCESS_MODE__READ_ONLY) ? PAGE_READONLY :
						PAGE_READWRITE), pThis->gPrivate_hiddenByteSize, true);

				if(pThis->gPrivate_mappingHandle == NULL)
					{vIsNoError = false;}
#endif
			}

			if(vIsNoError && pThis->gPrivate_isRwLockInitialized)
			{
				/*
					WE CAN NOT ADD THE Region INSTANCE TO THE Io INSTANCE, UNTIL THE UNDERLYING 
							MAPPING IS DONE, BECAUSE OTHERWISE, SEQUENTIAL ACCESSORS OF THE Io 
							INSTANCE MIGHT ACCESS IT. THE FOLLOWING HELPS UNLOCK Io::gPrivate_rwLock 
							QUICKLY WITHOUT HAVING TO CREATE A Region INSTANCE FIRST, WHILE STILL 
							SIGNALLING THAT REGIONS EXISTS EVEN THOUGH NOT ADDED TO THE Io INSTANCE 
							YET.
				*/
				pThis->gPrivate_numberOfActiveRegionCreations = 
						pThis->gPrivate_numberOfActiveRegionCreations + 1;
			}

			if(vIsNoError)
			{
				tRegion = crx_c_os_fileSystem_io_region_new();

				//STRICTLY, Io::gPrivate_rwLock__lockedPortions IS NOT REQUIRED HERE BECAUSE WE HAVE
				//		A WRITE LOCK ON Io::gPrivate_rwLock, BUT STICKING TO SCHEMA
				if(tRegion != NULL)
				{
					if(pThis->gPrivate_isRwLockInitialized)
					{
						if(crx_c_pthread_rwlock_wrlock(
								&(pThis->gPrivate_rwLock__lockedPortions)) == 0)
						{
							tRegion->gBaseOffset = vStartOffset;
							tRegion->gLength = vLength;
							tRegion->gNumberOfHeldPointers = 1;
							tRegion->gPortion__lock.gBaseOffset = vStartOffset;
							tRegion->gPortion__lock.gLength = vLength;

							crx_c_tree_insertElement(&(pThis->gPrivate_portions__locked),
									&(tRegion->gPortion__lock));

							crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock__lockedPortions));
						}
						else
						{
							crx_c_os_fileSystem_io_region_free(tRegion);
							tRegion = NULL;

							vIsNoError = false;
						}
					}
				}
				else
					{vIsNoError = false;}

				if(tIsLocked) //MEANING pThis->gPrivate_isRwLockInitialized IS true
				{
					crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock));
					tIsLocked = false;
				}

				
				if(vIsNoError && tIsGrowing)
				{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD))
					vIsNoError = crx_c_os_fileSystem_internal_posix_resizeFile(
							pThis->gPrivate_osFileHandle__2, true, 
							pThis->gPrivate_expectedByteSize);
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
					assert(pThis->gPrivate_mappingHandle == NULL);

					vIsNoError = crx_c_os_fileSystem_internal_win32_resizeFile(
							pThis->gPrivate_osFileHandle__2, true, 
							pThis->gPrivate_expectedByteSize);
#endif

					if(vIsNoError)
					{
						tWasGrown = true;

#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
						pThis->gPrivate_mappingHandle = 
								crx_c_os_fileSystem_private_win32_doCreateFileMapping(
								&(pThis->gPrivate_osFileHandle__2), 
								((pThis->gPrivate_fileAccessMode == 
										CRX__C__OS__FILE_SYSTEM__FILE_ACCESS_MODE__READ_ONLY) ? 
										PAGE_READONLY : PAGE_READWRITE), 
								pThis->gPrivate_expectedByteSize, true);

						if(pThis->gPrivate_mappingHandle == NULL)
							{vIsNoError = false;}
#endif
					}
				}

				if(!vIsNoError)
				{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD))
					/*
						EVEN IF READ ONLY, WE EXPECT CHANGES TO THE MAPPED REGION BY OTHER PROCESSES
						SHOULD BECOME VISIBLE, HENCE THE USE OF MAP_SHARED IN ALL CASES, CONTRARY TO THE
						ORIGINAL DESIGN. THE APPROACH IS SIMPLY PLAYING IT SAFE IN CASE IMPLEMENTATOIN
						DO NOT PROPOGATE CHANGES BY OTHER PROCESS IF MAP_PRIVATE IS USED BY THIS 
						PROCESS.
					*/
					vReturn = crx_c_os_fileSystem_internal_posix_mmap(NULL, vLength, 
							((pThis->gPrivate_fileAccessMode == 
							CRX__C__OS__FILE_SYSTEM__FILE_ACCESS_MODE__READ_ONLY) ? PROT_READ :
							PROT_READ | PROT_WRITE), MAP_SHARED, pThis->gPrivate_osFileHandle__2, 
							((int64_t)vStartOffset));
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
					vReturn = crx_c_os_fileSystem_private_win32_mapViewOfFile(
							pThis->gPrivate_mappingHandle, 
							((pThis->gPrivate_fileAccessMode == 
							CRX__C__OS__FILE_SYSTEM__FILE_ACCESS_MODE__READ_ONLY) ? FILE_MAP_READ :
							FILE_MAP_WRITE), vStartOffset, vLength);
#endif

					if(vReturn != NULL)
					{
						tRegion->gBaseAddress = vReturn;
						vReturn = tRegion->gBaseAddress + (vStartOffset - tRegion->gBaseOffset);
					}
					else
						{vIsNoError = false;}
				}
				else
					{vIsNoError = false;}
			
				if(!(pThis->gPrivate_isRwLockInitialized))
				{
					if(vIsNoError && (crx_c_os_fileSystem_io_private_patientlyLockForWriteRwLock(
							&(pThis->gPrivate_rwLock)) == 0))
						{tIsLocked = true;}
					else
						{vIsNoError = false;}
				}

				if(vIsNoError)
				{
					if(tWasGrown)
						{pThis->gPrivate_hiddenByteSize = pThis->gPrivate_expectedByteSize;}
					
					if((vStartOffset + vLength) > pThis->gPrivate_maximumTraversedByteSize)
						{pThis->gPrivate_maximumTraversedByteSize = vStartOffset + vLength;}

					if(!crx_c_tree_insertElement(&(pThis->gPrivate_regionsByAddress), &tRegion))
						{vIsNoError = false;}
					else
					{
						if(!crx_c_tree_insertElement(&(pThis->gPrivate_regionsByOffset), &tRegion))
						{
							crx_c_tree_removeElement(&(pThis->gPrivate_regionsByAddress), &tRegion);
							vIsNoError = false;
						}
					}
				}
				
				if(!vIsNoError && (tRegion != NULL) && (vReturn != NULL))
				{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD))
					crx_c_os_fileSystem_internal_posix_munmap(tRegion->gBaseAddress, 
							tRegion->gLength);
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
					crx_c_os_fileSystem_private_win32_unmapViewOfFile(
						tRegion->gBaseAddress);
#endif

					vReturn = NULL;
				}

				if(tRegion != NULL)
				{
					if(tIsLocked && (crx_c_os_fileSystem_io_private_patientlyLockForWriteRwLock(
							&(pThis->gPrivate_rwLock__lockedPortions)) == 0))
					{
						crx_c_tree_removeElement(&(pThis->gPrivate_portions__locked),
								&(tRegion->gPortion__lock));

						crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock__lockedPortions));
					}
					else
					{
						/*
							THIS WILL CAUSE A MEMORY LEAK, BUT THIS IS THE APPROACH CURRENTLY.
						*/
						tRegion = NULL;
						vIsNoError = false;
					}
				}

				if(tIsLocked)
				{
					pThis->gPrivate_numberOfActiveRegionCreations = 
							pThis->gPrivate_numberOfActiveRegionCreations - 1;
							
					crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock));
					tIsLocked = false;
				}

				if(!vIsNoError)
				{
					if(tRegion != NULL)
					{
						crx_c_os_fileSystem_io_region_free(tRegion);
						tRegion = NULL;
					}
				}
			}
		}
	}
	else
		{vIsNoError = false;}
	
	return vReturn;

	CRX_SCOPE_END
}

/*
	THIS MUST EITHER BE CALLED FROM THE SAME THREAD THAT CREATED THE SEQUENTIAL ACCESSOR WHOSE
	ID IS pSequentialAccessorId, OR CALLED WHILE HAVING A WRITE LOCK ON Io::gPrivate_rwLock
*/
CRX__LIB__PUBLIC_C_FUNCTION() bool crx_c_os_fileSystem_io_flush(
		Crx_C_Os_FileSystem_Io * pThis, uint32_t pSequentialAccessorId)
{
	CRX_SCOPE_META
	if(pThis->gPrivate_fileAccessMode == CRX__C__OS__FILE_SYSTEM__FILE_ACCESS_MODE__READ_ONLY)
		{return true;}
	/*else if(crx_c_os_fileSystem_io_private_prepareAndGetFileHandleForSequentialAccess(pThis) == 
			CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE)
		{return false;}*/

	CRX_SCOPE
	bool vIsNoError = true;

	crx_c_os_fileSystem_io_private_prepareAndGetFileHandleForSequentialAccess(pThis);

	if(!(pThis->gPrivate_isRwLockInitialized) ||
			(crx_c_pthread_rwlock_rdlock(&(pThis->gPrivate_rwLock)) == 0))
	{
		Crx_C_Os_FileSystem_Io_InfoOfSequential * tInfoOfSequential = NULL

		if(crx_c_hashTable_hasKey(&(pThis->gPrivate_infoOfSequentials), &pSequentialAccessorId))
		{
			tInfoOfSequential = ((Crx_C_Os_FileSystem_Io_InfoOfSequential *)crx_c_hashTable_get(
					&(pThis->gPrivate_infoOfSequentials), &pSequentialAccessorId));

			if(!tInfoOfSequential->gIsInUse)
				{tInfoOfSequential->gIsInUse = true;}
			else
				{vIsNoError = false;}
		}
		else
			{vIsNoError = false;}

		if(vIsNoError && 
				(pThis->gPrivate_osFileHandle != CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE))
		{
			//AVOIDING ATOMICS FOR THE TIME BEING
			if(crx_c_pthread_rwlock_wrlock(&(pThis->gPrivate_rwLock__lockedPortions)) == 0)
			{
				pThis->gPrivate_numberOfActiveSequentialRequests = 
						pThis->gPrivate_numberOfActiveSequentialRequests + 1;

				crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock__lockedPortions));
			}
			else
				{vIsNoError = false;}

			crx_c_pthread_rwlock_unlock(&(pThis->gPrivate_rwLock));

			if(vIsNoError)
			{
				if(tInfoOfSequential->gIsDirty)
				{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD))
					vIsNoError = crx_c_os_fileSystem_internal_posix_doPwrite(
							pThis->gPrivate_osFileHandle, 
							tInfoOfSequential->gWriteBuffer, 
							tInfoOfSequential->gIndex, 
							tInfoOfSequential->gOffset);
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
					vIsNoError = crx_c_os_fileSystem_private_win32_doWriteFile(
							pThis->gPrivate_osFileHandle,
							tInfoOfSequential->gWriteBuffer, 
							tInfoOfSequential->gIndex, 
							tInfoOfSequential->gOffset);
#endif
				}

				if(vIsNoError)
				{
					tInfoOfSequential->gOffset = tInfoOfSequential->gOffset + 
							tInfoOfSequential->gWriteBufferLength;
					tInfoOfSequential->gIndex = 0;
					tInfoOfSequential->gIsDirty = false;
					tInfoOfSequential->gWriteBufferLength = 0;
				}
			}
		}
		else if(vIsNoError && 
				(pThis->gPrivate_osFileHandle__2 != CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE))
		{
		}
	}
	
	return vIsNoError;
	CRX_SCOPE_END
}

CRX__LIB__PUBLIC_C_FUNCTION() int32_t crx_c_os_fileSystem_io_getOrderOfRegions1(
		void const * CRX_NOT_NULL pRegion, void const * CRX_NOT_NULL pRegion__2)
{
	if(((Crx_C_Os_FileSystem_Io_Region const *)pRegion)->gBaseAddress < 
			((Crx_C_Os_FileSystem_Io_Region const *)pRegion__2)->gBaseAddress)
		{return -1;}
	else if(((Crx_C_Os_FileSystem_Io_Region const *)pRegion)->gBaseAddress > 
			((Crx_C_Os_FileSystem_Io_Region const *)pRegion__2)->gBaseAddress)
		{return 1;}
	else
		{return 0;}
}
CRX__LIB__PUBLIC_C_FUNCTION() int32_t crx_c_os_fileSystem_io_getOrderOfRegions2(
		void const * CRX_NOT_NULL pRegion, void const * CRX_NOT_NULL pRegion__2)
{
	if(((Crx_C_Os_FileSystem_Io_Region const *)pRegion)->gBaseOffset < 
			((Crx_C_Os_FileSystem_Io_Region const *)pRegion__2)->gBaseOffset)
		{return -1;}
	else if(((Crx_C_Os_FileSystem_Io_Region const *)pRegion)->gBaseOffset > 
			((Crx_C_Os_FileSystem_Io_Region const *)pRegion__2)->gBaseOffset)
		{return 1;}
	else
	{
		if(((Crx_C_Os_FileSystem_Io_Region const *)pRegion)->gLength < 
				((Crx_C_Os_FileSystem_Io_Region const *)pRegion__2)->gLength)
			{return -1;}
		else if(((Crx_C_Os_FileSystem_Io_Region const *)pRegion)->gLength > 
				((Crx_C_Os_FileSystem_Io_Region const *)pRegion__2)->gLength)
			{return 1;}
		else
			{return 0;}
	}
}
CRX__LIB__PUBLIC_C_FUNCTION() int32_t crx_c_os_fileSystem_io_constructIndexFromRegion(
		void * CRX_NOT_NULL pIndex, void const * CRX_NOT_NULL pRegion)
	{*((unsigned char * *)pIndex) = ((Crx_C_Os_FileSystem_Io_Region)pRegion)->gBaseAddress;}
CRX__LIB__PUBLIC_C_FUNCTION() int32_t crx_c_os_fileSystem_io_constructIndexFromRegion2(
		void * CRX_NOT_NULL pIndex, void const * CRX_NOT_NULL pRegion)
	{*((uint64_t *)pIndex) = ((Crx_C_Os_FileSystem_Io_Region)pRegion)->gBaseOffset;}


CRX__LIB__PUBLIC_C_FUNCTION() int32_t crx_c_os_fileSystem_io_getOrderOfPortions(
		void const * CRX_NOT_NULL pPortion, void const * CRX_NOT_NULL pPortion__2)
{
	if(((Crx_C_Os_FileSystem_Io_Portion const *)pPortion)->gBaseOffset < 
			((Crx_C_Os_FileSystem_Io_Portion const *)pPortion__2)->gBaseOffset)
		{return -1;}
	else if(((Crx_C_Os_FileSystem_Io_Portion const *)pPortion)->gBaseOffset > 
			((Crx_C_Os_FileSystem_Io_Portion const *)pPortion__2)->gBaseOffset)
		{return 1;}
	else
	{
		if(((Crx_C_Os_FileSystem_Io_Portion const *)pPortion)->gLength < 
				((Crx_C_Os_FileSystem_Io_Portion const *)pPortion__2)->gLength)
			{return -1;}
		else if(((Crx_C_Os_FileSystem_Io_Portion const *)pPortion)->gLength > 
				((Crx_C_Os_FileSystem_Io_Portion const *)pPortion__2)->gLength)
			{return 1;}
		else
			{return 0;}
	}
}
CRX__LIB__PUBLIC_C_FUNCTION() int32_t crx_c_os_fileSystem_io_constructIndexFromPortion(
		void * CRX_NOT_NULL pIndex, void const * CRX_NOT_NULL pPortion)
	{*((uint64_t *)pIndex) = ((Crx_C_Os_FileSystem_Io_Portion)pPortion)->gBaseOffset;}
CRX__LIB__PUBLIC_C_FUNCTION() int32_t crx_c_os_fileSystem_io_getOrderOfUnsignedCharPointers(
		void const * CRX_NOT_NULL pUnsignedCharPointer, 
		void const * CRX_NOT_NULL pUnsignedCharPointer2)
{
	if((*((unsigned char * *)pUnsignedCharPointer)) < 
			(*((unsigned char * *)pUnsignedCharPointer2)))
		{return -1;}
	else if((*((unsigned char * *)pUnsignedCharPointer)) > 
			(*((unsigned char * *)pUnsignedCharPointer2)))
		{return 1;}
	else
		{return 0;}
}
CRX__LIB__PUBLIC_C_FUNCTION() int32_t crx_c_os_fileSystem_io_getOrderOfUint64(
		void const * CRX_NOT_NULL pUint64, void const * CRX_NOT_NULL pUint64__2)
{
	if((*((uint64_t *)pUint64)) < (*((uint64_t *)pUint64__2)))
		{return -1;}
	else if((*((uint64_t *)pUint64)) > (*((uint64_t *)pUint64__2)))
		{return 1;}
	else
		{return 0;}
}

CRX__LIB__PUBLIC_C_FUNCTION() Crx_C_Os_FileSystem_Io_Region * crx_c_os_fileSystem_io_region_new()
	{return ((Crx_C_Os_FileSystem_Io_Region *)calloc(1, sizeof(Crx_C_Os_FileSystem_Io_Region)));}

CRX__C__TYPE_BLUE_PRINT__DEFINE_GET_BLUE_PRINT(
		Crx_C_Os_FileSystem_Io_Region, crx_c_os_fileSystem_io_region_,
		CRXM__TRUE, CRXM__FALSE,
		CRXM__FALSE, CRXM__FALSE,
		CRXM__FALSE, CRXM__FALSE)

CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_io_region_free(
		Crx_C_Os_FileSystem_Io_Region * pThis)
	{free(pThis);}

CRX__LIB__PUBLIC_C_FUNCTION() Crx_C_TypeBluePrint const * 
		crx_c_os_fileSystem_io_region_getTypeBluePrintForPointer()
{
	static bool vIsNotFirstTime = false;
	static Crx_C_TypeBluePrint vReturn;

	if(!vIsNotFirstTime)
	{
		vReturn.gBYTE_SIZE = sizeof(Crx_C_Os_FileSystem_Io_Region *);
		vReturn.gIS_COPYABLE = true;
		vReturn.gIS_GENERIC = false;
		vReturn.gFUNC__DESTRUCT = NULL;
		vReturn.gFUNC__COPY_CONSTRUCT = NULL;
		vReturn.gFUNC__MOVE_CONSTRUCT = NULL;
		vReturn.gFUNC__MOVE_DESTRUCT = NULL;
		vReturn.gFUNC__GET_BYTE_SIZE_OF = NULL;

		vIsNotFirstTime = true;
	}

	return &vReturn;
}

CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_io_infoOfSequential_construct(
		Crx_C_Os_FileSystem_Io_InfoOfSequential * pThis, size_t pRegionAlignmentValue,
		size_t pBufferSize, bool pIsToIgnoreRegions)
{
	/*
		TO EASE THE DISTANCE REQUIREMENT ON CLIENT CODE FOR THE DISTANCE BETWEEN ACCESS DONE BY TWO 
		SEQUENTIAL WRITERS, ALL BUFFERS MUST BE LARGER THAN THE ALIGNMENT VALUE FOR REGIONS. 
		HOWEVER, THIS CORRECTION IS ONLY DONE IF THE ALIGNMENT VALUE IS SMALLER THAN THE LIMIT,
		CRX__C__OS__FILE_SYSTEM__IO__MAXIMUM_MEMORY_PAGE_SIZE_TO_CORRECT_BUFFER_WITH. THIS IS 
		BECAUSE THIS VALUE CAN BE VERY LARGE, REACHING THE TERABYTES RANGE. IN THAT CASE, THE Io 
		CLASS WILL NOT ALLOW MORE THAN ONE SEQUENTIAL ACCESSOR TO EXIST WHILE THERE ARE REGIONS 
		EXISTING, HENCE, IN THAT CASE THERE IS NO NEED TO WORRY ABOUT THE DISTANCE REQUIREMENT
		BETWEEN THE ACCESS OF TWO SEQUENTIAL ACCESSORS.
	*/
	size_t vBufferSize = ((pBufferSize <= 1) ? 0 : 
			(((pBufferSize < pRegionAlignmentValue) && (pRegionAlignmentValue < 
			CRX__C__OS__FILE_SYSTEM__IO__MAXIMUM_MEMORY_PAGE_SIZE_TO_CORRECT_BUFFER_WITH)) ? 
			pRegionAlignmentValue : pBufferSize));
	pThis->gOffset = 0;
	pThis->gIndex = 0;
	pThis->gIsDirty = false;
	pThis->gIsInUse = false;
	pThis->gIsToIgnoreRegions = pIsToIgnoreRegions;
	pThis->gBufferCapacity = 0;

	if(vBufferSize > 0)
	{
		//ROUNDING UP TO NEAREST CRX__OS__FILE_SYSTEM__SAFE_ROUND_SIZE.
		pThis->gBufferCapacity = ((vBufferSize + CRX__OS__FILE_SYSTEM__SAFE_ROUND_SIZE - 1) & 
				(~((size_t)CRX__OS__FILE_SYSTEM__SAFE_ROUND_INVERTED_MASK)));
		pThis->gWriteBuffer = ((unsigned char *)malloc(pThis->gBufferCapacity));
		pThis->gReadBuffer = crx_c_rings_unsignedChar_new(pThis->gBufferCapacity);

		if((pThis->gReadBuffer != NULL) && 
				crx_c_rings_unsignedChar_getCapacity(pThis->gReadBuffer) < pThis->gBufferCapacity)
		{
			crx_c_rings_unsignedChar_destruct(pThis->gReadBuffer);
			crx_c_rings_unsignedChar_free(pThis->gReadBuffer);
			pThis->gReadBuffer = NULL;
		}
	}
	else
	{
		pThis->gBufferCapacity = 0;
		pThis->gWriteBuffer = NULL;
		pThis->gReadBuffer = NULL;
	}

	CRX__SET_TO_ZERO(Crx_C_Os_FileSystem_Io_Region, &(pThis->gRegion__lock));
}
CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_io_infoOfSequential_copyConstruct(
		Crx_C_Os_FileSystem_Io_InfoOfSequential * pThis, 
		Crx_C_Os_FileSystem_Io_InfoOfSequential const * CRX_NOT_NULL pInfoOfSequential)
	{exit();}

CRX__LIB__PUBLIC_C_FUNCTION() Crx_C_Os_FileSystem_Io_InfoOfSequential * 
		crx_c_os_fileSystem_io_infoOfSequential_new(size_t pBufferSize, bool pIsToIgnoreRegions)
{
	Crx_C_Os_FileSystem_Io_InfoOfSequential * vReturn = 
			((Crx_C_Os_FileSystem_Io_InfoOfSequential *)malloc(sizeof(
			Crx_C_Os_FileSystem_Io_InfoOfSequential)));

	if(vReturn != NULL)
	{
		crx_c_os_fileSystem_io_infoOfSequential_construct(vReturn, pBufferSize, 
				pIsToIgnoreRegions);
	}

	return vReturn;
}
CRX__LIB__PUBLIC_C_FUNCTION() Crx_C_Os_FileSystem_Io_InfoOfSequential * 
		crx_c_os_fileSystem_io_infoOfSequential_copyNew(
		Crx_C_Os_FileSystem_Io_InfoOfSequential const * CRX_NOT_NULL pInfoOfSequential)
	{exit();}
CRX__LIB__PUBLIC_C_FUNCTION() Crx_C_Os_FileSystem_Io_InfoOfSequential * 
		crx_c_os_fileSystem_io_infoOfSequential_moveNew(
		Crx_C_Os_FileSystem_Io_InfoOfSequential * CRX_NOT_NULL pInfoOfSequential)
{
	Crx_C_Os_FileSystem_Io_InfoOfSequential * vReturn = 
			((Crx_C_Os_FileSystem_Io_InfoOfSequential *)malloc(
			sizeof(Crx_C_Os_FileSystem_Io_InfoOfSequential)));

	if(vReturn != NULL)
		{memcpy(vReturn, pInfoOfSequential, sizeof(Crx_C_Os_FileSystem_Io_InfoOfSequential));}

	return vReturn;
}

CRX__C__TYPE_BLUE_PRINT__DEFINE_GET_BLUE_PRINT(
		Crx_C_Os_FileSystem_Io_InfoOfSequential, crx_c_os_fileSystem_io_infoOfSequential_,
		CRXM__TRUE, CRXM__TRUE,
		CRXM__FALSE, CRXM__TRUE,
		CRXM__FALSE, CRXM__FALSE)

CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_io_infoOfSequential_destruct(
		Crx_C_Os_FileSystem_Io_InfoOfSequential * pThis)
{
	if(pThis->gWriteBuffer != NULL)
		{free(pThis->gWriteBuffer);}
	if(pThis->gReadBuffer != NULL)
	{
		crx_c_rings_unsignedChar_destruct(pThis->gReadBuffer);
		crx_c_rings_unsignedChar_free(pThis->gReadBuffer);
	}
}
CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_io_infoOfSequential_free(
		Crx_C_Os_FileSystem_Io_InfoOfSequential * pThis)
	{free(pThis);}

CRX__LIB__PUBLIC_C_FUNCTION() Crx_C_TypeBluePrint const * 
		crx_c_os_fileSystem_io_infoOfSequential_getTypeBluePrintForPointer()
{
	static bool vIsNotFirstTime = false;
	static Crx_C_TypeBluePrint vReturn;

	if(!vIsNotFirstTime)
	{
		vReturn.gBYTE_SIZE = sizeof(Crx_C_Os_FileSystem_Io_InfoOfSequential *);
		vReturn.gIS_COPYABLE = true;
		vReturn.gIS_GENERIC = false;
		vReturn.gFUNC__DESTRUCT = NULL;
		vReturn.gFUNC__COPY_CONSTRUCT = NULL;
		vReturn.gFUNC__MOVE_CONSTRUCT = NULL;
		vReturn.gFUNC__MOVE_DESTRUCT = NULL;
		vReturn.gFUNC__GET_BYTE_SIZE_OF = NULL;

		vIsNotFirstTime = true;
	}

	return &vReturn;
}

//----------------
//----------------
CRX__LIB__PUBLIC_C_FUNCTION() Crx_C_Os_FileSystem_Io_Portion * 
		crx_c_os_fileSystem_io_portion_new()
{
	Crx_C_Os_FileSystem_Io_Portion * vReturn = ((Crx_C_Os_FileSystem_Io_Portion *)malloc(sizeof(
			Crx_C_Os_FileSystem_Io_Portion)));

	if(vReturn != NULL)
		{crx_c_os_fileSystem_io_portion_construct(vReturn);}

	return vReturn;
}
CRX__LIB__PUBLIC_C_FUNCTION() Crx_C_Os_FileSystem_Io_Portion * 
		crx_c_os_fileSystem_io_portion_copyNew(
		Crx_C_Os_FileSystem_Io_Portion const * CRX_NOT_NULL pPortion)
{
	Crx_C_Os_FileSystem_Io_Portion * vReturn = 
			((Crx_C_Os_FileSystem_Io_Portion *)malloc(sizeof(Crx_C_Os_FileSystem_Io_Portion)));

	if(vReturn != NULL)
		{memcpy(vReturn, pPortion, sizeof(Crx_C_Os_FileSystem_Io_Portion));}

	return vReturn;
}
CRX__LIB__PUBLIC_C_FUNCTION() Crx_C_Os_FileSystem_Io_Portion *
		crx_c_os_fileSystem_io_portion_moveNew(
		Crx_C_Os_FileSystem_Io_Portion * CRX_NOT_NULL pPortion)
{
	Crx_C_Os_FileSystem_Io_Portion * vReturn = 
			((Crx_C_Os_FileSystem_Io_Portion *)malloc(sizeof(Crx_C_Os_FileSystem_Io_Portion)));

	if(vReturn != NULL)
		{memcpy(vReturn, pPortion, sizeof(Crx_C_Os_FileSystem_Io_Portion));}

	return vReturn;
}

CRX__C__TYPE_BLUE_PRINT__DEFINE_GET_BLUE_PRINT(
		Crx_C_Os_FileSystem_Io_Portion, crx_c_os_fileSystem_io_portion_,
		CRXM__TRUE, CRXM__FALSE,
		CRXM__TRUE, CRXM__FALSE,
		CRXM__FALSE, CRXM__FALSE)

CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_io_portion_free(
		Crx_C_Os_FileSystem_Io_Portion * pThis)
	{free();}
		
CRX__LIB__PUBLIC_C_FUNCTION() Crx_C_TypeBluePrint const * 
		crx_c_os_fileSystem_io_portion_getTypeBluePrintForPointer()
{
	static bool vIsNotFirstTime = false;
	static Crx_C_TypeBluePrint vReturn;

	if(!vIsNotFirstTime)
	{
		vReturn.gBYTE_SIZE = sizeof(Crx_C_Os_FileSystem_Io_Portion *);
		vReturn.gIS_COPYABLE = true;
		vReturn.gIS_GENERIC = false;
		vReturn.gFUNC__DESTRUCT = NULL;
		vReturn.gFUNC__COPY_CONSTRUCT = NULL;
		vReturn.gFUNC__MOVE_CONSTRUCT = NULL;
		vReturn.gFUNC__MOVE_DESTRUCT = NULL;
		vReturn.gFUNC__GET_BYTE_SIZE_OF = NULL;

		vIsNotFirstTime = true;
	}

	return &vReturn;
}
	



bool crx_c_os_fileSystem_isCharacterInvalidInSegment(uint32_t pCodePoint)
{
	static const uint_8 INVALID_CHARS[8] = {34/* " */, 42 /* * */, 47 /* / */, 58 /* : */,
			60 /* < */, 62 /* > */, 92 /* \ */, 124 /* | */};
	static Crx_C_HashTable/*<uint32_t>*/ vHashTable /*= ?*/;
	static bool vIsInitialized = false;

	if(!vIsInitialized)
	{
		uint32_t tUint32 = 9216;

		crx_c_hashTable_construct2(&vHashTable, 
				crx_c_typeBluePrint_getTypeBluePrintForUint32(),
				&crx_c_hashTable_areUint32sEqual,
				&crx_c_hashTable_getHashKeyForUint32);

		vIsInitialized = true;
		
		CRX_FOR(uint32_t tI = 0; tI < 32; tI++)
			{crx_c_hashTable_set(&vHashTable, &tI, NULL);}
		CRX_ENDFOR

		CRX_FOR(uint32_t tI = 0; tI < 8; tI++)
			{crx_c_hashTable_set(&vHashTable, &tI, NULL);}
		CRX_ENDFOR

		/* 
			"Based on observations on Mac OS 10.15.7 on HFS+ U+2400 appears to be replaced 
			by U+0." 
		*/		
		crx_c_hashTable_set(&vHashTable, &tUint32, NULL);
	}

	return (crx_c_hashTable_hasKey(&vHashTable, pCodePoint) ||
			crx_c_os_osString_isCodePointAComposite(pCodePoint));
}

bool crx_c_os_fileSystem_isFileSystemTrailVoidOfPrefix(Crx_C_String const * CRX_NOT_NULL pUtf8Path)
{
	if(crx_c_string_getSize(pUtf8Path) < 2)
		{return true;}
	else
	{
		Crx_C_String_Sub tSub /*= ?*/;

		crx_c_string_sub_construct2(&tSub, pUtf8Path);

		crx_c_os_osString_trimSub(&tSub);
		

		if(crx_c_string_sub_getLength(&tSub) < 2)
			{return true;}
		else
		{
			char const * tChars = crx_c_string_sub_getStartChar(&tSub);

			if((*tChars == 47 /* / */) || (*tChars == 92 /* \ */))
			{
				//COVERS MICROSOFT'S "\\." AND "\\?" AND "\\" AND LINUX'S "//". 
				//	ALSO PREVENTS "\\\" OR "///" AND MORE SLASHES. THIS IS BY DESIGN.
				//THIS CURRENTLY FORBIDS "/\" IN LINUX WHICH CAN BE VALID AND MEANS A DIRECTORY 
				//	WHOSE NAME IS '\' AND IS UNDER ROOT.
				if((*(tChars + 1) == 47 /* / */) || (*(tChars + 1) == 92 /* \ */))
					{return false;}
				else
				{
#if(CRX__NON_CRXED__OS__SETTING__IS_LINUX || \
		CRX__NON_CRXED__OS__SETTING__IS_MACOS || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD))
					return (crx_c_os_fileSystem_posix_getMountInformationFor(pUtf8Path, 2, 
							NULL) == 0);
					
					/*Crx_NonCrxed_Os_Posix_Dll_Libc_Stat tStat;

					if(stat(crx_c_string_constantGetCharsPointer, &tStat) == 0 && S_ISDIR(sb.st_mode))
					{
						...it is a directory...
					}*/
#else
					return true;
#endif
				}
			}
			//COVER'S MICROSOFT DRIVE LETTERS.
			else if((CRX__C__IS_CODE_POINT_ASCII_UPPER_CASE_ALPHABET(*tChars) ||
					CRX__C__IS_CODE_POINT_ASCII_LOWER_CASE_ALPHABET(*tChars)) && 
					(*(tChars + 1) == 58 /*:*/))
				{return false;}
			else
				{return;}
		}
	}
}

/*
	- THIS FUNCTION CAN WORK ON FULL FILESYSTEM PATH, AND PARTIAL FILESYSTEM PATH, AS THESE ARE
			DEFINED IN THE CRX URL STANDARD.
	- IF THE FUNCTION RETURNS TRUE, AND RUNTIME GUARANTEES ARE APPLICABLE, REMEMBER TO CALL
			crx_c_os_fileSystem_verifyFileSystemPath2()
	- pVerifiedPathPrefix IS GUARANTEED TO END WITH A '/'. PATH PREFIX IS AS DEFINED IN THE CRX URL
			STANDARD.
	- pVerifiedRoute IS A PARTIAL FILESYSTEM PATH. ROUTE IS AS DEFINED IN THE CRX URL STANDARD.
			REMEMBER, THIS IS NOT FINAL IF YOU NEED TO CALL 
			crx_c_os_fileSystem_verifyFileSystemPath2() WHICH IS LIKELY TO CONTINUE MOODIFYING
			TO ITS FINAL VERIFIED FORM.
	- pLengthResidueWhenDirectory, pLengthResidue: IF THE PATH IS FULL, AS FULL IS DEFINED IN THE
			CRX URL STANDARD, AND IF THESE ARE PASSED, THE VALUE IS SET TO THE REMAINING AMOUNT
			OF UNITS STILL ALLOWED IN THE PATH. THIS MIGHT BE BYTES OR UCS2 CHARACTERS CURRENTLY.
			IF THE PATH IS PARTIAL, AND IF THESE ARE PASSED, THEY ARE USED AS THE CURRENT REMAINING
			AMOUNT OF UNITS AVAILABLE, AND ARE THEN UPDATED WITH THE REMAINING NUMBER OF UNITS AFTER
			ACCOUNTING FOR THE NEW PARTIAL PATHS. IF THEY ARE NOT PASSED, THE CURRENT REMAINING
			IS JUST WHAT IS DEFINED BY 
			"CRX__OS__FILE_SYSTEM__MAXIMUM_FULL_PATH_WITH_PREFIX_UCS2_LENGTH" AND OTHERS.
	- REMEMBER, pDepthLimitOfReparsePoints IS APPLICABLE TO THE ROUTE ONLY, WHETHER A CRX EXTENDED
			PATH IS PASSED, OR A CRX PATH IS PASSED, OR A CRX ROUTE.
	- WARNING: WHILE pVerifiedPathPrefix AND pVerifiedRoute ARE USABLE FOR OS CALLS, 
			pVerifiedPathPrefix IS NOT USABLE IF IT IS TO BE RETURNED BACK TO USER. USE 
			pVerifiedOriginalPathPrefix AND pVerifiedRoute INSTEAD.
*/
bool crx_c_os_fileSystem_verifyFileSystemPath(
		Crx_C_Os_FileSystem_Contract const * pContract, Crx_C_Os_FileSystem_Operation pOperation,
		Crx_C_String const * pUtf8Path, uint32_t pDepthLimitOfReparsePoints,
		bool pIsToForceTreatAsSingleSegment, Crx_C_String * CRX_NOT_NULL pVerifiedPathPrefix, 
		Crx_C_String * CRX_NOT_NULL pVerifiedRoute, Crx_C_String * pVerifiedOriginalPathPrefix, 
		size_t * pLengthResidueWhenDirectory, size_t * pLengthResidue, 
		bool * pIsToNormalize, bool * pIsEntityName)
{
	bool vReturn = true;

	//ISSUE: ENCODING/CHARACTER SET
	/*THE FOLLOWING IS WHY THE ITERATOR IS NECESSARY. GIVEN THE REQUIREMENT THAT THE UNDERLYING 
			DESIGN MUST ALWAYS PROVIDE A WAY TO ACCESS EACH DIRECTORY AND FILE, UNDER WINDOWS 2000, 
			FOR EXAMPLE, IF A FILE NAME HAS A MALFORMED UCS16 NAME, THE CLIENT CODE WILL NOT BE ABLE 
			TO ACCESS IT EVEN UNDER READING OPERATIONS. AN ITERATOR, HOWEVER, ALLOWS THE CLIENT
			TO ACCESS SUCH A FILE INDIRECTLY WITHOUT FORMAL VIOLATION.*/
	if(pOperation != CRX__C__OS__FILE_SYSTEM__OPERATION__PATH_RETREIVAL)
	{
		if(!crx_c_os_osString_isStringValidForOs(pUtf8Path, pContract->gCharacterSet, 
				pContract->gGuaranteeLevel))
			{vReturn = false;}
	}
	else
	{
		if(!crx_c_os_osString_isStringValidFromOs(pUtf8Path, pContract->gCharacterSet, 
				pContract->gGuaranteeLevel))
			{vReturn = false;}
	}

	if(vReturn)
	{
		bool tIsToNormalizeRoute = false;

		if(!pIsToForceTreatAsSingleSegment)
		{
			vReturn = crx_c_os_fileSystem_extractPrefixAndRoute(pUtf8Path, pVerifiedPathPrefix, 
					pVerifiedRoute, pVerifiedOriginalPathPrefix, &tIsToNormalizeRoute);
		}
		else
			{vReturn = crx_c_string_appendString(pVerifiedRoute, pUtf8Path);}

		if(vReturn)
		{
			//ISSUE: COMPOSITION (A)
			if(pIsToForceTreatAsSingleSegment || 
					!crx_c_os_fileSystem_isFileSystemTrailVoidOfPrefix(pVerifiedRoute))
				{vReturn = false;}/*NO LONGER NECESSARY BUT KEEPING FOR NOW*/

			if(vReturn && tIsToNormalizeRoute)
			{
				Crx_C_String tString /*= ?*/;

				crx_c_string_copyConstruct(&tString, pVerifiedRoute);

				crx_c_os_fileSystem_normalizePathForOs(pVerifiedRoute, &tString, true);

#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
				/* THIS WAS WHEN THE LONGEST PREFIX WAS INTERPRETED TO BE THE CRX PATH PREFIX.
				if(!crx_c_string_isEqualTo2(pVerifiedRoute, CRX__OS__FILE_SYSTEM__SEPERATOR_STRING, 
						false) && (crx_c_string_getSize(pVerifiedPathPrefix) > 0))
				{
					Crx_C_String tPrefix2 /*= ?* /;
					Crx_C_String tRoute2 /*= ?* /;
					bool tIsToNormalizeRoute2 = true;

					crx_c_string_construct(&tPrefix2);
					crx_c_string_construct(&tRoute2);

					crx_c_string_insertElementsAt(&tString, 0, pVerifiedPathPrefix, 0, 
							crx_c_getLength(pVerifiedPathPrefix));
				
					if(crx_c_os_fileSystem_extractPrefixAndRoute(&tString, &tPrefix2, &tRoute2,
							&tIsToNormalizeRoute2))
					{
						if(!crx_c_string_isEqualTo(&tPrefix2, pVerifiedPathPrefix))
							{vReturn = false;}
					}
					else
						{vReturn = false;}

					crx_c_string_destruct(&tPrefix2);
					crx_c_string_destruct(&tRoute2);
				}*/
#endif

				crx_c_string_destruct(&tString);
			}

			if(vReturn && ((pOperation == CRX__C__OS__FILE_SYSTEM__OPERATION__WRITE) ||
					(pContract->gFileSystemGuaranteeLevel == 2)))
			{
				size_t tUcs2Length = crx_c_os_fileSystem_internal_getUtf8Ucs2Length(
						pVerifiedPathPrefix) - ((!tIsToNormalizeRoute && 
						!pIsToForceTreatAsSingleSegment && crx_c_string_isBeginningWith2(
						pVerifiedPathPrefix, "\\\\?\\", false)) ? 4 : 0) + (crx_c_string_isEqualTo3(
						pVerifiedRoute, CRX__OS__FILE_SYSTEM__SEPERATOR_STRING, 1, false) ? 0 : 
						(crx_c_os_fileSystem_internal_getUtf8Ucs2Length(pVerifiedRoute)));
				size_t tLimit = 0;

				if(crx_c_string_getSize(pVerifiedPathPrefix) > 0)
				{
					tLimit = (crx_c_string_isEndingWith3(pVerifiedRoute, 
							CRX__OS__FILE_SYSTEM__SEPERATOR_STRING, 1, false) ? 
							CRX__OS__FILE_SYSTEM__MAXIMUM_DIRECTORY_FULL_PATH_WITH_PREFIX_UCS2_LENGTH :
							CRX__OS__FILE_SYSTEM__MAXIMUM_FULL_PATH_WITH_PREFIX_UCS2_LENGTH);

					if(pLengthResidueWhenDirectory != NULL)
					{
						*pLengthResidueWhenDirectory = 
								CRX__OS__FILE_SYSTEM__MAXIMUM_DIRECTORY_FULL_PATH_WITH_PREFIX_UCS2_LENGTH;
					}
					if(pLengthResidue != NULL)
					{
						*pLengthResidue = 
								CRX__OS__FILE_SYSTEM__MAXIMUM_FULL_PATH_WITH_PREFIX_UCS2_LENGTH;
					}
				}
				else
				{
					if(crx_c_string_isEndingWith3(pVerifiedRoute, 
							CRX__OS__FILE_SYSTEM__SEPERATOR_STRING, 1, false))
					{
						if(pLengthResidueWhenDirectory != NULL)
							{tLimit = *pLengthResidueWhenDirectory;}
						else
						{
							tLimit =
									CRX__OS__FILE_SYSTEM__MAXIMUM_DIRECTORY_FULL_PATH_WITH_PREFIX_UCS2_LENGTH;
						}
					}
					else
					{
						if(pLengthResidue != NULL)
							{tLimit = *pLengthResidue;}
						else
						{
							tLimit =
									CRX__OS__FILE_SYSTEM__MAXIMUM_FULL_PATH_WITH_PREFIX_UCS2_LENGTH;
						}	
					}
				}

				//ISSUE: LENGTH	(A)
				if(tUcs2Length > tLimit)
					{vReturn = false;}
				else
				{
					if(pLengthResidueWhenDirectory != NULL)
						{*pLengthResidueWhenDirectory = *pLengthResidueWhenDirectory - tUcs2Length;}
					if(pLengthResidue != NULL)
						{*pLengthResidue = *pLengthResidue - tUcs2Length;}
					
					/*if(crx_c_string_getSize(pVerifiedPathPrefix) > 0)
					{
						if(pLengthResidueWhenDirectory != NULL)
							{*pLengthResidueWhenDirectory = *pLengthResidueWhenDirectory - tUcs2Length;}
						if(pLengthResidue != NULL)
							{*pLengthResidue = *pLengthResidue - tUcs2Length;}
					}
					else
					{
						if((pLengthResidueWhenDirectory != NULL) && 
								(pLengthResidue != NULL))
						{
							if((pLengthResidue - pLengthResidueWhenDirectory) !=
									(CRX__OS__FILE_SYSTEM__MAXIMUM_FULL_PATH_WITH_PREFIX_UCS2_LENGTH -
									CRX__OS__FILE_SYSTEM__MAXIMUM_DIRECTORY_FULL_PATH_WITH_PREFIX_UCS2_LENGTH))
							{
								assert(((pLengthResidue - pLengthResidueWhenDirectory) -
										(CRX__OS__FILE_SYSTEM__MAXIMUM_FULL_PATH_WITH_PREFIX_UCS2_LENGTH -
										CRX__OS__FILE_SYSTEM__MAXIMUM_DIRECTORY_FULL_PATH_WITH_PREFIX_UCS2_LENGTH)) == 
										1);

								/*
									WE ARE HEERE CATCHING AN ERROR TOO LATE. THE FOLLOWING CATCHES
										WHEN THE PREVIOUS CALL TO THIS FUNCTION PASSED A FULL
										PATH FOR A DIRECTORY, BUT DID NOT INCLUDE THE TRAILIING,
										AND THE LENGTH WAS SUC AS IT PASSED THE PREVIOUS 
										VERIFICATION WHEN IT SHOULD NOT. BEING IN THIS BLOCK WE 
										DEDICT WHEN THE PREVIOUS PATH WAS FOR A DIRECTORY BUT
										WITHOUT A TRAILING SLA ADDED, AND TE FOLLOWING USES
										THAT TO RETROACTIVELY VERY THE LENGTH.
								* /
								if(*pLengthResidue > 0)
									{*pLengthResidue = *pLengthResidue - 1;}
								else
									{vReturn = false;}
								if(*pLengthResidueWhenDirectory < tUcs2Length)
									{vReturn = false;}
							}

							*pLengthResidue = *pLengthResidue - tUcs2Length;
							*pLengthResidueWhenDirectory = *pLengthResidueWhenDirectory - 
									tUcs2Length - (crx_c_string_isEndingWith3(pVerifiedRoute, 
									CRX__OS__FILE_SYSTEM__SEPERATOR_STRING, 1, false) ? 0 : 1);
						}
					}
					*/
				}

				//ISSUE: "THIS WAS MISCATEGORIZED IN THE PAST AS PART OF THE ENCODING PROBLEM"
				//		"NOTE THAT WITH THIS THE CHECK BELOW IN 
				//		crx_c_os_fileSystem_isCharacterInvalidInSegment() CAN BE PARTIALLY REDUNDANT, 
				//		BUT SHOULD NOT BE AN ISSUE"
				if(vReturn && !crx_c_os_osString_isUtf8StringValidForMacOsUtf16(pVerifiedRoute, 
						NULL))
					{vReturn = false;}
					
				if(vReturn)
				{
					Crx_C_String_Sub tSub /*= ?*/;
					size_t tIndex = 0;

					crx_c_string_sub_construct(&pSub, pVerifiedRoute);

					while(vReturn && crx_c_string_findNextLeftTokenAndUpdateIndex(pVerifiedRoute, 
							&tSub, &tIndex, CRX__OS__FILE_SYSTEM__SEPERATOR_STRING, 0))
					{
						if(crx_c_string_sub_getLength(&tSub) == 0)
							{continue;}

						char const * tCurrentChar = crx_c_string_sub_getStartChar(&tSub);
						char const * tEndChar = tCurrentChar + crx_c_string_sub_getLength(&tSub);
						size_t tAvailableLength = crx_c_string_sub_getLength(&tSub);
						char const * tCurrentChar2 = NULL;
						bool tIsFirst = true;
						uint32_t tNumberOfDotsSeen = 0;
						bool tWasNonAsciiSeen = false;
						bool tWasAsciiCharacterSeen = false;
						size_t tLengthOfLastDotSegmentSeen = 0;
						size_t tLengthOfLargestDotSegmentSeen = 0;
						size_t tLengthOfShortestSegmentSeen = 0;
						bool tWasAsciiUpperCaseSeenInSecondToLastDotSegment = false;
						bool tWasAsciiLowerCaseSeenInSecondToLastLastDotSegment = false;
						bool tWasAsciiUpperCaseSeenInLastDotSegment = false;
						bool tWasAsciiLowerCaseSeenInLastDotSegment = false;

						//ISSUE: LENGTH	(B)
						//	NOTE THAT THE ABOVE GUARANTEE LEVEL CHECK IS NOT REQUIRED BECAUSE
						//		THE SEGMENT (NAME) MAXIMUIM LENGTH IS UNIVERSAL, BUT STICKING TO
						//		THE REQUIREMENT OF LEAVING IT TO THE OS IN THESE CASES.
						if(crx_c_string_sub_getLength(&tSub) > 
								CRX__OS__FILE_SYSTEM__MAXIMUM_SEGMENT_BYTE_LENGTH)
							{vReturn = false;}

						//ISSUE: COMPOSITION (B)
						//	NOTE THAT THIS PART OF COMPOSITION IS DEPENDENT ON THE GUARANTEE LEVELS.
						//	THIS IS NOT AN ERROR.
						if(vReturn)
						{
							Crx_C_String_Sub tSub2 = tSub;
							
							crx_c_os_osString_trimSub(tSub2);

							vReturn = !(crx_c_string_sub_isEqualTo2(tSub2, "..") ||
									crx_c_string_sub_isEqualTo2(tSub2, "."));
						}

						while(vReturn && (tCurrentChar != tEndChar) && (tAvailableLength > 0))
						{
							uint32_t tCodePoint = 0;
							char const * tLastChar = NULL;
							bool _fdgfdg;

							if(tCurrentChar2 == NULL)
							{
								tCurrentChar2 == tChrrentChar;

								tWasAsciiUpperCaseSeenInSecondToLastDotSegment = 
										tWasAsciiUpperCaseSeenInLastDotSegment;
								tWasAsciiUpperCaseSeenInLastDotSegment = false;
								
								tWasAsciiLowerCaseSeenInSecondToLastLastDotSegment =
										tWasAsciiLowerCaseSeenInLastDotSegment;
								tWasAsciiLowerCaseSeenInLastDotSegment = false;
							}
							
							CRX__C__CHAR__GET_NEXT_CODE_POINT_FROM_UTF8_CHAR_ARRAY(tCurrentChar, tCodePoint,
									tAvailableLength, _fdgfdg, CRXM__FALSE, CRXM__FALSE)

							//ISSUE: FORBIDDEN CHARACTERS (SITUATIONAL AND OTHERWISE)
							//
							//IMPORTANT: IF GURANATEE LEVEL SIMPLY REQUIRES WHAT SYSTEM REQUIRES, NO CHECKING
							//		IS DONE AT ALL. THE CHECK IS LEFT TO THE SYSTEM.
							if(tIsFirst && ((tCodePoint == 32 /*space*/) || (tCodePoint == 160)))
								{vReturn = false;}
							else if((tCurrentChar == tEndChar) && ((tCodePoint == 46 /*.*/) || 
									((tCodePoint == 32 /*space*/) || (tCodePoint == 160))))
								{vReturn = false;}
							else if(crx_c_os_fileSystem_isCharacterInvalidInSegment(tCodePoint))
								{vReturn = false;}

							//ISSUE: VFAT LONG FILE NAMES	(A)
							if(vReturn)
							{
								if(tCodePoint > 127)
									{tWasNonAsciiSeen = true;}
								else
								{
									if(tCodePoint == 46 /*.*/)
										{tNumberOfDotsSeen++;}
									else if(CRX__C__IS_CODE_POINT_ASCII_UPPER_CASE_ALPHABET(tCodePoint))
									{
										tWasAsciiUpperCaseSeenInLastDotSegment = true;
										tWasAsciiCharacterSeen = true;
									}
									else if(CRX__C__IS_CODE_POINT_ASCII_LOWER_CASE_ALPHABET(tCodePoint))
									{
										tWasAsciiLowerCaseSeenInLastDotSegment = true;
										tWasAsciiCharacterSeen = true;
									}
								}
								
								if((tCodePoint == 46 /*.*/)
								{
									if(!tIsFirst)
										{tLastChar = tCurrentChar - 2;}
									else
									{
										tLengthOfLastDotSegmentSeen = 0;
										tLengthOfShortestSegmentSeen = 0;
										tCurrentChar2 = NULL;
									}
								}
								else if((tCurrentChar) == tEndChar)
									{tLastChar = tCurrentChar;}

								if(tLastChar != NULL)
								{
									if(tCurrentChar2 <= tLastChar)
									{
										tLengthOfLastDotSegmentSeen = tLastChar - tCurrentChar2 + 1;

										if(tLengthOfLastDotSegmentSeen > tLengthOfLargestDotSegmentSeen)
											{tLengthOfLargestDotSegmentSeen = tLengthOfLastDotSegmentSeen;}
										
										if(tLengthOfLastDotSegmentSeen < tLengthOfShortestSegmentSeen)
											{tLengthOfShortestSegmentSeen = tLengthOfLastDotSegmentSeen;}
									}
									else
									{
										tLengthOfLastDotSegmentSeen = 0;
										tLengthOfShortestSegmentSeen = 0;
									}
									
									tCurrentChar2 = NULL;
								}
							}
							
							tIsFirst = false;
						}

						//ISSUE: VFAT LONG FILE NAMES	(B)
						if(!tWasNonAsciiSeen && (tNumberOfDotsSeen < 2) &&
								(tLengthOfLargestDotSegmentSeen <= 8) &&
								(tLengthOfShortestSegmentSeen > 0) && (tLengthOfLastDotSegmentSeen < 4) &&
								tWasAsciiCharacterSeen &&
								((tWasAsciiUpperCaseSeenInSecondToLastDotSegment == 
										tWasAsciiLowerCaseSeenInSecondToLastLastDotSegment) ||
								(tWasAsciiUpperCaseSeenInLastDotSegment == 
										tWasAsciiLowerCaseSeenInLastDotSegment)))
							{vReturn = false;}
					}
				}
			}
			
			if(vReturn && (((pOperation == CRX__C__OS__FILE_SYSTEM__OPERATION__WRITE) ||
					(pContract->gFileSystemGuaranteeLevel == 2)) || 
					CRXM__IFELSE2(CRX__NON_CRXED__OS__SETTING__IS_WINDOWS, true, false)))
			{
				//ISSUE: FORBIDDEN NAMES
				Crx_C_String_Sub tSub /*= ?*/;
				size_t tIndex = 0;

				crx_c_string_sub_construct(&pSub, pVerifiedRoute);
						

				while(crx_c_string_findNextLeftTokenAndUpdateIndex(pVerifiedRoute, &tSub, &tIndex,
						CRX__OS__FILE_SYSTEM__SEPERATOR_STRING, 0))
				{
					Crx_C_String_Sub tSub2 /*= ?*/;
					size_t tIndex2 = 0;

					crx_c_string_sub_construct(&pSub2);

					while(vReturn && crx_c_string_sub_findNextLeftTokenAndUpdateIndex4(&pSub2, 
							&tSub, &tIndex2, ".", 0))
					{
						crx_c_os_osString_trimSub(&tSub2);

						if(crx_c_string_sub_getLength(&tSub2) > 0)
						{
							vReturn = !(((crx_c_string_sub_getLength(&tSub2) == 3) &&
									(crx_c_string_sub_isEqualTo4(&tSub2, "CON", true) ||
									crx_c_string_sub_isEqualTo4(&tSub2, "PRN", true) ||
									crx_c_string_sub_isEqualTo4(&tSub2, "AUX", true) ||
									crx_c_string_sub_isEqualTo4(&tSub2, "NUL", true))) ||
									((crx_c_string_sub_getLength(&tSub2) == 4) &&
									(crx_c_string_sub_isBeginningWith4(&tSub2, "COM", true) ||
									crx_c_string_sub_isBeginningWith4(&tSub2, "LPT", true)) &&
									CRX__C__IS_CODE_POINT_ASCII_NUMERIC(crx_c_string_sub_copyGet(&tSub2, 3))));
						}
					}

					if(!vReturn)
						{break;}
				}
			}
		}

		//REMEMBER: LEVEL 3 GUARANTEES CAN BE DONE NOW, DESPITE BEING DESCRIBED AS RUNTIME. LEVELS 1 
		//	AND 2 STILL NEED ADDRESSING.
		if(vReturn && (pContract->gFileSystemRuntimeGuaranteeLevel == 3))
		{
			char const * tChars = crx_c_string_getCharsPointer(pUtf8Path);
			size_t tNumberOfRemainingCharacters = crx_c_string_getSize(pUtf8Path);
			bool _fgdgdf;

			while(vReturn && (tNumberOfRemainingCharacters > 0))
			{
				uint32_t tCodePoint = 0;

				CRX__C__CHAR__GET_NEXT_CODE_POINT_FROM_UTF8_CHAR_ARRAY(tChars, tCodePoint, 
						tNumberOfRemainingCharacters, _fgdgdf, CRXM__FALSE, CRXM__FALSE);

				if(!crx_c_os_osString_fileSystem_isCodePointSuitableForBijection(tCodePoint))
				{
					vReturn = false;
					break;
				}
			}
		}

		if(vReturn)
		{
			if(pPathPrefix != NULL)
			{
				crx_c_string_empty(pPathPrefix);

				if(!crx_c_string_tryToPilferSwapContentWith(pPathPrefix, pVerifiedPathPrefix))
				{
					crx_c_string_destruct(pPathPrefix);
					memcpy(pPathPrefix, pVerifiedPathPrefix, sizeof(Crx_C_String));
					crx_c_string_construct(pVerifiedPathPrefix);
				}
			}

			if(pRoute != NULL)
			{
				crx_c_string_empty(pRoute);

				if(!crx_c_string_tryToPilferSwapContentWith(pRoute, pVerifiedRoute))
				{
					crx_c_string_destruct(pRoute);
					memcpy(pRoute, pVerifiedRoute, sizeof(Crx_C_String));
					crx_c_string_construct(pVerifiedRoute);
				}
			}
		}

		if(vReturn && (pIsEntityName != NULL))
		{
			*pIsEntityName = false;

			if(pIsToForceTreatAsSingleSegment)
				{*pIsEntityName = true;}
			else
			{
				if(crx_c_string_getSize(pVerifiedPathPrefix) == 0)
				{
					if(crx_c_string_getIndexOfFirstOccuranceOf3(pVerifiedRoute, 
							0, CRX__OS__FILE_SYSTEM__SEPERATOR_STRING, 1, 0) == ((size_t)(-1)))
						{*pIsEntityName = true;}
				}
			}
		}
		
		if(!vReturn)
		{
			crx_c_string_empty(pVerifiedPathPrefix);
			crx_c_string_empty(pVerifiedRoute);

			if(pVerifiedOriginalPathPrefix != NULL)
				{crx_c_string_empty(pVerifiedOriginalPathPrefix);}
			if(pExtraLengthResidue != NULL)
				{*pExtraLengthResidue = 0;}
		}
	}

	return vReturn;
}
size_t crx_c_os_fileSystem_internal_getUtf8Ucs2Length(
		Crx_C_String const * CRX_NOT_NULL pValidUtf8String)
{
	char * vChar  = crx_c_string_constantGetCharsPointer(pValidUtf8String);
	size_t vRemainingLength = crx_c_string_getSize(pValidUtf8String);
	uint32_t vCodePoint = 0;
	bool _hgfhfg;
	size_t vReturn = 0;

	while(vRemainingLength > 0)
	{
		CRX__C__CHAR__GET_NEXT_CODE_POINT_FROM_UTF8_CHAR_ARRAY(vChar, vCodePoint,
				vRemainingLength, _hgfhfg, CRXM__FALSE, CRXM__FALSE);

		if(vCodePoint < 65536)
			{vReturn = vReturn + 1;}
		else
			{vReturn = vReturn + 2;}
	}

	return vReturn;
}
/*
	WARNING: pVerified1Utf8PathPrefix AND pVerified1Utf8Route MUST CONTAIN RETURNED STRINGS FROM 
			crx_c_os_fileSystem_verifyFileSystemPath() UNDER THE SAME CONTRACTS
			crx_c_os_fileSystem_verifyFileSystemPath() WAS CALLED WITH.
			
	WARNING: IF THE PATH ITSELF IS A REPARSE POINT/JUNCTION/SYMLINK/SYMBOLIC LINK/ETC, COLLECTIVELY 
			CALLED IN THIS WORK, REPARSE POINT, THE CURRENT BEHAVIOR OF THIS FUNCTION WHEN IT COMES
			TO VERFYING WHETHER THE ENTITY THAT THE REPARSE POINT MATCHES THE TYPE OF THE PATH,
			MEANING DIRECTORY OR NOT, IS NOT WELL DEFINED. IDEALLY, IF A PATH SUCH AS, /a/b/c
			IS A REPARSE POINT, THEN THE TARGET OF THIS REPARSE POINT MUST BE A FILE, AND IF THE
			PATH IS /a/b/c/ THEN THE TARGET OF THE REPARSE POINT SHOULD BE A DIRECTORY AND THIS 
			FUNCTION MUST RETURN false IF THIS IS VIOLATED. THIS IS CURRENTLY NOT GUARANTEED.(TODO)
			
			IF YOU COME BACK TO THIS, YOU COULD LIKELY USE FindFirstFileA("/a/b/c/*") WHEN YOU KNOW
			"/a/b/c" IS A SYMBOLIC LINK TO TEST WHETHER IT IS ALSO A DIRECTORY OR NOT.
			
	THIS FUNCTION WILL RETURN false IF pVerified1Utf8PathPrefix IS EMPTY. IN OTHER WORDS, DO NOT 
	CALL THIS FUNCTION IF crx_c_os_fileSystem_verifyFileSystemPath() WAS CALLED ON A ROUTE.
	A PARTIAL FILE SYSTEM PATH NEVER RESOLVES.
	
	pExistance SHALL HOLD THE STATE OF THE FILE OR DIRECTORY AFTER THE FUNCTION RETURNS.
	
	PASSING UINT32_MAX TO pDepthLimitOfReparsePoints ENABLES ALL REPARSE POINTS.
	
	pVerified1Utf8Route CAN BE MODIFIED BY THIS FUNCTION WHEN THE RESOLUTION MODEL IS
			CRX__C__OS__FILE_SYSTEM__RESOLUTION_MODEL__SLOT_AWARE. MAKE SURE TO USE THE MODIFIED
			VALUE AFTER THE FUNCTION RETURNS.

	REMEMBER: pVerified1Utf8PathPrefix ENDS WITH '/' OR '\' DEPENDING ON THE UNDERLYING OPERATING
			SYSTEM.
	REMEMBER: THIS FUNCTION EXPECTS THE PATH TO BE ALREADY NORMALIZED.
	REMEMBER: A NON EXISTANT FILE OR DIRECTORY EVALUATE TO FALSE. (SHOULD NO LONGER BE VALID. 
																	DOUBLE CHECK CODE.)
	REMEMBER: IF *pOsFileHandle IS -1 IT IS ASSUMED THAT THE FILE WAS NOT FOUND. CALLING CODE MUST
			ENSURE THAT THIS IS INDEED THE CASE, AND NOT SOME OTHER ERROR.
	NOTE: ON WINDOWS, GETTING A HANDLE TO DIRECTORY IS NOT STRAIGHT FARWARD AND REQUIRES EXTRA
			PRIVILAGES. HENCE, pOsFileHandle CAN BE EXPECTED TO BE NULL IN THIS CASE. AND IN 
			GENERAL, JUST PASSING NULL UNDER WINDOWS IS NOT AN ISSUE AS IT IS ON OTHER SYSTEM WHICH
			REQUIRE THAT WE WALK THE PATH FOR VERY LONG PATH.
			
	pExclusiveEndIndexOfExistingRoute HOLDS THE EXCLUSIVE END INDEX IN pVerified1Utf8Route FOR
	THE FULL ROUTE THAT WAS FOUND ON THE SYSTEM WHEN pExistance IS
	CRX__C__OS__FILE_SYSTEM__EXISTANCE__DOES_NOT_EXIST OR 
	CRX__C__OS__FILE_SYSTEM__EXISTANCE__EXISTS.
*/
bool crx_c_os_fileSystem_verifyFileSystemPath2(
		Crx_C_Os_FileSystem_Contract const * pContract, Crx_C_Os_FileSystem_Operation pOperation,
		Crx_C_Os_FileSystem_OsFileHandle * pOsFileHandle, uint32_t pDepthLimitOfReparsePoints,
		Crx_C_Os_FileSystem_Existance * CRX_NOT_NULL pExistance,
		Crx_C_String const * CRX_NOT_NULL pVerified1Utf8PathPrefix, 
		Crx_C_String * CRX_NOT_NULL  pVerified1Utf8Route, 
		size_t * pExclusiveEndIndexOfExistingRoute)
{
	/*
		- ABOUT POSIX:
			- WE CAN TRUST THAT ONLY ONE HARDLNK (ONE PATH) EXISTS FOR A DIRECTORY.
			- TWO HARDLINKS POINTING TO THE SAME FILE MEANS THAT WE HAVE ONE FILE WITH TWO
					DIFFERENT PATH. IF BOTH FILES ARE OPENED, THE FOLLOWING CAN BE USED
					TO CONFIRM THAT THEY ARE THE SAME FILE
				
							int areFilesTheSameFile(int fd1, int fd2)
							{
								struct stat stat1; 
								struct stat stat2;

								if(fstat(fd1, &stat1) < 0)
									{return false;}
								else if(fstat(fd2, &stat2) < 0)
									{return false;}
								else
								{
									return (stat1.st_dev == stat2.st_dev) && 
											(stat1.st_ino == stat2.st_ino);
								}
							}
			- A SYMLINK, AKA SOFTLINK, IS JUST A SHORTCUT.
	*/
	CRX_SCOPE_META

	CRX_SCOPE
	
	bool vIsNotDone = true;
	bool vReturn = false;
	#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	Crx_NonCrxed_Os_Posix_Dll_Libc_Stat tStat /*= ?*/;
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	BY_HANDLE_FILE_INFORMATION vByHandleFileInformation /*= ?*/;
	#endif
	
	*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__NULL;

	if(pExclusiveEndIndexOfExistingRoute != NULL)
		{*pExclusiveEndIndexOfExistingRoute = crx_c_string_getSize(pVerified1Utf8Route);}

	if(pOsFileHandle != NULL)
	{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
		if(*pOsFileHandle != CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE)
		{
			if(crx_c_os_fileSystem_private_posix_callFstat(*pOsFileHandle, &vStat) == 0)
			{
				if(S_ISDIR(vStat))
				{
					if(!crx_c_string_isEndingWith2(pVerified1Utf8Route, "/", false))
					{
						*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
						vIsNotDone = false;
					}
				}
				else
				{
					if(crx_c_string_isEndingWith2(pVerified1Utf8Route, "/", false))
					{
						*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
						vIsNotDone = false;
					}
				}
			}
			else
				{vIsNotDone = false;}
		}
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
		if(*pOsFileHandle != CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE)
		{
			if(crx_c_os_fileSystem_internal_win32_getFileInformationByHandle(*pOsFileHandle, 
					&vByHandleFleInformation) != 0)
			{
				if(vByHandleFleInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					if(!crx_c_string_isEndingWith2(pVerified1Utf8Route, "/", false))
					{
						*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
						vIsNotDone = false;
					}
				}
				else
				{
					if(crx_c_string_isEndingWith2(pVerified1Utf8Route, "/", false))
					{
						*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
						vIsNotDone = false;
					}
				}
			}
			else
				{vIsNotDone = false;}
		}
#endif
	}
	
	if(vIsNotDone)
	{
		assert(*pExistance == CRX__C__OS__FILE_SYSTEM__EXISTANCE__NULL);
		/*if(((pContract->gFileSystemRuntimeGuaranteeLevel == 3) ||
				((pContract->gFileSystemRuntimeGuaranteeLevel == 1) && 
						((pOperation == CRX__C__OS__FILE_SYSTEM__OPERATION__READ) ||
						(pOperation == CRX__C__OS__FILE_SYSTEM__OPERATION__PATH_RETREIVAL)))) && 
						(tDepthRemainingForReparsePoints == UINT32_MAX))
			{return true;}
		else*/ if(crx_c_string_getSize(pVerified1Utf8PathPrefix) == 0)
			{vIsNotDone = false /*true 	<==  WRANING: I AM UNSURE IF THIS WAS INDEED AN ERROR*/;}
		/*else if(crx_c_string_getIndexOfFirstOccuranceOf3(pVerified1Utf8Path, 0, 
				CRX__OS__FILE_SYSTEM__SEPERATOR_STRING, 1, 0) == ((size_t)(-1)))
			{return false;}*/
		else if(crx_c_string_isEqualTo3(pVerified1Utf8Route, 
				CRX__OS__FILE_SYSTEM__SEPERATOR_STRING /*"/"*/, 1, false))
		{
			vReturn = true;
			*pExistance == CRX__C__OS__FILE_SYSTEM__EXISTANCE__EXISTS;
			vIsNotDone = false;
		}
	}
	
	if(vIsNotDone)
	{
		bool tIsToCheckForReparsePointsOnly = 
				!(((pContract->gFileSystemRuntimeGuaranteeLevel == 1) && 
				(pOperation == CRX__C__OS__FILE_SYSTEM__OPERATION__WRITE)) ||
				(pContract->gFileSystemRuntimeGuaranteeLevel == 2));
		uint32_t tDepthRemainingForReparsePoints = pDepthLimitOfReparsePoints;
		char * tChars = crx_c_string_constantGetCharsPointer(pVerified1Utf8Route);
		size_t tLength = crx_c_getLength(pVerified1Utf8Route);
		bool tIsADirectory = (*(tChars + tLength - 1) == CRX__OS__FILE_SYSTEM__SEPERATOR_CODE);
		size_t tCurrentIndex = 0;
		size_t tCurrentEndIndex = crx_c_string_getIndexOfFirstOccuranceOf3(pVerified1Utf8Route, 
				0, CRX__OS__FILE_SYSTEM__SEPERATOR_STRING, 1, 0);
		bool tIsLast = false;
		bool tIsNoSystemApiError = true;
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
		DIR * tDir = NULL;
		Crx_NonCrxed_Os_Posix_Dll_Libc_Dirent * tDirent = NULL;
		Crx_C_Os_FileSystem_Internal_Posix_FileHandle tFileHandle /*= ?*/;
		Crx_C_String tRemainingPath /*= ?*/;
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
		Crx_C_Os_FileSystem_Internal_Win32_Win32FindData tWin32FindData /*= ?*/;
		HANDLE tHandle /*= ?*/;
		Crx_C_String tRaw_seperator /*= ?*/;//IN OS ENCODING, NOT UTF8
		Crx_C_String tRaw_currentFullPath /*= ?*/;	//IN OS ENCODING, NOT UTF8
		Crx_C_String tRaw_currentPathSegmentString /*= ?*/;	//CARRIES A PATH SEGMENT IN THE OS ENCODING, NOT UTF8.
		Crx_C_String tCurrentPathSegmentString /*= ?*/;	//CARRIES A PATH SEGMENT
		Crx_C_String tTempString /*= ?*/;
#endif

#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
		crx_c_os_fileSystem_internal_posix_fileHandle_construct(&tFileHandle);
		crx_c_string_construct(&tRemainingPath);
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
		crx_c_os_fileSystem_internal_win32_win32FindData_construct(&tWin32FindData);
		crx_c_string_construct(&tRaw_seperator);
		crx_c_string_construct(&tRaw_currentFullPath);
		crx_c_string_construct(&tRaw_currentPathSegmentString);
		crx_c_string_construct(&tCurrentPathSegmentString);
		crx_c_string_construct(&tTempString);
#endif

		if((tCurrentEndIndex == tLength) || (tCurrentEndIndex == ((size_t)(-1))))
		{
			tCurrentEndIndex = tLength;

			tIsLast = true;
		}
		
		//ROUTE HERE MUST NOT BEGIN WITH '/'
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
		if(!tIsToCheckForReparsePointsOnly)
		{
			if((pContract->gResolutionModel == CRX__C__OS__FILE_SYSTEM__RESOLUTION_MODEL__NULL) ||
					(pContract->gResolutionModel == 
							CRX__C__OS__FILE_SYSTEM__RESOLUTION_MODEL__PRECISE))
			{
				if(crx_c_os_fileSystem_internal_posix_lookAt2(&tFileHandle, pVerified1Utf8PathPrefix,
						&tRemainingPath))
				{
					if(crx_c_os_fileSystem_internal_posix_replaceOpenat(&tFileHandle,
							crx_c_string_getCString(tRemainingPath), 
							O_RDONLY /*| O_DIRECTORY DOES NOT EXIST ON MACOS 10.7*/, 0))
						{crx_c_os_fileSystem_internal_posix_fdopendir(&tDir, &tFileHandle);}
					else
						{crx_c_os_fileSystem_internal_posix_close(&tFileHandle);}
				}

				if(tDir != NULL)
				{
					tDirent = crx_c_os_fileSystem_internal_posix_readdir(tDir);
					
					if(tDirent == NULL)
					{
						int32_t tErrorCode = errno; //FROM readdir
						Crx_C_String tString /*= ?*/;

						crx_c_string_construct(&tString);
						
						Crx_NonCrxed_Os_Posix_Dll_Libc_Closedir(tDir);
						tDir = NULL;
						crx_c_os_fileSystem_internal_posix_close(&tFileHandle);

						crx_c_string_appendString(&tString, pVerified1Utf8PathPrefix);
						crx_c_string_appendString(&tString, pVerified1Utf8Route);

						if(crx_c_os_fileSystem_internal_posix_lookAt2(&tFileHandle, 
								&tString, &tRemainingPath))
						{
							Crx_NonCrxed_Os_Posix_Dll_Libc_Stat tStat;

							if(!crx_c_os_fileSystem_internal_posix_fstatat(&tFileHandle, &tRemainingPath, 
									&tStat, false))
								{tIsNoSystemApiError = false;}
						}
						else
							{tIsNoSystemApiError = false;}

						if(!tIsNoSystemApiError)
						{
							if(tErrorCode == ENOENT)
							{
								tIsNoSystemApiError = true;

								if((pOsFileHandle == NULL) || (*pOsFileHandle == -1))
									{*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__DOES_NOT_EXIST;}
								else
									{*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;}
							}
						}

						crx_c_string_destruct(&tString);
					}
				}
				else
				{
					tIsNoSystemApiError = false;//REMEMBER, THIS IS CONCERNING A MOUNT POINT
					
					crx_c_os_fileSystem_internal_posix_close(&tFileHandle);
				}

				if(tIsNoSystemApiError && (tDirent != NULL))
				{
					Crx_C_String tCurrentPathSegmentString /*= ?*/;
					Crx_C_String tFirstEntryAtCurrentLevel /*= ?*/;
					bool tIsFoundAtCurrentLevel = false;
					bool tIsFirstAtCurrentLevel = true;
					Crx_NonCrxed_Os_Posix_Dll_Libc_Stat tStat /*= ?*/;
					bool tIsToContinue = true;

					crx_c_string_construct(&tCurrentPathSegmentString);
					crx_c_string_construct(&tFirstEntryAtCurrentLevel);
					CRX__SET_TO_ZERO(Crx_NonCrxed_Os_Posix_Dll_Libc_Stat, tStat);

					crx_c_string_appendCString(&tCurrentPathSegmentString, tChars + tCurrentIndex, 
							tCurrentEndIndex - tCurrentIndex);

					while(tIsToContinue)
					{
						bool tIsNotToSkipCurrent = 
								((crx_c_fileSystem_strcmp(tDirent->d_name, '.') != 0) && 
								(crx_c_fileSystem_strcmp(tDirent->d_name, '..') != 0));

						if(tIsNotToSkipCurrent && tIsFirstAtCurrentLevel)
						{
							crx_c_string_appendCString(&tFirstEntryAtCurrentLevel, tDirent->d_name);
							tIsFirstAtCurrentLevel = false;
						}

						if(tIsNotToSkipCurrent && crx_c_string_isEqualTo2(&tCurrentPathSegmentString, 
								tDirent->d_name, true))
						{
							/*CONFIRMED AT_SYMLINK_NOFOLLOW IS SUPPORTED ON ALL OF LINUX, MACOS, AND 
									FREEBSD WHEN fstatat() IS SUPPORTED*/
							/*THIS SHOULD ATUOMATICALLY BE PER THE stat64 IN MACOS 10.7 AND UP, I HOPE.*/
							bool tIsSymbolicLink = false;

							if(crx_c_os_fileSystem_internal_posix_fstatat(&tFileHandle,
									tDirent->d_name, &tStat, true))
							{
								tIsSymbolicLink = (S_ISLNK(tStat) ? true : false);
								
								//CHECKING BIND MOUNTS ON LINUX AND POSSIBLY FREEBSD BUT UNSURE IF THIS
								//		WORKS ON FREEBSD 
								//		{SRC:http://blog.schmorp.de/2016-03-03-detecting-a-mount-point.html}
								//REMEMBER, THE FOLLOWING IS DETECTING ALL MOUNTS, AND NOT ONLY BIND 
								//		MOUNTS. HOWEVER
								if(!tIsSymbolicLink) 
								{
									tIsSymbolicLink = crx_c_os_fileSystem_private_posix_isAMountPointAt(
											&tFileHandle, tDirent->d_name);
								}
							}
							else
								{tIsNoSystemApiError = false;}
							
							if(tIsNoSystemApiError && crx_c_os_fileSystem_internal_posix_fstatat(
									&tFileHandle, tDirent->d_name, &tStat, false))
							{
								if(S_ISDIR(tStat))
								{
									if(tIsLast)
									{
										if(tIsADirectory)
										{
											if((tDepthRemainingForReparsePoints == 0) && 
													tIsSymbolicLink)
											{
												*pExistance = 
														CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
											}
											
											tIsFoundAtCurrentLevel = true;
										}
										else
										{
											/* CHOOSING TO CONTINUE LOOKING FOR FILE WITH SAME NAME EVEN IF 
													THIS IS WRONG ON POSIX BASED SYSTEMS */
											*pExistance = 
													CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
										}
									}
									else
									{
										tIsFoundAtCurrentLevel = true;

										if((tDepthRemainingForReparsePoints == 0) && tIsSymbolicLink)
										{
											*pExistance = 
													CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
										}
									}
								}
								else if(S_ISREG(tStat))
								{
									if(tIsLast)
									{
										if(!tIsADirectory)
										{
											tIsFoundAtCurrentLevel = true;

											if((tDepthRemainingForReparsePoints == 0) && 
													tIsSymbolicLink)
											{
												*pExistance = 
														CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
											}
										}
										else
										{
											/* CHOOSING TO CONTINUE LOOKING FOR DIRECTORY WITH SAME NAME EVEN IF 
													THIS IS WRONG ON POSIX BASED SYSTEMS */
											*pExistance = 
													CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
										}
									}
									else
									{
										/* CHOOSING TO CONTINUE LOOKING FOR DIRECTORY WITH SAME NAME 
												EVEN IF THIS IS WRONG ON POSIX BASED SYSTEMS */
										*pExistance = 
												CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
									}
								}
								else
								{
									/* CHOOSING TO CONTINUE LOOKING FOR DIRECTORY OR FILE WITH SAME NAME 
											EVEN IF THIS IS WRONG ON POSIX BASED SYSTEMS */
									*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
								}
							}
							else
								{tIsNoSystemApiError = false;}
						}

						if(tIsNoSystemApiError)
						{
							if(tIsFoundAtCurrentLevel)
							{
								Crx_NonCrxed_Os_Posix_Dll_Libc_Stat tStat2/*= ?*/;
								bool tIsNoConflict = true;

								//REMEMBER, THE DIRENT POINTS THE ENTITY ITSELF, NOT WHAT IT IS POINTING
								//		TO IN THE CASE WHERE IT IS A SYMBOLIC LINK.
								if(crx_c_os_fileSystem_internal_posix_fstatat(&tFileHandle,
										crx_c_string_getCString(&tCurrentPathSegmentString), &tStat, 
										true))
								{
									tIsNoConflict = 
											crx_c_os_fileSystem_private_posix_isStatOfFileLikelySameAsOneFromDirent(
											tDirent, &tStat);
								}
								else
									{tIsNoSystemApiError = false;}
								
								if(tIsNoSystemApiError && tIsNoConflict)
								{
									tDirent = crx_c_os_fileSystem_internal_posix_readdir(tDir);
									
									if(tDirent != NULL)
									{
										crx_c_os_fileSystem_internal_posix_fstatat(&tFileHandle,
												tDirent->d_name, &tStat2, true);
									}
									else
									{
										crx_c_os_fileSystem_internal_posix_fstatat(&tFileHandle,
												crx_c_string_getCString(&tFirstEntryAtCurrentLevel), 
												&tStat2, true);
									}
									
									if(crx_c_os_fileSystem_internal_posix_fstatat(&tFileHandle,
											crx_c_string_getCString(&tCurrentPathSegmentString), 
											&tStat2, true))
									{
										tIsNoConflict = ((tStat2.st_dev == tStat.st_dev) && 
												(tStat2.st_ino == tStat.st_ino));
									}
									else
										{tIsNoSystemApiError = false;}
								}
								
								if(tIsLast && tIsNoSystemApiError && tIsNoConflict)
								{
									if(pOsFileHandle != NULL)
									{
										//WE HAVE TO DO THIS AGAIN, BECAUSE tStat WAS NOT WHILE 
										//	FOLLOWING SYMBOLIC LINKS, WHILE THE HANDLE FROM THE
										//	CALLER IS EXPECTED TO HAVE FOLLOWED SYMBOLIC LINKS.
										if(crx_c_os_fileSystem_internal_posix_fstatat(&tFileHandle,
												crx_c_string_getCString(&tCurrentPathSegmentString), 
												&tStat, false))
										{
											tIsNoConflict = ((vStat.st_dev == tStat.st_dev) && 
													(vStat.st_ino == tStat.st_ino));
										}
										else
											{tIsNoSystemApiError = false;}
									}
								}
								
								if(tIsNoSystemApiError)
								{
									if(!tIsNoConflict)
									{
										*pExistance = 
												CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
									}
									else
									{
										if(!tIsLast)
										{
											if(crx_c_os_fileSystem_internal_posix_replaceOpenat(
													&tFileHandle, crx_c_string_getCString(
													&tCurrentPathSegmentString), O_RDONLY) &&
													crx_c_os_fileSystem_internal_posix_fdopendir(
													&tDir, &tFileHandle))
											{
												tDirent = crx_c_os_fileSystem_internal_posix_readdir(
														tDir);

												if(tDirent != NULL)
												{
													if(tDepthRemainingForReparsePoints > 0)
													{
														tDepthRemainingForReparsePoints = 
																tDepthRemainingForReparsePoints - 1;
													}

													tCurrentIndex = tCurrentEndIndex + 1;
													tCurrentEndIndex = 
															crx_c_string_getIndexOfFirstOccuranceOf3(
															pVerified1Utf8Route, tCurrentIndex, "/", 1, 
															0);

													if(tCurrentEndIndex == ((size_t)(-1)))
													{
														tCurrentEndIndex = tLength;

														tIsLast = true;
													}
													else if(tIsADirectory && 
															(tCurrentEndIndex == (tLength - 1)))
														{tIsLast = true;}
														
													crx_c_string_empty(&tCurrentPathSegmentString);
													crx_c_string_appendCString(
															&tCurrentPathSegmentString,
															tChars + tCurrentIndex, 
															tCurrentEndIndex - tCurrentIndex);
															
													tIsFoundAtCurrentLevel = false;
													tIsFirstAtCurrentLevel = true;
													crx_c_string_empty(&tFirstEntryAtCurrentLevel);
												}
												else
												{
													if(errno == 0)
													{
														*pExistance = 
																CRX__C__OS__FILE_SYSTEM__EXISTANCE__DOES_NOT_EXIST;
													}
													else
														{tIsNoSystemApiError = false;}

													tIsToContinue = false;
												}
											}
											else
											{
												tIsNoSystemApiError = false;

												tIsToContinue = false;
											}
										}
										else
										{
											*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__EXISTS;

											tIsToContinue = false;
										}
									}
								}
								else
									{tIsToContinue = false;}
							}
							else
							{
								tDirent = crx_c_os_fileSystem_internal_posix_readdir(tDir);
								
								if(tDirent == NULL)
								{
									if(errno == 0)
									{
										if(*pExistance == CRX__C__OS__FILE_SYSTEM__EXISTANCE__NULL)
										{
											if(!crx_c_os_fileSystem_internal_posix_fstatat(&tFileHandle,
													crx_c_string_getCString(&tCurrentPathSegmentString), 
													&tStat, true))
											{
												if(errno == ENOENT)
												{
													if((pOsFileHandle == NULL) || 
															(*pOsFileHandle == -1))
													{
														*pExistance = 
																CRX__C__OS__FILE_SYSTEM__EXISTANCE__DOES_NOT_EXIST;
													}
													else
													{
														*pExistance = 
																CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
													}
												}
												else
													{tIsNoSystemApiError = false;}
											}
											else
											{
												*pExistance = 
														CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
											}
										}
									}
									else
										{tIsNoSystemApiError = false;}

									tIsToContinue = false;
								}
							}
						}
						else
							{tIsToContinue = false;}

						if(!tIsToContinue)
						{
							if(tDir != NULL)
							{
								Crx_NonCrxed_Os_Posix_Dll_Libc_Closedir(tDir);
								tDir = NULL;
							}

							crx_c_os_fileSystem_internal_posix_close(&tFileHandle);
							tDirent = NULL;
						}
					}
					
					crx_c_string_destruct(&tCurrentPathSegmentString);
					crx_c_string_destruct(&tFirstEntryAtCurrentLevel);
				}

				if(tIsNoSystemApiError && (
						(*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__EXISTS) ||
						(*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__DOES_NOT_EXIST)))
					{vReturn = true;}
			}
			else if((pContract->gResolutionModel == 
							CRX__C__OS__FILE_SYSTEM__RESOLUTION_MODEL__PRECISE_AND_SLOT_AWARE) ||
					(pContract->gResolutionModel == 
							CRX__C__OS__FILE_SYSTEM__RESOLUTION_MODEL__SLOT_AWARE))
			{
				if(crx_c_os_fileSystem_internal_posix_lookAt2(&tFileHandle, pVerified1Utf8PathPrefix,
						&tRemainingPath))
				{
					if(crx_c_os_fileSystem_internal_posix_replaceOpenat(&tFileHandle,
							crx_c_string_getCString(tRemainingPath), 
							O_RDONLY /*| O_DIRECTORY DOES NOT EXIST ON MACOS 10.7*/, 0))
						{crx_c_os_fileSystem_internal_posix_fdopendir(&tDir, &tFileHandle);}
					else
						{crx_c_os_fileSystem_internal_posix_close(&tFileHandle);}
				}

				if(tDir != NULL)
				{
					tDirent = crx_c_os_fileSystem_internal_posix_readdir(tDir);
					
					if(tDirent == NULL)
					{
						int32_t tErrorCode = errno; //FROM readdir
						Crx_C_String tString /*= ?*/;

						crx_c_string_construct(&tString);
						
						Crx_NonCrxed_Os_Posix_Dll_Libc_Closedir(tDir);
						tDir = NULL;
						crx_c_os_fileSystem_internal_posix_close(&tFileHandle);

						crx_c_string_appendString(&tString, pVerified1Utf8PathPrefix);
						crx_c_string_appendString(&tString, pVerified1Utf8Route);

						if(crx_c_os_fileSystem_internal_posix_lookAt2(&tFileHandle, 
								&tString, &tRemainingPath))
						{
							Crx_NonCrxed_Os_Posix_Dll_Libc_Stat tStat;

							if(!crx_c_os_fileSystem_internal_posix_fstatat(&tFileHandle, &tRemainingPath, 
									&tStat, false))
								{tIsNoSystemApiError = false;}
						}
						else
							{tIsNoSystemApiError = false;}

						if(!tIsNoSystemApiError)
						{
							if(tErrorCode == ENOENT)
							{
								tIsNoSystemApiError = true;

								if((pOsFileHandle == NULL) || (*pOsFileHandle == -1))
									{*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__DOES_NOT_EXIST;}
								else
								{
									if(pContract->gResolutionModel == 
											CRX__C__OS__FILE_SYSTEM__RESOLUTION_MODEL__PRECISE_AND_SLOT_AWARE)
									{
										*pExistance = 
												CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
									}
									else
									{
										*pExistance = 
												CRX__C__OS__FILE_SYSTEM__EXISTANCE__AMBIGUOUS;
									}
								}
							}
						}

						crx_c_string_destruct(&tString);
					}
				}
				else
				{
					tIsNoSystemApiError = false;//REMEMBER, THIS IS CONCERNING A MOUNT POINT
					
					crx_c_os_fileSystem_internal_posix_close(&tFileHandle);
				}

				if(tIsNoSystemApiError && (tDirent != NULL))
				{
					Crx_C_String tCurrentPathSegmentString /*= ?*/;
					Crx_C_String tCurrentEntityNameFromOs /*= ?*/;
					Crx_C_String tTempString /*= ?*/;
					Crx_C_String tVerifiedUtf8Route /*= ?*/;
					uint32_t tNumberOfFoundAtCurrentLevel = 0;
					bool tIsPreciselyFoundAtCurrentLevel = false;
					Crx_NonCrxed_Os_Posix_Dll_Libc_Stat tStat /*= ?*/;
					bool tIsToContinue = true;

					crx_c_string_construct(&tCurrentPathSegmentString);
					crx_c_string_construct(&tCurrentEntityNameFromOs);
					crx_c_string_construct(&tTempString);
					crx_c_string_construct(&tVerifiedUtf8Route);
					CRX__SET_TO_ZERO(Crx_NonCrxed_Os_Posix_Dll_Libc_Stat, tStat);

					crx_c_string_appendCString(&tCurrentPathSegmentString, tChars + tCurrentIndex, 
							tCurrentEndIndex - tCurrentIndex);

					while(tIsToContinue)
					{
						bool tIsNotToSkipCurrent = 
								((crx_c_fileSystem_strcmp(tDirent->d_name, '.') != 0) && 
								(crx_c_fileSystem_strcmp(tDirent->d_name, '..') != 0));
						bool tIsAMatch = false;

						crx_c_string_appendCString(&tTempString, tDirent->d_name);
						crx_c_os_osString_fileSystem_normalizeToUnicodeNfd(
								&tCurrentEntityNameFromOs, &tTempString, true);
						crx_c_string_empty(&tTempString);

						if(tIsNotToSkipCurrent && (crx_c_os_osString_fileSystem_compareStrings(
								&tCurrentPathSegmentString, &tCurrentEntityNameFromOs, true) == 0))
						{
							tNumberOfFoundAtCurrentLevel = tNumberOfFoundAtCurrentLevel + 1;

							if(tNumberOfFoundAtCurrentLevel > 1)
							{
								if(pContract->gResolutionModel == 
										CRX__C__OS__FILE_SYSTEM__RESOLUTION_MODEL__PRECISE_AND_SLOT_AWARE)
								{
									*pExistance = 
											CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
								}
								else
								{
									*pExistance = 
											CRX__C__OS__FILE_SYSTEM__EXISTANCE__AMBIGUOUS;
								}
								
								tIsToContinue = false;
								tIsNotToSkipCurrent = false;
							}
							else
							{
								if(pContract->gResolutionModel == 
										CRX__C__OS__FILE_SYSTEM__RESOLUTION_MODEL__PRECISE_AND_SLOT_AWARE)
								{
									if(crx_c_string_isEqualTo2(&tCurrentPathSegmentString, 
											tDirent->d_name, false))
										{tIsPreciselyFoundAtCurrentLevel = true;}
									else
									{
										*pExistance = 
												CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
										tIsToContinue = false;
										tIsNotToSkipCurrent = false;
									}
								}
							}
							
							tIsAMatch = true;
							
							if(tIsToContinue)
							{
								if(crx_c_string_getLength(&tVerifiedUtf8Route) > 0)
									{crx_c_string_appendCString(&tVerifiedUtf8Route, "/");}

								crx_c_string_appendCString(&tVerifiedUtf8Route, tDirent->d_name);
							}
						}

						if(tIsNotToSkipCurrent && tIsAMatch)
						{
							/*CONFIRMED AT_SYMLINK_NOFOLLOW IS SUPPORTED ON ALL OF LINUX, MACOS, AND 
									FREEBSD WHEN fstatat() IS SUPPORTED*/
							/*THIS SHOULD ATUOMATICALLY BE PER THE stat64 IN MACOS 10.7 AND UP, I HOPE.*/
							bool tIsSymbolicLink = false;

							if(crx_c_os_fileSystem_internal_posix_fstatat(&tFileHandle,
									tDirent->d_name, &tStat, true))
							{
								tIsSymbolicLink = (S_ISLNK(tStat) ? true : false);
								
								//CHECKING BIND MOUNTS ON LINUX AND POSSIBLY FREEBSD BUT UNSURE IF THIS
								//		WORKS ON FREEBSD 
								//		{SRC:http://blog.schmorp.de/2016-03-03-detecting-a-mount-point.html}
								//REMEMBER, THE FOLLOWING IS DETECTING ALL MOUNTS, AND NOT ONLY BIND 
								//		MOUNTS. HOWEVER
								if(!tIsSymbolicLink) 
								{
									tIsSymbolicLink = crx_c_os_fileSystem_private_posix_isAMountPointAt(
											&tFileHandle, tDirent->d_name);
								}
							}
							else
								{tIsNoSystemApiError = false;}
							
							if(tIsNoSystemApiError && crx_c_os_fileSystem_internal_posix_fstatat(
									&tFileHandle, tDirent->d_name, &tStat, false))
							{
								if(S_ISDIR(tStat))
								{
									if(tIsLast)
									{
										if(tIsADirectory)
										{
											if((tDepthRemainingForReparsePoints == 0) && 
													tIsSymbolicLink)
											{
												*pExistance = 
														CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
											}
										}
										else
										{
											/* CHOOSING TO CONTINUE LOOKING FOR FILE WITH SAME NAME EVEN IF 
													THIS IS WRONG ON POSIX BASED SYSTEMS */
											*pExistance = 
													CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
										}
									}
									else
									{
										if((tDepthRemainingForReparsePoints == 0) && tIsSymbolicLink)
										{
											*pExistance = 
													CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
										}
									}
								}
								else if(S_ISREG(tStat))
								{
									if(tIsLast)
									{
										if(!tIsADirectory)
										{
											if((tDepthRemainingForReparsePoints == 0) && 
													tIsSymbolicLink)
											{
												*pExistance = 
														CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
											}
										}
										else
										{
											*pExistance = 
													CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
										}
									}
									else
									{
										*pExistance = 
												CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
									}
								}
								else
								{
									*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
								}
							}
							else
								{tIsNoSystemApiError = false;}
							
							if(tIsNoSystemApiError)
							{
								if(*pExistance == CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED)
								{
									tIsToContinue = false;
									tIsNotToSkipCurrent = false;
									
									if(pContract->gResolutionModel == 
											CRX__C__OS__FILE_SYSTEM__RESOLUTION_MODEL__SLOT_AWARE)
									{
										*pExistance = 
												CRX__C__OS__FILE_SYSTEM__EXISTANCE__AMBIGUOUS;
									}
								}
							}
						}

						if(tIsNoSystemApiError && tIsToContinue)
						{
							tDirent = crx_c_os_fileSystem_internal_posix_readdir(tDir);
							
							if(tDirent == NULL)
							{
								if(errno == 0)
								{
									if(tNumberOfFoundAtCurrentLevel == 0)
									{
										*pExistance = 
												CRX__C__OS__FILE_SYSTEM__EXISTANCE__DOES_NOT_EXIST;
										tIsToContinue = false;
										
										crx_c_string_appendChars(&tVerifiedUtf8Route, 
												crx_c_string_getChars(pVerified1Utf8Route) + 
														tCurrentIndex, 
												crx_c_string_getChars(pVerified1Utf8Route) + 
														crx_c_string_getSize(pVerified1Utf8Route) - 
														tCurrentIndex);
														
										/*if(tCurrentEndIndex != tLength)
											{crx_c_string_appendCString(&tVerifiedUtf8Route, "/");}*/
										
										if(pExclusiveEndIndexOfExistingRoute != NULL)
											{*pExclusiveEndIndexOfExistingRoute = tCurrentIndex;}
									}
									else
									{
										assert(tNumberOfFoundAtCurrentLevel == 1);
										
										if(!tIsLast)
										{
											if(crx_c_os_fileSystem_internal_posix_replaceOpenat(
													&tFileHandle, crx_c_string_getCString(
													&tCurrentPathSegmentString), O_RDONLY) &&
													crx_c_os_fileSystem_internal_posix_fdopendir(
													&tDir, &tFileHandle))
											{
												tDirent = crx_c_os_fileSystem_internal_posix_readdir(
														tDir);

												if(tDirent != NULL)
												{
													if(tDepthRemainingForReparsePoints > 0)
													{
														tDepthRemainingForReparsePoints = 
																tDepthRemainingForReparsePoints - 1;
													}

													tCurrentIndex = tCurrentEndIndex + 1;
													tCurrentEndIndex = 
															crx_c_string_getIndexOfFirstOccuranceOf3(
															pVerified1Utf8Route, tCurrentIndex, "/", 
															1, 0);

													if(tCurrentEndIndex == ((size_t)(-1)))
													{
														tCurrentEndIndex = tLength;

														tIsLast = true;
													}
													else if(tIsADirectory && 
															(tCurrentEndIndex == (tLength - 1)))
														{tIsLast = true;}
														
													crx_c_string_empty(&tCurrentPathSegmentString);
													crx_c_string_appendCString(
															&tCurrentPathSegmentString,
															tChars + tCurrentIndex, 
															tCurrentEndIndex - tCurrentIndex);
															
													tNumberOfFoundAtCurrentLevel = 0;
													tIsPreciselyFoundAtCurrentLevel = false;
												}
												else
												{
													if(errno == 0)
													{
														*pExistance = 
																CRX__C__OS__FILE_SYSTEM__EXISTANCE__DOES_NOT_EXIST;
													}
													else
														{tIsNoSystemApiError = false;}

													tIsToContinue = false;
												}
											}
											else
											{
												tIsNoSystemApiError = false;

												tIsToContinue = false;
											}
										}
										else
										{
											*pExistance = 
													CRX__C__OS__FILE_SYSTEM__EXISTANCE__EXISTS;

											tIsToContinue = false;
										}
									}
								}
								else
									{tIsNoSystemApiError = false;}

								tIsToContinue = false;
							}
						}

						if(!tIsToContinue)
						{
							if(tDir != NULL)
							{
								Crx_NonCrxed_Os_Posix_Dll_Libc_Closedir(tDir);
								tDir = NULL;
							}

							crx_c_os_fileSystem_internal_posix_close(&tFileHandle);
							tDirent = NULL;
						}
					}
					
					if(tIsNoSystemApiError && (
							(*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__EXISTS) ||
							(*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__DOES_NOT_EXIST)))
					{
						if(crx_c_string_trySwapContentWith(&tVerifiedUtf8Route, pVerified1Utf8Route))
							{vReturn = true;}
						else
						{
							if(crx_c_string_copyAssignFrom(pVerified1Utf8Route, 
									&tVerifiedUtf8Route))
								{vReturn = true;}
							else
								{*pExistance = CRX__C__OS__FILE_SYSTEM__ERROR__NULL;}
						}
					}
					
					crx_c_string_destruct(&tCurrentPathSegmentString);
					crx_c_string_destruct(&tCurrentEntityNameFromOs);
					crx_c_string_destruct(&tTempString);
					crx_c_string_destruct(&tVerifiedUtf8Route);
				}
			}
		}
		else
		{
			Crx_C_String tString /*= ?*/;
			int32_t tFileDescriptor = -1;

			crx_c_string_construct(&tString);

			crx_c_string_appendCString(&tString, tChars + tCurrentIndex, 
					tCurrentEndIndex - tCurrentIndex);
					
			if(crx_c_os_fileSystem_internal_posix_lookAt2(&tFileHandle, pVerified1Utf8PathPrefix,
					&tRemainingPath))
			{
				if(!crx_c_os_fileSystem_internal_posix_replaceOpenat(&tFileHandle,
						crx_c_string_getCString(tRemainingPath), 
						O_RDONLY /*| O_DIRECTORY DOES NOT EXIST ON MACOS 10.7*/, 0))
					{crx_c_os_fileSystem_internal_posix_close(&tFileHandle);}
			}
			else
				{crx_c_os_fileSystem_internal_posix_close(&tFileHandle);}
			
			tIsNoSystemApiError = (tFileHandle.gFileDescriptor != -1);	//REMEMBER, THIS IS
																		//	CONCERNING A MOUNT 
																		//	POINT

			while(tFileHandle.gFileDescriptor != -1)
			{
				/*UNSURE IF AT_SYMLINK_NOFOLLOW IS REQUIRED. CONFIRMED AT_SYMLINK_NOFOLLOW 
						IS SUPPORTED ON ALL OF LINUX, MACOS, AND FREEBSD WHEN fstatat()
						IS SUPPORTED*/
				Crx_NonCrxed_Os_Posix_Dll_Libc_Stat tStat /*= ?*/;
				
				if(crx_c_os_fileSystem_internal_posix_fstatat(&tFileHandle, 
						crx_c_string_getCString(&tString), &tStat, true))
				{
					tIsSymbolicLink = (S_ISLNK(tStat) ? true : false);
					
					//CHECKING BIND MOUNTS ON LINUX AND POSSIBLY FREEBSD BUT UNSURE IF THIS
					//		WORKS ON FREEBSD 
					//		{SRC:http://blog.schmorp.de/2016-03-03-detecting-a-mount-point.html}
					//REMEMBER, THE FOLLOWING IS DETECTING ALL MOUNTS, AND NOT ONLY BIND 
					//		MOUNTS. HOWEVER
					if(!tIsSymbolicLink) 
					{
						tIsSymbolicLink = crx_c_os_fileSystem_private_posix_isAMountPointAt(
								&tFileHandle, crx_c_string_getCString(&tString));
					}
				}
				else
					{tIsNoSystemApiError = false;}

				if(tIsNoSystemApiError && crx_c_os_fileSystem_internal_posix_fstatat(
						&tFileHandle, crx_c_string_getCString(&tString), &tStat, false))
				{
					if((tDepthRemainingForReparsePoints > 0) || !tIsSymbolicLink)
					{
						if(S_ISDIR(tStat))
						{
							if(tIsLast)
							{
								crx_c_os_fileSystem_internal_posix_close(&tFileHandle);

								if(tIsADirectory)
								{
									*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__EXISTS;
									vReturn = true;
								}
								else
								{
									*pExistance = 
											CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
								}
							}
							else
							{
								if(crx_c_os_fileSystem_internal_posix_replaceOpenat(
										&tFileHandle, crx_c_string_getCString(&tString), 
										O_RDONLY /*| O_DIRECTORY DOES NOT EXIST ON MACOS 10.7*/, 
										0))
								{
									if(tDepthRemainingForReparsePoints > 0)
										{tDepthRemainingForReparsePoints = tDepthRemainingForReparsePoints - 1;}

									tCurrentIndex = tCurrentEndIndex + 1;
									tCurrentEndIndex = crx_c_string_getIndexOfFirstOccuranceOf3(
											pVerified1Utf8Route, tCurrentIndex, "/", 1, 0);

									if(tCurrentEndIndex == ((size_t)(-1)))
									{
										tCurrentEndIndex = tLength;

										tIsLast = true;
									}
									else if(tIsADirectory && 
											(tCurrentEndIndex == (tLength - 1)))
										{tIsLast = true;}

									crx_c_string_empty(&tString);
									crx_c_string_appendCString(&tString, tChars + tCurrentIndex, 
											tCurrentEndIndex - tCurrentIndex);
								}
								else
								{
									if(errno == ENOENT)
									{
										*pExistance = 
												CRX__C__OS__FILE_SYSTEM__EXISTANCE__DOES_NOT_EXIST;
									}
									else
										{tIsNoSystemApiError = false;}

									crx_c_os_fileSystem_internal_posix_close(&tFileHandle);
								}
							}
						}
						else if(S_ISREG(tStat))
						{
							crx_c_os_fileSystem_internal_posix_close(&tFileHandle);

							if(tIsLast)
							{
								if(!tIsADirectory)
								{
									*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__EXISTS;
									vReturn = true;
								}
								else
								{
									*pExistance = 
											CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
								}
							}
							else
							{
								*pExistance = 
										CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
							}
						}
						else
						{
							crx_c_os_fileSystem_internal_posix_close(&tFileHandle);

							*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
						}
					}
					else
					{
						crx_c_os_fileSystem_internal_posix_close(&tFileHandle);

						*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
					}
				}
				else
				{
					if(errno == ENOENT)
						{*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__DOES_NOT_EXIST;}
					else
						{tIsNoSystemApiError = false;}

					crx_c_os_fileSystem_internal_posix_close(&tFileHandle);
				}
			}

			crx_c_string_destruct(&tString);
		}
		
		if(!tIsNoSystemApiError)
			{*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__NULL;}
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
		crx_c_string_appendCString(&tTempString, CRX__OS__FILE_SYSTEM__SEPERATOR_STRING);
		crx_c_os_osString_unsafeConvertToOsString(&tRaw_seperator, &tTempString);
		crx_c_string_empty(&tTempString);
		
		crx_c_os_osString_unsafeConvertToOsString(&tRaw_currentFullPath, pVerified1Utf8PathPrefix);
		
		crx_c_string_appendChars(&tCurrentPathSegmentString, tChars + tCurrentIndex, 
				tCurrentEndIndex - tCurrentIndex);
		crx_c_os_osString_unsafeConvertToOsString(&tRaw_currentPathSegmentString, 
				&tCurrentPathSegmentString);
		
		//CHECKING FOR REPARSE POINTS ONLY IS COVERED BY THE CODE ABOVE.
		if(!tIsToCheckForReparsePointsOnly)
		{
			if((pContract->gResolutionModel == CRX__C__OS__FILE_SYSTEM__RESOLUTION_MODEL__NULL) ||
					(pContract->gResolutionModel == 
							CRX__C__OS__FILE_SYSTEM__RESOLUTION_MODEL__PRECISE))
			{
				HANDLE tHandle = INVALID_HANDLE_VALUE;
				Crx_C_String tRaw_asterix /*= ?*/;//IN OS ENCODING, NOT UTF8

				crx_c_string_construct(&tRaw_asterix);

				crx_c_string_appendCString(&tTempString, "*");
				crx_c_os_osString_unsafeConvertToOsString(&tRaw_asterix, &tTempString);
				crx_c_string_empty(&tTempString);
				
				if(pOsFileHandle != NULL)
				{
					crx_c_os_fileSystem_internal_win32_getFileInformationByHandle(pOsFileHandle, 
							&vByHandleFileInformation);
				}

				crx_c_string_appendString(&tRaw_currentFullPath, &tRaw_asterix);

				tHandle = crx_c_os_fileSystem_internal_win32_findFirstFile(true, &tRaw_currentFullPath, 
						&tWin32FindData);
						
				crx_c_string_removeChars(&tRaw_currentFullPath, 
						crx_c_string_getSize(&tRaw_currentFullPath) - 
								crx_c_string_getSize(&tRaw_asterix), 
						crx_c_string_getSize(&tRaw_asterix));
						
				if(tHandle == INVALID_HANDLE_VALUE)
				{
					int32_t tErrorCode = GetLastError();

					if((tErrorCode == ERROR_FILE_NOT_FOUND) ||
							(tErrorCode == ERROR_NO_MORE_FILES)) //UNSURE WHAT I WOULD GET WHEN USING *
					{
						Crx_C_String tString /*= ?*/;

						crx_c_string_construct(&tString);
						
						crx_c_string_appendString(&tString, pVerified1Utf8PathPrefix);
						crx_c_string_appendString(&tString, pVerified1Utf8Route);
						
						tHandle = crx_c_os_fileSystem_internal_win32_findFirstFile(true, 
								&tString, &tWin32FindData);

						if(tHandle == INVALID_HANDLE_VALUE)
						{
							if(tErrorCode == ENOENT)
							{
								tIsNoSystemApiError = true;

								if((pOsFileHandle == NULL) || (*pOsFileHandle == -1))
									{*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__DOES_NOT_EXIST;}
								else
									{*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;}
							}
							else
								{tIsNoSystemApiError = false;}
						}
						else
						{
							*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;

							crx_c_os_fileSystem_internal_win32_findClose(tHandle);
							tHandle = INVALID_HANDLE_VALUE;
						}

						crx_c_string_destruct(&tString);
					}
					else
						{tIsNoSystemApiError = false;}
				}
				else
					{tIsNoSystemApiError = false;}//REMEMBER, THIS IS CONCERNING THE CRX PATH PREFIX

				if(tIsNoSystemApiError && (tHandle != INVALID_HANDLE_VALUE))
				{
					Crx_C_String tFirstEntryAtCurrentLevel /*= ?*/;
					bool tIsFoundAtCurrentLevel = false;
					bool tIsFirstAtCurrentLevel = true;
					bool tIsToContinue = true;

					crx_c_string_construct(&tFirstEntryAtCurrentLevel);

					while(tIsToContinue)
					{
						bool tIsNotToSkipCurrent = ((!crx_c_string_isEqualTo2(
								&(tWin32FindData.cFileName), '.')  && !crx_c_string_isEqualTo2(
								&(tWin32FindData.cFileName), '..') == 0));
						
						if(tIsNotToSkipCurrent && tIsFirstAtCurrentLevel)
						{
							crx_c_string_appendCString(&tFirstEntryAtCurrentLevel, 
									&(tWin32FindData.cFileName));
							tIsFirstAtCurrentLevel = false;
						}
						
						/*FindFirstFileW/FindFirstFileA SEARCHES BOTH SHORT AND LONG FILE NAMES.
							cFileName SHOULD CONTAIN THE LONG FILE NAME ONLY, AND PER THE DESIGN,
							WE DO NOT DEAL WITH SHORT NAMES.
						*/
						if(tIsNotToSkipCurrent && crx_c_string_isEqualTo(&(tWin32FindData.cFileName), 
								&tCurrentPathSegmentString, true))
						{
							//QUOTE: "Junction Points and Symbolic Links are both Reparse Points"
							/*
								ON WINDOWS THE ASSUMPTION IS THAT IF THE ENTRY IS A REPARSE POINT,
								THE dwFileAttributes MEMBER ALSO TELLS US WHETHER IT POINTS TO
								A DIRECTORY OF FILE. I DO NOT THINK THAT THIS ACTUALLY TRUE, HOWEVER.
								TO RELIABLY KNOW WHETHER THE REPARSE POINT EVALUATES TO A DIRRECTORY
								OR FILE, IT SEEMS THAT ONE MUST USE DeviceIoControl(). THE FUNCTOIN
								EXISTS IN WIN95 AND UP, BUT THE OPTION "FSCTL_GET_REPARSE_POINT" DOES
								NOT EXIST IN WIN9X.
								
								A REPARSE POINT IS NOT NECCESSARILY A 
								"JUMP" POINT. IT IS SIMPLY A FILE THAT CONTAINS EXTRA INFORMATION
								THAT A DRIVER REGISTERED FOR THE TYPE OF INFORMATION WOULD BE
								CALLED FOR. HOWEVER, "JUMP" POINTS ARE IMPLEMENTED USING REPARSE
								POINT. "JUMP" POINTS LIKE JUNCTIONS POINTS AND SYMBOLIC LINKS.
								NOTE THAT CURRENTLY, ALL REPARSE POINTS AER TREATED AS "JUMP" POINTS.
							*/
							if(tWin32FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
							{
								if(tIsLast)
								{
									if(tIsADirectory)
									{
										if((tDepthRemainingForReparsePoints == 0) &&
												(tWin32FindData.dwFileAttributes & 
												FILE_ATTRIBUTE_REPARSE_POINT))
										{
											*pExistance = 
													CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
										}

										tIsFoundAtCurrentLevel = true;
									}
									else
									{
										/* CHOOSING TO CONTINUE LOOKING FOR FILE WITH SAME NAME */
										*pExistance = 
												CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
									}
								}
								else
								{
									if((tDepthRemainingForReparsePoints == 0) &&
											(tWin32FindData.dwFileAttributes & 
											FILE_ATTRIBUTE_REPARSE_POINT))
									{
										*pExistance = 
												CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
									}

									tIsFoundAtCurrentLevel = true;
								}
							}
							else
							{
								if(tIsLast)
								{
									if(!tIsADirectory)
									{
										if((tDepthRemainingForReparsePoints == 0) &&
												(tWin32FindData.dwFileAttributes & 
												FILE_ATTRIBUTE_REPARSE_POINT))
										{
											*pExistance = 
													CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
										}

										tIsFoundAtCurrentLevel = true;
									}
									else
									{
										/* CHOOSING TO CONTINUE LOOKING FOR DIRECTORY WITH SAME NAME */
										*pExistance = 
												CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
									}
								}
								else
								{
									/* CHOOSING TO CONTINUE LOOKING FOR DIRECTORY WITH SAME NAME */
									*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
								}
							}
						}

						if(tIsNoSystemApiError)
						{
							if(tIsFoundAtCurrentLevel)
							{
								HANDLE tHandle2 /*= ?*/;
								Crx_C_Os_FileSystem_Internal_Win32_Win32FindData tWin32FindData2 /*= ?*/;
								bool tIsNoConflict = true;
								
								crx_c_os_fileSystem_internal_win32_win32FindData_construct(
										&tWin32FindData2);

								crx_c_string_appendString(&tRaw_currentFullPath, 
										&tRaw_currentPathSegmentString);

								tHandle2 = crx_c_os_fileSystem_internal_win32_findFirstFile(true, 
										&tRaw_currentFullPath, &tWin32FindData2);
										
								if(tHandle2 != INVALID_HANDLE_VALUE)
								{
									tIsNoConflict = 
											crx_c_os_fileSystem_private_win32_areWin32FindDataLikelyForSameEntry(
											&tWin32FindData, &tWin32FindData2);

									crx_c_os_fileSystem_internal_win32_findClose(tHandle2);
									tHandle2 = INVALID_HANDLE_VALUE;
								}
								else
									{tIsNoSystemApiError = false;}

								if(tIsNoSystemApiError && tIsNoConflict)
								{
									crx_c_string_removeChars(&tRaw_currentFullPath, 
											crx_c_string_getSize(&tRaw_currentFullPath) - 
											crx_c_string_getSize(&tRaw_currentPathSegmentString), 
											crx_c_string_getSize(&tRaw_currentPathSegmentString));

									if(crx_c_os_fileSystem_internal_win32_findNextFile(tHandle, 
											&tWin32FindData2))
									{
										crx_c_os_osString_unsafeConvertToOsString(&tTempString
												&(tWin32FindData.cFileName));
										crx_c_string_appendString(&tRaw_currentFullPath, &tTempString);

										tHandle2 = crx_c_os_fileSystem_internal_win32_findFirstFile(true, 
												&tRaw_currentFullPath, &tWin32FindData2);
										crx_c_os_fileSystem_internal_win32_findClose(tHandle2);
										tHandle2 = INVALID_HANDLE_VALUE;								
									}
									else
									{
										crx_c_os_osString_unsafeConvertToOsString(&tTempString
												&tFirstEntryAtCurrentLevel);
										crx_c_string_appendString(&tRaw_currentFullPath, &tTempString);
										
										tHandle2 = crx_c_os_fileSystem_internal_win32_findFirstFile(true, 
												&tRaw_currentFullPath, &tWin32FindData2);
										crx_c_os_fileSystem_internal_win32_findClose(tHandle2);
										tHandle2 = INVALID_HANDLE_VALUE;
									}

									crx_c_string_removeChars(&tRaw_currentFullPath, 
											crx_c_string_getSize(&tRaw_currentFullPath) - 
											crx_c_string_getSize(&tTempString), 
											crx_c_string_getSize(&tTempString));
									crx_c_string_empty(&tTempString);
									
									crx_c_string_appendString(&tRaw_currentFullPath,
											&tRaw_currentPathSegmentString);

									tHandle2 = crx_c_os_fileSystem_internal_win32_findFirstFile(true, 
											&tRaw_currentFullPath, &tWin32FindData2);
											
									if(tHandle2 != INVALID_HANDLE_VALUE)
									{
										tIsNoConflict = 
												crx_c_os_fileSystem_private_win32_areWin32FindDataLikelyForSameEntry(
												&tWin32FindData, &tWin32FindData2);

										crx_c_os_fileSystem_internal_win32_findClose(tHandle2);
										tHandle2 = INVALID_HANDLE_VALUE;
									}
									else
										{tIsNoSystemApiError = false;}	
								}
								
								if(tIsLast && tIsNoSystemApiError && tIsNoConflict)
								{
									if(pOsFileHandle != NULL)
									{
										tIsNoConflict = 
												crx_c_os_fileSystem_private_win32_areWin32FindDataAndByHandleFileInformatinLikelyForSameEntry(
												&tWin32FindData, &vByHandleFileInformation)
									}
								}
								
								if(tIsNoSystemApiError)
								{
									if(!tIsNoConflict)
									{
										*pExistance = 
												CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
									}
									else
									{
										if(!tIsLast)
										{
											crx_c_string_appendString(&tRaw_currentFullPath, 
													&tRaw_seperator);
											crx_c_string_appendString(&tRaw_currentFullPath, 
													&tRaw_asterix);

											crx_c_os_fileSystem_internal_win32_findClose(tHandle);
											tHandle = crx_c_os_fileSystem_internal_win32_findFirstFile(
													true, &tRaw_currentFullPath, &tWin32FindData);

											crx_c_string_removeChars(&tRaw_currentFullPath, 
													crx_c_string_getSize(&tRaw_currentFullPath) - 
															crx_c_string_getSize(&tRaw_asterix), 
													crx_c_string_getSize(&tRaw_asterix));

											if(tHandle != INVALID_HANDLE_VALUE)
											{
												if(tDepthRemainingForReparsePoints > 0)
												{
													tDepthRemainingForReparsePoints = 
															tDepthRemainingForReparsePoints - 1;
												}

												tCurrentIndex = tCurrentEndIndex + 1;
												tCurrentEndIndex = 
														crx_c_string_getIndexOfFirstOccuranceOf3(
														pVerified1Utf8Route, tCurrentIndex, "/", 1, 0);

												if(tCurrentEndIndex == ((size_t)(-1)))
												{
													tCurrentEndIndex = tLength;

													tIsLast = true;
												}
												else if(tIsADirectory && 
														(tCurrentEndIndex == (tLength - 1)))
													{tIsLast = true;}

												crx_c_string_empty(&tRaw_currentPathSegmentString);
												crx_c_string_empty(&tCurrentPathSegmentString);
												crx_c_string_appendChars(&tCurrentPathSegmentString, 
														tChars + tCurrentIndex, 
														tCurrentEndIndex - tCurrentIndex);
												crx_c_os_osString_unsafeConvertToOsString(
														&tRaw_currentPathSegmentString, 
														&tCurrentPathSegmentString);

												tIsFoundAtCurrentLevel = false;
												tIsFirstAtCurrentLevel = true;
												crx_c_string_empty(&tFirstEntryAtCurrentLevel);
											}
											else
											{
												/*int32_t tErrorCode = GetLastError();

												//I AM UNSURE WHICH ERROR CODE WOULD BE RETURNED FOR
												//	A DIRECTORY THAT EXISTS BUT IS EMPTY, WHEN USING
												//	FindFirstFileA(). FOR NOW, BOTH THE FOLLOWING ARE 
												//	ASSUMED TO MEAN AN EMPTY DIRECTORY
												if((tErrorCode == ERROR_FILE_NOT_FOUND) ||
														(tErrorCode == ERROR_NO_MORE_FILES))
												{
													*pExistance = 
															CRX__C__OS__FILE_SYSTEM__EXISTANCE__DOES_NOT_EXIST;
												}
												else
													{tIsNoSystemApiError = false;}*/
												
												/*
													REMEMBER IF IT DISAPPEARS AFTER WE FOUND, THE 
														CONVENTION FOR THIS ALGORITHM IS TO TREAT THIS 
														AS A SYSTEM API  ERROR
												*/
												tIsNoSystemApiError = false;
												
												tIsToContinue = false;
											}
										}
										else
										{
											*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__EXISTS;

											tIsToContinue = false;
										}
									}
								}
								else
									{tIsToContinue = false;}

								crx_c_os_fileSystem_internal_win32_win32FindData_destruct(
										&tWin32FindData2);
							}
							else
							{
								if(!crx_c_os_fileSystem_internal_win32_findNextFile(tHandle, 
										&tWin32FindData))
								{
									int32_t tErrorCode = GetLastError();

									crx_c_os_fileSystem_internal_win32_findClose(tHandle);
									tHandle = INVALID_HANDLE_VALUE;

									if(tErrorCode == ERROR_NO_MORE_FILES)
									{
										if(*pExistance == CRX__C__OS__FILE_SYSTEM__EXISTANCE__NULL)
										{
											HANDLE tHandle2 = INVALID_HANDLE_VALUE;
											Crx_C_Os_FileSystem_Internal_Win32_Win32FindData 
													tWin32FindData2 /*= ?*/;
								
											crx_c_os_fileSystem_internal_win32_win32FindData_construct(
													&tWin32FindData2);

											crx_c_string_appendString(&tRaw_currentFullPath, 
													&tRaw_currentPathSegmentString);
											tHandle2 = crx_c_os_fileSystem_internal_win32_findFirstFile(
													true, &tRaw_currentFullPath, &tWin32FindData2);

											if((tHandle2 == INVALID_HANDLE_VALUE) &&
													(GetLastError() == ERROR_NO_MORE_FILES) &&
													((pOsFileHandle == NULL) || (*pOsFileHandle == -1)))
											{												
												*pExistance = 
														CRX__C__OS__FILE_SYSTEM__EXISTANCE__DOES_NOT_EXIST;
											}
											else
											{
												*pExistance = 
														CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;

												crx_c_os_fileSystem_internal_win32_findClose(tHandle2);
												tHandle2 = INVALID_HANDLE_VALUE;
											}

											crx_c_os_fileSystem_internal_win32_win32FindData_destruct(
													&tWin32FindData2);
										}
									}
									else
										{tIsNoSystemApiError = false;}

									tIsToContinue = false;
								}
							}
						}
						else
							{tIsToContinue = false;}

						if(!tIsToContinue)
						{
							crx_c_os_fileSystem_internal_win32_findClose(tHandle);
							tHandle = INVALID_HANDLE_VALUE;
						}
					}

					crx_c_os_fileSystem_internal_win32_findClose(tHandle);
					tHandle = INVALID_HANDLE_VALUE;
					
					crx_c_string_destruct(&tFirstEntryAtCurrentLevel);
				}
				
				if(tIsNoSystemApiError && (
						(*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__EXISTS) ||
						(*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__DOES_NOT_EXIST)))
					{vReturn = true;}

				crx_c_string_destruct(&tRaw_asterix);
			}
			else if((pContract->gResolutionModel == 
							CRX__C__OS__FILE_SYSTEM__RESOLUTION_MODEL__PRECISE_AND_SLOT_AWARE) ||
					(pContract->gResolutionModel == 
							CRX__C__OS__FILE_SYSTEM__RESOLUTION_MODEL__SLOT_AWARE))
			{
				HANDLE tHandle = INVALID_HANDLE_VALUE;
				Crx_C_String tRaw_asterix /*= ?*/;//IN OS ENCODING, NOT UTF8

				crx_c_string_construct(&tRaw_asterix);

				crx_c_string_appendCString(&tTempString, "*");
				crx_c_os_osString_unsafeConvertToOsString(&tRaw_asterix, &tTempString);
				crx_c_string_empty(&tTempString);

				if(pOsFileHandle != NULL)
				{
					crx_c_os_fileSystem_internal_win32_getFileInformationByHandle(pOsFileHandle, 
							&vByHandleFileInformation);
				}

				crx_c_string_appendString(&tRaw_currentFullPath, &tRaw_asterix);

				tHandle = crx_c_os_fileSystem_internal_win32_findFirstFile(true, &tRaw_currentFullPath, 
						&tWin32FindData);
						
				crx_c_string_removeChars(&tRaw_currentFullPath, 
						crx_c_string_getSize(&tRaw_currentFullPath) - 
								crx_c_string_getSize(&tRaw_asterix), 
						crx_c_string_getSize(&tRaw_asterix));
						
				if(tHandle == INVALID_HANDLE_VALUE)
				{
					int32_t tErrorCode = GetLastError();

					if((tErrorCode == ERROR_FILE_NOT_FOUND) ||
							(tErrorCode == ERROR_NO_MORE_FILES)) //UNSURE WHAT I WOULD GET WHEN USING *
					{
						Crx_C_String tString /*= ?*/;

						crx_c_string_construct(&tString);
						
						crx_c_string_appendString(&tString, pVerified1Utf8PathPrefix);
						crx_c_string_appendString(&tString, pVerified1Utf8Route);
						
						tHandle = crx_c_os_fileSystem_internal_win32_findFirstFile(true, 
								&tString, &tWin32FindData);

						if(tHandle == INVALID_HANDLE_VALUE)
						{
							if(tErrorCode == ENOENT)
							{
								tIsNoSystemApiError = true;

								if((pOsFileHandle == NULL) || (*pOsFileHandle == -1))
									{*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__DOES_NOT_EXIST;}
								else
									{*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;}
							}
							else
								{tIsNoSystemApiError = false;}
						}
						else
						{
							*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;

							crx_c_os_fileSystem_internal_win32_findClose(tHandle);
							tHandle = INVALID_HANDLE_VALUE;
						}

						crx_c_string_destruct(&tString);
					}
					else
						{tIsNoSystemApiError = false;}
				}
				else
					{tIsNoSystemApiError = false;}//REMEMBER, THIS IS CONCERNING THE CRX PATH PREFIX

				if(tIsNoSystemApiError && (tHandle != INVALID_HANDLE_VALUE))
				{
					Crx_C_String tCurrentPathSegmentString /*= ?*/;
					Crx_C_String tCurrentEntityNameFromOs /*= ?*/;
					Crx_C_String tTempString /*= ?*/;
					Crx_C_String tVerifiedUtf8Route /*= ?*/;
					uint32_t tNumberOfFoundAtCurrentLevel = 0;
					bool tIsPreciselyFoundAtCurrentLevel = false;
					bool tIsToContinue = true;
					
					crx_c_string_construct(&tCurrentPathSegmentString);
					crx_c_string_construct(&tCurrentEntityNameFromOs);
					crx_c_string_construct(&tTempString);
					crx_c_string_construct(&tVerifiedUtf8Route);

					while(tIsToContinue)
					{
						bool tIsNotToSkipCurrent = ((!crx_c_string_isEqualTo2(
								&(tWin32FindData.cFileName), '.')  && !crx_c_string_isEqualTo2(
								&(tWin32FindData.cFileName), '..') == 0));
						bool tIsAMatch = false;

						crx_c_os_osString_fileSystem_normalizeToUnicodeNfd(
								&tCurrentEntityNameFromOs, &(tWin32FindData.cFileName), true);

						if(tIsNotToSkipCurrent && (crx_c_os_osString_fileSystem_compareStrings(
								&tCurrentPathSegmentString, &tCurrentEntityNameFromOs, true) == 0))
						{
							tNumberOfFoundAtCurrentLevel = tNumberOfFoundAtCurrentLevel + 1;

							if(tNumberOfFoundAtCurrentLevel > 1)
							{
								if(pContract->gResolutionModel == 
										CRX__C__OS__FILE_SYSTEM__RESOLUTION_MODEL__PRECISE_AND_SLOT_AWARE)
								{
									*pExistance = 
											CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
								}
								else
								{
									*pExistance = 
											CRX__C__OS__FILE_SYSTEM__EXISTANCE__AMBIGUOUS;
								}
								
								tIsToContinue = false;
								tIsNotToSkipCurrent = false;
							}
							else
							{
								if(pContract->gResolutionModel == 
										CRX__C__OS__FILE_SYSTEM__RESOLUTION_MODEL__PRECISE_AND_SLOT_AWARE)
								{
									if(crx_c_string_isEqualTo(&tCurrentPathSegmentString, 
											&(tWin32FindData.cFileName), false))
										{tIsPreciselyFoundAtCurrentLevel = true;}
									else
									{
										*pExistance = 
												CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
										tIsToContinue = false;
										tIsNotToSkipCurrent = false;
									}
								}
							}
							
							tIsAMatch = true;
							
							if(tIsToContinue)
							{
								if(crx_c_string_getLength(&tVerifiedUtf8Route) > 0)
									{crx_c_string_appendCString(&tVerifiedUtf8Route, "\\");}

								crx_c_string_appendCString(&tVerifiedUtf8Route, 
										&(tWin32FindData.cFileName));
								crx_c_os_osString_unsafeConvertToOsString(
										&tRaw_currentPathSegmentString, 
										&(tWin32FindData.cFileName));
							}
						}
						
						/*FindFirstFileW/FindFirstFileA SEARCHES BOTH SHORT AND LONG FILE NAMES.
							cFileName SHOULD CONTAIN THE LONG FILE NAME ONLY, AND PER THE DESIGN,
							WE DO NOT DEAL WITH SHORT NAMES.
						*/
						if(tIsNotToSkipCurrent && tIsAMatch)
						{
							//QUOTE: "Junction Points and Symbolic Links are both Reparse Points"
							/*
								ON WINDOWS THE ASSUMPTION IS THAT IF THE ENTRY IS A REPARSE POINT,
								THE dwFileAttributes MEMBER ALSO TELLS US WHETHER IT POINTS TO
								A DIRECTORY OF FILE. I DO NOT THINK THAT THIS ACTUALLY TRUE, HOWEVER.
								TO RELIABLY KNOW WHETHER THE REPARSE POINT EVALUATES TO A DIRRECTORY
								OR FILE, IT SEEMS THAT ONE MUST USE DeviceIoControl(). THE FUNCTOIN
								EXISTS IN WIN95 AND UP, BUT THE OPTION "FSCTL_GET_REPARSE_POINT" DOES
								NOT EXIST IN WIN9X.
								
								A REPARSE POINT IS NOT NECCESSARILY A 
								"JUMP" POINT. IT IS SIMPLY A FILE THAT CONTAINS EXTRA INFORMATION
								THAT A DRIVER REGISTERED FOR THE TYPE OF INFORMATION WOULD BE
								CALLED FOR. HOWEVER, "JUMP" POINTS ARE IMPLEMENTED USING REPARSE
								POINT. "JUMP" POINTS LIKE JUNCTIONS POINTS AND SYMBOLIC LINKS.
								NOTE THAT CURRENTLY, ALL REPARSE POINTS AER TREATED AS "JUMP" POINTS.
							*/
							if(tWin32FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
							{
								if(tIsLast)
								{
									if(tIsADirectory)
									{
										if((tDepthRemainingForReparsePoints == 0) &&
												(tWin32FindData.dwFileAttributes & 
												FILE_ATTRIBUTE_REPARSE_POINT))
										{
											*pExistance = 
													CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
										}
									}
									else
									{
										*pExistance = 
												CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
									}
								}
								else
								{
									if((tDepthRemainingForReparsePoints == 0) &&
											(tWin32FindData.dwFileAttributes & 
											FILE_ATTRIBUTE_REPARSE_POINT))
									{
										*pExistance = 
												CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
									}
								}
							}
							else
							{
								if(tIsLast)
								{
									if(!tIsADirectory)
									{
										if((tDepthRemainingForReparsePoints == 0) &&
												(tWin32FindData.dwFileAttributes & 
												FILE_ATTRIBUTE_REPARSE_POINT))
										{
											*pExistance = 
													CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
										}
									}
									else
									{
										*pExistance = 
												CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
									}
								}
								else
								{
									*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
								}
							}
							
							if(tIsNoSystemApiError)
							{
								if(*pExistance == CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED)
								{
									tIsToContinue = false;
									tIsNotToSkipCurrent = false;
									
									if(pContract->gResolutionModel == 
											CRX__C__OS__FILE_SYSTEM__RESOLUTION_MODEL__SLOT_AWARE)
									{
										*pExistance = 
												CRX__C__OS__FILE_SYSTEM__EXISTANCE__AMBIGUOUS;
									}
								}
							}
						}

						if(tIsNoSystemApiError && tIsToContinue)
						{
							if(!crx_c_os_fileSystem_internal_win32_findNextFile(tHandle, 
									&tWin32FindData))
							{
								int32_t tErrorCode = GetLastError();

								crx_c_os_fileSystem_internal_win32_findClose(tHandle);
								tHandle = INVALID_HANDLE_VALUE;

								if(tErrorCode == ERROR_NO_MORE_FILES)
								{
									if(tNumberOfFoundAtCurrentLevel == 0)
									{
										*pExistance = 
												CRX__C__OS__FILE_SYSTEM__EXISTANCE__DOES_NOT_EXIST;
										tIsToContinue = false;

										crx_c_string_appendChars(&tVerifiedUtf8Route, 
												crx_c_string_getChars(pVerified1Utf8Route) + 
														tCurrentIndex, 
												crx_c_string_getSize(pVerified1Utf8Route) - 
														tCurrentIndex);

										/*if(tCurrentEndIndex != tLength)
											{crx_c_string_appendCString(&tVerifiedUtf8Route, "\\");}*/

										if(pExclusiveEndIndexOfExistingRoute != NULL)
											{*pExclusiveEndIndexOfExistingRoute = tCurrentIndex;}
									}
									else
									{
										assert(tNumberOfFoundAtCurrentLevel == 1);

										if(!tIsLast)
										{
											crx_c_string_appendString(&tRaw_currentFullPath, 
													&tRaw_currentPathSegmentString);
											crx_c_string_appendString(&tRaw_currentFullPath, 
													&tRaw_seperator);
											crx_c_string_appendString(&tRaw_currentFullPath, 
													&tRaw_asterix);

											crx_c_os_fileSystem_internal_win32_findClose(tHandle);
											tHandle = crx_c_os_fileSystem_internal_win32_findFirstFile(
													true, &tRaw_currentFullPath, &tWin32FindData);

											crx_c_string_removeChars(&tRaw_currentFullPath, 
													crx_c_string_getSize(&tRaw_currentFullPath) - 
															crx_c_string_getSize(&tRaw_asterix), 
													crx_c_string_getSize(&tRaw_asterix));

											if(tHandle != INVALID_HANDLE_VALUE)
											{
												if(tDepthRemainingForReparsePoints > 0)
												{
													tDepthRemainingForReparsePoints = 
															tDepthRemainingForReparsePoints - 1;
												}

												tCurrentIndex = tCurrentEndIndex + 1;
												tCurrentEndIndex = 
														crx_c_string_getIndexOfFirstOccuranceOf3(
														pVerified1Utf8Route, tCurrentIndex, "/", 1, 0);

												if(tCurrentEndIndex == ((size_t)(-1)))
												{
													tCurrentEndIndex = tLength;

													tIsLast = true;
												}
												else if(tIsADirectory && 
														(tCurrentEndIndex == (tLength - 1)))
													{tIsLast = true;}

												crx_c_string_empty(&tRaw_currentPathSegmentString);
												crx_c_string_empty(&tCurrentPathSegmentString);
												crx_c_string_appendChars(&tCurrentPathSegmentString, 
														tChars + tCurrentIndex, 
														tCurrentEndIndex - tCurrentIndex);

												tNumberOfFoundAtCurrentLevel = 0;
												tIsPreciselyFoundAtCurrentLevel = false;
											}
											else
											{
												int32_t tErrorCode = GetLastError();

												//I AM UNSURE WHICH ERROR CODE WOULD BE RETURNED FOR
												//	A DIRECTORY THAT EXISTS BUT IS EMPTY, WHEN USING
												//	FindFirstFileA(). FOR NOW, BOTH THE FOLLOWING ARE 
												//	ASSUMED TO MEAN AN EMPTY DIRECTORY
												if((tErrorCode == ERROR_FILE_NOT_FOUND) ||
														(tErrorCode == ERROR_NO_MORE_FILES))
												{
													*pExistance = 
															CRX__C__OS__FILE_SYSTEM__EXISTANCE__DOES_NOT_EXIST;
												}
												else
													{tIsNoSystemApiError = false;}
												
												tIsToContinue = false;
											}
										}
										else
										{
											*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__EXISTS;

											tIsToContinue = false;
										}
									}
								}
								else
									{tIsNoSystemApiError = false;}

								tIsToContinue = false;
							}
						}
						else
							{tIsToContinue = false;}

						if(!tIsToContinue)
						{
							crx_c_os_fileSystem_internal_win32_findClose(tHandle);
							tHandle = INVALID_HANDLE_VALUE;
						}
					}

					crx_c_os_fileSystem_internal_win32_findClose(tHandle);
					tHandle = INVALID_HANDLE_VALUE;
					
					if(tIsNoSystemApiError && (
							(*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__EXISTS) ||
							(*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__DOES_NOT_EXIST)))
					{
						if(crx_c_string_trySwapContentWith(&tVerifiedUtf8Route, pVerified1Utf8Route))
							{vReturn = true;}
						else
						{
							if(crx_c_string_copyAssignFrom(pVerified1Utf8Route, 
									&tVerifiedUtf8Route))
								{vReturn = true;}
							else
								{*pExistance = CRX__C__OS__FILE_SYSTEM__ERROR__NULL;}
						}
					}
					
					crx_c_string_destruct(&tCurrentPathSegmentString);
					crx_c_string_destruct(&tCurrentEntityNameFromOs);
					crx_c_string_destruct(&tTempString);
					crx_c_string_destruct(&tVerifiedUtf8Route);
				}

				crx_c_string_destruct(&tRaw_asterix);
			}
		}
		else
		{
			crx_c_string_appendChars(&tRaw_currentFullPath, &tRaw_currentPathSegmentString);
			
			tHandle = crx_c_os_fileSystem_internal_win32_findFirstFile(true, &tRaw_currentFullPath, 
				&tWin32FindData);

			if(tHandle == INVALID_HANDLE_VALUE)
			{
				if(GetLastError() == ERROR_FILE_NOT_FOUND)
					{*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__DOES_NOT_EXIST;}
				else
					{tIsNoSystemApiError = false;}
			}

			while(tHandle != INVALID_HANDLE_VALUE)
			{
				crx_c_os_fileSystem_internal_win32_findClose(tHandle);

				//QUOTE: "Junction Points and Symbolic Links are both Reparse Points"
				if((tDepthRemainingForReparsePoints > 0) || 
						!(tWin32FindData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT))
				{
					if(tWin32FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						if(tIsLast)
						{
							tHandle = INVALID_HANDLE_VALUE;

							if(tIsADirectory)
							{
								*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__EXISTS;
								vReturn = true;
							}
							else
							{
								*pExistance = 
										CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
							}
						}
						else
						{
							if(tDepthRemainingForReparsePoints > 0)
							{
								tDepthRemainingForReparsePoints = 
										tDepthRemainingForReparsePoints - 1;
							}

							tCurrentIndex = tCurrentEndIndex + 1;
							tCurrentEndIndex = crx_c_string_getIndexOfFirstOccuranceOf3(
									pVerified1Utf8Route, tCurrentIndex, "/", 1, 0);

							if(tCurrentEndIndex == ((size_t)(-1)))
							{
								tCurrentEndIndex = tLength;

								tIsLast = true;
							}
							else if(tIsADirectory && (tCurrentEndIndex == (tLength - 1)))
								{tIsLast = true;}

							crx_c_string_empty(&tRaw_currentPathSegmentString);
							crx_c_string_empty(&tCurrentPathSegmentString);
							crx_c_string_appendChars(&tCurrentPathSegmentString, 
									tChars + tCurrentIndex, tCurrentEndIndex - tCurrentIndex);
							crx_c_os_osString_unsafeConvertToOsString(
									&tRaw_currentPathSegmentString, &tCurrentPathSegmentString);

							crx_c_string_appendString(&tRaw_currentFullPath, &tRaw_seperator);
							crx_c_string_appendString(&tRaw_currentFullPath, 
									&tRaw_currentPathSegmentString);
						}
					}
					else
					{
						tHandle = INVALID_HANDLE_VALUE;

						if(tIsLast)
						{
							if(!tIsADirectory)
							{
								*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__EXISTS;
								vReturn = true;
							}
							else
							{
								*pExistance = 
										CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
							}
						}
						else
							{*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;}
					}
					
					if(tHandle != INVALID_HANDLE_VALUE)
					{
						tHandle = crx_c_os_fileSystem_internal_win32_findFirstFile(true, 
								&tRaw_currentFullPath, &tWin32FindData);

						if(tHandle == INVALID_HANDLE_VALUE)
						{
							if(GetLastError() == ERROR_FILE_NOT_FOUND)
							{
								*pExistance = 
										CRX__C__OS__FILE_SYSTEM__EXISTANCE__DOES_NOT_EXIST;
							}
							else
								{tIsNoSystemApiError = false;}
						}
					}
				}
				else
				{
					*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;
					tHandle = INVALID_HANDLE_VALUE;
				}
			}
		}
		
		if(tIsNoSystemApiError)
		{
			if((*pExistance == CRX__C__OS__FILE_SYSTEM__EXISTANCE__NULL) ||
					(*pExistance == CRX__C__OS__FILE_SYSTEM__EXISTANCE__DOES_NOT_EXIST))
			{
				if(pOsFileHandle != NULL)
				{
					if(*pOsFileHandle != CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE)
						{*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;}
					else
					{
						*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__DOES_NOT_EXIST;
						vReturn = true;
					}
				}
				else
				{
					uint32_t /*DWORD*/ tFileAttributes = INVALID_FILE_ATTRIBUTES;

					crx_c_os_osString_unsafeConvertToOsString(&tRaw_currentFullPath, 
								pVerified1Utf8PathPrefix);
					crx_c_os_osString_unsafeConvertToOsString(&tCurrentPathSegmentString, 
							pVerified1Utf8Route);

					crx_c_string_appendString(&tRaw_currentFullPath, &tCurrentPathSegmentString);
					
					/*
						QUOTE: When GetFileAttributes is called on a directory that is a 
								mounted folder, it returns the file system attributes of the 
								directory, not those of the root directory in the volume that the 
								mounted folder associates with the directory. To obtain the file 
								attributes of the associated volume, call 
								GetVolumeNameForVolumeMountPoint to obtain the name of the 
								associated volume. Then use the resulting name in a call to 
								GetFileAttributes. The results are the attributes of the root 
								directory on the associated volume.
								{SRC: https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfileattributesa?redirectedfrom=MSDN}
						THIS SHOULD BE NOT AN ISSUE HERE. IF IT ENDS UP BEING AN ISSUE, SWITCH TO 
						USING CreateFile() INSTEAD.
					*/
					tFileAttributes = 
							crx_c_os_fileSystem_private_win32_getFileAttributes(true, 
									&tRaw_currentFullPath);

					if(tFileAttributes == INVALID_FILE_ATTRIBUTES)
					{
						*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__DOES_NOT_EXIST;
						vReturn = true;
					}
					else
						{*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED;}
				}
			}
		}
		else
			{*pExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__NULL;}
#endif

#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
		crx_c_os_fileSystem_internal_posix_fileHandle_destruct(&tFileHandle);
		crx_c_string_destruct(&tRemainingPath);
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
		crx_c_os_fileSystem_internal_win32_win32FindData_destruct(&tWin32FindData);
		crx_c_string_destruct(&tRaw_seperator);
		crx_c_string_destruct(&tRaw_currentFullPath);
		crx_c_string_destruct(&tRaw_currentPathSegmentString);
		crx_c_string_destruct(&tCurrentPathSegmentString);
		crx_c_string_destruct(&tTempString);
#endif
		
		CRX_SCOPE_END
	}

	return vReturn;
}
/*
	WARNING: pValueOfIsToNormalize MUST BE THE VALUE OF pIsToNormalize RETURNED FROM THE 
			CORRSPONDING CALL TO crx_c_os_fileSystem_verifyFileSystemPath()
	WARNING: pLengthResidueWhenDirectory AND pLengthResidue MUST THOSE RETURNED FROM THE
			CORRSPONDING CALL TO crx_c_os_fileSystem_verifyFileSystemPath()
	WARNING: pVerifiedFullPath MUST BE THE VERIFIED FULL PATH CONSTRUCTED FROM THE 
			CORRSPONDING CALL TO crx_c_os_fileSystem_verifyFileSystemPath(), OR A SUB FULL PATH
			OF IT.
*/
bool crx_c_os_fileSystem_internal_updateLengthResidues(
		Crx_C_String const * CRX_NOT_NULL pVerifiedFullPath, 
		bool pValueOfIsToNormalize, size_t * CRX_NOT_NULL pLengthResidueWhenDirectory, 
		size_t * CRX_NOT_NULL pLengthResidue)
{
	size_t tUcs2Length  = crx_c_os_fileSystem_internal_getUtf8Ucs2Length(pVerifiedFullPath) - 
			((!pValueOfIsToNormalize && crx_c_string_isBeginningWith2(pVerifiedFullPath, "\\\\?\\", 
			false)) ? 4 : 0);
	size_t vLengthResidueWhenDirectory = 0;
	size_t vLengthResidue = 0;
	bool vIsADirectory = crx_c_string_isEndingWith2(pVerifiedFullPath, 
			CRX__OS__FILE_SYSTEM__SEPERATOR_STRING);
	bool vIsNoError = true;

	if(CRX__OS__FILE_SYSTEM__MAXIMUM_DIRECTORY_FULL_PATH_WITH_PREFIX_UCS2_LENGTH <= 
			vLengthResidueWhenDirectory)
	{
		vLengthResidueWhenDirectory = (
				CRX__OS__FILE_SYSTEM__MAXIMUM_DIRECTORY_FULL_PATH_WITH_PREFIX_UCS2_LENGTH - 
				vLengthResidueWhenDirectory);
	}
	else
		{vLengthResidueWhenDirectory = 0;}

	if(CRX__OS__FILE_SYSTEM__MAXIMUM_FULL_PATH_WITH_PREFIX_UCS2_LENGTH <= tUcs2Length)
	{
		vLengthResidue = (
				CRX__OS__FILE_SYSTEM__MAXIMUM_FULL_PATH_WITH_PREFIX_UCS2_LENGTH - tUcs2Length);
	}
	else
		{vLengthResidue = 0;}

	if((vLengthResidue >= *pLengthResidue) &&
			(vLengthResidueWhenDirectory >= *pLengthResidueWhenDirectory))
	{
		*pLengthResidueWhenDirectory = vLengthResidueWhenDirectory;
		*pLengthResidue = vLengthResidue;
	}
}



//PER THE CRX SECURITY PROJECT.
//WARNING: IN TERMS OF CRX URL (SEE THE STANDARD), THIS NORMALIZES A CRX PATH, OR A CRX ROUTE.
//		THIS DOES NOT NORMALIZE A CRX URL, NOR A CRX EXTENDED PATH.
//REMEMBER: BEING PER THE CRX SECURITY PROJECT, THIS TREATS '/' AND '\' AS BEING THE SAME. ON UNIX
//		LIKE SYSTEMS THIS CAN DAMAGE THE PATH BECAUSE '\' CAN BE PART OF THE DIRECTORY OR FILENAME
//		WHILE THE FOLLOWING REMOVES THEM.
void crx_c_os_fileSystem_normalizePath(Crx_C_String * CRX_NOT_NULL pReturn, 
		Crx_C_String const * pPath, bool pIsToBarInPath CRX_DEFAULT(false))
{
	Crx_C_String vPath /*= ?*/;

	crx_c_string_copyConstruct(&vPath, pPath);

	crx_c_os_osString_trim(&vPath);
	crx_c_string_empty(pReturn);

	if(crx_c_string_getSize(&vPath) > 0)
	{
		Crx_C_Arrays_Size vIndices /*= ?*/;
		Crx_C_Arrays_String vNormalizedPathTokens/*= ?*/;
		
		crx_c_arrays_size_construct(&vIndices, 0);
		crx_c_arrays_string_construct(&vNormalizedPathTokens, 0);
		
		crx_c_string_replaceOccurancesOfChar(&vPath, 0, 92 /* \ */, 47 /* / */, false);
		crx_c_string_computeIndicesOfAllOccurancesOf3(&vPath, vIndices, 0, 47 /* / */, 1, false, 0);

		CRX_FOR(size_t tI = 0, tI <= crx_c_arrays_getLength(vIndices), tI++)
		{
			crx_c_string tPathToken /*= ?*/;
			
			crx_c_string_construct(&tPathToken);

			//THE FOLLOWING MUST BE PER PHP'S explode()
			if((tI == 0) && ((crx_c_arrays_getLength(vIndices) == 0) ||
					(crx_c_arrays_size_copyGet(&vIndices, 0) == 0)))
			{
				crx_c_string_insertElementsAt(&tPathToken, 0 &vPath, 0, 
						crx_c_string_getSize(&vPath));
			}
			else
			{
				if(tI == 0)
				{
					crx_c_string_insertElementsAt(&tPathToken, 0, &vPath, 0,
							crx_c_arrays_size_copyGet(&vIndices, 0));
				}
				else
				{
					if(tI < crx_c_arrays_getLength(vIndices))
					{
						if(crx_c_arrays_size_copyGet(&vIndices, tI) >
								(crx_c_arrays_size_copyGet(&vIndices, tI - 1) + 1))
						{
							size_t tStartIndex = crx_c_arrays_size_copyGet(&vIndices, tI - 1) + 1;

							crx_c_string_insertElementsAt(&tPathToken, 0, &vPath, tStartIndex,
									crx_c_arrays_size_copyGet(&vIndices, tI) - tStartIndex - 1);
						}
					}
					else 
					{
						if((tI == crx_c_arrays_getLength(vIndices)) &&
								(crx_c_arrays_size_copyGet(&vIndices, tI - 1) < 
										(crx_c_string_getSize(&vPath) - 1)))
						{
							crx_c_string_insertElementsAt(&tPathToken, 0, &vPath, 
									crx_c_arrays_size_copyGet(&vIndices, tI - 1) + 1,
									crx_c_string_getSize(&vPath) - 
									crx_c_arrays_size_copyGet(&vIndices, tI - 1) - 1);
						}
					}
				}
			}
			
			crx_c_os_osString_trim(&tPathToken);

			if(crx_c_string_getSize(&tPathToken) == 0)
			{
				if(tI == 0)
					{crx_c_arrays_string_push(&vNormalizedPathTokens, &tPathToken);}
			}
			else if(crx_c_string_isEqualTo2(&tPathToken, ".", false))
				{}
			else if(crx_c_string_isEqualTo2(&tPathToken, "..", false))
			{
				if(crx_c_arrays_string_getLength(&vNormalizedPathTokens) === 0)
				{
					if(pIsToBarInPath)
					{
						crx_c_string_empty(pReturn);

						return;
					}
					else
						{crx_c_arrays_string_push(&vNormalizedPathTokens, &tPathToken /*= .. */);}
				}
				else if(crx_c_string_isEqualTo2(crx_c_arrays_string_get(&vNormalizedPathTokens
						crx_c_arrays_string_getLength(&vNormalizedPathTokens) - 1), "..", false))
				{
					Crx_C_String tString /*= ?*/;

					crx_c_string_construct(&tString);

					crx_c_string_appendCString(&tString, "..");

					crx_c_arrays_string_insertElementAt(&vNormalizedPathTokens, 0, &tString);

					crx_c_string_destruct(&tString);
				}
				else
					{crx_c_arrays_string_pop(&vNormalizedPathTokens);}
			}
			else
				{crx_c_arrays_string_push(&vNormalizedPathTokens, &tPathToken);}
			
			//echo "II:<pre>"; print_r($vNormalizedPathTokens); echo "</pre><br/>";
		}
		CRX_ENDFOR

		if((crx_c_arrays_string_getLength(&vNormalizedPathTokens) == 1) && 
				crx_c_string_getSize(crx_c_arrays_string_get(&vNormalizedPathTokens, 0)) == 0)
		{
			crx_c_string_empty(pReturn);
			crx_c_string_appendCString(pReturn, "/");
		}
		else if((crx_c_arrays_string_getLength(&vNormalizedPathTokens) == 0))
			{crx_c_string_empty(pReturn);}
		else
		{
			bool tIsNotFirst = false;

			CRX_FOR(size_t tI = 0, tI < crx_c_arrays_string_getLength(&vNormalizedPathTokens),
					tI++)
			{
				if(tIsNotFirst)
					{crx_c_string_push(pReturn, 47 /* / */);}
				else
					{tIsNotFirst = false;}

				crx_c_string_appendString(pReturn, 
						crx_c_arrays_string_get(&vNormalizedPathTokens, tI));
			}
			CRX_ENDFOR

			crx_c_string_appendCString(pReturn, 
					(crx_c_string_isEndingWith2(&vPath, "/", false) ? "/" : ""));
		}

		if(crx_c_string_isEqualTo2(pReturn, "//", false))
		{
			crx_c_string_empty(pReturn);
			crx_c_string_appendCString(pReturn, "/");
		}
		
		crx_c_arrays_size_destruct(&vIndices);
		crx_c_arrays_string_destruct(&vNormalizedPathTokens);
	}

	crx_c_string_destruct(&vPath);
}
void crx_c_os_fileSystem_normalizePathForOs(Crx_C_String * CRX_NOT_NULL pReturn, 
		Crx_C_String const * pPath, bool pIsToBarInPath CRX_DEFAULT(false))
{
	crx_c_os_fileSystem_normalizePath(pReturn, pPath, pIsToBarInPath);
	crx_c_string_replaceOccurancesOfChar(&pReturn, 0, 
			CRX__OS__FILE_SYSTEM__PRIVATE__NOT_SEPERATOR_CHAR, 
			CRX__OS__FILE_SYSTEM__SEPERATOR_CHAR, false);
}

bool crx_c_os_fileSystem_enforcePathAsDirectoryPath(Crx_C_String * CRX_NOT_NULL pPath, 
		bool pIsToBeOsDependent)
{
	crx_c_os_osString_trim(pPath);

	if(pIsToBeOsDependent)
	{
		if(*(crx_c_string_get(pPath, crx_c_string_getSize(pPath) - 1)) != 
				CRX__OS__FILE_SYSTEM__SEPERATOR_CHAR)
			{return crx_c_string_appendChar(vPath, CRX__OS__FILE_SYSTEM__SEPERATOR_CHAR);}
		else
			{return true;}
	}
	else
	{
		char * tChar = crx_c_string_get(pPath, crx_c_string_getSize(pPath) - 1);

		if((*tChar != 47 /* / */) && (*tChar != 92 /* \ */))
			{return crx_c_string_appendChar(vPath, 47 /* / */);}
		else
			{return true;}
	}
}

//WARNING: pVerifiedPath MUST BE VERIFIED. THERE IS NO WAY TO KNOW IF A SLASH IS PART OF A NAME OR
//		IS A PATH SEPERATOR. THIS IS WHY THIS OPERATION CAN NEVER BE PUBLIC, AND WHY THE PATH 
//		MUST BE VERIFIED FIRST. YOU COULD IMPLEMENT A PUBLIC VERSION OF THIS FUNCTION, BUT THE
//		PATH WOULD ALWAYS HAVE TO BE FULL.
//bool crx_c_os_fileSystem_internal_getEntityName(Crx_C_String * CRX_NOT_NULL pVerifiedrOUTE)

/*
	NOTES: 
		- IF YOU PROVIDE A "RAW" VERSION OF THIS FOR THE EARLY SETUP OF THE PROGRAM WHEN THERE IS
				NO HEAP AND OTHER FACILITIES, YOU DO NOT NEED TO CONSIDER VERY LONG FILE PATH. LONG 
				FILE PATHS ARE SUPPORTED BY THE "at" POSIX FUNCTIONS, SUCH AS openat, AND THERE IS
				NOT SUCH THING AS dlopenat. REMEMBER, THE DLL PART OF CrxRt, IS LIKELY THE ONLY 
				CLIENT FOR THE "RAW" OPERATIONS OF FUNCTIONS. 
*/
Crx_C_Os_FileSystem_OsFileHandle crx_c_os_fileSystem_open(
		Crx_C_String const * CRX_NOT_NULL pFileFullPath,
		uint32_t pDepthLimitOfReparsePoints,
		Crx_C_Os_FileSystem_FileOpenMode pFileOpenMode,
		Crx_C_Os_FileSystem_FileAccessMode pFileAccessMode,
		Crx_C_Os_FileSystem_FileShareMode pFileShareMode,
		Crx_C_Os_FileSystem_Contract const * CRX_NOT_NULL pContract)
{
	return crx_c_os_fileSystem_internal_doOpen(pFileFullPath, pDepthLimitOfReparsePoints, NULL,
			pFileOpenMode, pFileAccessMode, pFileShareMode, pContract, false, NULL);
}
Crx_C_Os_FileSystem_OsFileHandle crx_c_os_fileSystem_open2(
		Crx_C_Os_FileSystem_Iterator_Record * pRecord,
		Crx_C_Os_FileSystem_FileAccessMode pFileAccessMode,
		Crx_C_Os_FileSystem_FileShareMode pFileShareMode)
{
	return crx_c_os_fileSystem_internal_doOpen(
			crx_c_os_fileSystem_iterator_record_getInternalFullPath(pRecord), ((size_t)(-1)), 
			pRecord, CRX__C__OS__FILE_SYSTEM__FILE_OPEN_MODE__OPEN, pFileAccessMode, pFileShareMode, 
			NULL, false, NULL);
}
/*
	pIsToEnableShareDeleteWhenPossible IS MEANT FOR THE LOCK IMPLEMENTATION, AND POSSIBLY OTHER
			INTERNAL USE. DO NOT EXPOSE THIS TO CLIENT CODE.
*/
Crx_C_Os_FileSystem_OsFileHandle crx_c_os_fileSystem_internal_doOpen(
		Crx_C_String const * CRX_NOT_NULL pFileFullPath,
		uint32_t pDepthLimitOfReparsePoints,
		Crx_C_Os_FileSystem_Iterator_Record * pRecord,
		Crx_C_Os_FileSystem_FileOpenMode pFileOpenMode,
		Crx_C_Os_FileSystem_FileAccessMode pFileAccessMode,
		Crx_C_Os_FileSystem_FileShareMode pFileShareMode,
		Crx_C_Os_FileSystem_Contract const * pContract,
		bool pIsToEnableShareDeleteWhenPossible,
		Crx_C_String * pInternalFileFullPath)
{
	CRX_SCOPE_META
	assert((pRecord == NULL) || CRX__ARE_POINTERS_TO_SAME_OBJECT(pFileFullPath,
			crx_c_os_fileSystem_iterator_record_getInternalFullPath(pRecord), true));
	assert((pRecord == NULL) || (pContract != NULL));

	CRX_SCOPE
	Crx_C_Os_FileSystem_Operation vOperation = CRX__C__OS__FILE_SYSTEM__OPERATION__NULL;
	Crx_C_Os_FileSystem_Existance vExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__NULL;
	size_t vExclusiveEndIndexOfExistingRoute = 0;
	Crx_C_String vVerifiedPathPrefix /*= ?*/;
	Crx_C_String vVerifiedRoute /*= ?*/;
	Crx_C_String vFullPath /*= ?*/;
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	Crx_C_String vRemainingPartialPath /*= ?*/;
	Crx_C_Os_FileSystem_Internal_Posix_FileHandle vFileHandle /*= ?*/;
	int32_t vFlags = 0;
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	uint32_t vCreationDisposition = 0;
	uint32_t vDesiredAccess = 0;
	uint32_t vShareMode = 0;
#endif
	size_t vLengthResidueWhenDirectory = 0;
	size_t vLengthResidue = 0;
	bool vIsNoError = true;
	Crx_C_Os_FileSystem_OsFileHandle vReturn = CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE;
	
	crx_c_string_construct(&vVerifiedPathPrefix);
	crx_c_string_construct(&vVerifiedRoute);
	crx_c_string_construct(&vFullPath);
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	crx_c_string_construct(&vRemainingPartialPath);
	crx_c_os_fileSystem_internal_posix_fileHandle_construct(&vFileHandle);
#endif

	if((pFileAccessMode == CRX__C__OS__FILE_SYSTEM__FILE_ACCESS_MODE__NORMAL) ||
			(pFileAccessMode == CRX__C__OS__FILE_SYSTEM__FILE_ACCESS_MODE__WRITE_ONLY))
		{vOperation = CRX__C__OS__FILE_SYSTEM__OPERATION__WRITE;}
	else if(pFileOpenMode == CRX__C__OS__FILE_SYSTEM__FILE_OPEN_MODE__CREATE)
		{vOperation = CRX__C__OS__FILE_SYSTEM__OPERATION__WRITE;}
	else
		{vOperation = CRX__C__OS__FILE_SYSTEM__OPERATION__READ;}

	if(pRecord == NULL)
	{
		if(crx_c_os_fileSystem_verifyFileSystemPath(pContract, vOperation,
				pFileFullPath, pDepthLimitOfReparsePoints, false, 
				&vVerifiedPathPrefix, &vVerifiedRoute,
				NULL, &vLengthResidueWhenDirectory, &vLengthResidue, NULL, NULL))
		{
			vIsNoError = crx_c_os_fileSystem_verifyFileSystemPath2(pContract, vOperation, 
					NULL, pDepthLimitOfReparsePoints, &vExistance, 
					&vVerifiedPathPrefix, &vVerifiedRoute, &vExclusiveEndIndexOfExistingRoute)
		}
		else
			{vIsNoError = false;}

		if(vIsNoError && crx_c_string_isEndingWith2(&vVerifiedRoute, 
				CRX__OS__FILE_SYSTEM__SEPERATOR_STRING))
			{vIsNoError = false;}

		if(vIsNoError && (vExistance == CRX__C__OS__FILE_SYSTEM__EXISTANCE__DOES_NOT_EXIST) &&
				(vOperation == CRX__C__OS__FILE_SYSTEM__OPERATION__READ) &&
				(pFileOpenMode == CRX__C__OS__FILE_SYSTEM__FILE_OPEN_MODE__FORCE_OPEN))
		{
			vOperation = CRX__C__OS__FILE_SYSTEM__OPERATION__WRITE;

			if(crx_c_os_fileSystem_verifyFileSystemPath(pContract, vOperation,
					pFileFullPath, pDepthLimitOfReparsePoints, false, 
					&vVerifiedPathPrefix, &vVerifiedRoute,
					NULL, &vLengthResidueWhenDirectory, &vLengthResidue, NULL, NULL))
			{
				vIsNoError = crx_c_os_fileSystem_verifyFileSystemPath2(pContract, vOperation, 
						NULL, pDepthLimitOfReparsePoints, &vExistance, 
						&vVerifiedPathPrefix, &vVerifiedRoute, &vExclusiveEndIndexOfExistingRoute)

				if(vIsNoError && (vExistance != CRX__C__OS__FILE_SYSTEM__EXISTANCE__DOES_NOT_EXIST))
					{vIsNoError = false;}
			}
			else
				{vIsNoError = false;}
		}

		if(vIsNoError && !crx_c_string_appendCString(&vFullPath, &vVerifiedPathPrefix))
			{vIsNoError = false;}
		if(vIsNoError && !crx_c_string_appendChars(&vFullPath, 
				crx_c_string_getCharsPointer(&vVerifiedRoute), vExclusiveEndIndexOfExistingRoute))
			{vIsNoError = false;}
	}
	else
		{vIsNoError = crx_c_string_copyAssignFrom(&vFullPath, pFileFullPath);}

	if(vIsNoError && (pRecord != NULL) &&
			(vExistance == CRX__C__OS__FILE_SYSTEM__EXISTANCE__DOES_NOT_EXIST))
	{
		if((pFileOpenMode == CRX__C__OS__FILE_SYSTEM__FILE_OPEN_MODE__CREATE) ||
				(pFileOpenMode == CRX__C__OS__FILE_SYSTEM__FILE_OPEN_MODE__FORCE_OPEN))
		{
			bool tIsFirstTime = true;
			size_t tCurrentIndex = vExclusiveEndIndexOfExistingRoute;
			size_t tCurrentEndIndex = crx_c_string_getIndexOfFirstOccuranceOf3(&vVerifiedRoute, 
					tCurrentIndex, CRX__OS__FILE_SYSTEM__SEPERATOR_STRING, 1, 0);

			while(vIsNoError && (tCurrentEndIndex != ((size_t)(-1))))
			{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
	(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
	(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
				if(tIsFirstTime)
				{
					if(vIsNoError && !crx_c_string_appendChars(&vFullPath, 
							crx_c_string_getCharsPointer(&vVerifiedRoute) + tCurrentIndex, 
							tCurrentEndIndex - tCurrentIndex + 1))
						{vIsNoError = false;}

					if(vIsNoError && !crx_c_os_fileSystem_internal_posix_lookAt2(&vFileHandle,
							&vFullPath, &vRemainingPartialPath))
						{vIsNoError = false;}

					tIsFirstTime = false;
				}
				else
				{
					if(vIsNoError && !crx_c_os_fileSystem_internal_posix_replaceOpenat(&vFileHandle,
							&vRemainingPartialPath, 
							O_RDONLY /*| O_DIRECTORY DOES NOT EXIST ON MACOS 10.7*/, 0))
						{vIsNoError = false;}

					if(!vIsNoError)
					{
						crx_c_string_empty(&vRemainingPartialPath);

						vIsNoError = crx_c_string_appendChars(&vRemainingPartialPath, 
								crx_c_string_getCharsPointer(&vVerifiedRoute) + tCurrentIndex, 
								tCurrentEndIndex - tCurrentIndex + 1);
					}
				}

				if(vIsNoError && !crx_c_os_fileSystem_internal_posix_mkdirat(&vFileHandle,
						crx_c_getCString(&vRemainingPartialPath), 0777))
					{vIsNoError = false;}
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
				vIsNoError = crx_c_string_appendChars(&vFullPath, 
						crx_c_string_getCharsPointer(&vVerifiedRoute) + tCurrentIndex, 
						tCurrentEndIndex - tCurrentIndex + 1);
						
				if(vIsNoError && !crx_c_os_fileSystem_private_win32_doCreateDirectory(
						false, &vFullPath, NULL, NULL))
					{vIsNoError = false;}
#endif

				tCurrentIndex = tCurrentEndIndex + 1;
				tCurrentEndIndex = crx_c_string_getIndexOfFirstOccuranceOf3(&vVerifiedRoute, 
						tCurrentIndex, CRX__OS__FILE_SYSTEM__SEPERATOR_STRING, 1, 0);
			}
			
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
	(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
	(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
			crx_c_os_fileSystem_internal_posix_close(&vFileHandle);
#endif
			
			if(vIsNoError && !crx_c_string_appendChars(&vFullPath, 
					crx_c_string_getCharsPointer(&vVerifiedRoute) + tCurrentIndex, 
					crx_c_string_getSize(&vVerifiedRoute) - tCurrentIndex))
				{vIsNoError = false;}
		}
		else
			{vIsNoError = false;}
	}

#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
	(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
	(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	if(pFileOpenMode == CRX__C__OS__FILE_SYSTEM__FILE_OPEN_MODE__CREATE)
		{vFlags = (O_CREAT | O_EXCL);}
	else if(pFileOpenMode == CRX__C__OS__FILE_SYSTEM__FILE_OPEN_MODE__FORCE_OPEN)
		{vFlags = O_CREAT;}
	
	if(pFileAccessMode == CRX__C__OS__FILE_SYSTEM__FILE_ACCESS_MODE__READ_ONLY)
		{vFlags = (vFlags | O_RDONLY);}
	else if(pFileAccessMode == CRX__C__OS__FILE_SYSTEM__FILE_ACCESS_MODE__NORMAL)
		{vFlags = (vFlags | O_RDWR);}
	else if(pFileAccessMode == CRX__C__OS__FILE_SYSTEM__FILE_ACCESS_MODE__WRITE_ONLY)
		{vFlags = (vFlags | O_WRONLY);}

	if(vIsNoError && !crx_c_os_fileSystem_internal_posix_lookAt2(&vFileHandle,
			&vFullPath, &vRemainingPartialPath))
		{vIsNoError = false;}
	if(vIsNoError && !crx_c_os_fileSystem_internal_posix_replaceOpenat(&vFileHandle, 
			&vRemainingPartialPath, vFlags, 0777))
		{vIsNoError = false;}
		
	if(vIsNoError)
	{
		if(pFileShareMode == CRX__C__OS__FILE_SYSTEM__FILE_SHARE_MODE__EXCLUSIVE)
			{vIsNoError = crx_c_os_fileSystem_private_posix_flock(&vFileHandle, LOCK_EX|LOCK_NB);}
		else if(pFileShareMode == CRX__C__OS__FILE_SYSTEM__FILE_SHARE_MODE__SHARED)
			{vIsNoError = crx_c_os_fileSystem_private_posix_flock(&vFileHandle, LOCK_SH|LOCK_NB);}

		if(!vIsNoError)
			{crx_c_os_fileSystem_internal_posix_close(&vFileHandle);}

		vReturn = vFileHandle.gFileDescriptor;
	}
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	if(pFileOpenMode == CRX__C__OS__FILE_SYSTEM__FILE_OPEN_MODE__CREATE)
		{vCreationDisposition = CREATE_NEW;}
	else if(pFileOpenMode == CRX__C__OS__FILE_SYSTEM__FILE_OPEN_MODE__OPEN)
		{vCreationDisposition = OPEN_EXISTING;}
	else if(pFileOpenMode == CRX__C__OS__FILE_SYSTEM__FILE_OPEN_MODE__FORCE_OPEN)
		{vCreationDisposition = OPEN_ALWAYS;}

	if(pFileAccessMode == CRX__C__OS__FILE_SYSTEM__FILE_ACCESS_MODE__READ_ONLY)
		{vDesiredAccess = GENERIC_READ;}
	else if(pFileAccessMode == CRX__C__OS__FILE_SYSTEM__FILE_ACCESS_MODE__NORMAL)
		{vDesiredAccess = (GENERIC_READ | GENERIC_WRITE);}
	else if(pFileAccessMode == CRX__C__OS__FILE_SYSTEM__FILE_ACCESS_MODE__WRITE_ONLY)
		{vDesiredAccess = GENERIC_WRITE;}

	if(pFileShareMode == CRX__C__OS__FILE_SYSTEM__FILE_SHARE_MODE__SHARED)
	{
		vShareMode = (FILE_SHARE_READ | FILE_SHARE_WRITE | (pIsToEnableShareDeleteWhenPossible ?
				FILE_SHARE_DELETE : 0));
	}
	else if(pFileShareMode == CRX__C__OS__FILE_SYSTEM__FILE_SHARE_MODE__EXCLUSIVE)
		{vShareMode = (pIsToEnableShareDeleteWhenPossible ? FILE_SHARE_DELETE : 0);}
	

	if(vIsNoError)
	{
		vReturn = crx_c_os_fileSystem_private_win32_createFile(false, &vFullPath, vDesiredAccess, 
				vShareMode, NULL, vCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
	}
#endif

	if(vReturn && pInternalFileFullPath)
	{
		if(!crx_c_string_tryToPilferSwapContentWith(pInternalFileFullPath, &vFullPath))
		{
			crx_c_string_destruct(pInternalFileFullPath);
			memcpy(pInternalFileFullPath, &vFullPath, sizeof(Crx_C_String));
			crx_c_string_construct(&vFullPath);
		}
	}

	crx_c_string_destruct(&vVerifiedPathPrefix);
	crx_c_string_destruct(&vVerifiedRoute);
	crx_c_string_destruct(&vFullPath);
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	crx_c_os_fileSystem_internal_posix_fileHandle_destruct(&vFileHandle);
	crx_c_string_destruct(&vRemainingPartialPath);
#endif

	return vReturn;

	CRX_SCOPE_END
}
/*
	pMinimumPrefixLength MUST BE THE LENGTH OF THE PREFIX, WHICH INCLUDES THE ENDING SLASH, OR LESS 
			SUCH AS IT FALLS IN THE LAST SEGMENT OF THE PREFIX
			
	CURRENTLY, pFileFullPath SHOULD BE NO MORE THAN 1000 BYTES.
*/
Crx_C_Os_FileSystem_OsFileHandle crx_c_os_fileSystem_internal_rawOpen(
		char const * CRX_NOT_NULL pFileFullPath, size_t pMinimumPrefixLength,
		Crx_C_Os_FileSystem_FileOpenMode pFileOpenMode,
		Crx_C_Os_FileSystem_FileAccessMode pFileAccessMode,
		Crx_C_Os_FileSystem_FileShareMode pFileShareMode,
		bool pIsToEnableShareDeleteWhenPossible)
{
	char vFileFullPath[1024] /*= ?*/;
	Crx_C_Os_FileSystem_Operation vOperation = CRX__C__OS__FILE_SYSTEM__OPERATION__NULL;
	Crx_C_Os_FileSystem_Existance vExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__NULL;
	size_t vExclusiveEndIndexOfExistingPath = 0;
	size_t vLength = strlen(pFileFullPath);
	size_t vCurrentIndex = pMinimumPrefixLength;
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	int32_t vFlags = 0;
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	uint32_t vCreationDisposition = 0;
	uint32_t vDesiredAccess = 0;
	uint32_t vShareMode = 0;
#endif
	bool vIsNoError = true;
	Crx_C_Os_FileSystem_OsFileHandle vReturn = CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE;
	
	if(vLength < 1001)//THIS IS NOT A MISTAKE.
		{memcpy(&(vFileFullPath[0]), pFileFullPath, vLength + 1);}
	else
		{vIsNoError = false;}
	
	if(vIsNoError && (vFileFullPath[vLength - 1] == CRX__OS__FILE_SYSTEM__SEPERATOR_CHAR))
		{vIsNoError = false;}

	if((pFileAccessMode == CRX__C__OS__FILE_SYSTEM__FILE_ACCESS_MODE__NORMAL) ||
			(pFileAccessMode == CRX__C__OS__FILE_SYSTEM__FILE_ACCESS_MODE__WRITE_ONLY))
		{vOperation = CRX__C__OS__FILE_SYSTEM__OPERATION__WRITE;}
	else if(pFileOpenMode == CRX__C__OS__FILE_SYSTEM__FILE_OPEN_MODE__CREATE)
		{vOperation = CRX__C__OS__FILE_SYSTEM__OPERATION__WRITE;}
	else
		{vOperation = CRX__C__OS__FILE_SYSTEM__OPERATION__READ;}

	if(vIsNoError && ((pFileOpenMode == CRX__C__OS__FILE_SYSTEM__FILE_OPEN_MODE__FORCE_OPEN) ||
			(pFileOpenMode == CRX__C__OS__FILE_SYSTEM__FILE_OPEN_MODE__CREATE)))
	{
		while((vFileFullPath[vCurrentIndex] != CRX__OS__FILE_SYSTEM__SEPERATOR_CHAR) &&
				(vCurrentIndex < vLength))
			{vCurrentIndex++;}

		if(vCurrentIndex < vLength)
		{
			size_t tCurrentIndex = vLength - 1;
			
			while(tCurrentIndex != vCurrentIndex)
			{
				while((vFileFullPath[tCurrentIndex] != CRX__OS__FILE_SYSTEM__SEPERATOR_CHAR) &&
						(tCurrentIndex > vCurrentIndex))
					{tCurrentIndex--;}

				if(tCurrentIndex != vCurrentIndex)
				{
					vFileFullPath[tCurrentIndex] = 0;

					if(crx_c_os_fileSystem_rawDoesEntityExists(&(vFileFullPath[0])))
					{
						vFileFullPath[tCurrentIndex] = CRX__OS__FILE_SYSTEM__SEPERATOR_CHAR;
						vExclusiveEndIndexOfExistingPath = tCurrentIndex;
						
						break;
					}
					else
					{
						vFileFullPath[tCurrentIndex] = CRX__OS__FILE_SYSTEM__SEPERATOR_CHAR;
						tCurrentIndex = tCurrentIndex - 1;
					}
				}
			}
		}
		else
			{vExclusiveEndIndexOfExistingPath = vLength;}
	}

	if(vIsNoError && (vExclusiveEndIndexOfExistingPath < vLength))
	{
		bool tIsFirstTime = true;
		size_t tCurrentIndex = vExclusiveEndIndexOfExistingPath;
		size_t tCurrentEndIndex = tCurrentIndex + 1;
		
		while((vFileFullPath[tCurrentEndIndex] != CRX__OS__FILE_SYSTEM__SEPERATOR_CHAR) &&
				(tCurrentEndIndex < vLength))
			{tCurrentEndIndex++;}

		while(vIsNoError && (tCurrentEndIndex != vLength))
		{
			vFileFullPath[tCurrentEndIndex] = 0;

#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
			crx_c_os_fileSystem_internal_posix_rawMkdir(&(vFileFullPath[0]), 0777);
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
			crx_c_os_fileSystem_private_win32_rawDoCreateDirectory(&(vFileFullPath[0]), NULL, NULL);
#endif

			vFileFullPath[tCurrentEndIndex] = CRX__OS__FILE_SYSTEM__SEPERATOR_CHAR;
			
			while((vFileFullPath[tCurrentEndIndex] != CRX__OS__FILE_SYSTEM__SEPERATOR_CHAR) &&
					(tCurrentEndIndex < vLength))
				{tCurrentEndIndex++;}
		}
	}

#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
	(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
	(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	if(pFileOpenMode == CRX__C__OS__FILE_SYSTEM__FILE_OPEN_MODE__CREATE)
		{vFlags = (O_CREAT | O_EXCL);}
	else if(pFileOpenMode == CRX__C__OS__FILE_SYSTEM__FILE_OPEN_MODE__FORCE_OPEN)
		{vFlags = O_CREAT;}
	
	if(pFileAccessMode == CRX__C__OS__FILE_SYSTEM__FILE_ACCESS_MODE__READ_ONLY)
		{vFlags = (vFlags | O_RDONLY);}
	else if(pFileAccessMode == CRX__C__OS__FILE_SYSTEM__FILE_ACCESS_MODE__NORMAL)
		{vFlags = (vFlags | O_RDWR);}
	else if(pFileAccessMode == CRX__C__OS__FILE_SYSTEM__FILE_ACCESS_MODE__WRITE_ONLY)
		{vFlags = (vFlags | O_WRONLY);}

	if(vIsNoError && !crx_c_os_fileSystem_internal_posix_rawOpen(&vReturn, 
			&(vFileFullPath[0]), vFlags, 0777))
		{vIsNoError = false;}
		
	if(vIsNoError)
	{
		if(pFileShareMode == CRX__C__OS__FILE_SYSTEM__FILE_SHARE_MODE__EXCLUSIVE)
			{vIsNoError = crx_c_os_fileSystem_private_posix_rawFlock(vReturn, LOCK_EX|LOCK_NB);}
		else if(pFileShareMode == CRX__C__OS__FILE_SYSTEM__FILE_SHARE_MODE__SHARED)
			{vIsNoError = crx_c_os_fileSystem_private_posix_rawFlock(vReturn, LOCK_SH|LOCK_NB);}

		if(!vIsNoError)
			{crx_c_os_fileSystem_close(&vReturn);}
	}
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	if(pFileOpenMode == CRX__C__OS__FILE_SYSTEM__FILE_OPEN_MODE__CREATE)
		{vCreationDisposition = CREATE_NEW;}
	else if(pFileOpenMode == CRX__C__OS__FILE_SYSTEM__FILE_OPEN_MODE__OPEN)
		{vCreationDisposition = OPEN_EXISTING;}
	else if(pFileOpenMode == CRX__C__OS__FILE_SYSTEM__FILE_OPEN_MODE__FORCE_OPEN)
		{vCreationDisposition = OPEN_ALWAYS;}

	if(pFileAccessMode == CRX__C__OS__FILE_SYSTEM__FILE_ACCESS_MODE__READ_ONLY)
		{vDesiredAccess = GENERIC_READ;}
	else if(pFileAccessMode == CRX__C__OS__FILE_SYSTEM__FILE_ACCESS_MODE__NORMAL)
		{vDesiredAccess = (GENERIC_READ | GENERIC_WRITE);}
	else if(pFileAccessMode == CRX__C__OS__FILE_SYSTEM__FILE_ACCESS_MODE__WRITE_ONLY)
		{vDesiredAccess = GENERIC_WRITE;}

	if(pFileShareMode == CRX__C__OS__FILE_SYSTEM__FILE_SHARE_MODE__SHARED)
	{
		vShareMode = (FILE_SHARE_READ | FILE_SHARE_WRITE | (pIsToEnableShareDeleteWhenPossible ?
				FILE_SHARE_DELETE : 0));
	}
	else if(pFileShareMode == CRX__C__OS__FILE_SYSTEM__FILE_SHARE_MODE__EXCLUSIVE)
		{vShareMode = (pIsToEnableShareDeleteWhenPossible ? FILE_SHARE_DELETE : 0);}

	if(vIsNoError)
	{
		vReturn = crx_c_os_fileSystem_private_win32_rawCreateFile(&(vFileFullPath[0]), 
				vDesiredAccess, vShareMode, NULL, vCreationDisposition, FILE_ATTRIBUTE_NORMAL, 
				NULL);
	}
#endif

	return vReturn;
}

/*
	REMEMBER, NOT ONLY IS IT DIFFICULT TO OPEN DIRECTORIES IN WINDOWS, OPENING DIRECTORIES DOES NOT 
			APPEAR SUPPORTED ON WINDOWS9X AT ALL. THEREFORE, ONLY CREATION IS IMPLEMENTED HERE.
*/
bool crx_c_os_fileSystem_createDirectory(
		Crx_C_String const * CRX_NOT_NULL pDirectoryFullPath,
		uint32_t pDepthLimitOfReparsePoints,
		Crx_C_Os_FileSystem_Contract const * CRX_NOT_NULL pContract)
{
	Crx_C_Os_FileSystem_Existance vExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__NULL;
	size_t vExclusiveEndIndexOfExistingRoute = 0;
	Crx_C_String vVerifiedPathPrefix /*= ?*/;
	Crx_C_String vVerifiedRoute /*= ?*/;
	Crx_C_String vFullPath /*= ?*/;
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	Crx_C_String vRemainingPartialPath /*= ?*/;
	Crx_C_Os_FileSystem_Internal_Posix_FileHandle vFileHandle /*= ?*/;
#endif
	size_t vLengthResidueWhenDirectory = 0;
	size_t vLengthResidue = 0;
	bool vIsNoError = true;
	
	crx_c_string_construct(&vVerifiedPathPrefix);
	crx_c_string_construct(&vVerifiedRoute);
	crx_c_string_construct(&vFullPath);
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	crx_c_string_construct(&vRemainingPartialPath);
	crx_c_os_fileSystem_internal_posix_fileHandle_construct(&vFileHandle);
#endif

	if(pRecord == NULL)
	{
		if(crx_c_os_fileSystem_verifyFileSystemPath(pContract, 
				CRX__C__OS__FILE_SYSTEM__OPERATION__WRITE,
				pDirectoryFullPath, pDepthLimitOfReparsePoints, false, 
				&vVerifiedPathPrefix, &vVerifiedRoute,
				NULL, &vLengthResidueWhenDirectory, &vLengthResidue, NULL, NULL))
		{
			vIsNoError = crx_c_os_fileSystem_verifyFileSystemPath2(pContract, 
					CRX__C__OS__FILE_SYSTEM__OPERATION__WRITE, 
					NULL, pDepthLimitOfReparsePoints, &vExistance, 
					&vVerifiedPathPrefix, &vVerifiedRoute, &vExclusiveEndIndexOfExistingRoute)
		}
		else
			{vIsNoError = false;}

		if(vIsNoError && !crx_c_string_isEndingWith2(&vVerifiedRoute, 
				CRX__OS__FILE_SYSTEM__SEPERATOR_STRING))
			{vIsNoError = false;}

		if(vIsNoError && !crx_c_string_appendCString(&vFullPath, &vVerifiedPathPrefix))
			{vIsNoError = false;}
		if(vIsNoError && !crx_c_string_appendChars(&vFullPath, 
				crx_c_string_getCharsPointer(&vVerifiedRoute), vExclusiveEndIndexOfExistingRoute))
			{vIsNoError = false;}
	}
	else
		{vIsNoError = crx_c_string_copyAssignFrom(&vFullPath, pDirectoryFullPath);}

	if(vIsNoError &&
			(vExistance == CRX__C__OS__FILE_SYSTEM__EXISTANCE__DOES_NOT_EXIST))
	{
		bool tIsFirstTime = true;
		size_t tCurrentIndex = vExclusiveEndIndexOfExistingRoute;
		size_t tCurrentEndIndex = crx_c_string_getIndexOfFirstOccuranceOf3(&vVerifiedRoute, 
				tCurrentIndex, CRX__OS__FILE_SYSTEM__SEPERATOR_STRING, 1, 0);

		while(vIsNoError && (tCurrentEndIndex != ((size_t)(-1))))
		{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
			if(tIsFirstTime)
			{
				if(vIsNoError && !crx_c_string_appendChars(&vFullPath, 
						crx_c_string_getCharsPointer(&vVerifiedRoute) + tCurrentIndex, 
						tCurrentEndIndex - tCurrentIndex + 1))
					{vIsNoError = false;}

				if(vIsNoError && !crx_c_os_fileSystem_internal_posix_lookAt2(&vFileHandle,
						&vFullPath, &vRemainingPartialPath))
					{vIsNoError = false;}

				tIsFirstTime = false;
			}
			else
			{
				if(vIsNoError && !crx_c_os_fileSystem_internal_posix_replaceOpenat(&vFileHandle,
						&vRemainingPartialPath, 
						O_RDONLY /*| O_DIRECTORY DOES NOT EXIST ON MACOS 10.7*/, 0))
					{vIsNoError = false;}

				if(!vIsNoError)
				{
					crx_c_string_empty(&vRemainingPartialPath);

					vIsNoError = crx_c_string_appendChars(&vRemainingPartialPath, 
							crx_c_string_getCharsPointer(&vVerifiedRoute) + tCurrentIndex, 
							tCurrentEndIndex - tCurrentIndex + 1);
				}
			}

			if(vIsNoError && !crx_c_os_fileSystem_internal_posix_mkdirat(&vFileHandle,
					crx_c_getCString(&vRemainingPartialPath), 0777))
				{vIsNoError = false;}
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
			vIsNoError = crx_c_string_appendChars(&vFullPath, 
					crx_c_string_getCharsPointer(&vVerifiedRoute) + tCurrentIndex, 
					tCurrentEndIndex - tCurrentIndex + 1);
					
			if(vIsNoError && !crx_c_os_fileSystem_private_win32_doCreateDirectory(
					false, &vFullPath, NULL, NULL))
				{vIsNoError = false;}
#endif

			tCurrentIndex = tCurrentEndIndex + 1;
			tCurrentEndIndex = crx_c_string_getIndexOfFirstOccuranceOf3(&vVerifiedRoute, 
					tCurrentIndex, CRX__OS__FILE_SYSTEM__SEPERATOR_STRING, 1, 0);
		}

#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
		crx_c_os_fileSystem_internal_posix_close(&vFileHandle);
#endif
			
		if(vIsNoError && !crx_c_string_appendChars(&vFullPath, 
				crx_c_string_getCharsPointer(&vVerifiedRoute) + tCurrentIndex, 
				crx_c_string_getSize(&vVerifiedRoute) - tCurrentIndex))
			{vIsNoError = false;}
	}

	crx_c_string_destruct(&vVerifiedPathPrefix);
	crx_c_string_destruct(&vVerifiedRoute);
	crx_c_string_destruct(&vFullPath);
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	crx_c_os_fileSystem_internal_posix_fileHandle_destruct(&vFileHandle);
	crx_c_string_destruct(&vRemainingPartialPath);
#endif

	return vIsNoError;
}
bool crx_c_os_fileSystem_internal_rawCreateDirectory(
		char const * CRX_NOT_NULL pDirectoryFullPath,
		uint32_t pDepthLimitOfReparsePoints)
{
	
	char vDirectoryFullPath[1024] /*= ?*/;
	size_t vLength = strlen(pDirectoryFullPath);
	size_t vExclusiveEndIndexOfExistingPath = 0;
	bool vIsNoError = true;
	
	if(vLength < 1001)//THIS IS NOT A MISTAKE.
		{memcpy(&(vDirectoryFullPath[0]), pDirectoryFullPath, vLength + 1);}
	else
		{vIsNoError = false;}

	if(vIsNoError && (vDirectoryFullPath[vLength - 1] != CRX__OS__FILE_SYSTEM__SEPERATOR_CHAR))
		{vIsNoError = false;}
	
	if(vIsNoError)
	{
		size_t tCurrentIndex = vLength - 1;

		while(vFileFullPath[vCurrentIndex] != CRX__OS__FILE_SYSTEM__SEPERATOR_CHAR)
			{vCurrentIndex++;}
		
		if(tCurrentIndex != vCurrentIndex)
		{
			while(tCurrentIndex != vCurrentIndex)
			{
				bool tIsToBreak = false;

				vFileFullPath[tCurrentIndex] = 0;

				if(crx_c_os_fileSystem_rawDoesEntityExists(&(vFileFullPath[0])))
				{
					vFileFullPath[tCurrentIndex] = CRX__OS__FILE_SYSTEM__SEPERATOR_CHAR;
					vExclusiveEndIndexOfExistingPath = tCurrentIndex;
					
					tIsToBreak = true;
				}
				else
				{
					vFileFullPath[tCurrentIndex] = CRX__OS__FILE_SYSTEM__SEPERATOR_CHAR;
					tCurrentIndex = tCurrentIndex - 1;
				}

				if(!tIsToBreak)
				{
					while((vFileFullPath[tCurrentIndex] != CRX__OS__FILE_SYSTEM__SEPERATOR_CHAR) &&
							(tCurrentIndex > vCurrentIndex))
						{tCurrentIndex--;}
				}
				else
					{break;}
			}
			
			if(vExclusiveEndIndexOfExistingPath < vCurrentIndex)
				{vExclusiveEndIndexOfExistingPath = vCurrentIndex;}
		}
		else
			{vExclusiveEndIndexOfExistingPath = vLength;} //PATH PREFIX ALWAYS EXISTS OR A CONTRADICTION
	}

	if(vIsNoError && (vExclusiveEndIndexOfExistingPath < vLength))
	{
		bool tIsFirstTime = true;
		size_t tCurrentIndex = vExclusiveEndIndexOfExistingPath;
		size_t tCurrentEndIndex = tCurrentIndex + 1;
		
		while((vFileFullPath[tCurrentEndIndex] != CRX__OS__FILE_SYSTEM__SEPERATOR_CHAR) &&
				(tCurrentEndIndex < vLength))
			{tCurrentEndIndex++;}

		while(vIsNoError && (tCurrentEndIndex != vLength))
		{
			vFileFullPath[tCurrentEndIndex] = 0;

#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
			crx_c_os_fileSystem_internal_posix_rawMkdir(&(vFileFullPath[0]), 0777);
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
			crx_c_os_fileSystem_private_win32_rawDoCreateDirectory(&(vFileFullPath[0]), NULL, NULL);
#endif

			vFileFullPath[tCurrentEndIndex] = CRX__OS__FILE_SYSTEM__SEPERATOR_CHAR;
			
			while((vFileFullPath[tCurrentEndIndex] != CRX__OS__FILE_SYSTEM__SEPERATOR_CHAR) &&
					(tCurrentEndIndex < vLength))
				{tCurrentEndIndex++;}
		}
	}

	return vIsNoError;
}



bool crx_c_os_fileSystem_close(Crx_C_Os_FileSystem_OsFileHandle * pOsFileHandle)
{
	CRX_SCOPE_META
	if((pOsFileHandle == NULL) || (pOsFileHandle == CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE))
		{return true;}

	CRX_SCOPE
	bool vReturn = true;

#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	vReturn = crx_c_os_fileSystem_private_posix_rawClose(pOsFileHandle);
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	vReturn = (CloseHandle(pOsFileHandle) == 0);
#endif

	*pOsFileHandle = CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE;
	
	return vReturn;
	CRX_SCOPE_END
}

/*
	NOTE: THE PARAMETER pInternalFullPath CAN BE USED TO GET THE PATH IN ITS CORRECT CASING.
	
	IF THIS FUNCTION RETURNS CRX__C__OS__FILE_SYSTEM__EXISTANCE__NULL, EXISTANCE COULD NOT BE
			ESTABLISHED. THERE WAS A CRITICAL ERROR.
	IF THIS FUNCTION DOES NOT RETURN  CRX__C__OS__FILE_SYSTEM__EXISTANCE__NULL, AND 
			pInternalFullPath IS NOT NULL, THIS FUNCTION MUST STILL RETURN WITH AN EMPTY 
			pInternalFullPath. THIS HAPPENS IF THERE WAS AN ERROR WHILE ESTABLISHING THE INTERNAL
			PATH. AS OF THIS WRITING, THIS IS JUST A LOW MEMORY ERROR.
*/
Crx_C_Os_FileSystem_Existance crx_c_os_fileSystem_getExistanceOf(
		Crx_C_String const * CRX_NOT_NULL pFullPath, uint32_t pDepthLimitOfReparsePoints,
		Crx_C_Os_FileSystem_Contract const * pContract,
		Crx_C_String * pInternalFullPath)
{
	Crx_C_Os_FileSystem_Existance vExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__NULL;
	Crx_C_String vVerifiedPathPrefix /*= ?*/;
	Crx_C_String vVerifiedRoute /*= ?*/;
	Crx_C_String vFullPath /*= ?*/;
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	Crx_C_String vRemainingPartialPath /*= ?*/;
#endif
	size_t vLengthResidueWhenDirectory = 0;
	size_t vLengthResidue = 0;
	bool vIsNoError = true;
	
	crx_c_string_construct(&vVerifiedPathPrefix);
	crx_c_string_construct(&vVerifiedRoute);
	crx_c_string_construct(&vFullPath);
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	crx_c_string_construct(&vRemainingPartialPath);
#endif

	if(crx_c_os_fileSystem_verifyFileSystemPath(pContract, 
			CRX__C__OS__FILE_SYSTEM__OPERATION__READ,
			pFullPath, pDepthLimitOfReparsePoints, false, 
			&vVerifiedPathPrefix, &vVerifiedRoute,
			NULL, &vLengthResidueWhenDirectory, &vLengthResidue, NULL, NULL))
	{
		vIsNoError = crx_c_os_fileSystem_verifyFileSystemPath2(pContract, 
				CRX__C__OS__FILE_SYSTEM__OPERATION__READ, 
				NULL, pDepthLimitOfReparsePoints, &vExistance, 
				&vVerifiedPathPrefix, &vVerifiedRoute, NULL);
	}
	else
		{vIsNoError = false;}

	if(pInternalFullPath != NULL)
	{
		if(vIsNoError && !crx_c_string_copyAssignFrom(pInternalFullPath, &vVerifiedPathPrefix))
			{vIsNoError = false;}
		if(vIsNoError && !crx_c_string_isEqualTo2(&vVerifiedRoute, 
				CRX__OS__FILE_SYSTEM__SEPERATOR_STRING) && 
				!crx_c_string_appendString(pInternalFullPath, &vVerifiedRoute))
			{vIsNoError = false;}

		if(!vIsNoError)
			{crx_c_string_empty(pInternalFullPath);}
	}

	crx_c_string_destruct(&vVerifiedPathPrefix);
	crx_c_string_destruct(&vVerifiedRoute);
	crx_c_string_destruct(&vFullPath);
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	crx_c_string_destruct(&vRemainingPartialPath);
#endif

	return vExistance;
}
bool crx_c_os_fileSystem_rawDoesEntityExists(char const * CRX_NOT_NULL pFullPath)
{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	Crx_NonCrxed_Os_Posix_Dll_Libc_Stat vStat /*= ?*/;

	return crx_c_os_fileSystem_internal_posix_rawStat(pFullPath, &vStat);
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	return (crx_c_os_fileSystem_private_win32_rawGetFileAttributes(pFullPath) != 
		INVALID_FILE_ATTRIBUTES);
#endif
}

/*
	IS NOT SUPPORTED FOR exFat EVEN THOUGH OTHER WRITINGS ON exFat ARE. THIS IS BECAUSE IT CAN NOT
	BE GUARANTEED THAT THE DATES SET ARE EXACTLY WHAT THE USER PASSED. NEITHER IS IT GUARANTEED OUT
	THERE THAT ALL SYSTEMS WOULD SET THE SAME DATE VALUE GIVING THE SAME INPUT DATE.
	
	THE FOLLOWING FOLLOWS REPARSE POINTS; DOES NOT ACT ON REPARSE POINTS BUT INSTEAD ACTS ON THEIR
	TARGETS.
	
	THE FOLLOWING GUARANTEES THAT THE TIME SET IS EXACTLY WHAT IS PASSED ROUNDED TO THE NEAREST 2
	SECONDS. HENCE, IF YOU NEED TO STORE LOCAL TIMES, YOU NEED TO DO THE CORRECTION YOURSELF BEFORE
	CALLING THIS FUNCTION. MOUNT OPTIONS ON SOME SYSTEMS, AND AUTO CONVERSIONS BY OTHERS, ARE
	EXPLICITLY CANCELLED OUT BY THIS FUNCTION TO GIVE THE AFORE MENTIONED GUARANTEE.
*/
bool crx_c_os_fileSystem_setModificationDate(Crx_C_String const * CRX_NOT_NULL pFullPath,
		uint32_t pDepthLimitOfReparsePoints, Crx_C_Os_FileSystem_Time const * CRX_NOT_NULL pTime, 
		Crx_C_Os_FileSystem_Contract const * CRX_NOT_NULL pContract)
{
	return crx_c_os_fileSystem_setModificationDate__do(pFullPath, pDepthLimitOfReparsePoints, NULL,
			pTime, pContract);
}
bool crx_c_os_fileSystem_setModificationDate2(
		Crx_C_Os_FileSystem_Iterator_Record const * CRX_NOT_NULL pRecord,
		Crx_C_Os_FileSystem_Time const * CRX_NOT_NULL pTime)
{
	return crx_c_os_fileSystem_setModificationDate__do(
			crx_c_os_fileSystem_iterator_record_getInternalFullPath(pRecord), 0, pRecord, pTime, 
			NULL);
}
bool crx_c_os_fileSystem_setModificationDate__do(Crx_C_String const * CRX_NOT_NULL pFullPath,
		uint32_t pDepthLimitOfReparsePoints,
		Crx_C_Os_FileSystem_Iterator_Record * pRecord,
		Crx_C_Os_FileSystem_Time const * pTime, 
		Crx_C_Os_FileSystem_Contract const * pContract)
{
	CRX_SCOPE_META
	assert((pRecord == NULL) || CRX__ARE_POINTERS_TO_SAME_OBJECT(pFullPath,
			crx_c_os_fileSystem_iterator_record_getInternalFullPath(pRecord), true));
	assert((pRecord == NULL) || (pContract != NULL));

	CRX_SCOPE
	Crx_C_String vVerifiedPathPrefix /*= ?*/;
	Crx_C_String vVerifiedRoute /*= ?*/;
	Crx_C_String vFullPath /*= ?*/;
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	Crx_C_String vRemainingPartialPath /*= ?*/;
	Crx_C_Os_FileSystem_Internal_Posix_FileHandle vFileHandle /*= ?*/;
#endif
	size_t vLengthResidueWhenDirectory = 0;
	size_t vLengthResidue = 0;
	bool vIsNoError = true;
	
	crx_c_string_construct(&vVerifiedPathPrefix);
	crx_c_string_construct(&vVerifiedRoute);
	crx_c_string_construct(&vFullPath);
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	crx_c_string_construct(&vRemainingPartialPath);
	crx_c_os_fileSystem_internal_posix_fileHandle_construct(&vFileHandle);
#endif


	if(pRecord == NULL)
	{
		if(crx_c_os_fileSystem_verifyFileSystemPath(
				pContract, CRX__C__OS__FILE_SYSTEM__OPERATION__WRITE,
				pFullPath, pDepthLimitOfReparsePointsOfNewFullPath, false,
				&vVerifiedPathPrefix, &vVerifiedRoute,
				NULL, &vLengthResidueWhenDirectory, &vLengthResidue, NULL, NULL))
		{
			Crx_C_Os_FileSystem_Existance tExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__NULL;
				
			if(crx_c_os_fileSystem_verifyFileSystemPath2(pContract, 
					CRX__C__OS__FILE_SYSTEM__OPERATION__WRITE, NULL, pDepthLimitOfReparsePoints, 
					&tExistance, &vVerifiedPathPrefix, &vVerifiedRoute, NULL))
			{
				vIsNoError = crx_c_string_appendCString(&vFullPath, &vVerifiedPathPrefix);
				if(vIsNoError && !crx_c_string_appendCString(&vFullPath, &vVerifiedRoute))
					{vIsNoError = false;}
			}
		}
		else
			{vIsNoError = false;}
	}
	
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
	(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
	(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	if(vIsNoError && !crx_c_os_fileSystem_internal_posix_lookAt2(&vFileHandle,
			&vFullPath, &vRemainingPartialPath))
		{vIsNoError = false;}
#endif
	
	
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	if(vIsNoError)
	{
		Crx_C_Os_FileSystem_MountInfo tMounfInfo /*= ?*/;
		size_t tLength = 0;
		Crx_C_Os_FileSystem_Time tTimes[2] /*= ?*/;

		crx_c_os_fileSystem_mountInfo_construct(&tMounfInfo);
		memset(&(tTimes[0]), 0, 2 * sizeof(Crx_C_Os_FileSystem_Time));
		
		crx_c_os_fileSystem_posix_getMountInformationFor(&vFullPath, 2, &tMounfInfo);

		if(crx_c_string_isEqualTo2(crx_c_os_fileSystem_mountInfo_getFileSystemName(&tMounfInfo),
				"EXFAT"))
			{vIsNoError = false;}
		else
		{
			if(pTime != NULL)
				{memcpy(&(tTimes[1]), pTime, sizeof(Crx_C_Os_FileSystem_Time));}
			else if(crx_c_os_fileSystem_mountInfo_getOffsetToUtc(&tMounfInfo) != 0)
			{
				//crx_c_os_fileSystem_private_posix_time(&(tTimes[1].gSeconds));
				//crx_c_os_fileSystem_time_setFemtoSeconds(&(tTimes[1]), 0);
				crx_c_os_fileSystem_time_setToCurrentTime(&(tTimes[1]));
			}

			if(crx_c_os_fileSystem_mountInfo_getOffsetToUtc(&tMounfInfo) != 0)
			{
				//REMEMBER, WE ARE UNDOING WHAT THE SYSTEM WOULD LATER DO.
				 tTimes[1].gSeconds = tTimes[1].gSeconds -
						crx_c_os_fileSystem_mountInfo_getOffsetToUtc(&tMounfInfo);
			}
			
			if(tTimes[1].gSeconds > 0)
			{
				if((((uint64_t)(tTimes[1].gSeconds)) << 63) > 0)
					{tTimes[1].gSeconds = tTimes[1].gSeconds + 1;}
			}
			else if(tTimes[1].gSeconds < 0)
			{
				uint64_t tUint64_t = ((uint64_t)(tTimes[1].gSeconds * (-1)));

				if((((uint64_t)(tTimes[1].gSeconds)) << 63) > 0)
					{tUint64_t = tUint64_t - 1;}

				tTimes[1].gSeconds = ((int64_t)tUint64_t);
			}

			if(pRecord == NULL)
			{
				vIsNoError = crx_c_os_fileSystem_private_posix_futimesat(&vFileHandle,
						crx_c_string_getCString(&vRemainingPartialPath), &(tTimes[0]),
						0, (((pTime != NULL) && (crx_c_os_fileSystem_mountInfo_getOffsetToUtc(
						&tMounfInfo) != 0)) ? 1 : 2));
			}
			else
			{
				if(crx_c_os_fileSystem_internal_posix_replaceOpenat(&vFileHandle,
						crx_c_string_getCString(&vRemainingPartialPath), O_WRONLY, 0))
					{vIsNoError = crx_c_os_fileSystem_private_posix_flock(&vFileHandle, LOCK_SH);}
				else
					{vIsNoError = false;}
				
				if(vIsNoError && !crx_c_os_fileSystem_iterator_verifyRecordAgainst(pRecord,
						vFileHandle->gFileDescriptor))
					{vIsNoError = false;}
				if(vIsNoError && crx_c_os_fileSystem_private_posix_futimes(&vFileHandle,
						&(tTimes[0]), 0, 
						(((pTime != NULL) && (crx_c_os_fileSystem_mountInfo_getOffsetToUtc(
								&tMounfInfo) != 0)) ? 1 : 2)))
					{vIsNoError = false;}
			}
		}

		crx_c_os_fileSystem_internal_posix_close(&vFileHandle); //UNLOCKS AS WELL.

		crx_c_os_fileSystem_mountInfo_destruct(&tMounfInfo);
	}
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	if(vIsNoError)
	{
		Crx_C_String tFileSystemName /*= ?*/;
		HANDLE tHandle = INVALID_HANDLE_VALUE;

		crx_c_string_construct(&tFileSystemName);

		if(crx_c_os_fileSystem_private_win32_doGetVolumeInformation(&vFullPath, 
				((pRecord == NULL) ? crx_c_string_getSize(&vVerifiedRoute) : 
						pRecord->gPrivate_byteSizeOfPathPrefixInInternalFullPath),
				NULL, NULL, NULL, NULL, &tFileSystemName))
		{
			tHandle = crx_c_os_fileSystem_private_win32_createFile(false, &vFullPath, 
					GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0,
					NULL);
		}
				
		if(tHandle != INVALID_HANDLE_VALUE)
		{
			if(pRecord != NULL)
			{
				vIsNoError = 
						crx_c_os_fileSystem_iterator_verifyRecordAgainst(pRecord, tHandle);
			}
		}
		else
			{vIsNoError = false;}

		if(vIsNoError)
		{
			FILETIME tFileTime /*= ?*/;

			crx_c_os_fileSystem_time_private_getAsFileTime(&pTime, tFileTime);

			/*
				I AM NOT SO SURE ABOUT EXFAT YET. IT SEEMS THAT IT HAS BEEN INTRODUCED IN VISTA. IT
				HAS A FLAG OF SORT THAT INDICATES THAT THE TIMESTAMP HAS TIME ZONE OFFSET 
				INFORMATION, BUT I AM UNSURE WHEN THIS INFORMATION BECAME USED.
				
				EXFAT APEPARS TO STORE THE LOCAL TIME, AND THEN STORE AN OFFSET WITH IT. AS LONG
				AS ALL SYSTEMS UNDERSTAND AND WORK WITH THIS OFFSET, WE CAN SIMPLY ASSUME EXFAT
				USES UTC. HOWEVER, THIS DOES NOT APPEAR TO BE THE CASE. WINDOWS XP WITH EXFAT
				INSTALLED, DEALS WITH THE OFFSET CORRECTLY IT SEEMS, WHILE VISTA, OR PERHAPS EARLY
				VISTA, DID NOT. IT SEEMS THAT IT IS COMMON OUT THERE TO SIMPLY TREAT EXFAT AS
				FAT AND IGNORE THE BIT THAT INDICATES THE EXISTANCE OF AN OFFSET. WHEN THIS HAPPENS
				THE NET BEHAVIOR IS JUST LIKE VFAT AND THE FOLLOWING COULD BE USED. BUT IF NOT,
				THE FOLLOWING CAN NOT BE USED, AND IT WOULD BETTER TO SIMPLY ASSUME A UTC0 SITUATION
				AS MENTIONED EARLIER.
			*/
			if(crx_c_string_isEqualTo2(&tFileSystemName, "EXFAT", true))
				{vIsNoError = false;}
			else
			{
				if(crx_c_string_isEqualTo2(&tFileSystemName, "FAT", true) ||
						crx_c_string_isEqualTo2(&tFileSystemName, "FAT32", true) /* ||
						crx_c_string_isEqualTo2(&tFileSystemName, "EXFAT", true)*/)
				{
					FILETIME tFileTime2 /*= ?*/;

					LocalFileTimeToFileTime(&tFileTime, &tFileTime2);
					
					memcpy(&tFileTime2, &tFileTime, sizeof(FILETIME));
				}

				SetFileTime(tHandle, NULL, NULL, &tFileTime);
			}
			
			CloseHandle(tHandle);
			tHandle = INVALID_HANDLE_VALUE;
		}

		crx_c_string_destruct(&tFileSystemName);
	}
#endif
			
	
	crx_c_string_destruct(&vVerifiedPathPrefix);
	crx_c_string_destruct(&vVerifiedRoute);
	crx_c_string_destruct(&vFullPath);
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	crx_c_os_fileSystem_internal_posix_fileHandle_destruct(&vFileHandle);
	crx_c_string_destruct(&vRemainingPartialPath);
#endif

	return vIsNoError;

	CRX_SCOPE_END
}

/*
	IS NOT SUPPORTED FOR exFat EVEN THOUGH OTHER READINGS ON exFat ARE. THIS IS BECAUSE IT CAN NOT
	BE GUARANTEED THAT THE DATES READ ARE EXACTLY WHAT THE FILE SYSTEM HAS STORED, NEITHER IS IT
	GUARANTEED OUT THERE THAT ALL SYSTEMS WOULD READ THE SAME DATE VALUE GIVEN THE SAME STORED
	ACTUAL DATE.
	
	THE FOLLOWING FOLLOWS REPARSE POINTS; DOES NOT ACT ON REPARSE POINTS BUT INSTEAD ACTS ON THEIR
	TARGETS.
	
	THE FOLLOWING GUARANTEES THAT THE TIME READ IS EXACTLY WHAT IS STORED ON THE FILE SYSTEM. HENCE, 
	IF YOU NEED TO READ LOCAL TIMES, YOU NEED TO DO THE CORRECTION YOURSELF AFTER CALLING THIS 
	FUNCTION. MOUNT OPTIONS ON SOME SYSTEMS, AND AUTO CONVERSIONS BY OTHERS, ARE
	EXPLICITLY CANCELLED OUT BY THIS FUNCTION TO GIVE THE AFORE MENTIONED GUARANTEE.
	
	THE FOLLOWING ATTEMPTS TO READ THE MODIFICATION DATE: 
		- IF IT IS IN THE PAST, IT IS RETURNED.
		- IF IT IS MORE THAN 2 SECONDS IN THE FUTURE, IT IS RETURNED.
		- IF IT IS LESS THAN 2 SECONDS IN THE FUTURE, THE CALLING THREAD IS PUT TO SLEEP UNTIL
				ENOUGH TIME HAS PASSED FOR THE READING TO BE IN THE PAST. WHEN WOKEN THEN THE DATE 
				IS RE READ, AND THE AFORE MENTIONED CHECKS ARE RE DONE.
*/
bool crx_c_os_fileSystem_getModificationDate(Crx_C_Os_FileSystem_Time * CRX_NOT_NULL pReturn,
		Crx_C_String const * CRX_NOT_NULL pFullPath, uint32_t pDepthLimitOfReparsePoints,  
		Crx_C_Os_FileSystem_Contract const * CRX_NOT_NULL pContract)
{
	return crx_c_os_fileSystem_getModificationDate__do(pReturn, pFullPath, 
			pDepthLimitOfReparsePoints, NULL, pContract);
}
bool crx_c_os_fileSystem_getModificationDate2(Crx_C_Os_FileSystem_Time const * CRX_NOT_NULL pReturn,
		Crx_C_Os_FileSystem_Iterator_Record const * CRX_NOT_NULL pRecord)
{
	return crx_c_os_fileSystem_getModificationDate__do(pReturn,
			crx_c_os_fileSystem_iterator_record_getInternalFullPath(pRecord), 0, pRecord, NULL);
}
bool crx_c_os_fileSystem_getModificationDate__do(Crx_C_Os_FileSystem_Time * CRX_NOT_NULL pReturn,
		Crx_C_String const * CRX_NOT_NULL pFullPath,
		uint32_t pDepthLimitOfReparsePoints, 
		Crx_C_Os_FileSystem_Iterator_Record * pRecord,
		Crx_C_Os_FileSystem_Contract const * pContract)
{
	CRX_SCOPE_META
	assert((pRecord == NULL) || CRX__ARE_POINTERS_TO_SAME_OBJECT(pFullPath,
			crx_c_os_fileSystem_iterator_record_getInternalFullPath(pRecord), true));
	assert((pRecord == NULL) || (pContract != NULL));

	CRX_SCOPE
	Crx_C_String vVerifiedPathPrefix /*= ?*/;
	Crx_C_String vVerifiedRoute /*= ?*/;
	Crx_C_String vFullPath /*= ?*/;
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	Crx_C_String vRemainingPartialPath /*= ?*/;
	Crx_C_Os_FileSystem_Internal_Posix_FileHandle vFileHandle /*= ?*/;
#endif
	size_t vLengthResidueWhenDirectory = 0;
	size_t vLengthResidue = 0;
	bool vIsNoError = true;
	
	crx_c_string_construct(&vVerifiedPathPrefix);
	crx_c_string_construct(&vVerifiedRoute);
	crx_c_string_construct(&vFullPath);
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	crx_c_string_construct(&vRemainingPartialPath);
	crx_c_os_fileSystem_internal_posix_fileHandle_construct(&vFileHandle);
#endif


	if(pRecord == NULL)
	{
		if(crx_c_os_fileSystem_verifyFileSystemPath(
				pContract, CRX__C__OS__FILE_SYSTEM__OPERATION__READ,
				pFullPath, pDepthLimitOfReparsePointsOfNewFullPath, false
				&vVerifiedPathPrefix, &vVerifiedRoute,
				NULL, &vLengthResidueWhenDirectory, &vLengthResidue, NULL, NULL))
		{
			Crx_C_Os_FileSystem_Existance tExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__NULL;
				
			if(crx_c_os_fileSystem_verifyFileSystemPath2(pContract, 
					CRX__C__OS__FILE_SYSTEM__OPERATION__READ, NULL, pDepthLimitOfReparsePoints, 
					&tExistance, &vVerifiedPathPrefix, &vVerifiedRoute, NULL))
			{
				vIsNoError = crx_c_string_appendCString(&vFullPath, &vVerifiedPathPrefix);
				if(vIsNoError && !crx_c_string_appendCString(&vFullPath, &vVerifiedRoute))
					{vIsNoError = false;}
			}
		}
		else
			{vIsNoError = false;}
	}
	
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
	(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
	(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	if(vIsNoError && !crx_c_os_fileSystem_internal_posix_lookAt2(&vFileHandle,
			&vFullPath, &vRemainingPartialPath))
		{vIsNoError = false;}
#endif
	
	
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	if(vIsNoError)
	{
		Crx_C_Os_FileSystem_MountInfo tMounfInfo /*= ?*/;
		size_t tLength = 0;

		crx_c_os_fileSystem_mountInfo_construct(&tMounfInfo);
		
		crx_c_os_fileSystem_posix_getMountInformationFor(&vFullPath, 2, &tMounfInfo);

		if(crx_c_string_isEqualTo2(crx_c_os_fileSystem_mountInfo_getFileSystemName(&tMounfInfo),
				"EXFAT"))
			{vIsNoError = false;}
		else
		{
			Crx_NonCrxed_Os_Posix_Dll_Libc_Stat tStat /*= ?*/;

			CRX__SET_TO_ZERO(Crx_NonCrxed_Os_Posix_Dll_Libc_Stat, tStat);

			if(pRecord == NULL)
			{
				vIsNoError = crx_c_os_fileSystem_internal_posix_fstatat(&vFileHandle,
						crx_c_string_getCString(&vRemainingPartialPath), &tStat, false);
			}
			else
			{
				if(crx_c_os_fileSystem_internal_posix_replaceOpenat(&vFileHandle,
						crx_c_string_getCString(&vRemainingPartialPath), O_RDONLY, 0))
					{vIsNoError = crx_c_os_fileSystem_private_posix_flock(&vFileHandle, LOCK_SH);}
				else
					{vIsNoError = false;}
				
				if(vIsNoError && !crx_c_os_fileSystem_iterator_verifyRecordAgainst(pRecord,
						vFileHandle->gFileDescriptor))
					{vIsNoError = false;}
			}
		}

		if(vIsNoError)
		{
			while(true)
			{
				Crx_C_Os_FileSystem_Time tTime /*= ?*/;

				CRX__SET_TO_ZERO(Crx_C_Os_FileSystem_Time, tTime);

				crx_c_os_fileSystem_time_setToCurrentTime(&tTime);

				if(vIsNoError && !crx_c_os_fileSystem_internal_posix_fstat(&vFileHandle, &tStat))
					{vIsNoError = false;}
				
				pReturn->gSeconds = tStat.st_mtime + 
						crx_c_os_fileSystem_mountInfo_getOffsetToUtc(&tMounfInfo);
				crx_c_os_fileSystem_time_setFemtoSeconds(pReturn, 0);
				
				crx_c_os_fileSystem_time_subtract(&tTime, pReturn);

				if(tTime.gSeconds >= 0)
					{break;}
				else
				{
					if(tTime.gSeconds <= (-2))
					{
						tTime.gSeconds = tTime.gSeconds * (-1);
						
						crx_c_pthread_crx_sleep(crx_c_os_fileSystem_time_getAsThreadingInterval(
								&tTime));
					}
				}
			}
		}

		crx_c_os_fileSystem_internal_posix_close(&vFileHandle); //UNLOCKS AS WELL.

		crx_c_os_fileSystem_mountInfo_destruct(&tMounfInfo);
	}
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	if(vIsNoError)
	{
		Crx_C_String tFileSystemName /*= ?*/;
		HANDLE tHandle = INVALID_HANDLE_VALUE;

		crx_c_string_construct(&tFileSystemName);

		if(crx_c_os_fileSystem_private_win32_doGetVolumeInformation(&vFullPath, 
				((pRecord == NULL) ? crx_c_string_getSize(&vVerifiedRoute) : 
						pRecord->gPrivate_byteSizeOfPathPrefixInInternalFullPath),
				NULL, NULL, NULL, NULL, &tFileSystemName))
		{
			tHandle = crx_c_os_fileSystem_private_win32_createFile(false, &vFullPath, 
					GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0,
					NULL);
		}
				
		if(tHandle != INVALID_HANDLE_VALUE)
		{
			if(pRecord != NULL)
			{
				vIsNoError = 
						crx_c_os_fileSystem_iterator_verifyRecordAgainst(pRecord, tHandle);
			}
		}
		else
			{vIsNoError = false;}

		if(vIsNoError)
		{
			if(crx_c_string_isEqualTo2(&tFileSystemName, "EXFAT", true))
				{vIsNoError = false;}
			else
			{
				bool tIsFatLike = (crx_c_string_isEqualTo2(&tFileSystemName, "FAT", true) ||
						crx_c_string_isEqualTo2(&tFileSystemName, "FAT32", true) /* ||
						crx_c_string_isEqualTo2(&tFileSystemName, "EXFAT", true)*/)

				while(true)
				{
					FILETIME tFileTime /*= ?*/;

					if(tIsFatLike)
					{
						Crx_C_Os_FileSystem_Internal_Win32_Win32FindData tWin32FindData /*= ?*/;
						HANDLE tHandle = INVALID_HANDLE_VALUE;
								
						crx_c_os_fileSystem_internal_win32_win32FindData_construct(&tWin32FindData);
						
						//FindFirstFile() DOES NOT FOLLOW REPARSE POINTS, BUT BECAUSE WE ARE ON A
						//		FAT FILE SYSTEM WHICH DO NOT SUPPORT REPARSE POINT, THIS IS NOT AN
						//		ISSUE.
						tHandle = crx_c_os_fileSystem_internal_win32_findFirstFile(false, &vFullPath, 
								&tWin32FindData);

						if(tHandle != INVALID_HANDLE_VALUE)
						{
							vIsNoError = crx_c_os_fileSystem_iterator_verifyRecordAgainst2(pRecord, 
									&tWin32FindData);

							if(vIsNoError)
							{
								FileTimeToLocalFileTime(&(tWin32FindData.ftLastWriteTime), 
										&tFileTime);
							}
							
							crx_c_os_fileSystem_internal_win32_findClose(tHandle);
							tHandle = INVALID_HANDLE_VALUE;
						}

						crx_c_os_fileSystem_internal_win32_win32FindData_destruct(&tWin32FindData);
					}
					else
						{vIsNoError = (GetFileTime(tHandle, NULL, NULL, &tFileTime) != 0);}

					if(vIsNoError)
					{
						Crx_C_Os_FileSystem_Time tTime /*= ?*/;

						CRX__SET_TO_ZERO(Crx_C_Os_FileSystem_Time, tTime);
						
						crx_c_os_fileSystem_time_setToCurrentTime(&tTime);
						
						crx_c_os_fileSystem_time_private_setFromFileTime(pReturn, &tFileTime);

						crx_c_os_fileSystem_time_subtract(&tTime, pReturn);

						if(tTime.gSeconds >= 0)
							{break;}
						else
						{
							if(tTime.gSeconds <= (-2))
							{
								tTime.gSeconds = tTime.gSeconds * (-1);
								
								crx_c_pthread_crx_sleep(
										crx_c_os_fileSystem_time_getAsThreadingInterval(&tTime));
							}
						}
					}
					else
						{break;}
				}
			}
			
			CloseHandle(tHandle);
			tHandle = INVALID_HANDLE_VALUE;
		}

		crx_c_string_destruct(&tFileSystemName);
	}
#endif
			
	
	crx_c_string_destruct(&vVerifiedPathPrefix);
	crx_c_string_destruct(&vVerifiedRoute);
	crx_c_string_destruct(&vFullPath);
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	crx_c_os_fileSystem_internal_posix_fileHandle_destruct(&vFileHandle);
	crx_c_string_destruct(&vRemainingPartialPath);
#endif

	return vIsNoError;

	CRX_SCOPE_END
}

CRX__LIB__PUBLIC_C_FUNCTION() bool crx_c_os_fileSystem_getFileSize(uint64_t * CRX_NOT_NULL pReturn,
		Crx_C_String const * CRX_NOT_NULL pFullPath, uint32_t pDepthLimitOfReparsePoints,  
		Crx_C_Os_FileSystem_Contract const * CRX_NOT_NULL pContract)
{
	return crx_c_os_fileSystem_getFileSize__do(pReturn, pFullPath, 
			pDepthLimitOfReparsePoints, NULL, pContract);
}
CRX__LIB__PUBLIC_C_FUNCTION() bool crx_c_os_fileSystem_getFileSize2(uint64_t * CRX_NOT_NULL pReturn,
		Crx_C_Os_FileSystem_Iterator_Record const * CRX_NOT_NULL pRecord)
{
	return crx_c_os_fileSystem_getFileSize__do(pReturn,
			crx_c_os_fileSystem_iterator_record_getInternalFullPath(pRecord), 0, pRecord, NULL);
}
bool crx_c_os_fileSystem_getFileSize__do(Crx_C_Os_FileSystem_Time * CRX_NOT_NULL pReturn,
		Crx_C_String const * CRX_NOT_NULL pFullPath,
		uint32_t pDepthLimitOfReparsePoints, 
		Crx_C_Os_FileSystem_Iterator_Record * pRecord,
		Crx_C_Os_FileSystem_Contract const * pContract)
{
	CRX_SCOPE_META
	assert((pRecord == NULL) || CRX__ARE_POINTERS_TO_SAME_OBJECT(pFullPath,
			crx_c_os_fileSystem_iterator_record_getInternalFullPath(pRecord), true));
	assert((pRecord == NULL) || (pContract != NULL));

	CRX_SCOPE
	Crx_C_String vVerifiedPathPrefix /*= ?*/;
	Crx_C_String vVerifiedRoute /*= ?*/;
	Crx_C_String vFullPath /*= ?*/;
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	Crx_C_String vRemainingPartialPath /*= ?*/;
	Crx_C_Os_FileSystem_Internal_Posix_FileHandle vFileHandle /*= ?*/;
#endif
	size_t vLengthResidueWhenDirectory = 0;
	size_t vLengthResidue = 0;
	uint64_t vFileSize = 0;
	bool vIsNoError = true;
	
	crx_c_string_construct(&vVerifiedPathPrefix);
	crx_c_string_construct(&vVerifiedRoute);
	crx_c_string_construct(&vFullPath);
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	crx_c_string_construct(&vRemainingPartialPath);
	crx_c_os_fileSystem_internal_posix_fileHandle_construct(&vFileHandle);
#endif

	if(pRecord == NULL)
	{
		if(crx_c_os_fileSystem_verifyFileSystemPath(
				pContract, CRX__C__OS__FILE_SYSTEM__OPERATION__READ,
				pFullPath, pDepthLimitOfReparsePointsOfNewFullPath, false,
				&vVerifiedPathPrefix, &vVerifiedRoute,
				NULL, &vLengthResidueWhenDirectory, &vLengthResidue, NULL, NULL))
		{
			Crx_C_Os_FileSystem_Existance tExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__NULL;
				
			if(crx_c_os_fileSystem_verifyFileSystemPath2(pContract, 
					CRX__C__OS__FILE_SYSTEM__OPERATION__READ, NULL, pDepthLimitOfReparsePoints, 
					&tExistance, &vVerifiedPathPrefix, &vVerifiedRoute, NULL))
			{
				vIsNoError = crx_c_string_appendCString(&vFullPath, &vVerifiedPathPrefix);
				if(vIsNoError && !crx_c_string_appendCString(&vFullPath, &vVerifiedRoute))
					{vIsNoError = false;}
			}
		}
		else
			{vIsNoError = false;}
	}
	
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
	(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
	(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	if(vIsNoError && !crx_c_os_fileSystem_internal_posix_lookAt2(&vFileHandle,
			&vFullPath, &vRemainingPartialPath))
		{vIsNoError = false;}
#endif
	
	
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	if(vIsNoError)
	{
		Crx_NonCrxed_Os_Posix_Dll_Libc_Stat tStat;
		
		CRX__SET_TO_ZERO(Crx_NonCrxed_Os_Posix_Dll_Libc_Stat, tStat);
		
		if(pRecord == NULL)
		{
			vIsNoError = crx_c_os_fileSystem_internal_posix_fstatat(&vFileHandle,
					crx_c_string_getCString(&vRemainingPartialPath), &tStat, false);
		}
		else
		{
			if(!crx_c_os_fileSystem_internal_posix_replaceOpenat(&vFileHandle,
					crx_c_string_getCString(&vRemainingPartialPath), O_RDONLY, 0))
				{vIsNoError = false;}
			if(vIsNoError && !crx_c_os_fileSystem_iterator_verifyRecordAgainst(pRecord,
					vFileHandle->gFileDescriptor))
				{vIsNoError = false;}

			if(vIsNoError && !crx_c_os_fileSystem_internal_posix_fstat(&vFileHandle, &tStat))
				{vIsNoError = false;}
		}

		if(vIsNoError)
			{vFileSize = ((uint64_t)(tStat.st_size > 0 ? tStat.st_size : 0));}

		crx_c_os_fileSystem_internal_posix_close(&vFileHandle);
	}
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	if(vIsNoError)
	{
		ULARGE_INTEGER tULargeInteger;
		BY_HANDLE_FILE_INFORMATION tByHandleFileInformation /*= ?*/;

		if(pRecord == NULL)
		{
			vIsNoError = crx_c_os_fileSystem_private_win32_getFileInformation(
					false, &vFullPath, crx_c_string_isEndingWith2(&vFullPath, 
					CRX__OS__FILE_SYSTEM__SEPERATOR_STRING), &tByHandleFileInformation);
		}
		else
		{
			HANDLE tHandle = crx_c_os_fileSystem_private_win32_createFile(false, &vFullPath,
					GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0,
					NULL);

			if(tHandle != INVALID_HANDLE_VALUE)
			{
				if(crx_c_os_fileSystem_iterator_verifyRecordAgainst(pRecord, tHandle))
				{
					vIsNoError = (crx_c_os_fileSystem_internal_win32_getFileInformationByHandle(
							tHandle, &tByHandleFileInformation) != 0);
				}
				else
					{vIsNoError = false;}

				CloseHandle(tHandle);
				tHandle = INVALID_HANDLE_VALUE;
			}
			else
				{vIsNoError = false;}
		}

		if(vIsNoError)
		{				
			tULargeInteger.u.HighPart = tByHandleFileInformation.nFileSizeHigh;
			tULargeInteger.u.LowPart = tByHandleFileInformation.nFileSizeLow;
			
			vFileSize = tULargeInteger.QuadPart;
		}
	}
#endif

	crx_c_string_destruct(&vVerifiedPathPrefix);
	crx_c_string_destruct(&vVerifiedRoute);
	crx_c_string_destruct(&vFullPath);
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	crx_c_os_fileSystem_internal_posix_fileHandle_destruct(&vFileHandle);
	crx_c_string_destruct(&vRemainingPartialPath);
#endif

	return vFileSize;

	CRX_SCOPE_END
}

/*
	WARNING: IF THIS RETURNS true, THE ENTITY MAY OR MAY NOT HAVE BEEN UNLIKNED. THIS RETURNS
			FALSE IF NO ATTEMPT TO UNLINK WAS MADE. THIS HAPPENS IF THE ENTITY IS OPEN, AND 
			pModeForCheckingFileNotInUse IS SUCH AS CHECKING WAS REQUESTED, OR THE CONTRACT IS
			VIOLATED BY THE PASSED PATH, OR IF THE PATH IS A DIRECTORY, THE RESULTING PARTIAL
			PATH IF pContractApplicationLevel IS LARGER THAN 1.

	NO SUPPORT FOR RECORDS, BECAUSE DELETION ULTIMATELY WILL HAVE TO USE A PATH. HENCE THE RECORD 
			CAN NOT BE VERIFIED ON THE FINAL RESOLUTION OF THE PATH BEFORE DELETION.
	
	NOTE: THE FOLLOWING DOES NOT DELETE REPARSE POINTS.
	
	THE FOLLOWING PROMISES, IF IT SUCCEEDS, THAT THE PATH pFullPath WILL EVENTUALLY BE AVAILABLE
	FOR USE, MEANING FOR CREATING A NEW DIRECTORY OR FILE THERE. THE FOLLOWING DOES NOT PROMISE
	THAT THE FILE WILL BE DELETED, NOR THAT IT WILL EVENTUALLY BE DELETED.
	
	IF A DIRECTORY IS PASSED, THE DIRECTORY AND ALL ITS CONTENT ARE DELETED.
	
	CHECKS ARE DONE AGAINST THE CONTRACT PER THE CONTRACT APPLICATION LEVEL. SEE 
	crx_c_os_fileSystem_isEntityNotInUse()
*/
bool crx_c_os_fileSystem_unlink(Crx_C_String const * CRX_NOT_NULL pFullPath,
		uint32_t pDepthLimitOfReparsePoints, 
		Crx_C_Os_FileSystem_Contract const * pContract, uint32_t pModeForCheckingFileNotInUse,
		uint32_t pContractApplicationLevel)
{
	Crx_C_String vVerifiedPathPrefix /*= ?*/;
	Crx_C_String vVerifiedRoute /*= ?*/;
	Crx_C_String vFullPath /*= ?*/;
	bool vIsADirectory = false;
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	Crx_C_String vRemainingPartialPath /*= ?*/;
	Crx_C_Os_FileSystem_Internal_Posix_FileHandle vFileHandle /*= ?*/;
#endif
	size_t vLengthResidueWhenDirectory = 0;
	size_t vLengthResidue = 0;
	bool vIsNoError = true;
	
	crx_c_string_construct(&vVerifiedPathPrefix);
	crx_c_string_construct(&vVerifiedRoute);
	crx_c_string_construct(&vFullPath);
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	crx_c_string_construct(&vRemainingPartialPath);
	crx_c_os_fileSystem_internal_posix_fileHandle_construct(&vFileHandle);
#endif

	if(crx_c_os_fileSystem_verifyFileSystemPath(
			pContract, CRX__C__OS__FILE_SYSTEM__OPERATION__WRITE,
			pFullPath, pDepthLimitOfReparsePointsOfNewFullPath, false,
			&vVerifiedPathPrefix, &vVerifiedRoute,
			NULL, &vLengthResidueWhenDirectory, &vLengthResidue, NULL, NULL))
	{
		Crx_C_Os_FileSystem_Existance tExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__NULL;
			
		if(crx_c_os_fileSystem_verifyFileSystemPath2(pContract,
				CRX__C__OS__FILE_SYSTEM__OPERATION__WRITE, NULL, pDepthLimitOfReparsePoints, 
				&tExistance, &vVerifiedPathPrefix, &vVerifiedRoute, NULL))
		{
			if(tExistance == CRX__C__OS__FILE_SYSTEM__EXISTANCE__EXISTS)
			{
				vIsNoError = crx_c_string_appendCString(&vFullPath, &vVerifiedPathPrefix);
				if(vIsNoError && !crx_c_string_appendCString(&vFullPath, &vVerifiedRoute))
					{vIsNoError = false;}
			}
			else if(tExistance != CRX__C__OS__FILE_SYSTEM__EXISTANCE__DOES_NOT_EXIST)
				{vIsNoError = false;}
		}
	}
	else
		{vIsNoError = false;}

	if(vIsNoError)
	{
		vIsADirectory = crx_c_string_isEndingWith2(&vFullPath, 
				CRX__OS__FILE_SYSTEM__SEPERATOR_STRING);

		if(pContractApplicationLevel == 1)
		{
			vIsNoError = crx_c_os_fileSystem_internal_isEntityNotInUse(pModeForCheckingFileNotInUse, 
					&vFullPath, NULL, 0, 0, NULL, false);
		}
		else if((pContractApplicationLevel == 2) || (pContractApplicationLevel == 3))
		{
			vIsNoError = crx_c_os_fileSystem_internal_isEntityNotInUse(pModeForCheckingFileNotInUse, 
					&vFullPath, &vFullPath, vLengthResidueWhenDirectory, vLengthResidue, pContract,
					false /*DELETING IS ONLY A WRITE OPERATION.*/);
		}
		else
			{assert(false);}
	}
	
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
	(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
	(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	if(vIsNoError && !crx_c_os_fileSystem_internal_posix_lookAt2(&vFileHandle,
			&vFullPath, &vRemainingPartialPath))
		{vIsNoError = false;}
#endif

	if(vIsNoError && vIsADirectory)
	{
		//AT THIS POINT, WE NO LONGER STOP FOR VERIFICATION AND FILE USAGE ERRORS.
		Crx_C_Os_FileSystem_Contract tContract /*= ?*/;
		Crx_C_Os_FileSystem_Iterator tIterator /*= ?*/;
		Crx_C_String tTempString /*= ?*/;
		bool tIsToTryAgain = true;

		//FORMAL ERROR
		CRX__SET_TO_ZERO(Crx_C_Os_FileSystem_Contract, tContract);
		{
			tContract.gCharacterSet = CRX__C__OS__INFO__CHARACTER_SET__UNICODE;
			tContract.gGuaranteeLevel = 1;
			tContract.gFileSystemGuaranteeLevel = 1;
			tContract.gFileSystemRuntimeGuaranteeLevel = 1;
			tContract.gResolutionModel = CRX__C__OS__FILE_SYSTEM__RESOLUTION_MODEL__PRECISE;
		}
		crx_c_os_fileSystem_iterator_internal_construct(&tIterator, &tContract, false, false);
		crx_c_string_construct(&tTempString);

		while(vIsNoError && tIsToTryAgain)
		{
			tIsToTryAgain = false;

			while(vIsNoError && crx_c_os_fileSystem_iterator_isValid(&vIterator))
			{
				if(crx_c_os_fileSystem_iterator_set(&vIterator, &vFullPath, UINT32_MAX, NULL))
				{
					Crx_C_Os_FileSystem_Iterator_EntryType tEntryType = 
							crx_c_os_fileSystem_iterator_getCurrentEntryType(&vIterator);

					if(crx_c_os_fileSystem_iterator_getCurrentRunCount(&vIterator) == 1)
					{
						//ITERATING OVER FILES THEN DIRECTORIES. DELETING FILES ONLY.
						if(tEntryType == CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY_TYPE__DIRECTORY)
							{crx_c_os_fileSystem_iterator_queueThenNext(&tIterator);}
						else if(tEntryType == CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY_TYPE__FILE)
						{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
	(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
	(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
							if(crx_c_os_fileSystem_private_posix_unlinkat(
									crx_c_os_fileSystem_iterator_internal_getParentDirectoryFileHandleOnPosix(
											&tIterator),
									crx_c_os_fileSystem_iterator_getCurrentName(&tIterator)))
								{tIsToTryAgain = true;}
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
							if(crx_c_os_fileSystem_iterator_getCurrentInternalFullPath(&tIterator, 
									&tTempString))
							{
								if(crx_c_os_fileSystem_private_win32_deleteFile(false,
										&tTempString))
									{tIsToTryAgain = true;}
							}
#endif
							crx_c_os_fileSystem_iterator_next(&tIterator);
						}
						else
							{crx_c_os_fileSystem_iterator_next(&tIterator);}
					}
				}
			}

			if(vIsNoError && !crx_c_os_fileSystem_iterator_isNoError(&vIterator))
				{vIsNoError = false;}
		}
		
		tIsToTryAgain = true;

		while(vIsNoError && tIsToTryAgain)
		{
			tIsToTryAgain = false;

			while(vIsNoError && crx_c_os_fileSystem_iterator_isValid(&vIterator))
			{
				if(crx_c_os_fileSystem_iterator_set(&vIterator, &vFullPath, UINT32_MAX, NULL))
				{
					Crx_C_Os_FileSystem_Iterator_EntryType tEntryType = 
							crx_c_os_fileSystem_iterator_getCurrentEntryType(&vIterator);

					//ITERATING OVER FILES THEN DIRECTORIES. DELETING DIRECTORIES ONLY.
					if(tEntryType == CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY_TYPE__DIRECTORY)
					{
						if(crx_c_os_fileSystem_iterator_getCurrentIsAMoveOut(&vIterator))
						{
							if(!crx_c_os_fileSystem_iterator_getCurrentIsAForcedMoveOut(&vIterator))
							{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
	(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
	(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
								if(crx_c_string_copyAssignFrom(&tTempString,
										crx_c_os_fileSystem_iterator_getCurrentName(&tIterator)) &&
										crx_c_string_appendString(&tTempString, 
										CRX__OS__FILE_SYSTEM__SEPERATOR_STRING))
								{
									if(crx_c_os_fileSystem_private_posix_unlinkat(
											crx_c_os_fileSystem_iterator_internal_getParentDirectoryFileHandleOnPosix(
											&tIterator), &tTempString))
										{tIsToTryAgain = true;}
								}
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
								if(crx_c_os_fileSystem_iterator_getCurrentInternalFullPath(
										&tIterator, &tTempString))
								{
									if(crx_c_os_fileSystem_private_win32_removeDirectory(false,
											&tTempString))
										{tIsToTryAgain = true;}
								}
#endif	
							}
							crx_c_os_fileSystem_iterator_next(&tIterator);
						}
						else if(!crx_c_os_fileSystem_iterator_getCurrentIsBarredEntry(&vIterator))
							{crx_c_os_fileSystem_iterator_next(&tIterator);}
						else
							{crx_c_os_fileSystem_iterator_nextThenAddress(&tIterator);}
					}
					else if(tEntryType == CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY_TYPE__FILE)
						{crx_c_os_fileSystem_iterator_next(&tIterator);}//CONTINUE TO DELETE WHAT WE CAN
					else
						{crx_c_os_fileSystem_iterator_next(&tIterator);}
				}
			}
		}
		
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
		crx_c_os_fileSystem_private_posix_unlinkat(&vFileHandle, &vRemainingPartialPath);
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
		crx_c_os_fileSystem_private_win32_removeDirectory(false, &vFullPath);
#endif	
		
		crx_c_os_fileSystem_iterator_destruct(&tIterator);
		crx_c_string_destruct(&tTempString);
	}
	else if(vIsNoError)
	{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
	(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
	(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
		Crx_NonCrxed_Os_Posix_Dll_Libc_Stat tStat /*= ?*/;

		CRX__SET_TO_ZERO(Crx_NonCrxed_Os_Posix_Dll_Libc_Stat, tStat);
		
		if(!crx_c_os_fileSystem_internal_posix_fstatat(&vFileHandle, 
				crx_c_string_getCString(&vRemainingPartialPath), &tStat, true))
			{vIsNoError = false;}

		if(vIsNoError && (S_ISLNK(tStat) || !S_ISREG(tStat) ||
				crx_c_os_fileSystem_private_posix_isAMountPointAt(&vFileHandle, 
				&vRemainingPartialPath)))//SEE DESIGN
			{vIsNoError = false;}

		//AT THIS POINT, WE NO LONGER STOP FOR VERIFICATION AND FILE USAGE ERRORS.
		crx_c_os_fileSystem_private_posix_unlinkat(&vFileHandle,
				crx_c_string_getCString(&vRemainingPartialPath));
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
		Crx_C_Os_FileSystem_Internal_Win32_Win32FindData tWin32FindData /*= ?*/;
		HANDLE tHandle = INVALID_HANDLE_VALUE;

		crx_c_os_fileSystem_internal_win32_win32FindData_construct(&tWin32FindData);
		tHandle = crx_c_os_fileSystem_internal_win32_findFirstFile(pIsNotUtf8, &vFullPath,
				&tWin32FindData);

		vIsNoError = (tHandle != INVALID_HANDLE_VALUE);

		if(vIsNoError && !(((tWin32FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) &&
				((tWin32FindData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) == 0)))//SEE DESIGN
			{vIsNoError = false;}

		//AT THIS POINT, WE NO LONGER STOP FOR VERIFICATION AND FILE USAGE ERRORS.
		crx_c_os_fileSystem_private_win32_deleteFile(false, &vFullPath);

		if(tHandle != INVALID_HANDLE_VALUE)
		{
			crx_c_os_fileSystem_internal_win32_findClose(tHandle);
			tHandle = INVALID_HANDLE_VALUE;
		}

		crx_c_os_fileSystem_internal_win32_win32FindData_destruct(&tWin32FindData);
#endif
	}

	crx_c_string_destruct(&vVerifiedPathPrefix);
	crx_c_string_destruct(&vVerifiedRoute);
	crx_c_string_destruct(&vFullPath);
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	crx_c_os_fileSystem_internal_posix_fileHandle_destruct(&vFileHandle);
	crx_c_string_destruct(&vRemainingPartialPath);
#endif
}

bool crx_c_os_fileSystem_rename(Crx_C_String const * CRX_NOT_NULL pFullPath,
		uint32_t pDepthLimitOfReparsePoints, Crx_C_String const * CRX_NOT_NULL pNewEntityName,
		Crx_C_Os_FileSystem_Contract const * CRX_NOT_NULL pContract, 
		uint32_t pModeForCheckingFileNotInUse, uint32_t pContractApplicationLevel)
{
	Crx_C_String vVerifiedPathPrefix /*= ?*/;
	Crx_C_String vVerifiedRoute /*= ?*/;
	Crx_C_String vFullPath /*= ?*/;
	Crx_C_String vEntityName /*= ?*/;
	Crx_C_String vNewEntityName /*= ?*/;
	bool vWasNormalized = false;
	bool vIsADirectory = false;
	size_t vLengthResidueWhenDirectory = 0;
	size_t vLengthResidue = 0;
	size_t vTarget_lengthResidueWhenDirectory = 0;
	size_t vTarget_LengthResidue = 0;
	
	crx_c_string_construct(&vVerifiedPathPrefix);
	crx_c_string_construct(&vVerifiedRoute);
	crx_c_string_construct(&vFullPath);
	crx_c_string_construct(&vEntityName);
	crx_c_string_construct(&vNewEntityName);
	
	if(crx_c_os_fileSystem_verifyFileSystemPath(
			pContract, CRX__C__OS__FILE_SYSTEM__OPERATION__WRITE,
			pFullPath, pDepthLimitOfReparsePointsOfNewFullPath, false,
			&vVerifiedPathPrefix, &vVerifiedRoute,
			NULL, &vLengthResidueWhenDirectory, &vLengthResidue, &vWasNormalized, NULL))
	{
		Crx_C_Os_FileSystem_Existance tExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__NULL;
			
		if(crx_c_os_fileSystem_verifyFileSystemPath2(pContract, 
				CRX__C__OS__FILE_SYSTEM__OPERATION__WRITE, NULL, pDepthLimitOfReparsePoints, 
				&tExistance, &vVerifiedPathPrefix, &vVerifiedRoute, NULL))
		{
			if(tExistance != CRX__C__OS__FILE_SYSTEM__EXISTANCE__EXISTS)
				{vIsNoError = crx_c_string_appendCString(&vFullPath, &vVerifiedPathPrefix);}
			else if(tExistance != CRX__C__OS__FILE_SYSTEM__EXISTANCE__DOES_NOT_EXIST)
				{vIsNoError = false;}
		}
	}
	else
		{vIsNoError = false;}

	if(vIsNoError && crx_c_string_isEqualTo2(&vVerifiedRoute,
			CRX__OS__FILE_SYSTEM__SEPERATOR_STRING))
		{vIsNoError = false;}

	if(vIsNoError)
	{
		size_t tEndIndex = 0;

		vIsADirectory = crx_c_string_isEndingWith2(&vVerifiedRoute, 
				CRX__OS__FILE_SYSTEM__SEPERATOR_STRING);

		//REMEMBER: THE FOLLOWING WOULD BE UNSAFE IF THIS WAS NOT THE SEPERATOR CHARACTER AND
		//		THE PATH WAS NOT IN UTF8.
		tEndIndex = crx_c_string_getIndexOfFirstReverseOccuranceOf2(
				&vVerifiedRoute, crx_c_string_getSize(&vVerifiedRoute) - (vIsADirectory ? 2 : 1), 
				CRX__OS__FILE_SYSTEM__SEPERATOR_STRING, 0);

		vIsNoError = crx_c_string_appendChars(&vFullPath,
				crx_c_string_getCharsPointer(&vVerifiedRoute), tEndIndex + 1);

		if(vIsNoError && !crx_c_string_appendChars(&vEntityName, 
				crx_c_string_getCharsPointer(&vVerifiedRoute) + tEndIndex + 1, 
				crx_c_string_getSize(&vVerifiedRoute) - tEndIndex - (vIsADirectory ? 2 : 1)))
			{vIsNoError = false;}
			
		if(vIsNoError)
		{
			bool tIsEntityName = false;
			size_t vTarget_LengthResidue = 0;

			vIsNoError = crx_c_os_fileSystem_internal_updateLengthResidues(&vFullPath,
					vWasNormalized, &vLengthResidueWhenDirectory, &vLengthResidue);

			vTarget_lengthResidueWhenDirectory = vLengthResidueWhenDirectory;
			vTarget_LengthResidue = vLengthResidue;

			if(vIsNoError && !crx_c_os_fileSystem_verifyFileSystemPath(
					pContract, CRX__C__OS__FILE_SYSTEM__OPERATION__WRITE, pNewEntityName, 
					pDepthLimitOfReparsePointsOfNewFullPath, false,
					&vVerifiedPathPrefix, &vVerifiedRoute, NULL, 
					&vTarget_lengthResidueWhenDirectory, &vTarget_LengthResidue, NULL, &tIsEntityName))
				{vIsNoError = false;}
			if(vIsNoError && !tIsEntityName)
				{vIsNoError = false;}
			/*
				TODO: THE FOLLOWING NEEDS FIXING. THE CHECK AND CORRECTION SHOULD HAPPEN IN 
						crx_c_os_fileSystem_verifyFileSystemPath() BUT IT WOULD REQUIRE THAT
						FUNCTION TO SET &tIsEntityName TO true FOR A PATH LIKE "folder/". CURRENTLY
						IF WE APPEND '/' OURSELVES TO tIsEntityName BEFORE CALLING
						crx_c_os_fileSystem_verifyFileSystemPath(), THE VERIFICATION WOULD WORK
						CORRECTLY SUCH AS WE DO NOT THE FOLLOWING CHECK, BUT WILL NOT WORK CORRECTLY
						SUCH AS tIsEntityName WOULD NO LONGER BE VALID BECAUSE IT WOULD BE false
						WHEN IT IS EXPECTED TO BE true.
			*/
			if(vIsNoError && vIsADirectory && (vTarget_lengthResidueWhenDirectory < 1))
				{vIsNoError = false;}
			else
			{
				vTarget_lengthResidueWhenDirectory = vTarget_lengthResidueWhenDirectory - 1;
				vTarget_LengthResidue = vTarget_LengthResidue - 1;
			}
			
			if(vIsNoError && !crx_c_string_appendCString(&vNewEntityName, &vVerifiedRoute))
				{vIsNoError = false;}
		}
	}

	if(vIsNoError)
	{
		Crx_C_String tTargetFullPath /*= ?*/;
		size_t tByteLength = crx_c_string_getSize(&vFullPath);
		
		crx_c_string_construct(&tTargetFullPath);

		if(vIsNoError && ((pContractApplicationLevel == 2) || (pContractApplicationLevel == 3)) &&
				!crx_c_string_appendString(&tTargetFullPath, &vFullPath))
			{vIsNoError = false;}

		if(vIsNoError && !crx_c_string_appendString(&vFullPath, &vEntityName))
			{vIsNoError = false;}
		if(vIsNoError && vIsADirectory && !crx_c_string_appendCString(&vFullPath,
				CRX__OS__FILE_SYSTEM__SEPERATOR_STRING))
			{vIsNoError = false;}

		if(vIsNoError && !crx_c_string_appendString(&tTargetFullPath, &vNewEntityName))
			{vIsNoError = false;}
		if(vIsNoError && vIsADirectory && !crx_c_string_appendCString(&tTargetFullPath,
				CRX__OS__FILE_SYSTEM__SEPERATOR_STRING))
			{vIsNoError = false;}

		if(pContractApplicationLevel == 1)
		{
			vIsNoError = crx_c_os_fileSystem_internal_isEntityNotInUse(
					pModeForCheckingFileNotInUse, &vFullPath, NULL, NULL, 0, 0, false);
		}
		else if((pContractApplicationLevel == 2) || (pContractApplicationLevel == 3))
		{
			/*
				BECAUSE RENAMING A DIRECTORY AFFECTS THE PATH OF ENTITIES THAT ARE IN IT,
				THE FOLLOWING CONSIDERS THE OPERATION ON SUCH ENTITIES AS A WRITE OPERATION EVEN
				THOUGH THIS IS NOT TRUE. THIS AMBIGUOTY IS ONE REASON THE CONTRACT APPLICATION LEVEL
				EXISTS THE WAY IT DOES, MEANING INSTEAD OF BEING A PROPER CONTRACT.
				
				REMEMBER RENAMING A DIRECTORY CAN AFFECT THE LENGTH OF PATH OF THE ENTITIES IN IT.
			*/
			vIsNoError = crx_c_os_fileSystem_internal_isEntityNotInUse(
					pModeForCheckingFileNotInUse, &vFullPath, &tTargetFullPath,
					&vTarget_lengthResidueWhenDirectory, &vTarget_LengthResidue, pContract,
					false /*RENAMING IS ONLY A WRITE OPERATION.*/);
		}
		else
			{assert(false);}

		crx_c_string_removeChars(&vFullPath, tByteLength, 
				crx_c_string_getSize(&vFullPath) - tByteLength);

		crx_c_string_destruct(&tTargetFullPath);
	}

	if(vIsNoError && (crx_c_os_fileSystem_internal_rename(&vFullPath, &vEntityName, 
			pNewEntityName) != 0))
		{vIsNoError = false;}

	crx_c_string_destruct(&vVerifiedPathPrefix);
	crx_c_string_destruct(&vVerifiedRoute);
	crx_c_string_destruct(&vFullPath);
	crx_c_string_destruct(&vEntityName);
	crx_c_string_destruct(&vNewEntityName);

	return vIsNoError;
}
/*
	WARNING: READ CAREFULY. IF 
		- THE RETURN IS true, pNewEntityName HOLDS THE NEW NAME THAT WAS PASSED.
		- THE RETURN IS false AND pNewEntityName HOLDS THE NEW NAME THAT WAS PASSED, THE FUNCTION 
				FAILED.
		- THE RETURN IS false, AND pNewEntityName DOES NOT HOLD THE SAME NAME THAT WAS PASSED
				THE FUNCTION FAILED, BUT THE ALOGIRTHM OF THE FUNCTION RENAMED SOME OTHER ENTITY TO
				SOMETHING TEMPORARY AND FAILED TO RENAME IT BACK TO THE ORIGINAL NAME.
				CONSIDER TWO FILES, car.txt AND Car.txt, AND THE RECORD IS FOR car.txt, AND THE
				UNDERLYING SYSTEM IS CASE INSENSITIVE. THE ALGORITHM ENDED UP DOING A RENAME ON 
				Car.txt, AND THEN WAS UNABLE TO RENAME IT BACK BECAUSE THE SYSTEM NOW SEES car.txt
				AND HENCE THINKS THAT THE FILE EXISTS.
*/
bool crx_c_os_fileSystem_rename2(Crx_C_Os_FileSystem_Iterator_Record * CRX_NOT_NULL pRecord, 
		Crx_C_String * CRX_NOT_NULL pNewEntityName,
		Crx_C_Os_FileSystem_Contract const * CRX_NOT_NULL pContract, 
		uint32_t pModeForCheckingFileNotInUse, uint32_t pContractApplicationLevel)
{
	Crx_C_String vTargetFullPath /*= ?*/
	Crx_C_String vEntityName /*= ?*/;
	Crx_C_String vNewEntityName /*= ?*/;
	bool vWasNormalized = false;
	bool vIsADirectory = false;
	size_t vTarget_lengthResidueWhenDirectory = 0;
	size_t vTarget_lengthResidue = 0;
	Crx_C_String vTemporaryName /*= ?*/;
	
	crx_c_string_copyConstruct(vTargetFullPath, 
			crx_c_os_fileSystem_iterator_record_getInternalFullPath(pRecord));
	crx_c_string_construct(&vEntityName);
	crx_c_string_construct(&vNewEntityName);
	crx_c_string_construct(&vTemporaryName);
	
	vIsNoError = (crx_c_string_getSize(&vTargetFullPath) == crx_c_string_getSize(
			crx_c_os_fileSystem_iterator_record_getInternalFullPath(pRecord)));
	
	if(vIsNoError)
	{
		size_t tEndIndex = 0;
		Crx_C_String tVerifiedPathPrefix /*= ?*/;
		Crx_C_String tVerifiedRoute /*= ?*/;
		
		crx_c_string_construct(&tVerifiedPathPrefix);
		crx_c_string_construct(&tVerifiedRoute);

		vIsADirectory = crx_c_string_isEndingWith2(vTargetFullPath, 
				CRX__OS__FILE_SYSTEM__SEPERATOR_STRING);

		//REMEMBER: THE FOLLOWING WOULD BE UNSAFE IF THIS WAS NOT THE SEPERATOR CHARACTER AND
		//		THE PATH WAS NOT IN UTF8.
		tEndIndex = crx_c_string_getIndexOfFirstReverseOccuranceOf2(
				&vTargetFullPath, crx_c_string_getSize(&vTargetFullPath) - (vIsADirectory ? 2 : 1), 
				CRX__OS__FILE_SYSTEM__SEPERATOR_STRING, 0);

		vIsNoError = crx_c_string_appendChars(&vEntityName, 
				crx_c_string_getCharsPointer(&vTargetFullPath) + tEndIndex + 1, 
				crx_c_string_getSize(&vTargetFullPath) - tEndIndex - (vIsADirectory ? 2 : 1));

		crx_c_string_removeChars(&vTargetFullPath, tEndIndex + 1,
				crx_c_string_getSize(&vTargetFullPath) - tEndIndex - 1);

		//REMEMBER: THE PATH IN THE RECORD MUST HAVE BEEN CHECKED ALREADY AGAINST A DEPTH LIMIT OF
		//		REPARSE POINTS, HENCE THE USE OF ((size_t)(-1))
		//NOTICE HOW WE ARE VERIFYING THE PARENT PATH UNDER A WRITE OPERATION, AND NOT THE
		//		PATH ITSELF. IN OTHER WORDS, THIS DOES NOT CONTRADICT THE PURPOSE OF A RECORD. 
		//		HOWEVER IT DOES MEAN THAT WHEN A USER WISHES TO FIX A PATH USING THE RECORDS SYSTEM, 
		//		HE HAS TO START FROM THE TOP.
		if(vIsNoError && !crx_c_os_fileSystem_verifyFileSystemPath(
				pContract, CRX__C__OS__FILE_SYSTEM__OPERATION__WRITE, &vTargetFullPath, 
				((size_t)(-1)), false, &tVerifiedPathPrefix, &tVerifiedRoute, NULL, 
				&vTarget_lengthResidueWhenDirectory, &vTarget_lengthResidue, NULL, NULL))
			{vIsNoError = false;}

		if(vIsNoError)
		{
			bool tIsEntityName = false;

			if(vIsNoError && !crx_c_os_fileSystem_verifyFileSystemPath(
					pContract, CRX__C__OS__FILE_SYSTEM__OPERATION__WRITE, pNewEntityName, 
					0, false, &tVerifiedPathPrefix, &tVerifiedRoute,
					NULL, &vTarget_lengthResidueWhenDirectory, 
					&vTarget_lengthResidue, NULL, &tIsEntityName))
				{vIsNoError = false;}
			if(vIsNoError && !tIsEntityName)
				{vIsNoError = false;}

			/*
				TODO: THE FOLLOWING NEEDS FIXING. THE CHECK SHOULD HAPPEN IN 
						crx_c_os_fileSystem_verifyFileSystemPath() BUT IT WOULD REQUIRE THAT
						FUNCTION TO SET &tIsEntityName TO true FOR A PATH LIKE "folder/". CURRENTLY
						IF WE APPEND '/' OURSELVES TO tIsEntityName BEFORE CALLING
						crx_c_os_fileSystem_verifyFileSystemPath(), THE VERIFICATION WOULD WORK
						CORRECTLY SUCH AS WE DO NOT THE FOLLOWING CHECK, BUT WILL NOT WORK CORRECTLY
						SUCH AS tIsEntityName WOULD NO LONGER BE VALID BECAUSE IT WOULD BE false
						WHEN IT IS EXPECTED TO BE true.
			*/
			if(vIsNoError && (crx_c_os_fileSystem_iterator_record_getEntryType(pRecord) == 
					CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY_TYPE__DIRECTORY) &&
					(vTarget_lengthResidueWhenDirectory < 1))
				{vIsNoError = false;}
			else
			{
				vTarget_lengthResidueWhenDirectory = vTarget_lengthResidueWhenDirectory - 1;
				vTarget_lengthResidue = vTarget_lengthResidue - 1;
			}

			if(vIsNoError && !crx_c_string_appendCString(&vNewEntityName, &tVerifiedRoute))
				{vIsNoError = false;}
		}
		
		crx_c_string_destruct(&tVerifiedPathPrefix);
		crx_c_string_destruct(&tVerifiedRoute);
	}
	
	if(vIsNoError)
	{
		size_t tByteLength = crx_c_string_getSize(&vTargetFullPath);

		vIsNoError = crx_c_string_appendString(&vTargetFullPath, &vNewEntityName);
		if(vIsNoError && (crx_c_os_fileSystem_iterator_record_getEntryType(pRecord) == 
				CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY_TYPE__DIRECTORY) && 
				!crx_c_string_appendCString(&vTargetFullPath, 
				CRX__OS__FILE_SYSTEM__SEPERATOR_STRING))
			{vIsNoError = false;}

		if(pContractApplicationLevel == 1)
		{
			vIsNoError = crx_c_os_fileSystem_internal_isEntityNotInUse2(&vTemporaryName,
					pModeForCheckingFileNotInUse, 
					crx_c_os_fileSystem_iterator_record_getInternalFullPath(pRecord), 
					pRecord, pContract, NULL, 0, 0, false);
		}
		else if((pContractApplicationLevel == 2) || (pContractApplicationLevel == 3))
		{
			/*
				BECAUSE RENAMING A DIRECTORY AFFECTS THE PATH OF ENTITIES THAT ARE IN IT,
				THE FOLLOWING CONSIDERS THE OPERATION ON SUCH ENTITIES AS A WRITE OPERATION EVEN
				THOUGH THIS IS NOT TRUE. THIS AMBIGUOTY IS ONE REASON THE CONTRACT APPLICATION LEVEL
				EXISTS THE WAY IT DOES, MEANING INSTEAD OF BEING A PROPER CONTRACT.
				
				REMEMBER RENAMING A DIRECTORY CAN AFFECT THE LENGTH OF PATH OF THE ENTITIES IN IT.
			*/
			vIsNoError = crx_c_os_fileSystem_internal_isEntityNotInUse2(&vTemporaryName,
					pModeForCheckingFileNotInUse, 
					crx_c_os_fileSystem_iterator_record_getInternalFullPath(pRecord), 
					pRecord, pContract, &vTargetFullPath, 
					vTarget_lengthResidueWhenDirectory, vTarget_lengthResidue, 
					false /*RENAMING IS ONLY A WRITE OPERATION.*/);
		}
		else
			{assert(false);}

		crx_c_string_removeChars(&vTargetFullPath, tByteLength, 
				crx_c_string_getSize(&vTargetFullPath) - tByteLength);

		if(!vIsNoError && !crx_c_string_isEmpty(&vTemporaryName))
		{
			if(!crx_c_string_tryToPilferSwapContentWith(&vTemporaryName, pNewEntityName))
			{
				crx_c_string_destruct(pNewEntityName);
				memcpy(pNewEntityName, &vTemporaryName, sizeof(Crx_C_String));
				crx_c_string_construct(&vTemporaryName);
			}
		}
	}

	if(vIsNoError && (crx_c_os_fileSystem_internal_rename(&vTargetFullPath, &vTemporaryName, 
			&vNewEntityName) != 0))
		{vIsNoError = false;}

	crx_c_string_destruct(&vTargetFullPath);
	crx_c_string_destruct(&vEntityName);
	crx_c_string_destruct(&vNewEntityName);
	crx_c_string_destruct(&vTemporaryName);

	return vIsNoError;
}

/*
	NOTE: IF AN ERROR IS ENCOUNTERED, THIS FUNCTION RETURNS false ALSO. IN OTHER WORDS, AN ERROR
	IS TREATED AS ENTITY IS BEING USED. ERRORS INCLUDE VIOLATIONS PER THE PASSED CONTRACT.

	NOTE: CHECKS ARE DONE AGAINST THE CONTRACT PER THE CONTRACT APPLICATION LEVEL.
	
	pContractApplicationLevel:
		1: CHECKS ARE DONE AGAINST THE CONTRACT FOR THE PATH THAT IS PASSED ONLY.
		2: CHECKS ARE DONE AGAINST THE CONTRACT FOR PATHS IN THE BRANCH IF THE PATH ARE BEING
				USED FOR WRITE OPERATIONS. NOTE HOW THIS IS ONLY APPLICABLE WHEN PASSING A 
				DIRECTORY FULL PATH.
		3: CHECKS ARE DONE AGAINST THE CONTRACT FOR PATHS IN THE BRANCH IF THE PATH ARE BEING
				USED FOR READ OPERATIONS. NOTE HOW THIS IS ONLY APPLICABLE WHEN PASSING A 
				DIRECTORY FULL PATH.
				
	NOTE HOW THE CONTRACT APPLICATION LEVEL, pContractApplicationLevel, ONLY HAS AN EFFECT IN THIS 
	FUNCTION IF THE LEVEL IS SET TO 3.

	pMode:
		0: CHECKS FOR OPEN FILES IS NOT REQUIRED. DO NOT CHECK.
		1: CHECKS FOR OPEN FILES IS ONLY REQUIRED IF DEEMED NECESSARY. THIS HAPPENS WHEN THE 
				OPERATING SYSTEM ALLOWS REMOVAL OR DELETION OF FILES AND DIRECTORIES THAT ARE
				OPEN IN SUCH A WAY THAT IT CAN CAUSE APPLICATIONS THAT HAVE THESE OPEN TO CRASH.
		2: CHECKS FOR OPEN FILES IS REQUIRED. THE CHECKING ALGORITHM MUST BE SUCH AS ALL OPEN
				HANDLES REMAIN VALID AT ALL TIME. IN OTHER WORDS, THE CHECKING ALGORITHM MUST
				CAUSE NOT THE HANDLES TO BECOME INVLID EVEN IF JUST TEMPORARILY.
		3: CHECKS FOR OPEN FILES IS REQUIRED. THE CHECKING ALGORITHM MUST BE SUCH AS ALL OPEN
				HANDLES AND PATHS TO OPEN FILES REMAIN VALID AT ALL TIME. IN OTHER WORDS, THE 
				CHECKING ALGORITHM MUST CAUSE NOT THE HANDLES NOR PATHS TO BECOME INVLID EVEN IF 
				JUST TEMPORARILY.

	IF pMode IS 0, THE FUNCTION MIGHT STILL LOOP OVER A DIRECTORY DEPENDING ON THE VALUE OF 
	pContractApplicationLevel. IN THIS FUNCTION, THIS ONLY HAPPENS IF pContractApplicationLevel 
	IS 3.	
*/
bool crx_c_os_fileSystem_isEntityNotInUse(uint32_t pMode,
		Crx_C_String const * CRX_NOT_NULL pFullPath, uint32_t pDepthLimitOfReparsePoints,
		Crx_C_Os_FileSystem_Contract const * CRX_NOT_NULL pContract,
		uint32_t pContractApplicationLevel)
{
	//WE STILL NEED TO VERIFY AGAINST THE CONTRACT
	Crx_C_String vVerifiedPathPrefix /*= ?*/;
	Crx_C_String vVerifiedRoute /*= ?*/;
	bool vReturn = true;
	
	crx_c_string_construct(&vVerifiedPathPrefix);
	crx_c_string_construct(&vVerifiedRoute);
	
	if(crx_c_os_fileSystem_verifyFileSystemPath(
			pContract, CRX__C__OS__FILE_SYSTEM__OPERATION__READ
			pFullPath, pDepthLimitOfReparsePoints, false,
			&vVerifiedPathPrefix, &vVerifiedRoute,
			NULL, NULL, NULL, NULL, NULL))
	{
		Crx_C_Os_FileSystem_Existance tExistance = CRX__C__OS__FILE_SYSTEM__EXISTANCE__NULL;

		if(crx_c_os_fileSystem_verifyFileSystemPath2(pContract, 
				CRX__C__OS__FILE_SYSTEM__OPERATION__READ, NULL, pDepthLimitOfReparsePoints, 
				&tExistance, &vVerifiedPathPrefix, &vVerifiedRoute, NULL))
		{
			Crx_C_String tFullPath /*= ?*/;
			Crx_C_String tIsNoError = true;

			crx_c_string_construct(&tFullPath);
			
			tIsNoError = crx_c_string_appendString(&tFullPath, &vVerifiedPathPrefix) &&
					crx_c_string_appendString(&tFullPath, &vVerifiedRoute);

			if(tIsNoError)
			{
				if(tExistance == CRX__C__OS__FILE_SYSTEM__EXISTANCE__EXISTS)
				{
					vReturn = crx_c_os_fileSystem_internal_isEntityNotInUse(pMode, &tFullPath, 
							NULL, 0, 0, NULL, (pContractApplicationLevel == 3));
				}
				else if(tExistance != CRX__C__OS__FILE_SYSTEM__EXISTANCE__DOES_NOT_EXIST)
				{
					assert((tExistance == CRX__C__OS__FILE_SYSTEM__EXISTANCE__SLOT_OCCUPIED) ||
							(tExistance == CRX__C__OS__FILE_SYSTEM__EXISTANCE__AMBIGUOUS));

					vReturn = false;
				}
			}
			else
				{vReturn = false;}

			crx_c_string_destruct(&tFullPath);
		}
		else
			{vReturn = false;}
	}
	else
		{vReturn = false;}

	crx_c_string_destruct(&vVerifiedPathPrefix);
	crx_c_string_destruct(&vVerifiedRoute);
	
	return vReturn;
}
/*
	WARNING: THIS FUNCTION CAN FAIL, RETURN false, AND STILL HAVE RENAMED THE FILE. THIS HAPPENS IF 
			THE FUNCTION FAILS TO RENAME BACK. THIS CAN HAPPEN IN A SLOT OCCUPIED OR AMGIOGOUS 
			EXISTANCE SITUATIONS. IF false IS RETURNED CONTINUE TO CHECK THAT pNewFileName IS NOT
			EMPTY.
*/
bool crx_c_os_fileSystem_ensureEntityNotInUse(
		Crx_C_Os_FileSystem_Iterator_Record const * CRX_NOT_NULL pRecord, uint32_t pMode,
		Crx_C_String * CRX_NOT_NULL pNewFileName)
{
	return crx_c_os_fileSystem_internal_isEntityNotInUse2(pNewFileName, pMode,
			crx_c_os_fileSystem_iterator_record_getInternalFullPath(pRecord), pRecord, NULL,
			NULL, 0, 0, false);
}

/*
	WARNING: MAKE SURE pVerifiedFullPath AND pTargetDirectoryFullPath ARE ALREADY VERIFIED
	
	REMEMBER, THE CONTRACT IN THIS FUNCTION IS NOT FOR CHECKING THE PASSED IN PATH, BUT PARTIAL
			PATHS THAT MIGHT BE ENCOUNTERED LATER.

	IF pTarget_verifiedDirectoryFullPath IS NOT NULL, ENCOUNTERED PARTIAL PATHS ARE CHECKED
			AGAINST THE CONTRACT, pContract, UNDER A WRITE OPERATION.
	IF pIsToContractVerifyForReadAlso IS true, ENCOUNTERED PARTIAL PATHS ARE CHECKED
			AGAINST THE CONTRACT, pContract, UNDER A READ OPERATION.
			
	pTarget_lengthResidueWhenDirectory AND pTarget_lengthResidue MUST BE THE VALUES RETURNED FROM
			CHECKING pTarget_verifiedDirectoryFullPath AGAINST pContract BEFORE CALLING THIS
			FUNCTION, UNDER A WRITE OPERATION
*/
bool crx_c_os_fileSystem_internal_isEntityNotInUse(uint32_t pMode, 
		Crx_C_String * CRX_NOT_NULL pVerifiedFullPath, 
		Crx_C_String const * pTarget_verifiedDirectoryFullPath, 
		size_t pTarget_lengthResidueWhenDirectory, size_t pTarget_lengthResidue,
		Crx_C_Os_FileSystem_Contract const * pContract, bool pIsToContractVerifyForReadAlso)
{
	CRX_SCOPE_META
	assert((pContract == NULL) || (pTarget_verifiedDirectoryFullPath != NULL) || 
			pIsToContractVerifyForReadAlso);

	CRX_SCOPE
	//NOTE: AN OPEN DIRECTORY IS A DIRECTORY THAT HAS A FILE IN IT THAT IS OPEN.
	uint32_t vSituation = crx_c_os_fileSystem_internal_isEntityNotInUse__getSituation();
	bool vIsADirectory = crx_c_string_isEndingWith2(pVerifiedFullPath, 
			CRX__OS__FILE_SYSTEM__SEPERATOR_STRING);
	bool vIsToDoCheckForOpen = false;
	bool vIsToDoCheckForContractOnSources = false;
	bool vIsToDoCheckForContractOnTargets = false;
	
	if(pContract != NULL)
	{
		if(vIsADirectory)
		{
			if(pTarget_verifiedDirectoryFullPath != NULL)
				{vIsToDoCheckForContractOnTargets = true;}
			if(pIsToContractVerifyForReadAlso)
				{vIsToDoCheckForContractOnSources = true;}
		}
	}
	
	if(pMode > 0)
	{
		if(pMode == 1)
		{
			if(vSituation == 1)
				{vIsToDoCheckForOpen = true;}
		}
		else if(pMode == 2)
			{vIsToDoCheckForOpen = true;}
		else if(pMode == 3)
			{vIsToDoCheckForOpen = true;}
	}
	
	if(vIsToDoCheckForOpen || vIsToDoCheckForContractOnSources || vIsToDoCheckForContractOnTargets)
	{
		return crx_c_os_fileSystem_internal_isEntityNotInUse__doCheck(pVerifiedFullPath, 
				pTarget_verifiedDirectoryFullPath, pTarget_lengthResidueWhenDirectory, pTarget_lengthResidue, 
				pContract, !vIsToDoCheckForOpen, vIsToDoCheckForContractOnSources);
	}
	CRX_SCOPE_END
}
/*
	WARNING: THE ROUTE OF pVerifiedFullPath MUST NOT BE EMPTY
	
	IF 
		- THE RETURN IS true, pReturn HOLDS THE NEW NAME OF THE ENTITY. pReturn WOULD NOT BE EMPTY
		- THE RETURN IS false, AND pReturn IS EMPTY, THE FUNCTION FAILED.
		- THE RETURN IS false, AND pReturn IS NOT EMPTY, THE FUNCTION FAILED, BUT IT ALSO FAILED
				TO RENAME BACK THE ENTITY TO THE ORIGINAL NAME.
				
	REMEMBER: THE FUNCTION SIGNATURE IS NOT REDUCED.
	
	IF pTarget_verifiedDirectoryFullPath IS NOT NULL, TARGET SUB TREE PATH ARE CHECKED AGAINST THE
			CONRACT UNDER A WRITE OPERATION.
	IF pIsToContractVerifyForReadAlso IS true, SOURCE SUB TREE PATH ARE CHECKED AGAINST THE CONTRACT
			UNDER A READ OPERATION.
			
	pTarget_lengthResidueWhenDirectory AND pTarget_lengthResidue MUST BE THE VALUES RETURNED FROM
			CHECKING pTarget_verifiedDirectoryFullPath AGAINST pContract BEFORE CALLING THIS
			FUNCTION, UNDER A WRITE OPERATION
*/
bool crx_c_os_fileSystem_internal_isEntityNotInUse2(Crx_C_String * CRX_NOT_NULL pReturn, 
		uint32_t pMode, Crx_C_String const * CRX_NOT_NULL pVerifiedFullPath,
		Crx_C_Os_FileSystem_Iterator_Record const * CRX_NOT_NULL pRecord,
		Crx_C_Os_FileSystem_Contract const * pContract, 
		Crx_C_String const * pTarget_verifiedDirectoryFullPath, 
		size_t pTarget_lengthResidueWhenDirectory, 
		size_t pTarget_lengthResidue, bool pIsToContractVerifyForReadAlso)
{
	CRX_SCOPE_META
	assert((pRecord == NULL) || CRX__ARE_POINTERS_TO_SAME_OBJECT(pVerifiedFullPath,
			crx_c_os_fileSystem_iterator_record_getInternalFullPath(pRecord), true));
	assert((pContract == NULL) || (pTarget_verifiedDirectoryFullPath != NULL) || 
			pIsToContractVerifyForReadAlso);

	CRX_SCOPE
	//NOTE: AN OPEN DIRECTORY IS A DIRECTORY THAT HAS A FILE IN IT THAT IS OPEN.
	uint32_t vSituation = crx_c_os_fileSystem_internal_isEntityNotInUse__getSituation();
	bool vIsADirectory = crx_c_string_isEndingWith2(pVerifiedFullPath, 
			CRX__OS__FILE_SYSTEM__SEPERATOR_STRING);
	Crx_C_String vVerifiedFullPath /*= ?*/;
	bool vIsToDoFirstCheckForOpen = false;
	bool vIsToDoSecondCheckForOpen = false;
	bool vIsToDoCheckForContractOnSources = false;
	bool vIsToDoCheckForContractOnTargets = false;
	Crx_C_Os_FileSystem_Contract vContract /*= ?*/;
	bool vReturn = true;

	crx_c_string_copyConstruct(&vVerifiedFullPath, pVerifiedFullPath);
	CRX__SET_TO_ZERO(Crx_C_Os_FileSystem_Contract, tContract);

	crx_c_string_empty(pReturn);
	vReturn = (crx_c_string_getSize(&vVerifiedFullPath) == crx_c_string_getSize(pVerifiedFullPath));
	
	if(pContract != NULL)
	{
		if(vIsADirectory)
		{
			if(pTarget_verifiedDirectoryFullPath != NULL)
				{vIsToDoCheckForContractOnTargets = true;}
			if(pIsToContractVerifyForReadAlso)
				{vIsToDoCheckForContractOnSources = true;}
		}
	}
	
	if(pMode > 0)
	{
		if(pMode == 1)
		{
			if(vSituation == 1)
			{
				vIsToDoFirstCheckForOpen = true;
				vIsToDoSecondCheckForOpen = true;
			}
		}
		else if(pMode == 2)
		{
			if(vSituation == 1)
			{
				vIsToDoFirstCheckForOpen = true;
				vIsToDoSecondCheckForOpen = true;
			}
			else if(vSituation == 3)
				{vIsToDoSecondCheckForOpen = true;}
		}
		else if(pMode == 3)
		{
			if(vSituation == 1)
			{
				vIsToDoFirstCheckForOpen = true;
				vIsToDoSecondCheckForOpen = true;
			}
			else if(vSituation == 3)
			{
				vIsToDoFirstCheckForOpen = true;
				vIsToDoSecondCheckForOpen = true;
			}
		}
	}

	if(vReturn && vIsToDoFirstCheckForOpen)
	{
		CRX_SCOPE_META
		assert(vIsToDoSecondCheckForOpen);

		CRX_SCOPE
		Crx_C_Os_FileSystem_Contract tContract /*= ?*/;

		CRX__SET_TO_ZERO(Crx_C_Os_FileSystem_Contract, tContract);
		
		tContract.gCharacterSet = CRX__C__OS__INFO__CHARACTER_SET__UNICODE;
		tContract.gGuaranteeLevel = 1;
		tContract.gFileSystemGuaranteeLevel = 1;
		tContract.gFileSystemRuntimeGuaranteeLevel = 1;
		tContract.gResolutionModel = CRX__C__OS__FILE_SYSTEM__RESOLUTION_MODEL__PRECISE;

		//WE WANT TO CHECK FOR ONLY 'OPEN' AT THIS POINT.
		vReturn = crx_c_os_fileSystem_internal_isEntityNotInUse__doCheck(&vVerifiedFullPath,
				NULL, 0, 0, pContract, false, false);
		CRX_SCOPE_END
	}

	if(vReturn && (pRecord != NULL))
	{
		Crx_C_String tFullPath /*= ?*/;
		Crx_C_String tName /*= ?*/;
		Crx_C_String tNewName /*= ?*/;
		size_t tStartIndex = crx_c_string_getIndexOfFirstReverseOccuranceOf2(
				pVerifiedFullPath, crx_c_string_getSize(pVerifiedFullPath) - (vIsADirectory ? 2 : 1), 
				CRX__OS__FILE_SYSTEM__SEPERATOR_STRING, 0) + 1;
		size_t tByteSizeWithoutName = 0;

		crx_c_string_construct(&tFullPath);
		crx_c_string_construct(&tName);
		crx_c_string_construct(&tNewName);
		
		if(vReturn && !crx_c_string_appendChars(&tFullPath,
				crx_c_string_getCharsPointer(pVerifiedFullPath), tStartIndex))
			{vReturn = false;}
		if(vReturn && !crx_c_string_appendChars(&tName,
				crx_c_string_getCharsPointer(pVerifiedFullPath) + tStartIndex,
				crx_c_string_getSize(pVerifiedFullPath) - tStartIndex - (vIsADirectory ? 1 : 0)))
			{vReturn = false;}

		if(vReturn && !crx_c_os_fileSystem_internal_getRandomReplacementName(&tNewName, &tName))
			{vReturn = false;}
		
		if(vReturn)
		{
			uint32_t tResult = crx_c_os_fileSystem_internal_rename(&tFullPath, &tName, &tNewName);
			uint32_t tTriesRemaining = 10;

			//TODO: REMEMBER, THIS CURRENTLY CAN LEAD TO AN INFINITE LOOP.
			while((tResult == 1) && vReturn)
			{
				if(crx_c_os_fileSystem_internal_getRandomReplacementName(&tNewName, &tName))
				{
					tResult = crx_c_os_fileSystem_internal_rename(&tFullPath, &tName, &tNewName);
					tTriesRemaining = tTriesRemaining - 1;

					//A HACK FOR NOW TO AVOID AN INFINITE LOOP.
					if(tTriesRemaining == 0)
						{break;}
				}
				else
					{vReturn = false;}
			}
			
			if(vReturn && (tResult != 0))
				{vReturn = false;}
		}
		
		if(vReturn)
		{
			size_t tByteLength = crx_c_string_getSize(&tFullPath);

			if(vReturn && !crx_c_string_appendString(&tFullPath, &tNewName))
				{vReturn = false;}

			if(vReturn)
			{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
	(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
	(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
				Crx_C_Os_FileSystem_Internal_Posix_FileHandle tFileHandle /*= ?*/;
				Crx_C_String tRemainingPartialPath /*= ?*/;

				crx_c_os_fileSystem_internal_posix_fileHandle_construct(&tFileHandle);
				crx_c_string_construct(&tRemainingPartialPath);

				if(!crx_c_os_fileSystem_internal_posix_lookAt2(&tFileHandle,
						&tFullPath, &tRemainingPartialPath))
					{vReturn = false;}
				if(vReturn && !crx_c_os_fileSystem_internal_posix_replaceOpenat(&tFileHandle,
						crx_c_string_getCString(&tRemainingPartialPath), O_RDONLY, 0))
					{vReturn = false;}
				if(vReturn && !crx_c_os_fileSystem_iterator_verifyRecordAgainst(pRecord,
						tFileHandle.gFileDescriptor))
					{vReturn = false;}

				crx_c_os_fileSystem_internal_posix_close(&tFileHandle);
					
				crx_c_os_fileSystem_internal_posix_fileHandle_destruct(&tFileHandle);
				crx_c_string_destruct(&tRemainingPartialPath);
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
				HANDLE tHandle = crx_c_os_fileSystem_private_win32_createFile(false, &tFullPath, 
						GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0,
						NULL);

				if(tHandle != INVALID_HANDLE_VALUE)
				{
					vReturn = crx_c_os_fileSystem_iterator_verifyRecordAgainst(pRecord,
							&tHandle);

					CloseHandle(tHandle);
					tHandle = INVALID_HANDLE_VALUE;
				}
				else
					{vReturn = false;}
#endif
			}
			
			crx_c_string_removeChars(&tFullPath, tByteLength, 
					crx_c_string_getSize(&tFullPath) - tByteLength);
			
			if(!vReturn)
			{
				uint32_t tResult = crx_c_os_fileSystem_internal_rename(&tFullPath, &tNewName,
						&tName);

				if(tResult != 0)
				{
					if(!crx_c_string_tryToPilferSwapContentWith(pReturn, &tNewName))
					{
						crx_c_string_destruct(pReturn);
						memcpy(pReturn, &tNewName, sizeof(Crx_C_String));
						crx_c_string_construct(&tNewName);
					}
				}
			}
			else
			{
				if(!crx_c_string_tryToPilferSwapContentWith(pReturn, &tNewName))
				{
					crx_c_string_destruct(pReturn);
					memcpy(pReturn, &tNewName, sizeof(Crx_C_String));
					crx_c_string_construct(&tNewName);
				}
			}
		}
		
		crx_c_string_destruct(&tFullPath);
		crx_c_string_destruct(&tName);
		crx_c_string_destruct(&tNewName);
	}
	
	if(vReturn && (vIsToDoSecondCheckForOpen || vIsToDoCheckForContractOnSources || 
			vIsToDoCheckForContractOnTargets))
	{
		Crx_C_Os_FileSystem_Contract tContract /*= ?*/;

		CRX__SET_TO_ZERO(Crx_C_Os_FileSystem_Contract, tContract);
		
		if(vIsToDoCheckForContractOnSources || vIsToDoCheckForContractOnTargets)
			{memcpy(&tContract, pContract, sizeof(Crx_C_Os_FileSystem_Contract));}
		else
		{
			tContract.gCharacterSet = CRX__C__OS__INFO__CHARACTER_SET__UNICODE;
			tContract.gGuaranteeLevel = 1;
			tContract.gFileSystemGuaranteeLevel = 1;
			tContract.gFileSystemRuntimeGuaranteeLevel = 1;
			tContract.gResolutionModel = CRX__C__OS__FILE_SYSTEM__RESOLUTION_MODEL__PRECISE;
		}

		vReturn = crx_c_os_fileSystem_internal_isEntityNotInUse__doCheck(&vVerifiedFullPath,
				pTarget_verifiedDirectoryFullPath, pTarget_lengthResidueWhenDirectory, 
				pTarget_lengthResidue, &tContract, !vIsToDoSecondCheckForOpen,
				vIsToDoCheckForContractOnSources);
	}

	return vReturn;
	CRX_SCOPE_END
}
/*
	NOTICE HOW THE FUNCTION DOES NOT ACCEPT LENGTH RESIDUES FOR THE SOURCE. THIS IS BECAUSE THE 
			SOURCE PATH, MEANING pVerifiedFullPath, IS USED TO INSTANTIATE AN ITERATOR, AND THAT 
			INSTANTIATION CAUSES THE PATH TO BE REVERIFIED AGAIN.

	IF pTarget_verifiedDirectoryFullPath IS NOT NULL, TARGET SUB TREE PATH ARE CHECKED AGAINST THE
			CONRACT UNDER A WRITE OPERATION.
	IF pIsToContractVerifyForReadAlso IS true, SOURCE SUB TREE PATH ARE CHECKED AGAINST THE CONTRACT
			UNDER A READ OPERATION.
*/
bool crx_c_os_fileSystem_internal_isEntityNotInUse__doCheck(
		Crx_C_String * CRX_NOT_NULL pVerifiedFullPath,
		Crx_C_String const * pTarget_verifiedDirectoryFullPath, 
		size_t pTarget_lengthResidueWhenDirectory, size_t pTarget_lengthResidue, 
		Crx_C_Os_FileSystem_Contract const * CRX_NOT_NULL pContract,
		bool pIsNotToCheckForOpened, bool pIsToDoCheckForContractOnSources)
{
	bool vIsADirectory = crx_c_string_isEndingWith2(pVerifiedFullPath, 
			CRX__OS__FILE_SYSTEM__SEPERATOR_STRING);
	bool vReturn = true;

	if(!vIsADirectory)
	{
		vReturn = crx_c_os_fileSystem_internal_isEntityNotInUse__doCheck__checkIfFileIsOpen(
				pVerifiedFullPath);
	}
	else
	{
		Crx_C_Os_FileSystem_Iterator tIterator /*= ?*/;
		Crx_C_Os_FileSystem_Contract tContract /*= ?*/; //FOR READ OPERATION ONLY
		bool tIsToDoCheckForContractOnTargets = (pTarget_verifiedDirectoryFullPath != NULL);
		bool tAreSourceAndTargetPathsEqual = (!tIsToDoCheckForContractOnTargets ? false : 
				crx_c_string_isEqualTo(pVerifiedFullPath, pTarget_verifiedDirectoryFullPath));

		CRX__SET_TO_ZERO(Crx_C_Os_FileSystem_Contract, tContract);
		
		if(pIsToDoCheckForContractOnSources)
			{memcpy(&tContract, pContract, sizeof(Crx_C_Os_FileSystem_Contract));}
		else
		{
			tContract.gCharacterSet = CRX__C__OS__INFO__CHARACTER_SET__UNICODE;
			tContract.gGuaranteeLevel = 1;
			tContract.gFileSystemGuaranteeLevel = 1;
			tContract.gFileSystemRuntimeGuaranteeLevel = 1;
			tContract.gResolutionModel = CRX__C__OS__FILE_SYSTEM__RESOLUTION_MODEL__PRECISE;
		}

		crx_c_os_fileSystem_iterator_internal_construct(&tIterator, &tContract, 
				true, tIsToDoCheckForContractOnTargets);
		
		if(crx_c_os_fileSystem_iterator_set(&tIterator, pVerifiedFullPath, UINT32_MAX, NULL))
		{
			Crx_C_String tCurrentFullPath /*= ?*/;
			size_t tByteSizeOfCurrentFullPath = 0;
			Crx_C_String tLastSeenDirectoryName /*= ?*/;
			uint32_t tDepth = 1;
			
			crx_c_string_construct(&tCurrentFullPath);
			crx_c_string_construct(&tLastSeenDirectoryName);

			vReturn = crx_c_os_fileSystem_iterator_getInternalDirectoryFullPath(&tIterator, 
					&tCurrentFullPath);
			tByteSizeOfCurrentFullPath = crx_c_string_getSize(&tCurrentFullPath);

			while(vReturn && crx_c_os_fileSystem_iterator_isValid(&tIterator))
			{
				Crx_C_Os_FileSystem_Iterator_EntryType tEntryType = 
						crx_c_os_fileSystem_iterator_getCurrentEntryType(&tIterator);
				uint32_t tNewDepth = crx_c_os_fileSystem_iterator_getCurrentDepth(&tIterator);
				
				if(tNewDepth > tDepth)
				{//tNewDepth == tDepth + 1
					tDepth = tNewDepth;

					vReturn = crx_c_string_appendString(&tCurrentFullPath, &tLastSeenDirectoryName);
					tByteSizeOfCurrentFullPath = crx_c_string_getSize(&tCurrentFullPath);

					if(vReturn && !crx_c_string_appendCString(&tCurrentFullPath, 
							CRX__OS__FILE_SYSTEM__SEPERATOR_STRING))
						{vReturn = false;}
				}
				else if(tNewDepth < tDepth)
				{
					size_t tCurrentEndIndex = crx_c_string_getSize(&tCurrentFullPath);

					while(tNewDepth < tDepth)
					{
						tDepth = tDepth - 1;

						//REMEMBER: THE FOLLOWING WOULD BE UNSAFE IF THIS WAS NOT THE SEPERATOR CHARACTER AND
						//		THE PATH WAS NOT IN UTF8.
						tCurrentEndIndex = crx_c_string_getIndexOfFirstReverseOccuranceOf2(
								&vFullPath, tCurrentEndIndex - 1, 
								CRX__OS__FILE_SYSTEM__SEPERATOR_STRING, 0);

						if((tCurrentEndIndex < pByteLengthOfRoute) || 
								(tCurrentEndIndex == ((size_t)(-1))))
						{//SHOULD NOT HAPPEN
							vReturn = false;

							break;
						}
					}
					
					if(vReturn)
					{
						crx_c_string_removeChars(&tCurrentFullPath, tCurrentEndIndex + 1,
								crx_c_string_getSize(&tCurrentFullPath) - tCurrentEndIndex - 1);

						tByteSizeOfCurrentFullPath = crx_c_string_getSize(&tCurrentFullPath);
					}
				}

				if(vReturn)
				{
					bool tIsNotAddressed = true;

					/*
						ITERATING OVER FILES THEN SUBDIRECTORIES
					*/
					if(crx_c_os_fileSystem_iterator_getCurrentRunCount(&tIterator) == 1)
					{
						if(!crx_c_os_fileSystem_iterator_internal_getCurrentIsValid(&tIterator))
						{
							if((pThis->gPrivate_contract->gResolutionModel == 
									CRX__C__OS__FILE_SYSTEM__RESOLUTION_MODEL__PRECISE_AND_SLOT_AWARE) ||
									(pThis->gPrivate_contract->gResolutionModel == 
									CRX__C__OS__FILE_SYSTEM__RESOLUTION_MODEL__SLOT_AWARE))
							{
								/*
									THE FOLLOWING WOULD BE TRUE, OTHERWISE THE ITERATION WOULD HAVE
									STOPPED WITH A FATAL ERROR.
									pThis->gPrivate_contract->gFileSystemRuntimeGuaranteeLevel == 1
								*/

								vReturn = false;
								tIsNotAddressed = false;
							}
							else
							{
								assert(pThis->gPrivate_contract->gResolutionModel == 
										CRX__C__OS__FILE_SYSTEM__RESOLUTION_MODEL__PRECISE);

								if(!tAreSourceAndTargetPathsEqual)
								{		
									//TODO: CREATE AN EMPTY FILE, OR DIRECTORY IN THE TARGET WITH
									//		pTarget_verifiedDirectoryFullPath AND THE PARTIAL PATH OF 
									//		THE CURRENT ENTITY. IF CREATION FAILS BECAUSE FILE EXISTS, 
									//		STOP WITH A FATAL ERROR SETTING vReturn TO false AND 
									//		tIsNotAddressed TO false.
									//		REMEMBER TO DELETE ALL FILES ONCE THE CHECKING IS DONE.
								}
								else
								{
									//TODO: OPEN THE FILE USING ITS FULL PATH, AND VERIFY IT AGAINST
									//		THE RECORD, THEN CLOSE IT. IF IT DID NOT VERIFY, STOP 
									//		WITH A FATAL ERROR SETTING vReturn TO false AND 
									//		tIsNotAddressed TO false. IN WINDOWS YOU COULD USE
									//		FindFirstFile INSTEAD OF OPENING. SEE CODE ELSEWHERE.
								}
							}
						}
						
						if(tIsNotAddressed && (tEntryType == 
								CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY_TYPE__DIRECTORY))
						{
							crx_c_os_fileSystem_iterator_queueThenNext(&tIterator);
							tIsNotAddressed = false;
						}
					}
					else
					{
						//NOT BEING ABLE TO CHECK IS AN ERROR
						if(crx_c_os_fileSystem_iterator_getCurrentIsBarredEntry(
								&tIterator))
						{
							//IF HERE, tEntryType WOULD BE 
							//		CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY_TYPE__DIRECTORY
							vReturn = false;
							tIsNotAddressed = false;
						}
					}

					if(tIsNotAddressed)
					{
						if((tEntryType == CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY_TYPE__DIRECTORY) ||
								(tEntryType == CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY_TYPE__FILE))
						{
							if(tEntryType == 
									CRX__C__OS__FILESYSTEM__ITERATOR__ENTRY_TYPE__DIRECTORY)
							{
								crx_c_string_copyAssignFrom(&tLastSeenDirectoryName, 
										crx_c_os_fileSystem_iterator_getCurrentName(&tIterator));
							}

							if(tIsToDoCheckForContractOnTargets)
							{
								Crx_C_String tFilePartialPath /*= ?*/;
								Crx_C_String tVerifiedPathPrefix /*= ?*/;
								Crx_C_String tVerifiedRoute /*= ?*/;
								size_t tTarget_LengthResidueWhenDirectory = 
										pTarget_lengthResidueWhenDirectory;
								size_t tTarget_LengthResidue = pTarget_lengthResidue;

								crx_c_string_construct(&tFilePartialPath);
								crx_c_string_construct(&tVerifiedPathPrefix);
								crx_c_string_construct(&tVerifiedRoute);

								vReturn = crx_c_os_fileSystem_iterator_getCurrentPartialPath(
										&tIterator, &tFilePartialPath);

								if(vReturn && !crx_c_os_fileSystem_verifyFileSystemPath(
										pContract, CRX__C__OS__FILE_SYSTEM__OPERATION__WRITE,
										&tFilePartialPath, 0, false, &tVerifiedPathPrefix, 
										&tVerifiedRoute, NULL, &tTarget_LengthResidueWhenDirectory, 
										&tTarget_LengthResidue, NULL, NULL))
									{vReturn = false;}

								if(vReturn && !crx_c_string_appendString(&tCurrentFullPath,
										crx_c_os_fileSystem_iterator_getCurrentName(
										&tIterator)))
									{vReturn = false;}
								if(vReturn &&
										!crx_c_os_fileSystem_internal_isEntityNotInUse__doCheck__checkIfFileIsOpen(
										&tCurrentFullPath))
									{vReturn = false;}
								crx_c_string_removeChars(&tCurrentFullPath, 
										tByteSizeOfCurrentFullPath,
										crx_c_string_getSize(&tCurrentFullPath) - 
										tByteSizeOfCurrentFullPath);

								crx_c_string_destruct(&tFilePartialPath);
								crx_c_string_destruct(&tVerifiedPathPrefix);
								crx_c_string_destruct(&tVerifiedRoute);
							}
						}

						if(vReturn)
							{crx_c_os_fileSystem_iterator_next(&tIterator);}

						tIsNotAddressed = false;
					}
				}
			}
			
			crx_c_string_destruct(&tCurrentFullPath);
			crx_c_string_destruct(&tLastSeenDirectoryName);
		}
		
		crx_c_os_fileSystem_iterator_destruct(&tIterator);
	}
}
bool crx_c_os_fileSystem_internal_isEntityNotInUse__doCheck__checkIfFileIsOpen(
		Crx_C_String * CRX_NOT_NULL pFullFilePath)
{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	Crx_C_Os_FileSystem_Internal_Posix_FileHandle vFileHandle /*= ?*/;
	Crx_NonCrxed_Os_Posix_Dll_Libc_Stat vStat /*= ?*/;
	Crx_C_String vRemainingPartialPath /*= ?*/;
	bool vReturn = true;

	crx_c_os_fileSystem_internal_posix_fileHandle_construct(&vFileHandle);
	CRX__SET_TO_ZERO(Crx_NonCrxed_Os_Posix_Dll_Libc_Stat, vStat);
	crx_c_string_construct(&vRemainingPartialPath);
	
	if(!crx_c_os_fileSystem_internal_posix_lookAt2(&vFileHandle,
			pFullFilePath, &vRemainingPartialPath))
		{vReturn = false;}
	if(vReturn && !crx_c_os_fileSystem_internal_posix_replaceOpenat(&vFileHandle,
			crx_c_string_getCString(&vRemainingPartialPath), O_WRONLY, 0))
		{vReturn = false;}
	if(vReturn && !crx_c_os_fileSystem_internal_posix_fstat(&vFileHandle, &vStat, true))
		{vReturn = false;}
	if(vReturn && S_ISDIR(vStat))
		{vReturn = false;}

	if(vReturn)
		{vReturn = crx_c_os_fileSystem_private_posix_flock(&vFileHandle, LOCK_EX|LOCK_NB);}
	
	crx_c_os_fileSystem_internal_posix_close(&vFileHandle);

	crx_c_os_fileSystem_internal_posix_fileHandle_destruct(&vFileHandle);
	crx_c_string_destruct(&vRemainingPartialPath);
	
	return vReturn;
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	HANDLE vHandle = crx_c_os_fileSystem_private_win32_createFile(false, pFullFilePath, 
			GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	bool vReturn = true;

	if(vHandle == INVALID_HANDLE_VALUE)
	{
		if(GetLastError() == ERROR_SHARING_VIOLATION)
			{vReturn = false;}
	}
	else
	{
		CloseHandle(vHandle);
		vHandle = INVALID_HANDLE_VALUE;
	}

	return vReturn;
#endif
}
/*
	0: UNSET
	1: A SYSTEM WHERE DELETING AN OPEN DIRECTORY OR FILE DOES IS NOT STOPPED, BUT OPEN HANDLES 
			CAN CAUSE ERRORS AN CRASHES TO THE PROGRAMS HOLDING THEM.
			
			INCLUDES: Win9x
	2: A SYSTEM WHERE DELETING AN OPEN DIRECTORY OR FILE IS NOT STOPPED, BUT OPEN HANDLES
			REMAIN VALID FOR THE PROGRAMS THAT ARE HOLDING THEM, BUT THE FILE LOCATION REMAINS
			INVALID. THIS MEANS THAT NO FILE IN STEAD OF THE DELETED FILE CAN BE CREATED UNTIL
			THE FILE IS DELETED, HOWEVER THE FILE DOES ALSO REMAIN VISIBLE IN THE FILESYSTEM
			IT SEEMS, MEANING IF YOU ITERATE, YOU WOULD SEE IT.
			
			INCLUDES: Win 2000 UP TO Windows 10 year ~2019. IN 2019, THE DEFAULT BEHAVIOR
					BECAME SUCH AS THIS IS NOT THE CASE, BUT CODE COULD STILL ASK FOR THE
					OLD BEHAVIOR
	3: A SYSTEM WHERE DELETING AN OPEN DIRECTORY OR FILE IS NOT STOPPED, BUT OPEN HANDLES
			REMAIN VALID FOR THE PROGRAMS THAT ARE HOLDING THEM.

			INCLUDES: Linux, FreeBsd, Macos
	4: A SYSTEM WHERE DELETING AN OPEN DIRECTORY OR FILE IS STOPPED.
*/
uint32_t crx_c_os_fileSystem_internal_isEntityNotInUse__getSituation()
{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	return 3;
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	Crx_C_Os_Info_OsInformation tOsInformation /*= ?*/;

	CRX__SET_TO_ZERO(Crx_C_Os_Info_OsInformation, tOsInformation);

	crx_c_os_info_bestGuessGetOsInformation(&tOsInformation);

	if(tOsInformation.gMajorVersion >= 5)
		{return 4;}
	else
		{return 1}
#endif
}

/*
	THIS IS A RENAME OPERATION AS DEFINED IN THIS DESIGN, AND NOT AS DEFINED BY VARIOUS SYSTEMS. 
			ONLY THE FILE NAME, AS DEFINED IN THE FILE SYSTEM PATH SPECIFICATION THAT IS DEFINED PER
			THE CRX URL STANDARD, IS AFFECTED. 
			
	RETURNS:
		0: SUCCESS
		1: FILE ALREADY EXISTS
		2: OTHER ERRORS.
*/
uint32_t crx_c_os_fileSystem_internal_rename(
		Crx_C_String * CRX_NOT_NULL pVerifiedParentDirectoryFullPathOrFullPath, 
		Crx_C_String const * pVerifiedName, Crx_C_String * CRX_NOT_NULL pVerifiedNewName)
{
	Crx_C_String vTarget_fullPath /*= ?*/;
	size_t vByteSize = crx_c_string_getSize(pVerifiedParentDirectoryFullPathOrFullPath);
	bool vIsNoError = true;
	uint32_t vReturn = 0;

	crx_c_string_construct(&vTarget_fullPath);

	if(pVerifiedName != NULL)
	{
		vIsNoError = crx_c_string_appendString(&vTarget_fullPath, 
				pVerifiedParentDirectoryFullPathOrFullPath);

		if(vIsNoError && !crx_c_string_appendString(&vTarget_fullPath, pVerifiedName))
			{vIsNoError = false;}
		if(vIsNoError && !crx_c_string_appendString(pVerifiedParentDirectoryFullPathOrFullPath, 
				pVerifiedName))
			{vIsNoError = false;}
	}

	if(vIsNoError)
	{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
		Crx_C_Os_FileSystem_Internal_Posix_FileHandle vFileHandle /*= ?*/;
		Crx_C_Os_FileSystem_Internal_Posix_FileHandle vTarget_fileHandle /*= ?*/;
		Crx_NonCrxed_Os_Posix_Dll_Libc_Stat vStat /*= ?*/;
		Crx_C_String vRemainingPartialPath /*= ?*/;
		Crx_C_String vTarget_remainingPartialPath /*= ?*/;
			
		crx_c_os_fileSystem_internal_posix_fileHandle_construct(&vFileHandle);
		CRX__SET_TO_ZERO(Crx_NonCrxed_Os_Posix_Dll_Libc_Stat, vStat);
		crx_c_string_construct(&vRemainingPartialPath);
		crx_c_string_construct(&vTarget_remainingPartialPath);

		if(vIsNoError && !crx_c_os_fileSystem_internal_posix_lookAt2(&vFileHandle,
				pVerifiedParentDirectoryFullPathOrFullPath, &vRemainingPartialPath))
		{
			vIsNoError = false;
			vReturn = 2;
		}
		if(vIsNoError && !crx_c_os_fileSystem_internal_posix_lookAt2(&vTarget_fileHandle,
				&vTarget_fullPath, &vTarget_remainingPartialPath))
		{
			vIsNoError = false;
			vReturn = 2;
		}
		if(vIsNoError && crx_c_os_fileSystem_internal_posix_fstatat(&vTarget_fileHandle, 
				&vTarget_remainingPartialPath, &vStat, true))//WE NEED fstat TO FAIL
		{
			vIsNoError = false;
			vReturn = 1;
		}
		if(vIsNoError && (errno != ENOENT))
		{
			vIsNoError = false;
			vReturn = 2;
		}
		if(vIsNoError && !crx_c_os_fileSystem_private_posix_renameat(&vFileHandle, 
				&vRemainingPartialPath, &vTarget_fileHandle, &vTarget_remainingPartialPath))
		{
			vReturn = false;
			vReturn = 2;
		}

		crx_c_os_fileSystem_internal_posix_close(&vFileHandle);
		crx_c_os_fileSystem_internal_posix_close(&vTarget_fileHandle);

		crx_c_os_fileSystem_internal_posix_fileHandle_destruct(&vFileHandle);
		crx_c_string_destruct(&vTarget_fullPath);
		crx_c_string_destruct(&vRemainingPartialPath);
		crx_c_string_destruct(&vTarget_remainingPartialPath);
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
		uint32_t /*DWORD*/ tFileAttributes = INVALID_FILE_ATTRIBUTES;

		/*
			QUOTE: When GetFileAttributes is called on a directory that is a 
					mounted folder, it returns the file system attributes of the 
					directory, not those of the root directory in the volume that the 
					mounted folder associates with the directory. To obtain the file 
					attributes of the associated volume, call 
					GetVolumeNameForVolumeMountPoint to obtain the name of the 
					associated volume. Then use the resulting name in a call to 
					GetFileAttributes. The results are the attributes of the root 
					directory on the associated volume.
					{SRC: https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfileattributesa?redirectedfrom=MSDN}
			THIS SHOULD BE NOT AN ISSUE HERE. IF IT ENDS UP BEING AN ISSUE, SWITCH TO 
			USING CreateFile() INSTEAD.
		*/
		tFileAttributes = crx_c_os_fileSystem_private_win32_getFileAttributes(true, 
				pVerifiedParentDirectoryFullPathOrFullPath);

		if(tFileAttributes == INVALID_FILE_ATTRIBUTES)
		{
			if(!crx_c_os_fileSystem_internal_win32_doMoveFile(false, &vTarget_fullPath,
					pVerifiedParentDirectoryFullPathOrFullPath, false))
			{
				vIsNoError = false;
				vReturn = 2;
			}
		}
		else
		{
			vIsNoError = false;
			vReturn = 1;
		}
#endif
	}

	return vReturn;
}
/*
	THE FOLLOWING RETURNS A RANDOM NAME THAT IS GUARANTEED VALID NO MATTER THE CONTRACTS AND
	OPERATION.
	TODO: ADD AN OPTIONAL HASH TABLE PARAMETER TO AVOID INFINITE LOOPS.
*/
bool crx_c_os_fileSystem_internal_getRandomReplacementName(
		Crx_C_String * CRX_NOT_NULL pReturn, Crx_C_String const * pVerifiedName)
{
	//TEMPORARILY UNTILL THE FORMAL GLOBAL VARIABLE PROBLEM IS SOLVED, THEN WE WOULD USE GLOBAL 
	//		INSTANCE AVAILABLE TO ALL AS PART OF CRX RT.
	static Crx_C_Random_SafeRandom vSafeRandom /*= ?*/;
	static char vChars[29] = 
	{
		 34 /* ! */,  35 /* # */,  36 /* $ */,  37 /* % */,  38 /* & */,  39 /* ' */,  40 /* ( */, 
		 41 /* ) */,  43 /* + */,  44 /* , */,  45 /* - */,/*46/* . * /,  48 /* 0 */,  49 /* 1 */, 
		 50 /* 2 */,  51 /* 3 */,  52 /* 4 */,  53 /* 5 */,  54 /* 6 */,  55 /* 7 */,  56 /* 8 */, 
		 57 /* 9 */,  64 /* @ */,  91 /* [ */,  93 /* ] */,  94 /* ^ */,  95 /* _ */, 123 /* { */, 
		125 /* } */, 126 /* ~ */
	};
	static bool vIsInitialized = false;
	size_t vLength = crx_c_string_getSize(pVerifiedName);

	if(!vIsInitialized)
		{crx_c_random_safeRandom_construct(&vSafeRandom);}
	
	crx_c_string_empty(pReturn);

	CRX_FOR(size_t tI = 0, tI < vLength, tI++)
	{
		if(!crx_c_string_appendChar(pReturn, vChars[(size_t)(
				crx_c_random_safeRandom_getRandomDouble(&vSafeRandom) * 28)]))
			{return false;}
	}
	CRX_ENDFOR
	
	return true;
}

Crx_C_Os_FileSystem_OsFileHandle crx_c_os_fileSystem_move(
		Crx_C_String const * CRX_NOT_NULL pFullPath, uint32_t pDepthLimitOfReparsePointsOfFullPath,
		Crx_C_String const * CRX_NOT_NULL pNewFullPath, 
		uint32_t pDepthLimitOfReparsePointsOfNewFullPath, 
		Crx_C_Os_FileSystem_Contract const * CRX_NOT_NULL pContract)
{
	Crx_C_String vVerifiedPathPrefix /*= ?*/;
	Crx_C_String vVerifiedRoute /*= ?*/;
	size_t vLengthResidueWhenDirectory = 0;
	size_t vLengthResidue = 0;
	
	crx_c_string_construct(&vVerifiedPathPrefix);
	crx_c_string_construct(&vVerifiedRoute);
	
	if(!crx_c_os_fileSystem_verifyFileSystemPath(
			pContract, CRX__C__OS__FILE_SYSTEM__OPERATION__READ
			pFullPath, pDepthLimitOfReparsePointsOfNewFullPath, false,
			&vVerifiedPathPrefix, &vVerifiedRoute,
			NULL, &vLengthResidueWhenDirectory, &vLengthResidue, NULL, NULL))
			
	
	crx_c_string_destruct(&vVerifiedPathPrefix);
	crx_c_string_destruct(&vVerifiedRoute);
}

bool crx_c_os_fileSystem_lockByFile(
		Crx_C_Os_FileSystem_OsFileHandle * CRX_NOT_NULL pReturn,
		Crx_C_String const * CRX_NOT_NULL pFileFullPath, 
		uint32_t pDepthLimitOfReparsePointsOfFullPath,
		Crx_C_Os_FileSystem_Contract const * CRX_NOT_NULL pContract)
{
	Crx_C_Os_FileSystem_OsFileHandle vOsFileHandle = CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE;
	Crx_C_String vString /*= ?*/;

	crx_c_string_construct(&vString);

#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	vOsFileHandle = crx_c_os_fileSystem_internal_doOpen(pFileFullPath, 
			pDepthLimitOfReparsePoints, NULL, CRX__C__OS__FILE_SYSTEM__FILE_OPEN_MODE__CREATE,
			CRX__C__OS__FILE_SYSTEM__FILE_ACCESS_MODE__NORMAL,
			CRX__C__OS__FILE_SYSTEM__FILE_SHARE_MODE__EXCLUSIVE, pContract, false, &vString);
	//still need to write.		
	
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	vOsFileHandle = crx_c_os_fileSystem_internal_doOpen(pFileFullPath, pDepthLimitOfReparsePoints, 
			NULL, CRX__C__OS__FILE_SYSTEM__FILE_OPEN_MODE__FORCE_OPEN,
			CRX__C__OS__FILE_SYSTEM__FILE_ACCESS_MODE__NORMAL,
			CRX__C__OS__FILE_SYSTEM__FILE_SHARE_MODE__EXCLUSIVE, pContract, true, &vString);
#endif

	if(vOsFileHandle != CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE)
		{crx_c_os_fileSystem_byFileKey_internal_set(pReturn, vOsFileHandle, &vString)}
				
	crx_c_string_destruct(&vString);
	
	return (vOsFileHandle != CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE);
}
void crx_c_os_fileSystem_unlockByFile(Crx_C_Os_FileSystem_OsFileHandle pOsFileHandle)
{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	return crx_c_os_fileSystem_internal_doOpen(pFileFullPath, pDepthLimitOfReparsePoints, NULL,
			CRX__C__OS__FILE_SYSTEM__FILE_OPEN_MODE__CREATE,
			CRX__C__OS__FILE_SYSTEM__FILE_ACCESS_MODE__NORMAL,
			CRX__C__OS__FILE_SYSTEM__FILE_SHARE_MODE__EXCLUSIVE, pContract, false);
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	return crx_c_os_fileSystem_internal_doOpen(pFileFullPath, pDepthLimitOfReparsePoints, NULL,
			CRX__C__OS__FILE_SYSTEM__FILE_OPEN_MODE__FORCE_OPEN,
			CRX__C__OS__FILE_SYSTEM__FILE_ACCESS_MODE__NORMAL,
			CRX__C__OS__FILE_SYSTEM__FILE_SHARE_MODE__EXCLUSIVE, pContract, true);
#endif
}

/*
THIS FUNCTION SHOULD BE USED TO GET DEEP PATH ON SYSTEMS THAT SUPPORT "at" FUNCTIONS. THE FUNCTION
WOULD USE openat() TO FIND THE DEEP PATH UNTIL GETTING TO THE FINAL FILE DESCRIPTOR AND RETURN IT.
THIS FUNCTION IS FOR INTERNAL USE ONLY. OTHERS FUNCTION THEN RELY ON OTHER "at" FUNCTIONS TO DO
THEIR THING.

IF
	- RETURN IS NOT -1, AND pRemainingPath IS NOT EMPTY: FILE MAY OR MAY NOT EXIST.
			BYTE_LENGTH(pFullPath) > PATH_MAX
	- RETURN IS -1, AND pRemainingPath IS NOT EMPTY: FILE MAY OR MAY NOT EXIST.
			BYTE_LENGTH(pFullPath) <= PATH_MAX
	- RETURN IS -1, AND pRemainingPath IS EMPTY: FILE DOES NOT EXIST OR THERE WAS AN ERROR. CHECK
			pErrorCode WHICH WOULD HAVE THE VALUE OF errno IN THAT CASE. A VALUE OF "ENOENT" 
			INDICATES FILE/DIRECTORY NOT FOUND. IF pErrorCode IS ZERO, THE ERROR IS NOT SOMETHING
			THAT COULD BE REPRESENTED BY errno.
			BYTE_LENGTH(pFullPath) > PATH_MAX

IMPORTANT: ALWAYS ASSUME THAT PATH_MAX INCLUDES THE NULL CHARACTER. THE HYPOTHETICAL BYTE_LENGTH()
		THAT IS ABOVE COUNTS THE NULL CHARACTER. REMEMBER THAT crx::c::String::getLength()
		DOES NOT, ROUGHLY SPEAKING.
		
WARNING: ONLY CHECK pErrorCode IF THE RETURN IS -1, AND pRemainingPath IS EMPTY. IN OTHER WORDS,
		IF pErrorCode IS 0, IT DOES NOT MEAN THAT THE FILE EXISTS.

FOR EXAMPLE, AN "OPEN" WOULD CALL THIS FUNCTION TO GET THE FILE DESCRIPTOR FOR THE LONG PATH. IF
THE RETURN IS -1, THEN IF pRemainingPath IS EMPTY, THIS FUNCTION FAILED. IF pErrorCode WAS PASSED 
AND IT IS "ENOENT", THE FILE DOES NOT EXIST. OTHERWISE, THERE IS SOME FAILURE. IF pRemainingPath IS
NOT EMPTY, THE FUNCTION WOULD PROCEED TO USE open() ON pRemainingPath. ON THE OTHER HAND, IF THE 
RETURN IS NOT -1, THE "OPEN" FUNCTION THEN PROCEEDS TO USE openat() WITH THE RETURN OF THIS 
FUNCTION AND WITH pRemainingPath WHIH IS GUARANTEED TO BE NOT EMPTY IN THAT CASE. OTHER FUNCTIONS 
MIGHT USE unlinkat, removeat, ETC.

ALTERNATIVELY, AN "OPEN" WOULD CHECK THE PATH AGAINST PATH_MAX AND ONLY IF LARGER, CALL THIS 
FUNCTION. IF THE RETURN IS -1, pRemainingPath IS GUARANTEED TO BE EMPTY IN THIS CASE. IF pErrorCode 
WAS PASSED AND IT IS "ENOENT", THE FILE DOES NOT EXIST. OTHERWISE, THERE IS SOME FAILURE. ON THE 
OTHER HAND, IF THE RETURN IS NOT -1, THE "OPEN" FUNCTION THEN PROCEEDS TO USE openat() WITH THE 
RETURN OF THIS FUNCTION AND WITH pRemainingPath WHIH IS GUARANTEED TO BE NOT EMPTY IN THAT CASE.
OTHER FUNCTIONS MIGHT USE unlinkat, removeat, ETC. IF "OPEN" DID NOT FIND THE NEED TO CALL THIS
FUNCTION, IT WOULD SIMPLY PROCEED TO CALL open() ON THE PATH IT HAS.

pFullPath MUST ALREADY BE VERIFIED BY crx_c_os_fileSystem_verifyFileSystemPath().

IF pFullPath DOES NOT EXISTS, THIS FUNCTION MIGHT STILL RETURN A NON EMPTY pRemainingPath, AND
A RETURN THAT IS -1. CALLER STILL NEEDS TO CHECK FOR EXISTANCE.

I THINK ORIGINALLY I MADE A MISTAKE WHEN I WROTE THIS FUNCTION, AND WAS INSTEAD AIMING TO RETURNING
THE FILE DESCRIPTOR OF THE FILE OR DIRECTORY ITSELF, AND NOT ONE LEVEL UP, AND HENCE IT DID NOT HAVE
"pRemainingPath", AND ALSO NOTE SOME COMMENTED OUT CODE BELOW.
*/
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) \ || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
Crx_C_Os_FileSystem_OsFileHandle crx_c_os_fileSystem_private_posix_lookAt(
		Crx_C_String const * CRX_NOT_NULL pFullPath,
		Crx_C_String * CRX_NOT_NULL pRemainingPath, 
		int32_t * pErrorCode /*, const char * mode*/)
{
	/*
		- openat SUPPORTED BY:
				DragonFly since DragonFly 2.3.
				FreeBSD since FreeBSD 8.0.
				Linux since Linux 2.6.16.
				NetBSD since NetBSD 7.0.
				OpenBSD since OpenBSD 5.0.
				OS X since OS X 10.10.
				Solaris.
		- ON MACOS 10.8 TO 10.9, ONE CAN USE THE FOLLOWING ALTERNATIVE;
						int openat(int dirfd, const char *pathname, int flags, ...)
						{
							int fd;

							if(fchdir(dirfd) < 0)
							{
								perror("fchdir");
								return -1;
							}

							fd = open(pathname, flags, mode);
							// chdir to the original CWD

							return fd;
						}
				fchdir() CHANGES THE WORKING DIRECTORY USING A FILE DESCRIPTOR, AND ALLOWS open TO 
				WORK RELATIVE TO THAT IF PASSED A RELATIVE PATH. HOWEVER, fchdir() AFFECTS THE
				ENTIRE PROGRAM, AND EVEN ONE WAS TO USE LOCKS, THERE IS NO GUARANTEE THAT USER CODE
				WOULD NOT CALL fchdir() DIRECTLY.
				USING nftw() TO TRAVERSE THE DIRECTORY TREE UNTIL THE FILE IN QUESTION IS FOUND IS
				ALSO NOT SAFE BECAUSE IT ALSO USES fchdir()/chdir(). HENCE WHILE AVAILABLE IN MACOS
				10.7, IT IS NOT USEFUL.
	*/
	static bool vIsSet = false;
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Openat vFunc_openat /*= ?*/;
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Open vFunc_open /*= ?*/;
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Fstatat vFunc_fstatat /*= ?*/;
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Close vFunc_close /*= ?*/;
			
	if(!vIsSet)
	{
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX)
		//LINUX EXPOSED A DIFFERENT SYMBOL IN THE PAST, __fxstatat, AND VERSIONED IT.
		vFunc_openat = crx_c_os_dll_loadSystemFunction("openat", 2, 4, 0, 0, 0);
		vFunc_open = crx_c_os_dll_loadSystemFunction("open", 2, 2, 5, 0, 0);
		if(crx_c_os_dll_loadSystemFunction("__fxstatat", 2, 4, 0, 0, 0) != NULL)
			{vFunc_fstatat = &crx_c_os_fileSystem_private_posix_linux_fstatat;}
		else
			{vFunc_fstatat = NULL;}
		vFunc_close = crx_c_os_dll_loadSystemFunction("close", 2, 2, 5, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
		vFunc_openat = crx_c_os_dll_loadSystemFunction("openat", 1, 1, 0, 0, 0);
		vFunc_open = crx_c_os_dll_loadSystemFunction("open", 1, 0, 0, 0, 0);
		vFunc_fstatat = crx_c_os_dll_loadSystemFunction("fstatat", 1, 1, 0, 0, 0);
		vFunc_close = crx_c_os_dll_loadSystemFunction("close", 1, 0, 0, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
		vFunc_openat = crx_c_os_dll_loadSystemFunction("openat", 10, 10, 0, 0, 0);//MACOS >= 10.10
		vFunc_open = ((Crx_NonCrxed_Os_Posix_Dll_Libc_Open)&open);
		vFunc_fstatat = crx_c_os_dll_loadSystemFunction("fstatat", 10, 10, 0, 0, 0);//MACOS >= 10.10
		vFunc_close = ((Crx_NonCrxed_Os_Posix_Dll_Libc_Open)&close);
	#endif
	
		vIsSet = true;
	}
	

	crx_c_string_empty(pRemainingPath);
	
	if(pErrorCode != NULL)
		{*pErrorCode = 0;}

	if(crx_c_string_getSize(pFullPath) >= PATH_MAX)
	{
		if(vFunc_openat != NULL)
		{
			Crx_C_String tFullPath /*= ?*/;
			Crx_C_Os_FileSystem_OsFileHandle tReturn = -1;

			crx_c_string_copyConstruct(&tFullPath, pFullPath);

			if(crx_c_string_getSize(&tFullPath) == crx_c_string_getSize(pFullPath))
			{
				char * tChars = crx_c_string_getCString(&tFullPath);
				char * tLastChar = tChars + crx_c_string_getSize(&tFullPath) - 1;
				size_t tRemainingLength = crx_c_string_getSize(&tFullPath);
				int tFileDescriptor = -1;
				bool tIsFirst = true;
				bool tIsNoError = true;
				
				while((*tLastChar != 47 /* / */) && (tLastChar != tChars))
					{tLastChar--;}
				
				if(tLastChar == tChars)
					{tIsNoError = false;}
				
				tRemainingLength = tLastChar - tChars;

				while(tIsNoError && (tRemainingLength >= PATH_MAX))
				{
					char * tChars2 = tChars + PATH_MAX - 2;/*== tChars + (PATH_MAX - 1) - 1*/

					tRemainingLength = tRemainingLength - (PATH_MAX - 1);

					while((*tChars2 != 47 /* / */) && (tChars2 != tChars))
					{
						tChars2--;
						tRemainingLength++;
					}
					
					if(tChars2 != tChars)
					{
						*tChars2 = '\0';

						if(tIsFirst)
						{
							tFileDescriptor = (*vFunc_open)(tChars, 
									crx_c_os_fileSystem_internal_posix_amendOpenatFlags(0));

							if(tFileDescriptor < 0)
							{
								//ERR("open: %s: %s\n", tChars, strerror(errno));
								tIsNoError = false;

								if(pErrorCode != NULL)
									{*pErrorCode = errno;}
							}
							
							tIsFirst = false;
						}
						else
						{
							int tFileDescriptor2 = (*vFunc_openat)(tFileDescriptor, tChars, 
									crx_c_os_fileSystem_internal_posix_amendOpenatFlags(0));
							
							(*vFunc_close)(tFileDescriptor);
							
							if(tFileDescriptor2 < 0)
							{
								//ERR("openat: %s: %s\n", tChars, strerror(errno));

								tIsNoError = false;

								if(pErrorCode != NULL)
									{*pErrorCode = errno;}
							}
							else
								{tFileDescriptor = tFileDescriptor2;}
						}

						*tChars2 = '/';

						if(tIsNoError)
							{tChars = tChars2 + 1;}
					}
					else
						{tIsNoError = false;}
				}

				if(tIsNoError)
				{
					assert(tRemainingLength > 1);

					/*if(tRemainingLength > 0)
					{
						/* strlen(tChars) < PATH_MAX,
						 * tChars points to the last chunk and
						 * tFileDescriptor is the directory to base the real fopen
						 * /
						int tFileDescriptor2 = -1;
						
						tLastChar = '\0';
						
						tFileDescriptor2 = (*vFunc_openat)(tFileDescriptor, tChars, O_PATH/*O_RDONLY* /); /* this needs to be
											* adjusted, based on what is
											* specified in string mode *//*<= THIS IS NO LONGER REQUIRED* /
						(*vFunc_close)(tFileDescriptor);

						if(tFileDescriptor2 < 0)
						{
							//fprintf(stderr, "openat: %s: %s\n", work_name, strerror(errno));

							tIsNoError = false;
						}
						else
							{tFileDescriptor = tFileDescriptor2;}

						//tReturn = fdopen(tFileDescriptor2, mode);
						//tReturn = tReturn;
					}*/
					
					crx_c_string_appendCArray(pRemainingPath, tLastChar + 1, tRemainingLength - 1);
				}
			}	

			crx_c_string_destruct(&tFullPath);

			return tFileDescriptor;
		}
		else
			{return -1;}
    }
	else
	{
		crx_c_string_append(pRemainingPath, pFullPath);

		return -1; /*return (*vFunc_open)(filename, 
				crx_c_os_fileSystem_internal_posix_amendOpenatFlags(mode));*/
	}
}
#endif

#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX)
int32_t crx_c_os_fileSystem_private_posix_linux_fstatat(
		int32_t dirfd, char const * pathname, Crx_NonCrxed_Os_Posix_Dll_Libc_Stat * buf, 
		int32_t flags)
{
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Fstatat vFunc_fxstatat = 
			crx_c_os_dll_loadSystemFunction("__fxstatat", 2, 4, 0, 0, 0);

	//REMEMBER: WE DELIBRATELY DO NOT CHECK FOR NULL.
	#if defined (__aarch64__)
		//#define _STAT_VER 0
		return (*vFunc_fxstatat)(0, dirfd, pathname, buf, flags);
	#elif defined (__x86_64__)
		//#define _STAT_VER 1
		return (*vFunc_fxstatat)(1, dirfd, pathname, buf, flags);
	#else
		//#define _STAT_VER 3
		return (*vFunc_fxstatat)(3, dirfd, pathname, buf, flags);
	#endif
}
Crx_C_Os_FileSystem_OsFileHandle crx_c_os_fileSystem_private_posix_linux_stat(
		char const * pathname, Crx_NonCrxed_Os_Posix_Dll_Libc_Stat * buf)
{
	static Crx_NonCrxed_Os_Posix_Dll_Libc_StatFunc vFunc_stat = 
			crx_c_os_dll_loadSystemFunction("__xstat", 2, 2, 5, 0, 0);

	#if defined (__aarch64__)
		#define _STAT_VER 0
		return (*vFunc_stat)(0, pathname, buf);
	#elif defined (__x86_64__)
		#define _STAT_VER 1
		return (*vFunc_stat)(1, pathname, buf);
	#else
		#define _STAT_VER 3
		return (*vFunc_stat)(3, pathname, buf);
	#endif
}
Crx_C_Os_FileSystem_OsFileHandle crx_c_os_fileSystem_private_posix_linux_lstat(
		char const * pathname, Crx_NonCrxed_Os_Posix_Dll_Libc_Stat * buf)
{
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Lstat vFunc_lstat = 
			crx_c_os_dll_loadSystemFunction("__lxstat", 2, 2, 5, 0, 0);

	#if defined (__aarch64__)
		#define _STAT_VER 0
		return (*vFunc_lstat)(0, pathname, buf);
	#elif defined (__x86_64__)
		#define _STAT_VER 1
		return (*vFunc_lstat)(1, pathname, buf);
	#else
		#define _STAT_VER 3
		return (*vFunc_lstat)(3, pathname, buf);
	#endif
}
Crx_C_Os_FileSystem_OsFileHandle crx_c_os_fileSystem_private_posix_linux_fstat(
		int32_t dirfd, Crx_NonCrxed_Os_Posix_Dll_Libc_Stat * buf)
{
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Fstat vFunc_fxstat = 
			crx_c_os_dll_loadSystemFunction("__fxstat", 2, 2, 5, 0, 0);

	#if defined (__aarch64__)
		//#define _STAT_VER 0
		return (*vFunc_fxstat)(0, dirfd, buf);
	#elif defined (__x86_64__)
		//#define _STAT_VER 1
		return (*vFunc_fxstat)(1, dirfd, buf);
	#else
		//#define _STAT_VER 3
		return (*vFunc_fxstat)(3, dirfd, buf);
	#endif
}
#endif

/*
	NOTES: 
		- THIS FUNCTION EXTRACTS THE CRX PATH PREFIX AND THE CRX ROUTE AS CRX PATH PREFIX, CRX ROUTE 
				ARE DEFINED IN THE CRX URL STANDARD. 
		- THE PATH PREFIX IS GUARANTEED TO END WITH '/' OR '\'. THE CHOICE DEPENDS ON THE CURRENT
				TARGET OPRATING SYSTEM.
		- THE ROUTE IS GUARANTEED NOT TO START WITH '/' OR '\' IF THE PATH PREFIX IS NOT EMPTY. THIS
				IS REGARDLESS ON WHETHER pIsToNormalize__return IS TRUE OR NOT.
		- IF pIsToNormalize__return IS TRUE, THE ROUTE IS GUARANTEED THE CORRECT CHOICE BETWEEN
				'/' AND '\' GIVEN THE TARGET OPERATING SYSTEM.
	
	REMEMBER: PER THE CRX URL STANDARD, THE EXTRACTED ROUTE IS A PARTIAL FILE SYSTEM PATH. THIS 
			MEANS THAT IF IT IS EMPTY, IT IS SET TO '/' OR '\'. CODE MUST EXPLICITLY CHECK FOR
			THIS.
	WARNING: IF pIsToNormalize__return IS TRUE, AND THE CALLER NORMALIZES THE ROUTE, 
			pExtractedPrefix COMBINED WITH THE ROUTE MUST BE RECHECKED TO CONTAIN EXACTLY THE SAME
			PATH PREIX, AND IF NOT THE SAME, TO BE REJECTED. THIS SITUATION CAN HAPPEN POSIX
			BASED SYSTEMS BECAUSE OF THE AMBUITIY OF THE UNIX PARADIGNM, AS THE UNIX PARADIGM IS
			CATEGORIZED IN THE CRX URL STANDARD.
			(UPDATE: THIS IS ILL DEFINED. IN TERMS OF CRX URL, IF ON POSIX BASED SYSTEMS
			THE LONGEST MOUNT POINT IS INTERPRETED TO BE THE CRX PATH PREFIX, THE ABOVE IS 
			ESSENTIALLY A WORKAROUND TO VIOLATING THE CRX URL STANDARD WHERE A ROUTE CAN MAKE
			US ENTER ANOTHER PATH PREFIX, A CONTRADICTION. HENCE, THE SHORTEST MOUNT POINT
			IS THE CORRECT THING TO BE INTERPRETED AS THE CRX PATH PREFIX.
			
			I THINK INITIALLY I DID IT THIS WAY BECAUSE OF THE ASSUMPTION THAT A BLOCK DEVICE
			MOUNT POINT CAN NOT BE THE ANCESTOR OF A NON BLOCK DEVICE PATH, WHILE AT THE SAME
			TIME THERE WAS UNCERTAINTY WITH THE RESULT OF DETECTING A BLOCK DEVICE. THE CODE BELOW
			HOWEVER, TAKES THAT UNCERTAINTY AS RELIABLE IF THE RESULT SAYS BLOCK DEVICE. IN OTHER
			WORDS, IT ASSUMES THERE ARE NO FALSE POSITIVES, ONLY FALSE NEGATIVES. PERHAPS THIS
			ASSUMPTION CAME LATER IN MY THOUGHT PROCESS, AND THE PREVIOUS DECISION OF USING THE 
			MOUNT POINT WITH THE LONGEST PATH REMAINED.)
			
	WARNING: THIS FUNCTION STILL EFFECTIVELY TREATS BOTH '/' AND '\' AS EQUAL. I CURRENTLY KNOW NO
			SOLUTION FOR THIS. NOTE THE FOLLOWING:
		- ON POSIX, '\' IS A VALID PART OF THE ENTITY NAME. BY SIMPLY RUNNING ON LINUX, FOR EXAMPLE,
				'\' IS NOT A PATH SEPERATOR. HOWEVER, IT IS UNCLEAR WHAT WOULD HAPPEN IF THE
				UNDERLYING FILESYSTEM IS A WINDOWS ONE, SUCH AS FAT32. WOULD LINUX ALLOW '\' TO MAKE
				IT INTO THE NAME.
		- ON WINDOWS, IF THE PATH PREFIX IS "\\?\", THE PATH IS PASSED AS IS TO THE FILESYSTEM. IT 
				WOULD SEEM THAT '/' CAN END UP BEING PART OF AN ENTITY NAME. HOWEVER, THIS IS NOT
				EXPLICITLY MENTIONED ANYWHERE THAT I COULD FIND, AND I THINK THAT THIS SITUATION
				IS LIKE THAT OF LINUX FOR EXAMPLE, WHERE THE BAHVIOR DEPENDS ON THE UNDERLYING
				FILE SYSTEM. ON WINDOWS THERE SEEM TO BE WAYS TO LOAD EXT4 FOR EXAMPLE. PRIOR
				RESEARCH INDICATED THAT WINDOW'S WSL WILL BEHAVE LIKE POSIX, TREATING '/' AS THE
				SEPERATOR, AND ALLOWING '\' AS PART OF ENTITY NAMES. I AM ASSUMING IF THIS IS TRUE,
				THIS IS TRUE ON ATLEAST NTFS.
		- TO GAIN ACCESS TO A FILE THAT CONTAINS THE SLASH WHEN A SLASH IS NOT A SEPERATOR, ONE CAN
				STILL USE THE ITERATOR TO SEARCH FOR THE FILE AND AQUIRE A RECORD.
*/
bool crx_c_os_fileSystem_extractPrefixAndRoute(Crx_C_String const * CRX_NOT_NULL pTrail, 
		Crx_C_String * CRX_NOT_NULL pExtractedPrefix, Crx_C_String * CRX_NOT_NULL pExtractedRoute, 
		Crx_C_String * pExtractedOriginalPrefix, bool * pIsToNormalize__return)
{
	Crx_C_String vTrail /*= ?*/;
	bool vIsValid = true;
	bool vIsToNormalize = true;

	crx_c_string_copyConstruct(&vTrail, pTrail);
	
	crx_c_string_empty(pExtractedPrefix);
	crx_c_string_empty(pExtractedRoute);
	
	if(pExtractedOriginalPrefix != NULL)
		{crx_c_string_empty(pExtractedOriginalPrefix);}

	crx_c_os_osString_trim(&vTrail);
	crx_c_string_replaceOccurancesOfChar(&vTrail, 0, 92 /* \ */, 47 /* / */, false);

#if(CRX__NON_CRXED__OS__SETTING__IS_WINDOWS)
	if(crx_c_string_isBeginningWith2(&vTrail, "//", false))
	{
		Crx_C_String_Sub tSub /*= ?*/;
		char const * tChars = NULL;
		size_t tLength = crx_c_string_getSize(&vTrail);
		
		crx_c_string_sub_construct2(&vSub, &vTrail);

		crx_c_string_sub_leftCut(&vSub, 2);

		vIsValid = false;
		
		tChars = crx_c_string_sub_getFirstChar(&vSub);

		if((crx_c_string_sub_getLength(&vSub) > 1) && (*tChars == 63 /*?*/) &&
				(*(tChars + 1) == 47 /* / */))
		{
			Crx_C_Os_Info_OsInformation tOsInformation /*= ?*/;
			Crx_C_String_Sub tSub2 = vSub;
			size_t tAvailableLength = 0;

			CRX__SET_TO_ZERO(Crx_C_Os_Info_OsInformation, tOsInformation);

			vIsToNormalize = false;

			crx_c_os_info_bestGuessGetOsInformation(&tOsInformation);

			crx_c_string_sub_leftCut(&tSub2, 2);
			tAvailableLength = crx_c_string_sub_getLength(&tSub2);
			tChars = crx_c_string_sub_getFirstChar(&vSub2);

			while((tAvailableLength > 0) && (*tChars == 47 /* / */))
			{
				tChars++;
				tAvailableLength--;
			}

			crx_c_string_sub_leftCut(&tSub2, 
					crx_c_string_sub_getLength(&tSub2) - tAvailableLength);

			crx_c_os_osString_leftTrimSub(&tSub2);

			tAvailableLength = crx_c_string_sub_getLength(&tSub2);
			tChars = crx_c_string_sub_getFirstChar(&vSub2);
			
			if((tAvailableLength > 2) && (CRX__C__IS_CODE_POINT_ASCII_UPPER_CASE_ALPHABET(*tChars) ||
					CRX__C__IS_CODE_POINT_ASCII_LOWER_CASE_ALPHABET(*tChars)) && 
					(*(tChars + 1) == 58 /*:*/) && (*(tChars + 2) == 47 /* / */))
			{
				if(tOsInformation.gMajorVersion >= 5)
					{crx_c_string_appendCString(pExtractedPrefix, "\\\\?\\");}
				
				crx_c_string_appendChar(pExtractedPrefix, *tChars);
				crx_c_string_appendCString(pExtractedPrefix, ":\\");

				crx_c_string_sub_leftCut(&tSub2, 3);
				tChars = tChars + 3; //== crx_c_string_sub_getFirstChar(&vSub2);
				tAvailableLength = tAvailableLength - 3;//== crx_c_string_sub_getLength(&tSub2);

				while((tAvailableLength > 0) && (*tChars == 47 /* / */))
				{
					tChars++;
					tAvailableLength--;
				}
				
				crx_c_string_sub_leftCut(&tSub2, 
						crx_c_string_sub_getLength(&tSub2) - tAvailableLength);

				if(tAvailableLength > 0)
					{crx_c_string_appendSub(pExtractedRoute, &tSub2);}
				else
					{crx_c_string_appendChar(pExtractedRoute, 47 /* / */);}//92 /* \ */

				if(pExtractedOriginalPrefix != NULL)
					{crx_c_string_appendString(pExtractedOriginalPrefix, pExtractedPrefix);}

				vIsValid = true;
			}
			else if((tAvailableLength > 43) && 
					(((*tChars == 86 /*V*/) || (*tChars == 118 /*v*/)) &&
					((*(tChars + 1) == 79 /*O*/) || (*(tChars + 1) == 111 /*o*/)) &&
					((*(tChars + 2) == 76 /*L*/) || (*(tChars + 2) == 108 /*l*/)) &&
					((*(tChars + 3) == 85 /*U*/) || (*(tChars + 3) == 117 /*u*/)) &&
					((*(tChars + 4) == 77 /*M*/) || (*(tChars + 4) == 109 /*m*/)) &&
					((*(tChars + 5) == 69 /*E*/) || (*(tChars + 5) == 101 /*e*/))))
			{
				//FORMAT: \\?\Volume{b75e2c83-0000-0000-0000-602f00000000}\ 

				if(tOsInformation.gMajorVersion >= 5)
				{
					char const * tChars2 = tChars;

					tChars = tChars + 6;

					if(*tChars == 123 /*{*/)
					{
						uint32_t tCounts[5] = {8, 4, 4, 4, 12};

						tChars++;

						CRX_FOR(size_t tI = 0, tI < 5, tI++)
						{
							uint32_t tCount = tCounts[tI];

							while(vIsValid && (tCount > 0))
							{
								if(CRX__C__IS_CODE_POINT_ASCII_HEX_DIGIT(*tChars))
									{tChars++;}
								else
									{vIsValid = false;}
							}

							if(vIsValid)
							{
								if(tI != 4)
								{
									if(tChars != 45 /*-*/)
									{
										vIsValid = false;
										break;
									}
									else
										{tChars++;}
								}
							}
							else
								{break;}
						}
						CRX_ENDFOR

						if(vIsValid && (*tChars != 125 /*}*/))
							{vIsValid = false;}
					}

					if(vIsValid)
					{
						crx_c_string_sub_leftCut(&tSub2, 44);
						crx_c_os_osString_leftTrimSub(&tSub2);

						if((crx_c_string_sub_getLength(&tSub2) > 0) && 
								(crx_c_string_sub_getCharAt(&tSub2, 0) == 47 /* / */))
						{
							crx_c_string_appendCString(pExtractedPrefix, "\\\\?\\Volume");
						
							tChars2 = tChars2 + 6;
						
							crx_c_string_appendChars(pExtractedPrefix, tChars2, 38);
							crx_c_string_appendChar(pExtractedPrefix, 92 /* \ */);

							while((crx_c_string_sub_getLength(&tSub2) > 0) &&
									(crx_c_string_sub_getCharAt(&tSub2, 0) == 47 /* / */))
								{crx_c_string_sub_leftCut(&tSub2, 1);}

							crx_c_os_osString_leftTrimSub(&tSub2);

							if(crx_c_string_sub_getLength(&tSub2) > 0)
								{crx_c_string_appendSub(pExtractedRoute, &tSub2);}
							else
								{crx_c_string_appendChar(pExtractedRoute, 47 /* / */);}//92 /* \ */

							if(pExtractedOriginalPrefix != NULL)
							{
								crx_c_string_appendString(pExtractedOriginalPrefix, 
										pExtractedPrefix);
							}
						}
						else
							{vIsValid = false;}
					}
				}
				else
					{vIsValid = false;}
			}
			else
				{vIsValid = false;}
		}
		else
			{vIsValid = false;}
	}
	else
	{
		char const * tChars = crx_c_string_getCharsPointer(&vTrail);
		size_t tAvailableLength = crx_c_string_getSize(&vTrail);
		
		vIsToNormalize = true;

		if((tAvailableLength > 1) &&
				(CRX__C__IS_CODE_POINT_ASCII_UPPER_CASE_ALPHABET(*(tChars)) ||
					CRX__C__IS_CODE_POINT_ASCII_LOWER_CASE_ALPHABET(*(tChars))) &&
					(*(tChars + 1) == 58 /*:*/))
		{
			if((tAvailableLength > 2) && (*(tChars + 2) == 47 /* / */))
			{
				Crx_C_Os_Info_OsInformation tOsInformation /*= ?*/;
				Crx_C_String_Sub tSub /*= ?*/;

				CRX__SET_TO_ZERO(Crx_C_Os_Info_OsInformation, tOsInformation);
				crx_c_string_sub_construct2(&tSub, &vTrail);

				crx_c_os_info_bestGuessGetOsInformation(&tOsInformation);

				if(tOsInformation.gMajorVersion >= 5)
					{crx_c_string_appendCString(pExtractedPrefix, "\\\\?\\");}

				crx_c_string_appendChar(pExtractedPrefix, *tChars);
				crx_c_string_appendCString(pExtractedPrefix, ":\\");
				
				if(pExtractedOriginalPrefix != NULL)
				{
					crx_c_string_appendChar(pExtractedOriginalPrefix, *tChars);
					crx_c_string_appendCString(pExtractedOriginalPrefix, ":\\");
				}

				crx_c_string_sub_leftCut(&tSub, 3);

				while((crx_c_string_sub_getLength(&tSub) > 0) &&
						(crx_c_string_sub_getCharAt(&tSub, 0) == 47 /* / */))
					{crx_c_string_sub_leftCut(&tSub, 1);}

				crx_c_os_osString_leftTrimSub(&tSub);

				if(crx_c_string_sub_getLength(&tSub) > 0)
					{crx_c_string_appendSub(pExtractedRoute, &tSub);}
				else
					{crx_c_string_appendChar(pExtractedRoute, 47 /* / */);}//92 /* \ */
			}
			else
				{vIsValid = false;}
		}
		else
		{
			if(crx_c_string_isBeginningWith2(&vTrail, "//", false))
				{vIsValid = false;}
			else
			{
				if(!crx_c_string_tryToPilferSwapContentWith(pExtractedRoute, &vTrail))
				{
					crx_c_string_destruct(pExtractedRoute);
					memcpy(pExtractedRoute, &vTrail, sizeof(Crx_C_String));
					crx_c_string_construct(&vTrail);
				}
			}
		}
	}
#elif(CRX__NON_CRXED__OS__SETTING__IS_LINUX || \
		CRX__NON_CRXED__OS__SETTING__IS_MACOS || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD))

	vIsToNormalize = true;

	if(crx_c_string_isBeginningWith2(&vTrail, "//", false))
		{vIsValid = false;}
	else
	{
		if(crx_c_string_isBeginningWith2(&vTrail, "/", false))
		{
			Crx_C_Os_FileSystem_MountInfo tMounfInfo /*= ?*/;
			size_t tLength = 0;

			crx_c_os_fileSystem_mountInfo_construct(&tMounfInfo);
			
			/*
				ORIGINALY THE FOLLOWING IDENTIFIED THE CRX PATH PREFIX OF A FILE SYSTEM PATH AS
						THE LONGEST MOUNT FULL DIRECTORY PATH THAT PREFIXES THE ANALYZED FULL PATH.
						THIS LEAD TO CONTRADICTIONS. ONE IMPORTANT ONE WAS THAT APPENDED PARTIAL
						PATH CAN LEAD TO A CHANGE OF THE CRX PATH PREFIX. THIS IS A CONTRADICTION 
						PER THE CRX URL STANDARD, AND THE CRX SECURITY PROJECT.
			*/
			//tLength = crx_c_os_fileSystem_posix_getMountInformationFor(&vTrail, 1, &tMounfInfo);

			/*
				THE FOLLOWING WAS WRONG AGAIN. THE FOLLOWING WAS TO SOLVE THE ISSUE OF BIND MOUNTS 
				ON LINUX, AND POSSIBLY FREEBSD. THESE MOUNTS WOULD ALLOW A NON BLOCK DEVICE TO 
				APPEAR IN A PATH AFTER A BLOCK DEVICE MOUNT. THE FOLLOWING AIMED TO GIVE THE 
				SHORTEST MOUNT THAT IS NOT FOLLOWED BY A MOUNT THAT IS NOT A BLOCK DEVICE. HOWEVER, 
				THIS LEADS TO THE SAME CONTRADICTION THAT ARE LEAD TO BY USING THE LONGEST MOUNT 
				PATH NOT SHORTEST. IN OTHER WORDS, A PARTIAL FILESYSTEM PATH AS DEFINED IN THE 
				STANDARD, CAN CAUSE A CRX PATH PREFIX TO CHANGE, A CONTRADICTION.
				
				WITHOUT THE FOLLOWING, A NON BLOCK DEVICE CAN NOW APPEAR IN THE CRX ROUTE. 
				FURTHERMORE, BIND MOUNTS CAN NOT BE DISTINGUISHED FROM NORMAL MOUNTS. WITH THIS, 
				ALL MOUNTS AFTER THE MOUNT WITH THE SHORTEST PATH, GIVEN A FILESYSTEM FULL PATH, 
				SHALL BE TREATED AS SYMBOLIC LINKS. IN OTHER WORDS, WE SIMPLY TRUST THE USER TO 
				PROVIDE THE CORRECT DEPTH VALUE FOR SYMBOLIC LINKS.
			*/
			//tLength = crx_c_os_fileSystem_posix_getMountInformationFor(&vTrail, 3, &tMounfInfo);

			/*
			HISTORY:
			THE FOLLOWING CODE ASSUMED THAT A NON BLOCK DEVICE COULD NOT APPEAR AFTER A BLOCK
			DEVICE IN THE PATH. WITH BIND MOUNTS ON LINUX AND FREEBSD, THIS IS NO LONGER GUARANTEED.
			BIND MOUNTS COULD BE TREATED WITH THE RULES OF SYMBOLIC LINKS, HOWEVER IT IS NOT 
			POSSIBLE TO TELL THEM APART FROM NORMAL MOUNTS. THE TRICK THAT USES rename TO DETECT
			BIND MOUNTS, ALSO DETECTS ALL OTHER MOUNTS.
			WITH THIS, THE PATH PREFIX MUST BE THE SHORTEST MOUNT POINT THAT IS ITSELF NOT A BLOCK
			DEVICE, AND IS NOT FOLLOWED BY A BIND MOUNT. AND ELSWHERE, ANY MOUNTS FOLLOWING THE 
			PREFIX WILL BE TREATED AS SYMBOLIC LINKS TO BE SAFE. REMEMBER THAT IN THE PAST, MOUNTS
			APPEARING AFTER THE EXTRACTED PREFIX WERE NOT CONSIDERED SPECIAL, BUT THE CODE ABOVE
			DID DO A SAFETY CHECK TO MAKE SURE THAT INDEED NON OF THEM WERE NON BLOCK DEVICES.
			
			LATER:
			THE ABOVE BROUGHT BACK THE CONTRADICTION WITH USING THE LONGEST MOUNT IN THE PATH. HENCE
			THE FOLLOWING APPROACH WAS RE USED INSTEAD.
			*/
			tLength = crx_c_os_fileSystem_posix_getMountInformationFor(&vTrail, 2, &tMounfInfo);
			
			if(tLength > 0)
			{
				Crx_C_String const * tString = 
						crx_c_os_fileSystem_mountInfo_getFullPathOfMountPoint(tMountInfo);
				char * tChar = crx_c_string_getCharsPointer(&vTrail);
				size_t tLength2 = crx_c_string_getSize(&vTrail);
				size_t tIndex = crx_c_string_getSize(tString);

				//tString IS EXPECTED TO END WITH '/'
				crx_c_string_empty(pExtractedPrefix);
				crx_c_string_appendString(pExtractedPrefix, tString);
				crx_c_string_empty(pExtractedRoute);
				
				while((tIndex < tLength2) && (*(tChar + tIndex) == 47 /* / */))
					{tIndex++;}

				if(tIndex < tLength2)
				{
					crx_c_string_insertElementsAt(pExtractedRoute, 0, &vTrail, 
							tIndex, tLength2 - tIndex);
				}
				else
					{crx_c_string_appendChar(pExtractedRoute, 47 /* / */);}
			}
			else
				{vIsValid = false;}
			
			if(vIsValid)
			{
				if(pExtractedOriginalPrefix != NULL)
					{crx_c_string_appendString(pExtractedOriginalPrefix, pExtractedPrefix);}
			}
			
			/*struct stat tStat;

			if(stat(crx_c_string_constantGetCharsPointer, &tStat) == 0 && S_ISDIR(sb.st_mode))
			{
				...it is a directory...
			}*/
			crx_c_os_fileSystem_mountInfo_destruct(&tMounfInfo);
		}
		else
		{
			if(!crx_c_string_tryToPilferSwapContentWith(&pExtractedRoute, &vTrail))
			{
				crx_c_string_destruct(&pExtractedRoute);
				memcpy(&pExtractedRoute, &vTrail, sizeof(Crx_C_String));
				crx_c_string_construct(&vTrail);
			}
		}
	}
#endif

	if(vIsValid)
	{
		if(vIsToNormalize)
		{
			crx_c_string_replaceOccurancesOfChar(pExtractedRoute, 0, 
					CRX__OS__FILE_SYSTEM__PRIVATE__NOT_SEPERATOR_CODE, 
					CRX__OS__FILE_SYSTEM__SEPERATOR_CODE, false);
		}
	}
	
	if(pIsToNormalize__return != NULL)
		{*pIsToNormalize__return = vIsToNormalize;}

	crx_c_string_destruct(&vTrail);
	
	return vIsValid;
}


/*
	MODE: (pMode)
		0: EXACT
		1: LONGEST
		2: SHORTEST
		3: SHORTEST, BUT STILL LONGER THAN LONGEST NON BLOCK MOUNT (FOR HISTORICAL REASONS ONLY)
		
	THIS ONLY CONSIDERS MOUNTS TO BE MOUNTS IF THEY ARE BLOCK DEVICES.
	
	MODE 3 ABOVE DOES NOT WORK IN THE PRESENCE OF BIND MOUNTS.
		
	RETURNS THE BYTE LENGTH OF THE MOUNT POINT PATH THAT IS FOUND
*/
size_t crx_c_os_fileSystem_posix_getMountInformationFor(Crx_C_String const * pFullPath, 
		uint32_t pMode, Crx_C_Os_FileSystem_MountInfo * pMountInfo)
{
	CRX_SCOPE_META
	assert(pMode <= 3);

	CRX_SCOPE
	Crx_C_Array/*<Crx_C_Os_FileSystem_MountInfo>*/ vMountInfos /*= ?*/;
	bool vIsValid = true;
	bool vIsFound = false;
	size_t tLengthOfLongestNonBlockMount = 0;
	size_t vIndexOfFoundMountInfo = 0;
	size_t vReturn = 0;

	crx_c_array_construct(&vMountInfos, 
			crx_c_os_fileSystem_mountInfo_getTypeBluePrint(), 0, 0);

	crx_c_os_fileSystem_computeMountPointsIfApplicable(&vMountInfos);
	
	if(pMode == 0)
	{
		CRX_FOR(size_t tI = 0, tI < crx_c_array_getLength(&tMountInfos), tI++)
		{
			if(crx_c_string_isEqualTo(pFullPath, 
					crx_c_os_fileSystem_mountInfo_getFullPathOfMountPoint(
					(Crx_C_Os_FileSystem_MountInfo)crx_c_array_get(&tMountInfos, tI)), false))
			{
				vIsFound = true;

				break;
			}
		}
		CRX_ENDFOR	
	}
	else if(pMode == 2)
	{
		CRX_FOR(size_t tI = 0, tI < crx_c_array_getLength(&vMountInfos), tI++)
		{
			Crx_C_Os_FileSystem_MountInfo * tMountInfo = 
					(Crx_C_Os_FileSystem_MountInfo)crx_c_array_get(&vMountInfos, tI);

			if(crx_c_string_isBeginningWith2(pFullPath, 
					crx_c_os_fileSystem_mountInfo_getFullPathOfMountPoint(tMountInfo), false))
			{
				if(crx_c_os_fileSystem_mountInfo_isLikelyABlockDevice(tMountInfo))
				{
					if(!vIsFound || (crx_c_string_getSize(
							crx_c_os_fileSystem_mountInfo_getFullPathOfMountPoint(
							(Crx_C_Os_FileSystem_MountInfo)crx_c_array_get(&vMountInfos, 
							vIndexOfFoundMountInfo))) < crx_c_string_getSize(
							crx_c_os_fileSystem_mountInfo_getFullPathOfMountPoint(tMountInfo))))
					{
						vIndexOfFoundMountInfo = tI;
						vIsFound = true;
					}
				}
				else
				{
					vIsValid = false;

					break;
				}
			}
		}
		CRX_ENDFOR
	}
	else
	{
		if(pMode == 3)
		{
			CRX_FOR(size_t tI = 0, tI < crx_c_array_getLength(&vMountInfos), tI++)
			{
				Crx_C_Os_FileSystem_MountInfo * tMountInfo = 
						(Crx_C_Os_FileSystem_MountInfo)crx_c_array_get(&vMountInfos, tI);

				if(crx_c_string_isBeginningWith2(pFullPath, 
						crx_c_os_fileSystem_mountInfo_getFullPathOfMountPoint(tMountInfo), false))
				{
					if(!crx_c_os_fileSystem_mountInfo_isLikelyABlockDevice(tMountInfo))
					{
						if(tLengthOfLongestNonBlockMount < crx_c_string_getSize(
								crx_c_os_fileSystem_mountInfo_getFullPathOfMountPoint(
								tMountInfo)))
						{
							tLengthOfLongestNonBlockMount = crx_c_string_getSize(
									crx_c_os_fileSystem_mountInfo_getFullPathOfMountPoint(
									tMountInfo));
						}
					}
				}
			}
			CRX_ENDFOR
		}
		
		CRX_FOR(size_t tI = 0, tI < crx_c_array_getLength(&vMountInfos), tI++)
		{
			Crx_C_Os_FileSystem_MountInfo * tMountInfo = 
					(Crx_C_Os_FileSystem_MountInfo)crx_c_array_get(&vMountInfos, tI);

			if(crx_c_string_isBeginningWith2(pFullPath, 
					crx_c_os_fileSystem_mountInfo_getFullPathOfMountPoint(tMountInfo), false))
			{
				if(crx_c_os_fileSystem_mountInfo_isLikelyABlockDevice(tMountInfo))
				{
					if((!vIsFound || (crx_c_string_getSize(
									crx_c_os_fileSystem_mountInfo_getFullPathOfMountPoint(
									(Crx_C_Os_FileSystem_MountInfo)crx_c_array_get(&vMountInfos, 
									vIndexOfFoundMountInfo))) > crx_c_string_getSize(
									crx_c_os_fileSystem_mountInfo_getFullPathOfMountPoint(
									tMountInfo)))) && 
							((pMode == 2) || (crx_c_string_getSize(
									crx_c_os_fileSystem_mountInfo_getFullPathOfMountPoint(
									tMountInfo)) > tLengthOfLongestNonBlockMount)))
					{
						vIndexOfFoundMountInfo = tI;
						vIsFound = true;
					}
				}
			}
		}
		CRX_ENDFOR
	}
	
	
	if(!vIsValid)
		{vIsFound = false;}

	if(vIsFound)
	{
		if(pMountInfo != NULL)
		{
			vIsValid = crx_c_os_fileSystem_mountInfo_copyAssignFrom(pMountInfo, 
					(Crx_C_Os_FileSystem_MountInfo)crx_c_array_get(&vMountInfos, 
					vIndexOfFoundMountInfo));
		}
		if(vIsValid)
		{
			vReturn = crx_c_string_getSize(crx_c_os_fileSystem_mountInfo_getFullPathOfMountPoint(
					(Crx_C_Os_FileSystem_MountInfo)crx_c_array_get(&vMountInfos, 
					vIndexOfFoundMountInfo)));
		}
	}
	
	crx_c_array_destruct(&vMountInfos);
	
	return vReturn;
	
	CRX_SCOPE_END
}
//REFERECE: GNU Midnight Commander: "mountlist.c"
//MAKE SURE TO REVISE TH THREAD SAFETY OF THIS FUNCTION.
bool crx_c_os_fileSystem_computeMountPointsIfApplicable(
		Crx_C_Array/*<Crx_C_Os_FileSystem_MountInfo>*/ * CRX_NOT_NULL pReturn)
{
#if(CRX__NON_CRXED__OS__SETTING__IS_LINUX || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__ANDROID))
	/*
		FORMAT EXAMPLE:
		36 35 98:0 /mnt1 /mnt2 rw,noatime master:1 - ext3 /dev/root rw,errors=continue
        (1)(2)(3)   (4)   (5)      (6)      (7)       (8)   (9)   (10)      (11)
	*/
	FILE * vFile = fopen ("/proc/self/mountinfo", "r");
#elif(CRX__NON_CRXED__OS__SETTING__IS_MACOS || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD))
	Crx_NonCrxed_Os_Posix_Dll_Libc_Statfs * vStatfs = NULL;
    size_t vNumberOfRemainingEntries = crx_c_os_fileSystem_private_posix_getMntInfo(&vStatfs, 
			MNT_NOWAIT);
#endif
	Crx_C_String vString /*= ?*/;
	Crx_C_String vString2 /*= ?*/;
	Crx_C_String vMountOptionsString /*= ?*/;
	bool vIsNoError = true;

	crx_c_string_construct(&vString);
	crx_c_string_construct(&vString2);
	crx_c_string_construct(&vMountOptionsString);
	
	crx_c_arrays_string_empty(pReturn);

#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__ANDROID))
	if(vFile != NULL) //TRYING TO PARSE MOUNTINFO FIRST TO MAKE DEVICE IDS AVAILABLE. LINUX >= 2.6.26
	{
		char * tChars__line = NULL;
		size_t tSize = 0;

		//getline() FROM POSIX. fgets() FROM C.
		while(getline(&tChars__line, &tSize, vFile) != -1)
		{
			size_t tMountPointSubCharsStartIndex = 0;
			size_t tMountPointSubCharsEndIndex = 0;
			size_t tMountOptionsSubCharsStartIndex = 0;
			size_t tMountOptionsSubCharsEndIndex = 0;
			char test = '\0';
			int tNumberOfValuesScanned = sscanf(tChars__line, //NOTE THAT %n IS NOT A VALUE SCANNED
					"%*u "		/* id (1)*/
					"%*u "     	/* parent id (2)*/
					"%*u:%*u "  /* dev major:minor (3). 	THE VALUES IN stat.st_dev*/
					"%*s "		/* path in filesystem that is acting as its root  (4)*/
					/*
						's' MODIFIES THE STRING BECAUSE THE WHITE SPACE AFTER WHAT MATCHS 's' IS 
								REPLACED WITH THE NULL TERMINATOR. HENCE, IF I AM NOT WRONG, THE 
								USE OF *, WHICH IS TO READ THE DATA BUT IGNORE IT, AND n WHICH IS TO 
								GIVE US THE INDICES OF THE STRING AT THESE POINTS.
					*/
					"%n%*s%n"	/* path where file system is mounted, start and end  (5)*/
					"%n%*s%n"	/* MOUNT OPTIONS, I HOPE. (6)*/
					"%c",		/* more data...  */
					&tMountPointSubCharsStartIndex, &tMountPointSubCharsEndIndex, 
					&tMountOptionsSubCharsStartIndex, &tMountOptionsSubCharsEndIndex
					&test);

			if((tNumberOfValuesScanned == 1) || (tNumberOfValuesScanned == 3 /*%n IS COUNTED.*/) ||
					|| (tNumberOfValuesScanned == 5 /*%n IS COUNTED.*/))
			{
				Crx_C_Os_FileSystem_MountInfo tMountInfo /*= ?*/;
				size_t tFileSystemSubCharsStartIndex = 0;
				size_t tFileSystemSubCharsEndIndex = 0;
				char * tChars = strstr(tChars__line + tMountPointSubCharsEndIndex, " - ");
				struct stat tStat /*= ?*/;

				crx_c_os_fileSystem_mountInfo_construct(&tMountInfo);

				crx_c_os_fileSystem_computeMountPointsIfApplicable__decodeOctalEscapesToAscii(
						tChars__line + tMountPointSubCharsStartIndex, false);
				
				crx_c_string_appendChars(&vString, tChars__line + 
						tMountPointSubCharsStartIndex, 
						tMountPointSubCharsEndIndex - tMountPointSubCharsStartIndex);
				//crx_c_os_fileSystem_enforcePathAsDirectoryPath(&vString, true); //WITH BIND 
																				  //MOUNTS, THIS IS 
																				  //NOT CORRECT.

				if(tChars != NULL)
				{
					tNumberOfValuesScanned = sscanf(tChars, 
							" - " //
							"%n%*s%n " // Filesystem type, start and end (8)
							"%*s " // source, start and end	(9)
							"%c",      // more data...
							&tFileSystemSubCharsStartIndex, &tFileSystemSubCharsEndIndex, &test);

					if((tNumberOfValuesScanned == 1) || 
							(tNumberOfValuesScanned != 3 /*%n IS COUNTED*/))
					{
						tChars[tFileSystemSubCharsEndIndex] = '\0';
						
						crx_c_string_appendChars(&vString2, tChars + 
								tFileSystemSubCharsStartIndex, 
								tFileSystemSubCharsEndIndex - tFileSystemSubCharsStartIndex);
					}
				}

				if(lstat(crx_c_string_getCString(&vString), &tStat, 0) == 0)
				{
					if(S_ISDIR(tStat))
						{crx_c_os_fileSystem_enforcePathAsDirectoryPath(&vString, true);}
					else
					{
						while(crx_c_string_isEndingWith2(&vString, "/", true))
						{
							crx_c_string_removeChars(&vString, 
									crx_c_string_getSize(&vString) - 1, 1);
						}
					}
				}
				
				crx_c_string_appendChars(&vMountOptionsString, tChars__line + 
						tMountOptionsSubCharsStartIndex, 
						tMountOptionsSubCharsEndIndex - tMountOptionsSubCharsStartIndex);

				crx_c_os_fileSystem_mountInfo_set2(&tMountInfo, vString, vString2,
						crx_c_os_fileSystem_computeMountPointsIfApplicable__computeTimeOffset(
						crx_c_string_getCString(&vMountOptionsString)));
						
				crx_c_array_tryMoveAndPush(pReturn, &tMountInfo);
				crx_c_string_empty(&vString);
				crx_c_string_empty(&vString2);
				crx_c_string_empty(&vMountOptionsString);
			}
		}

		free(tChars__line);
		tChars__line = NULL;

		if(ferror(vFile) != 0)
		{
			int tErrno = errno;

			errno = tErrno;
			vIsNoError = false;
			fclose(vFile);
		}
		else if(fclose(vFile) == EOF)
			{vIsNoError = false;}
	}
	else                    /* fallback to /proc/self/mounts (/etc/mtab).  */
	{
		//TODO: THIS APPROACH REQUIRES getmntent() WHICH REQUIRES THE "FILE" TYPE WHICH MUST NOT BE
		//		USED.
		//HISTORY: getmntent IS NOT THREAD SAFE
		//vFile = setmntent(CRX__C__OS__FILE_SYSTEM__PRIVATE__MOUNTED_INFO_PATH, "r");
		//GLIBC ADDS ce TO THE MODE.
		vFile = fopen(CRX__C__OS__FILE_SYSTEM__PRIVATE__MOUNTED_INFO_PATH, "rce");

		if(vFile != NULL)
		{
			struct mntent tMntent;
			char * tCString = NULL;
			size_t tSize = 0;
			
			CRX__SET_TO_ZERO(struct mntent, tMntent);
			
			while(crx_c_os_fileSystem_computeMountPointsIfApplicable__getmntent(vFile, tMntent, 
					&tCString, &tSize))
			{
				Crx_C_Os_FileSystem_MountInfo tMountInfo /*= ?*/;

				crx_c_os_fileSystem_mountInfo_construct(&tMountInfo);

				crx_c_string_appendCString(&vString, tMntent->mnt_dir);
				crx_c_os_fileSystem_enforcePathAsDirectoryPath(&vString, true);
				crx_c_string_appendCString(&vString2, tMntent->mnt_type);

				crx_c_os_fileSystem_mountInfo_set2(&tMountInfo, vString, vString2,
						crx_c_os_fileSystem_computeMountPointsIfApplicable__computeTimeOffset(
						tMntent->mnt_opts));

				crx_c_array_tryMoveAndPush(pReturn, &tMountInfo);
				crx_c_string_empty(&vString);
				crx_c_string_empty(&vString2);
				
				free(tCString);
				tCString = NULL;
			}

			if(tCString != NULL)//CAN HAPPEN. SEE POSIX'S getLine()
			{
				free(tCString);
				tCString = NULL;
			}
			
			if(fclose(vFile) != 0)//SHOULD NOT HAPPEN
				{vIsNoError = false;}
		}
		else
			{vIsNoError = false;}
	}
#elif((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD))
	if(vNumberOfRemainingEntries >= 0)
	{
		crx_c_array_ensureCapacity(pReturn, vNumberOfRemainingEntries);

		while(vNumberOfRemainingEntries > 0)
		{
			//THERE IS statfs::f_fstypename (STRING, RECENT MACOS, FREEBSD) AND 
			//		statfs::f_type (INT), FOR THE FILE SYSTEM TYPE.
			Crx_C_Os_FileSystem_MountInfo tMountInfo /*= ?*/;

			crx_c_os_fileSystem_mountInfo_construct(&tMountInfo);

			crx_c_string_appendCString(&vString, vStatfs->f_mntonname);
			crx_c_os_fileSystem_enforcePathAsDirectoryPath(&vString, true);
			crx_c_string_appendCString(&vString2, vStatfs->f_fstypename);
			
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
			crx_c_os_fileSystem_mountInfo_set2(&tMountInfo, vString, vString2, 0);
	#else
			crx_c_os_fileSystem_mountInfo_set2(&tMountInfo, vString, vString2, 0);
	#endif

			crx_c_array_tryMoveAndPush(pReturn, &tMountInfo);
			crx_c_string_empty(&(vMountInfo.gFullPath));
			crx_c_string_empty(&(vMountInfo.gFileSystemName));

			vStatfs = vStatfs + 1;
			vNumberOfRemainingEntries = vNumberOfRemainingEntries - 1;
		}
	}
	else
		{vIsNoError = false;}
#endif

	if(!vIsNoError)
		{crx_c_arrays_string_empty(pReturn);}


	crx_c_string_destruct(&vString);
	crx_c_string_destruct(&vString2);
	crx_c_string_destruct(&vMountOptionsString);
	
	/*
	TODO: THIS IS INCOMPLETE. YOU MUST NOT LOOK OVER EVERY MOUNT POINT AND CHECK IF IT IS A BLOCK
	DEVICE. USE lstat. I HAVE CONFIRMED THAT THE COMMAND LINE stat FUNCTION DETECTS MOUNT POINTS OF
	BLOCK DEVICES AS BLOCK DEVICES. I AM HOPING THE C FUNCTIONS, stat AND lstat, DO THE SAME.
	WITH THIS, CALLING CODE CAN USE THE ASSUMPTION THAT IF A PATH IS NOT BLOCK DEVICE, IT CAN NOT
	BE THE PARENT PATH OF A PATH THAT IS A BLOCK DEVICE. HOWEVER, THE OPPOSITE IS NOT TRUE. CALLING
	CODE MUST NOT ONLY CHECK IF PATH BEGINS WITH A BLOCK DEVICE MOUNT POINT, BUT THAT IT ALSO DOES
	NOT BEGIN WITH A NON BLOCK DEVICE MOUNT POINT. FOR THIS PURPOSE THIS FUNCTION MUST NO LONGER 
	RETURN AN ARRAY OF String, BUT AN ARRAY OF MountInfo. SEE ABOVE. USE THE GENERIC ARRAY FOR THE 
	PURPOSE.
	
	ON CAMMAND LINE SOMETHING LIKE THE FOLLOWING CAN CHECK IF PATH IS BLOCK DEVICE.
					stat -f /gf/gfd/gdf
			THE "test -b" COMMAND AND OPTION WERE NOT USEFUL.
	ON COMMAND LINE SOMETHING LIKE THE FOLLOWING PRINTS ALL THE MOUNT POITNS.
					cat /proc/self/mountinfo
					
	UPDATE: 
		lstat, MEANING S_ISBLK MACRO, DID NOT WORK. IT SEEMS lstat WILL ONLY TELL YOU IF 
				SOMETHING IS A BLOCK DEVICE IF THE PATH IS THE BLOCK DEVICE ITSELF, LIKE /dev/sda1. 
				ON A MOUNT POINT. HOWEVER, IT DOES NOT WORK.
		NEW APPROACH TO USE FILESYSTEM TO DETECT WHETHER BLOCK DEVICE OR NOT.
	*/

	return vIsNoError;
}
//"The fifth field in mountinfo may contain \-escaped octal sequences in case the \ itself or 
//		<space>, <newline>, <tab> characters are part of the path."
//		{SOURCE: https://unix.stackexchange.com/questions/645937/listing-the-files-that-are-being-watched-by-inotify-instances}
static void crx_c_os_fileSystem_computeMountPointsIfApplicable__decodeOctalEscapesToAscii(
		char * CRX_NOT_NULL pString, bool pIsToCheckForDoubleSlahes)
{
    size_t vIndex = 0;
    size_t vLength = strlen(pString);

    CRX_FOR(size_t tI = 0, tI < vLength, tI++)
    {
        if((*(pString + tI) == 92 /* \ */) && (tI + 3 < vLength) &&
				(*(pString + tI + 1) >= '0') && (*(pString + tI + 1) <= '3') &&
				(*(pString + tI + 2) >= '0') && (*(pString + tI + 2) <= '7') && 
				(*(pString + tI + 3) >= '0') && (*(pString + tI + 3) <= '7'))
        {
            *(pString + vIndex) = ((*(pString + tI + 1) - '0') << 6) + 
					((*(pString + tI + 2) - '0') << 3) + (*(pString + tI + 3) - '0');
            tI += 3;
        }
		else if(pIsToCheckForDoubleSlahes && (*(pString + tI) == 92 /* \ */) && 
				(tI + 1 < vLength) && (*(pString + tI + 1) == 92 /* \ */))
		{
			//THIS CHECK IS FROM GLIBC C, PERTAINING TO getmntent(). GLIBC'S CONVERSION FUNCTION
			//		EXPECTS TO SEE TWO '\' AND CONVERTS THEM TO ONE '\'
			*(pString + vIndex) = *(pString + tI);
		}
        else
			{*(pString + vIndex) = *(pString + tI);}

		vIndex = vIndex + 1;
    }
	CRX_ENDFOR
	
	*(pString + vIndex) = '\0';
}
//REFERENCE: GLIBC C mntent_r.c (2012, AND 2025)(.\misc\mntent_r.c)

struct mntent * crx_c_os_fileSystem_computeMountPointsIfApplicable__getmntent(FILE * stream, 
		struct mntent * mp, char * * pOutPutString, size_t * pSize)
{
	char * cp = NULL;
	char * head = NULL;

	assert(*pOutPutString == NULL);

	while(getLine(pOutPutString, pSize, stream) != -1)
	{
		char *end_ptr;

		//FROM NEWER GLIBC. NEWER GLIBC TRIMS END SPACE FIRST.
		while((end_ptr != *pOutPutString) && (*(end_ptr - 1) == ' ' || *(end_ptr - 1) == '\t'))
			{end_ptr--;}

		//IN OLDER AND NEWER GLIBC C, THIS WAS ALSO PART OF MAKING SURE THAT THE WHOLE LINE WAS
		//		GRABBED. GLIBC C USES fgets() INSTEAD OF getLine().
		*end_ptr = '\0'; 

		head = *pOutPutString + strspn (*pOutPutString, " \t");
		/* skip empty lines and comment lines:  */
		
		if((*head != '\0') && (*head != '#'))
			{break;}
	}

	if(head != NULL)
	{
		/*
			struct mntent
			{
				char *mnt_fsname;	// Device or server for filesystem.
				char *mnt_dir;		// Directory mounted on.
				char *mnt_type;		// Type of filesystem: ufs, nfs, etc.
				char *mnt_opts;		// Comma-separated options for fs.
				int mnt_freq;		// Dump frequency (in days).
				int mnt_passno;		// Pass number for `fsck'.
			};
		*/
		cp = __strsep (&head, " \t");
		mp->mnt_fsname = ((cp != NULL) ? 
				crx_c_os_fileSystem_computeMountPointsIfApplicable__decodeOctalEscapesToAscii(cp,
				true) : ((char *)""));

		if(head)
			{head += strspn (head, " \t");}
		cp = __strsep(&head, " \t");
		mp->mnt_dir = ((cp != NULL) ? 
				crx_c_os_fileSystem_computeMountPointsIfApplicable__decodeOctalEscapesToAscii(cp, 
				true) : ((char *) ""));
		
		if(head)
			{head += strspn (head, " \t");}
		cp = __strsep(&head, " \t");
		mp->mnt_type = ((cp != NULL) ? 
				crx_c_os_fileSystem_computeMountPointsIfApplicable__decodeOctalEscapesToAscii(cp,
				pIsToCheckForDoubleSlahes) : ((char *) ""));
		
		if(head)
			{head += strspn (head, " \t");}
		cp = __strsep (&head, " \t");
		mp->mnt_opts = ((cp != NULL) ? 
				crx_c_os_fileSystem_computeMountPointsIfApplicable__decodeOctalEscapesToAscii(cp,
				pIsToCheckForDoubleSlahes) : ((char *) ""));
		
		switch(head ? sscanf (head, " %d %d ", &mp->mnt_freq, &mp->mnt_passno) : 0)
		{
			case 0:
				mp->mnt_freq = 0;
				mp->mnt_passno = 0;
				
				break;
			case 1:
				mp->mnt_passno = 0;
				
				break;
			case 2:
				break;
		}
		
		//NEWER GLIBC C WILL CHECK IF mp->mnt_type IS EQUAL TO "autofs" AND hasmntopt(mp, "ignore")
		//		EQUALS true, AND IF true, mp IS ZEROED BEFORE RETURNING IT.
		
		return true;
	}
	else
		{return false;}
}
//REFERENCE: GLIBC C mntent_r.c (2012, AND 2025)
char * crx_c_os_fileSystem_computeMountPointsIfApplicable__hasmntopt(char const * pCString, 
		const char * opt)
{
	const size_t optlen = strlen(opt);
	//char * rest = mnt->mnt_opts; 
	char * rest = pCString; 
	char * p;

	while((p = strstr (rest, opt)) != NULL)
	{
		if((p == rest || *(p - 1) == ',') && 
				((*(p + optlen) == '\0') || (*(p + optlen) == '=') || (*(p + optlen) == ',')))
			{return p;}

		rest = strchr(p, ',');
		if(rest == NULL)
			{break;}
		++rest;
	}

	return NULL;
}
/*
	IF THIS RETURNS:
		- NULL AND SETS pSizeOfReturn TO 0, THE OPTION DOES NOT EXIST.
		- NULL AND SETS pSizeOfReturn TO 1, THE OPTION EXISTS, BUT IS A BOOLEAN OPTION. IT HAS NO
				VALUE.
		- NOT NULL, THE OPTION EXISTS AND THE SIZE OF THE VALUE C STRING IS pSizeOfReturn
		
	WARNING: THE FOLLOWING ASSUMES THAT IF THE VALUE IS NOT VALID, THE OPTION WILL NOT APPEAR AT 
			ALL. THE ASSUMPTION IS NOT IN THE IMPLEMENTATION OF THIS FUNCTION, BUT IN ITS USAGE,
			MEANING IN THE FUNCTION'S EXISTANCE.
*/
char const * crx_c_os_fileSystem_computeMountPointsIfApplicable__getMountOption(
		char const * pCString, char const * CRX_NOT_NULL pName, 
		size_t * CRX_NOT_NULL pSizeOfReturn)
{
	size_t vLength = strlen(pName);
	char const * vOption = crx_c_os_fileSystem_computeMountPointsIfApplicable__hasmntopt(pCString,
			pName);

	if(vOption == NULL)
	{
		*pSizeOfReturn = 0;
		
		return NULL;
	}
	else
	{
		if((*(vOption + vLength) == '\0') || (*(vOption + vLength) == ','))
		{
			*pSizeOfReturn = 1;

			return NULL;
		}
		else if(*(vOption + vLength) == '=')
		{
			char const * tChar = strchr(*(vOption + vLength), ((unsigned char)','));

			if(tChar != NULL)
			{
				*pSizeOfReturn = tChar - (vOption + vLength + 1);

				return (vOption + vLength);
			}
		}
		else
		{
			*pSizeOfReturn = 0;

			return NULL;
		}
	}
}

int32_t crx_c_os_fileSystem_computeMountPointsIfApplicable__computeTimeOffset(
		char const * pOptionsString)

{
	size_t vByteSizeOfValue = 0;
	char const * vValue = crx_c_os_fileSystem_computeMountPointsIfApplicable__getMountOption(
			pOptionsString, "tz", &vByteSizeOfValue);

	//STRICTLY, THIS SHOULD BE tz=UTC. HOWEVER, I AM UNSURE IF tz CAN BE ANYTHING ELSE,
	//		OR IS CASE SENSITIVE.
	if((vValue != NULL) && (vByteSizeOfValue == 3) && 
			((*vValue == 'U') || (*vValue == 'u')) &&
			((*(vValue + 1) == 'T') || (*(vValue + 1) == 't')) &&
			((*(vValue + 2) == 'C') || (*(vValue + 2) == 'c')))
		{return 0;}
	else
	{
		int32_t tMinutes = 0;
		vValue = crx_c_os_fileSystem_computeMountPointsIfApplicable__getMountOption(
				crx_c_string_getCString(&vString), "time_offset", &vByteSizeOfValue);

		if((vValue != NULL) && (vByteSizeOfValue < 9))
		{
			char tChars2[10];
			
			memcpy(&(tChars2[0]), vValue, vByteSizeOfValue);
			tChars2[vByteSizeOfValue] = '\0';
			
			//REMEMBER: THE C++ VAR WORK DOES NOT USE C'S FUNCTIONS FOR SUCH CONVERSIONS,
			//		WHICH MEANS THAT IT CONCLUDED SUCH FUNCTIONS ARE NOT FORMALLY
			//		CORRECT. BUT KEEPING THE FOLLOWING FOR NOW, WHICH, REMEMBER, IS
			//		AFFECTED BY LOCALE.
			if(sscanf(&(tChars2[0]), "%i", &tMinutes) != 1)
				{tMinutes = 0;}
		}
		else if(vValue != NULL)
		{
			tMinutes = 
					crx_c_os_fileSystem_computeMountPointsIfApplicable__computeTimeOffset2();
		}
		
		return (tMinutes * 60);
	}
}
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
int32_t crx_c_os_fileSystem_computeMountPointsIfApplicable__computeTimeOffset2()
{
	int64_t vTime = crx_c_os_fileSystem_private_posix_time(NULL);
    Crx_NonCrxed_Os_Posix_Dll_Libc_Tm vTm /*= ?*/;
	
	crx_c_os_fileSystem_private_posix_localtimeR(&vTime, &vTm);

	//"tm_gmtoff" EXISTS ON MACOS 10.7, AND IS A LONG (INT64). I ASSUMING THAT I DO NOT NEED TO
	//		ADDRESS THE "tm_isdst" MEMBER
	return ((int32_t)vTm.tm_gmtoff);
}
#endif


//IF YOU END UP USING THIS FUNCTION, REMEMBER THAT WINDOWS IMPLEMENTATION IS NOT DONE.(TODO)
CRX__LIB__PUBLIC_C_FUNCTION() bool crx_c_os_fileSystem_areOsFileHandlesToSameEntity(
		bool * CRX_NOT_NULL pReturn, Crx_C_Os_FileSystem_OsFileHandle pOsFileHandle, 
		Crx_C_Os_FileSystem_OsFileHandle pOsFileHandle__2)
{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	Crx_NonCrxed_Os_Posix_Dll_Libc_Stat vStat; 
	Crx_NonCrxed_Os_Posix_Dll_Libc_Stat vStat__2;

	if(crx_c_os_fileSystem_private_posix_callFstat(fd1, &vStat) < 0)
		{return false;}
	else if(crx_c_os_fileSystem_private_posix_callFstat(fd2, &vStat__2) < 0)
		{return false;}
	else
	{
		*pReturn = (vStat.st_dev == vStat__2.st_dev) && 
				(vStat.st_ino == vStat__2.st_ino);
	}
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	
#endif
}




CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_internal_posix_fileHandle_construct(
		Crx_C_Os_FileSystem_Internal_Posix_FileHandle * pThis)
{
	pThis->gFileDescriptor = -1;
	crx_c_string_construct(&(pThis->gFullPath));
}
		
CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_internal_posix_fileHandle_destruct(
		Crx_C_Os_FileSystem_Internal_Posix_FileHandle * pThis)
{
	pThis->gFileDescriptor = -1;
	crx_c_string_destruct(&(pThis->gFullPath));
}

CRX__LIB__PRIVATE_C_FUNCTION() int32_t crx_c_os_fileSystem_internal_posix_amendOpenatFlags(
		int32_t pFlags)
{
	static int32_t vFlags = 0;
	static bool vIsSet = false;
	
	if(!vIsSet)
	{
		Crx_C_Os_Info_OsInformation tOsInformation /*= ?*/;

		CRX__SET_TO_ZERO(Crx_C_Os_Info_OsInformation, tOsInformation);
	
		crx_c_os_info_internal_bestGuessGetOsInformation(&tOsInformation, true);

#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX)
		if((tOsInformation.gMajorVersion > 2) || ((tOsInformation.gMajorVersion == 2) &&
				(tOsInformation.gMinorVersion >= 23)))
			{vFlags = CRX__C__OS__FILE_SYSTEM__O__CLOEXEC;}
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
		if((tOsInformation.gMajorVersion > 8) || ((tOsInformation.gMajorVersion == 8) &&
				(tOsInformation.gMinorVersion > 2)))
			{vFlags = CRX__C__OS__FILE_SYSTEM__O__CLOEXEC;}
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
	//ON MACOS, SUPPORT IS LIKELY AT LEAST SINCE MACOS 10.8 IF NOT BEFORE.
		if((tOsInformation.gMajorVersion > 10) || ((tOsInformation.gMajorVersion == 10) &&
				(tOsInformation.gMinorVersion >= 10)))
			{vFlags = CRX__C__OS__FILE_SYSTEM__O__CLOEXEC;}
#endif

		vIsSet = true;
	}

	return (pFlags | vFlags);
}

CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_internal_posix_lookAt2(
		Crx_C_Os_FileSystem_Internal_Posix_FileHandle * CRX_NOT_NULL pReturn,
		Crx_C_String const * CRX_NOT_NULL pFullPath,
		Crx_C_String * CRX_NOT_NULL pRemainingPartialPath)
{
	/*
		- openat SUPPORTED BY:
				DragonFly since DragonFly 2.3.
				FreeBSD since FreeBSD 8.0.
				Linux since Linux 2.6.16.
				NetBSD since NetBSD 7.0.
				OpenBSD since OpenBSD 5.0.
				OS X since OS X 10.10.
				Solaris.
		- ON MACOS 10.8 TO 10.9, ONE CAN USE THE FOLLOWING ALTERNATIVE;
						int openat(int dirfd, const char *pathname, int flags, ...)
						{
							int fd;

							if(fchdir(dirfd) < 0)
							{
								perror("fchdir");
								return -1;
							}

							fd = open(pathname, flags, mode);
							// chdir to the original CWD

							return fd;
						}
				fchdir() CHANGES THE WORKING DIRECTORY USING A FILE DESCRIPTOR, AND ALLOWS open TO 
				WORK RELATIVE TO THAT IF PASSED A RELATIVE PATH. HOWEVER, fchdir() AFFECTS THE
				ENTIRE PROGRAM, AND EVEN ONE WAS TO USE LOCKS, THERE IS NO GUARANTEE THAT USER CODE
				WOULD NOT CALL fchdir() DIRECTLY.
				USING nftw() TO TRAVERSE THE DIRECTORY TREE UNTIL THE FILE IN QUESTION IS FOUND IS
				ALSO NOT SAFE BECAUSE IT ALSO USES fchdir()/chdir(). HENCE WHILE AVAILABLE IN MACOS
				10.7, IT IS NOT USEFUL.
	*/
	static bool vIsSet = false;
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Openat vFunc_openat /*= ?*/;
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Open vFunc_open /*= ?*/;
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Fstatat vFunc_fstatat /*= ?*/;
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Close vFunc_close /*= ?*/;
	int32_t vErrorCode = 0;
	Crx_C_String vFullPath /*= ?*/;
	int32_t tFileDescriptor = -1;

	crx_c_string_copyConstruct(&vFullPath, pFullPath);
	
	if(!vIsSet)
	{
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX)
		//LINUX EXPOSED A DIFFERENT SYMBOL IN THE PAST, __fxstatat, AND VERSIONED IT.
		vFunc_openat = crx_c_os_dll_loadSystemFunction("openat", 2, 4, 0, 0, 0);
		vFunc_open = crx_c_os_dll_loadSystemFunction("open", 2, 2, 5, 0, 0);
		if(crx_c_os_dll_loadSystemFunction("__fxstatat", 2, 4, 0, 0, 0) != NULL)
			{vFunc_fstatat = &crx_c_os_fileSystem_private_posix_linux_fstatat;}
		else
			{vFunc_fstatat = NULL;}
		vFunc_close = crx_c_os_dll_loadSystemFunction("close", 2, 2, 5, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
		vFunc_openat = crx_c_os_dll_loadSystemFunction("openat", 1, 1, 0, 0, 0);
		vFunc_open = crx_c_os_dll_loadSystemFunction("open", 1, 0, 0, 0, 0);
		vFunc_fstatat = crx_c_os_dll_loadSystemFunction("fstatat", 1, 1, 0, 0, 0);
		vFunc_close = crx_c_os_dll_loadSystemFunction("close", 1, 0, 0, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
		vFunc_openat = crx_c_os_dll_loadSystemFunction("openat", 10, 10, 0, 0, 0);//MACOS >= 10.10
		vFunc_open = ((Crx_NonCrxed_Os_Posix_Dll_Libc_Open)&open);
		vFunc_fstatat = crx_c_os_dll_loadSystemFunction("fstatat", 10, 10, 0, 0, 0);//MACOS >= 10.10
		vFunc_close = ((Crx_NonCrxed_Os_Posix_Dll_Libc_Close)&close);
	#endif
	
		vIsSet = true;
	}

	pReturn->gFileDescriptor = -1;
	crx_c_string_empty(&(pReturn->gFullPath));
	crx_c_string_empty(pRemainingPartialPath);

	if(crx_c_string_getSize(&vFullPath) == crx_c_string_getSize(pFullPath))
	{
		bool tIsADirectory = false;
		bool tIsDone = false;
		
		crx_c_os_osString_trim(&vFullPath);
		
		if((crx_c_string_getSize(&vFullPath) == 0) || 
				(!crx_c_string_isBeginningWith(&vFullPath, "/", 1, true) &&
				!crx_c_string_isBeginningWith(&vFullPath, "\\", 1, true)))
		{
			vErrorCode = ENOENT;
			
			tIsDone = true;
		}

		if(!tIsDone)
		{
			char * tChars = crx_c_string_getCString(&vFullPath);
			char * tLastChar = tChars + crx_c_string_getSize(&vFullPath) - 1;
			
			if((*tLastChar == 47 /* / */) /*|| (tLastChar == 92 /* \ * /)*/)
			{
				tIsADirectory = true;

				while((tLastChar != tChars) &&
						((*tLastChar == 47 /* / */) /*|| (tLastChar == 92 /* \ * /)*/))
					{tLastChar--;}

				if(tLastChar == tChars)
				{
					pReturn->gFileDescriptor = (*vFunc_open)('/',
							crx_c_os_fileSystem_internal_posix_amendOpenatFlags(
							O_RDONLY /*| O_DIRECTORY DOES NOT EXIST ON MACOS 10.7*/));

					if(pReturn->gFileDescriptor < 0)
						{vErrorCode = errno;}
					else
					{
						crx_c_string_appendChar(&(pReturn->gFullPath), '/');
						crx_c_string_appendChar(pRemainingPartialPath, '/');
					}

					tIsDone = true;
				}
			}
			
			if(!tIsDone)
			{
				char tChars2 = tLastChar;

				while((tChars2 != tChars) &&
						(*tChars2 != 47 /* / */) /*&& (tChars2 != 92 /* \ * /)*/)
					{tChars2--;}
			
				if(tChars2 == tChars)
				{
					pReturn->gFileDescriptor = (*vFunc_open)('/',
							crx_c_os_fileSystem_internal_posix_amendOpenatFlags(
							O_RDONLY /*| O_DIRECTORY DOES NOT EXIST ON MACOS 10.7*/));

					if(pReturn->gFileDescriptor < 0)
						{vErrorCode = errno;}
					else
						{crx_c_string_appendChar(&(pReturn->gFullPath), '/');}

					tIsDone = true;
				}

				crx_c_string_appendChars(pRemainingPartialPath, tChars2 + 1, 
						tLastChar - tChars2);

				if(tIsADirectory)
					{crx_c_string_appendChar(pRemainingPartialPath, '/');}
				
				if(!tIsDone)
					{tLastChar = tChars2;}
			}

			if(!tIsDone)
			{
				crx_c_string_appendChars(&(pReturn->gFullPath), tChars, tLastChar - tChars + 1);

				if((tLastChar - tChars + 1) >= PATH_MAX)
				{
					if(vFunc_openat != NULL)
					{
						size_t tRemainingLength = (tLastChar - tChars + 1);
						bool tIsFirst = true;
						
						while(!tIsDone && (tRemainingLength >= PATH_MAX))
						{
							char * tChars2 = tChars + PATH_MAX - 2;/*== tChars + (PATH_MAX - 1) - 1*/

							tRemainingLength = tRemainingLength - (PATH_MAX - 1);

							while((tChars2 != tChars) && 
									(*tChars2 != 47 /* / */) /*&& (tLastChar == 92 /* \ * /)*/)
							{
								tChars2--;
								tRemainingLength++;
							}
							
							if(tChars2 != tChars)
							{
								*tChars2 = '\0';

								if(tIsFirst)
								{
									tFileDescriptor = (*vFunc_open)(tChars, 
											crx_c_os_fileSystem_internal_posix_amendOpenatFlags(0));

									if(tFileDescriptor < 0)
									{
										//ERR("open: %s: %s\n", tChars, strerror(errno));
										vErrorCode = errno;
										
										tIsDone = true;
									}
									
									tIsFirst = false;
								}
								else
								{
									int tFileDescriptor2 = (*vFunc_openat)(tFileDescriptor, 
											tChars, 
											crx_c_os_fileSystem_internal_posix_amendOpenatFlags(0));
									
									if(tFileDescriptor2 < 0)
									{
										//ERR("openat: %s: %s\n", tChars, strerror(errno));

										vErrorCode = errno;
										
										(*vFunc_close)(tFileDescriptor);

										tIsDone = true;
									}
									else
									{
										(*vFunc_close)(tFileDescriptor);
										tFileDescriptor = tFileDescriptor2;
									}
								}

								*tChars2 = '/';

								if(!tIsDone)
									{tChars = tChars2 + 1;}
							}
							else
							{
								vErrorCode = EBADF;
								
								tIsDone = true;
							}
						}

						if(!tIsDone)
						{
							assert(tRemainingLength > 1);

							pReturn->gFileDescriptor = (*vFunc_openat)(tFileDescriptor, 
									tChars, 
									crx_c_os_fileSystem_internal_posix_amendOpenatFlags(0));
							
							if(pReturn->gFileDescriptor < 0)
								{vErrorCode = errno;}
								
							(*vFunc_close)(tFileDescriptor);
							tFileDescriptor = -1;

							tIsDone = true;
						}
					}
					else
						{vErrorCode = ENAMETOOLONG;}
				}
				else
				{
					*tLastChar = '\0';

					/*
						INITIALLY, IT WAS THAT WE DOT NOT CARE FOR ERROR HERE. HOWEVER, GIVEN THE 
								FOLLOWING,
										"You need +x permissions on any and all parent directories to 
										cd or access a file in a directory."
										"You need +r permissions in directories in order to list files."
										{SRC: https://unix.stackexchange.com/questions/14171/directory-with-x-permission-parents-without-it-when-would-this-be-useful}
								AND GIVEN THAT x PERMISSION IS REQUIRED FOR OPENING A DIRECTORY ITSELF, 
								IGNORING AN ERROR IN THE FOLLOWING IS NO LONGER BENEFICIAL. IN OTHER
								WORDS, GIVEN THE SAME PATH TO ACCESS A FILE, IF ACCESSING ANY OF THE 
								PARENT DIRECTORIES GIVE A PERMISSION ERROR, ACCESSING THAT FILE USING
								THE SAME PATH WILL GIVE AN ERROR. ACCESSING THAT FILE USING A DIFFERENT
								PATH MIGHT GIVE NOT AN ERROR, BUT THAT IS NOT RELEVANT HERE.
					*/
					pReturn->gFileDescriptor = (*vFunc_open)(tChars, 
							crx_c_os_fileSystem_internal_posix_amendOpenatFlags(
							O_RDONLY /*| O_DIRECTORY DOES NOT EXIST ON MACOS 10.7*/));

					if(pReturn->gFileDescriptor < 0)
						{vErrorCode = errno;}
					else
					{					
						*tLastChar = '/';

						/*crx_c_string_appendChars(&(pReturn->gFullPath),
								tChars, tLastChar - tChars + 1);*/
					}
				}
			}
		}
	}
	else
		{vErrorCode = ENOMEM;}

	if(vErrorCode != 0)
	{
		pReturn->gFileDescriptor = -1;
		crx_c_string_empty(&(pReturn->gFullPath));
		crx_c_string_empty(pRemainingPartialPath);

		errno = vErrorCode;
	}
	
	crx_c_string_destruct(&vFullPath);
	
	return (vErrorCode == 0);
}
//NOTE: "And" HERE DOES NOT INDICATE ORDER.
CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_internal_posix_verifyFileSystemPathToOs2AndOpen(
		Crx_C_Os_FileSystem_Internal_Posix_FileHandle * CRX_NOT_NULL pFileHandle,
		Crx_C_String * CRX_NOT_NULL pFullPath, Crx_C_Os_FileSystem_Contract const * pContract, 
		Crx_C_Os_FileSystem_Operation pOperation, uint32_t pDepthLimitOfReparsePoints, 
		Crx_C_Os_FileSystem_Existance * CRX_NOT_NULL pExistance,
		Crx_C_String const * CRX_NOT_NULL pVerified1Utf8PathPrefix, 
		Crx_C_String * CRX_NOT_NULL pVerified1Utf8Route,
		int32_t pFlags, uint32_t pMode /*mode_t (SEE openat)*/)
{
	Crx_C_String vRemainingPath /*= ?*/;
	int32_t vErrorCode = 0;
	bool vReturn = true;
	
	crx_c_string_construct(&vRemainingPath);
	
	crx_c_string_empty(pFullPath);

	if((pContract->gResolutionModel == CRX__C__OS__FILE_SYSTEM__RESOLUTION_MODEL__NULL) ||
			(pContract->gResolutionModel == CRX__C__OS__FILE_SYSTEM__RESOLUTION_MODEL__PRECISE))
	{
		vIsNoError = crx_c_string_appendCString(pFullPath, pVerified1Utf8PathPrefix);

		if(vIsNoError)
		{
			if(!crx_c_string_isEqualTo3(pVerified1Utf8Route, "/", 1, false))
				{vIsNoError = crx_c_string_appendCString(pFullPath, pVerified1Utf8Route);}
		}
		
		if(!vIsNoError)
			{vErrorCode = ENOMEM;}

		if(vIsNoError)
		{
			if(crx_c_os_fileSystem_internal_posix_lookAt2(pFileHandle, pFullPath, &vRemainingPath))
			{
				if(crx_c_os_fileSystem_internal_posix_replaceOpenat(pFileHandle,
						crx_c_string_getCString(&vRemainingPath), 
						pFlags, pMode))
				{
					if(!crx_c_os_fileSystem_verifyFileSystemPath2(pContract, pOperation,
							&(pFileHandle->gFileDescriptor), pDepthLimitOfReparsePoints,
							pExistance, pVerified1Utf8PathPrefix, pVerified1Utf8Route, NULL))
					{
						vIsNoError = false;
						vErrorCode = EINVAL;
					}
				}
				else
					{vIsNoError = false;}
			}
			else
				{vIsNoError = false;}
		}
	}
	else if((pContract->gResolutionModel == 
					CRX__C__OS__FILE_SYSTEM__RESOLUTION_MODEL__PRECISE_AND_SLOT_AWARE) ||
			(pContract->gResolutionModel == 
					CRX__C__OS__FILE_SYSTEM__RESOLUTION_MODEL__SLOT_AWARE))
	{
		if(crx_c_os_fileSystem_verifyFileSystemPath2(pContract, pOperation,
				NULL, pDepthLimitOfReparsePoints,
				pExistance, pVerified1Utf8PathPrefix, pVerified1Utf8Route, NULL))
		{
			vIsNoError = crx_c_string_appendCString(pFullPath, pVerified1Utf8PathPrefix);

			if(vIsNoError)
			{
				if(!crx_c_string_isEqualTo3(pVerified1Utf8Route, "/", 1, false))
					{vIsNoError = crx_c_string_appendCString(pFullPath, pVerified1Utf8Route);}
			}

			if(!vIsNoError)
				{vErrorCode = ENOMEM;}

			if(vIsNoError)
			{
				if(crx_c_os_fileSystem_internal_posix_lookAt2(pFileHandle, pFullPath, 
						&vRemainingPath))
				{
					if(!crx_c_os_fileSystem_internal_posix_replaceOpenat(pFileHandle,
							crx_c_string_getCString(&vRemainingPath), 
							pFlags, pMode))
						{vIsNoError = false;}
				}
				else
					{vIsNoError = false;}
			}
		}
		else
		{
			vIsNoError = false;
			vErrorCode = EINVAL;
		}
	}
	else
		{assert(false);}
	
	crx_c_string_destruct(&vRemainingPath);
	
	if(!vIsNoError)
		{errno = vErrorCode;}

	return vIsNoError;
}

/*
	UNLIKE THE NATIVE SYSTEM FUNCTION, THIS FUNCTION AUTOMATICALLY SETS THE O_CLOEXEC IF AVAILABLE.
*/
CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_internal_posix_openat(
		Crx_C_Os_FileSystem_Internal_Posix_FileHandle * CRX_NOT_NULL pReturn,
		Crx_C_Os_FileSystem_Internal_Posix_FileHandle const * CRX_NOT_NULL pFileHandle,
		char const * pPartialPath, int32_t pFlags, uint32_t pMode /*mode_t*/)
{
	static bool vIsSet = false;
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Openat vFunc_openat /*= ?*/;
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Open vFunc_open /*= ?*/;
	bool vIsPartialPathEmpty = ((pPartialPath == NULL) || (strlen(pPartialPath) == 0) ||
			((strlen(pPartialPath) == 1) && ((*pPartialPath == '/') || (*pPartialPath == '\\'))));
	int32_t vErrorCode = 0;
	bool vIsDone = false;
	
	if(!vIsSet)
	{
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX)
		//LINUX EXPOSED A DIFFERENT SYMBOL IN THE PAST, __fxstatat, AND VERSIONED IT.
		vFunc_openat = crx_c_os_dll_loadSystemFunction("openat", 2, 4, 0, 0, 0);
		vFunc_open = crx_c_os_dll_loadSystemFunction("open", 2, 2, 5, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
		vFunc_openat = crx_c_os_dll_loadSystemFunction("openat", 1, 1, 0, 0, 0);
		vFunc_open = crx_c_os_dll_loadSystemFunction("open", 1, 0, 0, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
		vFunc_openat = crx_c_os_dll_loadSystemFunction("openat", 10, 10, 0, 0, 0);//MACOS >= 10.10
		vFunc_open = ((Crx_NonCrxed_Os_Posix_Dll_Libc_Open)&open);
	#endif
	
		vIsSet = true;
	}

	pReturn->gFileDescriptor = -1;
	crx_c_string_empty(&(pReturn->gFullPath));
	
	if(pFileHandle->gFileDescriptor != -1)
	{
		if(vFunc_openat != NULL)
		{
			if(!vIsPartialPathEmpty)
			{
				pReturn->gFileDescriptor = (*vFunc_openat)(pFileHandle->gFileDescriptor, 
						pPartialPath, crx_c_os_fileSystem_internal_posix_amendOpenatFlags(pFlags), 
						((mode_t)pMode));
						
				if(pReturn->gFileDescriptor == -1)
					{vErrorCode = errno;}				
			}
			else
				{pReturn->gFileDescriptor = pFileHandle->gFileDescriptor;}

			vIsDone = true;
		}
		else if(crx_c_string_getSize(&(pFileHandle->gFullPath)) == 0)
		{
			vErrorCode = EBADF;

			vIsDone = true;
		}
	}

	if(!vIsDone)
	{
		bool tIsNoError = true;

		if(!crx_c_string_appendString(&(pReturn->gFullPath), &(pFileHandle->gFullPath)))
			{tIsNoError = false;}
		if(tIsNoError && !crx_c_string_isEndingWith2(&(pReturn->gFullPath), "/", false))
		{
			if(!crx_c_string_appendChar(&(pReturn->gFullPath), '/'))
				{tIsNoError = false;}
		}
		if(tIsNoError && !vIsPartialPathEmpty)
		{
			if(!crx_c_string_appendCString(&(pReturn->gFullPath), pPartialPath))
				{tIsNoError = false;}
		}

		if(tIsNoError)
		{
			if(crx_c_string_getSize(&(pReturn->gFullPath)) < PATH_MAX)
			{
				pReturn->gFileDescriptor = (*vFunc_open)(crx_c_string_getCString(
						&(pReturn->gFullPath)), 
						crx_c_os_fileSystem_internal_posix_amendOpenatFlags(pFlags), 
						((mode_t)pMode));

				if(pReturn->gFileDescriptor == -1)
					{vErrorCode = errno;}
			}
			else
				{vErrorCode = ENAMETOOLONG;}
		}
		else
			{vErrorCode = ENOMEM;}
		
		if(vErrorCode != 0)
			{crx_c_string_empty(&(pReturn->gFullPath));}
		
		vIsDone = true;
	}

	if(vErrorCode != 0)
	{
		errno = vErrorCode;

		return false;
	}
	else
		{return true;}
}
/*
THIS FUNCTION WILL CLOSE THE CURRENT FILE HANDLE, AND OPEN A NEW FILE AND ASSIGN FILE HANDLE THE
NEW VALUE. IF OPENING THE NEW FILE FAILS, THE OLD HANDLE IS NOT CHANGED, NOT CLOSED.
*/
CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_internal_posix_replaceOpenat(
		Crx_C_Os_FileSystem_Internal_Posix_FileHandle const * CRX_NOT_NULL pFileHandle,
		char const * pPath, int32_t pFlags, uint32_t pMode /*mode_t*/)
{
	static bool vIsSet = false;
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Openat vFunc_openat /*= ?*/;
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Open vFunc_open /*= ?*/;
	bool vIsPartialPathEmpty = ((pPartialPath == NULL) || (strlen(pPartialPath) == 0) ||
			((strlen(pPartialPath) == 1) && ((*pPartialPath == '/') || (*pPartialPath == '\\'))));
	int32_t vErrorCode = 0;
	int32_t vFileDescriptor = -1;
	bool vIsDone = false;
	
	if(!vIsSet)
	{
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX)
		//LINUX EXPOSED A DIFFERENT SYMBOL IN THE PAST, __fxstatat, AND VERSIONED IT.
		vFunc_openat = crx_c_os_dll_loadSystemFunction("openat", 2, 4, 0, 0, 0);
		vFunc_open = crx_c_os_dll_loadSystemFunction("open", 2, 2, 5, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
		vFunc_openat = crx_c_os_dll_loadSystemFunction("openat", 1, 1, 0, 0, 0);
		vFunc_open = crx_c_os_dll_loadSystemFunction("open", 1, 0, 0, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
		vFunc_openat = crx_c_os_dll_loadSystemFunction("openat", 10, 10, 0, 0, 0);//MACOS >= 10.10
		vFunc_open = ((Crx_NonCrxed_Os_Posix_Dll_Libc_Open)&open);
	#endif
	
		vIsSet = true;
	}

	if(pFileHandle->gFileDescriptor != -1)
	{
		if(vFunc_openat != NULL)
		{
			if(!vIsPartialPathEmpty)
			{
				vFileDescriptor = (*vFunc_openat)(pFileHandle->gFileDescriptor, pPartialPath,
						crx_c_os_fileSystem_internal_posix_amendOpenatFlags(pFlags), 
						((mode_t)pMode));
						
				if(vFileDescriptor == -1)
					{vErrorCode = errno;}
			}
			else
				{vFileDescriptor = pFileHandle->gFileDescriptor;}
			
			vIsDone = true;
		}
		else if(crx_c_string_getSize(&(pFileHandle->gFullPath)) == 0)
		{
			vErrorCode = EBADF;

			vIsDone = true;
		}
	}

	if(!vIsDone)
	{
		bool tIsNoError = true;
		size_t tLength = crx_c_string_getSize(&(pFileHandle->gFullPath));

		if(tIsNoError && !crx_c_string_isEndingWith2(&(pFileHandle->gFullPath), "/", false))
		{
			if(!crx_c_string_appendChar(&(pFileHandle->gFullPath), '/'))
				{tIsNoError = false;}
		}
		if(tIsNoError && !vIsPartialPathEmpty)
		{
			if(!crx_c_string_appendCString(&(pFileHandle->gFullPath), pPartialPath))
				{tIsNoError = false;}
		}

		if(tIsNoError)
		{
			if(crx_c_string_getSize(&(pFileHandle->gFullPath)) < PATH_MAX)
			{
				vFileDescriptor = (*vFunc_open)(crx_c_string_getCString(&(pFileHandle->gFullPath)), 
						crx_c_os_fileSystem_internal_posix_amendOpenatFlags(pFlags), 
						((mode_t)pMode));

				if(vFileDescriptor == -1)
					{vErrorCode = errno;}
			}
			else
				{vErrorCode = ENAMETOOLONG;}
		}
		else
			{vErrorCode = ENOMEM;}
		
		if(vErrorCode != 0)
		{
			crx_c_string_removeChars(&(pFileHandle->gFullPath), tLength, 
					crx_c_string_getSize(&(pFileHandle->gFullPath)) - tLength);
		}
		
		vIsDone = true;
	}

	if(vErrorCode != 0)
	{
		errno = vErrorCode;

		return false;
	}
	else
	{
		crx_c_os_fileSystem_private_posix_doClose(pFileHandle, true);

		pFileHandle->gFileDescriptor = vFileDescriptor;

		return true;
	}
}
CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_internal_posix_rawOpen(
		int32_t * CRX_NOT_NULL pReturn, char const * pCString, int32_t pFlags, 
		uint32_t pMode /*mode_t*/)
{
	static bool vIsSet = false;
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Open vFunc_open /*= ?*/;
	int32_t vErrorCode = 0;
	bool vIsDone = false;
	
	*pReturn = -1;
	
	if(!vIsSet)
	{
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX)
		vFunc_open = crx_c_os_dll_loadSystemFunction("open", 2, 2, 5, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
		vFunc_open = crx_c_os_dll_loadSystemFunction("open", 1, 0, 0, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
		vFunc_open = ((Crx_NonCrxed_Os_Posix_Dll_Libc_Open)&open);
	#endif
	
		vIsSet = true;
	}

	pReturn->gFileDescriptor = -1;
	crx_c_string_empty(&(pReturn->gFullPath));

	if(!vIsDone)
	{
		if(strlen(pCString) < PATH_MAX)
		{
			*pReturn = (*vFunc_open)(pCString, crx_c_os_fileSystem_internal_posix_amendOpenatFlags(
					pFlags), ((mode_t)pMode));

			if(*pReturn == -1)
				{vErrorCode = errno;}
		}
		else
			{vErrorCode = ENAMETOOLONG;}
	}

	if(vErrorCode != 0)
	{
		errno = vErrorCode;

		return false;
	}
	else
		{return true;}
}


CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_internal_posix_mkdirat(
		Crx_C_Os_FileSystem_Internal_Posix_FileHandle const * CRX_NOT_NULL pFileHandle,
		char const * pPartialPath, uint32_t pMode /*mode_t*/)
{
	static bool vIsSet = false;
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Mkdirat vFunc_mkdirat /*= ?*/;
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Mkdir vFunc_mkdir /*= ?*/;
	bool vIsPartialPathEmpty = ((pPartialPath == NULL) || (strlen(pPartialPath) == 0) ||
			((strlen(pPartialPath) == 1) && ((*pPartialPath == '/') || (*pPartialPath == '\\'))));
	int32_t vErrorCode = 0;
	bool vIsDone = false;
	
	if(!vIsSet)
	{
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX)
		vFunc_mkdirat = crx_c_os_dll_loadSystemFunction("mkdirat", 2, 4, 0, 0, 0);
		vFunc_mkdir = crx_c_os_dll_loadSystemFunction("mkdir", 2, 2, 5, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
		vFunc_mkdirat = crx_c_os_dll_loadSystemFunction("mkdirat", 1, 0, 0, 0, 0);
		vFunc_mkdir = crx_c_os_dll_loadSystemFunction("mkdir", 1, 0, 0, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
		vFunc_mkdirat = crx_c_os_dll_loadSystemFunction("mkdirat", 10, 10, 0, 0, 0);//MACOS >= 10.10
		vFunc_mkdir = ((Crx_NonCrxed_Os_Posix_Dll_Libc_Mkdir)&mkdir);
	#endif
	
		vIsSet = true;
	}

	if(pFileHandle->gFileDescriptor != -1)
	{
		if(vFunc_mkdirat != NULL)
		{
			if((*vFunc_mkdirat)(pFileHandle->gFileDescriptor, 
					pPartialPath, pFlags, ((mode_t)pMode)) != 0)
				{vErrorCode = errno;}

			vIsDone = true;
		}
		else if(crx_c_string_getSize(&(pFileHandle->gFullPath)) == 0)
		{
			vErrorCode = EBADF;

			vIsDone = true;
		}
	}

	if(!vIsDone)
	{
		bool tIsNoError = true;
		size_t tLength = crx_c_string_getSize(&(pFileHandle->gFullPath));

		if(tIsNoError && !crx_c_string_isEndingWith2(&(pFileHandle->gFullPath), "/", false))
		{
			if(!crx_c_string_appendChar(&(pFileHandle->gFullPath), '/'))
				{tIsNoError = false;}
		}
		if(tIsNoError && !vIsPartialPathEmpty)
		{
			if(!crx_c_string_appendCString(&(pFileHandle->gFullPath), pPartialPath))
				{tIsNoError = false;}
		}

		if(tIsNoError)
		{
			if(crx_c_string_getSize(&(pFileHandle->gFullPath)) < PATH_MAX)
			{
				if((*vFunc_mkdir)(crx_c_string_getCString(&(pFileHandle->gFullPath)), 
						((mode_t)pMode)) != 0)
					{vErrorCode = errno;}
			}
			else
				{vErrorCode = ENAMETOOLONG;}
		}
		else
			{vErrorCode = ENOMEM;}
		
		crx_c_string_removeChars(&(pFileHandle->gFullPath), tLength, 
				crx_c_string_getSize(&(pFileHandle->gFullPath)) - tLength);
		
		vIsDone = true;
	}

	if(vErrorCode != 0)
	{
		errno = vErrorCode;

		return false;
	}
	else
		{return true;}
}
CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_internal_posix_rawMkdir(
		char const * pCString, uint32_t pMode /*mode_t*/)
{
	static bool vIsSet = false;
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Mkdir vFunc_mkdir /*= ?*/;
	int32_t vErrorCode = 0;
	bool vIsDone = false;
	
	if(!vIsSet)
	{
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX)
		vFunc_mkdir = crx_c_os_dll_loadSystemFunction("mkdir", 2, 2, 5, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
		vFunc_mkdir = crx_c_os_dll_loadSystemFunction("mkdir", 1, 0, 0, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
		vFunc_mkdir = ((Crx_NonCrxed_Os_Posix_Dll_Libc_Mkdir)&mkdir);
	#endif
	
		vIsSet = true;
	}

	if(strlen(pCString) < PATH_MAX)
	{
		if((*vFunc_mkdir)(pCString, ((mode_t)pMode)) != 0)
			{vErrorCode = errno;}
	}
	else
		{vErrorCode = ENAMETOOLONG;}
	

	if(vErrorCode != 0)
	{
		errno = vErrorCode;

		return false;
	}
	else
		{return true;}
}


//UNLIKE POSIX'S close() THIS EFFECTIVELY MODIFY THE FILE DESCRIPTOR TO -1, AND WILL CALL
//		POSIX'S close() ONLY IF THE FILE DESCRIPTOR IS NOT -1
CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_internal_posix_close(
		Crx_C_Os_FileSystem_Internal_Posix_FileHandle * CRX_NOT_NULL pFileHandle)
	{return crx_c_os_fileSystem_private_posix_doClose(pFileHandle, false);}
CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_private_posix_doClose(
		Crx_C_Os_FileSystem_Internal_Posix_FileHandle * CRX_NOT_NULL pFileHandle, 
		bool pIsToCloseDescriptorOnly)
{
	bool vReturn = crx_c_os_fileSystem_private_posix_rawClose(pFileHandle->gFileDescriptor);

	pFileHandle->gFileDescriptor = -1;

	if(vReturn && !pIsToCloseDescriptorOnly)
		{crx_c_string_empty(&(pFileHandle->gFullPath));}

	return vReturn;
}
CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_private_posix_rawClose(
		int32_t pFileDescriptor)
{
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Close vFunc_close = NULL;
	
	if(vFunc_close == NULL)
	{
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX)
		vFunc_close = crx_c_os_dll_loadSystemFunction("close", 2, 2, 5, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
		vFunc_close = crx_c_os_dll_loadSystemFunction("close", 1, 0, 0, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
		vFunc_close = ((Crx_NonCrxed_Os_Posix_Dll_Libc_Open)&close);
	#endif
	}
	
	if(pFileHandle->gFileDescriptor != -1)
		{return ((*vFunc_close)(pFileHandle->gFileDescriptor) == 0);}
	else
		{return true;}
}

//THIS DOES NOT CLOSE THE FILE DESCRIPTOR, ONLY CLEARS, AND IS EQUIVILANT TO SETTING A FILE 
//		DESCRIPTOR TO -1.
CRX__LIB__PRIVATE_C_FUNCTION() void crx_c_os_fileSystem_private_posix_clear(
		Crx_C_Os_FileSystem_Internal_Posix_FileHandle const * CRX_NOT_NULL pFileHandle)
{
	pFileHandle->gFileDescriptor = -1;
	crx_c_string_empty(&(pFileHandle->gFullPath));
}

/*
freebsd: #define	AT_SYMLINK_NOFOLLOW	0x0200
linux: #define AT_SYMLINK_NOFOLLOW	0x100 
macos: #define AT_SYMLINK_NOFOLLOW     0x0020
*/
/*
	WARNING: THIS FUNCTION TEMPORARILY MODIFIES pFileHandle; IT IS NOT THREAD SAFE.
*/
CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_internal_posix_fstatat(
		Crx_C_Os_FileSystem_Internal_Posix_FileHandle * CRX_NOT_NULL pFileHandle,
		char const * pPartialPath, Crx_NonCrxed_Os_Posix_Dll_Libc_Stat * pStat,
		bool pIsNotToFollowSymlinks)
{
	static bool vIsSet = false;
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Fstatat vFunc_fstatat /*= ?*/;
	static Crx_NonCrxed_Os_Posix_Dll_Libc_StatFunc vFunc_stat /*= ?*/;
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Lstat vFunc_lstat /*= ?*/;
	bool vIsPartialPathEmpty = ((pPartialPath == NULL) || (strlen(pPartialPath) == 0) || 
			((strlen(pPartialPath) == 1) && ((*pPartialPath == '/') || (*pPartialPath == '\\'))));
	int32_t vErrorCode = 0;
	
	if(!vIsSet)
	{
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX)
		//LINUX EXPOSED A DIFFERENT SYMBOL IN THE PAST, __fxstatat, AND VERSIONED IT.
		if(crx_c_os_dll_loadSystemFunction("__fxstatat", 2, 4, 0, 0, 0) != NULL)
			{vFunc_fstatat = &crx_c_os_fileSystem_private_posix_linux_fstatat;}
		else
			{vFunc_fstatat = NULL;}
		vFunc_stat = &crx_c_os_fileSystem_private_posix_linux_stat;
		vFunc_lstat = &crx_c_os_fileSystem_private_posix_linux_lstat;
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
		vFunc_fstatat = crx_c_os_dll_loadSystemFunction("fstatat", 1, 1, 0, 0, 0);
		vFunc_stat = crx_c_os_dll_loadSystemFunction("stat", 1, 0, 0, 0, 0);
		vFunc_lstat = crx_c_os_dll_loadSystemFunction("lstat", 1, 0, 0, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
		vFunc_fstatat = crx_c_os_dll_loadSystemFunction("fstatat", 10, 10, 0, 0, 0);//MACOS >= 10.10
		vFunc_stat = ((Crx_NonCrxed_Os_Posix_Dll_Libc_StatFunc)&stat);
		vFunc_lstat = ((Crx_NonCrxed_Os_Posix_Dll_Libc_Lstat)&lstat);
	#endif
	
		vIsSet = true;
	}

	if(pFileHandle->gFileDescriptor != -1)
	{
		if(vFunc_fstatat != NULL)
		{
			if((*vFunc_fstatat)(pFileHandle->gFileDescriptor, 
					(vIsPartialPathEmpty ? "." : pPartialPath), pStat, 
					(pIsNotToFollowSymlinks ? AT_SYMLINK_NOFOLLOW : 0)) != 0)
				{vErrorCode = errno;}

			vIsDone = true;
		}
		else if(crx_c_string_getSize(&(pFileHandle->gFullPath)) == 0)
		{
			vErrorCode = EBADF;

			vIsDone = true;
		}
	}

	if(!vIsDone)
	{
		bool tIsNoError = true;
		size_t tLength = crx_c_string_getSize(&(pFileHandle->gFullPath));

		if(tIsNoError && !crx_c_string_isEndingWith2(&(pFileHandle->gFullPath), "/", false))
		{
			if(!crx_c_string_appendChar(&(pFileHandle->gFullPath), '/'))
				{tIsNoError = false;}
		}
		if(tIsNoError && !vIsPartialPathEmpty)
		{
			if(!crx_c_string_appendCString(&(pFileHandle->gFullPath), pPartialPath))
				{tIsNoError = false;}
		}

		if(tIsNoError)
		{
			if(crx_c_string_getSize(&(pFileHandle->gFullPath)) < PATH_MAX)
			{
				int32_t tResult = 0;

				if(pIsNotToFollowSymlinks)
				{
					tResult = (*vFunc_lstat)(crx_c_string_getCString(&(pFileHandle->gFullPath)), 
							pStat);
				}
				else
				{
					tResult = (*vFunc_stat)(crx_c_string_getCString(&(pFileHandle->gFullPath)), 
						pStat);
				}

				if(tResult != 0)
					{vErrorCode = errno;}
			}
			else
				{vErrorCode = ENAMETOOLONG;}
		}
		else
			{vErrorCode = ENOMEM;}
		
		crx_c_string_removeChars(&(pFileHandle->gFullPath), tLength, 
				crx_c_string_getSize(&(pFileHandle->gFullPath)) - tLength);
		
		vIsDone = true;
	}

	if(vErrorCode != 0)
	{
		errno = vErrorCode;

		return false;
	}
	else
		{return true;}
}
CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_internal_posix_fstat(
		Crx_C_Os_FileSystem_Internal_Posix_FileHandle * CRX_NOT_NULL pFileHandle,
		Crx_NonCrxed_Os_Posix_Dll_Libc_Stat * pStat)
{
	if(pFileHandle->gFileDescriptor != -1)
	{
		return (crx_c_os_fileSystem_private_posix_callFstat(pFileHandle->gFileDescriptor, pStat) ==
				0);
	}
	else
	{
		errno = EBADF;

		return false;
	}
}
CRX__LIB__PRIVATE_C_FUNCTION() int32_t crx_c_os_fileSystem_private_posix_callFstat(
		int32_t pFileDescriptor, Crx_NonCrxed_Os_Posix_Dll_Libc_Stat * pStat)
{
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Fstat vFunc_fstat /*= ?*/;
	int32_t vErrorCode = 0;
	
	if(vFunc_fstat == NULL)
	{
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX)
		//LINUX EXPOSED A DIFFERENT SYMBOL IN THE PAST, __fxstat, AND VERSIONED IT.
		if(crx_c_os_dll_loadSystemFunction("__fxstat", 2, 2, 5, 0, 0) != NULL)
			{vFunc_fstat = &crx_c_os_fileSystem_private_posix_linux_fstat;}
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
		vFunc_fstat = crx_c_os_dll_loadSystemFunction("fstat", 1, 0, 0, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
		vFunc_fstat = ((Crx_NonCrxed_Os_Posix_Dll_Libc_Fstat)&fstat);
	#endif
	}

	return (*vFunc_fstat)(pFileDescriptor, pStat);
}
CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_internal_posix_rawStat(
		char const * pCString, Crx_NonCrxed_Os_Posix_Dll_Libc_Stat * pStat,
		bool pIsNotToFollowSymlinks)
{
	static bool vIsSet = false;
	static Crx_NonCrxed_Os_Posix_Dll_Libc_StatFunc vFunc_stat /*= ?*/;
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Lstat vFunc_lstat /*= ?*/;
	int32_t vErrorCode = 0;
	
	if(!vIsSet)
	{
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX)
		vFunc_stat = &crx_c_os_fileSystem_private_posix_linux_stat;
		vFunc_lstat = &crx_c_os_fileSystem_private_posix_linux_lstat;
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
		vFunc_stat = crx_c_os_dll_loadSystemFunction("stat", 1, 0, 0, 0, 0);
		vFunc_lstat = crx_c_os_dll_loadSystemFunction("lstat", 1, 0, 0, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
		vFunc_stat = ((Crx_NonCrxed_Os_Posix_Dll_Libc_StatFunc)&stat);
		vFunc_lstat = ((Crx_NonCrxed_Os_Posix_Dll_Libc_Lstat)&lstat);
	#endif
	
		vIsSet = true;
	}

	if(strlen(pCString) < PATH_MAX)
	{
		int32_t tResult = 0;

		if(pIsNotToFollowSymlinks)
			{tResult = (*vFunc_lstat)(pCString, pStat);}
		else
			{tResult = (*vFunc_stat)(pCString, pStat);}

		if(tResult != 0)
			{vErrorCode = errno;}
	}
	else
		{vErrorCode = ENAMETOOLONG;}

	if(vErrorCode != 0)
	{
		errno = vErrorCode;

		return false;
	}
	else
		{return true;}
}


CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_private_posix_renameat(
		Crx_C_Os_FileSystem_Internal_Posix_FileHandle * CRX_NOT_NULL pFileHandle,
		char const * pPartialPath, 
		Crx_C_Os_FileSystem_Internal_Posix_FileHandle * CRX_NOT_NULL pFileHandle__new,
		char const * pPartialPath__new)
{
	static bool vIsSet = false;
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Renameat vFunc_renameat /*= ?*/;
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Rename vFunc_rename /*= ?*/;
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Open vFunc_open /*= ?*/;
	bool vIsPartialPathEmpty = ((pPartialPath == NULL) || (strlen(pPartialPath) == 0) ||
			((strlen(pPartialPath) == 1) && ((*pPartialPath == '/') || (*pPartialPath == '\\'))));
	bool vIsPartialPathEmpty__new = ((pPartialPath__new == NULL) || ((strlen(pPartialPath__new) == 1) &&
			((*pPartialPath__new == '/') || (*pPartialPath__new == '\\'))));
	int32_t vErrorCode = 0;
	
	if(!vIsSet)
	{
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX)
		//LINUX EXPOSED A DIFFERENT SYMBOL IN THE PAST, __fxstatat, AND VERSIONED IT.
		vFunc_renameat = crx_c_os_dll_loadSystemFunction("renameat", 2, 4, 0, 0, 0);
		vFunc_rename = crx_c_os_dll_loadSystemFunction("rename", 2, 2, 5, 0, 0);
		vFunc_open = crx_c_os_dll_loadSystemFunction("open", 2, 2, 5, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
		vFunc_renameat = crx_c_os_dll_loadSystemFunction("renameat", 1, 1, 0, 0, 0);
		vFunc_rename = crx_c_os_dll_loadSystemFunction("rename", 1, 0, 0, 0, 0);
		vFunc_open = crx_c_os_dll_loadSystemFunction("open", 1, 0, 0, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
		vFunc_renameat = crx_c_os_dll_loadSystemFunction("renameat", 10, 10, 0, 0, 0);//MACOS >= 10.10
		vFunc_rename = ((Crx_NonCrxed_Os_Posix_Dll_Libc_Rename)&rename);
		vFunc_open = ((Crx_NonCrxed_Os_Posix_Dll_Libc_Open)&open);
	#endif
	
		vIsSet = true;
	}

	if((pFileHandle->gFileDescriptor != -1) || (pFileHandle__new->gFileDescriptor != -1))
	{
		if(vFunc_renameat != NULL)
		{
			if(pFileHandle->gFileDescriptor == -1)
			{
				pFileHandle->gFileDescriptor = (*vFunc_open)(crx_c_string_getCString(
						&(pFileHandle->gFullPath)), 
						crx_c_os_fileSystem_internal_posix_amendOpenatFlags(
						O_RDONLY /*| O_DIRECTORY DOES NOT EXIST ON MACOS 10.7*/));
			}
			if(pFileHandle__new->gFileDescriptor == -1)
			{
				pFileHandle__new->gFileDescriptor = (*vFunc_open)(crx_c_string_getCString(
						&(pFileHandle__new->gFullPath)), 
						crx_c_os_fileSystem_internal_posix_amendOpenatFlags(
						O_RDONLY /*| O_DIRECTORY DOES NOT EXIST ON MACOS 10.7*/));
			}
			
			if((pFileHandle->gFileDescriptor != -1) && (pFileHandle__new->gFileDescriptor != -1))
			{
				if((*vFunc_renameat)(pFileHandle->gFileDescriptor, 
						(vIsPartialPathEmpty ? "." : pPartialPath), 
						pFileHandle__new->gFileDescriptor, 
						(vIsPartialPathEmpty__new ? "." : pPartialPath__new)) != 0)
					{vErrorCode = errno;}

				vIsDone = true;
			}
			else if((crx_c_string_getSize(pFileHandle->gFullPath) == 0) || 
					(crx_c_string_getSize(pFileHandle->gFullPath) >= PATH_MAX) || 
					(crx_c_string_getSize(pFileHandle__new->gFullPath) == 0) ||
					(crx_c_string_getSize(pFileHandle__new->gFullPath) >= PATH_MAX))
			{
				vErrorCode = EBADF;
				
				vIsDone = true;
			}
		}
		else if(((pFileHandle->gFileDescriptor != -1) &&
				(crx_c_string_getSize(pFileHandle->gFullPath) == 0)) || 
				((pFileHandle__new->gFileDescriptor != -1) &&
				(crx_c_string_getSize(pFileHandle__new->gFullPath) == 0)))
		{
			vErrorCode = EBADF;

			vIsDone = true;
		}
	}

	if(!vIsDone)
	{
		Crx_C_String tString /*= ?*/;
		Crx_C_String tString2 /*= ?*/;
		bool tIsNoError = true;
		size_t tLength = crx_c_string_getSize(&(pFileHandle->gFullPath));
		
		crx_c_string_construct(&tString);
		crx_c_string_construct(&tString2);
		
		tIsNoError = crx_c_string_appendString(&tString, &(pFileHandle->gFullPath));
		if(tIsNoError && !crx_c_string_isEndingWith2(&tString, "/", false))
		{
			if(!crx_c_string_appendChar(&tString, '/'))
				{tIsNoError = false;}
		}
		if(tIsNoError && !vIsPartialPathEmpty)
		{
			if(!crx_c_string_appendCString(&tString, pPartialPath))
				{tIsNoError = false;}
		}
			
		if(tIsNoError && !crx_c_string_appendString(&tString2, &(pFileHandle->gFullPath)))
			{tIsNoError = false;}
		if(tIsNoError && !crx_c_string_isEndingWith2(&tString2, "/", false))
		{
			if(!crx_c_string_appendChar(&tString2, '/'))
				{tIsNoError = false;}
		}
		if(tIsNoError && !vIsPartialPathEmpty__new)
		{
			if(!crx_c_string_appendCString(&tString2, pPartialPath__new))
				{tIsNoError = false;}
		}

		if(tIsNoError)
		{
			if((crx_c_string_getSize(&tString) < PATH_MAX) &&
					(crx_c_string_getSize(&tString2) < PATH_MAX))
			{
				if((*vFunc_rename)(crx_c_string_getCString(&tString), 
						crx_c_string_getCString(&tString2)) != 0)
					{vErrorCode = errno;}
			}
			else
				{vErrorCode = ENAMETOOLONG;}
		}
		else
			{vErrorCode = ENOMEM;}
		
		vIsDone = true;
		
		crx_c_string_destruct(&tString);
		crx_c_string_destruct(&tString2);
	}

	if(vErrorCode != 0)
	{
		errno = vErrorCode;

		return false;
	}
	else
		{return true;}
}

//UNLIKE THE NATIVE FUNCTION, THIS WILL ATTEMPT TO AQUIRE AN EXCLUSIVE LOCK BEFORE DELETING
CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_private_posix_unlinkat(
		Crx_C_Os_FileSystem_Internal_Posix_FileHandle * CRX_NOT_NULL pFileHandle,
		char const * pPartialPath)
{
	static bool vIsSet = false;
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Unlinkat vFunc_unlinkat /*= ?*/;
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Unlink vFunc_unlink /*= ?*/;
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Rmdir vFunc_rmdir /*= ?*/;
	bool vIsPartialPathEmpty = ((pPartialPath == NULL) || (strlen(pPartialPath) == 0) || 
			(((strlen(pPartialPath) == 1) && ((*pPartialPath == '/') || (*pPartialPath == '\\'))));
	bool vIsADirectory = vIsPartialPathEmpty || (*(pPartialPath + strlen(pPartialPath) - 1) == '/') ||
			(*(pPartialPath + strlen(pPartialPath) - 1) == '\\');
	Crx_C_Os_FileSystem_Internal_Posix_FileHandle vFileHandle /*= ?*/;
	int32_t vErrorCode = 0;
	
	if(!vIsSet)
	{
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX)
		vFunc_unlinkat = crx_c_os_dll_loadSystemFunction("unlinkat", 2, 4, 0, 0, 0);
		vFunc_unlink = crx_c_os_dll_loadSystemFunction("unlink", 2, 2, 5, 0, 0);
		vFunc_rmdir = crx_c_os_dll_loadSystemFunction("rmdir", 2, 2, 5, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
		vFunc_unlinkat = crx_c_os_dll_loadSystemFunction("unlinkat", 1, 1, 0, 0, 0);
		vFunc_unlink = crx_c_os_dll_loadSystemFunction("unlink", 1, 0, 0, 0, 0);
		vFunc_rmdir = crx_c_os_dll_loadSystemFunction("rmdir", 1, 0, 0, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
		vFunc_unlinkat = crx_c_os_dll_loadSystemFunction("unlinkat", 10, 10, 0, 0, 0);//MACOS >= 10.10
		vFunc_unlink = ((Crx_NonCrxed_Os_Posix_Dll_Libc_Unlink)&unlink);
		vFunc_rmdir = ((Crx_NonCrxed_Os_Posix_Dll_Libc_Rmdir)&rmdir);
	#endif
	
		vIsSet = true;
	}
	crx_c_os_fileSystem_internal_posix_fileHandle_construct(&vFileHandle);
	
	if(crx_c_os_fileSystem_internal_posix_openat(&vFileHandle, pFileHandle,
			pPartialPath, O_RDONLY /*| O_DIRECTORY DOES NOT EXIST ON MACOS 10.7*/, 0))
	{
		if(!crx_c_os_fileSystem_private_posix_flock(&vFileHandle, LOCK_EX|LOCK_NB))
			{vErrorCode = errno;}
	}
	else
		{vErrorCode = errno;}
	
	if((vErrorCode == 0) && (pFileHandle->gFileDescriptor != -1))
	{
		if(vFunc_unlinkat != NULL)
		{
			if((*vFunc_unlinkat)(pFileHandle->gFileDescriptor, 
					(vIsPartialPathEmpty ? "." : pPartialPath), 
					(vIsADirectory ? AT_REMOVEDIR : 0)) != 0)
				{vErrorCode = errno;}

			vIsDone = true;
		}
		else if((pFileHandle->gFileDescriptor != -1) &&
				(crx_c_string_getSize(pFileHandle->gFullPath) == 0))
		{
			vErrorCode = EBADF;

			vIsDone = true;
		}
	}

	if((vErrorCode == 0) && !vIsDone)
	{
		Crx_C_String tString /*= ?*/;
		bool tIsNoError = true;
		size_t tLength = crx_c_string_getSize(&(pFileHandle->gFullPath));
		
		crx_c_string_construct(&tString);
		
		tIsNoError = crx_c_string_appendString(&tString, &(pFileHandle->gFullPath));
		if(tIsNoError && !crx_c_string_isEndingWith2(&tString, "/", false))
		{
			if(!crx_c_string_appendChar(&tString, '/'))
				{tIsNoError = false;}
		}
		if(tIsNoError && !vIsPartialPathEmpty)
		{
			if(!crx_c_string_appendCString(&tString, pPartialPath))
				{tIsNoError = false;}
		}

		if(tIsNoError)
		{
			if(crx_c_string_getSize(&tString) < PATH_MAX)
			{
				if(vIsADirectory)
				{
					if((*vFunc_rmdir)(crx_c_string_getCString(&tString)) != 0)
						{vErrorCode = errno;}
				}
				else
				{
					if((*vFunc_unlink)(crx_c_string_getCString(&tString)) != 0)
						{vErrorCode = errno;}
				}
			}
			else
				{vErrorCode = ENAMETOOLONG;}
		}
		else
			{vErrorCode = ENOMEM;}
		
		vIsDone = true;
		
		crx_c_string_destruct(&tString;
	}

	crx_c_os_fileSystem_internal_posix_close(&vFileHandle);

	crx_c_os_fileSystem_internal_posix_fileHandle_destruct(&vFileHandle);

	if(vErrorCode != 0)
	{
		errno = vErrorCode;

		return false;
	}
	else
		{return true;}
}

/*
	WARNING: THIS IS EQUIVILANT TO fdopendir(dup(tFileDescriptor))
	WARNING: IT IS ASSUMED THAT DIR WILL NEVER BE DEREFERENCED EXCEPT BY OTHER SYSTEM FUNCTIONS.
*/
CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_internal_posix_fdopendir(
		DIR * * CRX_NOT_NULL pReturn,
		Crx_C_Os_FileSystem_Internal_Posix_FileHandle * CRX_NOT_NULL pFileHandle)
{
	static bool vIsSet = false;
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Fdopendir vFunc_fdopendir /*= ?*/;
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Opendir vFunc_opendir /*= ?*/;
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Dup vFunc_dup /*= ?*/;
	int32_t vErrorCode = 0;
	DIR * vReturn /*= ?*/;

	if(!vIsSet)
	{
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX)
		vFunc_fdopendir = crx_c_os_dll_loadSystemFunction("fdopendir", 2, 4, 0, 0, 0);
		vFunc_opendir = crx_c_os_dll_loadSystemFunction("opendir", 2, 2, 5, 0, 0);
		vFunc_dup = crx_c_os_dll_loadSystemFunction("dup", 2, 2, 5, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
		vFunc_fdopendir = crx_c_os_dll_loadSystemFunction("fdopendir", 1, 1, 0, 0, 0);
		vFunc_opendir = crx_c_os_dll_loadSystemFunction("opendir", 1, 0, 0, 0, 0);
		vFunc_dup = crx_c_os_dll_loadSystemFunction("dup", 1, 0, 0, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
		vFunc_fdopendir = crx_c_os_dll_loadSystemFunction("fdopendir", 10, 10, 0, 0, 0);//MACOS >= 10.10
		vFunc_opendir = ((Crx_NonCrxed_Os_Posix_Dll_Libc_Open)&opendir);
		vFunc_dup = ((Crx_NonCrxed_Os_Posix_Dll_Libc_Dup)&dup);
	#endif
	
		vIsSet = true;
	}

	if(pFileHandle->gFileDescriptor != -1)
	{
		if(vFunc_fdopendir != NULL)
		{
			vReturn = (*vFunc_fdopendir)((*vFunc_dup)(tFileDescriptor)); //WITHOUT dup() FILE DESCRUPTOR IS LOST

			if(vReturn == NULL)
				{vErrorCode = errno;}

			vIsDone = true;
		}
		else if(crx_c_string_getSize(&(pFileHandle->gFullPath)) == 0)
		{
			vErrorCode = EBADF;

			vIsDone = true;
		}
	}

	if(!vIsDone)
	{
		Crx_C_String tString /*= ?*/;
		bool tIsNoError = true;

		crx_c_construct(&tString);

		if(!crx_c_string_appendString(&tString, &(pFileHandle->gFullPath)))
			{tIsNoError = false;}
		if(tIsNoError && !crx_c_string_isEndingWith2(&tString, "/", false))
		{
			if(!crx_c_string_appendChar(&tString, '/'))
				{tIsNoError = false;}
		}
		if(tIsNoError && !crx_c_string_appendCString(&tString, pPath))
			{tIsNoError = false;}
		if(tIsNoError && !crx_c_string_ensureCapacity(&tString, 
				crx_c_getLength(&tString) + 4))
			{tIsNoError = false;}

		if(tIsNoError)
		{
			if(crx_c_string_getSize(&tString) < PATH_MAX)
			{
				vReturn = (*vFunc_opendir)(crx_c_string_getCString(&tString));
				
				if(vReturn == NULL)
					{vErrorCode = errno;}
			}
			else
				{vErrorCode = ENAMETOOLONG;}
		}
		else
			{vErrorCode = ENOMEM;}
		
		if(!tIsNoError)
			{crx_c_string_empty(&(pReturn->gFullPath));}
		
		vIsDone = true;
	}

	*pReturn = vReturn;
	
	if(vErrorCode != 0)
	{
		errno = vErrorCode;

		return false;
	}
	else
		{return true;}
}

//NOTE: UNLIKE readdir(), THIS DOES NOT REQUIRE SETTING errno TO ZERO BEFORE CALLING IT. IT SETS IT
//		ITSELF BEFORE PROCEEDING.
CRX__LIB__PRIVATE_C_FUNCTION() Crx_NonCrxed_Os_Posix_Dll_Libc_Dirent * 
		crx_c_os_fileSystem_internal_posix_readdir(DIR * pDir)
{
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Readdir vFunc_readdir /*= ?*/;

	if(vFunc_readdir == NULL)
	{
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX)
		vFunc_readdir = crx_c_os_dll_loadSystemFunction("readdir", 2, 2, 5, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
		vFunc_readdir = crx_c_os_dll_loadSystemFunction("readdir", 1, 0, 0, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
		vFunc_readdir = ((Crx_NonCrxed_Os_Posix_Dll_Libc_Readdir)&readdir);
	#endif
	}

	errno = 0;
	
	return (*vFunc_readdir)(pDir);
}
CRX__LIB__PRIVATE_C_FUNCTION() int32_t crx_c_os_fileSystem_internal_posix_closedir(DIR * pDir)
{
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Closedir vFunc_closedir /*= ?*/;

	if(vFunc_closedir == NULL)
	{
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX)
		vFunc_closedir = crx_c_os_dll_loadSystemFunction("closedir", 2, 2, 5, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
		vFunc_closedir = crx_c_os_dll_loadSystemFunction("closedir", 1, 0, 0, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
		vFunc_closedir = ((Crx_NonCrxed_Os_Posix_Dll_Libc_Closedir)&closedir);
	#endif
	}

	if(pDir != NULL)
		{return (*vFunc_closedir)(pDir);}
	else
		{return 0;}
}

CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_internal_posix_doPwrite(
		int32_t pFileDescriptor, void const * pBuffer, size_t pCount, uint64_t pOffset)
{
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Pwrite vFunc_pwrite /*= ?*/;
	unsigned char const * vBuffer = ((unsigned char const *)pBuffer);
	uint64_t vOffset = pOffset;
	int64_t vLength = pCount;
	bool vReturn = true;

	if(vFunc_pwrite == NULL)
	{
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX)
		vFunc_pwrite = crx_c_os_dll_loadSystemFunction("pwrite", 2, 2, 5, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
		vFunc_pwrite = crx_c_os_dll_loadSystemFunction("pwrite", 1, 0, 0, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
		vFunc_pwrite = ((Crx_NonCrxed_Os_Posix_Dll_Libc_Pwrite)&pwrite);
	#endif
	}

	while(vLength > 0)
	{
		int64_t tLength = (*vFunc_pwrite)(pFileDescriptor, vBuffer, vLength, ((int64_t)vOffset));

		if(tLength >= 0)
		{
			vLength = vLength - tLength;
			vBuffer = vBuffer + tLength;
			vOffset = vOffset + tLength;
		}
		else
		{
			vReturn = false;

			break;
		}
	}

	return vReturn;
}
CRX__LIB__PRIVATE_C_FUNCTION() int64_t crx_c_os_fileSystem_internal_posix_doPread(
		int32_t pFileDescriptor, void const * pBuffer, size_t pCount, uint64_t pOffset)
{
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Pread vFunc_pread /*= ?*/;
	size_t vCount = pCount;
	int64_t vOffset = ((int64_t)pOffset);
	int64_t vReturn = 0;

	if(vFunc_pread == NULL)
	{
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX)
		vFunc_pread = crx_c_os_dll_loadSystemFunction("pread", 2, 2, 5, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
		vFunc_pread = crx_c_os_dll_loadSystemFunction("pread", 1, 0, 0, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
		vFunc_pread = ((Crx_NonCrxed_Os_Posix_Dll_Libc_Pread)&pread);
	#endif
	}

	while(vCount > 1073741824 /*1GB*/)
	{
		int64_t tCount = (*vFunc_pread)(pFileDescriptor, pBuffer, 1073741824, vOffset);

		if(tCount > 0)
		{
			/*IF vCount IS LESS THAN REQUESTED, THIS COULD POTENTIALLY BE AN ERROR, 
					NOT AN END OF FILE. HOWEVER, IT IS UNCLEAR HOW TO DIFFERENTIATE BETWEEN THE
					TWO CASES. */
			vCount = vCount - tCount;
			vOffset = vOffset + tCount;
			vReturn = vReturn + tCount;
		}
		else if(tCount == 0)
		{
			/*THIS COULD POTENTIALLY BE AN ERROR, NOT AN END OF FILE. HOWEVER, I WAS UNABLE TO FIND
					OUT WHAT EXACTLY IS REQUIRED TO DIFFERENTIATE THE TWO SIUTATIONS.*/
			break;
		}
		else
		{
			vReturn = tCount;

			break;
		} //ERROR
	}

	if(vReturn >= 0)
	{
		while(vCount > 0)
		{
			int64_t tCount = (*vFunc_pread)(pFileDescriptor, pBuffer, vCount, vOffset);
			
			if(tCount > 0)
			{
				/*IF vCount IS LESS THAN REQUESTED, THIS COULD POTENTIALLY BE AN ERROR, 
						NOT AN END OF FILE. HOWEVER, IT IS UNCLEAR HOW TO DIFFERENTIATE BETWEEN THE
						TWO CASES. */
				vCount = vCount - tCount;
				vOffset = vOffset + tCount;
				vReturn = vReturn + tCount;
			}
			else if(tCount == 0)
			{
				/*THIS COULD POTENTIALLY BE AN ERROR, NOT AN END OF FILE. HOWEVER, I WAS UNABLE TO FIND
						OUT WHAT EXACTLY IS REQUIRED TO DIFFERENTIATE THE TWO SIUTATIONS.*/
				break;
			}
			else
			{
				vReturn = tCount;

				break;
			} //ERROR
		}
	}

	return ((vReturn >= 0) ? vReturn : -1);
}
/*
	uint32_t IS USED FOR pLength INSTEAD OF size_t TO MATCH THE LIMITATION ON WINDOWS.
*/
CRX__LIB__PRIVATE_C_FUNCTION() void * crx_c_os_fileSystem_internal_posix_mmap(void * pAddress, 
		uint32_t pLength, int32_t pProtFlags, int32_t pFlags, int32_t pFileDescriptor, 
		int64_t pOffset)
{
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Mmap vFunc_mmap /*= ?*/;

	if(vFunc_mmap == NULL)
	{
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX)
		vFunc_mmap = crx_c_os_dll_loadSystemFunction("mmap", 2, 2, 5, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
		vFunc_mmap = crx_c_os_dll_loadSystemFunction("mmap", 1, 0, 0, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
		vFunc_mmap = ((Crx_NonCrxed_Os_Posix_Dll_Libc_Mmap)&mmap);
	#endif
	}

	if(pLength != 0)
		{return (*vFunc_mmap)(pAddress, pLength, pProtFlags, pFlags, pFileDescriptor, pOffset);}
	else
	{
		errno = EINVAL;

		return false;
	}
}
/*
	uint32_t IS USED FOR pLength INSTEAD OF size_t TO MATCH THE LIMITATION ON WINDOWS.
*/
CRX__LIB__PRIVATE_C_FUNCTION() int32_t crx_c_os_fileSystem_internal_posix_munmap(void * pAddress, 
		uint32_t pLength)
{
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Munmap vFunc_munmap /*= ?*/;

	if(vFunc_munmap == NULL)
	{
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX)
		vFunc_munmap = crx_c_os_dll_loadSystemFunction("munmap", 2, 2, 5, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
		vFunc_munmap = crx_c_os_dll_loadSystemFunction("munmap", 1, 0, 0, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
		vFunc_munmap = ((Crx_NonCrxed_Os_Posix_Dll_Libc_Munmap)&munmap);
	#endif
	}

	return (*vFunc_munmap)(pAddress, pLength);
}
/*
	uint32_t IS USED FOR pLength INSTEAD OF size_t TO MATCH THE LIMITATION ON WINDOWS.
	
	IF ALL MAPPING IS DONE WITH THE FLAG MAP_SHARED WHEN USING mmap(), THEN 
			pIsToInvalidateTheCacheItselfInstead SHOULD PROBABLY BE ALWAYS false, UNLESS THE
			CALLER IS SOMEONE WHO WROTE TO THE FILE USING OTHER MEANS, MEANING SOMETHING LIKE
			THE POSIX write(). THIS WOULD CAUSE THE PROCESSES THAT HAVE THE FILE MAPPED TO DISCARD
			WHAT THEY HAVE AND RELOAD FROM THE DISK (WHEN A DISK IS USED, WHICH IS ALWAYS
			ASSUMED UNDER THIS NAME SAPCE).
*/
CRX__LIB__PRIVATE_C_FUNCTION() int32_t crx_c_os_fileSystem_internal_posix_msync(void * pAddress, 
		uint32_t pLength, bool pIsImmediate, bool pIsToInvalidateTheCacheItselfInstead)
{
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Msync vFunc_msync /*= ?*/;
	int32_t vFlags = (pIsImmediate ? MS_SYNC : MS_ASYNC);

	if(vFunc_msync == NULL)
	{
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX)
		vFunc_msync = crx_c_os_dll_loadSystemFunction("msync", 2, 2, 5, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
		vFunc_msync = crx_c_os_dll_loadSystemFunction("msync", 1, 0, 0, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
		vFunc_msync = ((Crx_NonCrxed_Os_Posix_Dll_Libc_Msync)&msync);
	#endif
	}
	
	if(pIsToInvalidateTheCacheItselfInstead)
		{vFlags = vFlags | MS_INVALIDATE;}

	return (*vFunc_msync)(pAddress, pLength, pFlags);
}
CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_internal_posix_resizeFile(
		int32_t pFileDescriptor, bool pIsToFill, uint64_t pLength)
{
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Ftruncate vFunc_ftruncate /*= ?*/;
	static Crx_NonCrxed_Os_Posix_Dll_Libc_PosixFallocate vFunc_posixFallocate /*= ?*/;
	int32_t vErrorCode = 0;
	bool vIsNotDone = true;

	if(vFunc_ftruncate == NULL)
	{
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX)
		vFunc_ftruncate = crx_c_os_dll_loadSystemFunction("ftruncate", 2, 2, 5, 0, 0);
		vFunc_posixFallocate = crx_c_os_dll_loadSystemFunction("posix_fallocate", 2, 2, 5, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
		vFunc_ftruncate = crx_c_os_dll_loadSystemFunction("ftruncate", 1, 0, 0, 0, 0);
		vFunc_posixFallocate = crx_c_os_dll_loadSystemFunction("posix_fallocate", 1, 2, 0, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
		vFunc_ftruncate = ((Crx_NonCrxed_Os_Posix_Dll_Libc_Ftruncate)&ftruncate);
		vFunc_posixFallocate = NULL;
	#endif
	}

	if(pIsToFill || (vFunc_posixFallocate == NULL))
	{
		Crx_NonCrxed_Os_Posix_Dll_Libc_Stat tStat;
		
		if((crx_c_os_fileSystem_private_posix_callFstat(pFileDescriptor, &vStat) == 0) &&
				(vStat.st_size >= 0))
		{
			if(pLength > vStat.st_size)
			{
				if(vFunc_posixFallocate != NULL)
				{
					if((*vFunc_posixFallocate)(pFileDescriptor, ((uint64_t)vStat.st_size) - 1,
							((uint64_t)vStat.st_size) - pLength) == 0)
						{vIsNotDone = false;}
				}
				
				if(vIsNotDone)
				{
					if((*vFunc_ftruncate)(pFileDescriptor, pLength) == 0)
					{
						unsigned char vBuffer[16384];
						uint64_t tOffset = ((uint64_t)vStat.st_size) - 1;
						int64_t tLength = pLength - ((uint64_t)vStat.st_size);

						memset(&(vBuffer[0]), 0, 16384);

						/*
							pwritev() MIGHT BE BENEFICIAL FOR FREEBSD 8.X HERE BUT NOT MACOS WHERE
									IT DOES NOT EXIST WHEN NEEDED.
							THERE IS A FALLBACK FOR MACOS WHERE ONE WOULD USE fcntl() BUT IT USES
									NON STANDARD OPTIONS, AND DOES NOT SEEM GUARANTEED ON ALL
									FILESYSTEMS OTHER THAN THE NATIVE ONES.
						*/
						while(tLength >= 16384)
						{
							if(crx_c_os_fileSystem_internal_posix_doPwrite(
									pFileDescriptor, &(vBuffer[0]), 16384, tOffset))
							{
								tLength = tLength - 16384;
								tOffset = tOffset + 16384;
							}
							else
							{
								vErrorCode = errno;
								vIsNotDone = false;
							}
						}

						if(vErrorCode == 0)
						{
							if(tLength > 0)
							{
								if(!crx_c_os_fileSystem_internal_posix_doPwrite(
										pFileDescriptor, &(vBuffer[0]), tLength, tOffset))
								{
									vErrorCode = errno;
									
									break;
								}
							}
						}
					}
					else
						{vErrorCode = errno;}

					vIsNotDone = false;
				}
			}
		}
		else
			{vErrorCode = errno;}
	}

	if(!vIsNotDone && ((*vFunc_ftruncate)(pFileDescriptor, pLength) != 0))
	{
		vErrorCode = errno;
		vIsNotDone = false;
	}

	if(vErrorCode != 0)
		{errno = vErrorCode;}

	return (vErrorCode == 0);
}

/*
	UPDATE MODES:
		0: NO UPDATE
		1: UPDATE FROM pTimes
		2: UPDATE FROM CURRENT TIME
		
	WARNING: IF YOU DO NOT MODIFY A TIME STAMP, IT CAN STILL GET AFFECTED BY HAVING
			ITS NANO SECONDS COMPONENT SET TO ZERO. THE FOLLOWING ONLY GUARANTEES SECOND RESOLUTION
			EVEN IF THE UNDERLYING FILE SYSTEM SUPPORTS NANO SECONDS, ALTHOUGH SOMETIMES YOU MIGHT 
			GET NANO SECOND RESOLUTION. THIS IS BECAUSE THE IMPLEMENTATION FAVORS FUNCTIONS THAT 
			DO NOT USE NANO SECONDS. REMEMBER, THAT CRX OS ULTIMATELY ONLY SUPPORTS A RESOLUTION OF
			2 SECONDS FOR MODIFICATION DATE, AND DOES NOT SUPPORT ACCESS DATE AT ALL.
*/
CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_private_posix_futimesat(
		Crx_C_Os_FileSystem_Internal_Posix_FileHandle * CRX_NOT_NULL pFileHandle, 
		char const * pPartialPath, Crx_C_Os_FileSystem_Time const * pTimes /*pTimes[2]*/,
		uint32_t pAccessTimeStampUpdateMode, uint32_t pModifiedTimeStampUpdateMode)
{
	static bool vIsSet = false;
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Futimesat vFunc_futimesat /*= ?*/;
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Futimes vFunc_futimes /*= ?*/;
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Utimensat vFunc_utimensat /*= ?*/;
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Utimes vFunc_utimes /*= ?*/;
	bool vIsPartialPathEmpty = ((pPartialPath == NULL) || (strlen(pPartialPath) == 0) || 
			((strlen(pPartialPath) == 1) && ((*pPartialPath == '/') || (*pPartialPath == '\\'))));
	int32_t vErrorCode = 0;

	if(!vIsSet)
	{
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX)
		vFunc_futimesat = crx_c_os_dll_loadSystemFunction("futimesat", 2, 4, 0, 0, 0);
		vFunc_futimes = crx_c_os_dll_loadSystemFunction("futimes", 2, 3, 0, 0, 0);//NOT NEEDED
		vFunc_utimensat = crx_c_os_dll_loadSystemFunction("utimensat", 2, 6, 0, 0, 0);//NOT NEEDED
		vFunc_utimes = crx_c_os_dll_loadSystemFunction("utimes", 2, 2, 5, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
		vFunc_futimesat = crx_c_os_dll_loadSystemFunction("futimesat", 1, 1, 0, 0, 0);
		vFunc_futimes = crx_c_os_dll_loadSystemFunction("futimes", 1, 0, 0, 0, 0);//NOT NEEDED
		vFunc_utimensat = crx_c_os_dll_loadSystemFunction("utimensat", 1, 4, 0, 0, 0);//NOT NEEDED
		vFunc_utimes = crx_c_os_dll_loadSystemFunction("utimes", 1, 0, 0, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
		vFunc_futimesat = NULL;
		vFunc_futimes = ((Crx_NonCrxed_Os_Posix_Dll_Libc_Futimes)&futimes);
		vFunc_utimensat = crx_c_os_dll_loadSystemFunction("utimensat", 10, 13, 0, 0, 0);
		vFunc_utimes = ((Crx_NonCrxed_Os_Posix_Dll_Libc_Utimes)&utimes);
	#endif
	
		vIsSet = true;
	}
	
	//leave futimes to last. macos 10.7 - 10.12. we leave this to last because it requires explicit
	//		opening.
	
	if(pFileHandle->gFileDescriptor != -1)
	{
		if(vFunc_futimesat != NULL)
		{
			Crx_NonCrxed_Os_Posix_Dll_Libc_Stat tStat /*= ?*/;
			int64_t tAccess_seconds = 0;
			int64_t tModification_seconds = 0;
			uint64_t tNanoSeconds = 0;
			Crx_NonCrxed_Os_Posix_Dll_Libc_Timeval tTimevals[2];
			
			CRX__SET_TO_ZERO(Crx_NonCrxed_Os_Posix_Dll_Libc_Stat, tStat);
			
			if((pAccessTimeStampUpdateMode == 0) || (pModifiedTimeStampUpdateMode == 0))
			{
				crx_c_os_fileSystem_internal_posix_fstatat(pFileHandle, pPartialPath, &tStat, false);
			
				//WE HOPE NANOSECONDS WOULD NOT BE CHANGED
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX)
				tAccess_seconds = tStat.st_atime;
				tModification_seconds = tStat.st_mtime;
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
				//THE FOLLOWING LIKELY USES MACROS FOR st_atime AND st_mtime
				tAccess_seconds = tStat.st_atime;
				tModification_seconds = tStat.st_mtime;
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
				//THE FOLLOWING LIKELY USES MACROS FOR st_atime AND st_mtime
				tAccess_seconds = tStat.st_atime;
				tModification_seconds = tStat.st_mtime;
	#endif
			}
			
			if(pAccessTimeStampUpdateMode == 1)
				{crx_c_os_fileSystem_time_private_getAsTimeval(pTimes, &(tTimevals[0]));}
			else if(pAccessTimeStampUpdateMode == 2)
			{
				crx_c_os_fileSystem_private_posix_time(&(tTimevals[0].tv_sec));
				tTimevals[0].tv_usec = 0;
			}
			else
			{
				tTimevals[0].tv_sec = tAccess_seconds;
				tTimevals[0].tv_usec = 0;
			}
			
			if(pModifiedTimeStampUpdateMode == 1)
				{crx_c_os_fileSystem_time_private_getAsTimeval((pTimes + 1), &(tTimevals[1]));}
			else if(pModifiedTimeStampUpdateMode == 2)
			{
				crx_c_os_fileSystem_private_posix_time(&(tTimevals[1].tv_sec));
				tTimevals[1].tv_usec = 0;
			}
			else
			{
				tTimevals[1].tv_sec = tModification_seconds;
				tTimevals[1].tv_usec = 0;
			}
			
			if((*vFunc_futimesat)(pFileHandle->gFileDescriptor, 
					(vIsPartialPathEmpty ? "." : pPartialPath), &(tTimevals[0])) != 0)
				{vErrorCode = errno;}

			vIsDone = true;
		}
		else if(vFunc_utimensat != NULL)
		{
			/*
				utimensat() HAS BUGS ON LINUX, WHICH IS WHY WE ATTEMPT TO USE IT SECOND.
						IT WAS INTRODUCED IN LINUX V2.6.22, GLIBC V2.6, AND HAS BUGS IN LINUX BEFORE
						V2.6.26. NOTE THAT futimesat() WAS INTRODUCED IN LINUX V2.6.16, GLIBC V2.4
			*/
			Crx_NonCrxed_Os_Posix_Dll_Libc_Timespec tTimespecs[2];

			if(pAccessTimeStampUpdateMode == 1)
				{crx_c_os_fileSystem_time_private_getAsTimespec(pTimes, &(tTimespecs[0]));}
			else if(pAccessTimeStampUpdateMode == 2)
			{
				tTimespecs[0].tv_sec = 0;
				tTimespecs[0].tv_usec = UTIME_NOW;
			}
			else
			{
				tTimespecs[0].tv_sec = 0;
				tTimespecs[0].tv_usec = UTIME_OMIT;
			}

			if(pModifiedTimeStampUpdateMode == 1)
				{crx_c_os_fileSystem_time_private_getAsTimespec((pTimes + 1), &(tTimespecs[1]));}
			else if(pModifiedTimeStampUpdateMode == 2)
			{
				tTimespecs[1].tv_sec = 0;
				tTimespecs[1].tv_usec = UTIME_NOW;
			}
			else
			{
				tTimespecs[1].tv_sec = 0;
				tTimespecs[1].tv_usec = UTIME_OMIT;
			}

			if((*vFunc_utimensat)(pFileHandle->gFileDescriptor, 
					(vIsPartialPathEmpty ? "." : pPartialPath), &(tTimespecs[0]), 0) != 0)
				{vErrorCode = errno;}

			vIsDone = true;
		}
		else if(crx_c_string_getSize(&(pFileHandle->gFullPath)) == 0)
		{
			vErrorCode = EBADF;

			vIsDone = true;
		}
	}
	
	if(!vIsDone)
	{
		bool tIsNoError = true;
		size_t tLength = crx_c_string_getSize(&(pFileHandle->gFullPath));

		if(tIsNoError && !crx_c_string_isEndingWith2(&(pFileHandle->gFullPath), "/", false))
		{
			if(!crx_c_string_appendChar(&(pFileHandle->gFullPath), '/'))
				{tIsNoError = false;}
		}
		if(tIsNoError && !vIsPartialPathEmpty)
		{
			if(!crx_c_string_appendCString(&(pFileHandle->gFullPath), pPartialPath))
				{tIsNoError = false;}
		}

		if(tIsNoError)
		{
			if(crx_c_string_getSize(&(pFileHandle->gFullPath)) < PATH_MAX)
			{
				int64_t tAccess_seconds = 0;
				int64_t tModification_seconds = 0;
				uint64_t tNanoSeconds = 0;
				Crx_NonCrxed_Os_Posix_Dll_Libc_Timeval tTimevals[2];
				
				if((pAccessTimeStampUpdateMode == 0) || (pModifiedTimeStampUpdateMode == 0))
				{
					Crx_NonCrxed_Os_Posix_Dll_Libc_Stat tStat /*= ?*/;
					
					CRX__SET_TO_ZERO(Crx_NonCrxed_Os_Posix_Dll_Libc_Stat, tStat);
					
					crx_c_os_fileSystem_internal_posix_fstatat(pFileHandle, pPartialPath, &tStat, false);
				
					//WE HOPE NANOSECONDS WOULD NOT BE CHANGED
	#if(linux)
					tAccess_seconds = tStat.st_atime;
					tModification_seconds = tStat.st_mtime;
	#elif(freebsd)
					//THE FOLLOWING LIKELY USES MACROS FOR st_atime AND st_mtime
					tAccess_seconds = tStat.st_atime;
					tModification_seconds = tStat.st_mtime;
	#elif(macos)
					//THE FOLLOWING LIKELY USES MACROS FOR st_atime AND st_mtime
					tAccess_seconds = tStat.st_atime;
					tModification_seconds = tStat.st_mtime;
	#endif
				}
				
				if(pAccessTimeStampUpdateMode == 1)
					{crx_c_os_fileSystem_time_private_getAsTimeval(pTimes, &(tTimevals[0]));}
				else if(pAccessTimeStampUpdateMode == 2)
				{
					crx_c_os_fileSystem_private_posix_time(&(tTimevals[0].tv_sec));
					tTimevals[0].tv_usec = 0;
				}
				else
				{
					tTimevals[0].tv_sec = tAccess_seconds;
					tTimevals[0].tv_usec = 0;
				}
				
				if(pModifiedTimeStampUpdateMode == 1)
					{crx_c_os_fileSystem_time_private_getAsTimeval((pTimes + 1), &(tTimevals[1]));}
				else if(pModifiedTimeStampUpdateMode == 2)
				{
					crx_c_os_fileSystem_private_posix_time(&(tTimevals[1].tv_sec));
					tTimevals[1].tv_usec = 0;
				}
				else
				{
					tTimevals[1].tv_sec = tModification_seconds;
					tTimevals[1].tv_usec = 0;
				}
				
				if((*vFunc_utimes)(crx_c_string_getCString(&(pFileHandle->gFullPath)), 
							&(tTimevals[0])) != 0)
					{vErrorCode = errno;}
			}
			else
				{vErrorCode = ENAMETOOLONG;}
		}
		else
			{vErrorCode = ENOMEM;}
		
		crx_c_string_removeChars(&(pFileHandle->gFullPath), tLength, 
				crx_c_string_getSize(&(pFileHandle->gFullPath)) - tLength);
		
		vIsDone = true;
	}

	if(vErrorCode != 0)
	{
		errno = vErrorCode;

		return false;
	}
	else
		{return true;}
}
CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_private_posix_futimes(
		Crx_C_Os_FileSystem_Internal_Posix_FileHandle * CRX_NOT_NULL pFileHandle,
		Crx_C_Os_FileSystem_Time const * pTimes /*pTimes[2]*/,
		uint32_t pAccessTimeStampUpdateMode, uint32_t pModifiedTimeStampUpdateMode)
{
	static bool vIsSet = false;
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Futimes vFunc_futimes /*= ?*/;
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Utimes vFunc_utimes /*= ?*/;
	int32_t vErrorCode = 0;

	if(!vIsSet)
	{
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX)
		vFunc_futimes = crx_c_os_dll_loadSystemFunction("futimes", 2, 3, 0, 0, 0);
		vFunc_utimes = crx_c_os_dll_loadSystemFunction("utimes", 2, 2, 5, 0, 0); //NOT NEEDED
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
		vFunc_futimes = crx_c_os_dll_loadSystemFunction("futimes", 1, 0, 0, 0, 0);
		vFunc_utimes = crx_c_os_dll_loadSystemFunction("utimes", 1, 0, 0, 0, 0); //NOT NEEDED
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
		vFunc_futimes = ((Crx_NonCrxed_Os_Posix_Dll_Libc_Futimes)&futimes);
		vFunc_utimes = ((Crx_NonCrxed_Os_Posix_Dll_Libc_Utimes)&utimes); //NOT NEEDED
	#endif
	
		vIsSet = true;
	}
	
	//leave futimes to last. macos 10.7 - 10.12. we leave this to last because it requires explicit
	//		opening.
	
	if(pFileHandle->gFileDescriptor != -1)
	{
		if(vFunc_futimes != NULL)
		{
			Crx_NonCrxed_Os_Posix_Dll_Libc_Stat tStat /*= ?*/;
			int64_t tAccess_seconds = 0;
			int64_t tModification_seconds = 0;
			uint64_t tNanoSeconds = 0;
			Crx_NonCrxed_Os_Posix_Dll_Libc_Timeval tTimevals[2];
			
			CRX__SET_TO_ZERO(Crx_NonCrxed_Os_Posix_Dll_Libc_Stat, tStat);
			
			if((pAccessTimeStampUpdateMode == 0) || (pModifiedTimeStampUpdateMode == 0))
			{
				crx_c_os_fileSystem_internal_posix_fstatat(pFileHandle, pPartialPath, &tStat, false);
			
				//WE HOPE NANOSECONDS WOULD NOT BE CHANGED
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX)
				tAccess_seconds = tStat.st_atime;
				tModification_seconds = tStat.st_mtime;
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
				//THE FOLLOWING LIKELY USES MACROS FOR st_atime AND st_mtime
				tAccess_seconds = tStat.st_atime;
				tModification_seconds = tStat.st_mtime;
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
				//THE FOLLOWING LIKELY USES MACROS FOR st_atime AND st_mtime
				tAccess_seconds = tStat.st_atime;
				tModification_seconds = tStat.st_mtime;
	#endif
			}
			
			if(pAccessTimeStampUpdateMode == 1)
				{crx_c_os_fileSystem_time_private_getAsTimeval(pTimes, &(tTimevals[0]));}
			else if(pAccessTimeStampUpdateMode == 2)
			{
				crx_c_os_fileSystem_private_posix_time(&(tTimevals[0].tv_sec));
				tTimevals[0].tv_usec = 0;
			}
			else
			{
				tTimevals[0].tv_sec = tAccess_seconds;
				tTimevals[0].tv_usec = 0;
			}
			
			if(pModifiedTimeStampUpdateMode == 1)
				{crx_c_os_fileSystem_time_private_getAsTimeval((pTimes + 1), &(tTimevals[1]));}
			else if(pModifiedTimeStampUpdateMode == 2)
			{
				crx_c_os_fileSystem_private_posix_time(&(tTimevals[1].tv_sec));
				tTimevals[1].tv_usec = 0;
			}
			else
			{
				tTimevals[1].tv_sec = tModification_seconds;
				tTimevals[1].tv_usec = 0;
			}
			
			if((*vFunc_futimes)(pFileHandle->gFileDescriptor, &(tTimevals[0])) != 0)
				{vErrorCode = errno;}

			vIsDone = true;
		}
		else if(crx_c_string_getSize(&(pFileHandle->gFullPath)) == 0)
		{
			vErrorCode = EBADF;

			vIsDone = true;
		}
	}
	else
	{
		//ADDED LATER. LIKE ALL FUNCTIONS THAT MATCH POSIX FUNCTIONS AND ACT DIRECTLY ON A FILE
		//		DESCRIPTOR, THIS FUNCTION MUST NOT ATTEMPT TO OPEN THE FILE IF THE FILE DESCRIPOT
		//		IS NOT ALREADY SET.
		vErrorCode = EBADF;

		vIsDone = true;
	}
	
	//MADE A MISTAKE. THE futimes() FUNCTION MUST EXIST ON ALL SUPPORTED SYSTEMS, AND IF FILE 
	//		DESCRIPTOR IS NOT OPENED, IS -1, IN THE FILE HANDLE, NO FALL BACK IS ALLOWED.
	#if(0)
	if(!vIsDone)
	{
		bool tIsNoError = true;
		size_t tLength = crx_c_string_getSize(&(pFileHandle->gFullPath));

		if(tIsNoError && !crx_c_string_isEndingWith2(&(pFileHandle->gFullPath), "/", false))
		{
			if(!crx_c_string_appendChar(&(pFileHandle->gFullPath), '/'))
				{tIsNoError = false;}
		}
		if(tIsNoError && !vIsPartialPathEmpty)
		{
			if(!crx_c_string_appendCString(&(pFileHandle->gFullPath), pPartialPath))
				{tIsNoError = false;}
		}

		if(tIsNoError)
		{
			if(crx_c_string_getSize(&(pFileHandle->gFullPath)) < PATH_MAX)
			{
				int64_t tAccess_seconds = 0;
				int64_t tModification_seconds = 0;
				uint64_t tNanoSeconds = 0;
				Crx_NonCrxed_Os_Posix_Dll_Libc_Timeval tTimevals[2];
				
				if((pAccessTimeStampUpdateMode == 0) || (pModifiedTimeStampUpdateMode == 0))
				{
					Crx_NonCrxed_Os_Posix_Dll_Libc_Stat tStat /*= ?*/;
					
					CRX__SET_TO_ZERO(Crx_NonCrxed_Os_Posix_Dll_Libc_Stat, tStat);
					
					crx_c_os_fileSystem_internal_posix_fstatat(pFileHandle, pPartialPath, &tStat, false);
				
					//WE HOPE NANOSECONDS WOULD NOT BE CHANGED
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX)
					tAccess_seconds = tStat.st_atime;
					tModification_seconds = tStat.st_mtime;
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
					//THE FOLLOWING LIKELY USES MACROS FOR st_atime AND st_mtime
					tAccess_seconds = tStat.st_atime;
					tModification_seconds = tStat.st_mtime;
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
					//THE FOLLOWING LIKELY USES MACROS FOR st_atime AND st_mtime
					tAccess_seconds = tStat.st_atime;
					tModification_seconds = tStat.st_mtime;
	#endif
				}
				
				if(pAccessTimeStampUpdateMode == 1)
					{crx_c_os_fileSystem_time_private_getAsTimeval(pTimes, &(tTimevals[0]));}
				else if(pAccessTimeStampUpdateMode == 2)
				{
					crx_c_os_fileSystem_private_posix_time(&(tTimevals[0].tv_sec));
					tTimevals[0].tv_usec = 0;
				}
				else
				{
					tTimevals[0].tv_sec = tAccess_seconds;
					tTimevals[0].tv_usec = 0;
				}
				
				if(pModifiedTimeStampUpdateMode == 1)
					{crx_c_os_fileSystem_time_private_getAsTimeval((pTimes + 1), &(tTimevals[1]));}
				else if(pModifiedTimeStampUpdateMode == 2)
				{
					crx_c_os_fileSystem_private_posix_time(&(tTimevals[1].tv_sec));
					tTimevals[1].tv_usec = 0;
				}
				else
				{
					tTimevals[1].tv_sec = tModification_seconds;
					tTimevals[1].tv_usec = 0;
				}
				
				if((*vFunc_utimes)(crx_c_string_getCString(&(pFileHandle->gFullPath)), 
							&(tTimevals[0])) != 0)
					{vErrorCode = errno;}
			}
			else
				{vErrorCode = ENAMETOOLONG;}
		}
		else
			{vErrorCode = ENOMEM;}
		
		crx_c_string_removeChars(&(pFileHandle->gFullPath), tLength, 
				crx_c_string_getSize(&(pFileHandle->gFullPath)) - tLength);
		
		vIsDone = true;
	}
	#endif
	
	if(vErrorCode != 0)
	{
		errno = vErrorCode;

		return false;
	}
	else
		{return true;}
}

CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_private_posix_flock(
		Crx_C_Os_FileSystem_Internal_Posix_FileHandle * CRX_NOT_NULL pFileHandle,
		int32_t pOp)
{
	if(pFileHandle->gFileDescriptor != -1)
		{return crx_c_os_fileSystem_private_posix_doFlock(pFileHandle->gFileDescriptor, pOp);}
	else
	{
		errno = EBADF;

		return false;
	}
}
CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_private_posix_rawFlock(
		int32_t pFileDescriptor, int32_t pOp)
{
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Flock vFunc_flock /*= ?*/;
	bool vReturn = true;

	if(vFunc_flock == NULL)
	{
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX)
		vFunc_flock = crx_c_os_dll_loadSystemFunction("flock", 2, 2, 5, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
		vFunc_flock = crx_c_os_dll_loadSystemFunction("flock", 1, 0, 0, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
		vFunc_flock = ((Crx_NonCrxed_Os_Posix_Dll_Libc_Flock)&flock);
	#endif
	}
	
	if((*vFunc_flock)(pFileDescriptor, pOp) == -1)
		{vReturn = false;}
	
	return vReturn;
}


CRX__LIB__PRIVATE_C_FUNCTION() void crx_c_os_fileSystem_private_posix_time(
		int64_t * CRX_NOT_NULL pTime)
{
	static Crx_NonCrxed_Os_Posix_Dll_Libc_TimeFunc vFunc_time /*= ?*/;

	if(vFunc_time == NULL)
	{
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX)
		vFunc_time = crx_c_os_dll_loadSystemFunction("time", 2, 2, 5, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
		vFunc_time = crx_c_os_dll_loadSystemFunction("time", 1, 0, 0, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
		vFunc_time = ((Crx_NonCrxed_Os_Posix_Dll_Libc_TimeFunc)&time);
	#endif
	}

	(*vFunc_time)(pTime);
}
CRX__LIB__PRIVATE_C_FUNCTION() void crx_c_os_fileSystem_private_posix_localtimeR( //localetime_r
		int64_t const * pTime, Crx_NonCrxed_Os_Posix_Dll_Libc_Tm * CRX_NOT_NULL pTm)
{
	static Crx_NonCrxed_Os_Posix_Dll_Libc_LocaltimeR vFunc_localtime /*= ?*/;

	if(vFunc_time == NULL)
	{
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX)
		vFunc_localtime = crx_c_os_dll_loadSystemFunction("localtime_r", 2, 2, 5, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
		vFunc_localtime = crx_c_os_dll_loadSystemFunction("localtime_r", 1, 0, 0, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
		vFunc_localtime = ((Crx_NonCrxed_Os_Posix_Dll_Libc_LocaltimeR)&localtime_r);
	#endif
	}

	(*vFunc_localtime)(pTime, pTm);
}


CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_private_posix_isAMountPointAt(
		Crx_C_Os_FileSystem_Internal_Posix_FileHandle * pFileHandle,
		char const * pPartialPath)
{
	bool vIsPartialPathEmpty = ((pPartialPath == NULL) || (strlen(pPartialPath) == 0) ||
			((strlen(pPartialPath) == 1) && ((*pPartialPath == '/') || (*pPartialPath == '\\'))));
	bool vReturn = false;

	if(crx_c_string_getSize(&(pFileHandle->gFullPath)) > 0)
	{
		bool tIsNoError = true;
		size_t tLength = crx_c_string_getSize(&(pFileHandle->gFullPath));

		if(tIsNoError && !crx_c_string_isEndingWith2(&(pFileHandle->gFullPath), "/", false))
		{
			if(!crx_c_string_appendChar(&(pFileHandle->gFullPath), '/'))
				{tIsNoError = false;}
		}
		if(tIsNoError && !vIsPartialPathEmpty)
		{
			if(!crx_c_string_appendCString(&(pFileHandle->gFullPath), pPartialPath))
				{tIsNoError = false;}
		}
		if(tIsNoError && !crx_c_string_ensureCapacity(&(pFileHandle->gFullPath), 
				crx_c_getLength(&(pFileHandle->gFullPath)) + 4))
			{tIsNoError = false;}

		if(tIsNoError)
			{vReturn = crx_c_os_fileSystem_private_posix_isAMountPoint2(&(pFileHandle->gFullPath));}
		else
			{vErrorCode = ENOMEM;}
		
		if(vErrorCode != 0)
		{
			crx_c_string_removeChars(&(pFileHandle->gFullPath), tLength, 
					crx_c_string_getSize(&(pFileHandle->gFullPath)) - tLength);
		}
	}
	else
	{
		//CHECKING BIND MOUNTS ON LINUX AND POSSIBLY FREEBSD BUT UNSURE IF THIS
		//		WORKS ON FREEBSD 
		//		{SRC:http://blog.schmorp.de/2016-03-03-detecting-a-mount-point.html}
		//REMEMBER, THE FOLLOWING IS DETECTING ALL MOUNTS, AND NOT ONLY BIND 
		//		MOUNTS. HOWEVER

		Crx_C_String tString /*= ?*/;
		Crx_C_String tString2 /*= ?*/;

		crx_c_string_construct(&tString);
		crx_c_string_construct(&tString2);
		
		crx_c_string_appendCString(&tString, "/.");
		crx_c_string_appendCString(&tString2, pPartialPath);
		crx_c_string_appendCString(&tString2, "/.");
		
		if((crx_c_os_fileSystem_private_posix_renameat(pFileHandle,
				crx_c_string_getCString(&tString), pFileHandle,
				crx_c_string_getCString(&tString2)) != 0) &&
				(errno == EXDEV))
			{vReturn = true;}

		crx_c_string_destruct(&tString);
		crx_c_string_destruct(&tString2);
	}
	
	return vReturn;
}
CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_private_posix_isAMountPoint(
		Crx_C_String * CRX_NOT_NULL pFullPath)
	{return (crx_c_os_fileSystem_posix_getMountInformationFor(pFullPath, 0, NULL) > 0);}

/*
	WARNING: IF YOU MODIFY, MAKE SURE TO UPDATE 
			crx_c_os_fileSystem_iterator_verifyRecordAgainst()
*/
CRX__LIB__PRIVATE_C_FUNCTION() bool
		crx_c_os_fileSystem_private_posix_isStatOfFileLikelySameAsOneFromDirent(
		Crx_NonCrxed_Os_Posix_Dll_Libc_Stat * pStat, 
		Crx_NonCrxed_Os_Posix_Dll_Libc_Dirent * CRX_NOT_NULL pDirent)
{
	#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
			(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
		//ON MACOS I AM HOPING THIS WORKS WHETHER DARWIN_FEATURE_64_BIT_INODE IS DEFINED OR NOT.
		return (pDirent->d_ino == pStat->st_ino);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
		//ON FREEBSD d_fileno APPEARS TO BE uint32_t AND st_ino IS uint32_t.
		//		IN 2017 BOTH CHANGED TO unint64_t. SEE 
		//				https://github.com/freebsd/freebsd-src/commit/69921123490b99c2588b0c743bc4af32bbe6601c
		return (pDirent->d_fileno == pStat->st_ino);
	#else
		assert(false);
	#endif
}

/*
	THIS ONLY SUPPORTS THE OPTIONS IN THE ENUM, ::crx::c::os::fileSystem::Sc
*/
CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_private_posix_sysconf(
		int64_t * CRX_NOT_NULL pReturn, int32_t pNameId)
{
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Sysconf vFunc_sysconf /*= ?*/;
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Getpagesize vFunc_getpagesize /*= ?*/;
	int64_t vReturnOfSysconf = -1;
	bool vReturn = true;

	if(vFunc_sysconf == NULL)
	{
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX)
		vFunc_sysconf = crx_c_os_dll_loadSystemFunction("sysconf", 2, 2, 5, 0, 0);
		//NOT REALY NEEDED
		vFunc_getpagesize = crx_c_os_dll_loadSystemFunction("getpagesize", 2, 2, 5, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
		vFunc_sysconf = crx_c_os_dll_loadSystemFunction("sysconf", 1, 0, 0, 0, 0);
		//NOT REALY NEEDED
		vFunc_getpagesize = crx_c_os_dll_loadSystemFunction("getpagesize", 1, 0, 0, 0, 0);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
		vFunc_sysconf = ((Crx_NonCrxed_Os_Posix_Dll_Libc_Sysconf)&sysconf);
		//NEEDED BECAUSE MACOS 10.7 MIGHT NOT HAVE SUPPORT FOR _SC_PAGESIZE.
		vFunc_getpagesize = ((Crx_NonCrxed_Os_Posix_Dll_Libc_Getpagesize)&getpagesize);
	#endif
	}

	errno = 0;

	if(pNameId == CRX__C__OS__FILE_SYSTEM__SC__PAGE_SIZE)
	{
		vReturnOfSysconf = (*vFunc_sysconf)(CRX__C__OS__FILE_SYSTEM__SC__PAGE_SIZE);

		if(errno == EINVAL)
		{
			if(vFunc_getpagesize != NULL)
			{
				/*
					WE ASSUME THAT IF _SC_PAGESIZE IS NOT AVAILABLE, THE PAGE SIZE IS NOT SO LARGE
							AS TO OVERLOW int32_t, MEANING THE RETURN TYPE OF getpagesize()
				*/
				vReturnOfSysconf = (*vFunc_getpagesize)();
				errno = 0;
			}
		}
	}
	else
		{errno = EINVAL;}

	*pReturn = vReturnOfSysconf;

	return (errno != 0);
}


	#if(CRX__NON_CRXED__OS__SETTING__IS_MACOS || \
			(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD))
CRX__LIB__PRIVATE_C_FUNCTION() int32_t crx_c_os_fileSystem_private_posix_getMntInfo(
		Crx_NonCrxed_Os_Posix_Dll_Libc_Statfs * CRX_NOT_NULL mntbufp, int32_t mode)
{//getmntinfo
	static Crx_NonCrxed_Os_Posix_Dll_Libc_Getmntinfo vFunc_getmntinfo /*= ?*/;
	Crx_NonCrxed_Os_Posix_Dll_Libc_Statfs * vStatfs /*= ?*/
	vReturn = 0;

	if(vFunc_getmntinfo == NULL)
	{
		#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
		vFunc_getmntinfo = crx_c_os_dll_loadSystemFunction("getmntinfo", 1, 0, 0, 0, 0);
		#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
		vFunc_getmntinfo = ((Crx_NonCrxed_Os_Posix_Dll_Libc_Getmntinfo)&getmntinfo);
		#endif
	}
	
	vReturn = (*vFunc_getmntinfo)(&vStatfs, mode); //WARNING: REQUIRES SYNCHRONIZATION (TODO)
	memcpy(mntbufp, vStatfs, sizeof(Crx_NonCrxed_Os_Posix_Dll_Libc_Statfs));

	return vReturn;
}
	#endif

#if(!defined(FILE_ATTRIBUTE_REPARSE_POINT))
	#define CRX__C__OS__FILESYSTEM__WIN32__PRIVATE__FILE_ATTRIBUTE_REPARSE_POINT 1024
#else
	#define CRX__C__OS__FILESYSTEM__WIN32__PRIVATE__FILE_ATTRIBUTE_REPARSE_POINT \
		FILE_ATTRIBUTE_REPARSE_POINT
#endif



//REMEMBER FUNCTIONS IN crx::c::os::fileSystem::win32 NAMESPACE STILL EXPECT THEIR STRINGS
//		TO BE IN UTF8, NOT THE UNDERLYING SYSTEM CODE PAGE. HOWEVER, SOME FUNCTIONS MIGHT HAVE
//		A BOOLEAN OPTION RELATED TO THE MATTER.

CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_internal_win32_win32FindData_construct(
		Crx_C_Os_FileSystem_Internal_Win32_Win32FindData * pThis)
{
	CRX__SET_TO_ZERO(Crx_C_Os_FileSystem_Internal_Win32_Win32FindData, (*pThis));

	crx_c_string_construct(&(pThis->cFileName));
	crx_c_string_construct(&(pThis->cAlternateFileName));
}
CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_internal_win32_win32FindData_destruct(
		Crx_C_Os_FileSystem_Internal_Win32_Win32FindData * pThis)
{
	crx_c_string_destruct(&(pThis->cFileName));
	crx_c_string_destruct(&(pThis->cAlternateFileName));
}
		
CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_internal_win32_win32FindData_setA(
		Crx_C_Os_FileSystem_Internal_Win32_Win32FindData * pThis,
		WIN32_FIND_DATAA const * CRX_NOT_NULL pWind32FindDataA);
{
	/*
	WARNING: UNDER WINDOWS 95, FROM WHAT I HAVE FOUND
			WIN32_FIND_DATAA::dwFileType, WIN32_FIND_DATAA::dwCreatorType, 
			WIN32_FIND_DATAA::wFinderFlags. ARE NOT THERE. HOWEVER THEY ARE AT THE BOTTOM
			OF THE STRUCTURE. HOPEFULLY IT IS SAFE TO AVOID THEM. HOWEVER, I DO NOT KNOW WHAT
			WOULD HAPPEN IF THE CODE IS COMPILED WITH AN OLDER MICROSOFT HEADER, THEN USED
			ON A NEWER WINDOWS VERSION. MICROSOFT MARKS THESE MEMBERS AS DEPRECATED, BUT
			CHANCES ARE ON SOME VERSIONS OF WINDOWS THEY WERE USED.
			IT APPEARS THAT THESE THREE EXTRA PARAMETERS ARE WHEN COMPILING ON A MAC COMPUTER.
	*/
	Crx_C_String vString /*= ?*/;
	bool vReturn = true;
	
	crx_c_string_construct(vString);

	crx_c_string_ensureCapacity(vString, 18);
	
	pThis->dwFileAttributes = pWind32FindDataA->dwFileAttributes;
	pThis->ftCreationTime = pWind32FindDataA->ftCreationTime;
	pThis->ftLastAccessTime = pWind32FindDataA->ftLastAccessTime;
	pThis->ftLastWriteTime = pWind32FindDataA->ftLastWriteTime;
	pThis->nFileSizeHigh = pWind32FindDataA->nFileSizeHigh;
	pThis->nFileSizeLow = pWind32FindDataA->nFileSizeLow;
	pThis->dwReserved0 = pWind32FindDataA->dwReserved0;
	pThis->dwReserved1 = pWind32FindDataA->dwReserved1;
	pThis->ftCreationTime = pWind32FindDataA->ftCreationTime;
	pThis->ftCreationTime = pWind32FindDataA->ftCreationTime;
	
	vReturn = (crx_c_string_appendCString(&vString, pWind32FindDataA->cFileName) &&
			crx_c_os_osString_unsafeConvertFromOsString(&(pThis->cFileName), &vString));
	
	if(vReturn)
	{
		crx_c_string_empty(vString);
		vReturn = (crx_c_string_appendCString(&vString, pWind32FindDataA->cAlternateFileName) &&
				crx_c_os_osString_unsafeConvertFromOsString(&(pThis->cAlternateFileName), 
						&vString));
	}
	
	crx_c_string_destruct(&vString);
	
	return vReturn;
}
CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_internal_win32_win32FindData_setW(
		Crx_C_Os_FileSystem_Internal_Win32_Win32FindData * pThis,
		Crx_NonCrxed_Os_Windows_Dll_Kernel32_Win32FindDataW const * CRX_NOT_NULL pWind32FindDataW)
{
	Crx_C_String vString /*= ?*/;
	bool vReturn = true;
	
	crx_c_string_construct(vString);
	
	crx_c_string_ensureCapacity(vString, (14 * sizeof(wchar_t)) + 4);
	
	pThis->dwFileAttributes = pWind32FindDataW->dwFileAttributes;
	pThis->ftCreationTime = pWind32FindDataW->ftCreationTime;
	pThis->ftLastAccessTime = pWind32FindDataW->ftLastAccessTime;
	pThis->ftLastWriteTime = pWind32FindDataW->ftLastWriteTime;
	pThis->nFileSizeHigh = pWind32FindDataW->nFileSizeHigh;
	pThis->nFileSizeLow = pWind32FindDataW->nFileSizeLow;
	pThis->dwReserved0 = pWind32FindDataW->dwReserved0;
	pThis->dwReserved1 = pWind32FindDataW->dwReserved1;
	pThis->ftCreationTime = pWind32FindDataW->ftCreationTime;
	pThis->ftCreationTime = pWind32FindDataW->ftCreationTime;
	
	vReturn = crx_c_string_appendChars(&vString, pWind32FindDataW->cFileName, 
					crx_c_fileSystem_wstrlen(pWind32FindDataW->cFileName) * sizeof(wchar_t)) &&
			crx_c_os_osString_unsafeConvertFromOsString(&(pThis->cFileName), &vString);
	
	if(vReturn)
	{
		crx_c_string_empty(&vString);
		vReturn = crx_c_string_appendChars(&vString, pWind32FindDataW->cAlternateFileName,
						crx_c_fileSystem_wstrlen(pWind32FindDataW->cAlternateFileName) * 
						sizeof(wchar_t)) &&
				crx_c_os_osString_unsafeConvertFromOsString(&(pThis->cAlternateFileName), &vString);
	}
	
	crx_c_string_destruct(&vString);
	
	return vReturn;
}


/*
	THE FOLLOWING 3 FUNCTIONS ARE MEANT TO SOLVE THE ISSUE OF THE TRAILING SLASH FOR DIRECTORIES
	AS REQUIRED BY THE SPECIFICATION, AND ITS EFFECT ON LENGTH LIMITATIONS OF FILE SYSTEM PATHS.

	
	
	ALL FUNCTIONS THAT WRAP SYSTEM FUNCTIONS IN THE crx::c::os::fileSystem::internal::win32 
	NAMESPACE MUST MAKE USE addressDirectoryTrailingSlash() AND 
	undoAddressingDirectoryTrailingSlash(). HOWEVER, BE CAREFUL WHEN FUNCTIONS IN 
	crx::c::os::fileSystem::internal::win32 ARE CALLING OTHERS FUNCTIONS IN 
	crx::c::os::fileSystem::internal::win32 
*/
CRX__LIB__PUBLIC_C_FUNCTION() Crx_C_String const * 
		crx_c_os_fileSystem_internal_win32_getSeperatorInOsEncoding()
{
	static Crx_C_String vRaw_seperator /*= ?*/;
	static bool vIsSet = false;

	if(!vIsSet)
	{
		Crx_C_String tTempString /*= ?*/;

		crx_c_string_construct(&vRaw_seperator);
		crx_c_string_construct(&tTempString);
		
		crx_c_string_appendCString(&tTempString, CRX__OS__FILE_SYSTEM__SEPERATOR_STRING);
		crx_c_os_osString_unsafeConvertToOsString(&vRaw_seperator, &tTempString);

		crx_c_string_destruct(&tTempString);
	}

	return &vRaw_seperator;
}
CRX__LIB__PUBLIC_C_FUNCTION() Crx_C_String const * 
		crx_c_os_fileSystem_internal_win32_addressDirectoryTrailingSlash(bool pIsNotUtf8,
		Crx_C_String * CRX_NOT_NULL pString, bool * CRX_NOT_NULL pIsADirectory)
{
	Crx_C_String const * vRaw_seperator = 
			crx_c_os_fileSystem_internal_win32_getSeperatorInOsEncoding();

	if(pIsNotUtf8)
	{
		if(!crx_c_string_isEqualTo(pString, &vRaw_seperator, false) && 
				crx_c_string_isEndingWith(pString, &vRaw_seperator, false))
		{
			*pIsADirectory = true;
			
			crx_c_string_removeChars(pString, crx_c_string_getSize(pString) - 
					crx_c_string_getSize(&vRaw_seperator), 
					crx_c_string_getSize(&vRaw_seperator));
		}
	}
	else
	{
		if(!crx_c_string_isEqualTo2(pString, CRX__OS__FILE_SYSTEM__SEPERATOR_STRING, false) && 
				crx_c_string_isEndingWith2(pString, CRX__OS__FILE_SYSTEM__SEPERATOR_STRING, false))
		{
			*pIsADirectory = true;
			
			crx_c_string_removeChars(pString, crx_c_string_getSize(pString) - 1, 1);
		}
	}
}
//WARNING: PASS THE SAME PARAMETERS PASSED TO addressDirectoryTrailingSlash();
CRX__LIB__PUBLIC_C_FUNCTION() 
		crx_c_os_fileSystem_internal_win32_undoAddressingDirectoryTrailingSlash(bool pIsNotUtf8,
		Crx_C_String * CRX_NOT_NULL pString, bool pIsADirectory)
{
	CRX_SCOPE_META
	if(!pIsADirectory)
		{return;}

	CRX_SCOPE
	Crx_C_String const * vRaw_seperator = 
			crx_c_os_fileSystem_internal_win32_getSeperatorInOsEncoding();

	if(pIsNotUtf8)
		{crx_c_string_appendString(pString, &vRaw_seperator);}
	else
		{crx_c_string_appendCString(pString, CRX__OS__FILE_SYSTEM__SEPERATOR_STRING);}
	CRX_SCOPE_END
}


/*
	THE FOLLOWING ARE ALLOWED VALUES PER THE REQUIREMENT OF WINDOWS 95 AND UP, AND NOTHING ELSE.
			CONSIDERATION WAS GIVEN FOR THE ABSTRACT AS DEFINED IN THIS LIBRARY, NOR FOR THE
			DIFFERENT FILE SYSTEMS. HOWEVER, FOR SOME VALUES CONSIDERATIONS FOR THESE WERE MADE
			THE VALUES REMOVED FROM THE LISTS BELOW. ALWAYS REFER TO THE PROJECT'S DESIGN NOTES.
			THE FOLLOWING WAS COMPILED USING MSDN 1998.
		pDwDesiredAccess: VALID VALUES ARE ONLY 
				"GENERIC_READ" OR "GENERIC_WRITE" OR "0"
		pDwShareMode: VALID VALUES ARE ONLY
				"FILE_SHARE_READ" OR "FILE_SHARE_WRITE" OR "0"
		pDwCreationDisposition: VALID VALUES ARE ONLY
				"CREATE_NEW" OR "CREATE_ALWAYS" OR "OPEN_EXISTING" OR "OPEN_ALWAYS" OR 
				"TRUNCATE_EXISTING"
		pDwFlagsAndAttributes: VALID VALUES ARE ONLY
				"FILE_ATTRIBUTE_ARCHIVE", "FILE_ATTRIBUTE_HIDDEN", "FILE_ATTRIBUTE_NORMAL"
				"FILE_ATTRIBUTE_OFFLINE", "FILE_ATTRIBUTE_READONLY", "FILE_ATTRIBUTE_SYSTEM",
				"FILE_ATTRIBUTE_TEMPORARY"
				
				"FILE_FLAG_WRITE_THROUGH", "FILE_FLAG_OVERLAPPED", "FILE_FLAG_NO_BUFFERING",
				"FILE_FLAG_RANDOM_ACCESS", "FILE_FLAG_SEQUENTIAL_SCAN", "FILE_FLAG_DELETE_ON_CLOSE",
				"FILE_FLAG_BACKUP_SEMANTICS", "FILE_FLAG_OPEN_REPARSE_POINT"
		pHTemplateFile: VALUE VALUES ARE ONLY
				NULL
				
	UNLIKE THE NATIVE SYSTEM FUNCTION, THIS FUNCTION IF PASSED NULL FOR pLpSecurityAttributes,
			WILL INTERNALLY PASS A SECURITY_ATTRIBUTES INSTANCE WITH THE MEMBER
			SECURITY_ATTRIBUTES::lpSecurityDescriptor TO NULL AND THE MEMBER
			SECURITY_ATTRIBUTES::bInheritHandle TO FALSE. THIS IS TO AVOID HANDLE INHERITANCE BY
			CHILD PROCESSES.
*/
CRX__LIB__PRIVATE_C_FUNCTION() HANDLE crx_c_os_fileSystem_private_win32_createFile(bool pIsNotUtf8,
		Crx_C_String * CRX_NOT_NULL pString, DWORD pDwDesiredAccess, DWORD pDwShareMode,
		SECURITY_ATTRIBUTES * pLpSecurityAttributes, DWORD pDwCreationDisposition, 
		DWORD pDwFlagsAndAttributes, HANDLE pHTemplateFile)
{
	static Crx_NonCrxed_Os_Windows_Dll_Kernel32_CreateFileW vFunc_createFileW = 
			crx_c_os_dll_loadSystemFunction("CreateFileW", 3, 51, 0, 0, 2);
	bool vIsADirectory = false;
	int32_t vErrorCode = 0;
	SECURITY_ATTRIBUTES vSecurityAttributes /*= ?*/;
	HANDLE vReturn = INVALID_HANDLE_VALUE;

	CRX__SET_TO_ZERO(SECURITY_ATTRIBUTES, vSecurityAttributes);
	
	crx_c_os_fileSystem_internal_win32_addressDirectoryTrailingSlash(pIsNotUtf8, pString, 
			&vIsADirectory);

	if(pLpSecurityAttributes == NULL)
	{
		vSecurityAttributes.lpSecurityDescriptor = NULL;
		vSecurityAttributes.bInheritHandle = false;
	}

	if(vFunc_createFileW != NULL)
	{
		if(pIsNotUtf8)
		{
			vReturn = (*vFunc_createFileW)((wchar_t const *)crx_c_string_getCString(pString), 
					pDwDesiredAccess, pDwShareMode, ((pLpSecurityAttributes != NULL) ? 
					pLpSecurityAttributes : &vSecurityAttributes),
					pDwCreationDisposition, pDwFlagsAndAttributes, pHTemplateFile);
		}
		else
		{
			Crx_C_String tString /*= ?*/;

			crx_c_string_construct(&tString);
			
			if(crx_c_os_osString_unsafeConvertToOsString(&tString, pString))
			{
				vReturn = (*vFunc_createFileW)(((wchar_t const *)crx_c_string_getCString(&tString)), 
						pDwDesiredAccess, pDwShareMode, ((pLpSecurityAttributes != NULL) ? 
						pLpSecurityAttributes : &vSecurityAttributes),
						pDwCreationDisposition, pDwFlagsAndAttributes, pHTemplateFile);
			}
			else
				{SetLastError(ERROR_BAD_PATHNAME);}//THIS MIGHT NOT BE NECESSARILY CORRECT.

			crx_c_string_destruct(&tString);
		}
	}
	else
	{
		if(pIsNotUtf8)
		{
			vReturn = CreateFileA((char const *)crx_c_string_getCString(pString), 
					pDwDesiredAccess, pDwShareMode, ((pLpSecurityAttributes != NULL) ? 
					pLpSecurityAttributes : &vSecurityAttributes),
					pDwCreationDisposition, pDwFlagsAndAttributes, pHTemplateFile);
		}
		else
		{
			Crx_C_String tString /*= ?*/;

			crx_c_string_construct(&tString);
			
			if(crx_c_os_osString_unsafeConvertToOsString(&tString, pString))
			{
				vReturn = CreateFileA(((char const *)crx_c_string_getCString(&tString)), 
						pDwDesiredAccess, pDwShareMode, ((pLpSecurityAttributes != NULL) ? 
						pLpSecurityAttributes : &vSecurityAttributes),
						pDwCreationDisposition, pDwFlagsAndAttributes, pHTemplateFile);
			}
			else
				{SetLastError(ERROR_BAD_PATHNAME);}//THIS MIGHT NOT BE NECESSARILY CORRECT.

			crx_c_string_destruct(&tString);
		}
	}

	crx_c_os_fileSystem_internal_win32_undoAddressingDirectoryTrailingSlash(pIsNotUtf8,
			pString, vIsADirectory);

	return vReturn;
}
CRX__LIB__PRIVATE_C_FUNCTION() bool 
		crx_c_os_fileSystem_private_win32_verifyFileSystemPathToOs2AndCreateFile(
		HANDLE * CRX_NOT_NULL pFileHandle,
		Crx_C_String * CRX_NOT_NULL pFullPath, Crx_C_Os_FileSystem_Contract const * pContract, 
		Crx_C_Os_FileSystem_Operation pOperation, uint32_t pDepthLimitOfReparsePoints, 
		Crx_C_Os_FileSystem_Existance * CRX_NOT_NULL pExistance,
		Crx_C_String const * CRX_NOT_NULL pVerified1Utf8PathPrefix, 
		Crx_C_String * CRX_NOT_NULL pVerified1Utf8Route,
		DWORD pDwDesiredAccess, DWORD pDwShareMode,
		SECURITY_ATTRIBUTES * pLpSecurityAttributes, DWORD pDwCreationDisposition, 
		DWORD pDwFlagsAndAttributes, HANDLE pHTemplateFile)
{
	int32_t vErrorCode = 0;
	HANDLE vHandle = INVALID_HANDLE_VALUE;
	bool vReturn = true;
	
	crx_c_string_empty(pFullPath);

	if((pContract->gResolutionModel == CRX__C__OS__FILE_SYSTEM__RESOLUTION_MODEL__NULL) ||
			(pContract->gResolutionModel == CRX__C__OS__FILE_SYSTEM__RESOLUTION_MODEL__PRECISE))
	{
		vIsNoError = crx_c_string_appendCString(pFullPath, pVerified1Utf8PathPrefix);

		if(vIsNoError)
			{vIsNoError = crx_c_string_appendCString(pFullPath, pVerified1Utf8Route);}
		
		if(!vIsNoError)
			{SetLastError(ERROR_NOT_ENOUGH_MEMORY);}

		if(vIsNoError)
		{
			vHandle = crx_c_os_fileSystem_private_win32_createFile(false, pFullPath,
					pDwDesiredAccess, pDwShareMode, pLpSecurityAttributes, 
					pDwCreationDisposition, pDwFlagsAndAttributes, pHTemplateFile);

			if(vHandle != INVALID_HANDLE_VALUE)
			{
				if(!crx_c_os_fileSystem_verifyFileSystemPath2(pContract, pOperation,
						&vHandle, pDepthLimitOfReparsePoints,
						pExistance, pVerified1Utf8PathPrefix, pVerified1Utf8Route, NULL))
				{
					vIsNoError = false;
					SetLastError(ERROR_BAD_PATHNAME);
				}
			}
			else
				{vIsNoError = false;}
		}
	}
	else if((pContract->gResolutionModel == 
					CRX__C__OS__FILE_SYSTEM__RESOLUTION_MODEL__PRECISE_AND_SLOT_AWARE) ||
			(pContract->gResolutionModel == 
					CRX__C__OS__FILE_SYSTEM__RESOLUTION_MODEL__SLOT_AWARE))
	{
		if(crx_c_os_fileSystem_verifyFileSystemPath2(pContract, pOperation,
				NULL, pDepthLimitOfReparsePoints,
				pExistance, pVerified1Utf8PathPrefix, pVerified1Utf8Route, NULL))
		{
			vIsNoError = crx_c_string_appendCString(pFullPath, pVerified1Utf8PathPrefix);

			if(vIsNoError)
				{vIsNoError = crx_c_string_appendCString(pFullPath, pVerified1Utf8Route);}

			if(!vIsNoError)
				{SetLastError(ERROR_NOT_ENOUGH_MEMORY);}

			if(vIsNoError)
			{
				vHandle = crx_c_os_fileSystem_private_win32_createFile(false, pFullPath,
						pDwDesiredAccess, pDwShareMode, pLpSecurityAttributes, 
						pDwCreationDisposition, pDwFlagsAndAttributes, pHTemplateFile);
					
				if(vHandle == INVALID_HANDLE_VALUE)
					{vIsNoError = false;}
			}
		}
		else
		{
			vIsNoError = false;
			SetLastError(ERROR_BAD_PATHNAME)
		}
	}
	else
		{assert(false);}
	
	*pFileHandle = vHandle;

	return vIsNoError;
}
CRX__LIB__PRIVATE_C_FUNCTION() HANDLE crx_c_os_fileSystem_private_win32_rawCreateFile(
		char * CRX_NOT_NULL pCString, DWORD pDwDesiredAccess, DWORD pDwShareMode,
		SECURITY_ATTRIBUTES * pLpSecurityAttributes, DWORD pDwCreationDisposition, 
		DWORD pDwFlagsAndAttributes, HANDLE pHTemplateFile)
{
	static Crx_NonCrxed_Os_Windows_Dll_Kernel32_CreateFileW vFunc_createFileW = 
			crx_c_os_dll_loadSystemFunction("CreateFileW", 3, 51, 0, 0, 2);
	bool vIsADirectory = false;
	int32_t vErrorCode = 0;
	SECURITY_ATTRIBUTES vSecurityAttributes /*= ?*/;
	HANDLE vReturn = INVALID_HANDLE_VALUE;

	CRX__SET_TO_ZERO(SECURITY_ATTRIBUTES, vSecurityAttributes);
	
	if(pLpSecurityAttributes == NULL)
	{
		vSecurityAttributes.lpSecurityDescriptor = NULL;
		vSecurityAttributes.bInheritHandle = false;
	}

	if(vFunc_createFileW != NULL)
	{
		wchar_t tBuffer[1024];

		if(crx_c_os_osString_rawUnsafeConvertToOsString(&(tBuffer[0]), 1024, pCString, 
				strlen(pCString), false))
		{
			vReturn = (*vFunc_createFileW)(&(tBuffer[0]), 
					pDwDesiredAccess, pDwShareMode, ((pLpSecurityAttributes != NULL) ? 
					pLpSecurityAttributes : &vSecurityAttributes),
					pDwCreationDisposition, pDwFlagsAndAttributes, pHTemplateFile);
		}
		else
			{SetLastError(ERROR_BAD_PATHNAME);}//THIS MIGHT NOT BE NECESSARILY CORRECT.
	}
	else
	{
		char tBuffer[1024];

		if(crx_c_os_osString_rawUnsafeConvertToOsString(&(tBuffer[0]), 1024, pCString, 
				strlen(pCString), false))
		{
			vReturn = CreateFileA(&(tBuffer[0]), 
					pDwDesiredAccess, pDwShareMode, ((pLpSecurityAttributes != NULL) ? 
					pLpSecurityAttributes : &vSecurityAttributes),
					pDwCreationDisposition, pDwFlagsAndAttributes, pHTemplateFile);
		}
		else
			{SetLastError(ERROR_BAD_PATHNAME);}//THIS MIGHT NOT BE NECESSARILY CORRECT.
	}

	return vReturn;
}


CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_private_win32_doCreateDirectory(
		bool pIsNotUtf8, Crx_C_String * CRX_NOT_NULL pString, 
		SECURITY_ATTRIBUTES * pLpSecurityAttributes, Crx_C_String * pTemplateDirectoryFullPath)
{
	static Crx_NonCrxed_Os_Windows_Dll_Kernel32_CreateFileW vFunc_createDirectoryW = 
			crx_c_os_dll_loadSystemFunction("CreateDirectoryW", 3, 51, 0, 0, 2);
	static Crx_NonCrxed_Os_Windows_Dll_Kernel32_CreateFileW vFunc_createDirectoryExW = 
			crx_c_os_dll_loadSystemFunction("CreateDirectoryExW", 3, 51, 0, 0, 2);
	bool vIsADirectory = false;
	bool vIsADirectory2 = false;
	int32_t vErrorCode = 0;
	HANDLE vReturn = INVALID_HANDLE_VALUE;

	crx_c_os_fileSystem_internal_win32_addressDirectoryTrailingSlash(pIsNotUtf8, 
			pString, &vIsADirectory);
	crx_c_os_fileSystem_internal_win32_addressDirectoryTrailingSlash(pIsNotUtf8, 
			pTemplateDirectoryFullPath, &vIsADirectory2);

	if(vFunc_createDirectoryW != NULL)
	{
		if(pIsNotUtf8)
		{
			if(pTemplateDirectoryFullPath == NULL)
			{
				vReturn = ((*vFunc_createDirectoryW)(
						(wchar_t const *)crx_c_string_getCString(pString), 
						pLpSecurityAttributes) != 0);
			}
			else
			{
				vReturn = ((*vFunc_createDirectoryExW)(
						(wchar_t const *)crx_c_string_getCString(pTemplateDirectoryFullPath),
						(wchar_t const *)crx_c_string_getCString(pString), 
						pLpSecurityAttributes) != 0);
			}
		}
		else
		{
			Crx_C_String tString /*= ?*/;

			crx_c_string_construct(&tString);
			
			if(crx_c_os_osString_unsafeConvertToOsString(&tString, pString))
			{
				if(pTemplateDirectoryFullPath == NULL)
				{
					vReturn = ((*vFunc_createDirectoryW)(
							(wchar_t const *)crx_c_string_getCString(&tString), 
							pLpSecurityAttributes) != 0);
				}
				else
				{
					Crx_C_String tString2 /*= ?*/;

					crx_c_string_construct(&tString2);

					if(crx_c_os_osString_unsafeConvertToOsString(&tString2, 
							pTemplateDirectoryFullPath))
					{
						vReturn = ((*vFunc_createDirectoryExW)(
								(wchar_t const *)crx_c_string_getCString(&tString2),
								(wchar_t const *)crx_c_string_getCString(&tString), 
								pLpSecurityAttributes) != 0);
					}
					else
						{SetLastError(ERROR_BAD_PATHNAME);}//THIS MIGHT NOT BE NECESSARILY CORRECT.

					crx_c_string_destruct(&tString2);
				}
			}
			else
				{SetLastError(ERROR_BAD_PATHNAME);}//THIS MIGHT NOT BE NECESSARILY CORRECT.

			crx_c_string_destruct(&tString);
		}
	}
	else
	{
		if(pIsNotUtf8)
		{
			if(pTemplateDirectoryFullPath == NULL)
			{
				vReturn = CreateDirectoryA(
						(char const *)crx_c_string_getCString(pString), 
						pLpSecurityAttributes);
			}
			else
			{
				vReturn = CreateDirectoryExA(
						(char const *)crx_c_string_getCString(pTemplateDirectoryFullPath), 
						(char const *)crx_c_string_getCString(pString),
						pLpSecurityAttributes);
			}
		}
		else
		{
			Crx_C_String tString /*= ?*/;

			crx_c_string_construct(&tString);
			
			if(crx_c_os_osString_unsafeConvertToOsString(&tString, pString))
			{
				if(pTemplateDirectoryFullPath == NULL)
				{
					vReturn = (CreateDirectoryA(
							(char const *)crx_c_string_getCString(&tString), 
							pLpSecurityAttributes) != 0);
				}
				else
				{
					Crx_C_String tString2 /*= ?*/;

					crx_c_string_construct(&tString2);

					if(crx_c_os_osString_unsafeConvertToOsString(&tString2, 
							pTemplateDirectoryFullPath))
					{
						vReturn = (CreateDirectoryExA(
								(char const *)crx_c_string_getCString(&tString2),
								(char const *)crx_c_string_getCString(&tString), 
								pLpSecurityAttributes) != 0);
					}
					else
						{SetLastError(ERROR_BAD_PATHNAME);}//THIS MIGHT NOT BE NECESSARILY CORRECT.

					crx_c_string_destruct(&tString2);
				}
			}
			else
				{SetLastError(ERROR_BAD_PATHNAME);}//THIS MIGHT NOT BE NECESSARILY CORRECT.

			crx_c_string_destruct(&tString);
		}
	}

	crx_c_os_fileSystem_internal_win32_undoAddressingDirectoryTrailingSlash(pIsNotUtf8,
			pString, vIsADirectory);
	crx_c_os_fileSystem_internal_win32_undoAddressingDirectoryTrailingSlash(pIsNotUtf8,
			pTemplateDirectoryFullPath, vIsADirectory2);

	return vReturn;
}
CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_private_win32_rawDoCreateDirectory(
		Crx_C_String * CRX_NOT_NULL pCString, 
		SECURITY_ATTRIBUTES * pLpSecurityAttributes, Crx_C_String * pTemplateDirectoryFullPath)
{
	static Crx_NonCrxed_Os_Windows_Dll_Kernel32_CreateFileW vFunc_createDirectoryW = 
			crx_c_os_dll_loadSystemFunction("CreateDirectoryW", 3, 51, 0, 0, 2);
	static Crx_NonCrxed_Os_Windows_Dll_Kernel32_CreateFileW vFunc_createDirectoryExW = 
			crx_c_os_dll_loadSystemFunction("CreateDirectoryExW", 3, 51, 0, 0, 2);
	bool vIsADirectory = false;
	bool vIsADirectory2 = false;
	int32_t vErrorCode = 0;
	HANDLE vReturn = INVALID_HANDLE_VALUE;

	if(vFunc_createDirectoryW != NULL)
	{
		wchar_t tBuffer[1024];

		if(crx_c_os_osString_rawUnsafeConvertToOsString(&(tBuffer[0]), 1024, pCString, 
				strlen(pCString), false))
		{
			if(pTemplateDirectoryFullPath == NULL)
				{vReturn = ((*vFunc_createDirectoryW)(&(tBuffer[0]), pLpSecurityAttributes) != 0);}
			else
			{
				wchar_t tBuffer2[1024];

				if(crx_c_os_osString_rawUnsafeConvertToOsString(&(tBuffer2[0]), 1024, 
						pTemplateDirectoryFullPath, strlen(pTemplateDirectoryFullPath), false))
				{
					vReturn = ((*vFunc_createDirectoryExW)(&(tBuffer2[0]), &(tBuffer[0]), 
							pLpSecurityAttributes) != 0);
				}
				else
					{SetLastError(ERROR_BAD_PATHNAME);}//THIS MIGHT NOT BE NECESSARILY CORRECT.
			}
		}
		else
			{SetLastError(ERROR_BAD_PATHNAME);}//THIS MIGHT NOT BE NECESSARILY CORRECT.
	}
	else
	{
		char tBuffer[1024];

		if(crx_c_os_osString_rawUnsafeConvertToOsString(&(tBuffer[0]), 1024, pCString, 
				strlen(pCString), false))
		{
			if(pTemplateDirectoryFullPath == NULL)
				{vReturn = (CreateDirectoryA(&(tBuffer[0]), pLpSecurityAttributes) != 0);}
			else
			{
				char tBuffer2[1024];

				if(crx_c_os_osString_rawUnsafeConvertToOsString(&(tBuffer2[0]), 1024, 
						pTemplateDirectoryFullPath, strlen(pTemplateDirectoryFullPath), false))
				{
					vReturn = (CreateDirectoryExA(&(tBuffer2[0]), &(tBuffer[0]), 
							pLpSecurityAttributes) != 0);
				}
				else
					{SetLastError(ERROR_BAD_PATHNAME);}//THIS MIGHT NOT BE NECESSARILY CORRECT.
			}
		}
		else
			{SetLastError(ERROR_BAD_PATHNAME);}//THIS MIGHT NOT BE NECESSARILY CORRECT.
	}

	return vReturn;
}


//WARNING: pString CAN NOT HAVE A TRAILING SLASH, BE NULL, OR EMPTY 			(TODO)
//REMEMBER: YOU CAN USE THIS INSTEAD OF GetFileAttributesExA() WHICH IS NOT FOUND IN WIN95
/*
	CURRENTLY, IF pString ENDS WITH A TRAILING SLASH, THE '\' NOT THE '/', THE FUNCTION WILL
	REMOVE IT BEFORE CALLING THE UNDERLING FindFirstFile() FUNCTION. HOWEVER, IF pString
	ENDS WITH MORE THAN ONE TRAILING SLAH, OR THE WRONG TRAILING SLASH, THIS FUNCTION WILL NOT
	CORRECT IT.
	
	pString SHOULD BE STARTING WITH A FULL PATH, HOWEVER, IF THE STRING IS JUST A SLASH, WHICH WOULD
	NOT BE A FULL PATH, IT IS ALSO NOT TREATED AS AN EMPTY PARTIAL PATH, AND THE SLASH IS ALSO
	NOT REMOVED AND THE FUNCTION DOES NOT ASSUME DIRECTORY.
	
*/
CRX__LIB__PRIVATE_C_FUNCTION() HANDLE crx_c_os_fileSystem_internal_win32_findFirstFile(
		bool pIsNotUtf8, Crx_C_String * CRX_NOT_NULL pString, 
		Crx_C_Os_FileSystem_Internal_Win32_Win32FindData * pWin32FindData)
{
	static Crx_NonCrxed_Os_Windows_Dll_Kernel32_FindFirstFileW vFunc_findFirstFileW =
			crx_c_os_dll_loadSystemFunction("FindFirstFileW", 3, 51, 0, 0, 2);
	HANDLE vReturn = INVALID_HANDLE_VALUE;
	bool vIsADirectory = false;

	crx_c_os_fileSystem_internal_win32_addressDirectoryTrailingSlash(pIsNotUtf8, pString,
			&vIsADirectory);

	if(vFunc_findFirstFileW != NULL)
	{
		Crx_NonCrxed_Os_Windows_Dll_Kernel32_Win32FindDataW tWin32FindDataW;
		
		CRX__SET_TO_ZERO(Crx_NonCrxed_Os_Windows_Dll_Kernel32_Win32FindDataW, 
				tWin32FindDataW);

		if(pIsNotUtf8)
		{
			vReturn = (*vFunc_findFirstFileW)((wchar_t const *)crx_c_string_getCString(pString), 
					&tWin32FindDataW);
		}
		else
		{
			Crx_C_String tString /*= ?*/;

			crx_c_string_construct(&tString);
			
			if(crx_c_os_osString_unsafeConvertToOsString(&tString, pString))
			{
				vReturn = (*vFunc_findFirstFileW)(((wchar_t *)crx_c_string_getCString(&tString)), 
						&tWin32FindDataW);
			}
			else
				{SetLastError(ERROR_BAD_PATHNAME);}//THIS MIGHT NOT BE NECESSARILY CORRECT.

			crx_c_string_destruct(&tString);
		}
		
		if(vReturn != INVALID_HANDLE_VALUE)
		{
			if(!crx_c_os_fileSystem_internal_win32_win32FindData_setW(pWin32FindData, 
					&tWin32FindDataW))
			{
				FindClose(vReturn);
				vReturn = INVALID_HANDLE_VALUE;
				SetLastError(ERROR_NOT_ENOUGH_MEMORY);//THIS IS NOT NECESSARILY CORRECT.
			}
		}
	}
	else
	{
		WIN32_FIND_DATAA pWind32FindDataA;

		CRX__SET_TO_ZERO(WIN32_FIND_DATAA, tWind32FindDataA);

		if(pIsNotUtf8)
			{vReturn = FindFirstFileA(crx_c_string_getCString(pString), &tWind32FindDataA);}
		else
		{
			Crx_C_String tString /*= ?*/;

			crx_c_string_construct(&tString);
			
			if(crx_c_os_osString_unsafeConvertToOsString(&tString, pString))
				{vReturn = FindFirstFileA(crx_c_string_getCString(&tString), &tWind32FindDataA);}
			else
				{SetLastError(ERROR_BAD_PATHNAME);}//THIS MIGHT NOT BE NECESSARILY CORRECT.
			
			crx_c_string_destruct(&tString);
		}

		if(vReturn != INVALID_HANDLE_VALUE)
		{
			if(!crx_c_os_fileSystem_internal_win32_win32FindData_setA(pWin32FindData, 
					&tWind32FindDataA))
			{
				FindClose(vReturn);
				vReturn = INVALID_HANDLE_VALUE;
				SetLastError(ERROR_NOT_ENOUGH_MEMORY);//THIS IS NOT NECESSARILY CORRECT.
			}
		}
	}

	if(vIsADirectory)
	{
		if(vReturn != INVALID_HANDLE_VALUE)
		{
			if((pWin32FindData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{
				crx_c_os_fileSystem_internal_win32_findClose(vReturn);
				vReturn = INVALID_HANDLE_VALUE;
				SetLastError(ERROR_DIRECTORY);
			}
		}
	}

	crx_c_os_fileSystem_internal_win32_undoAddressingDirectoryTrailingSlash(pIsNotUtf8,
			pString, vIsADirectory);
	
	return vReturn;
}

/*
	NOTE, ERRORS BY FindNextFileA/FindNextFileW SHOULD BE CONSIDERED FATAL. WHILE THE ERROR
	ERROR_NO_MORE_FILES IS ALSO FATAL, IT IS SIMPLY THE END OF ITERATION, SO NOT AN ISSUE. BUT
	IN A RECURSIVE ITERATION SITUATION, MEANING ALL DIRECTORIES AND SUB DIRECTORIES,
	YOU MIGHT WANT TO STOP ALL ITERATIONS IF ONE ITERATOR ENCOUNTERS AN ERROR OTHER THAN
	ERROR_NO_MORE_FILES.
	
*/
CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_internal_win32_findNextFile(
		HANDLE pHandle, Crx_C_Os_FileSystem_Internal_Win32_Win32FindData * pWin32FindData)
{
	static Crx_NonCrxed_Os_Windows_Dll_Kernel32_FindNextFileW vFunc_findNextFileW = 
			crx_c_os_dll_loadSystemFunction("FindNextFileW", 3, 51, 0, 0, 2);
	bool vIsNoError = true;

	if(vFunc_findNextFileW != NULL)
	{
		Crx_NonCrxed_Os_Windows_Dll_Kernel32_Win32FindDataW tWin32FindDataW;
		
		CRX__SET_TO_ZERO(Crx_NonCrxed_Os_Windows_Dll_Kernel32_Win32FindDataW, 
				tWin32FindDataW);

		if((*vFunc_findNextFileW)(pHandle, &tWin32FindDataW) != 0)
		{
			if(!crx_c_os_fileSystem_internal_win32_win32FindData_setW(pWin32FindData, 
					&tWin32FindDataW))
			{
				vIsNoError = false;
				SetLastError(ERROR_NOT_ENOUGH_MEMORY);//THIS IS NOT NECESSARILY CORRECT.
			}
		}
		else
			{vIsNoError = false;}
	}
	else
	{
		WIN32_FIND_DATAA pWind32FindDataA;

		CRX__SET_TO_ZERO(WIN32_FIND_DATAA, tWind32FindDataA);

		if(FindNextFileA(pHandle, &tWind32FindDataA) != 0)
		{
			if(!crx_c_os_fileSystem_internal_win32_win32FindData_setA(pWin32FindData, 
					&tWind32FindDataA))
			{
				vIsNoError = false;
				SetLastError(ERROR_NOT_ENOUGH_MEMORY);//THIS IS NOT NECESSARILY CORRECT.
			}
		}
		else
			{vIsNoError = false;}
	}

	return vIsNoError;
}

CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_internal_win32_findClose(HANDLE pHandle)
{//THERE IS NO FindCloseW()
	if(pHandle != INVALID_HANDLE_VALUE) //NOT SAFE TO CALL OTHERWISE. CAUSES SYSTEM TO HANG
		{return FindClose(pHandle);}
	else
		{return true;}
}

/*
	WARNING: REMEMBER, THIS FUNCTION CAN STILL MOVE FILES ACROSS VOLUMES. IT STILL CAN NOT MOVE
			DIRECTORIES ACROSS VOLUMES.
			
	UNSURE IF NECESSARY, BUT THE FUNCTION TRIES TO MAKE SURE THAT CREATION AND MODIFICATION DATES
			ARE PRESERVED. I SUSPECT A LOSS MIGHT HAPPEN IF MOVING FILES ACROSS VOLUMES.

	pIsToForbidAcrossVolumes CAN BE USED TO DISABLE MOVING OF FILES ACROSS VOLUMES. IF YOU KNOW
			THAT THE TWO PATH ARE ON THE SAME VOLUME, DO NOT SET THIS TO true TO AVOID THE
			PERFORMANCE COST.
*/
CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_internal_win32_doMoveFile(
		bool pIsNotUtf8, Crx_C_String * CRX_NOT_NULL pSourceFullPath,
		Crx_C_String * CRX_NOT_NULL pTargetFullPath, bool pIsToForbidAcrossVolumes)
{
	static Crx_NonCrxed_Os_Windows_Dll_Kernel32_MoveFileW vFunc_moveFileW = NULL;
	bool vIsSourceADirectory = false;
	bool vIsTargetADirectory = false;
	Crx_C_Os_FileSystem_Internal_Win32_Win32FindData vWin32FindData /*= ?*/;
	HANDLE vHandle = INVALID_HANDLE_VALUE;
	bool vReturn = true;
	
	crx_c_os_fileSystem_internal_win32_win32FindData_construct(&vWin32FindData);

	if(pIsToForbidAcrossVolumes)
	{
		size_t tVolumeSerialNumber = 0;
		size_t tVolumeSerialNumber2 = 0;

		crx_c_os_fileSystem_private_win32_doGetVolumeInformation(
				pSourceFullPath, crx_c_os_fileSystem_private_win32_getByteLengthOfRoute(
				pSourceFullPath), NULL, &tVolumeSerialNumber, NULL, NULL, NULL);
		crx_c_os_fileSystem_private_win32_doGetVolumeInformation(
				pTargetFullPath, crx_c_os_fileSystem_private_win32_getByteLengthOfRoute(
				pTargetFullPath), NULL, &tVolumeSerialNumber2, NULL, NULL, NULL);

		if(tVolumeSerialNumber2 != tVolumeSerialNumber)
		{
			SetLastError(ERROR_NOT_SAME_DEVICE);
			vReturn = false;
		}
	}

	if(vReturn)
	{
		if(vFunc_moveFileW == NULL)
			{vFunc_moveFileW = crx_c_os_dll_loadSystemFunction("MoveFileW", 3, 51, 0, 0, 2);}
		
		crx_c_os_fileSystem_internal_win32_addressDirectoryTrailingSlash(pIsNotUtf8, 
				pSourceFullPath, &vIsSourceADirectory);
		crx_c_os_fileSystem_internal_win32_addressDirectoryTrailingSlash(pIsNotUtf8, 
				pTargetFullPath, &vIsTargetADirectory);

		if(vIsSourceADirectory == vIsTargetADirectory)
		{
			vHandle = crx_c_os_fileSystem_internal_win32_findFirstFile(pIsNotUtf8, 
					pSourceFullPath, &vWin32FindData);

			if(vHandle == INVALID_HANDLE_VALUE)
				{vReturn = false;}
		}
		else
		{
			SetLastError(ERROR_BAD_PATHNAME);
			vReturn = false;
		}

		if(vReturn)
		{
			if(vFunc_moveFileW != NULL)
			{
				if(pIsNotUtf8)
				{
					vReturn = ((*vFunc_moveFileW)((wchar_t const *)crx_c_string_getCString(
							pSourceFullPath), 
							(wchar_t const *)crx_c_string_getCString(pTargetFullPath)) != 0);
				}
				else
				{
					Crx_C_String tString /*= ?*/;
					Crx_C_String tString2 /*= ?*/;

					crx_c_string_construct(&tString);
					crx_c_string_construct(&tString2);
					
					if(crx_c_os_osString_unsafeConvertToOsString(&tString, pSourceFullPath) &&
							crx_c_os_osString_unsafeConvertToOsString(&tString2, pTargetFullPath))
					{
						vReturn = ((*vFunc_moveFileW)(((wchar_t *)crx_c_string_getCString(&tString)), 
								((wchar_t *)crx_c_string_getCString(&tString2))) != 0);
					}
					else
						{SetLastError(ERROR_BAD_PATHNAME);}//THIS MIGHT NOT BE NECESSARILY CORRECT.

					crx_c_string_destruct(&tString);
					crx_c_string_destruct(&tString2);
				}
			}
			else
			{
				if(pIsNotUtf8)
				{
					vReturn = (MoveFileA(crx_c_string_getCString(pSourceFullPath), 
							crx_c_string_getCString(pTargetFullPath)) != 0);
				}
				else
				{
					Crx_C_String tString /*= ?*/;
					Crx_C_String tString2 /*= ?*/;

					crx_c_string_construct(&tString);
					crx_c_string_construct(&tString2);
					
					if(crx_c_os_osString_unsafeConvertToOsString(&tString, pSourceFullPath) &&
							crx_c_os_osString_unsafeConvertToOsString(&tString2, pTargetFullPath))
					{
						vReturn = (MoveFileA(crx_c_string_getCString(&tString), 
								crx_c_string_getCString(&tString2)) != 0);
					}
					else
						{SetLastError(ERROR_BAD_PATHNAME);}//THIS MIGHT NOT BE NECESSARILY CORRECT.
					
					crx_c_string_destruct(&tString);
					crx_c_string_destruct(&tString2);
				}
			}
		}

		crx_c_os_fileSystem_internal_win32_undoAddressingDirectoryTrailingSlash(pIsNotUtf8,
				pSourceFullPath, vIsSourceADirectory);
		crx_c_os_fileSystem_internal_win32_undoAddressingDirectoryTrailingSlash(pIsNotUtf8,
				pTargetFullPath, vIsTargetADirectory);

		if(vReturn)
		{//TODO: WE CAN NOT CURRENTLY RECOVER FOMR AN ERROR HERE. FILE ALREADY MOVED.
			Crx_C_Os_FileSystem_Internal_Win32_Win32FindData tWin32FindData /*= ?*/;
			HANDLE tHandle = INVALID_HANDLE_VALUE;
			ULARGE_INTEGER tuLargeInteger;
			uint64_t vUint64;
			uint64_t vUint64__2;
		
			crx_c_os_fileSystem_internal_win32_win32FindData_construct(tvWin32FindData);
			
			tHandle = crx_c_os_fileSystem_internal_win32_findFirstFile(pIsNotUtf8, pTargetFullPath, 
					&tWin32FindData);

			if(vHandle != INVALID_HANDLE_VALUE)
			{
				bool tIsToSet = false;

				tuLargeInteger.LowPart = vWin32FindData.ftCreationTime.dwLowDateTime;
				tuLargeInteger.HighPart = vWin32FindData.ftCreationTime.dwHighDateTime;
				tUint64 = tuLargeInteger.QuadPart;
				tuLargeInteger.LowPart = tvWin32FindData.ftCreationTime.dwLowDateTime;
				tuLargeInteger.HighPart = tvWin32FindData.ftCreationTime.dwHighDateTime;
				tUint64__2 = tuLargeInteger.QuadPart;
				
				if((tUint64__2 -  tUint64) < UINT64_C(20000000))
				{
					tuLargeInteger.LowPart = vWin32FindData.ftLastWriteTime.dwLowDateTime;
					tuLargeInteger.HighPart = vWin32FindData.ftLastWriteTime.dwHighDateTime;
					tUint64 = tuLargeInteger.QuadPart;
					tuLargeInteger.LowPart = tvWin32FindData.ftLastWriteTime.dwLowDateTime;
					tuLargeInteger.HighPart = tvWin32FindData.ftLastWriteTime.dwHighDateTime;
					tUint64__2 = tuLargeInteger.QuadPart;
					
					if((tUint64__2 -  tUint64) > UINT64_C(20000000))
						{tIsToSet = true;}
				}
				else
					{tIsToSet = true;}
				
				if(tIsToSet)
				{				
					HANDLE tHandle2 = crx_c_os_fileSystem_private_win32_createFile(false, 
							pTargetFullPath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 
							NULL, OPEN_EXISTING, 0, NULL);
							
					if(vHandle != INVALID_HANDLE_VALUE)
					{
						SetFileTime(tHandle2, vWin32FindData.ftCreationTime,
								vWin32FindData.ftLastAccessTime, vWin32FindData.ftLastWriteTime);
					}
				}
			}
			
			crx_c_os_fileSystem_internal_win32_win32FindData_destruct(tvWin32FindData);
		}
			
		//TODO ENSURE ATTRIBUTES ARE THE SAME.
	}

	crx_c_os_fileSystem_internal_win32_win32FindData_destruct(&vWin32FindData);

	return vReturn;
}

/*
	NOTE: ORIGINALLY, UNLIKE GetFileAttributes() ON MOUNTS POINTS, THIS FUNCTION WOULD RETURN THE 
			INFORMATION OF THE ROOT OF THE MOUNT, AND NOT THE MOUNT POINT ITSELF. DECIDED AGAINST
			THAT LATER ON.
*/
CRX__LIB__PRIVATE_C_FUNCTION() DWORD crx_c_os_fileSystem_private_win32_getFileAttributes(
		bool pIsNotUtf8, Crx_C_String const * pString)
{
	static Crx_NonCrxed_Os_Windows_Dll_Kernel32_GetFileAttributesW vFunc_getFileAttributesW = 
			crx_c_os_dll_loadSystemFunction("GetFileAttributesW", 3, 51, 0, 0, 2);
	//IT IS ASSUMED THAT IF GetVolumeNameForVolumeMountPointA() EXISTS, 
	//		GetVolumeNameForVolumeMountPointW() EXISTS. THESE FUNCTIONS ARE NOT IN WIN9X.
	static Crx_NonCrxed_Os_Windows_Dll_Kernel32_GetVolumeNameForVolumeMountPointW 
			vFunc_getVolumeNameForVolumeMountPointW = crx_c_os_dll_loadSystemFunction(
			"GetVolumeNameForVolumeMountPointW", 5, 0, 0, 0, 2);
	static Crx_NonCrxed_Os_Windows_Dll_Kernel32_FindFirstFileW vFunc_findFirstFileW = 
			crx_c_os_dll_loadSystemFunction("FindFirstFileW", 3, 51, 0, 0, 2);
	bool vIsADirectory = false;
	DWORD vReturn = INVALID_FILE_ATTRIBUTES;

	crx_c_os_fileSystem_internal_win32_addressDirectoryTrailingSlash(pIsNotUtf8, pString, 
			&vIsADirectory);

	if(vFunc_getFileAttributesW != NULL)
	{
		Crx_C_String tString /*= ?*/;

		crx_c_string_construct(&tString);

		if(pIsNotUtf8)
		{
			wchar_t const * tCString = (wchar_t const *)crx_c_string_getCString(pString);

			if(tCString != NULL)
				{vReturn = (*vFunc_getFileAttributesW)(tCString);}
			else
				{SetLastError(ERROR_BAD_PATHNAME);}
		}
		else
		{
			if(crx_c_os_osString_unsafeConvertToOsString(&tString, pString))
			{
				vReturn = (*vFunc_getFileAttributesW)((wchar_t *)crx_c_string_getCString(
						&tString));
			}
			else
				{SetLastError(ERROR_BAD_PATHNAME);}//THIS MIGHT NOT BE NECESSARILY CORRECT.
		}
		
		/*
			"To determine if a specified directory is a mounted folder, first call the 
			GetFileAttributes function and inspect the FILE_ATTRIBUTE_REPARSE_POINT flag in the 
			return value to see if the directory has an associated reparse point. If it does, use 
			the FindFirstFile and FindNextFile functions to obtain the reparse tag in the 
			dwReserved0 member of the WIN32_FIND_DATA structure. To determine if the reparse point 
			is a mounted folder (and not some other form of reparse point), test whether the tag 
			value equals the value IO_REPARSE_TAG_MOUNT_POINT. For more information, see Reparse 
			Points.
			To obtain the target volume of a mounted folder, use the 
			GetVolumeNameForVolumeMountPoint function.
			In a similar manner, you can determine if a reparse point is a symbolic link by testing 
			whether the tag value is IO_REPARSE_TAG_SYMLINK."
		*/
		//vFunc_getVolumeNameForVolumeMountPointW IS NOT NULL, IT SEEMS THAT WE CAN CONCLUDE THAT 
		//		MOUNT POINTS ARE NOT SUPPORTED IF I AM NOT WRONG.
		//UPDATE: Decided against this. Instead, the function shall not follow mount point. If you
		//		later decide other wise, remove "false" from the following.
		if(false && (vFunc_getVolumeNameForVolumeMountPointW != NULL))
		{
			if(vReturn != INVALID_FILE_ATTRIBUTES)
			{
				if((vReturn & 
						CRX__C__OS__FILESYSTEM__WIN32__PRIVATE__FILE_ATTRIBUTE_REPARSE_POINT) != 0)
				{
					Crx_NonCrxed_Os_Windows_Dll_Kernel32_Win32FindDataW tWin32FindDataW;
					HANDLE tHandle = INVALID_HANDLE_VALUE;
	
					CRX__SET_TO_ZERO(Crx_NonCrxed_Os_Windows_Dll_Kernel32_Win32FindDataW, 
							tWin32FindDataW);

					tHandle = (*vFunc_findFirstFileW)((wchar_t *)crx_c_string_getCString(
							(pIsNotUtf8 ? pString : &tString)), &tWin32FindDataW);

					if(tHandle != INVALID_HANDLE_VALUE)
					{							
						if(tWin32FindDataW.dwReserved0 == IO_REPARSE_TAG_MOUNT_POINT)
						{
							wchar_t tWchars[100];

							if((*vFunc_getVolumeNameForVolumeMountPointW)(
									(wchar_t *)crx_c_string_getCString((pIsNotUtf8 ? pString : 
									&tString), &(tWchars[0]), 100)) != 0)
								{vReturn = (*vFunc_getFileAttributesW)(&(tWchars[0]));}
							else
								{vReturn = INVALID_FILE_ATTRIBUTES;}
						}

						FindClose(tHandle);
						tHandle = INVALID_HANDLE_VALUE;
					}
					else
						{vReturn = INVALID_FILE_ATTRIBUTES;}
				}
			}
		}

		crx_c_string_destruct(&tString);
	}
	else
	{
		if(pIsNotUtf8)
		{
			char const * tChars = crx_c_string_getCString(pString);

			if(tChars != NULL)
				{vReturn = GetFileAttributesA(tChars);}
			else
				{SetLastError(ERROR_BAD_PATHNAME);}
		}
		else
		{
			Crx_C_String tString /*= ?*/;

			crx_c_string_construct(&tString);
			
			if(crx_c_os_osString_unsafeConvertToOsString(&tString, pString))
				{vReturn = GetFileAttributesA(crx_c_string_getCString(&tString));}
			else
				{SetLastError(ERROR_BAD_PATHNAME);}//THIS MIGHT NOT BE NECESSARILY CORRECT.

			crx_c_string_destruct(&tString);
		}
	}

	crx_c_os_fileSystem_internal_win32_undoAddressingDirectoryTrailingSlash(pIsNotUtf8,
			pString, vIsADirectory);

	return vReturn;
}
CRX__LIB__PRIVATE_C_FUNCTION() DWORD crx_c_os_fileSystem_private_win32_rawGetFileAttributes(
		char const * pCString)
{
	static Crx_NonCrxed_Os_Windows_Dll_Kernel32_GetFileAttributesW vFunc_getFileAttributesW = 
			crx_c_os_dll_loadSystemFunction("GetFileAttributesW", 3, 51, 0, 0, 2);
	DWORD vReturn = INVALID_FILE_ATTRIBUTES;

	
	if(vFunc_getFileAttributesW != NULL)
	{
		wchar_t tBuffer[1024];

		if(crx_c_os_osString_rawUnsafeConvertToOsString(&(tBuffer[0]), 1024, pCString, 
				strlen(pCString), false))
			{vReturn = (*vFunc_getFileAttributesW)(&(tBuffer[0]));}
		else
			{SetLastError(ERROR_BAD_PATHNAME);}		
	}
	else
	{
		char vBuffer[1024];

		if(crx_c_os_osString_rawUnsafeConvertToOsString(&(tBuffer[0]), 1024, pCString, 
				strlen(pCString), false))
			{vReturn = GetFileAttributesA(&(tBuffer[0]));}
		else
			{SetLastError(ERROR_BAD_PATHNAME);}
	}

	return vReturn;
}


CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_private_win32_getFileInformation(
		bool pIsNotUtf8, Crx_C_String const * pString, bool pIsADirectory,
		BY_HANDLE_FILE_INFORMATION * pByHandleFileInformation)
{
	static Crx_NonCrxed_Os_Windows_Dll_Kernel32_CreateFileW vFunc_createFileW = 
			crx_c_os_dll_loadSystemFunction("CreateFileW", 3, 51, 0, 0, 2);
	lpSecurityAttributes vLpSecurityAttributes /*= ?*/;
	bool vIsADirectory = false;
	int32_t vHandle = INVALID_HANDLE_VALUE;
	DWORD vResult = 0;

	CRX__SET_TO_ZERO(lpSecurityAttributes, vLpSecurityAttributes);
	
	vLpSecurityAttributes.nLength = sizeof(lpSecurityAttributes);
	vLpSecurityAttributes.lpSecurityDescriptor = NULL;
	vLpSecurityAttributes.bInheritHandle = false;

	crx_c_os_fileSystem_internal_win32_addressDirectoryTrailingSlash(pIsNotUtf8, pString, 
			&vIsADirectory);
	
	if(vFunc_createFileW != NULL)
	{
		if(pIsNotUtf8)
		{
			vHandle = (*vFunc_createFileW)((wchar_t const *)crx_c_string_getCString(pString), 0, 
					FILE_SHARE_READ | FILE_SHARE_WRITE, &vLpSecurityAttributes, OPEN_EXISTING, 
					(vIsADirectory ? FILE_FLAG_BACKUP_SEMANTICS : FILE_ATTRIBUTE_NORMAL), NULL);
		}
		else
		{
			Crx_C_String tString /*= ?*/;

			crx_c_string_construct(&tString);
			
			if(crx_c_os_osString_unsafeConvertToOsString(&tString, pString))
			{
				vHandle = (*vFunc_createFileW)((wchar_t const *)crx_c_string_getCString(&tString), 
						0, FILE_SHARE_READ | FILE_SHARE_WRITE, &vLpSecurityAttributes, 
						OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			}
			else
				{SetLastError(ERROR_BAD_PATHNAME);}//THIS MIGHT NOT BE NECESSARILY CORRECT.

			crx_c_string_destruct(&tString);
		}
	}
	else
	{
		if(pIsNotUtf8)
		{
			vHandle = CreateFileA(crx_c_string_getCString(pString), 
					0, FILE_SHARE_READ | FILE_SHARE_WRITE, &vLpSecurityAttributes, 
					OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		}
		else
		{
			Crx_C_String tString /*= ?*/;

			crx_c_string_construct(&tString);
			
			if(crx_c_os_osString_unsafeConvertToOsString(&tString, pString))
			{
				vHandle = CreateFileA(crx_c_string_getCString(&tString), 
						0, FILE_SHARE_READ | FILE_SHARE_WRITE, &vLpSecurityAttributes, 
						OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			}
			else
				{SetLastError(ERROR_BAD_PATHNAME);}//THIS MIGHT NOT BE NECESSARILY CORRECT.

			crx_c_string_destruct(&tString);
		}
	}

	if(vHandle != INVALID_HANDLE_VALUE)
	{
		vResult = crx_c_os_fileSystem_internal_win32_getFileInformationByHandle(pHandle,
				pByHandleFileInformation);

		CloseHandle(vHandle);
		vHandle = INVALID_HANDLE_VALUE;
	}
	
	crx_c_os_fileSystem_internal_win32_undoAddressingDirectoryTrailingSlash(pIsNotUtf8,
			pString, vIsADirectory);
	
	return (vResult != 0);
}

CRX__LIB__PRIVATE_C_FUNCTION() uint32_t crx_c_os_fileSystem_internal_win32_getFileInformationByHandle(
		int32_t pHandle, BY_HANDLE_FILE_INFORMATION * pByHandleFileInformation)
	{return GetFileInformationByHandle(pHandle, pByHandleFileInformation);}

/*
	WARNING: CURRENTLY, crx_c_os_fileSystem_private_win32_doGetVolumeInformation() ASSUMES THAT
			THIS FUNCTION DOES NOT ADDRESS TRAILING SLASH FOR DIRECTORIES TO AVOID CONTRADICTIONS
			RELATED TO THE LENGTH LIMITATIONS. IF YOU CHANGE THIS IN THE FUTURE, YOU NEED TO FIX
			crx_c_os_fileSystem_private_win32_doGetVolumeInformation()
*/
CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_private_win32_getVolumeInformation(
		bool pIsNotUtf8, Crx_C_String const * CRX_NOT_NULL pDirectoryFullPathOfRoot, 
		Crx_C_String * pVolumeName, uint32_t * pVolumeSerialNumber, 
		uint32_t * pMaximumPathSegmentLength, uint32_t * pFileSystemFlags, 
		Crx_C_String * pFileSystemName)
{
	static Crx_NonCrxed_Os_Windows_Dll_Kernel32_GetVolumeInformationW vFunc_getVolumeInformationW = 
			crx_c_os_dll_loadSystemFunction("GetVolumeInformationW", 3, 51, 0, 0, 2);
	bool vReturn = true;

	if(vFunc_getVolumeInformationW != NULL)
	{
		wchar_t tVolumeNameBuffer[MAX_PATH + 1 + 1] /*= ?*/;
		wchar_t tFileSystemNameBuffer[MAX_PATH + 1 + 1] /*= ?*/;

		tVolumeNameBuffer[MAX_PATH + 2] = 0 /*\0*/;
		tFileSystemNameBuffer[MAX_PATH + 2] = 0 /*\0*/;

		if(pIsNotUtf8)
		{
			if((*vFunc_getVolumeInformationW)((wchar_t const *)crx_c_string_getCString(
					pDirectoryFullPathOfRoot), 0, &(tVolumeNameBuffer[0]),
					MAX_PATH + 1, pVolumeSerialNumber, pMaximumPathSegmentLength,
					pFileSystemFlags, &(tFileSystemNameBuffer[0]), MAX_PATH + 1) == 0)
				{vReturn = false;}
		}
		else
		{
			Crx_C_String tString /*= ?*/;

			crx_c_string_construct(&tString);
			
			if(crx_c_os_osString_unsafeConvertToOsString(&tString, pString))
			{
				if((*vFunc_getVolumeInformationW)((wchar_t const *)crx_c_string_getCString(
						&tString), 0, &(tVolumeNameBuffer[0]), MAX_PATH + 1, pVolumeSerialNumber, 
						pMaximumPathSegmentLength, pFileSystemFlags, &(tFileSystemNameBuffer[0]), 
						MAX_PATH + 1) == 0)
					{vReturn = false;}
			}
			else
				{SetLastError(ERROR_BAD_PATHNAME);}//THIS MIGHT NOT BE NECESSARILY CORRECT.

			crx_c_string_destruct(&tString);
		}
		
		if(vReturn)
		{
			if(pVolumeName != NULL)
			{
				vReturn = crx_c_os_osString_unsafeConvertFromOsString2(pVolumeName, 
						(unsigned char(&(tVolumeNameBuffer[0]))), 
						crx_c_fileSystem_wstrlen(&(tVolumeNameBuffer[0]))));
			}
			if(vReturn && (pFileSystemName != NULL))
			{
				vReturn = crx_c_os_osString_unsafeConvertFromOsString2(pFileSystemName, 
						(unsigned char(&(tFileSystemNameBuffer[0]))), 
						crx_c_fileSystem_wstrlen(&(tFileSystemNameBuffer[0])));
			}

			if(!vReturn)
				{SetLastError(ERROR_NOT_ENOUGH_MEMORY);}
		}
	}
	else
	{
		char tVolumeNameBuffer[MAX_PATH + 1 + 1] /*= ?*/;
		char tFileSystemNameBuffer[MAX_PATH + 1 + 1] /*= ?*/;

		tVolumeNameBuffer[MAX_PATH + 2] = 0 /*\0*/;
		tFileSystemNameBuffer[MAX_PATH + 2] = 0 /*\0*/;

		if(pIsNotUtf8)
		{
			if(GetVolumeInformationA(crx_c_string_getCString(pDirectoryFullPathOfRoot), 0, 
					&(tVolumeNameBuffer[0]), MAX_PATH + 1, pVolumeSerialNumber, 
					pMaximumPathSegmentLength, pFileSystemFlags, &(tFileSystemNameBuffer[0]), 
					MAX_PATH + 1) == 0)
				{vReturn = false;}
		}
		else
		{
			Crx_C_String tString /*= ?*/;

			crx_c_string_construct(&tString);
			
			if(crx_c_os_osString_unsafeConvertToOsString(&tString, pString))
			{
				if(GetVolumeInformationA(crx_c_string_getCString(&tString), 0, 
						&(tVolumeNameBuffer[0]), MAX_PATH + 1, pVolumeSerialNumber, 
						pMaximumPathSegmentLength, pFileSystemFlags, &(tFileSystemNameBuffer[0]), 
						MAX_PATH + 1) == 0)
					{vReturn = false;}
			}
			else
				{SetLastError(ERROR_BAD_PATHNAME);}//THIS MIGHT NOT BE NECESSARILY CORRECT.

			crx_c_string_destruct(&tString);
		}
		
		if(vReturn)
		{
			if(pVolumeName != NULL)
			{
				vReturn = crx_c_os_osString_unsafeConvertFromOsString2(pVolumeName, 
						(unsigned char(&(tVolumeNameBuffer[0]))), 
						crx_c_fileSystem_strlen(&(tVolumeNameBuffer[0]))));
			}
			if(vReturn && (pFileSystemName != NULL))
			{
				vReturn = crx_c_os_osString_unsafeConvertFromOsString2(pFileSystemName, 
						(unsigned char(&(tFileSystemNameBuffer[0]))), 
						crx_c_fileSystem_strlen(&(tFileSystemNameBuffer[0])));
			}

			if(!vReturn)
				{SetLastError(ERROR_NOT_ENOUGH_MEMORY);}
		}
	}

	if(!vReturn)
	{
		if(pVolumeName != NULL)
			{crx_c_string_empty(pVolumeName);}
		if(pVolumeSerialNumber != NULL)
			{*pVolumeSerialNumber = 0;}
		if(pMaximumPathSegmentLength != NULL)
			{*pMaximumPathSegmentLength = 0;}
		if(pFileSystemFlags != NULL)
			{*pFileSystemFlags = 0;}
		if(pFileSystemName != NULL)
			{crx_c_string_empty(pFileSystemName);}
	}
	
	return (vResult != 0);
}

/*
	DOES NOT TAKE pIsNotUtf8 TO AVOID ISSUES WITH MULTIBYTE ENCODINGS AND THE REVERSE SEARCH FOR
	'\' IN THAT ENCODING. EVEN DOING A FARWARD SEARCH COULD POTENTIONALY BE A PROBLEM BECAUSE
	::crx::c::string IS NOT AWARE OF ANY ENCODING, NOR OF THE SIZE OF THE CHARACTER TYPE USED
	IN THE ENCODING. (I MIGHT STILL ADDRESS THIS IN THE FUTURE.)
	
	THIS FUNCTION WILL WORK ON ANY PATH, AND NOT JUST THE ROOT PATH.
*/
CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_private_win32_doGetVolumeInformation(
		Crx_C_String const * CRX_NOT_NULL pFullPath, size_t pByteLengthOfRoute, 
		Crx_C_String * pVolumeName, uint32_t * pVolumeSerialNumber,
		uint32_t * pMaximumPathSegmentLength, uint32_t * pFileSystemFlags, 
		Crx_C_String * pFileSystemName)
{
	/*
		REMEMBER, WE ARE NOT USING 
						crx_c_os_fileSystem_internal_win32_addressDirectoryTrailingSlash()
						crx_c_os_fileSystem_internal_win32_undoAddressingDirectoryTrailingSlash()
				IN THIS FUNCTION. HENCE, WE ARE REMOVING THE TRAILING SLASH OURSELVES TO KEEP THE 
				INTEGRITY OF LENGTH LIMITATIONS.
	*/
	Crx_C_String vFullPath /*= ?*/;
	bool vIsNoError = true;
	size_t vCurrentEndIndex = 0;
	
	crx_c_string_copyConstruct(&vFullPath, pFullPath);
	
	vIsNoError = (crx_c_string_getSize(&vFullPath) == crx_c_string_getSize(pFullPath));

	if(vIsNoError)
	{
		vCurrentEndIndex = crx_c_string_getSize(&vFullPath) - 1;

		if(!crx_c_string_isEndingWith2(&vFullPath, CRX__OS__FILE_SYSTEM__SEPERATOR_STRING, false))
		{
			//REMEMBER: THE FOLLOWING WOULD BE UNSAFE IF THIS WAS NOT THE SEPERATOR CHARACTER AND
			//		THE PATH WAS NOT IN UTF8.
			vCurrentEndIndex = crx_c_string_getIndexOfFirstReverseOccuranceOf2(&vFullPath, 
					vCurrentEndIndex, CRX__OS__FILE_SYSTEM__SEPERATOR_STRING, 0);

			if((vCurrentEndIndex < pByteLengthOfRoute)/*JUST IN CASE*/ || 
					(vCurrentEndIndex == ((size_t)(-1))))
			{
				vIsNoError = false;
				
				SetLastError(ERROR_BAD_PATHNAME);
			}
			else
			{
				crx_c_string_removeChars(&vFullPath, vCurrentEndIndex, 
						crx_c_string_getSize(&vFullPath) - vCurrentEndIndex);
			}
		}
		else
		{
			crx_c_string_removeChars(&vFullPath, crx_c_string_getSize(&vFullPath) - 1, 1);
		}
	}
	
	while(vIsNoError && (vCurrentEndIndex > 0))
	{
		if(crx_c_os_fileSystem_private_win32_getVolumeInformation(false, &vFullPath, pVolumeName,
				pVolumeSerialNumber, pMaximumPathSegmentLength, pFileSystemFlags, pFileSystemName))
			{break;}
		
		//THERE ARE POTENTIALLY OTHER ERRORS THAT WOULD BE INVALID IF ENCOUNTERED WHICH NEEDS TO BE 
		//		CHECKED
		if(GetLastError() == ERROR_NOT_ENOUGH_MEMORY)
			{vIsNoError = false;}
		else
		{
			vCurrentEndIndex = crx_c_string_getIndexOfFirstReverseOccuranceOf2(&vFullPath, 
					vCurrentEndIndex - 1, CRX__OS__FILE_SYSTEM__SEPERATOR_STRING, 0);

			if((vCurrentEndIndex < pByteLengthOfRoute) || (vCurrentEndIndex == ((size_t)(-1))))
			{
				vIsNoError = false;
				
				SetLastError(ERROR_BAD_PATHNAME);
			}
			else
			{
				crx_c_string_removeChars(&vFullPath, vCurrentEndIndex, 
						crx_c_string_getSize(&vFullPath) - vCurrentEndIndex);
			}
		}
	}
}

CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_private_win32_deleteFile(
		bool pIsNotUtf8, Crx_C_String * pString)
{
	static Crx_NonCrxed_Os_Windows_Dll_Kernel32_DeleteFileW vFunc_deleteFileW = 
			crx_c_os_dll_loadSystemFunction("DeleteFileW", 3, 51, 0, 0, 2);
	bool vReturn = true;
	int32_t vHandle = INVALID_HANDLE_VALUE;
	DWORD vResult = 0;

	if(vFunc_deleteFileW != NULL)
	{
		if(pIsNotUtf8)
		{
			if(tCString != NULL)
			{
				vReturn = ((*vFunc_deleteFileW)((wchar_t const *)crx_c_string_getCString(
						pString)) != 0);
			}
		}
		else
		{
			Crx_C_String tString /*= ?*/;

			crx_c_string_construct(&tString);
			
			if(crx_c_os_osString_unsafeConvertToOsString(&tString, pString))
			{
				vReturn = ((*vFunc_deleteFileW)((wchar_t const *)crx_c_string_getCString(
						&tString)) != 0);
			}
			else
				{SetLastError(ERROR_BAD_PATHNAME);}//THIS MIGHT NOT BE NECESSARILY CORRECT.

			crx_c_string_destruct(&tString);
		}
	}
	else
	{
		HANDLE tHandle = INVALID_HANDLE_VALUE;

		if(pIsNotUtf8)
		{
			tHandle = crx_c_os_fileSystem_private_win32_createFile(true, pString, 
					FILE_SHARE_READ | FILE_SHARE_WRITE, 0, NULL, 
					OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

			if(tHandle != INVALID_HANDLE_VALUE)
				{vReturn = (DeleteFileA(crx_c_string_getCString(pString)) != 0);}
			else
				{vReturn = false;}
		}
		else
		{
			Crx_C_String tString /*= ?*/;

			crx_c_string_construct(&tString);
			
			if(crx_c_os_osString_unsafeConvertToOsString(&tString, pString))
			{
				tHandle = crx_c_os_fileSystem_private_win32_createFile(true,
					&tString, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, NULL,
					OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

				if(tHandle != INVALID_HANDLE_VALUE)
					{vReturn = (DeleteFileA(crx_c_string_getCString(pString)) != 0);}
				else
					{vReturn = false;}
			}
			else
				{SetLastError(ERROR_BAD_PATHNAME);}//THIS MIGHT NOT BE NECESSARILY CORRECT.

			crx_c_string_destruct(&tString);
		}

		if(tHandle != INVALID_HANDLE_VALUE)
		{
			CloseHandle(tHandle);
			tHandle = INVALID_HANDLE_VALUE;
		}
	}
	
	return (vResult != 0);
}
CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_private_win32_removeDirectory(
		bool pIsNotUtf8, Crx_C_String const * pString)
{
	static Crx_NonCrxed_Os_Windows_Dll_Kernel32_RemoveDirectoryW vFunc_removeDirectoryW = 
			crx_c_os_dll_loadSystemFunction("RemoveDirectoryW", 3, 51, 0, 0, 2);
	bool vIsADirectory = true; //REMEMBER: WE DO NOT USE THIS IN THIS FUNCTION
	bool vReturn = true;
	int32_t vHandle = INVALID_HANDLE_VALUE;
	DWORD vResult = 0;
	
	crx_c_os_fileSystem_internal_win32_addressDirectoryTrailingSlash(pIsNotUtf8, pString, 
			&vIsADirectory);

	if(vFunc_removeDirectoryW != NULL)
	{
		if(pIsNotUtf8)
		{
			if(tCString != NULL)
			{
				vReturn = ((*vFunc_removeDirectoryW)((wchar_t const *)crx_c_string_getCString(
						pString)) != 0);
			}
		}
		else
		{
			Crx_C_String tString /*= ?*/;

			crx_c_string_construct(&tString);
			
			if(crx_c_os_osString_unsafeConvertToOsString(&tString, pString))
			{
				vReturn = ((*vFunc_removeDirectoryW)((wchar_t const *)crx_c_string_getCString(
						&tString)) != 0);
			}
			else
				{SetLastError(ERROR_BAD_PATHNAME);}//THIS MIGHT NOT BE NECESSARILY CORRECT.

			crx_c_string_destruct(&tString);
		}
	}
	else
	{
		if(pIsNotUtf8)
			{vReturn = (RemoveDirectoryA(crx_c_string_getCString(pString)) != 0);}
		else
		{
			Crx_C_String tString /*= ?*/;

			crx_c_string_construct(&tString);
			
			if(crx_c_os_osString_unsafeConvertToOsString(&tString, pString))
				{vReturn = (RemoveDirectoryA(crx_c_string_getCString(pString)) != 0);}
			else
				{SetLastError(ERROR_BAD_PATHNAME);}//THIS MIGHT NOT BE NECESSARILY CORRECT.

			crx_c_string_destruct(&tString);
		}
	}

	crx_c_os_fileSystem_internal_win32_undoAddressingDirectoryTrailingSlash(pIsNotUtf8,
			pString, vIsADirectory);
	
	return (vResult != 0);
}

/*
	IF THIS RETURNS true, THE ENTIRE BUFFER WAS WRITTEN.
*/
CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_private_win32_doWriteFile(HANDLE pHandle,
		void const * pBuffer, size_t pCount, uint64_t pOffset)
{
	OVERLAPPED vOverlapped /*= ?*/;
	unsigned char const * vBuffer = ((unsigned char const *)pBuffer);
	ULARGE_INTEGER vULargeInteger;
	size_t vCount = pCount;
	bool vIsNoError = true;
	
	CRX__SET_TO_ZERO(OVERLAPPED, vOverlapped);
	vULargeInteger.QuadPart = pOffset;

	vOverlapped.Offset = vULargeInteger.u.LowPart;
	vOverlapped.OffsetHigh = vULargeInteger.u.HighPart;

	while(vCount > UINT32_MAX)
	{
		uint32_t tCount = 0;

		if(WriteFile(pHandle, pBuffer, UINT32_MAX, &tCount, &vOverlappeds) != 0)
		{
			vCount = vCount - tCount;
			vBuffer = vBuffer + tCount;
			vULargeInteger.QuadPart = vULargeInteger.QuadPart + tCount;
			vOverlapped.Offset = vULargeInteger.u.LowPart;
			vOverlapped.OffsetHigh = vULargeInteger.u.HighPart;
		}
		else
			{vIsNoError = false;}
	}

	if(vIsNoError)
	{
		while(vCount > 0)
		{
			uint32_t tCount = 0;

			if(WriteFile(pHandle, pBuffer, vCount, &tCount, &vOverlappeds) != 0)
			{
				vCount = vCount - tCount;
				vULargeInteger.QuadPart += tCount;
				vOverlapped.Offset = vULargeInteger.u.LowPart;
				vOverlapped.OffsetHigh = vULargeInteger.u.HighPart;
			}
			else
				{vIsNoError = false;}
		}
	}

	return vIsNoError;
}

CRX__LIB__PRIVATE_C_FUNCTION() int64_t crx_c_os_fileSystem_private_win32_doReadFile(HANDLE pHandle,
		void const * pBuffer, size_t pCount, uint64_t pOffset)
{
	OVERLAPPED vOverlapped /*= ?*/;
	ULARGE_INTEGER vULargeInteger;
	size_t vCount = pCount;
	bool vIsNoError = true;
	
	CRX__SET_TO_ZERO(OVERLAPPED, vOverlapped);
	vULargeInteger.QuadPart = pOffset;

	vOverlapped.Offset = vULargeInteger.u.LowPart;
	vOverlapped.OffsetHigh = vULargeInteger.u.HighPart;

	while(vCount > UINT32_MAX)
	{
		uint32_t tCount = 0;

		if(ReadFile(pHandle, pBuffer, UINT32_MAX, &tCount, &vOverlappeds) != 0)
		{
			vCount = vCount - tCount;
			vULargeInteger.QuadPart += tCount;
			vOverlapped.Offset = vULargeInteger.u.LowPart;
			vOverlapped.OffsetHigh = vULargeInteger.u.HighPart;
		}
		else
		{
			if(GetLastError() != ERROR_HANDLE_EOF)
				{vIsNoError = false;}
		}
	}

	if(vIsNoError)
	{
		while(vCount > 0)
		{
			uint32_t tCount = 0;

			if(ReadFile(pHandle, pBuffer, vCount, &tCount, &vOverlappeds) != 0)
			{
				vCount = vCount - tCount;
				vULargeInteger.QuadPart += tCount;
				vOverlapped.Offset = vULargeInteger.u.LowPart;
				vOverlapped.OffsetHigh = vULargeInteger.u.HighPart;
			}
			else
			{
				if(GetLastError() != ERROR_HANDLE_EOF)
					{vIsNoError = false;}
			}
		}
	}

	return (vIsNoError ? (pCount - vCount) : -1);
}
/*
	WARNING: UNLIKE WHAT CURRENT MICROSOFT DOCUMENTATION STATES, IF YOU USE "PAGE_WRITECOPY" MAKE 
			SURE YOU USED GENERIC_READ AND GENERIC_WRITE WHEN YOU OPENED THE FILE TO GET 
			pFileHandle.
			
	VALID VALUES FOR ARE pProtect
					PAGE_READONLY, 		PAGE_READWRITE, 	PAGE_WRITECOPY
					SEC_COMMIT,			SEC_IMAGE,			SEC_NOCACHE
					SEC_RESERVE

	SEC_RESERVE WHILE VALID, MEANING IT EXISTS AND SHOULD BE RELIABLE ON WINDOWS 95 AND UP, IT
			IS NOT VALID FOR FILESYSTEM WORK BECAUSE IT DOES NOT ALLOW AN EXPLICIT FILE HANDLE.
	
	IF USING 
		- PAGE_READONLY: MAKE SURE FILE WAS OPENED WITH GENERIC_READ
		- PAGE_READWRITE: MAKE SURE FILE WAS OPENED WITH GENERIC_READ AND GENERIC_WRITE
		- PAGE_WRITECOPY: MAKE SURE FILE WAS OPENED WITH GENERIC_READ AND GENERIC_WRITE
			
	PAGE_WRITECOPY DOES NOT EXIST ON WINDOW CE. THIS WILL MAKE A PRIVATE COPY OF THE MAPPED PAGE
			AND GIVE IT TO THE PROCESS WHEN PROCESS WRITES TO THE MAP. THIS EFFECTIVELY MAKES
			THE ACCESS READONLY IN TERMS OF THE FILE SYSTEM, WHILE AT THE SAME TIME ALLOWING
			THE PROCESS TO MODIFY THE MAP IN MEMORY. HOWEVER, ANY MODIFICATION BY THE PROCESS IS
			NOT SEEN BY OTHER PROCESSES.

	<DEPRECATED 
		THIS FUNCTION WILL NOT OPEN A NEW MAPPING TO THE FILE IF A MAPPING TO IT IS ALREADY OPEN, 
		NOR WILL IT RETURN THE ALREADY OPEN MAPPING.
		
		<REASON> I HAVE CONFIRMED THAT CloseHandle CAN STILL BE SAFELY CALLED ON THE HANDLE RETURNED
		WHEN A MAPPING ALREADY EXISTS WITHOUT AFFECTING THE OTHER MAPPING HANDLES TO THE SAME FILE
		</REASON>
	>

	pFileHandle WILL BE CLOSED IF THE FUNCTION IS SUCCESSFUL, AND ITS VALUE SET TO 
			INVALID_HANDLE_VALUE UNLESS pIsToUnsafeKeepFileHandleOpen IS SET TO TRUE. IF THIS IS
			DONE, CALLER MUST NOT USE pFileHandle FOR OTHER ACTIVITY UNTIL THE RETURNED MAPPING 
			HANDLE IS CLOSED.

	IF pSize IS 0, THE CURRENT FILE SIZE IS USED.
*/
CRX__LIB__PRIVATE_C_FUNCTION() HANDLE crx_c_os_fileSystem_private_win32_doCreateFileMapping(
		HANDLE * CRX_NOT_NULL pFileHandle, uint32_t pProtect, 
		uint64_t pSize CRX_DEFAULT(0), bool pIsToUnsafeKeepFileHandleOpen)
{
	ULARGE_INTEGER vULargeInteger;
	HANDLE vReturn = NULL;

	vULargeInteger.QuadPart = pSize;
	vReturn = CreateFileMappingA(pFileHandle, NULL, pProtect, vULargeInteger.u.HighPart,
			vULargeInteger.u.LowPart, NULL);

	/*if(vReturn != NULL)
	{
		//I INCORRECTLY THOUGHT THAT CloseHandle() SHOULD NOT BE USED IN THIS CASE
		if(GetLastError() == ERROR_ALREADY_EXISTS)
			{vReturn = NULL;}
	}*/

	//FILE HANDLE MUST NOT BE USED AFTER THIS ON WINDOWS 95, AND OTHER SOURCES SUGGESTED THAT IT
	//	CAN SIMPLY BE CLOSED.
	if((vReturn != NULL) && pIsToUnsafeKeepFileHandleOpen)
	{
		CloseHandle(*pFileHandle);
		*pFileHandle = INVALID_HANDLE_VALUE;
	}
	
	return vReturn;
}
/*
	
	WARNING. IF FOR pAccessMode YOU USE,
		- FILE_MAP_WRITE, pFileMappingHandle PARAMETER MUST HAVE BEEN CREATED WITH 
				PAGE_READWRITE
		- FILE_MAP_READ, pFileMappingHandle PARAMETER MUST HAVE BEEN CREATED WITH 
				PAGE_READWRITE OR PAGE_READONLY
		- FILE_MAP_ALL_ACCESS, pFileMappingHandle PARAMETER MUST HAVE BEEN CREATED WITH 
				PAGE_READWRITE
		- FILE_MAP_COPY, pFileMappingHandle PARAMETER MUST HAVE BEEN CREATED WITH 
				PAGE_WRITECOPY.
	
	IF 0 IS USED FOR pViewLength, THE ENTIRE FILE IS MAPPED, BUT I DO NOT KNOW WHAT WOULD HAPPEN
			IF THE FILE IS LARGER THAN 4GB. HENCE, IT IS BEST NOT TO USE THIS VALUE.
*/
CRX__LIB__PRIVATE_C_FUNCTION() void * crx_c_os_fileSystem_private_win32_mapViewOfFile(
		HANDLE pFileMappingHandle, uint32_t pAccessMode, uint64_t pOffset, 
		uint32_t pViewLength CRX_DEFAULT(0))
{
	ULARGE_INTEGER vULargeInteger;
	HANDLE vReturn = NULL;

	vULargeInteger.QuadPart = pOffset;
	
	return MapViewOfFile(pFileMappingHandle, pAccessMode, vULargeInteger.u.HighPart,
			vULargeInteger.u.LowPart, pViewLength);
}
/*WARNING: THE POINTER MUST BE THE BASE ADDRESS AND NOT SIMPLY AN ADDRESS IN THE REGION.*/
CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_private_win32_unmapViewOfFile(
		void const * pPointerToBaseAddress)
	{return (UnmapViewOfFile(pPointerToBaseAddress) != 0);}
/*WARNING: THE POINTER MUST BE THE BASE ADDRESS AND NOT SIMPLY AN ADDRESS IN THE REGION.*/
CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_private_win32_doFlushViewOfFile(
		void const * pPointerToBaseAddress /*, bool pIsImmediate*/)
{
	FlushViewOfFile(pPointerToBaseAddress, 0);
	
	if(pIsImmediate)
	{
		/*
			NEED THE UNDERLYING FILE HANDLE IF WE WANT TO GUARANTEE FLUSHING TO DISK BEFORE 
					RETURNING FROM THE ABOVE CALL. IF WE ALSO CALL THE FUNCTION BELOW, ON LINUX IT 
					APPEARS THAT THIS IS EQUIVILANT TO USING MS_SYNC WHEN CALLING msync(). AND IF 
					WE DO NOT CALL THE FUNCTION BELOW, IT IS EQUIVILANT TO USING MS_ASYNC WHEN 
					CALLING msync().
		*/
		//FlushFileBuffers(pFileHandle);
	}
}
/*
	IN REALITY pLength MUST BE SMALLER THAN (UINT64_MAX / 2)
*/
CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_internal_win32_resizeFile(
		HANDLE pFileHandle, bool pIsToFill, uint64_t pLength)
{
	ULARGE_INTEGER vULargeInteger;
	bool vIsNotDone = true;
	bool vReturn = true;

	vULargeInteger.QuadPart = pOffset;

	if(!pIsToFill)
	{
		/*
		GetFileSize() does not return correct size on windows 9x for large files.
		
		ULARGE_INTEGER tULargeInteger;
		
		tULargeInteger.QuadPart = 0;
		
		tULargeInteger.u.LowPart = GetFileSize(pFileHandle, &(tULargeInteger.u.HighPart));

		/*
			IF INSTEAD OF &(tULargeInteger.u.HighPart) WE PASSED NULL, THE CHECK ON GetLastError()
			IS NOT REQUIRED.
		* /
		vReturn = ((tULargeInteger.u.LowPart != 0xFFFFFFFF /*INVALID_FILE_SIZE* /) || 
				(GetLastError() == NO_ERROR));*/

		ULARGE_INTEGER tULargeInteger;
		BY_HANDLE_FILE_INFORMATION tByHandleFileInformation /*= ?*/;
	
		if(crx_c_os_fileSystem_internal_win32_getFileInformationByHandle(pFileHandle, 
				&tByHandleFileInformation) != 0)
		{				
			tULargeInteger.u.HighPart = tByHandleFileInformation.nFileSizeHigh;
			tULargeInteger.u.LowPart = tByHandleFileInformation.nFileSizeLow;
		}
		else
			{vReturn = false;}

		if(vReturn)
		{
			if(pLength > tULargeInteger.QuadPart)
			{
				unsigned char vBuffer[16384];
				uint64_t tOffset = tULargeInteger.QuadPart - 1;
				int64_t tLength = pLength - tULargeInteger.QuadPart;

				memset(&(vBuffer[0]), 0, 16384);

				/*
					SOME SOURCES SUGGESTED THAT THIS IS ONLY NECESSARY FOR WINDOWS 95, AND NOT FOR
					WINDOWS NT DESPITE OFFICIAL DOCUMENTATION.
				*/
				while(tLength >= 16384)
				{
					if(crx_c_os_fileSystem_private_win32_doWriteFile(
							pFileDescriptor, &(vBuffer[0]), 16384, 
							tULargeInteger.QuadPart, tOffset))
					{
						tLength = tLength - 16384;
						tOffset = tOffset + 16384;
					}
					else
						{vReturn = false;}
				}

				if(vReturn)
				{
					if(tLength > 0)
					{
						if(!crx_c_os_fileSystem_private_win32_doWriteFile(
								pFileDescriptor, &(vBuffer[0]), tLength, tOffset))
							{vReturn = false;}
					}
				}

				vIsNotDone = false;
			}
		}
		else
			{vIsNotDone = false;}
	}

	if(vIsNotDone)
	{
		/*
			IF INSTEAD OF &(vULargeInteger.u.HighPart) WE PASSED NULL, THE CHECK ON GetLastError()
			IS NOT REQUIRED.
		*/
		vReturn = (((SetFilePointer(pFileHandle, vULargeInteger.u.LowPart, 
				&(vULargeInteger.u.HighPart), FILE_BEGIN) != 
				0xFFFFFFFF /*INVALID_SET_FILE_POINTER*/) || (GetLastError() == NO_ERROR)) && 
				(SetEndOfFile(pFileHandle) != 0));

		vIsNotDone = false;
	}

	return vReturn;
}

CRX__LIB__PRIVATE_C_FUNCTION() size_t crx_c_os_fileSystem_private_win32_getByteLengthOfRoute(
		Crx_C_String const * CRX_NOT_NULL pTrail)
{
	bool vIsValid = true;
	bool vIsToNormalize = true;
	size_t vReturn = 0;
	Crx_C_String_Sub vSub /*= ?*/;
	
	crx_c_string_sub_construct2(&vSub, pTrail);

	crx_c_os_osString_leftTrimSub(&vSub);

	//crx_c_string_replaceOccurancesOfChar(&vTrail, 0, 92 /* \ */, 47 /* / */, false);
	if(crx_c_string_sub_isBeginningWith2(&vSub, "\\\\", false))
	{
		char const * tChars = NULL;
		
		crx_c_string_sub_leftCut(&vSub, 2);

		vIsValid = false;
		
		tChars = crx_c_string_sub_getFirstChar(&vSub);

		if((crx_c_string_sub_getLength(&vSub) > 1) && (*tChars == 63 /*?*/) &&
				(*(tChars + 1) == 92 /* \ */))
		{
			Crx_C_String_Sub tSub2 = vSub;
			size_t tAvailableLength = 0;

			vIsToNormalize = false;

			crx_c_string_sub_leftCut(&tSub2, 2);
			tAvailableLength = crx_c_string_sub_getLength(&tSub2);
			tChars = crx_c_string_sub_getFirstChar(&vSub2);

			while((tAvailableLength > 0) && (*tChars == 92 /* \ */))
			{
				tChars++;
				tAvailableLength--;
			}

			crx_c_string_sub_leftCut(&tSub2, 
					crx_c_string_sub_getLength(&tSub2) - tAvailableLength);

			crx_c_os_osString_leftTrimSub(&tSub2);

			tAvailableLength = crx_c_string_sub_getLength(&tSub2);
			tChars = crx_c_string_sub_getFirstChar(&vSub2);
			
			if((tAvailableLength > 2) && (CRX__C__IS_CODE_POINT_ASCII_UPPER_CASE_ALPHABET(*tChars) ||
					CRX__C__IS_CODE_POINT_ASCII_LOWER_CASE_ALPHABET(*tChars)) && 
					(*(tChars + 1) == 58 /*:*/) && (*(tChars + 2) == 92 /* \ */))
			{
				crx_c_string_sub_leftCut(&tSub2, 3);
				tChars = tChars + 3; //== crx_c_string_sub_getFirstChar(&vSub2);
				tAvailableLength = tAvailableLength - 3;//== crx_c_string_sub_getLength(&tSub2);

				while((tAvailableLength > 0) && (*tChars == 92 /* \ */))
				{
					tChars++;
					tAvailableLength--;
				}
				
				crx_c_string_sub_leftCut(&tSub2, 
						crx_c_string_sub_getLength(&tSub2) - tAvailableLength);

				vReturn = crx_c_string_sub_getSubStartIndex(&tSub2 + 1);

				vIsValid = true;
			}
			else if((tAvailableLength > 43) && 
					(((*tChars == 86 /*V*/) || (*tChars == 118 /*v*/)) &&
					((*(tChars + 1) == 79 /*O*/) || (*(tChars + 1) == 111 /*o*/)) &&
					((*(tChars + 2) == 76 /*L*/) || (*(tChars + 2) == 108 /*l*/)) &&
					((*(tChars + 3) == 85 /*U*/) || (*(tChars + 3) == 117 /*u*/)) &&
					((*(tChars + 4) == 77 /*M*/) || (*(tChars + 4) == 109 /*m*/)) &&
					((*(tChars + 5) == 69 /*E*/) || (*(tChars + 5) == 101 /*e*/))))
			{
				//FORMAT: \\?\Volume{b75e2c83-0000-0000-0000-602f00000000}\ 

				char const * tChars2 = tChars;

				tChars = tChars + 6;

				if(*tChars == 123 /*{*/)
				{
					uint32_t tCounts[5] = {8, 4, 4, 4, 12};

					tChars++;

					CRX_FOR(size_t tI = 0, tI < 5, tI++)
					{
						uint32_t tCount = tCounts[tI];

						while(vIsValid && (tCount > 0))
						{
							if(CRX__C__IS_CODE_POINT_ASCII_HEX_DIGIT(*tChars))
								{tChars++;}
							else
								{vIsValid = false;}
						}

						if(vIsValid)
						{
							if(tI != 4)
							{
								if(tChars != 45 /*-*/)
								{
									vIsValid = false;
									break;
								}
								else
									{tChars++;}
							}
						}
						else
							{break;}
					}
					CRX_ENDFOR

					if(vIsValid && (*tChars != 125 /*}*/))
						{vIsValid = false;}
				}

				if(vIsValid)
				{
					crx_c_string_sub_leftCut(&tSub2, 44);
					crx_c_os_osString_leftTrimSub(&tSub2);

					if((crx_c_string_sub_getLength(&tSub2) > 0) && 
							(crx_c_string_sub_getCharAt(&tSub2, 0) == 92 /* \ */))
					{
						tChars2 = tChars2 + 6;

						while((crx_c_string_sub_getLength(&tSub2) > 0) &&
								(crx_c_string_sub_getCharAt(&tSub2, 0) == 47 /* / */))
							{crx_c_string_sub_leftCut(&tSub2, 1);}

						crx_c_os_osString_leftTrimSub(&tSub2);

						vReturn = crx_c_string_sub_getSubStartIndex(&tSub2 + 1);
					}
					else
						{vIsValid = false;}
				}
			}
			else
				{vIsValid = false;}
		}
		else
			{vIsValid = false;}
	}
	else
	{
		Crx_C_String_Sub tSub = vSub;
		char const * tChars = crx_c_string_sub_getFirstChar(&tSub);
		size_t tAvailableLength = crx_c_string_sub_getLength(&tSub);
		
		vIsToNormalize = true;

		if((tAvailableLength > 1) &&
				(CRX__C__IS_CODE_POINT_ASCII_UPPER_CASE_ALPHABET(*(tChars)) ||
					CRX__C__IS_CODE_POINT_ASCII_LOWER_CASE_ALPHABET(*(tChars))) &&
					(*(tChars + 1) == 58 /*:*/))
		{
			if((tAvailableLength > 2) && ((*(tChars + 2) == 47 /* / */) ||
					(*(tChars + 2) == 92 /* \ */)))
			{
				crx_c_string_sub_leftCut(&tSub, 3);

				vReturn = crx_c_string_sub_getSubStartIndex(&tSub + 1);
			}
			else
				{vIsValid = false;}
		}
		else
			{vIsValid = false;}
	}

	return vReturn;
}

CRX__LIB__PRIVATE_C_FUNCTION() bool 
		crx_c_os_fileSystem_private_win32_areWin32FindDataLikelyForSameEntry(
		Crx_C_Os_FileSystem_Internal_Win32_Win32FindData * pWin32FindData, 
		Crx_C_Os_FileSystem_Internal_Win32_Win32FindData * pWin32FindData__2)
{
	if((pWin32FindData->dwFileAttributes == pWin32FindData__2->dwFileAttributes) &&
			(pWin32FindData->ftLastWriteTime == pWin32FindData__2->ftLastWriteTime) &&
			crx_c_string_isEqualTo(&(pWin32FindData->cFileName), 
					&(pWin32FindData__2->cFileName)) &&
			crx_c_string_isEqualTo(&(pWin32FindData->cAlternateFileName), 
					&(pWin32FindData__2->cAlternateFileName)))
	{
		if(pWin32FindData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{return true;}
		else
		{
			return ((pWin32FindData->nFileSizeHigh == pWin32FindData__2->nFileSizeLow) &&
					(pWin32FindData->nFileSizeHigh == pWin32FindData__2->nFileSizeLow));
		}
	}
	else
		{return false;}
}
/*
	WARNING: IF YOU MODIFY, MAKE SURE TO UPDATE 
			crx_c_os_fileSystem_iterator_verifyRecordAgainst()
			crx_c_os_fileSystem_iterator_verifyRecordAgainst2()
*/
CRX__LIB__PRIVATE_C_FUNCTION() bool 
		crx_c_os_fileSystem_private_win32_areWin32FindDataAndByHandleFileInformatinLikelyForSameEntry(
		Crx_C_Os_FileSystem_Internal_Win32_Win32FindData * CRX_NOT_NULL pWin32FindData, 
		BY_HANDLE_FILE_INFORMATION * CRX_NOT_NULL pByHandleFileInformation)
{
	if((pWin32FindData->dwFileAttributes == pByHandleFileInformation->dwFileAttributes) &&
			(pWin32FindData->ftLastWriteTime == pByHandleFileInformation->ftLastWriteTime))
	{
		if(pWin32FindData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{return true;}
		else
		{
			return ((pWin32FindData->nFileSizeHigh == pByHandleFileInformation->nFileSizeLow) &&
					(pWin32FindData->nFileSizeHigh == pByHandleFileInformation->nFileSizeLow));
		}
	}
	else
		{return false;}
}






CRX__LIB__PUBLIC_C_FUNCTION() Crx_C_Os_FileSystem_Time * crx_c_os_fileSystem_time_new()
	{return ((Crx_C_Os_FileSystem_Time *)calloc(1, sizeof(Crx_C_Os_FileSystem_Time)));}
CRX__LIB__PUBLIC_C_FUNCTION() Crx_C_Os_FileSystem_Time * crx_c_os_fileSystem_time_moveNew(
		Crx_C_Os_FileSystem_Time * CRX_NOT_NULL pTime)
{
	Crx_C_Os_FileSystem_Time * vReturn = 
			((Crx_C_Os_FileSystem_Time *)malloc(sizeof(Crx_C_Os_FileSystem_Time)));

	if(vReturn != NULL)
		{memcpy(vReturn, pTime, sizeof(Crx_C_Os_FileSystem_Time));}

	return vReturn;
}
CRX__LIB__PUBLIC_C_FUNCTION() Crx_C_Os_FileSystem_Time * crx_c_os_fileSystem_time_copyNew(
		Crx_C_Os_FileSystem_Time const * CRX_NOT_NULL pTime)
{
	Crx_C_Os_FileSystem_Time * vReturn = 
			((Crx_C_Os_FileSystem_Time *)malloc(sizeof(Crx_C_Os_FileSystem_Time)));

	if(vReturn != NULL)
		{memcpy(vReturn, pTime, sizeof(Crx_C_Os_FileSystem_Time));}

	return vReturn;
}
		
CRX__C__TYPE_BLUE_PRINT__DEFINE_GET_BLUE_PRINT(
		Crx_C_Os_FileSystem_Time, Crx_C_Os_FileSystem_Time_,
		CRXM__TRUE, CRXM__FALSE,
		CRXM__TRUE, CRXM__FALSE,
		CRXM__FALSE, CRXM__FALSE)
		
CRX__LIB__PUBLIC_C_FUNCTION() crx_c_os_fileSystem_time_free(Crx_C_Os_FileSystem_Time * pThis)
	{free(pThis);}

CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_time_add(
		Crx_C_Os_FileSystem_Time * pThis, Crx_C_Os_FileSystem_Time * CRX_NOT_NULL pTime)
{
	pThis->gSeconds = pThis->gSecond + pTime->gSecond;
	pThis->gPrivate_femtoSeconds = pThis->gPrivate_femtoSeconds + pTime->gPrivate_femtoSeconds;
	
	if(pThis->gPrivate_femtoSeconds >= UINT64_C(1000000000000000))
	{
		pThis->gSeconds = pThis->gSeconds + 1;
		pThis->gPrivate_femtoSeconds = pThis->gPrivate_femtoSeconds - UINT64_C(1000000000000000);
	}
}
CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_time_subtract(
		Crx_C_Os_FileSystem_Time * pThis, Crx_C_Os_FileSystem_Time * CRX_NOT_NULL pTime)
{
	if(pThis->gPrivate_femtoSeconds < pTime->gPrivate_femtoSeconds)
	{
		pThis->gPrivate_femtoSeconds = pThis->gPrivate_femtoSeconds + UINT64_C(1000000000000000);
		pThis->gSeconds = pThis->gSeconds - 1;
	}

	pThis->gSeconds = pThis->gSecond - pTime->gSecond;
	pThis->gPrivate_femtoSeconds = pThis->gPrivate_femtoSeconds - pTime->gPrivate_femtoSeconds;
}

CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_time_copyAssignFrom(
		Crx_C_Os_FileSystem_Time * pThis, Crx_C_Os_FileSystem_Time const * CRX_NOT_NULL pTime)
{
	pThis->gSeconds = pTime->gSeconds;
	pThis->gPrivate_femtoSeconds = pTime->gPrivate_femtoSeconds;
}

CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_time_setFemtoSeconds(
		Crx_C_Os_FileSystem_Time * pThis, uint64_t pFemtoSeconds)
{
	if(pFemtoSeconds >= UINT64_C(1000000000000000))
		{pThis->gPrivate_femtoSeconds = UINT64_C(999999999999999);}
	else
		{pThis->gPrivate_femtoSeconds = pFemtoSeconds;}
}
CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_time_getFemtoSeconds(
		Crx_C_Os_FileSystem_Time * pThis, uint64_t * CRX_NOT_NULL pFemtoSeconds)
	{*pFemtoSeconds = pThis->gPrivate_femtoSeconds;}

CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_time_private_setFromTimespec(
		Crx_C_Os_FileSystem_Time * pThis, 
		Crx_NonCrxed_Os_Posix_Dll_Libc_Timespec const * CRX_NOT_NULL pTimespec)
{
	pThis->gSeconds = pTimespec->tv_sec;
	pThis->gPrivate_femtoSeconds = pTimespec->tv_nsec * 1000000;
}
CRX__LIB__PRIVATE_C_FUNCTION() void crx_c_os_fileSystem_time_private_setFromTimeval(
		Crx_C_Os_FileSystem_Time * pThis, 
		Crx_NonCrxed_Os_Posix_Dll_Libc_Timeval const * CRX_NOT_NULL pTimeval)
{
	pThis->gSeconds = pTimespec->tv_sec;
	pThis->gPrivate_femtoSeconds = pTimespec->tv_usec * 1000000000;
}
CRX__LIB__PRIVATE_C_FUNCTION() void crx_c_os_fileSystem_time_private_setFromFileTime(
		Crx_C_Os_FileSystem_Time * pThis, FILETIME const * CRX_NOT_NULL pFileTime)
{
	ULARGE_INTEGER vuLargeInteger;
	uint64_t vUint64;
	
	vuLargeInteger.LowPart = pFileTime.dwLowDateTime;
	vuLargeInteger.HighPart = pFileTime.dwHighDateTime;
	vUint64 = vuLargeInteger.QuadPart;
	
	//WORRIED THAT THE COMPILER WOULD SIMPLY ((a/b) * b) INTO a INSTEAD OF DOING THE CALCULATION,
	//	WHICH IF a IS AN INTEGER WOULD BE INCORRECT. I DO NOT THINK ANY COMPILER WOULD DO THIS,
	//	BUT JUST IN CASE.
	pThis->gSeconds = ((int64_t)(vUint64 / UINT64_C(10000000)));
	pThis->gPrivate_femtoSeconds = (vUint64 - (pThis->gSeconds * UINT64_C(10000000))) * 
			UINT64_C(100000000);
	//DIFFERENCE FROM UNIX EPOCH, AND MICROSOFT'S EPOCH.
	pThis->gSeconds = pThis->gSeconds + UINT64_C(11644473600);
}

CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_time_private_getAsTimespec(
		Crx_C_Os_FileSystem_Time * pThis, 
		Crx_NonCrxed_Os_Posix_Dll_Libc_Timespec * CRX_NOT_NULL pReturn)
{
	pReturn->tv_sec = pThis->gSeconds;
	pReturn->tv_nsec = pThis->gPrivate_femtoSeconds / 1000000;
}
CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_time_private_getAsTimeval(
		Crx_C_Os_FileSystem_Time * pThis, 
		Crx_NonCrxed_Os_Posix_Dll_Libc_Timeval * CRX_NOT_NULL pReturn)
{
	pReturn->tv_sec = pThis->gSeconds;
	pReturn->tv_usec = pThis->gPrivate_femtoSeconds / 1000000000;
}
CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_time_private_getAsFileTime(
		Crx_C_Os_FileSystem_Time * pThis, FILETIME * CRX_NOT_NULL pReturn)
{
	CRX_SCOPE_META
	if((pThis->gSeconds < 0) || 
			(pThis->gSeconds > (UINT64_C(1844674407370) + UINT64_C(11644473600))))
		{return false;}

	CRX_SCOPE
	ULARGE_INTEGER vuLargeInteger;
	uint64_t vUint64_t = (((int64_t)(pThis->gSeconds)) - UINT64_C(11644473600)) * UINT64_C(10000000);

	vUint64_t = vUint64_t + (pThis->gPrivate_femtoSeconds / UINT64_C(100000000));
	
	vuLargeInteger.QuadPart = vUint64_t;
	pReturn->dwLowDateTime = vuLargeInteger.LowPart;
	pReturn->dwHighDateTime = vuLargeInteger.HighPart;
	CRX_SCOPE_END
}

//TIME FROM THIS FUNCTION SHOULD NOT BE USED TO MULTITHREADING TIMING PURPOSES. THE TIME IS NOT
//		GUARANTEED TO BE INCREASING ONLY.
CRX__LIB__PRIVATE_C_FUNCTION() bool crx_c_os_fileSystem_time_setToCurrentTime(
		Crx_C_Os_FileSystem_Time * pThis, FILETIME * CRX_NOT_NULL pReturn)
{
#if((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS))
	Crx_NonCrxed_Os_Posix_Dll_Libc_Timespec vTimespec /*= ?*/;

	CRX__SET_TO_ZERO(Crx_NonCrxed_Os_Posix_Dll_Libc_Timespec, vTimespec);

	//REMEMBER: THE FOLLOWING NEEDS TO BE ENCAPSULATED IN ITS OWN FUNCTION FOR DYNAMIC LOADING.
	//		LEAVING THIS FOR NOW BECAUSE THIS WILL HAS TO BE PART OF THE MULTITHREADING WORK
	clock_gettime(CLOCK_REALTIME, &vTimespec);

	crx_c_os_fileSystem_time_private_setFromTimespec(pThis, &vTimespec);
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	FILETIME vFileTime /*= ?*/;
    SYSTEMTIME vSystemTime /*= ?*/;

    GetSystemTime(&vSystemTime);
    SystemTimeToFileTime(&vSystemTime, &vFileTime);  

	crx_c_os_fileSystem_time_private_setFromFileTime(pThis, &vFileTime);
#endif
}

/*
	THIS FUNCTIONS RETURNS THE TIME IN MILLI SECONDS SUITABLE FOR ::crx::c::pthread.
	
	IF THE TIME IS NEGATIVE, 0 IS RETURNED
	IF THE TIME IN SECONDS IS LARGER THAN (INT64_MAX / 1000) UINT32_MAX IS RETURNED.
	IF THE TIME IN MILLISECONDS IS POSITIVE, BUT LARGER THAN UINT32_MAX / 1000, THE NUMBER RETURNED 
			IS THE REMAINDER OF DIVIDING BY UINT32_MAX.
*/

CRX__LIB__PUBLIC_C_FUNCTION() uint32_t crx_c_os_fileSystem_time_getAsThreadingInterval(
		Crx_C_Os_FileSystem_Time * pThis)
{
	if(pThis->gSeconds < 0)
		{return 0;}
	else
	{
		if(pThis->gSeconds <= 4294966) //(UINT32_MAX / 1000) - 1
		{
			return ((uint64_t)((pThis->gSeconds * 1000) + 
					(pThis->gPrivate_femtoSeconds / UINT64_C(1000000000000))));
		}
		else if(pThis->gSeconds > 9223372036854774) //INT64_MAX / 1000 - 1
			{return UINT32_MAX;}
		else
		{
			uint64_t tMilliseconds = ((uint64_t)(((pThis->gSeconds * 1000) % 
					((int64_t)UINT32_MAX)) + (pThis->gPrivate_femtoSeconds / 
					UINT64_C(1000000000000))));

			return ((uint32_t)(tMilliseconds % UINT32_MAX));
		}
	}
}


CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_byFileKey_construct(
		Crx_C_Os_FileSystem_ByFileKey * pThis)
{
	pThis->gPrivate_osFileHandle = CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE;
	crx_c_string_construct(&(pThis->gPrivate_fileFullPath));
}
CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_byFileKey_copyConstruct(
		Crx_C_Os_FileSystem_ByFileKey * pThis,
		Crx_C_Os_FileSystem_ByFileKey const * CRX_NOT_NULL pByFileKey)
	{exit();}

CRX__LIB__PUBLIC_C_FUNCTION() Crx_C_Os_FileSystem_ByFileKey * crx_c_os_fileSystem_byFileKey_new()
{
	Crx_C_Os_FileSystem_ByFileKey * vReturn = 
			((Crx_C_Os_FileSystem_ByFileKey *)malloc(sizeof(Crx_C_Os_FileSystem_ByFileKey)));

	if(vReturn != NULL)
		{crx_c_os_fileSystem_byFileKey_construct(vReturn);}

	return vReturn;
}
CRX__LIB__PUBLIC_C_FUNCTION() Crx_C_Os_FileSystem_ByFileKey * crx_c_os_fileSystem_byFileKey_moveNew(
		Crx_C_Os_FileSystem_ByFileKey * CRX_NOT_NULL pByFileKey)
{
	Crx_C_Os_FileSystem_ByFileKey * vReturn = 
			((Crx_C_Os_FileSystem_ByFileKey *)malloc(sizeof(Crx_C_Os_FileSystem_ByFileKey)));

	if(vReturn != NULL)
		{memcpy(vReturn, pByFileKey, sizeof(Crx_C_Os_FileSystem_ByFileKey));}

	return vReturn;
}
CRX__LIB__PUBLIC_C_FUNCTION() Crx_C_Os_FileSystem_ByFileKey * crx_c_os_fileSystem_byFileKey_copyNew(
		Crx_C_Os_FileSystem_ByFileKey const * CRX_NOT_NULL pByFileKey)
	{exit();}

CRX__C__TYPE_BLUE_PRINT__DEFINE_GET_BLUE_PRINT(
		Crx_C_Os_FileSystem_ByFileKey, crx_c_os_fileSystem_byFileKey_,
		CRXM__TRUE, CRXM__TRUE,
		CRXM__FALSE, CRXM__TRUE,
		CRXM__FALSE, CRXM__FALSE)

CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_byFileKey_free(
		Crx_C_Os_FileSystem_ByFileKey * pThis)
	{free(pThis);}
CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_byFileKey_destruct(
		Crx_C_Os_FileSystem_ByFileKey * pThis)
	{crx_c_string_destruct(&(pThis->gPrivate_fileFullPath));}
	
CRX__LIB__PUBLIC_C_FUNCTION() void crx_c_os_fileSystem_byFileKey_internal_set(
		Crx_C_Os_FileSystem_ByFileKey * pThis, 
		Crx_C_Os_FileSystem_OsFileHandle pOsFileHandle;
		Crx_C_String * CRX_NOT_NULL pInternalFileFullPath)
{
	pThis->gPrivate_osFileHandle = pOsFileHandle;

	if(crx_c_string_tryToPilferSwapContentWith(&(pThis->gPrivate_fileFullPath), 
			pInternalFileFullPath))
		{crx_c_string_empty(pInternalFileFullPath);}
	else
	{
		crx_c_string_destruct(&(pThis->gPrivate_fileFullPath));
		memcpy(&(pThis->gPrivate_fileFullPath), pInternalFileFullPath, sizeof(Crx_C_String));
		crx_c_string_construct(pInternalFileFullPath);
	}
}

CRX__LIB__PUBLIC_C_FUNCTION() bool crx_c_os_fileSystem_byFileKey_isSet(
		Crx_C_Os_FileSystem_ByFileKey * pThis)
	{return (pThis->gPrivate_osFileHandle != CRX__C__OS__FILE_SYSTEM__OS_FILE_NULL_HANDLE);}
CRX__LIB__PUBLIC_C_FUNCTION() bool crx_c_os_fileSystem_byFileKey_unlock(
		Crx_C_Os_FileSystem_ByFileKey * pThis)
{
	
}


CRX__LIB__C_CODE_END()