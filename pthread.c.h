#if !WINDOWS
	#include <sys/time.h>
	
	/*#if(MACOS)
		#include <mach/clock.h>
		#include <mach/mach.h>
	#endif*/
#endif
#if LINUX
	#include <unistd.h>
#endif
#if MACOS
	#include <libkern/OSAtomic.h>
	/*#include <os/lock.h> IF YOU ARE USING os_unfair_lock DIRECTLY, BUT WE DO NOT BECAUSE
			WE RELY ON dlsym().
	*/
#endif

//CRX__C__PTHREAD__PRIVATE__IS_INLINE_ASSEMBLY_AVAILABLE
#if((CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__MSVC) || \
		(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__AT_AND_T))
	#define CRX__C__PTHREAD__PRIVATE__IS_INLINE_ASSEMBLY_AVAILABLE CRXM__TRUE
#elif((CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__OTHER) && \
		(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__ARMCC) && \
		(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__ARM))
	#define CRX__C__PTHREAD__PRIVATE__IS_INLINE_ASSEMBLY_AVAILABLE CRXM__TRUE
#else
	#define CRX__C__PTHREAD__PRIVATE__IS_INLINE_ASSEMBLY_AVAILABLE CRXM__FALSE
#endif
//CRX__C__PTHREAD__PRIVATE__IS_TO_USE_ASSEMBLY_FOR_ATOMICS
#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	#if((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__X86) || \
			(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__AMD64))
		#if(CRX__C__PTHREAD__PRIVATE__IS_INLINE_ASSEMBLY_AVAILABLE)
			#define CRX__C__PTHREAD__PRIVATE__IS_TO_USE_ASSEMBLY_FOR_ATOMICS CRXM__TRUE
		#endif
	#endif
#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX)
	#if(CRX__C__PTHREAD__PRIVATE__IS_INLINE_ASSEMBLY_AVAILABLE)
		#define CRX__C__PTHREAD__PRIVATE__IS_TO_USE_ASSEMBLY_FOR_ATOMICS CRXM__TRUE
	#endif
#elif((CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__SOLARIS))
	#define CRX__C__PTHREAD__PRIVATE__IS_TO_USE_ASSEMBLY_FOR_ATOMICS CRXM__FALSE
#else
	#if((__STDC_VERSION__ >= 201112L) && !defined(__STDC_NO_ATOMICS__))
	#else
		#if((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__X86) || \
				(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__AMD64) || \
				(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__ARM) || \
				(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__ARM64))
			#if(CRX__C__PTHREAD__PRIVATE__IS_INLINE_ASSEMBLY_AVAILABLE)
				#define CRX__C__PTHREAD__PRIVATE__IS_TO_USE_ASSEMBLY_FOR_ATOMICS CRXM__TRUE
			#endif
		#elif(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__POWER_PC)
			#if(CRX__ARCHITECTURE__SIZE > 32)
				#if(CRX__C__PTHREAD__PRIVATE__IS_INLINE_ASSEMBLY_AVAILABLE)
					#define CRX__C__PTHREAD__PRIVATE__IS_TO_USE_ASSEMBLY_FOR_ATOMICS CRXM__TRUE
				#endif
			#endif
		#elif(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__SPARC)
			#if(CRX__ARCHITECTURE__SIZE > 32)
				#if(CRX__C__PTHREAD__PRIVATE__IS_INLINE_ASSEMBLY_AVAILABLE)
					#define CRX__C__PTHREAD__PRIVATE__IS_TO_USE_ASSEMBLY_FOR_ATOMICS CRXM__TRUE
				#endif
			#endif
		#endif
	#endif
#endif
#if(!defined(CRX__C__PTHREAD__PRIVATE__IS_TO_USE_ASSEMBLY_FOR_ATOMICS))
	#define CRX__C__PTHREAD__PRIVATE__IS_TO_USE_ASSEMBLY_FOR_ATOMICS CRXM__FALSE
#endif
//MSVC INTRINSICS
/*
	REFERENCE: BOOST V1.82.0, 	boost\atomic\detail\interlocked.hpp
	THE FOLLOWING LIKELY WAS MEANT TO SUPPORT MORE THAN JUST MSVC. HENCE, KEEPING THE _MSC_VER 
			CHECK.
			
	THE FOLLOWING IS MEANT FOR WINDOWS XP 64BIT, WHICH DOES NOT SUPPORT THE 64BIT INTRINSICS, AND
	WHEN THE COMPILER DOES NOT SUPPORT MSVC OR EXTENDED AT&T ASSEMBLY. FOR THIS REASON, ONLY THE 
	INTRINSICS ARE SUPPORTED. THE SOLUTION ONLY WORKS ON VISUAL STUDIO 2005 AND UP, WHICH IS NOT A 
	ISSUE BECAUSE THE CORRECT FORM OF C/C++, THE MAPPING TO THE ABSTRACT LANGUAGE, DOES NOT 
	PERMIT VISUAL STUDIO 2002 AND 2003 ALREADY, WHILE VISUAL STUDIO 6 CAN NOT GIVE 64BIT BUILDS.
*/
#if(defined(_MSC_VER) && _MSC_VER >= 1400)
	//VISUAL STUDIO 2005 IS INDEED MSVC
	#if(CRX__ARCHITECTURE__SIZE > 32)
		//MOVED UP, BUT KEPT HERE FOR PROPER REFERENCE
		/*#if(!defined(_WIN32_WCE))
			//IT APPEARS, JUDGING FROM BOOST'S CODE THAT THE FOLLOWING HEADER IS SUPPLIED BY OTHER 
			//		COMPILERS.
			#include <intrin.h>
		#else
			///UNSURE IF THIS IS SUPPLIED BY OTHER COMPILERS
			#include <cmnintrin.h>
		#endif*/
		#if(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__MICROSOFT)
			//32BIT ONES ARE NOT NEEDED, BUT KEEPING FOR REFERENCE BECAUSE IT WOULD SEEM THAT 
			//		COMPILERS MIGHT PROVIDE THESE INTRINSICS AS FUNCTIONS INSTEAD, AND IF THESE
			//		COMPILERS DO NOT ALLOW SUFFICIENT INLINE ASSEMBLY PER OUR CURRENT CAPACITY,
			//		WE CAN USE THESE 'INTRINSICS'.
			//#pragma intrinsic(_InterlockedCompareExchange)
			//#pragma intrinsic(_InterlockedExchangeAdd)
			//#pragma intrinsic(_InterlockedExchange)
			//#pragma intrinsic(_InterlockedIncrement)
			//#pragma intrinsic(_InterlockedDecrement)
			//#pragma intrinsic(_InterlockedAnd)
			//#pragma intrinsic(_InterlockedOr)
			#pragma intrinsic(_InterlockedCompareExchange64)
			#pragma intrinsic(_InterlockedExchangeAdd64)
			#pragma intrinsic(_InterlockedExchange64)
			#pragma intrinsic(_InterlockedIncrement64)
			#pragma intrinsic(_InterlockedDecrement64)
			#pragma intrinsic(_InterlockedAnd64)
			#pragma intrinsic(_InterlockedOr64)
		#endif
	#endif
#endif // defined(_MSC_VER)
//CRX__C__PTHREAD__PRIVATE__ARE_GNU_ATOMIC_BUILTINS_AVAILABLE
//CRX__C__PTHREAD__PRIVATE__ARE_GNU_ATOMIC_BUILTINS_AVAILABLE
#if(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__GCC)
	/*
		"When you invoke the Clang-based front end of IBM® XL C/C++ for AIX® 16.1 using the xlclang 
				or xlclang++ invocation command, GCC compare and swap built-in functions are not 
				supported. Use the C++11 atomics features instead."
				{SRC: https://www.ibm.com/docs/en/xl-c-and-cpp-aix/16.1.0?topic=functions-sync-val-compare-swap}
		GCC DOCUMENTATION STATES THAT IN MOST CASES THE __sync FUNCTION ARE FULL MEMORY BARRIERS. 
				HOWEVER, BEING UNCLEAR, THE CODE BELOW MUST CONTINUE TO	ISSUE FULL MEMORY BARRIERS
				FOR INTERLOCKED ATOMIC OPERATIONS.
		ON OLDER GCC VERSIONS, GCC APPEARS TO DEFAULT TO A TARGET ARCHITECTURE OF i386, AND WITH
				THIS SOME, IF NOT ALL, OF THE __sync FUNCTIONS DO NOT WORK. USERS MUST INSTEAD USE
				THE SETTING "-march=pentium"
	*/
	#if((CRX__COMPILER__MAJOR_VERSION > 4) || ((CRX__COMPILER__MAJOR_VERSION == 4) && \
			(CRX__COMPILER__MINOR_VERSION >= 1)))
		#if((CRX__COMPILER__MAJOR_VERSION > 4) || (CRX__COMPILER__MINOR_VERSION >= 8))
			#define CRX__C__PTHREAD__PRIVATE__ARE_GNU_ATOMIC_BUILTINS_AVAILABLE CRXM__TRUE
		#else
			#if(CRX__COMPILER__MINOR_VERSION >= 3)
				#if(defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4) && \
						defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_8))
					//THE TWO MACROS ABOVE APPEAR TO GUARANTEE NOT ONLY DEFINITION, AND AN 
					//		IMPLEMENTATION THAT DOES NOT USE EXTERNAL LIBRARIES
					#define CRX__C__PTHREAD__PRIVATE__ARE_GNU_SYNC_BUILTINS_AVAILABLE CRXM__TRUE
				#endif
			#else
				//JUST AN ASSUMPTION, WHICH COULD BE WRONG.
				#define CRX__C__PTHREAD__PRIVATE__ARE_GNU_SYNC_BUILTINS_AVAILABLE CRXM__TRUE
			#endif
		#endif
	#endif
#elif(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__DIGNUS)
	#if((CRX__COMPILER__MAJOR_VERSION > 2) || ((CRX__COMPILER__MAJOR_VERSION == 2) && \
			(CRX__COMPILER__MINOR_VERSION >= 25)))
		#define CRX__C__PTHREAD__PRIVATE__ARE_GNU_ATOMIC_BUILTINS_AVAILABLE CRXM__TRUE
	#endif
#endif
#if(!defined(CRX__C__PTHREAD__PRIVATE__ARE_GNU_ATOMIC_BUILTINS_AVAILABLE))
	#define CRX__C__PTHREAD__PRIVATE__ARE_GNU_ATOMIC_BUILTINS_AVAILABLE CRXM__FALSE
#endif
#if(!defined(CRX__C__PTHREAD__PRIVATE__ARE_GNU_SYNC_BUILTINS_AVAILABLE))
	#define CRX__C__PTHREAD__PRIVATE__ARE_GNU_SYNC_BUILTINS_AVAILABLE CRXM__FALSE
#endif


#define CRX__C__PTHREAD__PRIVATE__IS_TO_USE_LOCK_FOR_SYNC CRXM__FALSE



#if MACOS
mach_timebase_info_data_t gCrx_c_pthread_private_timebaseInfoData = {0, 0};
#endif

/*
	ABOUT MONOTONIC TIME:
		- IS NEVER DECREASING TIME.
		- IS NOT RELIABLE IN COUNTING ELAPSED TIME. IF A SECOND PASSES IN MONOTONIC TIME, ACTUAL 
				TIME PASSED MIGHT BE MUCH LARGER.
*/
int32_t crx_c_pthread_crx_computeCurrentMonotonicTime(uint64_t * pNanoSeconds__out)
{
#if WINDOWS
	//NOTE THAT ON WINDOWS ONE COULD USE GetTickCount() FOR WIN95 AND UP, OR GetTickCount64().
	//  GetTickCount() WOULD MAKE YOU LIMITED TO 49.7 DAYS UPTIME.
	static DWORD vDword__last = 0;
	static uint64_t vCycleCount = 0;
	DWORD vDword = GetTickCount();
	int32_t vReturn = 0;
	
	
	if(vDword < vDword__last)
	{
		vReturn = pthread_mutex_lock(&gCrx_c_pthread_private_mutex__monotonicTime);

		if(vReturn == 0)
		{
			vDword = GetTickCount();

			/*THE FOLLOWING ASSUME THAT WE DID NOT MISS MORE THAN ONE OVERFLOW*/
			if(vDword < vDword__last)
				{vCycleCount = vCycleCount + ((DWORD)(-1));}

			pthread_mutex_unlock(&gCrx_c_pthread_private_mutex__monotonicTime);
		}
	}

	if(vReturn == 0)
		{*pNanoSeconds__out = (vCycleCount + vDword) * 1000000;}
	
	return vReturn;
#elif APPLE
    uint64_t tTimeRead = mach_absolute_time();
	uint64_t vTime =  tTimeRead * (
			((double)gCrx_c_pthread_private_timebaseInfoData.numer) / 
			((double)gCrx_c_pthread_private_timebaseInfoData.denom));//IN 1E-9 SECONDS (NANOSECONDS)

	/*
		mach_absolute_time() USES THE RDTSC INSTRUCTION, WHICH ON A MULTICORE, DEPENDING HOW
				THE CPU IS SET UP CAN GIVE DIFFERENT TIMES ACROSS DIFFERENT CORES MAKING IT
				STRICTLY NON MONOTONIC. THE EFFECT, HOWEVER, APPEARS TO BE SMALLER THAN 1MS.
				SEE https://github.com/dotnet/runtime/issues/4394
				NOTE THAT ABOVE, THE VALUES OF numer AND denom, SHOULD BE WITHIN THE SAFE
				RANGE OF double FOR INTEGERS, MEANING 1e53 OR 1e52, I FORGET.
		IF YOU EVER NORMALIZE ATOMIC INSTRUCTIONS, YOU CAN DO SOMETHING SIMILAR TO 
						https://serce.me/posts/16-05-2019-the-matter-of-time
				AND AVOID ROUNDING TO 1MS.
		ONE COULD ALSO USE A SOLUTION OF host_get_clock_service(), clock_get_time() AND
				SYSTEM_CLOCK, BUT THAT APPROACH CAN BE AFFECTED ON OLDER MACOS IF SOMEONE CALLS
				clock_set_time().
	*/
	vTime = vTime / 1000000;
	*pNanoSeconds__out = vTime * 1000000;

    return 0;
#else
	struct timespec vTimespec;
	/*
		NOTE: I LEFT THE WORK ON THIS LIBRARY USING CLOCK_REALTIME. I FORGET WHY. MIGHT HAVE BEEN
				BECAUSE I DID NOT KNOW WHAT TO DO ABOUT THE LOWER POWER STATE ISSUE THAT EXISTS ON
				LINUX AND WINDOWS ATLEAST, WHERE MONOTONIC TIME IS NOT COUNTED WHILE IN SUSPEND. 
				HOWEVER, I ALREADY RESOLVED THIS IN THE NOTES SAYING THAT CLOCK_MONOTONIC SHALL
				BE DEFINED AS AN UNRELIABLE NEVER DECREASING TIME.
				I CHANGED THE FOLLOWING TO CLOCK_MONOTONIC FOR THE TIME BEING. CLOCK_MONOTONIC 
				EXISTS ON LINUX AND FREEBSD VERSIONS THAT I SUPPORT.
	*/
	//int32_t vReturn = clock_gettime(CLOCK_REALTIME, vTimespec);
	int32_t vReturn = clock_gettime(CLOCK_MONOTONIC, vTimespec);

	if(vReturn == 0)
	{
		*pNanoSeconds__out = (((uint64_t)vTimespec.tv_sec) * 1000000000) + 
				vReturn.tv_nsec;
	}
	
	return vReturn;
#endif
}
void crx_c_pthread_crx_getTimespecFromNanoseconds(struct timespec * pReturn,
		uint64_t pNanoSeconds)
{
	pReturn->tv_sec = pNanoSeconds / 1000000000;
    pReturn->tv_nsec = (pNanoSeconds - (pTimespec__out.tv_sec * 1000000000));
}
#if(!WINDOWS)
int32_t crx_c_pthread_private_getCurrentRealTime(
		struct timespec * CRX_NOT_NULL pTimespec__out)
{
	struct timeval vTimeval;
	int32_t vReturn = gettimeofday(&vTimeval, NULL);

	if(vReturn == 0)
    {
		pTimespec__out->tv_sec = vTimeval.tv_sec;
		pTimespec__out->tv_nsec = vTimeval.tv_usec * 1000;
	}
	else
		{vReturn = errno;}

	return vReturn;
}
#endif
struct timespec crx_c_pthread_private_getTimeDifference(struct timespec const * pTimespec__from, 
		struct timespec const * pTimespec__to, bool * pTimespec__isDifferenceNegative)
{
	int64_t vNanoSeconds = (pTimespec__to.tv_sec * 1000000000) + 
			pTimespec__to.tv_nsec - (pTimespec__from.tv_sec * 1000000000) -
			pTimespec__from.tv_nsec;
	struct timespec vReturn;

	if(vNanoSeconds < 0)
	{
		vNanoSeconds = 0 - vNanoSeconds;

		vReturn.tv_sec = vNanoSeconds / 1000000000;
		vReturn.tv_nsec = vNanoSeconds - (vReturn.tv_nsec * 1000000000);

		*pTimespec__isDifferenceNegative = true;
	}
	else
	{
		vReturn.tv_sec = 0;
		vReturn.tv_nsec = 0;
		*pTimespec__isDifferenceNegative = false;
	}

	return vReturn;
}
int32_t crx_c_pthread_private_getTimeRemaining(struct timespec * pTimespec__monotonicTime, 
		struct timespec * pTimespec__return)
{
	uint32_t vTime = 0;
	int32_t vReturn = crx_c_pthread_crx_computeCurrentMonotonicTime(&vTime);

	if(vReturn == 0)
    {
		struct timespec tTimespec /*= ?*/;
		struct timespec tTimeSpec_difference;
		bool tIsNegative = false;

		crx_c_pthread_crx_getTimespecFromNanoseconds(&tTimespec, vTime);
		tTimeSpec_difference = crx_c_pthread_private_getTimeDifference(&tTimespec,
				pTimespec__monotonicTime, &tIsNegative);

		if(tIsNegative)
		{
			pTimespec__return->tv_sec = 0;
			pTimespec__return->tv_nsec = 0;
		}
	}
	else
		{vReturn = errno;}

	return vReturn;
}
int32_t crx_c_pthread_private_getIsTimeRemaining(struct timespec * pTimespec__monotonicTime, 
		bool * pIsTimeRemaining)
{
	uint32_t vTime = 0;
	int32_t vReturn = crx_c_pthread_crx_computeCurrentMonotonicTime(&vTime);

	if(vReturn == 0)
    {
		struct timespec tTimespec /*= ?*/;
		bool tIsNegative = false;

		crx_c_pthread_crx_getTimespecFromNanoseconds(&tTimespec, vTime);
		*pIsTimeRemaining = ((tTimespec.tv_sec < pTimespec__monotonicTime.tv_sec) || 
				((tTimespec.tv_sec == pTimespec__monotonicTime.tv_sec) && 
						(tTimespec.tv_nsec < pTimespec__monotonicTime.tv_nsec)));
	}
	else
		{vReturn = errno;}

	return vReturn;
}
int32_t crx_c_pthread_private_getIsTimeRemaining2(uint64_t pNanoSeconds__monotonic, 
		bool * pIsTimeRemaining)
{
	uint64_t vTime = 0;
	int32_t vReturn = crx_c_pthread_crx_computeCurrentMonotonicTime(&vTime);

	if(vReturn == 0)
		{*pIsTimeRemaining = (vTime > pNanoSeconds__monotonic);}
	else
		{vReturn = errno;}

	return vReturn;
}


uint32_t crx_c_pthread_crx_getNumberOfLogicalCoresAvailableToProcess(void)
{
#if(windows)
	return pthread_num_processors_np();
#elif(linux)
	/*linux freebsd*/
	/*return sysconf(_SC_NPROCESSORS_ONLN);*/
	cpu_set_t vCpuSet /*= ?*/;
	uint32_t vReturn = 0;
	
	CPU_ZERO(&vCpuSet);	
	sched_getaffinity(0, sizeof(cpu_set_t), &vCpuSet);
	
	CRX_FOR(uint32_t tI = 0; tI < 64; tI++)
	{
		if(CPU_ISSET(tI, &vCpuSet))
			{vReturn++;}
		else
			{break;}
	}
	CRX_ENDFOR

	return vReturn;
#elif(freebsd)
	cpuset_t vCpuSet = 0;
	uint32_t vReturn = 0;

	CPU_ZERO(&vCpuSet);
	cpuset_getaffinity(CPU_LEVEL_WHICH, CPU_WHICH_PID, -1, sizeof(cpuset_t), &vCpuSet);

	CRX_FOR(uint32_t tI = 0; tI < 64; tI++)
	{
		if(CPU_ISSET(tI, &vCpuSet))
			{vReturn++;}
		else
			{break;}
	}
	CRX_ENDFOR

	return vReturn;
#elif(maxos)
	/*
	machdep.cpu.core_count: NUMBER OF PHYSICAL CORES AVAILABLE. EQUIVILANT TO hw.physicalcpu. 
			THERE IS ALSO hw.physicalcpu_max, hw.ncpu FOR ALL PHYSICAL CORES.
	machdep.cpu.thread_count: NUMBER OF LOGICAL CORES AVAILABLE. EQUIVILANT TO hw.logicalcpu,
			THERE IS ALSO hw.logicalcpu_max FOR ALL LOGICAL CORES.
			
	WHILE sysconf(_SC_NPROCESSORS_ONLN) APPEARS AVAILABLE ON MACOS, IT MIGHT BE UNRELIABLE. NEEDS
	CHECKING.
	
	NOTE: THE FOLLOWING CAN CURRENTLY RETURN AN OVERESTIMATE OF THE AVAILABLE CORES TO THE PROCESS.
	*/
	int32_t core_count = 1;
	size_t len = sizeof(core_count);
	
	return sysctlbyname("machdep.cpu.thread_count", &core_count, &len, 0, 0);
	
	/*
	FREEBSD, MACOS.
	int mib[4];
	int numCPU;
	std::size_t len = sizeof(numCPU);
	
	mib[0] = CTL_HW;
	mib[1] = HW_AVAILCPU;  // alternatively, try HW_NCPU (SAME AS hw.ncpu);

	sysctl(mib, 2, &numCPU, &len, NULL, 0);

	if (numCPU < 1) 
	{
		mib[1] = HW_NCPU;
		sysctl(mib, 2, &numCPU, &len, NULL, 0);
		if (numCPU < 1)
			numCPU = 1;
	}
	*/
#else
	return 1;
#endif
}
uint32_t crx_c_pthread_crx_getNumberOfOnlineLogicalCores(void)
{
#if(windows)
	SYSTEM_INFO vSystemInfo;

	GetSystemInfo(&vSystemInfo);

	return vSystemInfo.dwNumberOfProcessors;
#elif(linux freebsd)
	return sysconf(_SC_NPROCESSORS_ONLN);
#elif(maxos)
	int32_t vCount = 1;
	size_t vSize = sizeof(int32_t);
	
	return sysctlbyname("machdep.cpu.logicalcpu", &vCount, &vSize, 0, 0);
#else
	return 1;
#endif
}
void crx_c_pthread_crx_sleep(uint32_t pNumberOfMilliSeconds)
{
#if(WINDOWS)
	Sleep(pNumberOfMilliSeconds);
#else
	/*
		LINUX ALSO HAS select() WHICH IS MORE ACCURATE, UNLIKE nanosleep WHICH CAN CAUSE A DELAY
		OF 15MS WHEN ASKING FOR 1MS.
		
		MACOS CAN CAUSE VERY LARGE DELAYS WHEN ASKING FOR 1MS BECAUSE OF "TIMER COALESCING". THE
		FUNCTION "mach_wait_until" SEEM TO SUFFER FROM THE SAME PROBLEMS. THE DELAY CAN BE 60 TIMES
		LARGER FROM WHAT I HAVE SEEN.
	*/
	struct timespec tTimespec;
	struct timespec tTimespec__remaining;

	tTimespec.tv_sec  = 0;
	tTimespec.tv_nsec = pNumberOfMilliSeconds * 1000000 /*1MS*/;

	while(nanosleep(&tTimespec, &tTimespec__remaining) == EINTR)
		{tTimespec = tTimespec__remaining;}
#endif
}

void crx_c_pthread_crx_getRawThreadId(Crx_C_Pthread_RawId * CRX_NOT_NULL pReturn)
{
#if(WINDOWS)
	*pReturn = GetCurrentThreadId();
#else
	*pReturn = pthread_self();
#endif
}
bool crx_c_pthread_crx_isSameThreadAs(Crx_C_Pthread_RawId * CRX_NOT_NULL pRawId)
{
#if(WINDOWS)
	return (pRawId == GetCurrentThreadId());
#else
	return (pthread_equal(pRawId, pthread_self()) != 0);
#endif
}
bool crx_c_pthread_crx_areThreadsTheSame(Crx_C_Pthread_RawId * CRX_NOT_NULL pRawId,
		Crx_C_Pthread_RawId * CRX_NOT_NULL pRawId__2)
{
#if(WINDOWS)
	return (pRawId == pRawId__2);
#else
	return (pthread_equal(pRawId, pRawId__2) != 0);
#endif
}

void * crx_c_pthread_getspecific(Crx_C_Pthread_Key pKey)
{
	if(pKey == NULL)
		{return NULL;}

	if(pKey->gPrivate_isExtended)
	{
		Crx_C_Pthread_Private_ThreadData * tThreadData = 
				(Crx_C_Pthread_Private_ThreadData *)pthread_getspecific(
				gCrx_c_pthread_internal_key__threadData);

		if((tThreadData != NULL) && (tThreadData->gExtendedTlsSlots != NULL) && 
				(pKey->gPrivate_crx_key < CRX__C__PTHREAD__PRIVATE__EXTENDEDTLS_SIZE))
			{return (*(tThreadData->gExtendedTlsSlots + pKey->gPrivate_crx_key));}
		else
			{return NULL;}
	}
	else
		{return pthread_getspecific(pKey->gPrivate_Key);}
}
int32_t crx_c_pthread_setspecific(Crx_C_Pthread_Key pKey, void const * pValue)
{
	if(pKey == NULL)
		{return ENOENT;}

	if(pKey->gPrivate_isExtended)
	{
		Crx_C_Pthread_Private_ThreadData * tThreadData = 
				(Crx_C_Pthread_Private_ThreadData *)pthread_getspecific(
				gCrx_c_pthread_internal_key__threadData);
		int32_t tReturn = 0;
				
		if(tThreadData != NULL)
		{
			if(tThreadData->gExtendedTlsSlots == NULL)
			{
				tThreadData->gExtendedTlsSlots = calloc(CRX__C__PTHREAD__PRIVATE__EXTENDEDTLS_SIZE, 
						sizeof (void *));

				if(tThreadData->gExtendedTlsSlots == NULL)
					{tReturn = ENOMEM;}
			}

			if(tReturn == 0)
			{
				if(pKey->gPrivate_crx_key < CRX__C__PTHREAD__PRIVATE__EXTENDEDTLS_SIZE)
					{(*(tThreadData->gExtendedTlsSlots + pKey->gPrivate_crx_key)) = pValue;}
				else
					{tReturn = EINVAL;}
			}
		}
		else
			{tReturn = ENOENT;}

		return tReturn;
	}
	else
		{return pthread_setspecific(pKey->gPrivate_Key, pValue);}
}

void crx_c_pthread_exit(void * pValue)
	{return pthread_exit(pValue);}
int32_t crx_c_pthread_once(Crx_C_Pthread_Once * pOnce, void (* pFunc)(void))
	{return pthread_once(pOnce, pFunc);}
int32_t crx_c_pthread_initializeACrxRtCompliantMutexAttribute(pthread_mutexattr_t * pMutexAttr)
	{return crx_c_pthread_mutexattr_init(pMutexAttr);}

/*----*/

int32_t crx_c_pthread_thread_create(Crx_C_Pthread_Thread * pThread, 
		const Crx_C_Pthread_Attr * pAttr,
		void * (PTW32_CDECL * pFunc)(void *), void * pArguments)
	{return crx_c_pthread_thread_private_create(pThread, NULL, pAttr, pFunc, pArguments);}
#if(CRX__C__PTHREAD__PRIVATE__IS_USING_CUSTOM_PTHREAD)
int32_t crx_c_pthread_thread_crx_create(Crx_C_Pthread_Thread * pThread, 
		char const * pName, size_t pSizeOfName, Crx_C_Pthread_Attr const * pAttr,
		void * (PTW32_CDECL * pFunc)(void *), void * pArguments)
	{return crx_c_pthread_thread_private_create(pThread, pName, pAttr, pFunc, pArguments);}
#endif
int32_t crx_c_pthread_thread_private_create(Crx_C_Pthread_Thread * pThread, 
		char const * pName, size_t pSizeOfName, Crx_C_Pthread_Attr const * pAttr,
		void * (PTW32_CDECL * pFunc)(void *), void * pArguments)
{
	Crx_C_Pthread_Attr vAttr /*= ?*/;
	Crx_C_Pthread_Attr * vAttr__pointer = NULL;
	bool vIsToCleanup = false;
	int32_t vReturn = 0;

	if(pAttr == NULL)
	{
		vReturn = crx_c_pthread_attr_init(&vAttr);

		if(tReturn == 0)
		{
			vAttr__pointer = &vAttr;
			vIsToCleanup = true;
		}
	}
	else
		{vAttr__pointer = pAttr;}

	if(tReturn == 0)
	{
#if(!CRX__C__PTHREAD__PRIVATE__IS_USING_CUSTOM_PTHREAD)
		tReturn = pthread_create(pThread, vAttr__pointer, pFunc, pArguments);
#else
		Crx_C_Pthread_Private_Thread tThread = malloc(Crx_C_Pthread_Private_Thread);
		int tDetachedState = 0;

		tReturn = pthread_attr_getdetachstate(vAttr__pointer, &tDetachedState);
	
		if((tThread != NULL) && tReturn == 0)
		{
			pthread_mutexattr_t tMutexAttr;

			pThread->gPrivate_next = NULL;
			pThread->gPrivate_prev = NULL;
			/*pThread->gPrivate_cancelState = PTHREAD_CANCEL_ENABLE;
			pThread->gPrivate_cancelType = PTHREAD_CANCEL_DEFERRED;
			pThread->gPrivate_wasCancelRequestMade = false;*/
			pThread->gPrivate_isFinished = false;
			pThread->gPrivate_func_main = pFunc;
			pThread->gPrivate_userArguments = pArguments;
			pThread->gPrivate_userData = NULL;
			pThread->gPrivate_isDetached = (tDetachedState == PTHREAD_CREATE_DETACHED);
			pThread->gPrivate_threadPool = NULL;

			if((pName != NULL) && (pSizeOfName > 0))
			{
				size_t tLength = ((pSizeOfName <= 15) ? pSizeOfName : 15);

				pThread->gPrivate_name = malloc(tLength + 1);

				if(pThread->gPrivate_name != NULL)
				{
					memcpy(pThread->gPrivate_name, pName, tLength);
					*(pThread->gPrivate_name + tLength) = '\0';
				}
			}
			
			tReturn = crx_c_pthread_initializeACrxRtCompliantMutexAttribute(&tMutexAttr);

			if(tReturn == 0)
			{
				tReturn = crx_c_pthread_mutex_init(&(pThread->gPrivate_mutex__mail), &tMutexAttr);
				
				if(tReturn == 0)
				{
					tReturn = crx_c_pthread_cond_crx_init(&(pThread->gPrivate_cond__mail));

					if(tReturn != 0)
						{crx_c_pthread_mutex_destroy(&(pThread->gPrivate_mutex__mail));}
				}

				pthread_mutexattr_destroy(&tMutexAttr);
			}
			
			if(tReturn == 0)
			{
				&pThread = tThread;
			
				tReturn = pthread_create(&(pThread->gPrivate_pthread), vAttr__pointer, 
						crx_c_pthread_thread_private_create__threadMain, NULL);
			}
			else
			{
				free(tThread);
				tThread = NULL;
			}
		}
		else
			{tReturn = ENOMEM;}
#endif		
	}

	if(vIsToCleanup)
		{crx_c_pthread_attr_destroy(vAttr__pointer);}

	return tReturn;
}
#if(CRX__C__PTHREAD__PRIVATE__IS_USING_CUSTOM_PTHREAD)
void crx_c_pthread_thread_private_create__threadMain(void * pArguments)
{
	Crx_C_Pthread_Thread vThread = crx_c_pthread_thread_self();
	
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE);
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED);

	if(vThread->gPrivate_name != NULL)
	{
	#if(LINUX WINDOWS)
		pthread_setname_np(vThread->gPrivate_pthread, vThread->gPrivate_name);
	#elif(MACOS)
		pthread_setname_np(vThread->gPrivate_name);
	#elif(FREEBSD)
	#endif
	}

	(*gCrx_c_crxRt_private_func_executeWithPthreadCleanupPushAndPop)(
	vThread->gPrivate_func_main, vThread->gPrivate_userArguments,
	crx_c_pthread_thread_private_cleanUpIfDetached, vThread, 0);

	if(vThread->gPrivate_isDetached)
		{crx_c_pthread_thread_private_cleanUp(vThread);}
	else
	{
		if(crx_c_pthread_mutex_lock(&(vThread->gPrivate_mutex__mail)) == 0)
		{
			vThread->gPrivate_isFinished = true;
			crx_c_pthread_cond_signal(&(vThread->gPrivate_cond__mail));
			crx_c_pthread_mutex_crx_unlock(&(vThread->gPrivate_mutex__mail));
		}
		else
		{
			/*
				IDEALY, THE MUTEX SHOULD BE LOCKED FIRST. HOWEVER IF IT CAN NOT BE DONE,
				IT SHOULD BE NOT A PROBLEM HERE IN ANY CASE.
			*/
			vThread->gPrivate_isFinished = true;
			crx_c_pthread_cond_signal(&(vThread->gPrivate_cond__mail));
		}
	}
}
void crx_c_pthread_thread_private_cleanUpIfDetached(void * pThread)
{
	Crx_C_Pthread_Thread vThread = (Crx_C_Pthread_Thread) pThread;

	if(vThread->gPrivate_isDetached)
		{crx_c_pthread_thread_private_cleanUp(pThread);}
	else
	{
		if(crx_c_pthread_mutex_lock(&(vThread->gPrivate_mutex__mail)) == 0)
		{
			vThread->gPrivate_isFinished = true;
			crx_c_pthread_cond_signal(&(vThread->gPrivate_cond__mail));
			crx_c_pthread_mutex_crx_unlock(&(vThread->gPrivate_mutex__mail));
		}
		else
		{
			/*
				IDEALY, THE MUTEX SHOULD BE LOCKED FIRST. HOWEVER IF IT CAN NOT BE DONE,
				IT SHOULD BE NOT A PROBLEM HERE IN ANY CASE.
			*/
			vThread->gPrivate_isFinished = true;
			crx_c_pthread_cond_signal(&(vThread->gPrivate_cond__mail));
		}
	}
}
void crx_c_pthread_thread_private_cleanUp(Crx_C_Pthread_Thread pThread)
{
	int32_t vResult = 0;

	if(pThread->gPrivate_next != NULL)
		{pThread->gPrivate_next->gPrivate_prev = pThread->gPrivate_prev;}
	if(pThread->gPrivate_prev != NULL)
		{pThread->gPrivate_prev->gPrivate_next = pThread->gPrivate_next;}
	
	vResult = crx_c_pthread_mutex_destroy(&(pThread->gPrivate_mutex__mail));
	while(vResult == EBUSY)
	{
		crx_c_pthread_sched_crx_runOtherThreadsInstead();
		vResult = crx_c_pthread_mutex_destroy(&(pThread->gPrivate_mutex__mail));
	}
	
	vResult = crx_c_pthread_cond_destroy(&(pThread->gPrivate_cond__mail));
	while(vResult == EBUSY)
	{
		crx_c_pthread_sched_crx_runOtherThreadsInstead();
		vResult = crx_c_pthread_cond_destroy(&(pThread->gPrivate_cond__mail));
	}

	if(pThread->gPrivate_mails != NULL)
	{
		crx_c_queue_destruct(pThread->gPrivate_mails);
		crx_c_queue_free(pThread->gPrivate_mails);
		pThread->gPrivate_mails = NULL;
	}

	if(pThread->gPrivate_postponedMails != NULL)
	{
		crx_c_sortedQueue_destruct(pThread->gPrivate_postponedMails);
		crx_c_sortedQueue_free(pThread->gPrivate_postponedMails);
		pThread->gPrivate_postponedMails = NULL;
	}

	if(pThread->gPrivate_threadPool != NULL)
		{crx_c_pthread_thread_crx_detachFromThreadPool(pThread);}

	//LEAVING NAME UNTIL THE END TO AID DEBUGGING.
	if(vThread->gPrivate_name != NULL)
	{
		free(vThread->gPrivate_name);
		vThread->gPrivate_name = NULL;
	}
	
	free(*Crx_C_Pthread_Thread);
}

int32_t crx_c_pthread_getname_np(Crx_C_Pthread_Thread pThread, char * pBuffer, size_t pLength)
{
	size_t vLength = strlen(pThread->gPrivate_name);

	if(vLength >= pLength)
		{vLength = pLength - 1;}

	memcpy(pBuffer, pThread->gPrivate_name, vLength);
}
#endif
int32_t crx_c_pthread_thread_cancel(Crx_C_Pthread_Thread pThread)
{
#if(!CRX__C__PTHREAD__PRIVATE__IS_USING_CUSTOM_PTHREAD)
	return pthread_cancel(pThread);
#else
	pthread_t vThread = pThread->gPrivate_pthread;

	/*pThread->gPrivate_wasCancelRequestMade = true;

	if(pThread->gPrivate_cancelState == PTHREAD_CANCEL_ENABLE)
	{
		if(pThread->gPrivate_cancelType == PTHREAD_CANCEL_ASYNCHRONOUS)
		{
			if(pThread->gPrivate_isDetached)
				{crx_c_pthread_thread_private_cleanUp(pThread);}
		}
	}*/

	return pthread_cancel(vThread);
#endif
}
int32_t crx_c_pthread_thread_detach(Crx_C_Pthread_Thread pThread)
{
#if(!CRX__C__PTHREAD__PRIVATE__IS_USING_CUSTOM_PTHREAD)
	return pthread_detach(pThread);
#else
	if(!pThread->gPrivate_isDetached)
	{
		pthread_t tThread = pThread->gPrivate_pthread;
		int32_t tReturn = 0;
		/*bool vWasCleanuped = false;

		if(pThread->gPrivate_cancelState == PTHREAD_CANCEL_ENABLE)
		{
			if(pThread->gPrivate_cancelType == PTHREAD_CANCEL_ASYNCHRONOUS)
			{
				if(pThread->gPrivate_wasCancelRequestMade)
				{
					vWasCleanuped = true;
					crx_c_pthread_thread_private_cleanUp(pThread);
				}
			}
		}

		if(!vWasCleanuped)
			{pThread->gPrivate_isDetached = true;}*/
		pThread->gPrivate_isDetached = true;
		
		tReturn = pthread_detach(tThread);

		if(pThread->gPrivate_isFinished) //THIS MIGHT REQUIRE A MEMORY BARRIER
			{crx_c_pthread_thread_private_cleanUp(pThread);}

		return tReturn;
	}
#endif
}
#if(CRX__C__PTHREAD__PRIVATE__IS_USING_CUSTOM_PTHREAD)
int32_t crx_c_pthread_thread_isDetached(Crx_C_Pthread_Thread pThread)
	{return (pThread->gPrivate_isDetached ? 1: 0);}
#endif
int32_t crx_c_pthread_thread_equal(Crx_C_Pthread_Thread pThread, Crx_C_Pthread_Thread pThread__2)
{
#if(!CRX__C__PTHREAD__PRIVATE__IS_USING_CUSTOM_PTHREAD)
	return ((pthread_equal(pThread, pThread__2) == 0) ? 0 : 1);
#else
	return ((pthread_equal(pThread->gPrivate_pthread, pThread__2->gPrivate_pthread) == 0) ? 0 : 1);
#endif
}
int32_t crx_c_pthread_thread_join(Crx_C_Pthread_Thread pThread, void * * pValue)
{
	assert((pValue == NULL) || (*pValue == NULL));
#if(!CRX__C__PTHREAD__PRIVATE__IS_USING_CUSTOM_PTHREAD)
	return pthread_join(pThread, pValue);
#else
	int32_t vReturn = pthread_join(pThread->gPrivate_pthread, pValue);

	crx_c_pthread_thread_private_cleanUp(pThread);
#endif
}
#if(CRX__C__PTHREAD__PRIVATE__IS_USING_CUSTOM_PTHREAD)
int32_t crx_c_pthread_crx_timedjoin(Crx_C_Pthread_Thread pThread, void * * pValue, 
		uint64_t pNanoSeconds__monotonic)
{
	int32_t vReturn = 0;

	if(!pThread->gPrivate_isFinished)
	{
		bool tIsTimeRemaining = true;

		vReturn = crx_c_pthread_mutex_crx_timedlock(&(pThread->gPrivate_mutex__mail),
				pNanoSeconds__monotonic);

		if(vReturn == 0)
		{
			while(!pThread->gPrivate_isFinished)
			{
				vReturn = crx_c_pthread_cond_crx_timedwait(
						&(pThread->gPrivate_cond__mail), &((*pThread)->gPrivate_mutex__mail), 
						pNanoSeconds__monotonic);

				if(vReturn == 0)
				{
					vReturn = crx_c_pthread_private_getIsTimeRemaining2(pNanoSeconds__monotonic, 
							&tIsTimeRemaining);

					if(vReturn == 0)
					{
						if(!tIsTimeRemaining)
						{
							vReturn = ETIMEDOUT;

							break;
						}
					}
					else
						{break;}
				}
				else
					{break;}
			}

			crx_c_pthread_mutex_unlock(&(pThread->gPrivate_mutex__mail));

			if(vReturn == 0)
			{
				assert(pThread->gPrivate_isFinished);

				vReturn = pthread_join(pThread->gPrivate_pthread, pValue);
				crx_c_pthread_thread_private_cleanUp(pThread);
			}
		}
	}
	else
	{
		vReturn = pthread_join(pThread->gPrivate_pthread, pValue);
		crx_c_pthread_thread_private_cleanUp(pThread);
	}
	
	return vReturn;
}
int32_t crx_c_pthread_crx_tryjoin_np(Crx_C_Pthread_Thread pThread, void * * pValue)
{
	if(!pThread->gPrivate_isFinished)
		{return EBUSY;}
	else
	{
		/*
			WE COULD ALSO CALL crx_c_pthread_thread_join() BUT THE FOLLOWING IS AN IMPLICIT TEST
			TO SEE THAT CLEANING UP IN crx_c_pthread_thread_detach() AFTER THE THREAD MAIN IS
			FINISHED CONTINUES TO WORK. THE FOLLOWING IS ALSO USING PTHREAD WIN32 APPROACH
		*/
		return crx_c_pthread_thread_detach(pThread);
	}
}
#endif
int32_t crx_c_pthread_thread_crx_isThreadValid(Crx_C_Pthread_Thread pThread)
{
#if(!CRX__C__PTHREAD__PRIVATE__IS_USING_CUSTOM_PTHREAD)
	return ((pthread_kill(pThread, 0) == 0) ? 0 : 1);
#else
	return ((pThread != NULL) ? ((pthread_kill(pThread->gPrivate_pthread, 0) == 0) ? 0 : 1) : 0);
#endif
}
Crx_C_Pthread_Thread crx_c_pthread_thread_self(void)
{
#if(!CRX__C__PTHREAD__PRIVATE__IS_USING_CUSTOM_PTHREAD)
	return pthread_self();
#else
	Crx_C_Pthread_Private_ThreadData * vThreadData = ((Crx_C_Pthread_Private_ThreadData *)
			pthread_getspecific(gCrx_c_pthread_internal_key__threadData));

	if(vThreadData != NULL)
		{return vThreadData->gThread->gPrivate_pthread;}
	else
		{return NULL;}
#endif
}
int32_t crx_c_pthread_thread_setcancelstate(int32_t pState, int32_t * pOldState)
{
	int vOldState = 0;
	int32_t vReturn = 0;
#if(CRX__C__PTHREAD__PRIVATE__IS_USING_CUSTOM_PTHREAD)
	/*Crx_C_Pthread_Thread vThread = crx_c_pthread_thread_self();*/
#endif

#if(!CRX__C__PTHREAD__PRIVATE__IS_USING_CUSTOM_PTHREAD)
	vReturn = pthread_setcancelstate(pState, &vOldState);
#else
	/*if(vThread != NULL)
	{
		vThread->gPrivate_cancelState = pState;

		if(vThread->gPrivate_cancelState == PTHREAD_CANCEL_ENABLE)
		{
			if(vThread->gPrivate_cancelType == PTHREAD_CANCEL_ASYNCHRONOUS)
			{
				if(vThread->gPrivate_isDetached)
				{
					if(vThread->gPrivate_wasCancelRequestMade)
						{crx_c_pthread_thread_private_cleanUp(vThread);}
				}
			}
		}

		vReturn = pthread_setcancelstate(pState, &vOldState);
	}
	else
		{vReturn = EINVAL;}*/
	vReturn = pthread_setcancelstate(pState, &vOldState);
#endif

	*pOldState = vOldState;

	return vReturn;
}
int32_t crx_c_pthread_thread_setcanceltype(int32_t pType, int32_t * pOldType)
{
	int vOldType = 0;
	int32_t vReturn = 0;
#if(CRX__C__PTHREAD__PRIVATE__IS_USING_CUSTOM_PTHREAD)
	/*Crx_C_Pthread_Thread vThread = crx_c_pthread_thread_self();*/
#endif

#if(!CRX__C__PTHREAD__PRIVATE__IS_USING_CUSTOM_PTHREAD)
	vReturn = pthread_setcanceltype(pType, &vOldType);
#else
	/*if(vThread != NULL)
	{
		vThread->gPrivate_cancelType = pType;
		vReturn = pthread_setcanceltype(pType, &vOldType);
	}
	else
		{vReturn = EINVAL;}*/
	vReturn = pthread_setcanceltype(pType, &vOldType);
#endif

	*pOldType = vOldType;

	return vReturn;
}
bool crx_c_pthread_thread_crx_isSelfAPthread(void)
{
#if(WINDOWS)
	#if(!CRX__C__PTHREAD__PRIVATE__IS_USING_CUSTOM_PTHREAD)
	return pthread_ptw32_isSelfAPthread();
	#else
	return (crx_c_pthread_thread_self() != NULL);
	#endif
#else
	return true;
#endif
}
/*bool crx_c_pthread_thread_crx_isSelfANativePthread(void)
{
#if(WINDOWS)
	return pthread_ptw32_isSelfAPthread();
#else
	return true;
#endif
}*/
void crx_c_pthread_thread_testcancel(void)
{
#if(CRX__C__PTHREAD__PRIVATE__IS_USING_CUSTOM_PTHREAD)
	/*
	IF THIS WAS THE ONLY CANCELATION POINT, THIS WOULD HAVE BEEN FEASABLE.
	UNFORTUANETLY THERE ARE MANY CANCELATION POINTS, A LOT OUTSIDE THE PTHREAD
	LIBRARY.
	UPDATE: TO UNDERSTAND THE ABOVE TAKE FOR EXAMPLE, fcntl() UNDER LINUX, A 
	FUNCTION WHICH IS A CANCELLATION POINT. fcntl() WOULD NOT BE CALLING MY OWN 
	NORMALIZED PTHREAD FUNCTIONS, INCLUDIG THIS VERY FUNCTION TO CREATE ITS 
	CANCELATION POINT, AND ABSTRACTLY IT IS CALLING pthread_thread_testcancel() 
	DIRECTLY INSTEAD. HENCE, I CAN NOT INTERCEPT IT. I INTERCEPT WHEN MY OWN 
	NORMALIZING crx_c_pthread_thread_testcancel() IS CALLED.

	Crx_C_Pthread_Thread vThread = crx_c_pthread_thread_self();
	
	if(vThread != NULL)
	{
		if(vThread->gPrivate_cancelState == PTHREAD_CANCEL_ENABLE)
		{
			//WHETHER PTHREAD_CANCEL_ASYNCHRONOUS OR PTHREAD_CANCEL_DEFERRED
			if(pThread->gPrivate_isDetached)
			{
				if(pThread->gPrivate_wasCancelRequestMade)
					{crx_c_pthread_thread_private_cleanUp(pThread);}
			}
		}
	}*/
#endif

	pthread_testcancel();
}
#if(CRX__C__PTHREAD__PRIVATE__IS_USING_CUSTOM_PTHREAD)
int32_t crx_c_pthread_thread_crx_readOneMail(uint64_t * pSenderId, uint64_t * pShortData,
		void * * pMessage)
{
	CRX_SCOPE_META
	assert((pMessage == NULL) || (*pMessage == NULL));

	CRX_SCOPE
	Crx_C_Pthread_Thread vThread = crx_c_pthread_thread_self();
	vWasMailFound = false;
	Crx_C_Pthread_Crx_Private_Mail vMail;
	int32_t vReturn = 0;

	if(vThread != NULL)
	{
		vReturn = crx_c_pthread_mutex_lock(&(vThread->gPrivate_mutex__mail));

		if(vReturn == 0)
		{
			if((vThread->Crx_C_SortedQueue != NULL) && 
					(crx_c_sortedQueue_getLength(vThread->Crx_C_SortedQueue) > 0))
			{
				uint64_t tTime = crx_c_pthread_crx_computeCurrentMonotonicTime();
				Crx_C_Pthread_Crx_Private_PostponedMail tPostponedMail = 
						crx_c_sortedQueue_getFront(vThread->Crx_C_SortedQueue);

				if(tPostponedMail.gDueTime >= tTime)
				{
					crx_c_sortedQueue_popFromFront(vThread->Crx_C_SortedQueue);

					vWasMailFound = true;
					vMail = tPostponedMail->gMail;
				}
			}
			
			if(!vWasMailFound && (vThread->gPrivate_mails != NULL) && 
					(crx_c_queue_getLength(vThread->gPrivate_mails) > 0))
			{
				vWasMailFound = true;
				vMail = *(crx_c_queue_getFront(vThread->gPrivate_mails));
				
				crx_c_queue_popFromFront(vThread->gPrivate_mails);
			}

			crx_c_pthread_mutex_unlock(&(vThread->gPrivate_mutex__mail));
		}
	}

	if(vWasMailFound)
	{
		void * tMessage = NULL;

		if(vMail->gFunc != NULL)
			{tMessage = (*(vMail->gFunc))(vMail->gArguments);}
		else
			{tMessage = vMail->gArguments;}

		if(pSenderId != NULL)
			{*pSenderId = vMail->gSenderId;}
		if(pShortData != NULL)
			{*pShortData = vMail->gShortData;}
		if(pMessage != NULL)
			{*pMessage = tMessage;}
	}

	return vReturn;
	CRX_SCOPE_END
}
int32_t crx_c_pthread_thread_crx_waitForMail(void)
{
	Crx_C_Pthread_Thread vThread = crx_c_pthread_thread_self();
	int32_t vReturn = 0;

	if(vThread != NULL)
	{
		vReturn = crx_c_pthread_mutex_lock(&(vThread->gPrivate_mutex__mail));

		if(vReturn == 0)
		{
			while((vReturn == 0) && (vThread->gPrivate_mails == NULL) && 
					(vThread->gPrivate_postponedMails == NULL) &&
					(crx_c_queue_getLength(vThread->gPrivate_mails) == 0) &&
					(crx_c_sortedQueue_getLength(vThread->gPrivate_postponedMails) == 0))
			{
				vReturn = crx_c_pthread_cond_wait(&(vThread->gPrivate_cond__mail),
						&(vThread->gPrivate_mutex__mail));
			}

			crx_c_pthread_mutex_unlock(&(pThis->gPrivate_mutex__mail));
		}
	}
	else
		{vReturn = EINVAL;}

	return vReturn;
}
int32_t crx_c_pthread_thread_crx_waitForMail2(uint64_t pNanoSeconds__monotonic)
{
	Crx_C_Pthread_Thread vThread = crx_c_pthread_thread_self();
	int32_t vReturn = 0;

	if(vThread != NULL)
	{
		bool tIsTimeRemaining = true;

		vReturn = crx_c_pthread_mutex_crx_timedlock(&(vThread->gPrivate_mutex__mail),
				pNanoSeconds__monotonic);

		if(Return == 0)
		{
			while((vThread->gPrivate_mails == NULL) && 
					(vThread->gPrivate_postponedMails == NULL) &&
					(crx_c_queue_getLength(vThread->gPrivate_mails) == 0) &&
					(crx_c_sortedQueue_getLength(vThread->gPrivate_postponedMails) == 0))
			{
				vReturn = crx_c_pthread_cond_crx_timedwait(
						&(vThread->gPrivate_cond__mail), &((*vThread)->gPrivate_mutex__mail), 
						pNanoSeconds__monotonic);

				if(vReturn == 0)
				{
					vReturn = crx_c_pthread_private_getIsTimeRemaining2(pNanoSeconds__monotonic, 
							&tIsTimeRemaining);

					if(vReturn == 0)
					{
						if(!tIsTimeRemaining)
						{
							vReturn = ETIMEDOUT;

							break;
						}
					}
					else
						{break;}
				}
				else
					{break;}
			}

			crx_c_pthread_mutex_unlock(&(pThis->gPrivate_mutex__mail));
		}
	}
	else
		{vReturn = EINVAL;}

	return vReturn;
}
int32_t crx_c_pthread_thread_crx_sendMail(Crx_C_Pthread_Thread pThread, 
		uint64_t pSenderId, uint64_t pShortData, 
		void * (*pFunc)(void *), void * pMessage)
{
	Crx_C_Pthread_Thread vThread = crx_c_pthread_thread_self();
	int32_t vReturn = 0;

	if(vThread != NULL)
	{
		vReturn = crx_c_pthread_mutex_lock(&(vThread->gPrivate_mutex__mail));

		if(vReturn == 0)
		{
			if(vThread->gPrivate_mails == NULL)
			{
				vThread->gPrivate_mails = crx_c_queue_new(
						crx_c_pthread_crx_private_mail_getTypeBluePrint(), 8);
			}

			if(vThread->gPrivate_mails != NULL)
			{
				Crx_C_Pthread_Crx_Private_Mail tMail;
				
				tMail->gFunc = pFunc;
				tMail->gArguments = pMessage;
				tMail->pSenderId = pFunc;
				tMail->pShortData = pFunc;
				
				if(!crx_c_queue_push(vThread->gPrivate_mails, &tMail))
					{vReturn = ENOMEM;}
			}
			else
				{vReturn = ENOMEM;}

			if(vReturn == 0)
				{crx_c_pthread_cond_signal(&(vThread->gPrivate_cond__mail));}

			crx_c_pthread_mutex_unlock(&(vThread->gPrivate_mutex__mail));
		}
	}

	return vReturn;
}
int32_t crx_c_pthread_thread_crx_sendPostponedMail(Crx_C_Pthread_Thread pThread, 
		uint64_t pNanoSeconds__monotonic, uint64_t pSenderId, uint64_t pShortData,
		void * (* pFunc)(void *), void * pMessage)
{
	Crx_C_Pthread_Thread vThread = crx_c_pthread_thread_self();
	int32_t vReturn = 0;

	if(vThread != NULL)
	{
		vReturn = crx_c_pthread_mutex_lock(&(vThread->gPrivate_mutex__mail));

		if(vReturn == 0)
		{
			if(vThread->gPrivate_postponedMails == NULL)
			{
				vThread->gPrivate_postponedMails = crx_c_sortedQueue_new(
						crx_c_pthread_crx_private_mail_getTypeBluePrint(), 
						crx_c_pthread_crx_private_postponedMail_getOrder,
						false, 4);
			}

			if(vThread->gPrivate_postponedMails != NULL)
			{
				Crx_C_Pthread_Crx_Private_PostponedMail tPostponedMail;

				tPostponedMail->gMail->gFunc = pFunc;
				tPostponedMail->gMail->gArguments = pMessage;
				tPostponedMail->gMail->pSenderId = pFunc;
				tPostponedMail->gMail->pShortData = pFunc;
				tPostponedMail->gDueTime = pNanoSeconds__monotonic;
				tPostponedMail->gSetTime = crx_c_pthread_crx_computeCurrentMonotonicTime();

				if(!crx_c_sortedQueue_push(vThread->gPrivate_postponedMails, &tPostponedMail))
					{vReturn = ENOMEM;}
			}
			else
				{vReturn = ENOMEM;}

			if(vReturn == 0)
				{crx_c_pthread_cond_signal(&(vThread->gPrivate_cond__mail));}

			crx_c_pthread_mutex_unlock(&(vThread->gPrivate_mutex__mail));
		}
	}

	return vReturn;
}

int32_t crx_c_pthread_thread_crx_attachToThreadPool(Crx_C_Pthread_Thread pThread,
		Crx_C_Pthread_Crx_ThreadPool * pThreadPool)
{
	CRX_SCOPE_META
	if(pThread->gPrivate_threadPool != NULL)
		{return EINVAL;}

	CRX_SCOPE
	int32_t vReturn = crx_c_pthread_mutex_lock(&(pThreadPool->gPrivate_threadPool->gPrivate_mutex));

	if(vReturn == 0)
	{
		if(!crx_c_ring_push(&(pThreadPool->gPrivate_threadPool->gPrivate_threads),
				pThread))
			{vReturn = ENOMEM;}
		else
			{pThread->gPrivate_threadPool = pThreadPool->gPrivate_threadPool;}

		crx_c_pthread_mutex_unlock(&(pThreadPool->gPrivate_threadPool->gPrivate_mutex));
	}

	return vReturn;
	CRX_SCOPE_END
}
int32_t crx_c_pthread_thread_crx_detachFromThreadPool(Crx_C_Pthread_Thread pThread)
{
	int32_t vReturn = 0;

	if(pThread->gPrivate_threadPool != NULL)
	{
		vReturn = crx_c_pthread_mutex_lock(&(pThread->gPrivate_threadPool->gPrivate_mutex));

		if((vReturn == 0) && (pThread->gPrivate_threadPool != NULL))
		{
			Crx_C_Pthread_Crx_Private_ThreadPool * tThreadPool = 
					pThread->gPrivate_threadPool;

			pThread->gPrivate_threadPool = NULL;

			CRX_FOR(size_t tI = 0, 
					tI < crx_c_ring_getLength(&(tThreadPool->gPrivate_threads)), 
					tI++)
			{
				if(crx_c_pthread_thread_equal(*((Crx_C_Pthread_Thread *)crx_c_ring_get(
						&(tThreadPool->gPrivate_threads)), 0), pThread))
				{
					crx_c_ring_removeElements(&(tThreadPool->gPrivate_threads), tI, 
							1);

					break;
				}
			}

			pthread_mutex_unlock(&(tThreadPool->gPrivate_mutex));
		}
	}
}
int32_t crx_c_pthread_thread_crx_waitForThreadPoolTaskThenHandle()
{
	Crx_C_Pthread_Thread vThread = crx_c_pthread_thread_self();
	int32_t vReturn = 0;

	if(vThread != NULL)
	{
		vReturn = crx_c_pthread_mutex_lock(&(vThread->gPrivate_threadPool->gPrivate_mutex));

		if(vReturn == 0)
		{
			while((vReturn == 0) && (crx_c_queue_getLength(
					&(vThread->gPrivate_threadPool->gPrivate_threadPoolTasks)) == 0))
			{
				vReturn = crx_c_pthread_cond_wait(&(vThread->gPrivate_threadPool->gPrivate_cond),
						&(vThread->gPrivate_threadPool->gPrivate_mutex));
			}

			if(vReturn == 0)
			{
				Crx_C_Pthread_Crx_Private_ThreadPoolTask tThreadPoolTask = *(crx_c_queue_getFront(
						&(vThread->gPrivate_threadPool->gPrivate_threadPoolTasks)));

				crx_c_queue_popFromFront(
						&(vThread->gPrivate_threadPool->gPrivate_threadPoolTasks));

				crx_c_pthread_mutex_unlock(&(vThread->gPrivate_threadPool->gPrivate_mutex));

				(*(tThreadPoolTask.gFunc))(tThreadPoolTask.gArguments);
			}
			else
				{crx_c_pthread_mutex_unlock(&(vThread->gPrivate_threadPool->gPrivate_mutex));}
		}
	}

	return vReturn;
}
int32_t crx_c_pthread_thread_crx_waitForThreadPoolTaskThenHandle2(uint64_t pNanoSeconds__monotonic)
{
	Crx_C_Pthread_Thread vThread = crx_c_pthread_thread_self();
	int32_t vReturn = 0;

	if(vThread != NULL)
	{
		bool tIsTimeRemaining = true;

		vReturn = crx_c_pthread_mutex_crx_timedlock(&(vThread->gPrivate_threadPool->gPrivate_mutex),
				pNanoSeconds__monotonic);

		if(vReturn == 0)
		{
			while((vReturn == 0) && (crx_c_queue_getLength(
					&(vThread->gPrivate_threadPool->gPrivate_threadPoolTasks)) == 0))
			{
				vReturn = crx_c_pthread_cond_crx_timedwait(
						&(vThread->gPrivate_threadPool->gPrivate_cond),
						&(vThread->gPrivate_threadPool->gPrivate_mutex), 
						pNanoSeconds__monotonic);
						
				if(vReturn == 0)
				{
					vReturn = crx_c_pthread_private_getIsTimeRemaining2(pNanoSeconds__monotonic, 
							&tIsTimeRemaining);

					if(vReturn == 0)
					{
						if(!tIsTimeRemaining)
						{
							vReturn = ETIMEDOUT;

							break;
						}
					}
					else
						{break;}
				}
			}

			if(vReturn == 0)
			{
				Crx_C_Pthread_Crx_Private_ThreadPoolTask tThreadPoolTask = *(crx_c_queue_getFront(
						&(vThread->gPrivate_threadPool->gPrivate_threadPoolTasks)));

				crx_c_queue_popFromFront(
						&(vThread->gPrivate_threadPool->gPrivate_threadPoolTasks));

				crx_c_pthread_mutex_unlock(&(vThread->gPrivate_threadPool->gPrivate_mutex));
				
				(*(tThreadPoolTask.gFunc))(tThreadPoolTask.gArguments);
			}
			else
				{crx_c_pthread_mutex_unlock(&(vThread->gPrivate_threadPool->gPrivate_mutex));}
		}
	}

	return vReturn;
}

#endif

/*----*/

int32_t crx_c_pthread_attr_init(Crx_C_Pthread_Attr * pAttr)
{
	int vReturn = pthread_attr_init(pAttr);

#if(!windows)
	if(vReturn == 0)
	{
		/*
			BESIDE ENSURING DEFAULTS. THIS ALSO SOLVES BUGS IN IMPLEMENTATION SUCH AS GLIBC 2.8
			SOURCE: https://linux.die.net/man/3/pthread_attr_getinheritsched
		*/
		vReturn = crx_c_pthread_attr_setdetachstate(pAttr, PTHREAD_CREATE_JOINABLE);
		
		if(vReturn == 0)
			{vReturn = pthread_attr_setinheritsched(pAttr, PTHREAD_EXPLICIT_SCHED);}
		if(vReturn == 0)
			{pthread_attr_setschedpolicy(attr, SCHED_OTHER);}
		if(vReturn == 0)
		{
			struct sched_param tParam;
			
			memset(&vParam, 0, sizeof(struct sched_param));
			
			tParam.sched_priority = 0;

			vReturn = pthread_attr_setschedparam(pAttr, &tParam);
		}
		if(vReturn == 0)
			{vReturn = pthread_attr_setscope(pAttr, PTHREAD_SCOPE_SYSTEM);}
		if(vReturn == 0)
			{vReturn = crx_c_pthread_attr_setstacksize(pAttr, 1048576);}
	}
#endif

	return vReturn;
}
int32_t crx_c_pthread_attr_destroy(Crx_C_Pthread_Attr * pAttr)
	{return pthread_attr_destroy(pAttr);}

int32_t crx_c_pthread_attr_getdetachstate(Crx_C_Pthread_Attr const * pAttr, int * pDetachState)
	{return pthread_attr_getdetachstate(pAttr, pDetachState);}
int32_t crx_c_pthread_attr_setdetachstate(Crx_C_Pthread_Attr * pAttr, int pDetachState)
	{return pthread_attr_setdetachstate(pAttr, pDetachState);}

int32_t crx_c_pthread_attr_getstacksize(Crx_C_Pthread_Attr const * pAttr, size_t * pStackSize)
	{return pthread_attr_getstacksize(pAttr, pStackSize);}
int32_t crx_c_pthread_attr_setstacksize(Crx_C_Pthread_Attr * attr, size_t pStackSize)
{
	/*POSIX ONLY REQUIRES pStackSize TO BE THE MINIMUM SET*/
	size_t vStackSize = ((pStackSize < PTHREAD_STACK_MIN) ? PTHREAD_STACK_MIN : pStackSize);
	
	if(vStackSize == 0)
		{vStackSize = 1048576;}

	vStackSize = (vStackSize << 12) | (vStackSize & 0xE000);/*CLAMPED TO PAGE SIZE. ASSUMING 
																		4096 BYTES PAGE SIZE*/
	
	if(vStackSize < pStackSize)
		{vStackSize += 4096;}
	
	return pthread_attr_setstacksize(pAttr, pStackSize);
}

/*----*/

/*REFERENCE: https://github.com/isotes/pthread-barrier-macos*/
int32_t crx_c_pthread_barrier_crx_init(Crx_C_Pthread_Barrier * pBarrier, uint32_t pCount)
{
#if(!MACOS)
	pthread_barrierattr_t vBarrierAttr;
	int32_t vReturn = pthread_barrierattr_init(&vBarrierAttr);
	
	if(vReturn == 0)
	{
		vReturn = pthread_barrierattr_setpshared(&vBarrierAttr, PTHREAD_PROCESS_PRIVATE);
		
		if(vReturn == 0)
			{vReturn = pthread_barrier_init(pBarrier, &vBarrierAttr, pCount);}
		
		pthread_barrierattr_destroy(&vBarrierAttr);
	}

	return vReturn;
#else
	if((pBarrier == NULL) || (*pBarrier == NULL) || (pCount == 0))
		{return EINVAL;}
	else
	{
		Crx_C_Pthread_Private_Barrier * tBarrier = calloc(1, sizeof(Crx_C_Pthread_Private_Barrier));
		int tReturn = 0;
		
		if(tBarrier != NULL)
		{
			pthread_mutexattr_t tMutexAttr /*= ?*/;

			tReturn = crx_c_pthread_initializeACrxRtCompliantMutexAttribute(&tMutexAttr);

			if(tReturn == 0)
			{
				tReturn = pthread_mutex_init(&(tBarrier->gPrivate_mutex), &tMutexAttr);
			
				if(tReturn == 0)
				{
					tReturn = crx_c_pthread_cond_crx_init(&(tBarrier->gPrivate_cond));
					
					if(tReturn == 0)
					{
						tBarrier->gPrivate_initialHeight = pCount;
						tBarrier->gPrivate_currentHeight = pCount;
						tBarrier->gPrivate_runId = 0;

						*pBarrier = tBarrier;
					}
					else
					{
						pthread_mutex_destroy(&tBarrier->gPrivate_mutex);
						free(tBarrier);
						tBarrier = NULL;
					}
				}

				pthread_mutexattr_destroy(&tMutexAttr);
			}
			else
			{
				free(tBarrier);
				tBarrier = NULL;
			}
		}
		else
			{tReturn = ENOMEM;}
		
		return tReturn;
	}
#endif
}
int32_t crx_c_pthread_barrier_destroy(Crx_C_Pthread_Barrier * pBarrier)
{
#if(!MACOS)
	return pthread_barrier_destroy(pBarrier);
#else
	if((pBarrier == NULL) || (*pBarrier == NULL))
		{return EINVAL;}
	else
	{
		Crx_C_Pthread_Private_Barrier * tBarrier = *pBarrier;

		if(pthread_mutex_trylock(tBarrier->gPrivate_mutex) != 0)
			{return EBUSY;}
		else
		{
			if(tBarrier->gPrivate_currentHeight < tBarrier->gPrivate_initialHeight)
				{return EBUSY;}
			else
			{	
				int32_t tReturn = 0;
				int32_t tResult = 0;

				*pBarrier = NULL;

				CRX__C__PTHREAD__PRIVATE__WRITE_MEMORY_BARRIER();

				pthread_mutex_unlock(&(tBarrier->gPrivate_mutex));

				tReturn = pthread_mutex_destroy(&(tBarrier->gPrivate_mutex));
				while(tReturn == EBUSY)
				{
					crx_c_pthread_sched_crx_runOtherThreadsInstead();
					tReturn = pthread_mutex_destroy(&(tBarrier->gPrivate_mutex))
				}

				tResult = crx_c_pthread_cond_destroy(&(tBarrier->gPrivate_cond));
				while(tResult == EBUSY)
				{
					crx_c_pthread_sched_crx_runOtherThreadsInstead();
					tResult = crx_c_pthread_cond_destroy(&(tBarrier->gPrivate_cond));
				}

				free(tBarrier);
				tBarrier = NULL;
				
				return ((tReturn == 0) ? tResult : tReturn);
			}
		}
	}
#endif
}
int32_t crx_c_pthread_barrier_wait(Crx_C_Pthread_Barrier * pBarrier)
{
#if(!MACOS)
	return pthread_barrier_wait(pBarrier);
#else
	if((pBarrier == NULL) || (*pBarrier == NULL) || (*pBarrier == NULL))
		{return EINVAL;}
	else
	{
		int32_t tResult = pthread_mutex_lock(&((*pBarrier)->mutex));

		CRX__C__PTHREAD__PRIVATE__READ_MEMORY_BARRIER();

		if(tResult == 0)
		{
			if(*pBarrier != NULL)
			{
				Crx_C_Pthread_Private_Barrier * tBarrier = *pBarrier;

				tBarrier->gPrivate_currentHeight = tBarrier->gPrivate_currentHeight - 1;

				if(tBarrier->gPrivate_currentHeight != 0)
				{
					uint64_t tRunId = tBarrier->gPrivate_runId;
					int32_t tResult2 = 0;

					while((tResult2 == 0) && (tRunId == tBarrier->gPrivate_runId))
					{
						tResult2 = crx_c_pthread_cond_internalUseOnly_wait(&(tBarrier->gPrivate_cond), 
								&(tBarrier->gPrivate_mutex));
					}

					pthread_mutex_unlock(&(tBarrier->gPrivate_mutex));

					return 0;
				}
				else
				{
					tBarrier->gPrivate_runId += 1;
					tBarrier->gPrivate_currentHeight = tBarrier->gPrivate_initialHeight;
					
					crx_c_pthread_cond_broadcast(&(tBarrier->gPrivate_cond));
					pthread_mutex_unlock(&(tBarrier->gPrivate_mutex));

					return PTHREAD_BARRIER_SERIAL_THREAD;
				}
			}
		}
	}
#endif
}

/*----*/
int32_t crx_c_pthread_cond_crx_init(Crx_C_Pthread_Cond * pCond)
{
	pthread_condattr_t vCondAttr;
	int32_t vReturn = 0;

	vReturn = pthread_condattr_init(&vCondAttr);

	if(vReturn == 0)
	{
		vReturn = pthread_condattr_setpshared(&vCondAttr, PTHREAD_PROCESS_PRIVATE);

#if(FREEBSD LINUX)
		if(vReturn == 0)
			{vReturn = pthread_condattr_setclock(&vCondAttr, CLOCK_MONOTONIC);}
#endif

		if(vReturn == 0)
			{vReturn = pthread_cond_crx_init(pCond, &vCondAttr);}

		pthread_condattr_destroy(&vCondAttr);
	}

	return vReturn;
}
int32_t crx_c_pthread_cond_destroy(Crx_C_Pthread_Cond * pCond)
	{return pthread_cond_destroy(pCond);}
int32_t crx_c_pthread_cond_wait(Crx_C_Pthread_Cond * pCond, Crx_C_Pthread_Mutex * pMutex)
	{return pthread_cond_wait(pCond, &(pMutex->gPrivate_mutex));}
int32_t crx_c_pthread_cond_internalUseOnly_wait(Crx_C_Pthread_Cond * pCond, pthread_mutex_t * pMutex)
	{return pthread_cond_wait(pCond, pMutex);}
int32_t crx_c_pthread_cond_crx_timedwait(Crx_C_Pthread_Cond * pCond, Crx_C_Pthread_Mutex * pMutex,
		uint64_t pNanoSeconds__monotonic)
{
	return crx_c_pthread_cond_crx_internalUseOnly_timedwait(pCond, &(pMutex->gPrivate_mutex),
			pNanoSeconds__monotonic);
}
int32_t crx_c_pthread_cond_crx_internalUseOnly_timedwait(Crx_C_Pthread_Cond * pCond, 
		pthread_mutex_t * pMutex, uint64_t pNanoSeconds__monotonic)
{
#if(FREEBSD LINUX)
	/*WARNING: CLOCK_MONOTONIC IS ASSUMED IN THIS CASE*/
	struct timespec tTimespec /*= ?*/
	
	crx_c_pthread_crx_getTimespecFromNanoseconds(&tTimespec, pNanoSeconds__monotonic);

	return pthread_cond_timedwait(pCond, &(pMutex->gPrivate_mutex), &tTimespec);
#elif(MACOS IOS WINDOWS)
	uint64_t vNanoSeconds = 0;
	int32_t vReturn = crx_c_pthread_crx_computeCurrentMonotonicTime(&vNanoSeconds);

	if(vReturn == 0)
    {
		struct timespec tTimespec__diff;
		bool tIsError = false;

		if(pNanoSeconds__monotonic > vNanoSeconds)
		{
			crx_c_pthread_crx_getTimespecFromNanoseconds(&tTimespec__diff,
					pNanoSeconds__monotonic - vNanoSeconds);
		}
		else
			{tIsError = true;}

		if(tIsError)
		{
			tTimespec__diff.tv_sec = 0;
			tTimespec__diff.tv_nsec = 1;
		}
	
		vReturn = pthread_cond_timedwait_relative_np(pCond, &(pMutex->gPrivate_mutex), 
				&tTimespec__diff);
	}
	else
		{vReturn = errno;}
	/*
		NOTE: THERE IS NOT POINT USNG clock_get_time BECAUSE WITH CALENDAR_CLOCK
				THE RESOLUTION ENDS UP THE SAME AS THAT OF gettimeofday(), 1 MICRO SECOND.
				REMEMBER THAT THERE IS NO clock_gettime() ON MACOS AND WE WANT
				clock_gettime(CLOCK_REALTIME, ...)
		NOTE: THIS MUST GET THE EQUIVILANT TIME TO "CLOCK_REALTIME"
		
		REMEMBER THAT MACOS THERE IS NO WAY TO USE CLOCK_MONOTONIC FOR WAITING FUNCTIONS WITH 
		TIMEOUT, AND THEREFORE WE ARE BOUND TO THE DEFAULT "CLOCK_REALTIME" THAT CAN NOT BE
		CHANGED. THE CODE BELOW IS TO SOLVE THE PROBLEMS OF USING CLOCK_REALTIME.
		
		UPDATE: THE FOLLOWING HAD A DESIGN BUG. WHILE THE DESIGN ACCOUNTED FOR CHANGES TO CLOCK
				WHILE THE WAIT IS HAPPENING, IT DID NOT ACCOUNT FOR CHANGES TO CLOCK WHILE THE
				DELTA TIME IS CALCULATED. IN OTHER WORDS, BY THE TIME gettimeofday() IS CALLED 
				BELOW, THE CLOCK MIGHT HAVE CHANGED AGAIN, BUT AFTER THE USER GOT pTimespec ABOVE
				AND PASSED IT.
				THE FIX WAS MANDATING ALL NORMALIZING FUNCTION TO USE MONOTONIC TIME FOR THE
				TIME PARAMTERS.
	* /
	struct timeval vTimeval;
	struct timespec vTimespec;
	int32_t vReturn = gettimeofday(&vTimeval, NULL);

	if(vReturn == 0)
    {
		struct timespec tTimespec;
		struct timespec tTimespec__diff;
		bool tIsError = false;

		tTimespec.tv_sec = vTimeval.tv_sec;
		tTimespec.tv_nsec = vTimeval.tv_usec * 1000;

		if(tTimespec.tv_sec > pTimespec.tv_sec)
		{
			tTimespec__diff.tv_sec = tTimespec.tv_sec - pTimespec.tv_sec;
			
			if(tTimespec.tv_nsec < pTimespec.tv_nsec)
			{
				tTimespec__diff.tv_nsec = ((uint32_t)(tTimespec.tv_nsec)) + 1000000000 - 
						((uint32_t)(pTimespec.tv_sec));
				tTimespec__diff.tv_sec = tTimespec__diff.tv_sec - 1;
			}
			else
				{tTimespec__diff.tv_nsec = tTimespec.tv_nsec - pTimespec.tv_nsec;}
		}
		else if(tTimespec.tv_sec == pTimespec.tv_sec)
		{
			if(tTimespec.tv_nsec >= pTimespec.tv_nsec)
				{tTimespec__diff.tv_nsec = tTimespec.tv_nsec - pTimespec.tv_nsec;}
			else
				{tIsError = true;}
		}
		else
			{tIsError = true;}

		if(tIsError)
		{
			tTimespec__diff.tv_sec = 0;
			tTimespec__diff.tv_nsec = 100;
		}
	
		vReturn = pthread_cond_timedwait_relative_np(pCond, pMutex, &tTimespec__diff);
	}
	else
		{vReturn = errno;}*/

	return vReturn;
#elif(ANDROID)
	/*
		NEED TO DO RUN TIME CHECK, AND IF ANDROID >=5, RELY ON pthread_condattr_setclock(),
		OTHERWISE USE pthread_cond_timedwait_monotonic_np().
	*/
#endif
}

int32_t crx_c_pthread_cond_signal(Crx_C_Pthread_Cond * pCond)
	{return pthread_cond_signal(pCond);}
int32_t crx_c_pthread_cond_broadcast(Crx_C_Pthread_Cond * pCond)
	{return pthread_cond_broadcast(pCond);}

/*----*/
int32_t crx_c_pthread_mutex_init(Crx_C_Pthread_Mutex * pMutex, 
		Crx_C_Pthread_MutexAttr const * pMutexAttr)
{
	if(pMutexAttr == NULL)
	{
		pthread_mutexattr_t tMutexAttr /*= ?*/;
		int32_t tReturn = crx_c_pthread_initializeACrxRtCompliantMutexAttribute(&tMutexAttr);

		if(tReturn == 0)
		{
			tReturn = crx_c_pthread_mutex_init__do(pMutex, &tMutexAttr);

			pthread_mutexattr_destroy(&tMutexAttr);
		}

		return tReturn;
	}
	else
		{return crx_c_pthread_mutex_init__do(pMutex, pMutexAttr);}
}
int32_t crx_c_pthread_mutex_init__do(Crx_C_Pthread_Mutex * pMutex, 
		pthread_mutexattr_t const * CRX_NOT_NULL pMutexAttr)
{
	CRX_SCOPE_META
	/*if((pMutex == NULL) || (*pMutex != NULL))*/
	if(pMutex == NULL)
		{return EINVAL;}

	CRX_SCOPE
	Crx_C_Pthread_Private_Mutex * vMutex = calloc(1, sizeof(Crx_C_Pthread_Private_Mutex));
	int32_t vReturn = 0;

	if(vMutex != NULL)
	{
		vReturn = pthread_mutex_init(&(vMutex->gPrivate_mutex), pMutexAttr);

		if(vReturn == 0)
		{
			vReturn = pthread_mutex_init(&(vMutex->gPrivate_mutex__2), pMutexAttr);

			if(vReturn == 0)
			{
				vReturn = crx_c_pthread_cond_crx_init(&(vMutex->gPrivate_cond));

				if(vReturn == 0)
					{*pMutex = vMutex;}
				else
				{
					pthread_mutex_destroy(&(vMutex->gPrivate_mutex__2));
					pthread_mutex_destroy(&(vMutex->gPrivate_mutex));
					free(vMutex);
					vMutex = NULL;
				}
			}
			else
			{
				pthread_mutex_destroy(&(vMutex->gPrivate_mutex));
				free(vMutex);
				vMutex = NULL;
			}
		}
		else
		{
			free(vMutex);
			vMutex = NULL;
		}
	}
	else
		{vReturn = ENOMEM;}

	return vReturn;
	CRX_SCOPE_END
}
int32_t crx_c_pthread_mutex_destroy(Crx_C_Pthread_Mutex * pMutex)
{
	/*
		WARNING: THE FOLLOWING TRUSTS pthread_mutex_trylock(), pthread_mutex_lock() AND
		pthread_mutex_unlock() TO RETURN EINVAL WHEN THE MUTEX HAS BEEN DESTROYED
		BY THE TIME THEY GET CALLED, TO BE ROBUST. OTHERWISE, EXPLICITLY MAKE SURE THAT
		NO ONE IS USING THE MUTEX BEFORE CALLING THIS FUNCTION.
		
		TODO: USE THE APPROACH WITH THE BARRIER. DID NOT USE THAT APPROACH HERE BECAUSE
				OF ITS USE OF BARRIERS, AND WHILE THAT CODE IS FOR MACOS ONLY, AND THEREFORE
				DEFINING THE BARRIER MACROS IS MANAGABLE, THE CODE HERE IS FOR EVERY SYSTEM 
				EXCEPT WINDOWS.
	*/
	if((pMutex == NULL) || (*pMutex == NULL))
		{return EINVAL;}

	if(pMutex != CRX__C__PTHREAD__PTHREAD_MUTEX_INITIALIZER)
	{
		int32_t tReturn = pthread_mutex_destroy(&((*pMutex)->gPrivate_mutex));
		int32_t tResult = 0;
		int32_t tResult2 = 0;

		if(tReturn != EBUSY)
		{
			tResult = pthread_mutex_destroy(&((*pMutex)->gPrivate_mutex__2));
			while(tResult == EBUSY)
			{
				crx_c_pthread_sched_crx_runOtherThreadsInstead();

				tResult = pthread_mutex_destroy(&((*pMutex)->gPrivate_mutex__2));
			}

			tResult2 = crx_c_pthread_cond_destroy(&((*pMutex)->gPrivate_cond));
			while(tResult2 == EBUSY)
			{
				crx_c_pthread_sched_crx_runOtherThreadsInstead();

				tResult2 = crx_c_pthread_cond_destroy(&((*pMutex)->gPrivate_cond));
			}

			free(*pMutex);
			*pMutex = NULL;
		}

		return ((tReturn == 0) ? ((tResult == 0) ? tResult2 : tResult) : tReturn);
	}
	else
		{*pMutex = NULL;}
}

int32_t crx_c_pthread_mutex_lock(Crx_C_Pthread_Mutex * pMutex)
{
	if((pMutex == NULL) || (*pMutex == NULL) ||
			(crx_c_pthread_mutex_private_initializeIfStatic(pMutex) != 0))
		{return EINVAL;}

	return pthread_mutex_lock(&((*pMutex)->gPrivate_mutex));
}
int32_t crx_c_pthread_mutex_trylock(Crx_C_Pthread_Mutex * pMutex);
{
	if((pMutex == NULL) || (*pMutex == NULL) ||
			(crx_c_pthread_mutex_private_initializeIfStatic(pMutex) != 0))
		{return EINVAL;}

	return pthread_mutex_trylock(&((*pMutex)->gPrivate_mutex));
}
int32_t crx_c_pthread_mutex_crx_timedlock(Crx_C_Pthread_Mutex * pMutex, 
		uint64_t pNanoSeconds__monotonic)
{
	/*THE POSIX WIN32 LIBRARY HAS THE FUNCTION BUT IS NOT USING CLOCK_MONOTONIC*/
	CRX_SCOPE_META
	if((pMutex == NULL) || (*pMutex == NULL) || 
			(crx_c_pthread_mutex_private_initializeIfStatic(pMutex) != 0))
		{return EINVAL;}

	CRX_SCOPE
	int32_t vReturn = pthread_mutex_lock(&((*pMutex)->gPrivate_mutex__2));

	if(vReturn == 0)
	{
		bool tIsTimeRemaining = true;

		vReturn = pthread_mutex_trylock(&((*pMutex)->gPrivate_mutex));

		while((vReturn != 0) && (vReturn != EINVAL))
		{
			vReturn = crx_c_pthread_cond_crx_internalUseOnly_timedwait(&((*pMutex)->gPrivate_cond), 
					&((*pMutex)->gPrivate_mutex__2), pNanoSeconds__monotonic);

			if(vReturn == 0)
			{
				vReturn = crx_c_pthread_private_getIsTimeRemaining2(pNanoSeconds__monotonic, 
						&tIsTimeRemaining);

				if(vReturn == 0)
				{
					if(tIsTimeRemaining)
						{vReturn = pthread_mutex_trylock(&((*pMutex)->gPrivate_mutex));}
					else
					{
						vReturn = ETIMEDOUT;

						break;
					}
				}
				else
					{break;}
			}
			else
				{break;}
		}

		pthread_mutex_unlock(&((*pMutex)->gPrivate_mutex__2));
	}

	return vReturn;
	CRX_SCOPE_END
}
int32_t crx_c_pthread_mutex_unlock(Crx_C_Pthread_Mutex * pMutex);
{
	CRX_SCOPE_META
	if((pMutex == NULL) || (*pMutex == NULL))
		{return EINVAL;}
	else if(pMutex == CRX__C__PTHREAD__PTHREAD_MUTEX_INITIALIZER)
		{return 0;}

	CRX_SCOPE
	/*
		WE SHOULD NOT NEED TO LOCK Crx_C_Pthread_Mutex::gPrivate_mutex__2 BEFORE CALLING
		pthread_cond_signal() WHEN WE ARE EXPECTING ONLY ONE TO CALL pthread_cond_signal(). 
		HOWEVER, KEEPING IT SAFE IN CASE THIS VERY FUNCTION GETS CALLED BY MULTIPLE THREADS.
	*/
	int32_t vReturn = pthread_mutex_lock(&((*pMutex)->gPrivate_mutex__2));

	if(vReturn == 0)
	{
		vReturn = pthread_mutex_unlock(&((*pMutex)->gPrivate_mutex));

		if(vReturn == 0)
			{crx_c_pthread_cond_signal(&((*pMutex)->gPrivate_cond));}
	}
	
	pthread_mutex_unlock(&((*pMutex)->gPrivate_mutex__2));

	return vReturn;
	CRX_SCOPE_END
}
int32_t crx_c_pthread_mutex_private_initializeIfStatic(Crx_C_Pthread_Mutex * pMutex)
{
	if(*pMutex == CRX__C__PTHREAD__PTHREAD_MUTEX_INITIALIZER)
	{
		int32_t tReturn = pthread_mutex_lock(&gCrx_c_pthread_private_intializer_mutex)

		if(tReturn == 0)
		{
			 tReturn = crx_c_pthread_mutex_init(pMutex, NULL);
			 
			 pthread_mutex_unlock(&gCrx_c_pthread_private_intializer_mutex);
		}

		return tReturn;
	}
	else if(*pMutex == NULL)
		{return EINVAL;}
	else
		{return 0;}
}

/*----*/
int32_t crx_c_pthread_mutexattr_init(Crx_C_Pthread_MutexAttr * pMutexAttr)
{
	int vReturn = pthread_mutexattr_init(pMutexAttr);

	if(vReturn == 0)
	{
		/*IT IS ASSUMED THAT BY DEFAULT SHARE MODE IS PTHREAD_PROCESS_PRIVATE*/
#if((defined(__FreeBSD__) && __FreeBSD__ >= 11) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS) || \
		(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__LINUX))
		vReturn = pthread_mutexattr_setrobust(pMutexAttr, PTHREAD_MUTEX_STALLED);
#endif

		if(vReturn == 0)
			{vReturn = pthread_mutexattr_settype(pMutexAttr, PTHREAD_MUTEX_NORMAL);}
		if(vReturn != 0)
			{pthread_mutexattr_destroy(pMutexAttr);}
	}

	return vReturn;
}
int32_t crx_c_pthread_mutexattr_destroy(Crx_C_Pthread_MutexAttr * pMutexAttr)
	{return pthread_mutexattr_destroy(pMutexAttr);}

int32_t crx_c_pthread_mutexattr_gettype(Crx_C_Pthread_MutexAttr * pMutexAttr, int32_t * pKind)
{
	int vKind = *pKind;
	int32_t vReturn = pthread_mutexattr_gettype(pMutexAttr, &vKind);

	*pKind = vKind;

	return vReturn;
}
int32_t crx_c_pthread_mutexattr_settype(Crx_C_Pthread_MutexAttr * attr, int32_t * pKind)
	{return pthread_mutexattr_settype(pMutexAttr, *pKind);}

/*----*/
int32_t crx_c_pthread_key_create(Crx_C_Pthread_Key * pKey, void (* pFunc) (void *))
{
	int32_t vReturn = 0;
	Crx_C_Pthread_Private_Key * vKey = NULL;
	
	if((vKey = (Crx_C_Pthread_Private_Key *)calloc (1, sizeof(Crx_C_Pthread_Private_Key))) != NULL)
	{
		vKey->gPrivate_crx_key = 0;
		vKey->gPrivate_isExtended = false;
	}
	else
		{vReturn = ENOMEM;}
	
	if(vReturn == 0)
	{
		if(pFunc == NULL)
		{
			vReturn = pthread_mutex_lock(&gCrx_c_pthread_private_tls_mutex);

			if(vReturn == 0)
			{
				uint32_t tLargestKey = gCrx_c_pthread_private_tls_largestKey;
				bool tIsFound = false;

				CRX_FOR(size_t tI = tLargestKey, tI < CRX__C__PTHREAD__PRIVATE__EXTENDEDTLS_SIZE, 
						tI++)
				{
					if(!gCrx_c_pthread_private_tls_tlsSlots[tI])
					{
						gCrx_c_pthread_private_tls_tlsSlots[tI] = true;

						vKey->gPrivate_crx_key = tI;
						gCrx_c_pthread_private_tls_largestKey = tI;
						tIsFound = true;

						break;
					}
				}
				CRX_ENDFOR

				if(!tIsFound)
				{
					CRX_FOR(size_t tI = 0, tI < tLargestKey, tI++)
					{
						if(!gCrx_c_pthread_private_tls_tlsSlots[tI])
						{
							gCrx_c_pthread_private_tls_tlsSlots[tI] = true;

							vKey->gPrivate_crx_key = tI;
							gCrx_c_pthread_private_tls_largestKey = tI;
							tIsFound = true;

							break;
						}
					} 
					CRX_ENDFOR
				}

				if(tIsFound == 1)
					{vKey->gPrivate_isExtended = true;}
				else
					{vReturn = EAGAIN;}

				pthread_mutex_unlock(&gCrx_c_pthread_private_tls_mutex);
			}
			
			if(vReturn != 0)
			{
				free(vKey);
				vKey = NULL;
			}
		}
		else
		{
			vReturn = pthread_key_create(&(vKey->gPrivate_Key), pFunc);
			
			if(vReturn != 0)
			{
				free(vKey);
				vKey = NULL;
			}
		}
	}
	
	*pKey = vKey;

	return vReturn;
}
int32_t crx_c_pthread_key_delete(Crx_C_Pthread_Key pKey)
{
	if(pKey == NULL)
		{return 0;}

	if(pFunc == NULL)
	{
		int32_t tReturn = pthread_mutex_lock(&gCrx_c_pthread_private_tls_mutex);

		if(tReturn == 0)
		{
			gCrx_c_pthread_private_tls_tlsSlots[tI] = false;

			free(pKey);

			pthread_mutex_unlock(&gCrx_c_pthread_private_tls_mutex);
		}

		return tReturn;
	}
	else
		{return pthread_key_delete(pKey);}
}

/*----*/	
int32_t crx_c_pthread_rwlock_crx_init(Crx_C_Pthread_RwLock * pRwLock)
{
	pthread_rwlockattr_t vRwlockattr /*= ?*/;
	int32_t vReturn = pthread_rwlockattr_init(&vRwlockattr);

	if(vReturn == 0)
	{
		vReturn = pthread_rwlockattr_setpshared(&vRwlockattr, PTHREAD_PROCESS_PRIVATE);
		
		if(vReturn == 0)
		{
			Crx_C_Pthread_Private_RwLock * tRwLock = calloc(1, 
					sizeof(Crx_C_Pthread_Private_RwLock));

			if(tRwLock != NULL)
			{
				vReturn = pthread_rwlock_init(&(tRwLock->gPrivate_rwlock), &vRwlockattr);
				
				if(vReturn == 0)
				{
					pthread_mutexattr_t tMutexAttr /*= ?*/;

					tReturn = crx_c_pthread_initializeACrxRtCompliantMutexAttribute(&tMutexAttr);

					if(vReturn == 0)
					{
						vReturn = pthread_mutex_init(&(tRwLock->gPrivate_mutex), &tMutexAttr);

						if(vReturn == 0)
						{
							vReturn = crx_c_pthread_cond_crx_init(
									&(tRwLock->gPrivate_cond__reader));

							if(vReturn == 0)
							{
								vReturn = crx_c_pthread_cond_crx_init(
										&(tRwLock->gPrivate_cond__writer));

								if(vReturn == 0)
									{*pRwLock = tRwLock;}
								else
								{
									crx_c_pthread_cond_destroy(&(tRwLock->gPrivate_cond__reader));
									pthread_mutex_destroy(&(tRwLock->gPrivate_mutex));
									pthread_rwlock_destroy(&(tRwLock->gPrivate_rwlock));
									free(tRwLock);
									tRwLock = NULL;
								}
							}
							else
							{
								pthread_mutex_destroy(&(tRwLock->gPrivate_mutex));
								pthread_rwlock_destroy(&(tRwLock->gPrivate_rwlock));
								free(tRwLock);
								tRwLock = NULL;
							}
						}
						else
						{
							pthread_rwlock_destroy(&(tRwLock->gPrivate_rwlock));
							free(tRwLock);
							tRwLock = NULL;
						}

						pthread_mutexattr_destroy(&tMutexAttr);
					}
					else
					{
						pthread_rwlock_destroy(&(tRwLock->gPrivate_rwlock));
						free(tRwLock);
						tRwLock = NULL;
					}
				}
				else
				{
					free(tRwLock);
					tRwLock = NULL;
				}
			}
			else
				{vReturn = ENOMEM;}
		}

		pthread_rwlockattr_destroy(&vRwlockattr);
	}

	return vReturn;
}
int32_t crx_c_pthread_rwlock_destroy(Crx_C_Pthread_RwLock * pRwLock)
{
	int32_t vReturn = pthread_rwlock_destroy(&((*pRwLock)->gPrivate_rwlock));
	int32_t vResult = 0;
	int32_t vResult2 = 0;
	int32_t vResult3 = 0;

	if(vReturn == EBUSY)
		{vReturn = EBUSY;}
	else if(vReturn == 0)
	{
		vResult = pthread_mutex_destroy(&((*pRwLock)->gPrivate_mutex));
		while(vResult == EBUSY)
		{
			crx_c_pthread_sched_crx_runOtherThreadsInstead();

			vResult = pthread_mutex_destroy(&((*pRwLock)->gPrivate_mutex));
		}

		vResult2 = crx_c_pthread_cond_destroy(&((*pRwLock)->gPrivate_cond__writer));
		while(vResult2 == EBUSY)
		{
			crx_c_pthread_sched_crx_runOtherThreadsInstead();

			vResult2 = crx_c_pthread_cond_destroy(&((*pRwLock)->gPrivate_cond__writer));
		}

		vResult3 = crx_c_pthread_cond_destroy(&((*pRwLock)->gPrivate_cond__reader));
		while(vResult3 == EBUSY)
		{
			crx_c_pthread_sched_crx_runOtherThreadsInstead();

			vResult3 = crx_c_pthread_cond_destroy(&((*pRwLock)->gPrivate_cond__reader));
		}

		free(*pRwLock);
		*pRwLock = NULL;
	}

	return ((vReturn == 0) ? ((vResult == 0) ? ((vResult2 == 0) ? vResult3 : vResult2) : vResult) : 
			vReturn);
}

int32_t crx_c_pthread_rwlock_rdlock(Crx_C_Pthread_RwLock * pRwLock)
{
	if((pRwLock == NULL) || (*pRwLock == NULL))
		{return EINVAL;}

	return pthread_rwlock_rdlock(&((*pRwLock)->gPrivate_rwlock));
}
int32_t crx_c_pthread_rwlock_tryrdlock(Crx_C_Pthread_RwLock * pRwLock);
{
	if((pRwLock == NULL) || (*pRwLock == NULL))
		{return EINVAL;}

	return pthread_rwlock_tryrdlock(&((*pRwLock)->gPrivate_rwlock));
}
int32_t crx_c_pthread_rwlock_crx_timedrdlock(Crx_C_Pthread_RwLock * pRwLock,
		uint64_t pNanoSeconds__monotonic)
{
	/*
		THE POSIX WIN32 LIBRARY HAS THE FUNCTION BUT IS NOT USING CLOCK_MONOTONIC

		WARNING: THE FOLLOWING TRUSTS pthread_mutex_trylock(), pthread_mutex_lock() AND
		pthread_mutex_unlock(), pthread_rwlock_tryrdlock() TO RETURN EINVAL WHEN THE MUTEX HAS BEEN 
		DESTROYED BY THE TIME THEY GET CALLED, TO BE ROBUST. OTHERWISE, EXPLICITLY MAKE SURE THAT
		NO ONE IS USING THE MUTEX BEFORE CALLING THIS FUNCTION.
		
		TODO: USE THE APPROACH WITH THE POSIX BARRIER. DID NOT USE THAT APPROACH HERE BECAUSE
				OF ITS USE OF MEMORY BARRIERS, AND WHILE THAT CODE IS FOR MACOS ONLY, AND THEREFORE
				DEFINING THE MEMORY BARRIER MACROS IS MANAGABLE, THE CODE HERE IS FOR EVERY SYSTEM.
	*/
	CRX_SCOPE_META
	if((pRwLock == NULL) || (*pRwLock == NULL))
		{return EINVAL;}

	CRX_SCOPE
	int32_t vReturn = pthread_mutex_lock(&((*pRwLock)->gPrivate_mutex));

	if(vReturn == 0)
	{
		bool tIsTimeRemaining = true;

		vReturn = pthread_rwlock_tryrdlock(&((*pRwLock)->gPrivate_rwlock));

		while((vReturn != 0) && (vReturn != EINVAL))
		{
			vReturn = crx_c_pthread_cond_crx_internalUseOnly_timedwait(
					(*pRwLock)->gPrivate_cond__reader, &((*pRwLock)->gPrivate_mutex), 
					pNanoSeconds__monotonic);

			if(vReturn == 0)
			{
				vReturn = crx_c_pthread_private_getIsTimeRemaining2(pNanoSeconds__monotonic, 
						&tIsTimeRemaining);

				if(vReturn == 0)
				{
					if(tIsTimeRemaining)
						{vReturn = pthread_rwlock_tryrdlock(&((*pRwLock)->gPrivate_rwlock));}
					else
					{
						vReturn = ETIMEDOUT;

						break;
					}
					
				}
				else
					{break;}
			}
			else
				{break;}
		}

		pthread_mutex_unlock(&((*pRwLock)->gPrivate_mutex));
	}

	return vReturn;
	CRX_SCOPE_END
}

int32_t crx_c_pthread_rwlock_wrlock(Crx_C_Pthread_RwLock * pRwLock)
{
	if((pRwLock == NULL) || (*pRwLock == NULL))
		{return EINVAL;}

	return pthread_rwlock_wrlock(&((*pRwLock)->gPrivate_rwlock));
}
int32_t crx_c_pthread_rwlock_trywrlock(Crx_C_Pthread_RwLock * pRwLock)
{
	if((pRwLock == NULL) || (*pRwLock == NULL))
		{return EINVAL;}

	return pthread_rwlock_trywrlock(&((*pRwLock)->gPrivate_rwlock));
}
int32_t crx_c_pthread_rwlock_crx_timedwrlock(Crx_C_Pthread_RwLock * pRwLock,
		uint64_t pNanoSeconds__monotonic)
{
	/*
		WARNING: THE FOLLOWING TRUSTS pthread_mutex_trylock(), pthread_mutex_lock() AND
		pthread_mutex_unlock(), pthread_rwlock_tryrdlock() TO RETURN EINVAL WHEN THE MUTEX HAS BEEN 
		DESTROYED BY THE TIME THEY GET CALLED, TO BE ROBUST. OTHERWISE, EXPLICITLY MAKE SURE THAT
		NO ONE IS USING THE MUTEX BEFORE CALLING THIS FUNCTION.
		
		TODO: USE THE APPROACH WITH THE POSIX BARRIER. DID NOT USE THAT APPROACH HERE BECAUSE
				OF ITS USE OF MEMORY BARRIERS, AND WHILE THAT CODE IS FOR MACOS ONLY, AND THEREFORE
				DEFINING THE MEMORY BARRIER MACROS IS MANAGABLE, THE CODE HERE IS FOR EVERY SYSTEM 
				EXCEPT WINDOWS.
	*/
	CRX_SCOPE_META
	if((pRwLock == NULL) || (*pRwLock == NULL))
		{return EINVAL;}

	CRX_SCOPE
	int32_t vReturn = pthread_mutex_lock(&((*pRwLock)->gPrivate_mutex));

	if(vReturn == 0)
	{
		bool tIsTimeRemaining = true;
		vReturn = pthread_rwlock_trywrlock(&((*pRwLock)->gPrivate_rwlock));

		while((vReturn != 0) && (vReturn != EINVAL))
		{
			vReturn = crx_c_pthread_cond_crx_internalUseOnly_timedwait(
					(*pRwLock)->gPrivate_cond__writer, &((*pRwLock)->gPrivate_mutex), 
					pNanoSeconds__monotonic);

			if(vReturn == 0)
			{
				vReturn = crx_c_pthread_private_getIsTimeRemaining2(pNanoSeconds__monotonic, 
						&tIsTimeRemaining);

				if(vReturn == 0)
				{
					if(tIsTimeRemaining)
						{vReturn = pthread_rwlock_trywrlock(&((*pRwLock)->gPrivate_rwlock));}
					else
					{
						vReturn = ETIMEDOUT;

						break;
					}
				}
				else
					{break;}
			}
			else
				{break;}
		}

		pthread_mutex_unlock(&((*pRwLock)->gPrivate_mutex));
	}

	return vReturn;
	CRX_SCOPE_END
}

int32_t crx_c_pthread_rwlock_unlock(Crx_C_Pthread_RwLock * pRwLock)
{
	CRX_SCOPE_META
	if((pRwLock == NULL) || (*pRwLock == NULL))
		{return EINVAL;}

	CRX_SCOPE
	int32_t vReturn = pthread_mutex_lock(&((*pRwLock)->gPrivate_mutex));

	if(vReturn == 0)
	{
		vReturn = pthread_rwlock_unlock(&((*pRwLock)->gPrivate_rwlock));

		if(vReturn == 0)
		{
			crx_c_pthread_cond_signal(&((*pRwLock)->gPrivate_cond__writer));
			crx_c_pthread_cond_broadcast(&((*pRwLock)->gPrivate_cond__reader));
		}
	}

	pthread_mutex_unlock(&((*pRwLock)->gPrivate_mutex));

	return vReturn;
	CRX_SCOPE_END
}

	
/*----*/

/*
	REFERENCE: https://git.openrobots.org/issues/115

	IF MACOS EVER REMOVES THE DEPRECATED OSSpinLock, FALL BACK TO USING PTHREAD MUTEX, LIKE IN
	THE IOS CASE.
*/
int32_t crx_c_pthread_spin_crx_init(Crx_C_Pthread_SpinLock * pSpinLock)
{
#if(!MACOS && !IOS)
	return pthread_spin_crx_init(pSpinLock, PTHREAD_PROCESSES_PRIVATE);
#elif(MACOS)
	/*__asm__ __volatile__ ("" ::: "memory"); /* ORIGINAL COMMENT: this does not hurt to keep it*/
    *pSpinLock = 0;

    return 0;
#elif(IOS)
	pthread_mutexattr_t vMutexAttr /*= ?*/;
	int32_t vReturn = crx_c_pthread_initializeACrxRtCompliantMutexAttribute(&vMutexAttr);

	if(tReturn == 0)
		{tReturn = pthread_mutex_init(pSpinLock, &vMutexAttr);}

	pthread_mutexattr_destroy(&vMutexAttr);

	return tReturn;
#endif
}
int32_t crx_c_pthread_spin_destroy(Crx_C_Pthread_SpinLock * pSpinLock)
{
#if(!MACOS && !IOS)
	return pthread_spin_destroy(pSpinLock);
#elif(MACOS)
    return 0;
#elif(IOS)
	return pthread_mutex_destroy(pSpinLock);
#endif
}

int32_t crx_c_pthread_spin_lock(Crx_C_Pthread_SpinLock * pSpinLock)
{
#if(!MACOS && !IOS)
	return pthread_spin_lock(pSpinLock);
#elif(MACOS)
	OSSpinLockLock(pSpinLock);
	
	/*
	OR
	void (*vFunc__os_unfair_lock_lock)(void *) = dlsym(dlopen(NULL, RTLD_NOW | RTLD_GLOBAL), 
			"os_unfair_lock_lock");

	if(vFunc__os_unfair_lock_lock != NULL)
		{(*vFunc__os_unfair_lock_lock)(pSpinLock);}
	else
		{OSSpinLockLock(pSpinLock);} NOTICE HOW WE CAN USE THE SAME TYPE, HOWEVER, HACKY.
	*/

    return 0;
#elif(IOS)
	return pthread_mutex_lock(pSpinLock);
#endif
}
int32_t crx_c_pthread_spin_trylock(Crx_C_Pthread_SpinLock * pSpinLock)
{
#if(!MACOS && !IOS)
	return pthread_spin_trylock(pSpinLock);
#elif(MACOS)
	return (OSSpinLockTry(pSpinLock) ? 0 : EBUSY);
	
	/*
	OR
	void (*vFunc__os_unfair_lock_trylock)(void *) = dlsym(dlopen(NULL, RTLD_NOW | RTLD_GLOBAL), 
			"os_unfair_lock_trylock");

	if(vFunc__os_unfair_lock_trylock != NULL)
		{return ((*vFunc__os_unfair_lock_trylock)(pSpinLock) ? 0 : EBUSY);}
	else
		{return (OSSpinLockTry(pSpinLock) ? 0 : EBUSY);} NOTICE HOW WE CAN USE THE SAME TYPE, HOWEVER, HACKY.
	*/
#elif(IOS)
	return pthread_mutex_trylock(pSpinLock);
#endif
}
int32_t crx_c_pthread_spin_unlock(Crx_C_Pthread_SpinLock * pSpinLock)
{
#if(!MACOS && !IOS)
	return pthread_spin_unlock(pSpinLock);
#elif(MACOS)
	OSSpinLockUnlock(pSpinLock);
	
	/*
	OR
	void (*vFunc__os_unfair_lock_unlock)(void *) = dlsym(dlopen(NULL, RTLD_NOW | RTLD_GLOBAL), 
			"os_unfair_lock_unlock");

	if(vFunc__os_unfair_lock_unlock != NULL)
		{(*vFunc__os_unfair_lock_unlock)(pSpinLock);}
	else
		{(OSSpinLockUnlock(pSpinLock);} NOTICE HOW WE CAN USE THE SAME TYPE, HOWEVER, HACKY.
	*/

    return 0;
#elif(IOS)
	return pthread_mutex_trylock(pSpinLock);
#endif
}


/*----*/

/*int32_t crx_c_pthread_attr_getinheritsched(Crx_C_Pthread_Attr const * pAttr, 
		int * pInheritSchedule)
	{return pthread_attr_getinheritsched(pAttr, pInheritSchedule);}
int32_t crx_c_pthread_attr_setinheritsched(Crx_C_Pthread_Attr * pAttr, int32_t pInheritSchedule);
	{return pthread_attr_setinheritsched(pAttr, pInheritSchedule);}
	
int32_t crx_c_pthread_attr_getschedparam(Crx_C_Pthread_Attr const * pAttr, 
		Crx_C_Pthread_Sched_Param * pParam)
	{return pthread_attr_getschedparam(pAttr, pParam);}
int32_t crx_c_pthread_attr_setschedparam(Crx_C_Pthread_Attr * pAttr, 
		Crx_C_Pthread_Sched_Param const * pParam)
	{return pthread_attr_setschedparam(pAttr, pParam);}*/
	

int32_t crx_c_pthread_sched_crx_reScheduleThisThread(void)
	{return pthread_sched_yield();}

void crx_c_pthread_sched_crx_runOtherThreadsInstead(void)
{
#if(WINDOWS)
	Sleep(1);
#else
	/*
		LINUX ALSO HAS select() WHICH IS MORE ACCURATE, UNLIKE nanosleep WHICH CAN CAUSE A DELAY
		OF 15MS WHEN ASKING FOR 1MS.
		
		MACOS CAN CAUSE VERY LARGE DELAYS WHEN ASKING FOR 1MS BECAUSE OF "TIMER COALESCING". THE
		FUNCTION "mach_wait_until" SEEM TO SUFFER FROM THE SAME PROBLEMS. THE DELAY CAN BE 60 TIMES
		LARGER FROM WHAT I HAVE SEEN.
	*/
	struct timespec tTimespec;
	struct timespec tTimespec__remaining;

	tTimespec.tv_sec  = 0;
	tTimespec.tv_nsec = 1000000;/*1MS*/

	nanosleep(&tTimespec, &tTimespec__remaining);
#endif
}


/*----*/

int32_t crx_c_pthread_sem_crx_init(Crx_C_Pthread_Sem * pSem, uint32_t pValue)
{
	CRX_SCOPE_META
	if(pSem == NULL))
		{return EINVAL;}

	CRX_SCOPE
	Crx_C_Pthread_Private_Sem * vSem = calloc(1, sizeof(Crx_C_Pthread_Private_Sem));
	int32_t vReturn = 0;

	if(vSem != NULL)
	{
		pthread_mutexattr_t tMutexAttr;

		vReturn = crx_c_pthread_initializeACrxRtCompliantMutexAttribute(&tMutexAttr);

		if(vReturn == 0)
		{
			vReturn = pthread_mutex_init(&(vSem->gPrivate_mutex), &tMutexAttr);

			if(vReturn == 0)
			{
				vReturn = crx_c_pthread_cond_crx_init(&(vSem->gPrivate_cond));

				if(vReturn == 0)
				{
#if(!MACOS)
					vReturn = sem_init(&(vSem->gPrivate_sem), 0, pValue);
#else
					vSem->gPrivate_semaphore = dispatch_semaphore_create(pValue);

					if(vSem->gPrivate_semaphore == NULL)
						{vReturn = ENOMEM;}
#endif
					if(vReturn != 0)
					{
						crx_c_pthread_cond_destroy(&(vSem->gPrivate_cond));
						pthread_mutex_destroy(&(vSem->gPrivate_mutex));

						free(vSem);
						vSem = NULL;
					}
				}
				else
				{
					pthread_mutex_destroy(&(vSem->gPrivate_mutex));

					free(vSem);
					vSem = NULL;
				}
			}
			else
			{
				free(vSem);
				vSem = NULL;
			}

			pthread_mutexattr_destroy(&tMutexAttr);
		}
	}

	*pSem = vSem;

	return vReturn;
}
int32_t crx_c_pthread_sem_destroy(Crx_C_Pthread_Sem * pSem)
{
	/*
		NOTE: THIS FUNCTION HAS NOT BEEN ANALYZED FOR SAFE OPERATION IF CALLED AND 
				A THREAD SOMEWHERE IS STILL HOLDING ON TO THE Sem RESOURCES
	*/
	int32_t vReturn = 0;
#if(!MACOS)
	vReturn = sem_destroy(&((*pSem)->gPrivate_sem));
#else
	dispatch_release((*pSem)->gPrivate_semaphore);
#endif

	if(vReturn == 0)
	{
		int32_t tResult = 0;

		vReturn = pthread_mutex_destroy(&((*pSem)->gPrivate_mutex));
		while(vReturn == EBUSY)
		{
			crx_c_pthread_sched_crx_runOtherThreadsInstead();

			vReturn = pthread_mutex_destroy(&((*pSem)->gPrivate_mutex));
		}

		tResult = crx_c_pthread_cond_destroy(&((*pSem)->gPrivate_cond));
		while(tResult == EBUSY)
		{
			crx_c_pthread_sched_crx_runOtherThreadsInstead();

			tResult = crx_c_pthread_cond_destroy(&((*pSem)->gPrivate_cond));
		}

		vReturn = ((vReturn == 0) ? tResult : vReturn);
	}

	return vReturn;
}

int32_t crx_c_pthread_sem_post(Crx_C_Pthread_Sem * pSem)
{
	int32_t vReturn = pthread_mutex_lock(&((*pSem)->gPrivate_mutex));

	if(vReturn == 0)
	{
#if(!MACOS)
		vReturn = sem_post(&((*pSem)->gPrivate_sem));
#else
		dispatch_semaphore_signal((*pSem)->gPrivate_semaphore);
#endif
		if(vReturn == 0)
			{crx_c_pthread_cond_broadcast(&((*pSem)->gPrivate_cond));}

		pthread_mutex_unlock(&((*pSem)->gPrivate_mutex));
	}

	return vReturn;
}

int32_t crx_c_pthread_sem_trywait(Crx_C_Pthread_Sem * pSem)
{
#if(!MACOS)
	return sem_trywait(&((*pSem)->gPrivate_sem));
#else
	return ((dispatch_semaphore_wait((*pSem)->gPrivate_semaphore, DISPATCH_TIME_NOW) == 0) ? 0 : EAGAIN);
#endif
}
int32_t crx_c_pthread_sem_wait(Crx_C_Pthread_Sem * pSem)
{
	CRX_SCOPE_META
	if((pSem == NULL) || (*pSem == NULL))
		{return EINVAL;}

	CRX_SCOPE
#if(!MACOS)
	return sem_wait(&((*pSem)->gPrivate_sem))l
#else
	/*
		NOTE THAT WE DO NOT SIMPLY USE
						 dispatch_semaphore_wait(s->sem, DISPATCH_TIME_FOREVER);
				BECAUSE THEN OUR WAIT IMPLEMENTATION WOULD NOT BE A CANCELLATION POINT
				PER POSIX. THEREFORE WE RELY ON THE POSIX CONDITION VARIABLE'S WAIT BEING A
				CANCELLATION POINT.
	*/
	int32_t vReturn = pthread_mutex_lock((*pSem)->gPrivate_semaphore);

	if(vReturn == 0)
	{
		vReturn = ((dispatch_semaphore_wait((*pSem)->gPrivate_semaphore, 
				DISPATCH_TIME_NOW) == 0) ? 0 : EAGAIN);

		while(vReturn != 0)
		{
			vReturn = crx_c_pthread_cond_internalUseOnly_wait(&((*pSem)->gPrivate_cond), 
					&((*pSem)->gPrivate_mutex));

			if(vReturn == 0)
			{
				vReturn = ((dispatch_semaphore_wait((*pSem)->gPrivate_semaphore, 
						DISPATCH_TIME_NOW) == 0) ? 0 : EAGAIN);
			}
			else
				{break;}
		}

		pthread_mutex_unlock(&((*pSem)->gPrivate_mutex));
	}

	return vReturn;
#endif
	CRX_SCOPE_END
}
int32_t crx_c_pthread_sem_crx_timedwait(Crx_C_Pthread_Sem * pSem, 
		uint64_t pNanoSeconds__monotonic)
{
	CRX_SCOPE_META
	if((pSem == NULL) || (*pSem == NULL))
		{return EINVAL;}

	CRX_SCOPE
	int32_t vReturn = pthread_mutex_lock(&((*pSem)->gPrivate_mutex));

	if(vReturn == 0)
	{
		bool tIsTimeRemaining = true;

#if(!MACOS)
		vReturn = sem_trywait(&((*pSem)->gPrivate_sem));
#else
		vReturn = ((dispatch_semaphore_wait((*pSem)->gPrivate_semaphore, 
				DISPATCH_TIME_NOW) == 0) ? 0 : EAGAIN);
#endif

		while((vReturn != 0) && (vReturn != EINVAL))
		{
			vReturn = crx_c_pthread_cond_crx_internalUseOnly_timedwait(&((*pSem)->gPrivate_cond), 
					&((*pSem)->gPrivate_mutex), pNanoSeconds__monotonic);

			if(vReturn == 0)
			{
				vReturn = crx_c_pthread_private_getIsTimeRemaining2(pNanoSeconds__monotonic, 
						&tIsTimeRemaining);

				if(vReturn == 0)
				{
					if(tIsTimeRemaining)
					{
#if(!MACOS)
						vReturn = sem_trywait(&((*pSem)->gPrivate_sem));
#else
						vReturn = ((dispatch_semaphore_wait((*pSem)->gPrivate_semaphore, 
								DISPATCH_TIME_NOW) == 0) ? 0 : EAGAIN);
#endif
					}
					else
					{
						vReturn = ETIMEDOUT;

						break;
					}
				}
				else
					{break;}
			}
			else
				{break;}
		}

		pthread_mutex_unlock(&((*pSem)->gPrivate_mutex));
	}

	return vReturn;
	CRX_SCOPE_END
}

/*----*/


void crx_c_pthread_crx_event_construct(Crx_C_Pthread_Crx_Event * pThis, bool pIsAutoReset)
{
	pThis->gPrivate_event = malloc(sizeof(Crx_C_Pthread_Crx_Private_Event));

	if(pThis->gPrivate_event != NULL)
	{
		pThis->gPrivate_event->gPrivate_count = 0;
		pThis->gPrivate_event->vIS_AUTO_RESET = pIsAutoReset;
		pThis->gPrivate_event->gPrivate_notifyingEvent = NULL;
		pThis->gPrivate_event->gPrivate_events = NULL;

		if(crx_c_pthread_mutex_init(&(pThis->gPrivate_event->gPrivate_mutex)) == 0)
		{
			if(crx_c_pthread_cond_crx_init(&(pThis->gPrivate_event->gPrivate_cond)) != 0)
			{
				crx_c_pthread_mutex_destroy(&(pThis->gPrivate_event->gPrivate_mutex));

				free(pThis->gPrivate_event);
				pThis->gPrivate_event = NULL;
			}
		}
		else
		{
			free(pThis->gPrivate_event);
			pThis->gPrivate_event = NULL;
		}
			
	}
}
void crx_c_pthread_crx_event_private_construct(Crx_C_Pthread_Crx_Event * pThis)
	{pThis->gPrivate_event = NULL;}

CRX__C__TYPE_BLUE_PRINT__DEFINE_GET_BLUE_PRINT(
		Crx_C_Pthread_Crx_Event, crx_c_pthread_crx_event_,
		CRXM__TRUE, CRXM__FALSE,
		CRXM__TRUE, CRXM__FALSE,
		CRXM__FALSE, CRXM__FALSE)

void crx_c_pthread_crx_event_destroy(Crx_C_Pthread_Crx_Event * pThis)
{
	int32_t tResult = crx_c_pthread_mutex_destroy(&(pThis->gPrivate_event->gPrivate_mutex));

	while(tResult == EBUSY)
	{
		crx_c_pthread_sched_crx_runOtherThreadsInstead();
		tResult = crx_c_pthread_mutex_destroy(&(pThis->gPrivate_event->gPrivate_mutex))
	}

	tResult = crx_c_pthread_cond_destroy(&(pThis->gPrivate_event->gPrivate_cond));
	while(tResult == EBUSY)
	{
		crx_c_pthread_sched_crx_runOtherThreadsInstead();
		tResult = crx_c_pthread_cond_destroy(&(tBarrier->gPrivate_cond));
	}
}

int32_t crx_c_pthread_crx_event_isEqualTo(Crx_C_Pthread_Crx_Event const * pThis,
		Crx_C_Pthread_Crx_Event const * pEvent)
{
	return (CRX__ARE_POINTERS_TO_SAME_OBJECT(pThis->gPrivate_event,
			pEvent->gPrivate_event) ? 1 : 0);
}

int32_t crx_c_pthread_crx_event_notify(Crx_C_Pthread_Crx_Event * pThis)
{
	Crx_C_Pthread_Crx_Event vEvent;

	crx_c_pthread_crx_event_private_construct(&vEvent);

	return crx_c_pthread_crx_event_private_notify(pThis, &vEvent);
}
int32_t crx_c_pthread_crx_event_private_notify(Crx_C_Pthread_Crx_Event * pThis,
		Crx_C_Pthread_Crx_Event pEvent__notifyingEvent)
{
	CRX_SCOPE_META
	if(pThis->gPrivate_event == NULL)
		{return EINVAL;}
	
	CRX_SCOPE
	int32_t vReturn = crx_c_pthread_mutex_lock(&(pThis->gPrivate_event->gPrivate_mutex));
	
	if(vReturn == 0)
	{
		pThis->gPrivate_event->gPrivate_notifyingEvent = pEvent__notifyingEvent.gPrivate_event;

		if(!pThis->gPrivate_event->vIS_AUTO_RESET)
		{
			pThis->gPrivate_event->gPrivate_count++;

			crx_c_pthread_cond_signal(&(pThis->gPrivate_event->gPrivate_cond));
		}
		else
		{
			pThis->gPrivate_event->gPrivate_count = 1;
			crx_c_pthread_cond_broadcast(&(pThis->gPrivate_event->gPrivate_cond));
		}
		/*TODO: STILL TO NOTIY THE LINKED EVENTS AND SET THEIR gPrivate_notifyingEvent MEMBER*/
		
		if(pThis->gPrivate_event->gPrivate_events != NULL)
		{
			Crx_C_Pthread_Crx_Event tEvent;

			crx_c_pthread_crx_event_private_construct(&tEvent);

			CRX_FOR(size_t tI = 0, 
					tI < crx_c_array_getLength(pThis->gPrivate_event->gPrivate_events), tI++)
			{
				tEvent = *((Crx_C_Pthread_Crx_Event *)crx_c_array_get(
						pThis->gPrivate_event->gPrivate_events, tI));

				crx_c_pthread_crx_event_private_notify(&tEvent, pThis);
			}
			CRX_ENDFOR
		}

		crx_c_pthread_mutex_unlock(&(pThis->gPrivate_event->gPrivate_mutex));
	}
	CRX_SCOPE_END
}
void crx_c_pthread_crx_event_stopNotification(Crx_C_Pthread_Crx_Event * pThis)
{
	if((pThis->gPrivate_event != NULL) && (pThis->gPrivate_event->vIS_AUTO_RESET))
		{pThis->gPrivate_event->gPrivate_count = 0;}
}

int32_t crx_c_pthread_crx_event_addLinkedEvent(Crx_C_Pthread_Crx_Event * pThis, 
		Crx_C_Pthread_Crx_Event * pEvent)
{
	CRX_SCOPE_META
	if(pThis->gPrivate_event == NULL)
		{return EINVAL;}
	
	CRX_SCOPE
	int32_t vReturn = crx_c_pthread_mutex_lock(&(pThis->gPrivate_event->gPrivate_mutex));

	if(vReturn == 0)
	{
		if(pThis->gPrivate_event->gPrivate_events == NULL)
		{
			pThis->gPrivate_event->gPrivate_events = crx_c_array_new(
					crx_c_pthread_crx_event_getTypeBluePrint(), 0, 4);
		}

		if(pThis->gPrivate_event->gPrivate_events != NULL)
		{
			bool tIsAlreadyAssigned = false;

			CRX_FOR(size_t tI = 0, 
					tI < crx_c_array_getLength(pThis->gPrivate_event->gPrivate_events), tI++)
			{
				if(CRX__ARE_POINTERS_TO_SAME_OBJECT(pEvent->gPrivate_event, 
						(crx_c_array_get(pThis->gPrivate_event->gPrivate_events, tI))->
						gPrivate_event, true))
				{
					tIsAlreadyAssigned = true;

					break;
				}
			}
			CRX_ENDFOR

			if(!tIsAlreadyAssigned)
				{crx_c_array_push(pThis->gPrivate_event->gPrivate_events, *pEvent);}
		}
		else
			{vReturn = ENOMEM;}

		crx_c_pthread_mutex_unlock(&(pThis->gPrivate_event->gPrivate_mutex));
	}

	return vReturn;
	CRX_SCOPE_END
}
int32_t crx_c_pthread_crx_event_removeLinkedEvent(Crx_C_Pthread_Crx_Event * pThis, 
		Crx_C_Pthread_Crx_Event * pEvent)
{
	CRX_SCOPE_META
	if(pThis->gPrivate_event == NULL)
		{return EINVAL;}
	
	CRX_SCOPE
	int32_t vReturn = crx_c_pthread_mutex_lock(&(pThis->gPrivate_event->gPrivate_mutex));

	if(vReturn == 0)
	{
		if(pThis->gPrivate_event->gPrivate_events != NULL)
		{
			bool tIsAlreadyAssigned = false;

			CRX_FOR(size_t tI = 0, 
					tI < crx_c_array_getLength(pThis->gPrivate_event->gPrivate_events), tI++)
			{
				if(CRX__ARE_POINTERS_TO_SAME_OBJECT(pEvent->gPrivate_event, 
						(crx_c_array_get(pThis->gPrivate_event->gPrivate_events, tI))->
						gPrivate_event, true))
				{
					crx_c_array_removeElements(pThis->gPrivate_event->gPrivate_events, tI, 1);

					break;
				}
			}
			CRX_ENDFOR
		}

		crx_c_pthread_mutex_unlock(&(pThis->gPrivate_event->gPrivate_mutex));
	}

	return vReturn;
	CRX_SCOPE_END
}

int32_t crx_c_pthread_crx_event_wait(Crx_C_Pthread_Crx_Event * pThis)
{
	return crx_c_pthread_crx_event_private_wait(pThis, NULL, 
			CRX__C__PTHREAD__CRX__EVENT__INFINITE_WAIT);
}
int32_t crx_c_pthread_crx_event_wait2(Crx_C_Pthread_Crx_Event * pThis,
		uint64_t pNumberOfNanoSeconds)
{
	return crx_c_pthread_crx_event_private_wait(pThis, NULL, 
			(pNumberOfNanoSeconds != CRX__C__PTHREAD__CRX__EVENT__INFINITE_WAIT) ? 
			pNumberOfNanoSeconds : CRX__C__PTHREAD__CRX__EVENT__INFINITE_WAIT - 1);
}
int32_t crx_c_pthread_crx_event_private_wait(Crx_C_Pthread_Crx_Event * pThis, 
		Crx_C_Pthread_Crx_Event * * pEvent__notifyingEvent,
		uint64_t pNumberOfNanoSeconds)
{
	int32_t vReturn = 0;
	uint64_t vTime = 0;
	
	if(pNumberOfNanoSeconds == 0)
		{vReturn = crx_c_pthread_mutex_trylock(&(pThis->gPrivate_event->gPrivate_mutex));}
	else if(pNumberOfNanoSeconds == CRX__C__PTHREAD__CRX__EVENT__INFINITE_WAIT)
		{vReturn = crx_c_pthread_mutex_lock(&(pThis->gPrivate_event->gPrivate_mutex));}
	else
	{
		vReturn = crx_c_pthread_crx_computeCurrentMonotonicTime(&vTime);

		if(vReturn == 0)
		{
			vTime = vTime + pNumberOfNanoSeconds;
			vReturn = crx_c_pthread_mutex_crx_timedlock(&(pThis->gPrivate_event->gPrivate_mutex), vTime);
		}
	}

	if(vReturn == 0)
	{
		if(pNumberOfNanoSeconds == 0)
		{
			if(!pThis->gPrivate_event->vIS_AUTO_RESET)
			{
				if(pThis->gPrivate_event->gPrivate_count <= 0)
					{vReturn = ETIMEDOUT;}
				else
					{pThis->gPrivate_event->gPrivate_count--;}
			}
			else
			{
				if(pThis->gPrivate_event->gPrivate_count == 0)
					{vReturn = ETIMEDOUT;}
			}
		}
		else if(pNumberOfNanoSeconds == CRX__C__PTHREAD__CRX__EVENT__INFINITE_WAIT)
		{
			if(!pThis->gPrivate_event->vIS_AUTO_RESET)
			{
				pThis->gPrivate_event->gPrivate_count--;

				while(pThis->gPrivate_event->gPrivate_count < 0)
				{
					vReturn = crx_c_pthread_cond_wait(&(pThis->gPrivate_event->gPrivate_cond), 
							&(pThis->gPrivate_event->gPrivate_mutex));
				}
			}
			else
			{
				while(pThis->gPrivate_event->gPrivate_count == 0)
				{
					vReturn = crx_c_pthread_cond_wait(&(pThis->gPrivate_event->gPrivate_cond),
							&(pThis->gPrivate_event->gPrivate_mutex));
				}
			}
		}
		else
		{
			bool tIsTimeRemaining = true;

			if(!pThis->gPrivate_event->vIS_AUTO_RESET)
			{
				pThis->gPrivate_event->gPrivate_count--;

				while(pThis->gPrivate_event->gPrivate_count < 0)
				{
					vReturn = crx_c_pthread_cond_crx_timedwait(&(pThis->gPrivate_event->gPrivate_cond), 
							&(pThis->gPrivate_event->gPrivate_mutex), vTime);
					
					if(vReturn == 0)
					{
						vReturn = crx_c_pthread_private_getIsTimeRemaining2(pNanoSeconds__monotonic, 
								&tIsTimeRemaining);

						if(vReturn == 0)
						{
							if(!tIsTimeRemaining && (pThis->gPrivate_event->gPrivate_count < 0))
							{
								vReturn = ETIMEDOUT;

								break;
							}
						}
						else
							{break;}
					}
				}
				
				if(vReturn == ETIMEDOUT)
					{pThis->gPrivate_event->gPrivate_count++;}
			}
			else
			{
				while(pThis->gPrivate_event->gPrivate_count == 0)
				{
					vReturn = crx_c_pthread_cond_crx_timedwait(&(pThis->gPrivate_event->gPrivate_cond),
							&(pThis->gPrivate_event->gPrivate_mutex), vTime);
					
					if(vReturn == 0)
					{
						vReturn = crx_c_pthread_private_getIsTimeRemaining2(pNanoSeconds__monotonic, 
								&tIsTimeRemaining);

						if(vReturn == 0)
						{
							if(!tIsTimeRemaining && (pThis->gPrivate_event->gPrivate_count == 0))
							{
								vReturn = ETIMEDOUT;

								break;
							}
						}
						else
							{break;}
					}
				}
			}
		}
		
		if(vReturn == 0)
		{
			if(pEvent__notifyingEvent != NULL)
				{*pEvent__notifyingEvent = pThis->gPrivate_event->gPrivate_notifyingEvent;}

			pThis->gPrivate_event->gPrivate_notifyingEvent = NULL;
			
			vReturn = crx_c_pthread_mutex_unlock(&(pThis->gPrivate_event->gPrivate_mutex));
		}
		else
			{crx_c_pthread_mutex_unlock(&(pThis->gPrivate_event->gPrivate_mutex));}
	}

	return vReturn;
}

int32_t crx_c_pthread_crx_event_waitForAnyOf(Crx_C_Pthread_Crx_Event * pEvents, 
		size_t pNumberOfEvents, int32_t * pIndexOfEvent__out)
{
	return crx_c_pthread_crx_event_private_waitForAnyOf(pEvents, pIndexOfEvent__out, 
			CRX__C__PTHREAD__CRX__EVENT__INFINITE_WAIT);
}
int32_t crx_c_pthread_crx_event_waitForAnyOf2(Crx_C_Pthread_Crx_Event * pEvents, 
		size_t pNumberOfEvents, int32_t * pIndexOfEvent__out, uint64_t pNumberOfNanoSeconds)
{
	return crx_c_pthread_crx_event_private_waitForAnyOf(pEvents, pNumberOfEvents, pIndexOfEvent__out, 
			(pNumberOfNanoSeconds != CRX__C__PTHREAD__CRX__EVENT__INFINITE_WAIT) ?
			pNumberOfNanoSeconds : (CRX__C__PTHREAD__CRX__EVENT__INFINITE_WAIT - 1));
}
int32_t crx_c_pthread_crx_event_private_waitForAnyOf(Crx_C_Pthread_Crx_Event * pEvents, 
		size_t pNumberOfEvents, int32_t * pIndexOfEvent__out, uint64_t pNumberOfNanoSeconds)
{
	CRX_SCOPE_META
	if((pEvents == NULL) || (pNumberOfEvents == 0))
		{return 0;}
	
	CRX_SCOPE	
	Crx_C_Pthread_Crx_Event vEvent /*= ?*/;
	size_t vI = 0;
	
	crx_c_pthread_crx_event_construct(&vEvent);
	

	while((vI < pNumberOfEvents) && (vReturn == 0)
	{
		vReturn = crx_c_pthread_crx_event_addLinkedEvent(pEvents + vI, vEvent);
		vI++;
	}
	
	if(vReturn == 0)
	{
		Crx_C_Pthread_Crx_Private_Event * tEvent = NULL;

		vReturn = crx_c_pthread_crx_event_private_wait(&vEvent, &tEvent, pNumberOfNanoSeconds);

		if(pIndexOfEvent__out != NULL)
		{
			if(tEvent != NULL)
			{
				CRX_FOR(size_t tI = 0, tI < pNumberOfEvents, tI++)
				{
					if(CRX__ARE_POINTERS_TO_SAME_OBJECT((pEvents + tI)->gPrivate_event, tEvent),
							false)
					{
						*pIndexOfEvent__out = tI;

						break;
					}
				}
				CRX_ENDFOR
			}
			else
				{*pIndexOfEvent__out = -1;}
		}
	}

	vI = 0;
	while(vI < pNumberOfEvents)
	{
		vReturn = crx_c_pthread_crx_event_removeLinkedEvent(pEvents + vI, vEvent);
		vI++;
	}
	
	crx_c_pthread_crx_event_destruct(&vEvent);

	return vReturn;
	CRX_SCOPE_END
}


CRX__C__TYPE_BLUE_PRINT__DEFINE_GET_BLUE_PRINT(Crx_C_Pthread_Crx_Private_Mail,
		crx_c_pthread_crx_private_mail_, 
		CRXM__TRUE, CRXM__FALSE, 
		CRXM__TRUE, CRXM__FALSE, 
		CRXM__FALSE, CRXM__FALSE);
		
CRX__C__TYPE_BLUE_PRINT__DEFINE_GET_BLUE_PRINT(Crx_C_Pthread_Crx_Private_PostponedMail,
		crx_c_pthread_crx_private_postponedMail_, 
		CRXM__TRUE, CRXM__FALSE, 
		CRXM__TRUE, CRXM__FALSE, 
		CRXM__FALSE, CRXM__FALSE);
		
crx_c_pthread_crx_private_postponedMail_getOrder(
		Crx_C_Pthread_Crx_Private_PostponedMail * pPostponedMail,
		Crx_C_Pthread_Crx_Private_PostponedMail * pPostponedMail__2)
{
	if(pPostponedMail->gDueTime < pPostponedMail__2->gDueTime)
		{return -1;}
	else if(pPostponedMail->gDueTime > pPostponedMail__2->gDueTime)
		{return 1;}
	else
		{return 0;}
}



CRX__C__TYPE_BLUE_PRINT__DEFINE_GET_BLUE_PRINT(
		Crx_C_Pthread_Crx_Private_ThreadPoolTask, crx_c_pthread_crx_private_threadPoolTask_,
		CRXM__TRUE, CRXM__FALSE,
		CRXM__TRUE, CRXM__FALSE,
		CRXM__FALSE, CRXM__FALSE)

crx_c_pthread_crx_threadPool_construct(Crx_C_Pthread_Crx_ThreadPool * pThis)
{
	pThis->gPrivate_threadPool = malloc(sizeof(Crx_C_Pthread_Crx_Private_ThreadPool));

	if(pThis->gPrivate_threadPool != NULL)
	{
		pthread_mutexattr_t tMutexAttr;
		int32_t tResult = crx_c_pthread_initializeACrxRtCompliantMutexAttribute(&tMutexAttr);

		if(tResult == 0)
		{
			/*pthread_mutexattr_settype(pMutexAttr, PTHREAD_MUTEX_RECURSIVE);*/
			tResult = crx_c_pthread_mutex_init(&(pThis->gPrivate_threadPool->gPrivate_mutex), 
					&tMutexAttr);

			if(tResult == 0)
			{
				tResult = crx_c_pthread_cond_crx_init(&(pThis->gPrivate_threadPool->gPrivate_cond));

				if(tResult == 0)
				{
					crx_c_ring_construct(&(pThis->gPrivate_threadPool->gPrivate_threads), 
							crx_c_getTypeBluePrintForVoidPointer(), 0, 4, 0);
					crx_c_queue_construct(&(pThis->gPrivate_threadPool->gPrivate_threadPoolTasks), 
							crx_c_pthread_crx_private_threadPoolTask_getTypeBluePrint(), 8);
				}
				else
					{crx_c_pthread_mutex_destroy(&(pThis->gPrivate_threadPool->gPrivate_mutex));}
			}
			
			pthread_mutexattr_destroy(&tMutexAttr);
		}
		
		if(tResult != 0)
		{
			free(pThis->gPrivate_threadPool);
			pThis->gPrivate_threadPool = NULL;
		}
	}
}
crx_c_pthread_crx_threadPool_private_construct(Crx_C_Pthread_Crx_ThreadPool * pThis)
	{pThis->gPrivate_threadPool = NULL;}

CRX__C__TYPE_BLUE_PRINT__DEFINE_GET_BLUE_PRINT(
		Crx_C_Pthread_Crx_ThreadPool, crx_c_pthread_crx_threadPool_,
		CRXM__TRUE, CRXM__FALSE,
		CRXM__TRUE, CRXM__FALSE,
		CRXM__FALSE, CRXM__FALSE)
		
int32_t crx_c_pthread_crx_threadPool_destroy(Crx_C_Pthread_Crx_ThreadPool * pThis)
{
	int32_t vReturn = 0;

	if(pThis->gPrivate_threadPool != NULL)
	{
		vReturn = crx_c_pthread_mutex_trylock(&(pThis->gPrivate_threadPool->gPrivate_mutex));

		if(vReturn == 0)
		{
			CRX_FOR(size_t tI = 0; tI < crx_c_ring_getLength(
					&(pThis->gPrivate_threadPool->gPrivate_threads)), tI++)
			{
				Crx_C_Pthread_Thread tThread = *((Crx_C_Pthread_Thread *)crx_c_ring_get(
						&(pThis->gPrivate_threadPool->gPrivate_threads)), tI);

				tThread->gPrivate_threadPool = NULL;
			}
			CRX_ENDFOR
			
			crx_c_ring_empty(&(pThis->gPrivate_threadPool->gPrivate_threads));

			crx_c_pthread_mutex_unlock(&(pThis->gPrivate_threadPool->gPrivate_mutex));

			if(vReturn == 0)
			{
				vReturn = crx_c_pthread_mutex_destroy(&(pThis->gPrivate_threadPool->gPrivate_mutex));

				if(vReturn == 0)
				{
					vReturn = crx_c_pthread_cond_destroy(
							&(pThis->gPrivate_threadPool->gPrivate_cond));

					if(vReturn == 0)
					{
						crx_c_ring_destruct(&(pThis->gPrivate_threadPool->gPrivate_threads));
						crx_c_queue_destruct(
								&(pThis->gPrivate_threadPool->gPrivate_threadPoolTasks));
					}
				}
			}
		}
	}

	return vReturn;
}

int32_t crx_c_pthread_crx_threadPool_isEqualTo(Crx_C_Pthread_Crx_ThreadPool const * pThis,
		Crx_C_Pthread_Crx_ThreadPool const * pThreadPool)
{
	return (CRX__ARE_POINTERS_TO_SAME_OBJECT(pThis->gPrivate_threadPool,
			pThreadPool->gPrivate_threadPool) ? 1 : 0);
}

int32_t crx_c_pthread_crx_threadPool_addTask(Crx_C_Pthread_Crx_ThreadPool * pThis,
		void (* pFunc)(void *), void * pArguments)
{
	CRX_SCOPE_META
	if(pThis->gPrivate_threadPool == NULL)
		{return EINVAL;}
	
	CRX_SCOPE	
	Crx_C_Pthread_Crx_Private_ThreadPoolTask vThreadPoolTask /*= ?*/;
	int32_t vReturn = 0;

	vThreadPoolTask.gFunc = pFunc;
	vThreadPoolTask.gArguments = pArguments;

	vReturn = crx_c_pthread_mutex_lock(&(pThis->gPrivate_threadPool->gPrivate_mutex));

	if(vReturn == 0)
	{
		if(!crx_c_queue_push(&(pThis->gPrivate_threadPool->gPrivate_threadPoolTasks), vThreadPoolTask))
			{vReturn = ENOMEM;}

		crx_c_pthread_mutex_unlock(&(pThis->gPrivate_threadPool->gPrivate_mutex));
	}

	return vReturn;
	CRX_SCOPE_END
}

crx_c_pthread_crx_threadPool_broadcastMail(Crx_C_Pthread_Crx_ThreadPool * pThis, 
		uint64_t pSenderId, uint64_t pShortData, 
		void * (*pFunc)(void *), void * pMessage)
{
	CRX_SCOPE_META
	if(pThis->gPrivate_threadPool == NULL)
		{return EINVAL;}
	
	CRX_SCOPE
	vReturn = crx_c_pthread_mutex_lock(&(pThis->gPrivate_threadPool->gPrivate_mutex));

	if(vReturn == 0)
	{
		CRX_FOR(size_t tI = 0; tI < crx_c_ring_getLength(
				&(pThis->gPrivate_threadPool->gPrivate_threads)), tI++)
		{
			Crx_C_Pthread_Thread tThread = *((Crx_C_Pthread_Thread *)crx_c_ring_get(
					&(pThis->gPrivate_threadPool->gPrivate_threads)), tI);

			crx_c_pthread_thread_crx_sendMail(tThread, pSenderId, pShortData, pFunc, pMessage);
		}

		crx_c_pthread_mutex_unlock(&(pThis->gPrivate_threadPool->gPrivate_mutex));
	}
	CRX_ENDFOR

	return vReturn;
	CRX_SCOPE_END
}

crx_c_pthread_crx_threadPool_cancelAll()
{
	CRX_SCOPE_META
	if(pThis->gPrivate_threadPool == NULL)
		{return EINVAL;}
	
	CRX_SCOPE
	vReturn = crx_c_pthread_mutex_lock(&(pThis->gPrivate_threadPool->gPrivate_mutex));

	if(vReturn == 0)
	{
		CRX_FOR(size_t tI = 0; tI < crx_c_ring_getLength(
				&(pThis->gPrivate_threadPool->gPrivate_threads)), tI++)
		{
			Crx_C_Pthread_Thread tThread = *((Crx_C_Pthread_Thread *)crx_c_ring_get(
					&(pThis->gPrivate_threadPool->gPrivate_threads)), tI);

			crx_c_pthread_thread_cancel(tThread);
		}

		crx_c_pthread_mutex_unlock(&(pThis->gPrivate_threadPool->gPrivate_mutex));
	}
	CRX_ENDFOR

	return vReturn;
	CRX_SCOPE_END
}

crx_c_pthread_crx_threadPool_join()
{
	CRX_SCOPE_META
	if(pThis->gPrivate_threadPool == NULL)
		{return EINVAL;}
	
	CRX_SCOPE
	vReturn = crx_c_pthread_mutex_lock(&(pThis->gPrivate_threadPool->gPrivate_mutex));

	if(vReturn == 0)
	{
		size_t tI = 0;
		void * tCode = 0;

		while(crx_c_ring_getLength(&(pThis->gPrivate_threadPool->gPrivate_threads)) > 0)
		{
			Crx_C_Pthread_Thread tThread = *((Crx_C_Pthread_Thread *)crx_c_ring_get(
					&(pThis->gPrivate_threadPool->gPrivate_threads)), 0);

			if(crx_c_pthread_thread_isDetached(tThread) == 0)
			{
				crx_c_pthread_mutex_unlock(&(pThis->gPrivate_threadPool->gPrivate_mutex));
				crx_c_pthread_thread_join(tThread, &tCode);
				vReturn = crx_c_pthread_mutex_lock(&(pThis->gPrivate_threadPool->gPrivate_mutex));

				if(vReturn != 0)
					{break;}
			}
		}

		crx_c_pthread_mutex_unlock(&(pThis->gPrivate_threadPool->gPrivate_mutex));
	}

	return vReturn;
	CRX_SCOPE_END
}



/*----*/

void crx_c_pthread_crx_private_getLockForSync(int32_t * pInt32)
{
	pthread_mutex_lock(&(gCrx_c_pthread_crx_private_atomic_mutex[
			((((size_t)pInt32) >> 2) & (0x1F))]));
}
void crx_c_pthread_crx_private_releaseLockForSync(int32_t * pInt32)
{
	pthread_mutex_unlock(&(gCrx_c_pthread_crx_private_atomic_mutex[
			((((size_t)pInt32) >> 2) & (0x1F))]));
}

void crx_c_pthread_crx_private_getLockForSync2(int64_t * pInt64)
{
	pthread_mutex_lock(&(gCrx_c_pthread_crx_private_atomic_mutex[
			((((size_t)pInt64) >> 3) & (0x1F))]));
}
void crx_c_pthread_crx_private_releaseLockForSync2(int64_t * pInt64)
{
	pthread_mutex_unlock(&(gCrx_c_pthread_crx_private_atomic_mutex[
			((((size_t)pInt64) >> 3) & (0x1F))]));
}

/*
	CURRENTLY, ASSEMBLY CODE WHEN USED SUPPORTS THE FOLLOWING:
		- POWERPC 64.
		- ARMV6 AND UP, BOTH 32BIT AND 64BIT.
			- CODE IS WRITTENT TO SUPPORT ARMV5 AND UP, BUT BECAUSE PRE ARMV6 THERE IS NO MECHANISM
					FOR ATOMICS UNLESS FUNCTIONS ARE PROVIDED BY THE ENVIRONMENT, WE ARE BOUND
					BY THE LIMITS OF THE ENVIRONMENT, SUCH AS THE COMPILER, THE OPERATING SYSTEM,
					AND THE STANDARD LIBRARIES, BUT ULTIMATELY BY THE OPERATING SYSTEM ITSELF WHICH
					HAS TO DO SOMETHING USER MODE CODE CAN NOT POSSIBLY DO. ON LINUX, ROUTINES EXIST 
					THAT YOU CAN USE AS COMPARE AND EXCHANGE OPERATION, BUT THE ROUTINE THAT 
					SUPPORTS 64BIT INTEGERS DOES NOT EXIST UNTIL LINUX 3.1.
			- ARM 64BIT STARTS WITH ARMV8
			- 64BIT ATOMICS ON 32BIT ARM WOULD REQUIRE ARMv6K AND UP. ROUGHLY FROM 2002.
			- ARMV7 AND UP HAD INSTRUCTIONS FOR MEMORY BARRIERS, SUCH AS dmb, THAT THE CODE COULD 
					USE INSTEAD OF HAVING TO WRITE INSTRUCTIONS TO THE COPROCESSOR 15 WHICH
					CAN NOT BE DONE USER MODE CODE. HENCE, ON LINUX ARMV6 AND UP ARE SUPPORTED FOR 
					ARM 32BIT BECAUSE OF THE MEMORY BARRIER KERNEL ROUTINE THAT LINUX PROVIDES, BUT 
					ON OTHER SYSTEMS, IF USING ASSEMBLY, ONLY ARMV7 AND UP ARE SUPPORTED FOR ARM 
					32BIT.
			- IN 2023, ATOMICS AS DEFINED IN THE STANDARD, MEANING LEVEL 1 ROOMS, WERE INTRODUCED.
		- X86, BOTH 32BIT AND 64BIT. STARTING 166MHZ MACHINES, MEANING AMD K6, AND INTEL PENTIUM.
				PER THE 166MHZ REFERENCE POINT REQUIREMENT, IT IS AMD K6 AND UP, HOWEVER, AMD K5
				AND UP ALSO WORKS. IT HAS CMPXCHG8B.
		- SPARC 64BIT.
		- SYSTEM Z, BOTH 32BIT (MEANING SYSTEM 390) AND 64BIT.
			- NOTE THAT 32BIT IS ACTUALLY USING 31BIT ADDRESSING.
		- MIPS 64BIT
			- NOTE THAT THE CHECK "(_MIPS_SIM == _ABIO32)" IS NOT REQUIRED, BUT KEEPING FOR NOW.
					IT IS NOT REQUIRED BECAUSE THE O32 ABI, WHICH IS DEDECTED BY THE ABOVE, IS
					ASSUMED A 32BIT ARCHITECTURE BY CRX EVEN IF THE TARGET WAS THE 64BIT 
					ARCHITECTURE.
			
	A GENERIC FALLBACK COULD BE PROVIDED USING A SET OF 32 PTHREAD MUTICES. THE ATOMIC'S ADDRESS
			WOULD BE USED TO DECIDE WHICH MUTEX TO USE TO CONTROL ACCESS. COUNTING FROM LOW(LEFT)
			TO HIGH(RIGHT), BITS 0-2, MEANING FIRST 3 BITS, WOULD BE IGNORED BECAUSE A 64BIT VALUE
			IS EXPECTED TO BE ALIGNED TO 8 BYTES. BITS 3-7 WOULD BE USED TO FIND THE MUTEX TO USE
			TO CONTROL ACCESS TO THE ATOMIC. YOU COULD INCREASE THE COUNT TO 64 OR PERHAPS 128, BUT
			I AM HOPING 32 IS ENOUGH. REMEMBER, HIGHER BITS ARE LESS RANDOM THAN LOWER BITS, WHICH
			IS WHY WE RELY ON THE LOWER BITS.

	IT APPEARS THAT C IMPLEMENTATIONS WOULD BE USING AN APPROACH SIMILAR TO THE ABOVE, MEANING
			THEY DO NOT USE ONE LOCK PER ATOMIC WHEN USING LOCKS AS A FOR BACK. 'LOCK' HERE IS 
			INFORMAL. THIS MEANS THAT THERE IS NO REASON FOR THE ATOMICS TYPES IN C NOT TO BE
			PART OF THE TRIVIAL CATEGORY AS DEFINED IN THE STANDARD, BUT UNFORTUANETLY C HAS A
			KEYWORD JUST FOR ATOMICS, WHICH MAKES THE ATOMICS SPECIAL TYPES IT SEEMS. IF I AM NOT
			WRONG, THIS MEANS THAT A FALLBACK TO USING C'S IMPLEMENTATION IS STILL NOT FEASABLE.
			
			REMEMBER THERE ARE TWO ISSUES WHEN CONSIDERING A FALLBACK. ONE, THE ATOMIC TYPES MUST
			REMAIN AT LEAST FROM THE TRIVIAL TYPE CATEGORY, BUT BETTER, BE PRIMITIVES. IN OUR CASE,
			INT32 AND INT64. TWO, THE ATOMICS MUST BE ROOM LEVEL 1 OR ROOM LEVEL 2 AS DEFINED IN THE
			STANDARD. MEANING, WHAT IS CALLED ATOMICS IN THE CODE MUST BE WHAT IS FORMALLY CALLED 
			ATOMICS OR WHAT IS FORMALLY CALLED ARTIFICIAL ATOMICS IN THE STANDARD. 
			
	MACOS 10.12 DEPRECATES THE ATOMIC INTRINSICS, AND GCC CAN COMPLAIN. YOU MIGHT WANT TO TURN
			OFF THE DEPRECATED WARNINGS TEMPORARILY WHEN THIS HAPPENS

	FOR AIX, YOU MIGHT WANT TO SWITCH FROM USING LABELS IN GCC ASSEMBLY TO USING EXPLICIT OFFSETS
*/
int32_t crx_c_pthread_crx_doInterlockedSyncCompareExchange(int32_t volatile * pInt32,
		int32_t pInt32__2, int32_t pCurrentExpectedValue)
{
#if((CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__WATCOM) || \
		(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__OPEN_WATCOM))
	/* Don't report that result is not assigned a value before being referenced */
#	pragma disable_message (200)
#endif
	int32_t vReturn;

#if(CRX__C__PTHREAD__PRIVATE__IS_TO_USE_LOCK_FOR_SYNC)
	crx_c_pthread_crx_private_getLockForSync(pInt32);

	vReturn = *pInt32;
	
	if(vReturn == pCurrentExpectedValue)
		{*pInt32 = pInt32__2;}

	crx_c_pthread_crx_private_releaseLockForSync(pInt32);
#elif(CRX__C__PTHREAD__PRIVATE__IS_TO_USE_ASSEMBLY_FOR_ATOMICS)
	if(ptw32_smp_system)
	{
	#if((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__X86) || \
			(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__AMD64))
		#if((CRX__ARCHITECTURE__SIZE == 32) || (CRX__ARCHITECTURE__SIZE == 64))
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__MSVC)
		_asm
		{
			PUSH         ecx
			PUSH         edx
			MOV          ecx,dword ptr [pInt32]
			MOV          edx,dword ptr [pInt32__2]
			MOV          eax,dword ptr [pCurrentExpectedValue]
			LOCK CMPXCHG dword ptr [ecx],edx
			MOV          dword ptr [vReturn], eax
			POP          edx
			POP          ecx
		}
			#elif(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
			
		/*
		__asm__ __volatile__
		(
			"lock\n\t"
			"cmpxchgl       %2,%1"      //if (EAX == [pInt32])
										//	[pInt32] = pInt32__2
										//else
										//	EAX = [pInt32]
			:"=a" (vReturn)
			:"m"  (*pInt32), "r" (pInt32__2), "a" (pCurrentExpectedValue)
		);
		*/
		__asm__ __volatile__
		(
			"lock\n\t"
			"cmpxchgl       %2,%1"
			:"=a" (vReturn)
			:"m"  (*pInt32), "r" (pInt32__2), "a" (pCurrentExpectedValue)
			:"memory", "cc"
		);
			#else
		asser(false);
			#endif
		#else
		assert(false);
		#endif
	#elif(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__POWER_PC)
		#if(CRX__ARCHITECTURE__SIZE == 64)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		uint32_t * tLocation = ((uint32_t *)pInt32);
		uint32_t tComparisonValue = *((uint32_t *)(&pCurrentExpectedValue));
		uint32_t tNewValue = *((uint32_t *)(&pInt32__2));

		/*__asm__ __volatile__
		(
			//sync IS A FULL MEMORY BARRIER. ALL OUR ATOMICS MUST BE FULL MEMORY BARRIERS ALSO
			"sync\n\t"
			//LOAD WHAT IS REFERENCED BY %2 INTO %0. THIS IS THE 'STORE' SET BY lwarx TO BE REFERED
			//		TO BY stwcx. LATER. NOTE HOW '.' IS PART OF stwcx..
			//		lwarx EXPECTS 32-BIT UNSIGNED
			//A FOURTH PARAMETER CAN BE USED TO TELL THE MACHINE THAT ADJACENT CODE MIGHT BE CHANGED 
			//		OR NOT. IT IS A HINT TO THE MACHINE. THE MACHINE, WITH lwarx, KEEPS AN EYE ON 
			//		THE RESERVED VALUE, WHICH IN THIS CASE IS THE ADDRESS BY mem AND THE VALUE IT 
			//		HOLDS, BUT IS ALSO KEEPS AN EYE ON ADJACENT MEMORY. WHAT THIS MEANS, IF I AM 
			//		NOT WRONG, THAT ANY ADJACENT MEMORY CHANGES ARE STILL TREATED AS CHANGES TO THAT 
			//		RESERVED MEMORY LOCATION AND VALUE. THE HINT PERHAPS HELPS SPEED THINGS UP IN 
			//		THIS CASE. THIS IS NOT SUPPORTED EVERYTWHERE. AVOIDING.
			"1:\tlwarx %0,0,%2\n\t"
			//THE 'l' IN cmplw MEANS 'LOGICAL', MEANING UNSIGNED. THE 'w' MEANS WORD, 4 BYTES.
			"cmplw %3,%0\n\t"
			//THE - IN bne- DISABLES THE BRANCH PREDICTION. IT IS CALLED A "STATIC BRANCH HINT". 
			//		I THINK IT IS USEFUL IN JUMPING BACKWARDS, BUT I AM NOT SURE.
			//BOOST SEEMED TO THINK LABELS DO NOT EXIST IF _AIX IS DEFINED. 
			"bne- 2f\n\t"	//IF NO LABELS, USE "bne- +12" IF I AM NOT WRONG.
			//IF %2 STILL EQUALS TO %0, MEANING THE 'STORE', COMMIT %1 INTO %2, ELSE SET A FLAG
			//		YOU MUST ALWAYS COMMIT TO THE SAME LOCATION WHEN USING THE CORRESPONDING lwarx
			//		IN OTHER WORDS, %2 IN THIS CASE. HOWEVER, IF YOU SIMPLY NEED TO CLEAR THE 
			//		RESERVATION YOU CAN COMMIT INTO A HARMLESS LOCATION PROVIDED THAT YOU DO NOT
			//		ASSUME ANYTHING ABOUT THE RESULT. SOME OPERATING DO THIS DURING A CONTEXT
			//		SWITCH TO AVOID ONE THREAD 'SUCCEEDING' ON THE RESERVATION OF ANOTHER.
			"stwcx. %4,0,%2\n\t"
			"bne- 1b\n\t"	//IF NO LABELS, USE "bne- -16" IF I AM NOT WRONG.
			"b 3f\n\t"		//BRANCH. IF NO LABELS, USE "bne- +8" IF I AM NOT WRONG.
			"2:\n\t"
			//THIS IS FROM FREEBSD. UNLIKE ELSEWHERE, INCLUDING BOOST AND GLIBC, FREEBSD FAVORS 
			//		CLEARING THE RESERVATION. KEEPING TO THAT JUST IN CASE. THE ORIGINAL COMMENT WAS 
			//		"clear reservation (74xx)"
			"stwcx. %0,0,%2\n\t"
			"3:\n\t"
			//sync IS A FULL MEMORY BARRIER. ALL OUR ATOMICS MUST BE FULL MEMORY BARRIERS ALSO
			"sync\n\t"
			: "=&r" (vReturn), "=m" (*tLocation)
			: "r" (tLocation), "r" (tComparisonValue), "r" (tNewValue), "m" (*tLocation)
			//FREEBSD AND GLIBC USED cr0 HERE. BOOST USED cc SWITCHING TO cc JUST IN CASE.
			//"The special name cc, which specifies that the assembly altered condition flag (you 
			//		almost always should specify it on x86). On platforms that keep multiple sets 
			//		of condition flags as separate registers, it's also possible to name that 
			//		specific register (for instance, on PowerPC, you can specify that you clobber 
			//		cr0)"
			: "cc", "memory"
		);*/
		__asm__ __volatile__
		(
			"sync\n\t"
			"1:\tlwarx %0,0,%2\n\t"
			"cmplw %3,%0\n\t"
			"bne- 2f\n\t"
			"stwcx. %4,0,%2\n\t"
			"bne- 1b\n\t"
			"b 3f\n\t"
			"2:\n\t"
			"stwcx. %0,0,%2\n\t"
			"3:\n\t"
			"sync\n\t"
			: "=&r" (vReturn), "=m" (*tLocation)
			: "r" (tLocation), "r" (tComparisonValue), "r" (tNewValue), "m" (*tLocation)
			: "cc", "memory"
		);
			#else
		assert(false);
			#endif
		#else
		assert(false);
		#endif
	#elif((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__ARM) || \
				(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__ARM64))
		#if(CRX__ARCHITECTURE__SIZE == 32)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		int32_t tTemp;

		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	
		/*__asm__ __volatile__
		(
			THIS IS SIMILAR TO THE POWERPC INSTRUCTION OF SIMILAR NAME WHERE A PAIR OF INSUTRCTIONS
			ARE USED TO CREATE LEVEL 2 ROOMS, ARITIFICAL ATOMICS, AS DEFINED IN MY STANDARD. 
			ARMv8.1, FROM 2023, INTRODUCED LEVEL 1 ROOMS, ATOMICS, AS DEFINED IN MY STANDARD, AND 
			ARM CALLS THE FEATURE LSE, LARGE SYSTEM EXTENSION. THESE WILL NOT BE USED. THE ARM64
			IMPLEMENTATION IN FREEBSD HAS CODE THAT MAKES USE OF LSE, BE CAREFUL.
			
			"While LDAXR has an implicit synchronization, LDREX/STREX  is just an exclusive access 
			to an address. You will need a data barrier such as DMB to make sure all CPUs see the 
			same value."
			
			IT APEARS ALSO THAT ldxr/stxr ARE WITHUOT A BARRIER, WHILE ldaxr/stlxr ARE WITH A
			BARRIER AND IT SEEMS THAT YOU CAN MIX THEM, SUCH AS ldxr/stlxr AND ldaxr/stxr. IT APPEARS
			THAT 'a' STANDS FOR AQUIRE, AND 'l' STANDS FOR RELEASE. IT APPEARS THAT THESE 
			INSTRUCTIONS ARE ON ARM64 ONLY.
			
			IT IS UNCLEAR WHETHER ldrex/strex ARE STILL SUPPORTED ON ARM64. IT SEEMS IT DOES, BUT
			NOT WHEN THE MACHINE IS RUNNING IN 64BIT MODE. THE 64 BIT INSTRUCTIONS CAN STILL BE
			USED WITH 32BIT WIDE DATA.
			
			ARM64, AKA AARCH64, WAS INTRODUCED WITH ARMv8-A.
				#if(CRX__C__OS__INFO__OS != CRX__C__OS__INFO__OS__LINUX)
			"dmb\n"
				#endif
			"1: ldrex  %1,[%2]\n\t"
			"cmp       %1,%3\n\t"
			"bne       2f\n\t"
			/*MINE:  THE FOLLOWING WILL SET %0 TO 0 IF THE STORE OF %4 INTO [%2] SUCCEDED, 
					OTHERWISE IT STORES 1.
			* /
			"strex     %0,%4,[%2]\n\t"
			"cmp       %0,#0\n\t"
			"bne       1b\n" //FOLLOWED BY LABEL, THEREFORE NO '\t'
			"2: \n\t"
				#if(CRX__C__OS__INFO__OS != CRX__C__OS__INFO__OS__LINUX)
			"dmb\n\t"
				#endif
			:"=&r" (tTemp), "=&r" (vReturn)
			:"r" (pInt32), "r" (pCurrentExpectedValue), "r" (pInt32__2)
			:"cc", "memory"
		);*/
		__asm__ __volatile__
		(
			"1: ldrex  %1,[%2]\n\t"
			"cmp       %1,%3\n\t"
			"bne       2f\n\t"
			"strex     %0,%4,[%2]\n\t"
			"cmp       %0,#0\n\t"
			"bne       1b\n"
			"2: \n"
			:"=&r" (tTemp), "=&r" (vReturn)
			:"r" (pInt32), "r" (pCurrentExpectedValue), "r" (pInt32__2)
			:"cc", "memory"
		);

		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
			#elif((CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__OTHER) && \
					(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__ARMCC))
		int32_t tTemp;

		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();

		__asm
		{
			LABEL__START: 
				ldrex  vReturn,[pInt32]
				cmp    vReturn,pCurrentExpectedValue
				bne    LABEL__END
				strex  tTemp,pInt32__2,[pInt32]
				cmp    tTemp,#0
				bne    LABEL__START
			LABEL__END:
		}

		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
			#else
		assert(false);
			#endif
		#elif(CRX__ARCHITECTURE__SIZE == 64)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
			uint32_t tTemp;

			__asm__ __volatile__
			(
				"1: ldaxr	%w0,[%2]\n\t"
				"cmp		%w0,%w3\n\t"
				"b.ne		2f\n\t"
				"stlxr		%w1,%w4,[%2]\n\t"
				"cbnz		%w1,1b\n"
				"2:"
				:"=&r"(tTemp), "=&r"(vReturn)
				:"r" (pInt32), "r" (pCurrentExpectedValue), "r" (pInt32__2)
				:"cc", "memory"
			);
			#else
		assert(false);
			#endif
		#else
		assert(false);
		#endif				
	#elif(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__SPARC)
		#if(CRX__ARCHITECTURE__SIZE == 64)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();

		/*
			cas COMPARES THE TARGET OF THE ADDRESS AT THE FIRST PARAMETER TO THAT VALUE IN THE 
					SECOND PARAMETER AND IF EQUAL, UPDATES THE TARGET OF THE ADDRESS OF THE FIRST 
					PARAMETER WITH THE VALUE IN THE THIRD PARAMETER. IT THEN STORES THE OLD VALUE IN 
					THE TARGET ADDRESS IN THE THIRD PARAMETER.
			WARNING: cas IS FOR BIG ENDIAN. THERE IS ALSO casl FOR LITTLE ENDIAN MEMORY ACCESS.
		*/
		__asm__ __volatile__
		(
			"cas [%4], %2, %0"
			:"=r" (vReturn), "=m" (*pInt32)
			:"r" (pCurrentExpectedValue), "m" (*pInt32), "r" (pInt32), "0" (pInt32__2)
			:"memory"
		);
		
		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
			#else
		assert(false);
			#endif
		#else
		assert(false);
		#endif
	#elif(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__SYSTEM_Z)
		#if((CRX__ARCHITECTURE__SIZE == 32) || (CRX__ARCHITECTURE__SIZE == 64))
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		vReturn = pCurrentExpectedValue;

		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();

		/*
			cs LOADS %2 INTO THE ADDRESS AT %1 IF THE ADDRESS AT %1 IS EQUAL TO %0. IF NOT, %0
					IS UPDATED WITH THE VALUE AT ADDRESS %1
		*/
		__asm__ __volatile__
		(
			"cs %0,%2,%1"
			:"+d" (vReturn), "=Q" (*pInt32)
			:"d" (pInt32__2), "m" (*pInt32)
			:"cc", "memory"
		);

		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
			#else
		assert(false);
			#endif
		#else
		assert(false);
		#endif
	#elif(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__MIPS)
		#if(CRX__ARCHITECTURE__SIZE == 64)
		int32_t tTemp;
		/*
			ll WILL LOAD THE MEMORY LOCATION %5 INTO THE REGISTER %0 AND REMEMBER WHAT THE VALUE IS.
					SC WILL STORE %1 INTO THE MEMORY LOCATION IF THE VALUE HAS NOT CHANGED. %1 IS
					THEN UPDATED WITH VALUE 1 OR 0 DEPENDING ON WHTHER THE OPERATION SUCCEDDED, MEANING
					THE VALUE IN THE MEMORY WAS UPDATED BECAUSE IT DID NOT CHANGE, OR FAILED.
					
			MIPS WILL EXECUTE THE NEXT INSTRUCTION AFTER A BRANCH INSTRUCTION WHETHER THE BRANCHING
					HAPPENS OR NOT. THIS NEXT INSTRUCTION IS CALLED A 'DELAY SLOT'. THEREFORE, IF
					BRANCHING, ONE NEEDS TO INSERT SOME SORT OF NOP AFTER THE BREANCH. THE INSTRUCTION
					beqzl ON THE OTHER HAND WILL NOT EXECUTE THE DELAY SLOT. THE 'l' SRANDS FOR 
					"LIKELY", AND ITS ABSCENSE "NOT LIKELY". NOTICE HOW THE LINE
							"move	%1,%4\n\t"
					IS ALWATS EXECUTED. THIS ALSO INCLUDES WHATEVER GCC INSERTS AFTER THE LABEL '2'. I
					AM HOPING, LIKE GLIBC APPEARS TO THINK, THAT THIS IS SAFE.
					
					HOWEVER, THIS IS NOT EXACTLY WHY beqzl IS USED BELOW. QUOTE:
							"Certain revisions of the R10000 Processor need an LL/SC Workaround
							enabled.  Revisions before 3.0 misbehave on atomic operations, and
							Revs 2.6 and lower deadlock after several seconds due to other errata.

							To quote the R10K Errata:
							  Workaround: The basic idea is to inhibit the four instructions
							  from simultaneously becoming active in R10000. Padding all
							  ll/sc sequences with nops or changing the looping branch in the
							  routines to a branch likely (which is always predicted taken
							  by R10000) will work. The nops should go after the loop, and the
							  number of them should be 28. This number could be decremented for
							  each additional instruction in the ll/sc loop such as the lock
							  modifier(s) between the ll and sc, the looping branch and its
							  delay slot. For typical short routines with one ll/sc loop, any
							  instructions after the loop could also count as a decrement. The
							  nop workaround pollutes the cache more but would be a few cycles
							  faster if all the code is in the cache and the looping branch
							  is predicted not taken." SRC{"\ports\sysdeps\mips\bits\atomic.h" FROM
							  GLIBC OLD VERSION}
					
			GCC FAVORED TO DO .set push AND .set pop UNCONDITIONALLY, MEANING EVEN IF .set mip2 WAS NOT
					USED IN BETWEEEN.
					
			MIPS O32 ABI APPEARS TO ALSO DICTATE 32BIT INSTRUCTIONS ONLY EVEN IF THE TARGET IS 64BIT.
					I CONFIRMED THAT GCC ALIGNS INT64_T TO 8 BYTES ON 64BIT TARGETS WITH THE O32
					ABI. THE O32 ABI IS SELECTED USING THE COMMAND LINE OPTION "-mabi=32"
		*/
		__asm__ __volatile__
		(
			#if(defined(_MIPS_SIM) && (_MIPS_SIM == _ABIO32))
			".set	push\n\t"
			".set	mips2\n\t"
			#endif
			"1:\t"
			"ll	%0,%5\n\t"
			"move	%1,$0\n\t"
			"bne	%0,%3,2f\n\t"
			"move	%1,%4\n\t"
			"sc	%1,%2\n\t"
			#if(defined(_MIPS_ARCH_R10000))
			"beqzl	%1,1b\n"
			#else
			"beqz	%1,1b\n"
			#endif
			#if(defined(_MIPS_SIM) && (_MIPS_SIM == _ABIO32))
			".set	pop\n"
			#endif
			"2:\n\t"
			:"=&r" (vReturn), "=&r" (tTemp), "=m" (*pInt32)
			:"r" (pCurrentExpectedValue), "r" (pInt32__2), "m" (*pInt32)
			:"memory"
		);
		#else
		assert(false);
		#endif
	#else
		assert(false);
	#endif
	}
	else
	{
	#if((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__X86) || \
			(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__AMD64))
		#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__MSVC)
		_asm
		{
			PUSH         ecx
			PUSH         edx
			MOV          ecx,dword ptr [pInt32]
			MOV          edx,dword ptr [pInt32__2]
			MOV          eax,dword ptr [pCurrentExpectedValue]
			CMPXCHG      dword ptr [ecx],edx
			MOV          dword ptr [vReturn], eax
			POP          edx
			POP          ecx
		}
		#elif(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		/*
			__asm__ __volatile__
			(
				"cmpxchgl       %2,%1"     if (EAX == [pInt32])
												[pInt32] = pInt32__2
											else
												EAX = [pInt32]      
			)
		*/
		__asm__ __volatile__
		(
			"cmpxchgl       %2,%1"
			:"=a" (vReturn)
			:"m"  (*pInt32), "r" (pInt32__2), "a" (pCurrentExpectedValue)
			:"memory", "cc"
		);
		#else
		assert(false);
		#endif
	#else
		assert(false);
	#endif
	}
#else
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	vReturn = (int32_t)_InterlockedCompareExchange((PTW32_INTERLOCKED_LPLONG_FOR_FALLBACK)pInt32, 
			(PTW32_INTERLOCKED_LONG_FOR_FALLBACK)pInt32__2, 
			(PTW32_INTERLOCKED_LONG_FOR_FALLBACK)pCurrentExpectedValue);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
	if(OSAtomicCompareAndSwap32Barrier(pCurrentExpectedValue, pInt32__2, pInt32))
		{vReturn = pCurrentExpectedValue;}
	else
		{vReturn = *pInt32;}
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();

	if(atomic_cmpset_32(((uint32_t volatile *)pInt32), ((uint32_t)pCurrentExpectedValue), 
			((uint32_t)pInt32__2)) != 0)
		{vReturn = pCurrentExpectedValue;}
	else
		{vReturn = *pInt32;}

	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__SOLARIS)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = ((int32_t)atomic_cas_32(((uint32_t volatile *)pInt32), ((uint32_t)pCurrentExpectedValue),
			((uint32_t)pInt32__2)));
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	#else
		#if(CRX__C__PTHREAD__PRIVATE__ARE_GNU_SYNC_BUILTINS_AVAILABLE)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = __sync_val_compare_and_swap(pInt32, pCurrentExpectedValue, pInt32__2);
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
		#elif(CRX__C__PTHREAD__PRIVATE__ARE_GNU_ATOMIC_BUILTINS_AVAILABLE)
	vReturn = pCurrentExpectedValue;

	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	__atomic_compare_exchange_n(pInt32, &vReturn, pInt32__2, false, __ATOMIC_RELAXED, 
			__ATOMIC_RELAXED);
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
		#else
	assert(false);
		#endif
	#endif
#endif

	return vReturn;

#if((CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__WATCOM) || \
		(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__OPEN_WATCOM))
	#pragma enable_message (200)
#endif
}
int32_t crx_c_pthread_crx_doInterlockedSyncExchange(int32_t volatile * pInt32, int32_t pInt32__2)
{
#if((CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__WATCOM) || \
		(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__OPEN_WATCOM))
/* Don't report that result is not assigned a value before being referenced */
#	pragma disable_message (200)
#endif
	int32_t vReturn;

#if(CRX__C__PTHREAD__PRIVATE__IS_TO_USE_LOCK_FOR_SYNC)
	crx_c_pthread_crx_private_getLockForSync(pInt32);

	vReturn = *pInt32;
	*pInt32 = pInt32__2;

	crx_c_pthread_crx_private_releaseLockForSync(pInt32);
#elif(CRX__C__PTHREAD__PRIVATE__IS_TO_USE_ASSEMBLY_FOR_ATOMICS)
	if (ptw32_smp_system)
	{
	#if((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__X86) || \
			(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__AMD64))
		#if((CRX__ARCHITECTURE__SIZE == 32) || (CRX__ARCHITECTURE__SIZE == 64))
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__MSVC)
		_asm 
		{
			PUSH         ecx
			MOV          ecx,dword ptr [pInt32]
			MOV          eax,dword ptr [pInt32__2]
			XCHG         dword ptr [ecx],eax
			MOV          dword ptr [vReturn], eax
			POP          ecx
		}
			#elif(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		__asm__ __volatile__
		(
			"xchgl          %2,%1"
			:"=r" (vReturn)
			:"m"  (*pInt32), "0" (pInt32__2)
			:"memory", "cc"
		);
			#else
		assert(false);
			#endif
		#else
		assert(false);
		#endif
	#elif(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__POWER_PC)
		#if(CRX__ARCHITECTURE__SIZE == 64)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		/*__asm__ __volatile
		(
			//ALL OUR ATOMICS MUST BE FULL MEMORY BARRIERS ALSO
			"sync\n"
			//LIKE ABOVE, GLIBC USED A MUTEX HINT, WHICH WE ARE NOT.
			"1:	lwarx %0,0,%2\n\t"
			"stwcx.	%3,0,%2\n\t"
			"bne-	1b\n\t"	//IF NO LABELS, USE "bne- -8" IF I AM NOT WRONG.
			//ALL OUR ATOMICS MUST BE FULL MEMORY BARRIERS ALSO
			"sync\n"
			//FREEBSD USES +m INSTEAD OF =m, WHICH GLIBC USES AN EXTRA INPUT OPERAND LIKE BELOW.
			//		FOR NOW, USING THE GLIBC APPROACH. I AM ASSUMING THAT SOME HOW THE MEMORY 
			//		LOCATION FROM THE THIRD INPUT OPERAND BELOW IS LOADED INTO THE SECOND OUTPUT 
			//		OPERAND BELOW.
			: "=&r" (vReturn), "=m" (*pInt32)
			: "b" (pInt32), "r" (pInt32__2), "m" (*pInt32)
			: "cr0", "memory"
		);*/
		__asm__ __volatile
		(
			"sync\n"
			"1:	lwarx %0,0,%2\n\t"
			"stwcx.	%3,0,%2\n\t"
			"bne-	1b\n\t"
			"sync\n"
			: "=&r" (vReturn), "=m" (*pInt32)
			: "b" (pInt32), "r" (pInt32__2), "m" (*pInt32)
			: "cr0", "memory"
		);
			#else
		assert(false);
			#endif;
		#else
		assert(false);
		#endif
	#elif((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__ARM) || \
				(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__ARM64))
		#if(CRX__ARCHITECTURE__SIZE == 32)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		uint32_t tTemp;
	
		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();

		__asm__ __volatile__
		(
			"1: ldrex	%0,[%3]	\n\t"
			"strex	%1,%2,[%3]	\n\t"
			"teq	%1,#0			\n\t"
			"it	ne					\n\t"
			"bne	1b				\n"
			:"=&r" (vReturn), "=&r" (tTemp)
			:"r" (pInt32__2), "r" (pInt32)
			:"cc", "memory"
		);
		
		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
			#elif((CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__OTHER) && \
					(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__ARMCC))
		//STICKING TO THE FOLLOWING UNTIL ARMASM CODE IS CONFIRMED WORKING
		/*THE FOLLOWING CAN STILL BE NON ATOMIC IN HOPEFULY RARE CASES*/
		vReturn = *pInt32;
		
		while(vReturn != crx_c_pthread_crx_doInterlockedSyncCompareExchange(pInt32, pInt32__2, 
				vReturn))
			{vReturn = *pInt32;}
			#else
		assert(false);
			#endif;
		#elif(CRX__ARCHITECTURE__SIZE == 64)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		uint32_t tTemp;

		__asm__ __volatile__
		(
			"1: ldaxr	%w1,[%2]\n\t"
			"stlxr		%w0,%w3,[%2]\n\t"
			"cbnz		%w0,1b\n"
			:"=&r" (tTemp), "=&r" (vReturn)
			:"r" (pInt32), "r" (pInt32__2)
			:"memory"
		);
			#else
		assert(false);
			#endif;
		#else
		assert(false);
		#endif
	#elif(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__SPARC)
		#if(CRX__ARCHITECTURE__SIZE == 64)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
		/*
			swap APPEARS DEPRECATED IN V9.
		*/
		__asm__ __volatile__
		(
			"swap %0,%1"
			:"=m" (*pInt32), "=r" (vReturn)
			:"m" (*pInt32), "1" (pInt32__2)
			:"memory"
		);
		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
			#else
		assert(false);
			#endif
		#else
		assert(false);
		#endif
	#elif(((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__SYSTEM_Z) && \
			((CRX__ARCHITECTURE__SIZE == 32) || (CRX__ARCHITECTURE__SIZE == 64)) && \
			(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)))
		/*THE FOLLOWING CAN STILL BE NON ATOMIC IN HOPEFULY RARE CASES*/
		vReturn = *pInt32;
		
		while(vReturn != crx_c_pthread_crx_doInterlockedSyncCompareExchange(pInt32, pInt32__2, 
				vReturn))
			{vReturn = *pInt32;}
	#elif(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__MIPS)
		#if(CRX__ARCHITECTURE__SIZE == 64)
		int32_t tTemp;

		__asm__ __volatile__
		(
			#if(defined(_MIPS_SIM) && (_MIPS_SIM == _ABIO32))
			".set	push\n\t"
			".set	mips2\n\t"
			#endif
			"1:\t"
			"ll	%0,%4\n\t"
			"move	%1,%3\n\t"
			"sc	%1,%2\n\t"
			#if(defined(_MIPS_ARCH_R10000))
			"beqzl	%1,1b\n"
			#else
			"beqz	%1,1b\n"
			#endif
			#if(defined(_MIPS_SIM) && (_MIPS_SIM == _ABIO32))
			".set	pop\n"
			#endif
			"2:\n\t"
			:"=&r" (vReturn), "=&r" (tTemp), "=m" (*pInt32)
			:"r" (pInt32__2), "m" (*pInt32)
			:"memory"
		);
		#else
		assert(false);
		#endif
	#else
		assert(false);
	#endif
	}
	else
	{
	#if((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__X86) || \
			(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__AMD64))
		#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__MSVC)
		_asm 
		{
			PUSH         ecx
			PUSH         edx
			MOV          ecx,dword ptr [pInt32]
			MOV          edx,dword ptr [pInt32__2]
			L1:	MOV          eax,dword ptr [ecx]
			CMPXCHG      dword ptr [ecx],edx
			JNZ          L1
			MOV          dword ptr [vReturn], eax
			POP          edx
			POP          ecx
		}
		#elif(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		__asm__ __volatile__
		(
			"0:\n\t"
			"movl           %1,%%eax\n\t"
			"cmpxchgl       %2,%1\n\t"
			"jnz            0b"
			:"=&a" (vReturn)
			:"m"  (*pInt32), "r" (pInt32__2)
			:"memory", "cc"
		);
		#else
		assert(false);
		#endif
	#else
		assert(false);
	#endif
	}
#else
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	vReturn = _InterlockedExchange(pInt32, pInt32__2);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
	/*THE FOLLOWING CAN STILL BE NON ATOMIC IN HOPEFULY RARE CASES*/
	int32_t tLocation = *pInt32;
	
	vReturn = (tLocation == 0 ? 1 : 0);
	
	while(vReturn != tLocation)
	{
		tLocation = *pInt32;

		if(OSAtomicCompareAndSwap32Barrier(tLocation, pInt32__2, pInt32))
			{vReturn = tLocation;}
		else
			{vReturn = *pInt32;}
	}
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
	int32_t tLocation = *pInt32;
	
	vReturn = (tLocation == 0 ? 1 : 0);
	
	while(vReturn != tLocation)
	{
		tLocation = *pInt32;

		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();

		if(atomic_cmpset_32(((uint32_t volatile *)pInt32), ((uint32_t)tLocation), 
				((uint32_t)pInt32__2)) != 0)
			{vReturn = tLocation;}
		else
			{vReturn = *pInt32;}

		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	}
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__SOLARIS)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = ((int32_t)atomic_swap_32(((uint32_t volatile *)pInt32), ((uint32_t)pInt32__2)));
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	#else
		#if(CRX__C__PTHREAD__PRIVATE__ARE_GNU_SYNC_BUILTINS_AVAILABLE)
	int32_t tLocation = *pInt32;
	
	vReturn = (tLocation == 0 ? 1 : 0);
	
	while(vReturn != tLocation)
	{
		tLocation = *pInt32;

		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
		vReturn = __sync_val_compare_and_swap(pInt32, tLocation, pInt32__2);
		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	}
		#elif(CRX__C__PTHREAD__PRIVATE__ARE_GNU_ATOMIC_BUILTINS_AVAILABLE)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = __atomic_exchange_n(pInt32, pInt32__2, __ATOMIC_RELAXED);
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
		#else
	assert(false);
		#endif
	#endif
#endif

	return vReturn;

#if((CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__WATCOM) || \
		(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__OPEN_WATCOM))
	#pragma enable_message (200)
#endif
}

//RETURNS THE NEW VALUE
int32_t crx_c_pthread_crx_doInterlockedSyncDecrement(int32_t volatile * pInt32)
{
#if((CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__WATCOM) || \
		(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__OPEN_WATCOM))
/* Don't report that result is not assigned a value before being referenced */
#pragma disable_message (200)
#endif
	int32_t vReturn;

#if(CRX__C__PTHREAD__PRIVATE__IS_TO_USE_LOCK_FOR_SYNC)
	crx_c_pthread_crx_private_getLockForSync(pInt32);

	vReturn = *pInt32 - 1;
	*pInt32 = vReturn;

	crx_c_pthread_crx_private_releaseLockForSync(pInt32);
#elif(CRX__C__PTHREAD__PRIVATE__IS_TO_USE_ASSEMBLY_FOR_ATOMICS)
	if(ptw32_smp_system)
	{
	#if((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__X86) || \
			(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__AMD64))
		#if((CRX__ARCHITECTURE__SIZE == 32) || (CRX__ARCHITECTURE__SIZE == 64))
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__MSVC)
		/*
			XADD: "Exchanges the first operand (destination operand) with the second operand 
					(source operand), then loads the sum of the two values into the destination 
					operand. The destination operand can be a register or a memory location; the 
					source operand is a register."
		*/
		_asm
		{
			PUSH		ecx
			MOV			ecx, dword ptr [pInt32]
			MOV			eax, 0FFFFFFFFh
			LOCK XADD 	dword ptr [ecx], eax
			DEC			eax
			MOV			dword ptr [vReturn], eax
			POP			ecx
		}
			#elif(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		vReturn = -1;

		__asm__ __volatile__
		(
			"lock\n\t"
			"xaddl	 %0,(%1)"
			:"+r" (vReturn)
			:"r" (pInt32)
			:"memory", "cc"
		);

		--vReturn;
			#else
		assert(false);
			#endif
		#else
		assert(false);
		#endif
	#elif(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__POWER_PC)
		#if(CRX__ARCHITECTURE__SIZE == 64)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		/*__asm__ __volatile__
		(
			//ALL OUR ATOMICS MUST BE FULL MEMORY BARRIERS ALSO
			"sync\n"
			"1:	lwarx	%0,0,%2\n\t"
			//subi IS SIGNED. IT DOES FIRST_PARAMETER = SECOND_PARAMETER - THIRD_PARAMETER.
			//		i STANDS FOR "IMMEDIATE"
			"subi	%0,%0,1\n\t"
			"stwcx.	%0,0,%2\n\t"
			"bne-	1b\n\t"
			//ALL OUR ATOMICS MUST BE FULL MEMORY BARRIERS ALSO
			"sync\n"
			//'b' IS POWER PC SPECIFIC. IT MEANS A BASE REGISTER, LIKE r0-r31, BUT EXCLUDES r0
			:"=&b" (vReturn), "=m" (*pInt32)
			:"b" (pInt32), "m" (*pInt32)
			:"cr0", "memory"
		);*/
		__asm__ __volatile__
		(
			"sync\n"
			"1:	lwarx	%0,0,%2\n\t"
			"subi	%0,%0,1\n\t"
			"stwcx.	%0,0,%2\n\t"
			"bne-	1b\n\t"
			"sync\n"
			:"=&b" (vReturn), "=m" (*pInt32)
			:"b" (pInt32), "m" (*pInt32)
			:"cr0", "memory"
		);
			#else
		assert(false);
			#endif
		#else
		assert(false);
		#endif
	#elif((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__ARM) || \
				(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__ARM64))
		#if(CRX__ARCHITECTURE__SIZE == 32)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		uint32_t tTemp = 0;
		uint32_t tTemp2 = 0;
		
		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();

		__asm__ __volatile__
		(
			"1: ldrex	%0,[%3]\n\t"
			"sub	%1,%0,#1\n\t"
			"strex	%2,%1,[%3]\n\t"
			"cmp	%2,#0\n\t"
			"it	ne\n\t"
			"bne	1b\n"
			:"+r" (vReturn), "=&r" (tTemp), "+r" (tTemp2), "+r" (pInt32)
			:
			:"cc", "memory"
		);
		
		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();

		vReturn = vReturn - 1;
			#elif((CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__OTHER) && \
					(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__ARMCC))
		//STICKING TO THE FOLLOWING UNTIL ARMASM CODE IS CONFIRMED WORKING
		/*THE FOLLOWING CAN STILL BE NON ATOMIC IN HOPEFULY RARE CASES*/
		vReturn = *pInt32;
		
		while(vReturn != crx_c_pthread_crx_doInterlockedSyncCompareExchange(pInt32, vReturn - 1, 
				vReturn))
			{vReturn = *pInt32;}

		vReturn--;
			#else
		assert(false);
			#endif
		#elif(CRX__ARCHITECTURE__SIZE == 64)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		uint32_t tTemp;
		uint32_t tTemp2;

		__asm__ __volatile__
		(
			"1: ldaxr	%w2,[%3]\n\t"
			"sub		%w0,%w2,#1\n\t"
			"stlxr		%w1,%w0,[%3]\n\t"
			"cbnz		%w1,1b\n"
			:"=&r" (tTemp), "=&r" (tTemp2), "=&r" (vReturn)
			:"r" (pInt32)
			:"memory"
		);

		vReturn = vReturn - 1;
			#else
		assert(false);
			#endif
		#else
		assert(false);
		#endif
	#elif(((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__SPARC) && \
					(CRX__ARCHITECTURE__SIZE == 64) && \
					(CRX__INLINE_ASSEMBLY__DIALECT == \
							CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)) || \
			((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__SYSTEM_Z) && \
					((CRX__ARCHITECTURE__SIZE == 32) || (CRX__ARCHITECTURE__SIZE == 64)) && \
					(CRX__INLINE_ASSEMBLY__DIALECT == \
							CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)))
		/*THE FOLLOWING CAN STILL BE NON ATOMIC IN HOPEFULY RARE CASES*/
		vReturn = *pInt32;
		
		while(vReturn != crx_c_pthread_crx_doInterlockedSyncCompareExchange(pInt32, vReturn - 1, 
				vReturn))
			{vReturn = *pInt32;}

		vReturn--;
	#elif(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__MIPS)
		#if(CRX__ARCHITECTURE__SIZE == 64)
		int32_t tTemp;
		
		/*
			addu, UNLIKE add, WILL NOT TRAP ON OVERFLOW.
		*/

		__asm__ __volatile__
		(
			#if(defined(_MIPS_SIM) && (_MIPS_SIM == _ABIO32))
			".set	push\n\t"
			".set	mips2\n\t"
			#endif
			"1:\t"
			"ll	%0,%4\n\t"
			"addiu	%1,%0,$-1\n\t"
			"move	%0,%1\n\t"
			"sc	%1,%2\n\t"
			#if(defined(_MIPS_ARCH_R10000))
			"beqzl	%1,1b\n"
			#else
			"beqz	%1,1b\n"
			#endif
			#if(defined(_MIPS_SIM) && (_MIPS_SIM == _ABIO32))
			".set	pop\n"
			#endif
			"2:\n\t"
			:"=&r" (vReturn), "=&r" (tTemp), "=m" (*pInt32)
			:"r" (pInt32__2), "m" (*pInt32)
			:"memory"
		);
		#else
		assert(false);
		#endif
	#else
		assert(false);
	#endif
	}
	else
	{
	#if((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__X86) || \
			(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__AMD64))
		#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__MSVC)
		_asm
		{
			PUSH		ecx
			MOV			ecx, dword ptr [pInt32]
			MOV			eax, 0FFFFFFFFh
			XADD	 	dword ptr [ecx], eax
			DEC			eax
			MOV			dword ptr [vReturn], eax
			POP			ecx
		}
		#elif(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		vReturn	= -1;

		__asm__ __volati2le__
		(
			"xaddl	 %0,(%1)"
			:"+r" (vReturn)
			:"r" (pInt32)
			:"memory", "cc"
		);

		--vReturn;
		#else
		assert(false);
		#endif
	#else
		assert(false);
	#endif
	}
#else
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	vReturn = _InterlockedDecrement(pInt32);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
	vReturn = OSAtomicDecrement32Barrier(pInt32);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = ((int32_t)atomic_fetchadd_32(((uint32_t volatile *)pInt32), ((uint32_t)(-1))));
	vReturn = vReturn - 1;
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__SOLARIS)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = ((int32_t)atomic_dec_32_nv(((uint32_t volatile *)pInt32)));
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	#else
		#if(CRX__C__PTHREAD__PRIVATE__ARE_GNU_SYNC_BUILTINS_AVAILABLE)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = __sync_fetch_and_add(pInt32, -1);
	vReturn = vReturn - 1;
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
		#elif(CRX__C__PTHREAD__PRIVATE__ARE_GNU_ATOMIC_BUILTINS_AVAILABLE)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = __atomic_fetch_add(pInt32, -1, __ATOMIC_RELAXED);
	vReturn = vReturn - 1;
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
		#else
	assert(false);
		#endif
	#endif
#endif

	return vReturn;
	
#if((CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__WATCOM) || \
		(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__OPEN_WATCOM))
	#pragma enable_message (200)
#endif
}
//RETURNS THE NEW VALUE
int32_t crx_c_pthread_crx_doInterlockedSyncIncrement(int32_t volatile * pInt32)
{
#if((CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__WATCOM) || \
		(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__OPEN_WATCOM))
	/* Don't report that result is not assigned a value before being referenced */
	#pragma disable_message (200)
#endif
	int32_t vReturn;

#if(CRX__C__PTHREAD__PRIVATE__IS_TO_USE_LOCK_FOR_SYNC)
	crx_c_pthread_crx_private_getLockForSync(pInt32);

	vReturn = *pInt32 + 1;
	*pInt32 = vReturn;

	crx_c_pthread_crx_private_releaseLockForSync(pInt32);
#elif(CRX__C__PTHREAD__PRIVATE__IS_TO_USE_ASSEMBLY_FOR_ATOMICS)
	if(ptw32_smp_system)
	{
	#if((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__X86) || \
			(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__AMD64))
		#if((CRX__ARCHITECTURE__SIZE == 32) || (CRX__ARCHITECTURE__SIZE == 64))
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__MSVC)
		_asm
		{
			PUSH		ecx
			MOV			ecx, dword ptr [pInt32]
			MOV			eax, 1
			LOCK XADD 	dword ptr [ecx], eax
			INC			eax
			MOV			dword ptr [vReturn], eax
			POP			ecx
		}
			#elif(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		vReturn = 1;

		__asm__ __volatile__
		(
			"lock\n\t"
			"xaddl	 %0,(%1)"
			:"+r" (vReturn)
			:"r" (pInt32)
			:"memory", "cc"
		);

		++vReturn;
			#else
		assert(false);
			#endif
		#else
		assert(false);
		#endif
	#elif(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__POWER_PC)
		#if(CRX__ARCHITECTURE__SIZE == 64)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		__asm__ __volatile__
		(
			"sync\n"
			"1: lwarx	%0,0,%2\n\t"
			"addi	%0,%0,1\n\t"
			"stwcx.	%0,0,%2\n\t"
			"bne-	1b\n\t"
			"sync\n"
			:"=&b" (vReturn), "=m" (*pInt32)
			:"b" (pInt32), "m" (*pInt32)
			:"cr0", "memory"
		);
			#else
		assert(false);
			#endif
		#else
		assert(false);
		#endif
	#elif((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__ARM) || \
				(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__ARM64))
		#if(CRX__ARCHITECTURE__SIZE == 32)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		uint32_t tTemp = 0;
		uint32_t tTemp2 = 0;
		
		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();

		__asm__ __volatile__
		(
			"1: ldrex	%0,[%3]\n\t"
			"add	%1,%0,#1\n\t"
			"strex	%2,%1,[%3]\n\t"
			"cmp	%2,#0\n\t"
			"it	ne\n\t"
			"bne	1b\n"
			:"+r" (vReturn), "=&r" (tTemp), "+r" (tTemp2), "+r" (pInt32)
			:
			:"cc", "memory"
		);
		
		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();

		vReturn = vReturn + 1;
			#elif((CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__OTHER) && \
					(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__ARMCC))
		//STICKING TO THE FOLLOWING UNTIL ARMASM CODE IS CONFIRMED WORKING
		/*THE FOLLOWING CAN STILL BE NON ATOMIC IN HOPEFULY RARE CASES*/
		vReturn = *pInt32;
		
		while(vReturn != crx_c_pthread_crx_doInterlockedSyncCompareExchange(pInt32, vReturn + 1, 
				vReturn))
			{vReturn = *pInt32;}

		vReturn++;
			#else
		assert(false);
			#endif
		#elif(CRX__ARCHITECTURE__SIZE == 64)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		uint32_t tTemp;
		uint32_t tTemp2;

		__asm__ __volatile__
		(
			"1: ldaxr	%w2,[%3]\n\t"
			"add		%w0,%w2,#1\n\t"
			"stlxr		%w1,%w0,[%3]\n\t"
			"cbnz		%w1,1b\n"
			:"=&r" (tTemp), "=&r" (tTemp2), "=&r" (vReturn)
			:"r" (pInt32)
			:"memory"
		);

		vReturn = vReturn + 1;
			#else
		assert(false);
			#endif
		#else
		assert(false);
		#endif
	#elif(((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__SPARC) && \
					(CRX__ARCHITECTURE__SIZE == 64) && \
					(CRX__INLINE_ASSEMBLY__DIALECT == \
							CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)) || \
			((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__SYSTEM_Z) && \
					((CRX__ARCHITECTURE__SIZE == 32) || (CRX__ARCHITECTURE__SIZE == 64)) && \
					(CRX__INLINE_ASSEMBLY__DIALECT == \
							CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)))
		/*THE FOLLOWING CAN STILL BE NON ATOMIC IN HOPEFULY RARE CASES*/
		vReturn = *pInt32;
		
		while(vReturn != crx_c_pthread_crx_doInterlockedSyncCompareExchange(pInt32, vReturn + 1, 
				vReturn))
			{vReturn = *pInt32;}

		vReturn++;
	#elif(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__MIPS)
		#if(CRX__ARCHITECTURE__SIZE == 64)
		int32_t tTemp;
		
		/*
			addu, UNLIKE add, WILL NOT TRAP ON OVERFLOW.
		*/

		__asm__ __volatile__
		(
			#if(defined(_MIPS_SIM) && (_MIPS_SIM == _ABIO32))
			".set	push\n\t"
			".set	mips2\n\t"
			#endif
			"1:\t"
			"ll	%0,%4\n\t"
			"addiu	%1,%0,$1\n\t"
			"move	%0,%1\n\t"
			"sc	%1,%2\n\t"
			#if(defined(_MIPS_ARCH_R10000))
			"beqzl	%1,1b\n"
			#else
			"beqz	%1,1b\n"
			#endif
			#if(defined(_MIPS_SIM) && (_MIPS_SIM == _ABIO32))
			".set	pop\n"
			#endif
			"2:\n\t"
			:"=&r" (vReturn), "=&r" (tTemp), "=m" (*pInt32)
			:"r" (pInt32__2), "m" (*pInt32)
			:"memory"
		);
		#else
		assert(false);
		#endif
	#else
		assert(false);
	#endif
	}
	else
	{
	#if((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__X86) || \
			(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__AMD64))
		#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__MSVC)
		_asm
		{
			PUSH		ecx
			MOV			ecx, dword ptr [pInt32]
			MOV			eax, 1
			XADD	 	dword ptr [ecx], eax
			INC			eax
			MOV			dword ptr [vReturn], eax
			POP			ecx
		}
		#elif(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		vReturn = 1;

		__asm__ __volatile__
		(
			"xaddl	 %0,(%1)"
			:"+r" (vReturn)
			:"r" (pInt32)
			:"memory", "cc"
		);

		++vReturn;
		#else
		assert(false);
		#endif
	#else
		assert(false);
	#endif
	}
#else
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	vReturn = _InterlockedIncrement(pInt32);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
	vReturn = OSAtomicIncrement32Barrier(pInt32);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = ((int32_t)atomic_fetchadd_32(((uint32_t volatile *)pInt32), ((uint32_t)1)));
	vReturn = vReturn + 1;
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__SOLARIS)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = ((int32_t)atomic_inc_32_nv(((uint32_t volatile *)pInt32)));
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	#else
		#if(CRX__C__PTHREAD__PRIVATE__ARE_GNU_SYNC_BUILTINS_AVAILABLE)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = __sync_fetch_and_add(pInt32, 1);
	vReturn = vReturn + 1;
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
		#elif(CRX__C__PTHREAD__PRIVATE__ARE_GNU_ATOMIC_BUILTINS_AVAILABLE)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = __atomic_fetch_add(pInt32, 1, __ATOMIC_RELAXED);
	vReturn = vReturn + 1;
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
		#else
	assert(false);
		#endif
	#endif
#endif

	return vReturn;
	
#if((CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__WATCOM) || \
		(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__OPEN_WATCOM))
	#pragma enable_message (200)
#endif
}

//RETURNS THE OLD VALUE
int32_t crx_c_pthread_crx_doInterlockedSyncExchangeAdd(int32_t volatile * pInt32, int32_t pInt32__2)
{
#if((CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__WATCOM) || \
		(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__OPEN_WATCOM))
/* Don't report that result is not assigned a value before being referenced */
#pragma disable_message (200)
#endif
	int32_t vReturn;

#if(CRX__C__PTHREAD__PRIVATE__IS_TO_USE_LOCK_FOR_SYNC)
	crx_c_pthread_crx_private_getLockForSync(pInt32);

	vReturn = *pInt32;
	*pInt32 = vReturn + pInt32__2;

	crx_c_pthread_crx_private_releaseLockForSync(pInt32);
#elif(CRX__C__PTHREAD__PRIVATE__IS_TO_USE_ASSEMBLY_FOR_ATOMICS)
	if(ptw32_smp_system)
	{
	#if((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__X86) || \
			(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__AMD64))
		#if((CRX__ARCHITECTURE__SIZE == 32) || (CRX__ARCHITECTURE__SIZE == 64))
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__MSVC)
		_asm
		{
			PUSH		ecx
			MOV			ecx, dword ptr [pInt32]
			MOV			eax, dword ptr [pInt32__2]
			LOCK XADD 	dword ptr [ecx], eax
			MOV			dword ptr [vReturn], eax
			POP			ecx
		}
			#elif(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		__asm__ __volatile__
		(
			"lock\n\t"
			"xaddl	 %0,(%1)"
			:"=r" (vReturn)
			:"r" (pInt32), "0" (pInt32__2)
			:"memory", "cc"
		);
			#else
		assert(false);
			#endif
		#else
		assert(false);
		#endif
	#elif(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__POWER_PC)
		#if(CRX__ARCHITECTURE__SIZE == 64)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		int32_t tTemp;

		__asm__ __volatile__
		(
			"sync\n"
			"1: lwarx	%0,0,%3\n\t"
			"add	%1,%0,%4\n\t"
			"stwcx.	%1,0,%3\n\t"
			"bne-	1b\n\t"
			"sync\n"
			:"=&b" (vReturn), "=&r" (tTemp), "=m" (*pInt32)
			:"b" (pInt32), "r" (pInt32__2), "m" (*pInt32)
			:"cr0", "memory"
		);
			#else
		assert(false);
			#endif
		#else
		assert(false);
		#endif
	#elif((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__ARM) || \
			(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__ARM64))
		#if(CRX__ARCHITECTURE__SIZE == 32)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		uint32_t tTemp = 0;
		uint32_t tTemp2 = 0;

		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();

		__asm__ __volatile__
		(
			"1: ldrex	%0,[%3]\n\t"
			"add	%1,%0,%4\n\t"
			"strex	%2,%1,[%3]\n\t"
			"cmp	%2,#0\n\t"
			"it	ne\n\t"
			"bne	1b\n"
			:"+r" (vReturn), "=&r" (tTemp), "+r" (tTemp2), "+r" (pInt32), "+r" (pInt32__2)
			:
			:"cc", "memory"
		);

		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
			#elif((CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__OTHER) && \
					(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__ARMCC))
		//STICKING TO THE FOLLOWING UNTIL ARMASM CODE IS CONFIRMED WORKING
		/*THE FOLLOWING CAN STILL BE NON ATOMIC IN HOPEFULY RARE CASES*/
		vReturn = *pInt32;
		
		while(vReturn != crx_c_pthread_crx_doInterlockedSyncCompareExchange(pInt32, 
				vReturn + pInt32__2, vReturn))
			{vReturn = *pInt32;}
			#else
		assert(false);
			#endif
		#elif(CRX__ARCHITECTURE__SIZE == 64)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		uint32_t tTemp;
		uint32_t tTemp2;

		__asm__ __volatile__
		(
			"1: ldaxr	%w2,[%3]\n\t"
			"add		%w0,%w2,%w4\n\t"
			"stlxr		%w1,%w0,[%3]\n\t"
			"cbnz		%w1,1b\n"
			:"=&r" (tTemp), "=&r" (tTemp2), "=&r" (vReturn)
			:"r" (pInt32), "r" (pInt32__2)
			:"memory"
		);
			#else
		assert(false);
			#endif
		#else
		assert(false);
		#endif
	#elif(((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__SPARC) && \
					(CRX__ARCHITECTURE__SIZE == 64) && \
					(CRX__INLINE_ASSEMBLY__DIALECT == \
							CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)) || \
			((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__SYSTEM_Z) && \
					((CRX__ARCHITECTURE__SIZE == 32) || (CRX__ARCHITECTURE__SIZE == 64)) && \
					(CRX__INLINE_ASSEMBLY__DIALECT == \
							CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)))
		/*THE FOLLOWING CAN STILL BE NON ATOMIC IN HOPEFULY RARE CASES*/
		vReturn = *pInt32;
		
		while(vReturn != crx_c_pthread_crx_doInterlockedSyncCompareExchange(pInt32, 
				vReturn + pInt32__2, vReturn))
			{vReturn = *pInt32;}
	#elif(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__MIPS)
		#if(CRX__ARCHITECTURE__SIZE == 64)
		int32_t tTemp;
		
		/*
			addu, UNLIKE add, WILL NOT TRAP ON OVERFLOW.
		*/

		__asm__ __volatile__
		(
			#if(defined(_MIPS_SIM) && (_MIPS_SIM == _ABIO32))
			".set	push\n\t"
			".set	mips2\n\t"
			#endif
			"1:\t"
			"ll	%0,%4\n\t"
			"addu	%1,%0,%3\n\t"
			"sc	%1,%2\n\t"
			#if(defined(_MIPS_ARCH_R10000))
			"beqzl	%1,1b\n"
			#else
			"beqz	%1,1b\n"
			#endif
			#if(defined(_MIPS_SIM) && (_MIPS_SIM == _ABIO32))
			".set	pop\n"
			#endif
			"2:\n\t"
			:"=&r" (vReturn), "=&r" (tTemp), "=m" (*pInt32)
			:"r" (pInt32__2), "m" (*pInt32)
			:"memory"
		);
		#else
		assert(false);
		#endif
	#else
		assert(false);
	#endif
	}
	else
	{
	#if((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__X86) || \
			(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__AMD64))
		#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__MSVC)
		_asm
		{
			PUSH		ecx
			MOV			ecx, dword ptr [pInt32]
			MOV			eax, dword ptr [pInt32__2]
			XADD	 	dword ptr [ecx], eax
			MOV			dword ptr [vReturn], eax
			POP			ecx
		}
		#elif(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		__asm__ __volatile__
		(
			"xaddl	 %0,(%1)"
			:"=r" (vReturn)
			:"r" (pInt32), "0" (pInt32__2)
			:"memory", "cc"
		);
		#else
		assert(false);
		#endif
	#else
		assert(false);
	#endif
	}
#else
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	vReturn = _InterlockedExchangeAdd(pInt32, pInt32__2);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
	vReturn = OSAtomicAdd32Barrier(pInt32__2, pInt32);
	vReturn = vReturn - pInt32__2;
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = ((int32_t)atomic_fetchadd_32(((uint32_t volatile *)pInt32), ((uint32_t)pInt32__2)));
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__SOLARIS)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = ((int32_t)atomic_add_32_nv(((uint32_t volatile *)pInt32), pInt32__2));
	vReturn = vReturn - pInt32__2
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	#else
		#if(CRX__C__PTHREAD__PRIVATE__ARE_GNU_SYNC_BUILTINS_AVAILABLE)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = __sync_fetch_and_add(pInt32, pInt32__2);
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
		#elif(CRX__C__PTHREAD__PRIVATE__ARE_GNU_ATOMIC_BUILTINS_AVAILABLE)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = __atomic_fetch_add(pInt32, pInt32__2, __ATOMIC_RELAXED);
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
		#else
	assert(false);
		#endif
	#endif
#endif

	return vReturn;
	
#if((CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__WATCOM) || \
		(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__OPEN_WATCOM))
	#pragma enable_message (200)
#endif
}

//RETURNS THE OLD VALUE
int32_t crx_c_pthread_crx_doInterlockedSyncExchangeOr(int32_t volatile * pInt32, int32_t pInt32__2)
{
	/*WE NEED THE OLD VALUE, WHICH IS WHY THE CPU INSTRUCTION lock or CAN NOT BE USED ON X86*/
#if((CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__WATCOM) || \
		(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__OPEN_WATCOM))
	/* Don't report that result is not assigned a value before being referenced */
#	pragma disable_message (200)
#endif
	int32_t vReturn;

#if(CRX__C__PTHREAD__PRIVATE__IS_TO_USE_LOCK_FOR_SYNC)
	crx_c_pthread_crx_private_getLockForSync(pInt32);

	vReturn = *pInt32;
	*pInt32 = ((int32_t)(((uint32_t)vReturn) | ((uint32_t)pInt32__2)));

	crx_c_pthread_crx_private_releaseLockForSync(pInt32);
#elif(CRX__C__PTHREAD__PRIVATE__IS_TO_USE_ASSEMBLY_FOR_ATOMICS)
	if(ptw32_smp_system)
	{
	#if((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__X86) || \
			(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__AMD64))
		#if((CRX__ARCHITECTURE__SIZE == 32) || (CRX__ARCHITECTURE__SIZE == 64))
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__MSVC)
		/*
		_asm
		{
			PUSH 			ebx
			PUSH			ecx
			PUSH         	edx
			//prefetchw 	byte ptr [pInt32]
			MOV     		ebx, dword ptr [pInt32]
			MOV				eax, dword ptr [ebx]
			MOV				edx, dword ptr [pInt32__2]
			//npad			3
			JUMP_POINT:
			MOV     		ecx, eax
			OR				ecx, edx
			LOCK CMPXCHG 	DWORD PTR [ebx], ecx
			JNE     		SHORT JUMP_POINT
			MOV				DWORD PTR [vReturn], eax
			POP				edx
			POP				ecx
			POP				ebx
		}
		*/
		_asm
		{
			PUSH 			ebx
			PUSH			ecx
			PUSH         	edx
			MOV     		ebx, dword ptr [pInt32]
			MOV				eax, dword ptr [ebx]
			MOV				edx, dword ptr [pInt32__2]
			JUMP_POINT:
			MOV     		ecx, eax
			OR				ecx, edx
			LOCK CMPXCHG 	dword ptr [ebx], ecx
			JNE     		SHORT JUMP_POINT
			MOV				dword ptr [vReturn], eax
			POP				edx
			POP				ecx
			POP				ebx
		}
			#elif(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		int32_t tTemp;

		//CHANGED "%=:\n\t"  TO "0:\n\t"
		//CHANGED "jne %=b\n\t" TO "jne 0b\n\t"
		__asm__ __volatile__
		(
			"0:\n\t"
			"mov     		%3,%%ecx\n\t"
			"or				%4,%%ecx \n\t"
			"lock cmpxchgl 	%%ecx,%1\n\t"
			"jne     		0b\n\t"
			"mov    		%3,%0"
			:"+r" (vReturn), "+m" (*pInt32)
			:"r"  (pInt32), "a" (*pInt32), "r" (pInt32__2), "r" (tTemp)
			:"ecx", "memory", "cc"
		);
			#else
		assert(false);
			#endif
		#else
		assert(false);
		#endif
	#elif(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__POWER_PC)
		#if(CRX__ARCHITECTURE__SIZE == 64)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		int32_t tTemp;

		__asm__ __volatile__
		(
			"sync\n"
			"1: lwarx	%0,0,%3\n\t"
			"or		%1,%0,%4\n\t"
			"stwcx.	%1,0,%3\n\t"
			"bne-	1b\n\t"
			"sync\n"
			:"=&b" (vReturn), "=&r" (tTemp), "=m" (*pInt32)
			:"b" (pInt32), "r" (pInt32__2), "m" (*pInt32)
			:"cr0", "memory"
		);
			#else
		assert(false);
			#endif
		#else
		assert(false);
		#endif
	#elif((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__ARM) || \
			(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__ARM64))
		#if(CRX__ARCHITECTURE__SIZE == 32)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		uint32_t tTemp = 0;
		uint32_t tTemp2 = 0;
		
		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();

		__asm__ __volatile__
		(
			"1: ldrex	%0,[%3]\n\t"
			"orr	%1,%0,%4\n\t"
			"strex	%2,%1,[%3]\n\t"
			"cmp	%2,#0\n\t"
			"it	ne\n\t"
			"bne	1b\n\t"
			:"+r" (vReturn), "=&r" (tTemp), "+r" (tTemp2), "+r" (pInt32), "+r" (pInt32__2)
			: 
			:"cc", "memory"
		);
		
		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
			#elif((CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__OTHER) && \
					(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__ARMCC))
		//STICKING TO THE FOLLOWING UNTIL ARMASM CODE IS CONFIRMED WORKING
		/*THE FOLLOWING CAN STILL BE NON ATOMIC IN HOPEFULY RARE CASES*/
		vReturn = *pInt32;
		
		while(vReturn != crx_c_pthread_crx_doInterlockedSyncCompareExchange(pInt32, 
				((int32_t)(((uint32_t)vReturn) | ((uint32_t)pInt32__2))), vReturn))
			{vReturn = *pInt32;}
			#else
		assert(false);
			#endif
		#elif(CRX__ARCHITECTURE__SIZE == 64)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		uint32_t tTemp;
		uint32_t tTemp2;

		__asm__ __volatile__
		(
			"1: ldaxr	%w2,[%3]\n\t"
			"orr		%w0,%w2,%w4\n\t"
			"stlxr		%w1,%w0,[%3]\n\t"
			"cbnz		%w1,1b\n"
			:"=&r" (tTemp), "=&r" (tTemp2), "=&r" (vReturn)
			:"r" (pInt32), "r" (pInt32__2)
			:"memory"
		);
			#else
		assert(false);
			#endif
		#else
		assert(false);
		#endif
	#elif(((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__SPARC) && \
					(CRX__ARCHITECTURE__SIZE == 64) && \
					(CRX__INLINE_ASSEMBLY__DIALECT == \
							CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)) || \
			((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__SYSTEM_Z) && \
					((CRX__ARCHITECTURE__SIZE == 32) || (CRX__ARCHITECTURE__SIZE == 64)) && \
					(CRX__INLINE_ASSEMBLY__DIALECT == \
							CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)))
		/*THE FOLLOWING CAN STILL BE NON ATOMIC IN HOPEFULY RARE CASES*/
		vReturn = *pInt32;
		
		while(vReturn != crx_c_pthread_crx_doInterlockedSyncCompareExchange(pInt32, 
				((int32_t)(((uint32_t)vReturn) | ((uint32_t)pInt32__2))), vReturn))
			{vReturn = *pInt32;}
	#elif(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__MIPS)
		#if(CRX__ARCHITECTURE__SIZE == 64)
		int32_t tTemp;

		__asm__ __volatile__
		(
			#if(defined(_MIPS_SIM) && (_MIPS_SIM == _ABIO32))
			".set	push\n\t"
			".set	mips2\n\t"
			#endif
			"1:\t"
			"ll	%0,%4\n\t"
			"or	%1,%0,%3\n\t"
			"sc	%1,%2\n\t"
			#if(defined(_MIPS_ARCH_R10000))
			"beqzl	%1,1b\n"
			#else
			"beqz	%1,1b\n"
			#endif
			#if(defined(_MIPS_SIM) && (_MIPS_SIM == _ABIO32))
			".set	pop\n"
			#endif
			"2:\n\t"
			:"=&r" (vReturn), "=&r" (tTemp), "=m" (*pInt32)
			:"r" (pInt32__2), "m" (*pInt32)
			:"memory"
		);
		#else
		assert(false);
		#endif
	#else
		assert(false);
	#endif
	}
	else
	{
	#if((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__X86) || \
			(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__AMD64))
		#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__MSVC)
		/*
		_asm
		{
			PUSH 			ebx
			PUSH			ecx
			PUSH         	edx
			//prefetchw 	byte ptr [pInt32]
			MOV     		ebx, dword ptr [pInt32]
			MOV				eax, dword ptr [ebx]
			MOV				edx, dword ptr [pInt32__2]
			//npad			3
			JUMP_POINT2:
			MOV     		ecx, eax
			OR				ecx, edx
			CMPXCHG 		dword ptr [ebx], ecx
			JNE     		SHORT JUMP_POINT2
			MOV				dword ptr [vReturn], eax
			POP				edx
			POP				ecx
			POP				ebx
		}
		*/
		_asm
		{
			PUSH 			ebx
			PUSH			ecx
			PUSH         	edx
			MOV     		ebx, dword ptr [pInt32]
			MOV				eax, dword ptr [ebx]
			MOV				edx, dword ptr [pInt32__2]
			JUMP_POINT2:
			MOV     		ecx, eax
			OR				ecx, edx
			CMPXCHG 		dword ptr [ebx], ecx
			JNE     		SHORT JUMP_POINT2
			MOV				dword ptr [vReturn], eax
			POP				edx
			POP				ecx
			POP				ebx
		}
		#elif(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		int32_t tTemp;

		//CHANGED "%=:\n\t"  TO "0:\n\t"
		//CHANGED "jne %=b\n\t" TO "jne 0b\n\t"
		__asm__ __volatile__
		(
			"0:\n\t"
			"mov     		%3,%%ecx\n\t"
			"or				%4,%%ecx\n\t"
			"cmpxchgl 		%%ecx,%1\n\t"
			"jne     		0b\n\t"
			"mov    		%3,%0"
			:"+r" (vReturn), "+m" (*pInt32)
			:"r"  (pInt32), "a" (*pInt32), "r" (pInt32__2), "r" (tTemp)
			:"ecx", "memory", "cc"
		);
		#else
		assert(false);
		#endif
	#else
		assert(false);
	#endif
	}
#else
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	vReturn = InterlockedOr(&pInt32, pInt32__2);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
	vReturn = OSAtomicOr32OrigBarrier(((uint32_t)pInt32__2), ((uint32_t volatile *)pInt32));
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
	/*THE FOLLOWING CAN STILL BE NON ATOMIC IN HOPEFULY RARE CASES*/
	int32_t tLocation = *pInt32;
	
	vReturn = (tLocation == 0 ? 1 : 0);
	
	while(vReturn != tLocation)
	{
		tLocation = *pInt32;

		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();

		if(atomic_cmpset_32(((uint32_t volatile *)pInt32), ((uint32_t)tLocation), 
				((uint32_t)tLocation) | ((uint32_t)pInt32__2)) != 0)
			{vReturn = tLocation;}
		else
			{vReturn = *pInt32;}

		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	}
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__SOLARIS)
	int32_t tLocation = *pInt32;
	
	vReturn = (tLocation == 0 ? 1 : 0);
	
	while(vReturn != tLocation)
	{
		tLocation = *pInt32;

		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
		vReturn = ((int32_t)atomic_cas_32(((uint32_t volatile *)pInt32), ((uint32_t)tLocation),
				((uint32_t)tLocation) | ((uint32_t)pInt32__2)));
		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	}
	#else
		#if(CRX__C__PTHREAD__PRIVATE__ARE_GNU_SYNC_BUILTINS_AVAILABLE)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = __sync_fetch_and_or(pInt32, pInt32__2);
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
		#elif(CRX__C__PTHREAD__PRIVATE__ARE_GNU_ATOMIC_BUILTINS_AVAILABLE)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = __atomic_fetch_or(pInt32, pInt32__2, __ATOMIC_RELAXED);
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
		#else
	assert(false);
		#endif
	#endif
#endif

	return vReturn;

#if((CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__WATCOM) || \
		(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__OPEN_WATCOM))
	#pragma enable_message (200)
#endif
}
//RETURNS THE OLD VALUE
int32_t crx_c_pthread_crx_doInterlockedSyncExchangeAnd(int32_t volatile * pInt32, int32_t pInt32__2)
{
#if((CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__WATCOM) || \
		(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__OPEN_WATCOM))
	/* Don't report that result is not assigned a value before being referenced */
	#pragma disable_message (200)
#endif
	int32_t vReturn;

#if(CRX__C__PTHREAD__PRIVATE__IS_TO_USE_LOCK_FOR_SYNC)
	crx_c_pthread_crx_private_getLockForSync(pInt32);

	vReturn = *pInt32;
	*pInt32 = ((int32_t)(((uint32_t)vReturn) & ((uint32_t)pInt32__2)));

	crx_c_pthread_crx_private_releaseLockForSync(pInt32);
#elif(CRX__C__PTHREAD__PRIVATE__IS_TO_USE_ASSEMBLY_FOR_ATOMICS)
	if(ptw32_smp_system)
	{
	#if((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__X86) || \
			(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__AMD64))
		#if((CRX__ARCHITECTURE__SIZE == 32) || (CRX__ARCHITECTURE__SIZE == 64))
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__MSVC)
		/*
		_asm
		{
			PUSH 			ebx
			PUSH			ecx
			PUSH         	edx
			//prefetchw 	byte ptr [pInt32]
			MOV     		ebx, dword ptr [pInt32]
			MOV				eax, dword ptr [ebx]
			MOV				edx, dword ptr [pInt32__2]
			//npad			3
			JUMP_POINT:
			MOV     		ecx, eax
			AND				ecx, edx
			LOCK CMPXCHG 	dword ptr [ebx], ecx
			JNE     		SHORT JUMP_POINT
			MOV				dword ptr [vReturn], eax
			POP				edx
			POP				ecx
			POP				ebx
		}
		*/
		_asm
		{
			PUSH 			ebx
			PUSH			ecx
			PUSH         	edx
			MOV     		ebx, dword ptr [pInt32]
			MOV				eax, dword ptr [ebx]
			MOV				edx, dword ptr [pInt32__2]
			JUMP_POINT:
			MOV     		ecx, eax
			AND				ecx, edx
			LOCK CMPXCHG 	dword ptr [ebx], ecx
			JNE     		SHORT JUMP_POINT
			MOV				dword ptr [vReturn], eax
			POP				edx
			POP				ecx
			POP				ebx
		}
			#elif(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		int32_t tTemp;

		//CHANGED "%=:\n\t"  TO "0:\n\t"
		//CHANGED "jne %=b\n\t" TO "jne 0b\n\t"
		__asm__ __volatile__
		(
			"0:\n\t"
			"mov     		%3,%%ecx\n\t"
			"and			%4,%%ecx\n\t"
			"lock cmpxchgl 	%%ecx,%1\n\t"
			"jne     		0b\n\t"
			"mov    		%3,%0"
			:"+r" (vReturn), "+m" (*pInt32)
			:"r"  (pInt32), "a" (*pInt32), "r" (pInt32__2), "r" (tTemp)
			:"ecx", "memory", "cc"
		);
			#else
		assert(false);
			#endif
		#else
		assert(false);
		#endif
	#elif(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__POWER_PC)
		#if(CRX__ARCHITECTURE__SIZE == 64)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		int32_t tTemp;

		__asm__ __volatile__
		(
			"sync\n"
			"1: lwarx	%0,0,%3\n\t"
			"and	%1,%0,%4\n\t"
			"stwcx.	%1,0,%3\n\t"
			"bne-	1b\n\t"
			"sync\n"
			:"=&b" (vReturn), "=&r" (tTemp), "=m" (*pInt32)
			:"b" (pInt32), "r" (pInt32__2), "m" (*pInt32)
			:"cr0", "memory"
		);
			#else
		assert(false);
			#endif
		#else
		assert(false);
		#endif
	#elif((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__ARM) || \
			(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__ARM64))
		#if(CRX__ARCHITECTURE__SIZE == 32)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		uint32_t tTemp = 0;
		uint32_t tTemp2 = 0;
		
		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();

		__asm__ __volatile__
		(
			"1: ldrex	%0,[%3]\n\t"
			"and	%1,%0,%4\n\t"
			"strex	%2,%1,[%3]\n\t"
			"cmp	%2,#0\n\t"
			"it	ne\n\t"
			"bne	1b\n"
			:"+r" (vReturn), "=&r" (tTemp), "+r" (tTemp2), "+r" (pInt32), "+r" (pInt32__2)
			: 
			:"cc", "memory"
		);
		
		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
			#elif((CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__OTHER) && \
					(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__ARMCC))
		//STICKING TO THE FOLLOWING UNTIL ARMASM CODE IS CONFIRMED WORKING
		/*THE FOLLOWING CAN STILL BE NON ATOMIC IN HOPEFULY RARE CASES*/
		vReturn = *pInt32;
		
		while(vReturn != crx_c_pthread_crx_doInterlockedSyncCompareExchange(pInt32, 
				((int32_t)(((uint32_t)vReturn) & ((uint32_t)pInt32__2))), vReturn))
			{vReturn = *pInt32;}
			#else
		assert(false);
			#end
		#elif(CRX__ARCHITECTURE__SIZE == 64)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		uint32_t tTemp;
		uint32_t tTemp2;

		__asm__ __volatile__
		(
			"1: ldaxr	%w2,[%3]\n\t"
			"and		%w0,%w2,%w4\n\t"
			"stlxr		%w1,%w0,[%3]\n\t"
			"cbnz		%w1,1b\n"
			:"=&r" (tTemp), "=&r" (tTemp2), "=&r" (vReturn)
			:"r" (pInt32), "r" (pInt32__2)
			:"memory"
		);
			#else
		assert(false);
			#endif
		#else
		assert(false);
		#endif		
	#elif(((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__SPARC) && \
					(CRX__ARCHITECTURE__SIZE == 64) && \
					(CRX__INLINE_ASSEMBLY__DIALECT == \
							CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)) || \
			((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__SYSTEM_Z) && \
					((CRX__ARCHITECTURE__SIZE == 32) || (CRX__ARCHITECTURE__SIZE == 64)) && \
					(CRX__INLINE_ASSEMBLY__DIALECT == \
							CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)))
		/*THE FOLLOWING CAN STILL BE NON ATOMIC IN HOPEFULY RARE CASES*/
		vReturn = *pInt32;
		
		while(vReturn != crx_c_pthread_crx_doInterlockedSyncCompareExchange(pInt32, 
				((int32_t)(((uint32_t)vReturn) & ((uint32_t)pInt32__2))), vReturn))
			{vReturn = *pInt32;}
	#elif(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__MIPS)
		#if(CRX__ARCHITECTURE__SIZE == 64)
		int32_t tTemp;

		__asm__ __volatile__
		(
			#if(defined(_MIPS_SIM) && (_MIPS_SIM == _ABIO32))
			".set	push\n\t"
			".set	mips2\n\t"
			#endif
			"1:\t"
			"ll	%0,%4\n\t"
			"and	%1,%0,%3\n\t"
			"sc	%1,%2\n\t"
			#if(defined(_MIPS_ARCH_R10000))
			"beqzl	%1,1b\n"
			#else
			"beqz	%1,1b\n"
			#endif
			#if(defined(_MIPS_SIM) && (_MIPS_SIM == _ABIO32))
			".set	pop\n"
			#endif
			"2:\n\t"
			:"=&r" (vReturn), "=&r" (tTemp), "=m" (*pInt32)
			:"r" (pInt32__2), "m" (*pInt32)
			:"memory"
		);
		#else
		assert(false);
		#endif
	#else
		assert(false);
	#endif
	}
	else
	{
	#if((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__X86) || \
			(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__AMD64))
		#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__MSVC)
		/*
		_asm
		{
			PUSH 			ebx
			PUSH			ecx
			PUSH         	edx
			//prefetchw 	byte ptr [pInt32]
			MOV     		ebx, dword ptr [pInt32]
			MOV				eax, dword ptr [ebx]
			MOV				edx, dword ptr [pInt32__2]
			//npad			3
			JUMP_POINT2:
			MOV     		ecx, eax
			AND				ecx, edx
			LOCK CMPXCHG 	dword ptr [ebx], ecx
			JNE     		SHORT JUMP_POINT2
			MOV				dword ptr [vReturn], eax
			POP				edx
			POP				ecx
			POP				ebx
		}
		*/
		_asm
		{
			PUSH 			ebx
			PUSH			ecx
			PUSH         	edx
			MOV     		ebx, dword ptr [pInt32]
			MOV				eax, dword ptr [ebx]
			MOV				edx, dword ptr [pInt32__2]
			JUMP_POINT2:
			MOV     		ecx, eax
			AND				ecx, edx
			LOCK CMPXCHG 	dword ptr [ebx], ecx
			JNE     		SHORT JUMP_POINT2
			MOV				dword ptr [vReturn], eax
			POP				edx
			POP				ecx
			POP				ebx
		}
		#elif(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		int32_t tTemp;
	
		//CHANGED "%=:\n\t"  TO "0:\n\t"
		//CHANGED "jne %=b\n\t" TO "jne 0b\n\t"
		__asm__ __volatile__
		(
			"0:\n\t"
			"mov     		%3,%%ecx\n\t"
			"and			%4,%%ecx\n\t"
			"lock cmpxchgl 	%%ecx,%1\n\t"
			"jne     		0b\n\t"
			"mov    		%3,%0"
			:"+r" (vReturn), "+m" (*pInt32)
			:"r"  (pInt32), "a" (*pInt32), "r" (pInt32__2), "r" (tTemp)
			:"ecx", "memory", "cc"
		);
		#else
		assert(false);
		#endif
	#else
		assert(false);
	#endif
	}
#else
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	vReturn = InterlockedAnd(&pInt32, pInt32__2);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
	vReturn = OSAtomicAnd32OrigBarrier(((uint32_t)pInt32__2), ((uint32_t volatile *)pInt32));
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
	/*THE FOLLOWING CAN STILL BE NON ATOMIC IN HOPEFULY RARE CASES*/
	int32_t tLocation = *pInt32;
	
	vReturn = (tLocation == 0 ? 1 : 0);
	
	while(vReturn != tLocation)
	{
		tLocation = *pInt32;

		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();

		if(atomic_cmpset_32(((uint32_t volatile *)pInt32), ((uint32_t)tLocation), 
				((uint32_t)tLocation) & ((uint32_t)pInt32__2)) != 0)
			{vReturn = tLocation;}
		else
			{vReturn = *pInt32;}

		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	}
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__SOLARIS)
	int32_t tLocation = *pInt32;
	
	vReturn = (tLocation == 0 ? 1 : 0);
	
	while(vReturn != tLocation)
	{
		tLocation = *pInt32;

		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
		vReturn = ((int32_t)atomic_cas_32(((uint32_t volatile *)pInt32), ((uint32_t)tLocation),
				((uint32_t)tLocation) & ((uint32_t)pInt32__2)));
		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	}
	#else
		#if(CRX__C__PTHREAD__PRIVATE__ARE_GNU_SYNC_BUILTINS_AVAILABLE)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = __sync_fetch_and_and(pInt32, pInt32__2);
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
		#elif(CRX__C__PTHREAD__PRIVATE__ARE_GNU_ATOMIC_BUILTINS_AVAILABLE)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = __atomic_fetch_and(pInt32, pInt32__2, __ATOMIC_RELAXED);
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
		#else
	assert(false);
		#endif
	#endif
#endif

	return vReturn;

#if((CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__WATCOM) || \
		(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__OPEN_WATCOM))
	#pragma enable_message (200)
#endif
}


int64_t crx_c_pthread_crx_doInterlockedSyncCompareExchange2(int64_t volatile * pInt64,
		int64_t pInt64__2, int64_t pCurrentExpectedValue)
{
#if((CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__WATCOM) || \
		(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__OPEN_WATCOM))
	/* Don't report that result is not assigned a value before being referenced */
#	pragma disable_message (200)
#endif
	int64_t vReturn;

#if(CRX__C__PTHREAD__PRIVATE__IS_TO_USE_LOCK_FOR_SYNC)
	crx_c_pthread_crx_private_getLockForSync2(pInt64);

	vReturn = *pInt64;
	
	if(vReturn == pCurrentExpectedValue)
		{*pInt64 = pInt64__2;}

	crx_c_pthread_crx_private_releaseLockForSync2(pInt64);
#elif(CRX__C__PTHREAD__PRIVATE__IS_TO_USE_ASSEMBLY_FOR_ATOMICS)
	if(ptw32_smp_system)
	{
	#if((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__X86) || \
			(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__AMD64))
		#if(CRX__ARCHITECTURE__SIZE == 32)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__MSVC)
		_asm
		{
			PUSH			eax
			PUSH			ebx
			PUSH			ecx
			PUSH			edx
			PUSH			esi
			MOV				esi, [pInt64]
			MOV				ebx, dword ptr [pInt64__2 + 0]
			MOV				ecx, dword ptr [pInt64__2 + 4]
			MOV				eax, dword ptr [pCurrentExpectedValue + 0]
			MOV				edx, dword ptr [pCurrentExpectedValue + 4]
			LOCK CMPXCHG8B 	qword ptr [esi]
			MOV				dword ptr [vReturn + 0], eax
			MOV				dword ptr [vReturn + 4], edx
			POP				esi
			POP				edx
			POP				ecx
			POP				ebx
			POP				eax
		}
			#elif(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		//REFERENCE: https://stackoverflow.com/questions/6756985/correct-way-to-wrap-cmpxchg8b-in-gcc-inline-assembly-32-bits
		
		/* THE FOLLOWING SIMPLY RETURNS A FLAG.
		__asm__ __volatile__
		(
			/ *"push %%ebx\n\t" // -fPIC uses ebx, so save it* /
			"mov %5,%%ebx\n\t" // load ebx with needed value
			"lock\n\t"
			"cmpxchg8b %0\n\t" // perform CAS operation
			"setz %%al\n\t" // eax potentially modified anyway
			"movzx %%al,%1\n\t" // store result of comparison in 'vRetun'
			/ *"pop %%ebx\n\t" // restore ebx * /
			: "+m" (*pInt64), "=r" (vRetun)
			: "d" ((uint32_t)(((uint64_t)pCurrentExpectedValue) >> 32)), 
					"a" ((uint32_t)(((uint64_t)pCurrentExpectedValue) & 0xffffffff)), 
					"c" ((uint32_t)(((uint64_t)pInt64__2) >> 32)), 
					"r" ((uint32_t)(((uint64_t)pInt64__2) & 0xffffffff))
			: "ebx", "flags", "memory"
		);*/
		
		union {int64_t s; struct {uint32_t l; uint32_t h;};} ret2;//UNSURE IF SYNTAX COMPATIBLE WITH OLDER GCC
		uint32_t * ret2A = &ret2.l;
		uint32_t * ret2B = &ret2.h;

		/*
		__asm__ __volatile__
		(
			"mov %6,%%ebx\n\t" // load ebx with needed value
			"lock\n\t"
			"cmpxchg8b %0\n\t" // perform CAS operation
			"mov %%eax,%1\n\t" //
			"mov %%edx,%2\n\t" //
			: "+m" (*pInt64), "+m" (*ret2A), "+m" (*ret2B)
			: "d" ((uint32_t)(((uint64_t)pCurrentExpectedValue) >> 32)), 
					"a" ((uint32_t)(((uint64_t)pCurrentExpectedValue) & 0xffffffff)), 
					"c" ((uint32_t)(((uint64_t)pInt64__2) >> 32)), 
					"r" ((uint32_t)(((uint64_t)pInt64__2) & 0xffffffff))
			: "ebx", "flags", "memory"
		);
		*/

		__asm__ __volatile__
		(
			"mov %6,%%ebx\n\t"
			"lock\n\t"
			"cmpxchg8b %0\n\t"
			"mov %%eax,%1\n\t"
			"mov %%edx,%2\n\t"
			: "+m" (*pInt64), "+m" (*ret2A), "+m" (*ret2B)
			: "d" ((uint32_t)(((uint64_t)pCurrentExpectedValue) >> 32)), 
					"a" ((uint32_t)(((uint64_t)pCurrentExpectedValue) & 0xffffffff)), 
					"c" ((uint32_t)(((uint64_t)pInt64__2) >> 32)), 
					"r" ((uint32_t)(((uint64_t)pInt64__2) & 0xffffffff))
			: "ebx", "flags", "memory", "cc"
		);
		
		vReturn = ret2.s;
			#else
		assert(false);
			#endif
		#elif(CRX__ARCHITECTURE__SIZE == 64)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__MSVC)
		_asm
		{
			PUSH         rax
			PUSH         rcx
			PUSH         rbx
			MOV          rcx,qword ptr [pInt64]
			MOV          rbx,qword ptr [pInt64__2]
			MOV          rax,qword ptr [pCurrentExpectedValue]
			LOCK CMPXCHG qword ptr [rcx],rbx
			MOV          qword ptr [vReturn], rax
			POP          rbx
			POP          rcx
			POP          rax
		}
			#elif(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		__asm__ __volatile__
		(
			"lock\n\t"
			"cmpxchgq      %2,(%1)"
			:"=a" (vReturn)
			:"r"  (pInt64), "r" (pInt64__2), "a" (pCurrentExpectedValue)
			:"memory", "cc"
		);
			#else
		assert(false);
			#endif
		#else
		assert(false);
		#endif
	#elif(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__POWER_PC)
		#if(CRX__ARCHITECTURE__SIZE == 64)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		uint64_t * tLocation = ((uint64_t *)pInt64);
		uint64_t tComparisonValue = *((uint64_t *)(&pCurrentExpectedValue));
		uint64_t tNewValue = *((uint64_t *)(&pInt64__2));

		/*__asm__ __volatile__
		(
			//sync IS A FULL MEMORY BARRIER. ALL OUR ATOMICS MUST BE FULL MEMORY BARRIERS ALSO
			"sync\n"
			//LOAD WHAT IS REFERENCED BY %2 INTO %0. THIS IS THE 'STORE' SET BY ldarx TO BE REFERED
			//		TO BY stdcx. LATER. NOTE HOW '.' IS PART OF stdcx..
			//		ldarx EXPECTS 64-BIT UNSIGNED
			//A FOURTH PARAMETER CAN BE USED TO TELL THE MACHINE THAT ADJACENT CODE MIGHT BE CHANGED 
			//		OR NOT. IT IS A HINT TO THE MACHINE. THE MACHINE, WITH ldarx, KEEPS AN EYE ON 
			//		THE RESERVED VALUE, WHICH IN THIS CASE IS THE ADDRESS BY mem AND THE VALUE IT 
			//		HOLDS, BUT IS ALSO KEEPS AN EYE ON ADJACENT MEMORY. WHAT THIS MEANS, IF I AM 
			//		NOT WRONG, THAT ANY ADJACENT MEMORY CHANGES ARE STILL TREATED AS CHANGES TO THAT 
			//		RESERVED MEMORY LOCATION AND VALUE. THE HINT PERHAPS HELPS SPEED THINGS UP IN 
			//		THIS CASE. THIS IS NOT SUPPORTED EVERYTWHERE. AVOIDING.
			"1:\tldarx %0,0,%2\n\t"
			//THE 'l' IN cmplw MEANS 'LOGICAL', MEANING UNSIGNED. THE 'w' MEANS WORD, 4 BYTES.
			"cmplw %3,%0\n\t"
			//THE - IN bne- DISABLES THE BRANCH PREDICTION. IT IS CALLED A "STATIC BRANCH HINT". 
			//		I THINK IT IS USEFUL IN JUMPING BACKWARDS, BUT I AM NOT SURE.
			//BOOST SEEMED TO THINK LABELS DO NOT EXIST IF _AIX IS DEFINED. 
			"bne- 2f\n\t"	//IF NO LABELS, USE "bne- +12" IF I AM NOT WRONG.
			//IF %2 STILL EQUALS TO %0, MEANING THE 'STORE', COMMIT %1 INTO %2, ELSE SET A FLAG
			//		YOU MUST ALWAYS COMMIT TO THE SAME LOCATION WHEN USING THE CORRESPONDING ldarx
			//		IN OTHER WORDS, %2 IN THIS CASE. HOWEVER, IF YOU SIMPLY NEED TO CLEAR THE 
			//		RESERVATION YOU CAN COMMIT INTO A HARMLESS LOCATION PROVIDED THAT YOU DO NOT
			//		ASSUME ANYTHING ABOUT THE RESULT. SOME OPERATING DO THIS DURING A CONTEXT
			//		SWITCH TO AVOID ONE THREAD 'SUCCEEDING' ON THE RESERVATION OF ANOTHER.
			"stdcx. %4,0,%2\n\t"
			"bne- 1b\n\t"	//IF NO LABELS, USE "bne- -16" IF I AM NOT WRONG.
			"b 3f\n"		//BRANCH. IF NO LABELS, USE "bne- +8" IF I AM NOT WRONG.
			"2:\n\t"
			//THIS IS FROM FREEBSD. UNLIKE ELSEWHERE, INCLUDING BOOST AND GLIBC, FREEBSD FAVORS 
			//		CLEARING THE RESERVATION. KEEPING TO THAT JUST IN CASE. THE ORIGINAL COMMENT WAS 
			//		"clear reservation (74xx)"
			"stdcx. %0,0,%2\n"
			"3:\n\t"
			//sync IS A FULL MEMORY BARRIER. ALL OUR ATOMICS MUST BE FULL MEMORY BARRIERS ALSO
			"sync\n\t"
			: "=&r" (vReturn), "=m" (*tLocation)
			: "r" (tLocation), "r" (tComparisonValue), "r" (tNewValue), "m" (*tLocation)
			//FREEBSD AND GLIBC USED cr0 HERE. BOOST USED cc SWITCHING TO cc JUST IN CASE.
			//"The special name cc, which specifies that the assembly altered condition flag (you 
			//		almost always should specify it on x86). On platforms that keep multiple sets 
			//		of condition flags as separate registers, it's also possible to name that 
			//		specific register (for instance, on PowerPC, you can specify that you clobber 
			//		cr0)"
			: "cc", "memory"
		);*/
		__asm__ __volatile__
		(
			"sync\n"
			"1:\tldarx %0,0,%2\n\t"
			"cmpld %3,%0\n\t"
			"bne- 2f\n\t"
			"stdcx. %4,0,%2\n\t"
			"bne- 1b\n\t"
			"b 3f\n"
			"2:\n\t"
			"stdcx. %0, 0, %2\n"
			"3:\n\t"
			"sync\n\t"
			: "=&r" (vReturn), "=m" (*tLocation)
			: "r" (tLocation), "r" (tComparisonValue), "r" (tNewValue), "m" (*tLocation)
			: "cc", "memory"
		);
			#else
			assert(false);
			#endif
		#else
		assert(false);
		#endif
	#elif((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__ARM) || \
				(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__ARM64))
		#if(CRX__ARCHITECTURE__SIZE == 32)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		uint32_t tTemp;	
		int64_t tTemp2;

		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();

		/*__asm__ __volatile__
		(
			/*
				IT WAS DIFFICULT TO FIND DOCUMENTATION ON %Q AND %R, BUT IT SEEMS THAT THEY WILL 
						AUTOMATICALLY SPREAD A 8BYTE VALUE INTO TWO ADJACENT 4 BYTE REGISTERS WITH 
						THE CORRECT REGISTER NUMERING. THERE IS A RESTRICTION ON WHETHER THE FIRST 
						ONE SHOULD BE AN EVEN OR ODD REGISTER. 
						%Q IS FOR THE LOW BITS IT SEEMS, BUT THIS MIGHT BE FOR LITTLE ENDIAN.
				ldrexd WAS INTRODUCED WITH ARMv6K, NOT ARMv6. ARMv6K WAS INTRODUCED IT SEEMS WITH 
						ARM1136JF-S REV1
			* /
				#if(CRX__C__OS__INFO__OS != CRX__C__OS__INFO__OS__LINUX)
			"dmb\n"
				#endif
			"1: ldrexd   %Q2,%R2,[%3]\n\t"
			"ldrd     %Q1,%R1,[%4]\n\t"
			"teq      %Q2,%Q1\n\t"
			"bne      2f\n\t"
			"teq      %R2,%R1\n\t"
			"bne      2f\n\t"
			"strexd   %0,%Q5,%R5,[%3]\n\t"
			"eorseq   %0,#1\n\t"
			"beq      1b\n"
			"2:\n\t"
				#if(CRX__C__OS__INFO__OS != CRX__C__OS__INFO__OS__LINUX)
			"dmb\n"
				#endif
			:"=&r" (tTemp), "=&r" (vReturn), "=&r" (tTemp2)
			:"r" (pInt64), "r" (&pCurrentExpectedValue), "r" (pInt64__2)
			: "cc", "memory"
		);*/
		__asm__ __volatile__
		(
			"1: ldrexd   %Q2,%R2,[%3]\n\t"
			"ldrd     %Q1,%R1,[%4]\n\t"
			"teq      %Q2,%Q1\n\t"
			"bne      2f\n\t"
			"teq      %R2,%R1\n\t"
			"bne      2f\n\t"
			"strexd   %0,%Q5,%R5,[%3]\n\t"
			"eorseq   %0,#1\n\t"
			"beq      1b\n"
			"2: \n"
			:"=&r" (tTemp), "=&r" (vReturn), "=&r" (tTemp2)
			:"r" (pInt64), "r" (&pCurrentExpectedValue), "r" (pInt64__2)
			: "cc", "memory"
		);
			#elif((CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__OTHER) && \
					(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__ARMCC))
		/*
			BIG DIFFERENCE BETWEEN THIS SYNTAX, AND MICROSOFT'S IS THAT YOU CAN NOT REFER TO 
					REGISTER NAMES, AND YOU DO NOT NEED TO PREPARE THE ASSEMBLY CONTEXT. NO PUSHING
					AND POPING OF REGISTERS IS REQUIRED.
			COMPILER IMPLICITLY DECLARES VARIABLE R0 TO R12 AND r0 AND r12 AS SIGNED INT32, HOWEVER
					THEY DO NOT NECESSARILY MAP TO THE REGISTERS IMPLIED BY THEIR NAME, AND IT IS
					BETTER TO EXPLICTLY DECLARE THEM TO AVOID COMPILER WARNING.
			THE FOLLOWING IS FOR LITTLE ENDIAN
			UNSURE IF LABELS CAN BE NUMERIC
			UNSURE IF __asm ACTS AS A COMPILER BARRIER OR NOT
		*/
		//uint32_t tTemp__h = 0;  	IN CASE COMPILER PICKS WRONG REGISTER
		//uint32_t tTemp__l = 0;	IN CASE COMPILER PICKS WRONG REGISTER
		register uint32_t tTemp__h __asm("r2") = 0;
		register uint32_t tTemp__l __asm("r3") = 0;
		uint32_t tTemp2 = 0;
		register uint32_t tCurrentExpectedValue__h __asm("r4") = 
				((uint32_t)(((uint64_t)pCurrentExpectedValue) >> 32));
		register uint32_t tCurrentExpectedValue__l __asm("r5") = 
				((uint32_t)(((uint64_t)pCurrentExpectedValue) & 0xFFFFFFFF));
		register uint32_t tNewValue__h __asm("r6") = ((uint32_t)(((uint64_t)pInt64__2) >> 32));
		register uint32_t tNewValue__l __asm("r7") = 
				((uint32_t)(((uint64_t)pInt64__2) & 0xFFFFFFFF));

		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();

		__asm
		{
			LABEL__START:
				ldrexd	tTemp__l, tTmp__h,[pInt64]
				teq		tTemp__l, tCurrentExpectedValue__l
				bne		LABEL__DONE
				teq		tTemp__h, tCurrentExpectedValue__h
				bne		LABEL__DONE
				strexd	tTemp2, tNewValue__l,tNewValue__h,[pInt64]
				eorseq	tTemp2, #1
				beq     LABEL__START
			LABEL__DONE:
		};

		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
		
		vReturn = ((int64_t)((((uint64_t)tCurrentExpectedValue__h) << 32) |
				(((uint64_t)tCurrentExpectedValue__l))));
			#else
		assert(false);
			#endif
		#elif(CRX__ARCHITECTURE__SIZE == 64)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		uint32_t tTemp; //THIS IS INDEED 32BIT.

		/*
			'w' ON ARM64, SUCH AS %w1 MEANS REGISTER FOR 32BIT.
		*/
		__asm__ __volatile__
		(
			"1: ldaxr	%0,[%2]\n\t"
			"cmp		%0,%3\n\t"
			"b.ne		2f\n\t"
			"stlxr		%w1,%4,[%2]\n\t"
			"cbnz		%w1,1b\n"
			"2:"
			:"=&r"(vReturn), "=&r"(tTemp)
			:"r" (pInt64), "r" (pCurrentExpectedValue), "r" (pInt64__2)
			:"cc", "memory"
		);
			#else
			assert(false);
			#endif
		#else
		assert(false);
		#endif
	#elif(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__SPARC)
		#if(CRX__ARCHITECTURE__SIZE == 64)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();

		/*
			casx BEHAVES LIKE cas.
			WARNING: casx IS FOR BIG ENDIAN. THERE IS ALSO casxl FOR LITTLE ENDIAN MEMORY ACCESS.
			I DO NOT KNOW WHY, BUT GLIBC FAVORED EXPLICIT CASTING AS DONE BELOW.
		*/
		__asm__ __volatile__
		(
			"casx [%4],%2,%0"
			:"=r" (vReturn), "=m" (*pInt64)
			:"r" ((int64_t)pCurrentExpectedValue), "m" (*pInt64), "r" (pInt64), 
					"0" ((int64_t)pInt64__2)
			:"memory"
		);

		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
			#else
		assert(false);
			#endif
		#else
		assert(false);
		#endif
	#elif(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__SYSTEM_Z)
		#if(CRX__ARCHITECTURE__SIZE == 32)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		/*
			THE FOLLOWING IS LIKELY BROKEN. I HAVE BEEN UNABLE TO FIND MUCH INFORMATION ON IBM'S S90
					TO DO THE FOLLOWING.
			"cds  r2,r4,%4  \n" WOULD TURN INTO "cds  r2,r4,0(%4)  \n"
			IF rX SYNTAX DOES NOT WORK FOR REGISTERS, USE %%rX SYNTAX.
			IF YOU COME BACK TO THIS, TRY USING THE Z80 EMULATOR FOR TESTING.
		*/
		uint32_t tCurrentExpectedValue__h = ((uint32_t)(((uint64_t)pCurrentExpectedValue) >> 32));
		uint32_t tCurrentExpectedValue__l = 
				((uint32_t)(((uint64_t)pCurrentExpectedValue) & 0xFFFFFFFF));
		uint32_t tNewValue__h = ((uint32_t)(((uint64_t)pInt64__2) >> 32));
		uint32_t tNewValue__l = ((uint32_t)(((uint64_t)pInt64__2) & 0xFFFFFFFF));

		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();

		__asm__ __volatile__
		(
			"lr   r2,%0\n\t"
			"lr   r3,%1\n\t"
			"lr   r4,%2\n\t"
			"lr   r5,%3\n\t"
			"cds  r2,r4,%4\n\t"
			"lr   %0,r2\n\t"
			"lr   %1,r3\n\t"
			"lr   %2,r4\n\t"
			"lr   %3,r5\n"
			:"=&r" (tCurrentExpectedValue__h), "=&r" (tCurrentExpectedValue__l), 
					"=&r" (tNewValue__h), "=&r" (tNewValue__l), "=Q" (*pInt64)
			:"m" (*pInt64)
			:"r2", "r3", "r4", "r5", "cc", "memory"
		);

		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();

		vReturn = ((int64_t)((((uint64_t)tCurrentExpectedValue__h) << 32) |
				(((uint64_t)tCurrentExpectedValue__l))));
			#else
		assert(false);
			#endif
		#elif(CRX__ARCHITECTURE__SIZE == 64)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		vReturn = pCurrentExpectedValue;
	
		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();

		/*
			csg WORKS LIKE cs
		*/
		__asm__ __volatile__
		(
			"csg %0,%2,%1"
			:"+d" (vReturn), "=Q" (*pInt64)
			:"d" ((int64_t)pInt64__2), "m" (*pInt64)
			:"cc", "memory"
		);

		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
			#else
		assert(false);
			#endif
		#else
		assert(false);
		#endif
	#elif(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__MIPS)
		#if(CRX__ARCHITECTURE__SIZE == 64)
		int64_t tTemp; //GLIBC FAVORS 32BIT INSTEAD FOR SOME REASON.

		__asm__ __volatile__ 
		(
			"1:\t"
			"lld	%0,%5\n\t"
			"move	%1,$0\n\t"
			"bne	%0,%3,2f\n\t"
			"move	%1,%4\n\t"
			"scd	%1,%2\n\t"
			#if(defined(_MIPS_ARCH_R10000))
			"beqzl	%1,1b\n"
			#else
			"beqz	%1,1b\n"
			#endif
			"2:\n\t"
			:"=&r" (vReturn), "=&r" (tTemp), "=m" (*pInt64)
			:"r" (pCurrentExpectedValue), "r" (pInt64__2), "m" (*pInt64)
			:"memory"
		);
		#else
		assert(false);
		#endif
	#else
		assert(false);
	#endif
	}
	else
	{
	#if((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__X86) || \
			(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__AMD64))
		#if(CRX__ARCHITECTURE__SIZE == 32)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__MSVC)
		_asm
		{
			PUSH			eax
			PUSH			ebx
			PUSH			ecx
			PUSH			edx
			PUSH			esi
			MOV				esi, [pInt64]
			MOV				ebx, dword ptr [pInt64__2 + 0]
			MOV				ecx, dword ptr [pInt64__2 + 4]
			MOV				eax, dword ptr [pCurrentExpectedValue + 0]
			MOV				edx, dword ptr [pCurrentExpectedValue + 4]
			LOCK CMPXCHG8B 	qword ptr [esi]
			MOV				dword ptr [vReturn + 0], eax
			MOV				dword ptr [vReturn + 4], edx
			POP				esi
			POP				edx
			POP				ecx
			POP				ebx
			POP				eax
		}
			#elif(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		union {int64_t s; struct {uint32_t l; uint32_t h;};} ret2;//UNSURE IF SYNTAX COMPATIBLE WITH OLDER GCC
		uint32_t * ret2A = &ret2.l;
		uint32_t * ret2B = &ret2.h;

		/*
		__asm__ __volatile__
		(
			"mov %6,%%ebx\n\t" // load ebx with needed value
			"cmpxchg8b %0\n\t" // perform CAS operation
			"mov %%eax,%1\n\t" //
			"mov %%edx,%2\n\t" //
			: "+m" (*pInt64), "+m" (*ret2A), "+m" (*ret2B)
			: "d" ((uint32_t)(((uint64_t)pCurrentExpectedValue) >> 32)), 
					"a" ((uint32_t)(((uint64_t)pCurrentExpectedValue) & 0xffffffff)), 
					"c" ((uint32_t)(((uint64_t)pInt64__2) >> 32)), 
					"r" ((uint32_t)(((uint64_t)pInt64__2) & 0xffffffff))
			: "ebx", "flags", "memory"
		);
		*/

		__asm__ __volatile__
		(
			"mov %6,%%ebx\n\t"
			"cmpxchg8b %0\n\t"
			"mov %%eax,%1\n\t"
			"mov %%edx,%2\n\t"
			: "+m" (*pInt64), "+m" (*ret2A), "+m" (*ret2B)
			: "d" ((uint32_t)(((uint64_t)pCurrentExpectedValue) >> 32)), 
					"a" ((uint32_t)(((uint64_t)pCurrentExpectedValue) & 0xffffffff)), 
					"c" ((uint32_t)(((uint64_t)pInt64__2) >> 32)), 
					"r" ((uint32_t)(((uint64_t)pInt64__2) & 0xffffffff))
			: "ebx", "flags", "memory", "cc"
		);
		
		vReturn = ret2.s;
			#else
		assert(false);
			#endif
		#elif(CRX__ARCHITECTURE__SIZE == 64)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__MSVC)
		_asm
		{
			PUSH         rax
			PUSH         rcx
			PUSH         rbx
			MOV          rcx,qword ptr [pInt64]
			MOV          rbx,qword ptr [pInt64__2]
			MOV          rax,qword ptr [pCurrentExpectedValue]
			CMPXCHG      qword ptr [rcx],rbx
			MOV          qword ptr [vReturn], rax
			POP          rbx
			POP          rcx
			POP          rax
		}
			#elif(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		__asm__ __volatile__
		(
			"cmpxchgq      %2,(%1)"
			:"=a" (vReturn)
			:"r"  (pInt64), "r" (pInt64__2), "a" (pCurrentExpectedValue)
			:"memory", "cc"
		);
			#else
		assert(false);
			#endif
		#else
		assert(false);
		#endif
	#else
		assert(false);
	#endif
	}
#else
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	vReturn = _InterlockedCompareExchange64(pInt64, pInt64__2, pCurrentExpectedValue);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
	if(OSAtomicCompareAndSwap64Barrier(pCurrentExpectedValue, pInt64__2, pInt64))
		{vReturn = pCurrentExpectedValue;}
	else
		{vReturn = *pInt64;}
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();

	if(atomic_cmpset_64(((uint64_t volatile *)pInt64), ((uint64_t)pCurrentExpectedValue), 
			((uint64_t)pInt64__2)) != 0)
		{vReturn = pCurrentExpectedValue;}
	else
		{vReturn = *pInt64;}

	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__SOLARIS)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = ((int64_t)atomic_cas_64(((uint64_t volatile *)pInt64), ((uint64_t)pCurrentExpectedValue),
			((uint64_t)pInt64__2)));
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	#else
		#if(CRX__C__PTHREAD__PRIVATE__ARE_GNU_SYNC_BUILTINS_AVAILABLE)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = __sync_val_compare_and_swap(pInt64, pCurrentExpectedValue, pInt64__2);
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
		#elif(CRX__C__PTHREAD__PRIVATE__ARE_GNU_ATOMIC_BUILTINS_AVAILABLE)
	vReturn = pCurrentExpectedValue;

	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	__atomic_compare_exchange_n(pInt64, &vReturn, pInt64__2, false, __ATOMIC_RELAXED, 
			__ATOMIC_RELAXED);
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
		#else
	assert(false);
		#endif
	#endif
#endif

	return vReturn;

#if((CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__WATCOM) || \
		(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__OPEN_WATCOM))
	#pragma enable_message (200)
#endif	
}
int64_t crx_c_pthread_crx_doInterlockedSyncExchange2(int64_t volatile * pInt64,
		int64_t pInt64__2)
{
#if((CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__WATCOM) || \
		(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__OPEN_WATCOM))
	/* Don't report that result is not assigned a value before being referenced */
	#pragma disable_message (200)
#endif
	int64_t vReturn;

#if(CRX__C__PTHREAD__PRIVATE__IS_TO_USE_LOCK_FOR_SYNC)
	crx_c_pthread_crx_private_getLockForSync2(pInt64);

	vReturn = *pInt64;
	*pInt64 = pInt64__2;

	crx_c_pthread_crx_private_releaseLockForSync2(pInt64);
#elif(CRX__C__PTHREAD__PRIVATE__IS_TO_USE_ASSEMBLY_FOR_ATOMICS)
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
		#if(CRX__ARCHITECTURE__SIZE == 32)
	/*THE FOLLOWING CAN STILL BE NON ATOMIC IN HOPEFULY RARE CASES*/
	int64_t tLocation = *pInt64;
	
	vReturn = (tLocation == 0 ? 1 : 0);
	
	while(vReturn != tLocation)
	{
		tLocation = *pInt64;

		vReturn = crx_c_pthread_crx_doInterlockedSyncCompareExchange2(pInt64, pInt64__2, tLocation);
	}
		#elif(CRX__ARCHITECTURE__SIZE == 64)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__MSVC)
	_asm 
	{
		PUSH         rax
		PUSH         rcx
		MOV          rcx,qword ptr [pInt64]
		MOV          rax,qword ptr [pInt64__2]
		XCHG         qword ptr [rcx],rax
		MOV          qword ptr [vReturn], rax
		POP          rcx
		POP          rax
	}
			#elif(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
	__asm__ __volatile__
	(
		"xchgq          %2,%1"
		:"=r" (vReturn)
		:"m"  (*pInt64), "0" (pInt64__2)
		:"memory", "cc"
	);
			#else
	assert(false);
			#endif
		#else
	assert(false);
		#endif
	#elif(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__POWER_PC)
		#if(CRX__ARCHITECTURE__SIZE == 64)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
	/*__asm__ __volatile
	(
		//ALL OUR ATOMICS MUST BE FULL MEMORY BARRIERS ALSO
		"sync\n"
		//LIKE ABOVE, GLIBC USED A MUTEX HINT, WHICH WE ARE NOT.
		"1:	ldarx %0,0,%2\n\t"
		"stdcx.	%3,0,%2\n\t"
		"bne-	1b\n\t"	//IF NO LABELS, USE "bne- -8" IF I AM NOT WRONG.
		//ALL OUR ATOMICS MUST BE FULL MEMORY BARRIERS ALSO
		"sync\n"
		//FREEBSD USES +m INSTEAD OF =m, WHICH GLIBC USES AN EXTRA INPUT OPERAND LIKE BELOW.
		//		FOR NOW, USING THE GLIBC APPROACH. I AM ASSUMING THAT SOME HOW THE MEMORY 
		//		LOCATION FROM THE THIRD INPUT OPERAND BELOW IS LOADED INTO THE SECOND OUTPUT 
		//		OPERAND BELOW.
		: "=&r" (vReturn), "=m" (*pInt64)
		: "b" (pInt64), "r" (pInt64__2), "m" (*pInt64)
		: "cr0", "memory"
	);*/
	__asm__ __volatile
	(
		"sync\n"
		"1:	ldarx %0,0,%2\n\t"
		"stdcx.	%3,0,%2\n\t"
		"bne-	1b\n\t"
		"sync\n"
		: "=&r" (vReturn), "=m" (*pInt64)
		: "b" (pInt64), "r" (pInt64__2), "m" (*pInt64)
		: "cr0", "memory"
	);
			#else
		assert(false);
			#endif
		#else
	assert(false);
		#endif
	#elif((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__ARM) || \
				(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__ARM64))
		#if(CRX__ARCHITECTURE__SIZE == 32)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
	uint32_t tTemp;

	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();

	__asm__ __volatile__
	(
		"1: ldrexd	%Q0,%R0,[%3]\n\t"
		"strexd	%1,%Q2,%R2,[%3]\n\t"
		"teq	%1,#0\n\t"
		"it	ne\n\t"
		"bne	1b\n"
		:"=&r" (vReturn), "=&r" (tTemp)
		:"r"   (pInt64__2), "r"   (pInt64)
		:"cc", "memory"
	);

	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
			#elif((CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__OTHER) && \
					(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__ARMCC))
	//STICKING TO THE FOLLOWING UNTIL ARMASM CODE IS CONFIRMED WORKING
	/*THE FOLLOWING CAN STILL BE NON ATOMIC IN HOPEFULY RARE CASES*/
	vReturn = *pInt64;
	
	while(vReturn != crx_c_pthread_crx_doInterlockedSyncCompareExchange2(pInt64, pInt64__2,
			vReturn))
		{vReturn = *pInt64;}
			#else
			assert(false);
			#endif
		#elif(CRX__ARCHITECTURE__SIZE == 64)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
	uint32_t tTemp; //THIS IS INDEED 32BIT.

	__asm__ __volatile__
	(
		"1: ldaxr	%1,[%2]\n\t"
		"stlxr		%w0,%3,[%2]\n\t"
		"cbnz		%w0,1b\n"
		:"=&r" (tTemp), "=&r" (vReturn)
		:"r" (pInt64), "r" (pInt64__2)
		:"memory"
	);
			#else
			assert(false);
			#endif
		#else
		assert(false);
		#endif
	#elif(((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__SPARC) && \
					(CRX__ARCHITECTURE__SIZE == 64) && \
					(CRX__INLINE_ASSEMBLY__DIALECT == \
							CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)) || \
			((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__SYSTEM_Z) && \
					((CRX__ARCHITECTURE__SIZE == 32) || (CRX__ARCHITECTURE__SIZE == 64)) && \
					(CRX__INLINE_ASSEMBLY__DIALECT == \
							CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)))
	/*THE FOLLOWING CAN STILL BE NON ATOMIC IN HOPEFULY RARE CASES*/
	vReturn = *pInt64;
	
	while(vReturn != crx_c_pthread_crx_doInterlockedSyncCompareExchange2(pInt64, pInt64__2,
			vReturn))
		{vReturn = *pInt64;}
	#elif(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__MIPS)
		#if(CRX__ARCHITECTURE__SIZE == 64)
	int64_t tTemp; //GLIBC FAVORS 32BIT INSTEAD FOR SOME REASON.

	__asm__ __volatile__
	(
			#if(defined(_MIPS_SIM) && (_MIPS_SIM == _ABIO32))
		".set	push\n\t"
		".set	mips2\n\t"
			#endif
		"1:\n"
		"lld	%0,%4\n\t"
		"move	%1,%3\n\t"
		"scd	%1,%2\n\t"
			#if(defined(_MIPS_ARCH_R10000))
		"beqzl	%1,1b\n"
			#else
		"beqz	%1,1b\n"
			#endif
			#if(defined(_MIPS_SIM) && (_MIPS_SIM == _ABIO32))
		".set	pop\n"
			#endif
		"2:\n\t"
		:"=&r" (vReturn), "=&r" (tTemp), "=m" (*pInt64)
		:"r" (pInt64__2), "m" (*pInt64)
		:"memory"
	);
		#else
	assert(false);
		#endif
	#else
	assert(false);
	#endif
#else
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	vReturn = _InterlockedExchange64(pInt64, pInt64__2);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
	/*THE FOLLOWING CAN STILL BE NON ATOMIC IN HOPEFULY RARE CASES*/
	int64_t tLocation = *pInt64;
	
	vReturn = (tLocation == 0 ? 1 : 0);
	
	while(vReturn != tLocation)
	{
		tLocation = *pInt64;

		if(OSAtomicCompareAndSwap64Barrier(tLocation, pInt64__2, pInt64))
			{vReturn = tLocation;}
		else
			{vReturn = *pInt64;}
	}
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
	int64_t tLocation = *pInt64;
	
	vReturn = (tLocation == 0 ? 1 : 0);
	
	while(vReturn != tLocation)
	{
		tLocation = *pInt64;

		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();

		if(atomic_cmpset_64(((uint64_t volatile *)pInt64), ((uint64_t)tLocation), 
				((uint64_t)pInt64__2)) != 0)
			{vReturn = tLocation;}
		else
			{vReturn = *pInt64;}

		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	}
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__SOLARIS)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = ((int64_t)atomic_swap_64(((uint64_t volatile *)pInt64), ((uint64_t)pInt64__2)));
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	#else
		#if(CRX__C__PTHREAD__PRIVATE__ARE_GNU_SYNC_BUILTINS_AVAILABLE)
	int32_t tLocation = *pInt64;
	
	vReturn = (tLocation == 0 ? 1 : 0);
	
	while(vReturn != tLocation)
	{
		tLocation = *pInt64;

		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
		vReturn = __sync_val_compare_and_swap(pInt64, tLocation, pInt64__2);
		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	}
		#elif(CRX__C__PTHREAD__PRIVATE__ARE_GNU_ATOMIC_BUILTINS_AVAILABLE)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = __atomic_exchange_n(pInt64, pInt64__2, __ATOMIC_RELAXED);
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
		#else
	assert(false);
		#endif
	#endif
#endif

	return vReturn;

#if((CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__WATCOM) || \
		(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__OPEN_WATCOM))
	#pragma enable_message (200)
#endif
}

//RETURNS THE NEW VALUE
int64_t crx_c_pthread_crx_doInterlockedSyncDecrement2(int64_t volatile * pInt64)
{
#if((CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__WATCOM) || \
		(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__OPEN_WATCOM))
	/* Don't report that result is not assigned a value before being referenced */
#	pragma disable_message (200)
#endif
	int64_t vReturn;

#if(CRX__C__PTHREAD__PRIVATE__IS_TO_USE_LOCK_FOR_SYNC)
	crx_c_pthread_crx_private_getLockForSync2(pInt64);

	vReturn = *pInt64 - 1;
	*pInt64 = vReturn;

	crx_c_pthread_crx_private_releaseLockForSync2(pInt64);
#elif(CRX__C__PTHREAD__PRIVATE__IS_TO_USE_ASSEMBLY_FOR_ATOMICS)
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
		#if(CRX__ARCHITECTURE__SIZE == 32)
	/*THE FOLLOWING CAN STILL BE NON ATOMIC IN HOPEFULY RARE CASES*/
	vReturn = *pInt64;
	
	while(vReturn != crx_c_pthread_crx_doInterlockedSyncCompareExchange2(pInt64, vReturn - 1, 
			vReturn))
		{vReturn = *pInt64;}

	vReturn--;
		#elif(CRX__ARCHITECTURE__SIZE == 64)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__MSVC)
	_asm
	{
		PUSH		rax
		PUSH		rcx
		MOV			rcx, qword ptr [pInt64]
		MOV			rax, 0FFFFFFFFFFFFFFFFh
		LOCK XADD 	qword ptr [rcx], rax
		DEC			rax
		MOV			qword ptr [vReturn], rax
		POP			rcx
		POP			rax
	}
			#elif(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
	vReturn = -1;

	__asm__ __volatile__
	(
		"lock\n\t"
		"xaddq	 %0,(%1)"
		:"+r" (vReturn)
		:"r" (pInt64)
		:"memory", "cc"
	);

	--vReturn;
			#else
	assert(false);
			#endif
		#else
	assert(false);
		#endif
	#elif(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__POWER_PC)
		#if(CRX__ARCHITECTURE__SIZE == 64)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		/*__asm__ __volatile__
		(
			//ALL OUR ATOMICS MUST BE FULL MEMORY BARRIERS ALSO
			"sync\n"
			"1:	ldarx	%0,0,%2\n\t"
			//subi IS SIGNED. IT DOES FIRST_PARAMETER = SECOND_PARAMETER - THIRD_PARAMETER.
			//		i STANDS FOR "IMMEDIATE"
			"subi	%0,%0,1\n\t"
			"stdcx.	%0,0,%2\n\t"
			"bne-	1b\n\t"
			//ALL OUR ATOMICS MUST BE FULL MEMORY BARRIERS ALSO
			"sync\n"
			//'b' IS POWER PC SPECIFIC. IT MEANS A BASE REGISTER, LIKE r0-r31, BUT EXCLUDES r0
			:"=&b" (vReturn), "=m" (*pInt64)
			:"b" (pInt64), "m" (*pInt64)
			:"cr0", "memory"
		);*/
		__asm__ __volatile__
		(
			"sync\n"
			"1:	ldarx	%0,0,%2\n\t"
			"subi	%0,%0,1\n\t"
			"stdcx.	%0,0,%2\n\t"
			"bne-	1b\n\t"
			"sync\n"
			:"=&b" (vReturn), "=m" (*pInt64)
			:"b" (pInt64), "m" (*pInt64)
			:"cr0", "memory"
		);
			#else
			assert(false);
			#endif
		#else
		assert(false);
		#endif
	#elif((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__ARM) || \
				(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__ARM64))
		#if(CRX__ARCHITECTURE__SIZE == 32)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
	int64_t tTemp;
	uint32_t tTemp2;//THIS IS INDEED 32BIT.

	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();

	__asm__ __volatile__
	(
	    "1:\n\t"
	    "ldrexd	%Q0,%R0,[%3]\n\t"
	    "subs	%Q2,%Q0,#1\n\t"
	    "sbc	%R2,%R0,#0\n\t"
	    "strexd	%1,%Q2,%R2,[%3]\n\t"
	    "teq	%1,#0\n\t"
	    "it	ne\n\t"
	    "bne	1b\n"
	    :"=&r" (tTemp), "=&r" (tTemp2), "=&r" (vReturn)
	    :"r"   (pInt64)
	    :"cc", "memory"
	);

	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
			#elif((CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__OTHER) && \
					(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__ARMCC))
	//STICKING TO THE FOLLOWING UNTIL ARMASM CODE IS CONFIRMED WORKING
	/*THE FOLLOWING CAN STILL BE NON ATOMIC IN HOPEFULY RARE CASES*/
	vReturn = *pInt64;
	
	while(vReturn != crx_c_pthread_crx_doInterlockedSyncCompareExchange2(pInt64, vReturn - 1,
			vReturn))
		{vReturn = *pInt64;}

	vReturn--;
			#else
			assert(false);
			#endif
		#elif(CRX__ARCHITECTURE__SIZE == 64)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
	int64_t tTemp;
	uint32_t tTemp2; //THIS IS INDEED 32BIT.

	__asm__ __volatile__
	(
		"1: ldaxr	%2,[%3]\n\t"
		"sub		%0,%2,#1\n\t"
		"stlxr		%w1,%0,[%3]\n\t"
		"cbnz		%w1,1b\n"
		:"=&r" (vReturn), "=&r" (tTemp2), "=&r" (tTemp)
		:"r" (pInt64)
		:"memory"
	);
			#else
			assert(false);
			#endif
		#else
		assert(false);
		#endif
	#elif(((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__SPARC) && \
					(CRX__ARCHITECTURE__SIZE == 64) && \
					(CRX__INLINE_ASSEMBLY__DIALECT == \
							CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)) || \
			((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__SYSTEM_Z) && \
					((CRX__ARCHITECTURE__SIZE == 32) || (CRX__ARCHITECTURE__SIZE == 64)) && \
					(CRX__INLINE_ASSEMBLY__DIALECT == \
							CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)))
	/*THE FOLLOWING CAN STILL BE NON ATOMIC IN HOPEFULY RARE CASES*/
	vReturn = *pInt64;
	
	while(vReturn != crx_c_pthread_crx_doInterlockedSyncCompareExchange2(pInt64, vReturn - 1,
			vReturn))
		{vReturn = *pInt64;}

	vReturn--;
	#elif(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__MIPS)
		#if(CRX__ARCHITECTURE__SIZE == 64)
	int64_t tTemp; //GLIBC FAVORS 32BIT INSTEAD FOR SOME REASON.

	__asm__ __volatile__
	(
			#if(defined(_MIPS_SIM) && (_MIPS_SIM == _ABIO32))
		".set	push\n\t"
		".set	mips2\n\t"
			#endif
		"1:\t"
		"lld	%0,%4\n\t"
		"daddiu	%1,%0,$-1\n\t"
		"move	%0,%1\n\t"
		"scd	%1,%2\n\t"
			#if(defined(_MIPS_ARCH_R10000))
		"beqzl	%1,1b\n"
			#else
		"beqz	%1,1b\n"
			#endif
			#if(defined(_MIPS_SIM) && (_MIPS_SIM == _ABIO32))
		".set	pop\n"
			#endif
		"2:\n\t"
		:"=&r" (vReturn), "=&r" (tTemp), "=m" (*pInt64)
		:"r" (pInt64__2), "m" (*pInt64)
		:"memory"
	);
		#else
	assert(false);
		#endif
	#else
	assert(false);
	#endif
#else
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	vReturn = _InterlockedDecrement64(pInt64);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
	vReturn = OSAtomicDecrement64Barrier(pInt64);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = ((int64_t)atomic_fetchadd_64(((uint64_t volatile *)pInt64), ((uint64_t)(-1))));
	vReturn = vReturn - 1;
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__SOLARIS)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = ((int64_t)atomic_dec_64_nv(((uint64_t volatile *)pInt64)));
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	#else
		#if(CRX__C__PTHREAD__PRIVATE__ARE_GNU_SYNC_BUILTINS_AVAILABLE)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = __sync_fetch_and_add(pInt64, ((int64_t)(-1)));
	vReturn = vReturn - 1;
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
		#elif(CRX__C__PTHREAD__PRIVATE__ARE_GNU_ATOMIC_BUILTINS_AVAILABLE)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = __atomic_fetch_add(pInt64, ((int64_t)(-1)), __ATOMIC_RELAXED);
	vReturn = vReturn - 1;
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
		#else
	assert(false);
		#endif
	#endif
#endif

	return vReturn;

#if((CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__WATCOM) || \
		(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__OPEN_WATCOM))
	#pragma enable_message (200)
#endif
}
//RETURNS THE NEW VALUE
int64_t crx_c_pthread_crx_doInterlockedSyncIncrement2(int64_t volatile * pInt64)
{
#if((CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__WATCOM) || \
		(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__OPEN_WATCOM))
	/* Don't report that result is not assigned a value before being referenced */
#	pragma disable_message (200)
#endif
	int64_t vReturn;

#if(CRX__C__PTHREAD__PRIVATE__IS_TO_USE_LOCK_FOR_SYNC)
	crx_c_pthread_crx_private_getLockForSync2(pInt64);

	vReturn = *pInt64 + 1;
	*pInt64 = vReturn;

	crx_c_pthread_crx_private_releaseLockForSync2(pInt64);
#elif(CRX__C__PTHREAD__PRIVATE__IS_TO_USE_ASSEMBLY_FOR_ATOMICS)
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
		#if(CRX__ARCHITECTURE__SIZE == 32)
	/*THE FOLLOWING CAN STILL BE NON ATOMIC IN HOPEFULY RARE CASES*/
	vReturn = *pInt64;
	
	while(vReturn != crx_c_pthread_crx_doInterlockedSyncCompareExchange2(pInt64, vReturn + 1, 
			vReturn))
		{vReturn = *pInt64;}

	vReturn++;
		#elif(CRX__ARCHITECTURE__SIZE == 64)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__MSVC)
	_asm
	{
		PUSH		rax
		PUSH		rcx
		MOV			rcx, qword ptr [pInt64]
		MOV			rax, 1
		LOCK XADD 	qword ptr [rcx], rax
		INC			rax
		MOV			qword ptr [vReturn], rax
		POP			rcx
		POP			rax
	}
			#elif(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
	vReturn = 1;

	__asm__ __volatile__
	(
		"lock\n\t"
		"xaddq	 %0,(%1)"
		:"+r" (vReturn)
		:"r" (pInt64)
		:"memory", "cc"
	);

	++vReturn;
			#else
	assert(false);
			#endif
		#else
	assert(false);
		#endif
	#elif(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__POWER_PC)
		#if(CRX__ARCHITECTURE__SIZE == 64)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
		__asm__ __volatile__
		(
			"sync\n"
			"1: ldarx	%0,0,%2\n\t"
			"addi	%0,%0,1\n\t"
			"stdcx.	%0,0,%2\n\t"
			"bne-	1b\n\t"
			"sync\n"
			:"=&b" (vReturn), "=m" (*pInt64)
			:"b" (pInt64), "m" (*pInt64)
			:"cr0", "memory"
		);
			#else
			assert(false);
			#endif
		#else
		assert(false);
		#endif
	#elif((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__ARM) || \
				(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__ARM64))
		#if(CRX__ARCHITECTURE__SIZE == 32)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
	int64_t tTemp;
	uint32_t tTemp2;//THIS IS INDEED 32BIT.

	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();

	__asm__ __volatile__
	(
	    "1:\n"
	    "ldrexd	%Q0,%R0,[%3]\n\t"
	    "adds	%Q2,%Q0,#1\n\t"
	    "adc	%R2,%R0,#0\n\t"
	    "strexd	%1,%Q2,%R2,[%3]\n\t"
	    "teq	%1,#0\n\t"
	    "it	ne\n\t"
	    "bne	1b\n"
	    :"=&r" (tTemp), "=&r" (tTemp2), "=&r" (vReturn)
	    :"r"   (pInt64)
	    :"cc", "memory"
	);

	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
			#elif((CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__OTHER) && \
					(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__ARMCC))
	//STICKING TO THE FOLLOWING UNTIL ARMASM CODE IS CONFIRMED WORKING
	/*THE FOLLOWING CAN STILL BE NON ATOMIC IN HOPEFULY RARE CASES*/
	vReturn = *pInt64;
	
	while(vReturn != crx_c_pthread_crx_doInterlockedSyncCompareExchange2(pInt64, vReturn + 1,
			vReturn))
		{vReturn = *pInt64;}

	vReturn++;
			#else
			assert(false);
			#endif
		#elif(CRX__ARCHITECTURE__SIZE == 64)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
	int64_t tTemp;
	uint32_t tTemp2; //THIS IS INDEED 32BIT.

	__asm__ __volatile__
	(
		"1: ldaxr	%2,[%3]\n\t"
		"add		%0,%2,#1\n\t"
		"stlxr		%w1,%0,[%3]\n\t"
		"cbnz		%w1,1b\n"
		:"=&r" (vReturn), "=&r" (tTemp2), "=&r" (tTemp)
		:"r" (pInt64)
		:"memory"
	);
			#else
			assert(false);
			#endif
		#else
		assert(false);
		#endif
	#elif(((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__SPARC) && \
					(CRX__ARCHITECTURE__SIZE == 64) && \
					(CRX__INLINE_ASSEMBLY__DIALECT == \
							CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)) || \
			((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__SYSTEM_Z) && \
					((CRX__ARCHITECTURE__SIZE == 32) || (CRX__ARCHITECTURE__SIZE == 64)) && \
					(CRX__INLINE_ASSEMBLY__DIALECT == \
							CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)))
	/*THE FOLLOWING CAN STILL BE NON ATOMIC IN HOPEFULY RARE CASES*/
	vReturn = *pInt64;
	
	while(vReturn != crx_c_pthread_crx_doInterlockedSyncCompareExchange2(pInt64, vReturn + 1,
			vReturn))
		{vReturn = *pInt64;}

	vReturn++;
	#elif(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__MIPS)
		#if(CRX__ARCHITECTURE__SIZE == 64)
	int64_t tTemp; //GLIBC FAVORS 32BIT INSTEAD FOR SOME REASON.

	__asm__ __volatile__
	(
			#if(defined(_MIPS_SIM) && (_MIPS_SIM == _ABIO32))
		".set	push\n\t"
		".set	mips2\n\t"
			#endif
		"1:\t"
		"lld	%0,%4\n\t"
		"daddiu	%1,%0,$1\n\t"
		"move	%0,%1\n\t"
		"scd	%1,%2\n\t"
			#if(defined(_MIPS_ARCH_R10000))
		"beqzl	%1,1b\n"
			#else
		"beqz	%1,1b\n"
			#endif
			#if(defined(_MIPS_SIM) && (_MIPS_SIM == _ABIO32))
		".set	pop\n"
			#endif
		"2:\n\t"
		:"=&r" (vReturn), "=&r" (tTemp), "=m" (*pInt64)
		:"r" (pInt64__2), "m" (*pInt64)
		:"memory"
	);
		#else
	assert(false);
		#endif
	#else
	assert(false);
	#endif
#else
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	vReturn = _InterlockedIncrement64(pInt64);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
	vReturn = OSAtomicIncrement64Barrier(pInt64);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = ((int64_t)atomic_fetchadd_64(((uint64_t volatile *)pInt64), ((uint64_t)1)));
	vReturn = vReturn + 1;
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__SOLARIS)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = ((int64_t)atomic_inc_64_nv(((uint64_t volatile *)pInt64)));
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	#else
		#if(CRX__C__PTHREAD__PRIVATE__ARE_GNU_SYNC_BUILTINS_AVAILABLE)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = __sync_fetch_and_add(pInt64, 1);
	vReturn = vReturn + 1;
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
		#elif(CRX__C__PTHREAD__PRIVATE__ARE_GNU_ATOMIC_BUILTINS_AVAILABLE)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = __atomic_fetch_add(pInt64, ((int64_t)1), __ATOMIC_RELAXED);
	vReturn = vReturn + 1;
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
		#else
	assert(false);
		#endif
	#endif
#endif

	return vReturn;

#if((CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__WATCOM) || \
		(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__OPEN_WATCOM))
	#pragma enable_message (200)
#endif
}

//RETURNS THE OLD VALUE
int64_t crx_c_pthread_crx_doInterlockedSyncExchangeAdd2(int64_t volatile * pInt64, int64_t pInt64__2)
{
#if((CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__WATCOM) || \
		(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__OPEN_WATCOM))
	/* Don't report that result is not assigned a value before being referenced */
#	pragma disable_message (200)
#endif
	int64_t vReturn;

#if(CRX__C__PTHREAD__PRIVATE__IS_TO_USE_LOCK_FOR_SYNC)
	crx_c_pthread_crx_private_getLockForSync2(pInt64);

	vReturn = *pInt64;
	*pInt64 = vReturn + pInt64__2;

	crx_c_pthread_crx_private_releaseLockForSync2(pInt64);
#elif(CRX__C__PTHREAD__PRIVATE__IS_TO_USE_ASSEMBLY_FOR_ATOMICS)
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
		#if(CRX__ARCHITECTURE__SIZE == 32)
	/*THE FOLLOWING CAN STILL BE NON ATOMIC IN HOPEFULY RARE CASES*/
	vReturn = *pInt64;
	
	while(vReturn != crx_c_pthread_crx_doInterlockedSyncCompareExchange2(pInt64, 
			vReturn + pInt64__2, vReturn))
		{vReturn = *pInt64;}
		#elif(CRX__ARCHITECTURE__SIZE == 64)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__MSVC)
	_asm
	{
		PUSH		rax
		PUSH		rcx
		MOV			rcx, qword ptr [pInt64]
		MOV			rax, qword ptr [pInt64__2]
		LOCK XADD 	qword ptr [rcx], rax
		MOV			qword ptr [vReturn], rax
		POP			rcx
		POP			rax
	}
			#elif(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
	__asm__ __volatile__
	(
		"lock\n\t"
		"xaddq	 %0,(%1)"
		:"=r" (vReturn)
		:"r" (pInt64), "0" (pInt64__2)
		:"memory", "cc"
	);
			#else
	assert(false);
			#endif
		#else
	assert(false);
		#endif
	#elif(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__POWER_PC)
		#if(CRX__ARCHITECTURE__SIZE == 64)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
	int64_t tTemp;

	__asm__ __volatile__
	(
		"sync\n"
		"1: ldarx	%0,0,%3\n\t"
		"add	%1,%0,%4\n\t"
		"stdcx.	%1,0,%3\n\t"
		"bne-	1b\n\t"
		"sync\n"
		:"=&b" (vReturn), "=&r" (tTemp), "=m" (*pInt64)
		:"b" (pInt64), "r" (pInt64__2), "m" (*pInt64)
		:"cr0", "memory"
	);
			#else
		assert(false);
			#endif
		#else
	assert(false);
		#endif
	#elif((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__ARM) || \
				(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__ARM64))
		#if(CRX__ARCHITECTURE__SIZE == 32)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
	uint64_t tTemp;
	uint32_t tTemp2;//THIS IS INDEED 32BIT.

	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();

	__asm__ __volatile__
	(
	    "1:\n\t"
	    "ldrexd	%Q0,%R0,[%3]\n\t"
	    "adds	%Q2,%Q0,%Q4\n\t"
	    "adc	%R2,%R0,%R4\n\t"
	    "strexd	%1,%Q2,%R2,[%3]\n\t"
	    "teq	%1,#0\n\t"
	    "it	ne\n\t"
	    "bne	1b\n"
	    :"=&r" (vReturn), "=&r" (tTemp2), "=&r" (tTemp)
	    :"r"   (pInt64), "r" (pInt64__2)
	    :"cc", "memory"
	);

	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
			#elif((CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__OTHER) && \
					(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__ARMCC))
	//STICKING TO THE FOLLOWING UNTIL ARMASM CODE IS CONFIRMED WORKING
	/*THE FOLLOWING CAN STILL BE NON ATOMIC IN HOPEFULY RARE CASES*/
	vReturn = *pInt64;
	
	while(vReturn != crx_c_pthread_crx_doInterlockedSyncCompareExchange2(pInt64, 
			vReturn + pInt64__2, vReturn))
		{vReturn = *pInt64;}
			#else
			assert(false);
			#endif
		#elif(CRX__ARCHITECTURE__SIZE == 64)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
	int64_t tTemp;
	uint32_t tTemp2; //THIS IS INDEED 32BIT.

	__asm__ __volatile__
	(
		"1: ldaxr	%2,[%3]\n\t"
		"add		%0,%2,%4\n\t"
		"stlxr		%w1,%0,[%3]\n\t"
		"cbnz		%w1,1b\n"
		:"=&r" (tTemp), "=&r" (tTemp2), "=&r" (vReturn)
		:"r" (pInt64), "r" (pInt64__2)
		:"memory"
	);
			#else
			assert(false);
			#endif
		#else
		assert(false);
		#endif
	#elif(((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__SPARC) && \
					(CRX__ARCHITECTURE__SIZE == 64) && \
					(CRX__INLINE_ASSEMBLY__DIALECT == \
							CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)) || \
			((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__SYSTEM_Z) && \
					((CRX__ARCHITECTURE__SIZE == 32) || (CRX__ARCHITECTURE__SIZE == 64)) && \
					(CRX__INLINE_ASSEMBLY__DIALECT == \
							CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)))
	/*THE FOLLOWING CAN STILL BE NON ATOMIC IN HOPEFULY RARE CASES*/
	vReturn = *pInt64;
	
	while(vReturn != crx_c_pthread_crx_doInterlockedSyncCompareExchange2(pInt64, 
			vReturn + pInt64__2, vReturn))
		{vReturn = *pInt64;}
	#elif(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__MIPS)
		#if(CRX__ARCHITECTURE__SIZE == 64)
	int64_t tTemp; //GLIBC FAVORS 32BIT INSTEAD FOR SOME REASON.

	__asm__ __volatile__
	(
			#if(defined(_MIPS_SIM) && (_MIPS_SIM == _ABIO32))
		".set	push\n\t"
		".set	mips2\n\t"
			#endif
		"1:\t"
		"lld	%0,%4\n\t"
		"daddu	%1,%0,%3\n\t"
		"scd	%1,%2\n\t"
			#if(defined(_MIPS_ARCH_R10000))
		"beqzl	%1,1b\n"
			#else
		"beqz	%1,1b\n"
			#endif
			#if(defined(_MIPS_SIM) && (_MIPS_SIM == _ABIO32))
		".set	pop\n"
			#endif
		"2:\n\t"
		:"=&r" (vReturn), "=&r" (tTemp), "=m" (*pInt64)
		:"r" (pInt64__2), "m" (*pInt64)
		:"memory"
	);
		#else
	assert(false);
		#endif
	#else
	assert(false);
	#endif
#else
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	vReturn = _InterlockedExchangeAdd64(pInt64, pInt64__2);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
	vReturn = OSAtomicAdd64Barrier(pInt64__2, pInt64);
	vReturn = vReturn - pInt64__2;
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = ((int64_t)atomic_fetchadd_64(((uint64_t volatile *)pInt64), ((uint64_t)pInt64__2)));
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__SOLARIS)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = ((int64_t)atomic_add_64_nv(((uint64_t volatile *)pInt64), pInt64__2));
	vReturn = vReturn - pInt64__2
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	#else
		#if(CRX__C__PTHREAD__PRIVATE__ARE_GNU_SYNC_BUILTINS_AVAILABLE)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = __sync_fetch_and_add(pInt64, pInt64__2);
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
		#elif(CRX__C__PTHREAD__PRIVATE__ARE_GNU_ATOMIC_BUILTINS_AVAILABLE)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = __atomic_fetch_add(pInt64, pInt64__2, __ATOMIC_RELAXED);
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
		#else
	assert(false);
		#endif
	#endif
#endif

	return vReturn;

#if((CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__WATCOM) || \
		(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__OPEN_WATCOM))
	#pragma enable_message (200)
#endif
}

//RETURNS THE OLD VALUE
int64_t crx_c_pthread_crx_doInterlockedSyncExchangeOr2(int64_t volatile * pInt64, int64_t pInt64__2)
{
#if((CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__WATCOM) || \
		(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__OPEN_WATCOM))
	/* Don't report that result is not assigned a value before being referenced */
#	pragma disable_message (200)
#endif
	int64_t vReturn;

#if(CRX__C__PTHREAD__PRIVATE__IS_TO_USE_LOCK_FOR_SYNC)
	crx_c_pthread_crx_private_getLockForSync2(pInt64);

	vReturn = *pInt64;
	*pInt64 = ((int64_t)(((uint64_t)vReturn) | ((unit64_t)pInt64__2)));

	crx_c_pthread_crx_private_releaseLockForSync2(pInt64);
#elif(CRX__C__PTHREAD__PRIVATE__IS_TO_USE_ASSEMBLY_FOR_ATOMICS)
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
		#if(CRX__ARCHITECTURE__SIZE == 32)
	/*THE FOLLOWING CAN STILL BE NON ATOMIC IN HOPEFULY RARE CASES*/
	vReturn = *pInt64;
	
	while(vReturn != crx_c_pthread_crx_doInterlockedSyncCompareExchange2(pInt64, 
			((int64_t)(((uint64_t)vReturn) | ((uint64_t)pInt64__2))), vReturn))
		{vReturn = *pInt64;}
		#elif(CRX__ARCHITECTURE__SIZE == 64)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__MSVC)
	_asm
	{
		PUSH 			rax
		PUSH 			rbx
		PUSH			rcx
		PUSH         	rdx
		MOV     		rbx, qword ptr [pInt64]
		MOV				rax, qword ptr [rbx]
		MOV				rdx, qword ptr [pInt64__2]
		JUMP_POINT:
		MOV     		rcx, rax
		OR				rcx, rdx
		LOCK CMPXCHG 	qword ptr [ebx], rcx
		JNE     		SHORT JUMP_POINT
		MOV				qword ptr [vReturn], rax
		POP				rdx
		POP				rcx
		POP				rbx
		POP				rax
	}
			#elif(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
	//CHANGED "%=:\n\t"  TO "0:\n\t"
	//CHANGED "jne %=b\n\t" TO "jne 0b\n\t"
	__asm__ __volatile__
	(
		"0:\n\t"
		"mov     		%3,%%rcx\n\t"
		"or				%4,%%rcx\n\t"
		"lock cmpxchgq 	%%rcx,%1\n\t"
		"jne     		0b\n\t"
		"mov    		%3,%0"
		:"+r" (vReturn), "+m" (*pInt64)
		:"r"  (pInt64), "a" (*pInt64), "r" (pInt64__2)
		:"rcx", "memory", "cc"
	);
			#else
	assert(false);
			#endif
		#else
	assert(false);
		#endif
	#elif(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__POWER_PC)
		#if(CRX__ARCHITECTURE__SIZE == 64)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
	int64_t tTemp;

	__asm__ __volatile__
	(
		"sync\n"
		"1: ldarx	%0,0,%3\n\t"
		"or		%1,%0,%4\n\t"
		"stdcx.	%1,0,%3\n\t"
		"bne-	1b\n\t"
		"sync\n"
		:"=&b" (vReturn), "=&r" (tTemp), "=m" (*pInt64)
		:"b" (pInt64), "r" (pInt64__2), "m" (*pInt64)
		:"cr0", "memory"
	);
			#else
		assert(false);
			#endif
		#else
	assert(false);
		#endif
	#elif((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__ARM) || \
				(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__ARM64))
		#if(CRX__ARCHITECTURE__SIZE == 32)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
	int64_t tTemp;
	uint32_t tTemp2;//THIS IS INDEED 32BIT.

	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();

	__asm__ __volatile__
	(
	    "1:\n\t"
	    "ldrexd	%Q0,%R0,[%3]\n\t"
	    "orr	%Q2,%Q0,%Q4\n\t"
	    "orr	%R2,%R0,%R4\n\t"
	    "strexd	%1,%Q2,%R2,[%3]\n\t"
	    "teq	%1,#0\n\t"
	    "it	ne\n\t"
	    "bne	1b\n"
	    :"=&r" (vReturn), "=&r" (tTemp2), "=&r" (tTemp)
	    :"r"   (pInt64), "r" (pInt64__2)
	    :"cc", "memory"
	);

	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
			#elif((CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__OTHER) && \
					(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__ARMCC))
	//STICKING TO THE FOLLOWING UNTIL ARMASM CODE IS CONFIRMED WORKING
	/*THE FOLLOWING CAN STILL BE NON ATOMIC IN HOPEFULY RARE CASES*/
	vReturn = *pInt64;
	
	while(vReturn != crx_c_pthread_crx_doInterlockedSyncCompareExchange2(pInt64, 
			((int64_t)(((uint64_t)vReturn) | ((uint64_t)pInt64__2))), vReturn))
		{vReturn = *pInt64;}
			#else
			assert(false);
			#endif
		#elif(CRX__ARCHITECTURE__SIZE == 64)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
	int64_t tTemp;
	uint32_t tTemp2; //THIS IS INDEED 32BIT.

	__asm__ __volatile__
	(
		"1: ldaxr	%2,[%3]\n\t"
		"orr		%0,%2,%4\n\t"
		"stlxr		%w1,%0,[%3]\n\t"
		"cbnz		%w1,1b\n"
		:"=&r" (tTemp), "=&r" (tTemp2), "=&r" (vReturn)
		:"r" (pInt64), "r" (pInt64__2)
		:"memory"
	);
			#else
			assert(false);
			#endif
		#else
		assert(false);
		#endif
	#elif(((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__SPARC) && \
					(CRX__ARCHITECTURE__SIZE == 64) && \
					(CRX__INLINE_ASSEMBLY__DIALECT == \
							CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)) || \
			((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__SYSTEM_Z) && \
					((CRX__ARCHITECTURE__SIZE == 32) || (CRX__ARCHITECTURE__SIZE == 64)) && \
					(CRX__INLINE_ASSEMBLY__DIALECT == \
							CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)))
	/*THE FOLLOWING CAN STILL BE NON ATOMIC IN HOPEFULY RARE CASES*/
	vReturn = *pInt64;
	
	while(vReturn != crx_c_pthread_crx_doInterlockedSyncCompareExchange2(pInt64, 
			((int64_t)(((uint64_t)vReturn) | ((uint64_t)pInt64__2))), vReturn))
		{vReturn = *pInt64;}
	#elif(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__MIPS)
		#if(CRX__ARCHITECTURE__SIZE == 64)
	int64_t tTemp; //GLIBC FAVORS 32BIT INSTEAD FOR SOME REASON.

	__asm__ __volatile__
	(
			#if(defined(_MIPS_SIM) && (_MIPS_SIM == _ABIO32))
		".set	push\n\t"
		".set	mips2\n\t"
			#endif
		"1:\t"
		"lld	%0,%4\n\t"
		"dor	%1,%0,%3\n\t"
		"scd	%1,%2\n\t"
			#if(defined(_MIPS_ARCH_R10000))
		"beqzl	%1,1b\n"
			#else
		"beqz	%1,1b\n"
			#endif
			#if(defined(_MIPS_SIM) && (_MIPS_SIM == _ABIO32))
		".set	pop\n"
			#endif
		"2:\n\t"
		:"=&r" (vReturn), "=&r" (tTemp), "=m" (*pInt64)
		:"r" (pInt64__2), "m" (*pInt64)
		:"memory"
	);
		#else
	assert(false);
		#endif
	#else
	assert(false);
	#endif
#else
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	vReturn = _InterlockedOr64(pInt64, pInt64__2);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
	vReturn = OSAtomicOr32OrigBarrier(((uint64_t)pInt64__2), ((uint64_t volatile *)pInt64));
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
	/*THE FOLLOWING CAN STILL BE NON ATOMIC IN HOPEFULY RARE CASES*/
	int64_t tLocation = *pInt64;
	
	vReturn = (tLocation == 0 ? 1 : 0);
	
	while(vReturn != tLocation)
	{
		tLocation = *pInt64;

		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();

		if(atomic_cmpset_64(((uint64_t volatile *)pInt64), ((uint64_t)tLocation), 
				((uint64_t)tLocation) | ((uint64_t)pInt64__2)) != 0)
			{vReturn = tLocation;}
		else
			{vReturn = *pInt64;}

		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	}
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__SOLARIS)
	int64_t tLocation = *pInt64;
	
	vReturn = (tLocation == 0 ? 1 : 0);
	
	while(vReturn != tLocation)
	{
		tLocation = *pInt64;

		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
		vReturn = ((int64_t)atomic_cas_64(((uint64_t volatile *)pInt64), ((uint64_t)tLocation),
				((uint64_t)tLocation) | ((uint64_t)pInt64__2)));
		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	}
	#else
		#if(CRX__C__PTHREAD__PRIVATE__ARE_GNU_SYNC_BUILTINS_AVAILABLE)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = __sync_fetch_and_or(pInt64, pInt64__2);
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
		#elif(CRX__C__PTHREAD__PRIVATE__ARE_GNU_ATOMIC_BUILTINS_AVAILABLE)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = __atomic_fetch_or(pInt64, pInt64__2, __ATOMIC_RELAXED);
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
		#else
	assert(false);
		#endif
	#endif
#endif

	return vReturn;

#if((CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__WATCOM) || \
		(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__OPEN_WATCOM))
	#pragma enable_message (200)
#endif
}
//RETURNS THE OLD VALUE
int64_t crx_c_pthread_crx_doInterlockedSyncExchangeAnd2(int64_t volatile * pInt64, int64_t pInt64__2)
{
#if((CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__WATCOM) || \
		(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__OPEN_WATCOM))
	/* Don't report that result is not assigned a value before being referenced */
#	pragma disable_message (200)
#endif
	int64_t vReturn;

#if(CRX__C__PTHREAD__PRIVATE__IS_TO_USE_LOCK_FOR_SYNC)
	crx_c_pthread_crx_private_getLockForSync2(pInt64);

	vReturn = *pInt64;
	*pInt64 = ((int64_t)(((uint64_t)vReturn) & ((unit64_t)pInt64__2)));

	crx_c_pthread_crx_private_releaseLockForSync2(pInt64);
#elif(CRX__C__PTHREAD__PRIVATE__IS_TO_USE_ASSEMBLY_FOR_ATOMICS)
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
		#if(CRX__ARCHITECTURE__SIZE == 32)
	/*THE FOLLOWING CAN STILL BE NON ATOMIC IN HOPEFULY RARE CASES*/
	vReturn = *pInt64;
	
	while(vReturn != crx_c_pthread_crx_doInterlockedSyncCompareExchange2(pInt64, 
			((int64_t)(((uint64_t)vReturn) & ((uint64_t)pInt64__2))), vReturn))
		{vReturn = *pInt64;}
		#elif(CRX__ARCHITECTURE__SIZE == 64)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__MSVC)
	_asm
	{
		PUSH 			rax
		PUSH 			rbx
		PUSH			rcx
		PUSH         	rdx
		MOV     		rbx, qword ptr [pInt64]
		MOV				rax, qword ptr [rbx]
		MOV				rdx, qword ptr [pInt64__2]
		JUMP_POINT:
		MOV     		rcx, rax
		AND				rcx, rdx
		LOCK CMPXCHG 	qword ptr [rbx], rcx
		JNE     		SHORT JUMP_POINT
		MOV				qword ptr [vReturn], rax
		POP				rdx
		POP				rcx
		POP				rbx
		POP				rax
	}
			#elif(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
	//CHANGED "%=:\n\t"  TO "0:\n\t"
	//CHANGED "jne %=b\n\t" TO "jne 0b\n\t"
	__asm__ __volatile__
	(
		"0:\n\t"
		"mov     		%3,%%rcx\n\t"
		"and			%4,%%rcx\n\t"
		"lock cmpxchgq 	%%rcx,%1\n\t"
		"jne     		0b\n\t"
		"mov    		%3,%0"
		:"+r" (vReturn), "+m" (*pInt64)
		:"r"  (pInt64), "a" (*pInt64), "r" (pInt64__2)
		:"rcx", "memory", "cc"
	);
			#else
	assert(false);
			#endif
		#else
	assert(false);
		#endif
	#elif(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__POWER_PC)
		#if(CRX__ARCHITECTURE__SIZE == 32)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
	int64_t tTemp;

	__asm__ __volatile__
	(
		"sync\n"
		"1: ldarx	%0,0,%3\n\t"
		"and	%1,%0,%4\n\t"
		"stdcx.	%1,0,%3\n\t"
		"bne-	1b\n\t"
		"sync\n"
		:"=&b" (vReturn), "=&r" (tTemp), "=m" (*pInt64)
		:"b" (pInt64), "r" (pInt64__2), "m" (*pInt64)
		:"cr0", "memory"
	);
			#else
		assert(false);
			#endif
		#else
	assert(false);
		#endif
	#elif((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__ARM) || \
				(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__ARM64))
		#if(CRX__ARCHITECTURE__SIZE == 32)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
	int64_t tTemp;
	uint32_t tTemp2;//THIS IS INDEED 32BIT.

	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();

	__asm__ __volatile__
	(
	    "1:\n\t"
	    "ldrexd	%Q0,%R0,[%3]\n\t"
	    "and	%Q2,%Q0,%Q4\n\t"
	    "and	%R2,%R0,%R4\n\t"
	    "strexd	%1,%Q2,%R2,[%3]\n\t"
	    "teq	%1,#0\n\t"
	    "it	ne\n\t"
	    "bne	1b\n"
	    :"=&r" (vReturn), "=&r" (tTemp2), "=&r" (tTemp)
	    :"r"   (pInt64), "r" (pInt64__2)
	    :"cc", "memory"
	);

	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
			#elif((CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__OTHER) && \
					(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__ARMCC))
	//STICKING TO THE FOLLOWING UNTIL ARMASM CODE IS CONFIRMED WORKING
	/*THE FOLLOWING CAN STILL BE NON ATOMIC IN HOPEFULY RARE CASES*/
	vReturn = *pInt64;
	
	while(vReturn != crx_c_pthread_crx_doInterlockedSyncCompareExchange2(pInt64, 
			((int64_t)(((uint64_t)vReturn) & ((uint64_t)pInt64__2))), vReturn))
		{vReturn = *pInt64;}
			#else
			assert(false);
			#endif
		#elif(CRX__ARCHITECTURE__SIZE == 64)
			#if(CRX__INLINE_ASSEMBLY__DIALECT == CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)
	int64_t tTemp;
	uint32_t tTemp2; //THIS IS INDEED 32BIT.

	__asm__ __volatile__
	(
		"1: ldaxr	%2,[%3]\n\t"
		"and		%0,%2,%4\n\t"
		"stlxr		%w1,%0,[%3]\n\t"
		"cbnz		%w1,1b\n"
		:"=&r" (tTemp), "=&r" (tTemp2), "=&r" (vReturn)
		:"r" (pInt64), "r" (pInt64__2)
		:"memory"
	);
			#else
			assert(false);
			#endif
		#else
		assert(false);
		#endif
	#elif(((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__SPARC) && \
					(CRX__ARCHITECTURE__SIZE == 64) && \
					(CRX__INLINE_ASSEMBLY__DIALECT == \
							CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)) || \
			((CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__SYSTEM_Z) && \
					((CRX__ARCHITECTURE__SIZE == 32) || (CRX__ARCHITECTURE__SIZE == 64)) && \
					(CRX__INLINE_ASSEMBLY__DIALECT == \
							CRX__INLINE_ASSEMBLY__DIALECT__EXTENDED_AT_AND_T)))
	/*THE FOLLOWING CAN STILL BE NON ATOMIC IN HOPEFULY RARE CASES*/
	vReturn = *pInt64;
	
	while(vReturn != crx_c_pthread_crx_doInterlockedSyncCompareExchange2(pInt64, 
			((int64_t)(((uint64_t)vReturn) & ((uint64_t)pInt64__2))), vReturn))
		{vReturn = *pInt64;}
	#elif(CRX__ARCHITECTURE__TYPE == CRX__ARCHITECTURE__TYPE__MIPS)
		#if(CRX__ARCHITECTURE__SIZE == 64)
	int64_t tTemp; //GLIBC FAVORS 32BIT INSTEAD FOR SOME REASON.

	__asm__ __volatile__
	(
			#if(defined(_MIPS_SIM) && (_MIPS_SIM == _ABIO32))
		".set	push\n\t"
		".set	mips2\n\t"
			#endif
		"1:\t"
		"lld	%0,%4\n\t"
		"dand	%1,%0,%3\n\t"
		"scd	%1,%2\n\t"
			#if(defined(_MIPS_ARCH_R10000))
		"beqzl	%1,1b\n"
			#else
		"beqz	%1,1b\n"
			#endif
			#if(defined(_MIPS_SIM) && (_MIPS_SIM == _ABIO32))
		".set	pop\n"
			#endif
		"2:\n\t"
		:"=&r" (vReturn), "=&r" (tTemp), "=m" (*pInt64)
		:"r" (pInt64__2), "m" (*pInt64)
		:"memory"
	);
		#else
	assert(false);
		#endif
	#else
	assert(false);
	#endif
#else
	#if(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__WINDOWS)
	vReturn = _InterlockedAnd64(pInt64, pInt64__2);
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__MACOS)
	vReturn = OSAtomicAnd64OrigBarrier(((uint64_t)pInt64__2), ((uint64_t volatile *)pInt64));
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__FREE_BSD)
	/*THE FOLLOWING CAN STILL BE NON ATOMIC IN HOPEFULY RARE CASES*/
	int64_t tLocation = *pInt64;
	
	vReturn = (tLocation == 0 ? 1 : 0);
	
	while(vReturn != tLocation)
	{
		tLocation = *pInt64;

		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();

		if(atomic_cmpset_64(((uint64_t volatile *)pInt64), ((uint64_t)tLocation), 
				((uint64_t)tLocation) & ((uint64_t)pInt64__2)) != 0)
			{vReturn = tLocation;}
		else
			{vReturn = *pInt64;}

		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	}
	#elif(CRX__C__OS__INFO__OS == CRX__C__OS__INFO__OS__SOLARIS)
	int64_t tLocation = *pInt64;
	
	vReturn = (tLocation == 0 ? 1 : 0);
	
	while(vReturn != tLocation)
	{
		tLocation = *pInt64;

		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
		vReturn = ((int64_t)atomic_cas_64(((uint64_t volatile *)pInt64), ((uint64_t)tLocation),
				((uint64_t)tLocation) & ((uint64_t)pInt64__2)));
		CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	}
	#else
		#if(CRX__C__PTHREAD__PRIVATE__ARE_GNU_SYNC_BUILTINS_AVAILABLE)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = __sync_fetch_and_and(pInt64, pInt64__2);
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
		#elif(CRX__C__PTHREAD__PRIVATE__ARE_GNU_ATOMIC_BUILTINS_AVAILABLE)
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
	vReturn = __atomic_fetch_and(pInt64, pInt64__2, __ATOMIC_RELAXED);
	CRX__C__PTHREAD__PRIVATE__FULL_MEMORY_BARRIER();
		#else
	assert(false);
		#endif
	#endif
#endif

	return vReturn;

#if((CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__WATCOM) || \
		(CRX__COMPILER__TYPE == CRX__COMPILER__TYPE__OPEN_WATCOM))
	#pragma enable_message (200)
#endif
}
