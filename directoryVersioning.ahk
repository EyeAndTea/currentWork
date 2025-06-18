#NoEnv
;SetBatchLines, 40		;THIS DID NOT PROVE WORTH IT. MIGHT BE WHEN WE ARE NOT IO BOUND.
#Include ./Lib
#Include Crx/AHK/crx/Window.ahk
#Include <HashFile>
#Include <CriticalSection>
#Include <export>
#Include Crx/AHK/crx/Lambda.ahk


MODE__PROCESSING__ONE_BY_ONE := 1
MODE__PROCESSING__MANY_BY_ONE := 2
MODE__THREADS__SINGLE := 1
MODE__THREADS__MULTIPLE := 2
MODE__READING__NOT_ORDERED := 1
MODE__READING__ORDERED := 2		;CURRENT IMPLEMENTATION PROVED SLOWER

MODE__PROCESSING := MODE__PROCESSING__MANY_BY_ONE
MODE__THREADS := MODE__THREADS__MULTIPLE
MODE__READING := MODE__READING__NOT_ORDERED
SIZE_OF_FILE_BATCHES := 100


class MyApp extends Crx_Window
{
	CONDITION__NULL := 0
	CONDITION__IN_NEW_DIRECTORY := 1
	CONDITION__IN_OLD_DIRECTORY := 2
	CONDITION__IN_BOTH_DIRECTORIES := 3
	IS_TO_COPY__NULL := 0
	IS_TO_COPY__NO := 1
	IS_TO_COPY__YES := 2
	gFileMask := "*"
	gHwnd_text_labelForOldDir := 0
	gHwnd_text_labelForNewDir := 0
	gHwnd_text_labelForOutputDir := 0
	gHwnd_text_messages := 0
	gHwnd_edit_oldDir := 0
	gHwnd_edit_newDir := 0
	gHwnd_edit_outputDir := 0
	gHwnd_button_oldDir := 0
	gHwnd_button_newDir := 0
	gHwnd_button_outputDir := 0
	gHwnd_button_start := 0
	gHwnd_button_stop := 0
	gPathOfOldDir := ""
	gPathOfNewDir := ""
	gPathOfOutputDir := ""
	gIsStarted := false
	gAhkDllThread := CRX_NULL
	gAhkDllThread__2 := CRX_NULL
	gCriticalSection := 0
	gCriticalSection__2 := 0

	__New(pArgs*)
	{
		base.__New(pArgs*)

		vString := ""

		this.gHwnd_text_labelForOldDir := this.Gui("Add", "Text", "Hwnd" . Crx_Window.RET01 . " x12 y20 w50 h20", "Old Dir")[1]
		this.gHwnd_edit_oldDir := this.Gui("Add", "Edit", "Hwnd" . Crx_Window.RET01 . " x82 y20 w360 h20", "")[1]
		this.gHwnd_button_oldDir := this.Gui("Add", "Button", "Hwnd" . Crx_Window.RET01 . " x452 y20 w50 h20", "Explore")[1]
		this.GuiControl("+g", this.gHwnd_button_oldDir, this)

		this.gHwnd_text_labelForNewDir := this.Gui("Add", "Text", "Hwnd" . Crx_Window.RET01 . " x12 y50 w50 h20", "New Dir")[1]
		this.gHwnd_edit_newDir := this.Gui("Add", "Edit", "Hwnd" . Crx_Window.RET01 . " x82 y50 w360 h20", "")[1]
		this.gHwnd_button_newDir := this.Gui("Add", "Button", "Hwnd" . Crx_Window.RET01 . " x452 y50 w50 h20", "Explore")[1]
		this.GuiControl("+g", this.gHwnd_button_newDir, this)

		this.gHwnd_text_labelForOutputDir := this.Gui("Add", "Text", "Hwnd" . Crx_Window.RET01 . " x12 y80 w50 h20", "Output Dir")[1]
		this.gHwnd_edit_outputDir := this.Gui("Add", "Edit", "Hwnd" . Crx_Window.RET01 . " x82 y80 w360 h20", "")[1]
		this.gHwnd_button_outputDir := this.Gui("Add", "Button", "Hwnd" . Crx_Window.RET01 . " x452 y80 w50 h20", "Explore")[1]
		this.GuiControl("+g", this.gHwnd_button_outputDir, this)

		this.gHwnd_button_start := this.Gui("Add", "Button", "Hwnd" . Crx_Window.RET01 . " x452 y110 w50 h20", "Start")[1]
		this.GuiControl("+g", this.gHwnd_button_start, this)

		this.gHwnd_button_stop := this.Gui("Add", "Button", "Hwnd" . Crx_Window.RET01 . " x452 y140 w50 h20 +Disabled", "Stop")[1]
		this.GuiControl("+g", this.gHwnd_button_stop, this)

		this.gHwnd_text_messages := this.Gui("Add", "Text", "Hwnd" . Crx_Window.RET01 . " x102 y120 w290 h30", "")[1]

		; Generated using SmartGUI Creator for SciTE
		this.Gui("Show", "w523 h165", "Directory Versioning")

		this.gAhkDllThread := AhkDllThread("AutohotkeyH.dll")
		this.gAhkDllThread__2 := AhkDllThread("AutohotkeyH.dll")

		this.gAhkDllThread.ahkdll()
		this.gAhkDllThread__2.ahkdll()

		this.gCriticalSection := CriticalSection()
		this.gCriticalSection__2 := CriticalSection()

		this.gAhkDllThread.ahkExec(this.getThreadCode(this.gCriticalSection))
		this.gAhkDllThread__2.ahkExec(this.getThreadCode(this.gCriticalSection__2))

		return this
	}

	getThreadCode(pLpCriticalSection)
	{
		global

		local vString := " "
				. "#Include ./Lib `n"
				. "#Include <HashFile> `n"
				. "#Include <CriticalSection> `n"
				. "#Include <export> `n"

				. "gLpCriticalSection := " . pLpCriticalSection . " `n"
				. "gIsFinished := ""false"" `n"
				. "gMainThread_filePath := """" `n"
				. "gMainThread_filePaths := """" `n"
				. "gMainThread_rootPath := """" `n"
				. "gReturn := """" `n"
				. "gReturn2 := """" `n"

				. "calculateHash() `n"
				. "{ `n"
					. "global `n"

					. "gReturn := HashFile(gMainThread_filePath) `n"
					. "Lock(gLpCriticalSection) `n"
					. "gIsFinished := ""true"" `n"  ;BEFORE USING LOCKS, IF THIS IS COMMENTED OUT, THE PROGRAM NO LONGER FREEZES
					. "UnLock(gLpCriticalSection) `n"
				. "} `n"

				. "calculateHashes() `n"
				. "{ `n"
					. "global `n"

					. "local vMainThread_filePaths := JSON.parse(gMainThread_filePaths) `n"
					. "local vHashes := [] `n"

					. "loop % vMainThread_filePaths.Length() `n"
					. "{ `n"
						. "vHashes.push(HashFile(gMainThread_rootPath . vMainThread_filePaths[A_Index])) `n"
					. "} `n"

					. "gReturn2 := JSON.stringify(vHashes) `n"
					;. "gReturn2 := ""WW"" . vHashes.Length() `n"
					. "Lock(gLpCriticalSection) `n"
					. "gIsFinished := ""true"" `n"
					. "UnLock(gLpCriticalSection) `n"
				. "} `n"

				. "START: `n"
				. "gIsFinished := ""false"" `n"
				. "gReturn := """" `n"
				. "calculateHash() `n"
				. "return `n"

				. "START2: `n"
				. "gIsFinished := ""false"" `n"
				. "gReturn2 := """" `n"
				. "calculateHashes() `n"
				. "return `n"

		return vString
	}

	process()
	{
		global

		if(!this.gIsStarted)
		{
			return
		}

		local vStartTime := A_TickCount
		local vNonEmptyDirectories := {}
		local vFileEntries := {}
		local vIndexToDirectoryEntry := []
		local vDirectoryPathToIndex := {}
		local vNewDirFileListCount := 0
		local vIndex := 0
		local vNewDirEmptyDirectories := {}
		local vOldDirEmptyDirectories := {}
		local vOutputDirFileList := []
		local vOutputDirFileListCount := 0
		local vOutputDirCopyErrorFileList := []
		local vOldDirDeletedFileList := {}
		local vOldDirDeletedEmptyDirList := {}
		local vOldDirDeletedFilePathListString := ""
		local vCount := 0
		local vProgressInfo := CRX_NULL
		local vLengthOfOutputDirector := StrLen(this.gPathOfOutputDir)
		local vCount2 := 0
		local vFilePaths__forThreads := []
		local vAreDirectoriesOnSameLogicalDrives := (this.parseFileSystemPath(this.gPathOfOldDir)).gDriveLetter = (this.parseFileSystemPath(this.gPathOfNewDir)).gDriveLetter



		this.setMessage("Started(1/4): Gathering New Files")
		loop % this.gPathOfNewDir . "/" . this.gFileMask, 0, 1
		{
			;tPath := this.getRelativePath(A_LoopFileFullPath, this.gPathOfNewDir)
			tRelativePathOfDirectoriesInPath := this.getRelativePathOfDirectoriesInPath(this.getNormalizedFileSystemPath(A_LoopFileDir), this.gPathOfNewDir)
			tDirectoryRelativePath := tRelativePathOfDirectoriesInPath[tRelativePathOfDirectoriesInPath.length()]

			if(!this.gIsStarted)
			{
				return
			}

			loop % tRelativePathOfDirectoriesInPath.Length()
			{
				if(!vDirectoryPathToIndex.HasKey(tRelativePathOfDirectoriesInPath[A_Index]))
				{
					vIndexToDirectoryEntry.push(this.newDirectoryEntry(tRelativePathOfDirectoriesInPath[A_Index], this.CONDITION__IN_NEW_DIRECTORY, false))
					vDirectoryPathToIndex[tRelativePathOfDirectoriesInPath[A_Index]] := vIndexToDirectoryEntry.length()
				}
				;vNonEmptyDirectories[tRelativePathOfDirectoriesInPath[A_Index]] := 1
			}

			vFileEntries["" . vDirectoryPathToIndex[tDirectoryRelativePath] . "_" . A_LoopFileName] := this.newFileEntry(A_LoopFileTimeModified, vDirectoryPathToIndex[tDirectoryRelativePath], A_LoopFileSize, this.CONDITION__IN_NEW_DIRECTORY, this.IS_TO_COPY__YES)
			;vFileEntries[tPath]["fullPath"] := A_LoopFileFullPath
			vNewDirFileListCount := vNewDirFileListCount + 1
		}
		loop % this.gPathOfNewDir . "/" . this.gFileMask, 2, 1
		{
			if(!this.gIsStarted)
			{
				return
			}

			tDirectoryRelativePath := this.enforceAndGetPathAsDirectoryPath(this.getRelativePath(this.getNormalizedFileSystemPath(A_LoopFileFullPath), this.gPathOfNewDir))

			;if((tDirectoryRelativePath != "") and !vNonEmptyDirectories.HasKey(tDirectoryRelativePath))
			;{
			;	vNewDirEmptyDirectories[tDirectoryRelativePath] := 1
			;}
			if(!vDirectoryPathToIndex.HasKey(tDirectoryRelativePath))
			{
				vIndexToDirectoryEntry.push(this.newDirectoryEntry(tDirectoryRelativePath, this.CONDITION__IN_NEW_DIRECTORY, true))
				vDirectoryPathToIndex[tDirectoryRelativePath] := vIndexToDirectoryEntry.length()
			}
		}



		this.setMessage("Started(2/4): Gathering Old Files")
		;VarSetCapacity(vNonEmptyDirectories, 0)
		;vNonEmptyDirectories := {}

		Loop % this.gPathOfOldDir . "/" . this.gFileMask, 0, 1
		{
			;tPath := this.getRelativePath(A_LoopFileFullPath, this.gPathOfOldDir)
			tRelativePathOfDirectoriesInPath := this.getRelativePathOfDirectoriesInPath(this.getNormalizedFileSystemPath(A_LoopFileDir), this.gPathOfOldDir)
			tDirectoryRelativePath := tRelativePathOfDirectoriesInPath[tRelativePathOfDirectoriesInPath.length()]
			tKey := ""

			if(!this.gIsStarted)
			{
				return
			}

			loop % tRelativePathOfDirectoriesInPath.Length()
			{
				if(!vDirectoryPathToIndex.HasKey(tRelativePathOfDirectoriesInPath[A_Index]))
				{
					vIndexToDirectoryEntry.push(this.newDirectoryEntry(tRelativePathOfDirectoriesInPath[A_Index], this.CONDITION__IN_OLD_DIRECTORY, false))
					vDirectoryPathToIndex[tRelativePathOfDirectoriesInPath[A_Index]] := vIndexToDirectoryEntry.length()
				}
				else
				{
					vIndexToDirectoryEntry[vDirectoryPathToIndex[tRelativePathOfDirectoriesInPath[A_Index]]].gCondition := this.CONDITION__IN_BOTH_DIRECTORIES
				}
				;vNonEmptyDirectories[tRelativePathOfDirectoriesInPath[A_Index]] := 1
			}

			tKey := "" . vDirectoryPathToIndex[tDirectoryRelativePath] . "_" . A_LoopFileName

			if(vFileEntries.HasKey(tKey))
			{
				vFileEntries[tKey].gCondition := this.CONDITION__IN_BOTH_DIRECTORIES

				if(vFileEntries[tKey].gRelevantSize = A_LoopFileSize)
				{
					vFileEntries[tKey].gIsToCopy := this.IS_TO_COPY__NULL
				}
			}
			else
			{
				vFileEntries[tKey] := this.newFileEntry(A_LoopFileTimeModified, vDirectoryPathToIndex[tDirectoryRelativePath], A_LoopFileSize, this.CONDITION__IN_OLD_DIRECTORY, this.IS_TO_COPY__NO)
			}
		}
		Loop % this.gPathOfOldDir . "/" . this.gFileMask, 2, 1
		{
			if(!this.gIsStarted)
			{
				return
			}

			tDirectoryRelativePath := this.enforceAndGetPathAsDirectoryPath(this.getRelativePath(this.getNormalizedFileSystemPath(A_LoopFileFullPath), this.gPathOfOldDir))

			/*if((tDirectoryRelativePath != "") and !vNonEmptyDirectories.HasKey(tDirectoryRelativePath))
			{
				vOldDirEmptyDirectories[tDirectoryRelativePath] := 1
			}
			*/
			if(!vDirectoryPathToIndex.HasKey(tDirectoryRelativePath))
			{
				vIndexToDirectoryEntry.push(this.newDirectoryEntry(tDirectoryRelativePath, this.CONDITION__IN_OLD_DIRECTORY, true))
				vDirectoryPathToIndex[tDirectoryRelativePath] := vIndexToDirectoryEntry.length()
			}
		}

		vProgressInfo := this.newProgressInfo(vNewDirFileListCount)
		;this.setPercentageInfoForReport(vNewDirFileListCount, vCount, vLastPercentageShown, vCountToNextShownPercentage, vTimeStamp)
		this.setPercentageInfoForReport(vProgressInfo, vCount)
		this.setMessage("Started(3/4): Comparing Files (" . Format("{1:4.2f}", vProgressInfo.gPercentage) . "%)")
		for tKey, tVal in vFileEntries
		{
			if(vFileEntries[tKey].gCondition = this.CONDITION__IN_OLD_DIRECTORY)
			{
				;vOldDirDeletedFileList[tKey] := vFileEntries[tKey]
			}
			else
			{
				tFileRelativePath := vIndexToDirectoryEntry[SubStr(tKey, 1, this.stringGetPos2(tKey, "_") - 1)].gPath . SubStr(tKey, this.stringGetPos2(tKey, "_") + 1)

				if(MODE__PROCESSING = MODE__PROCESSING__ONE_BY_ONE)
				{
					if(vFileEntries[tKey].gIsToCopy = this.IS_TO_COPY__NULL)
					{
						;if(HashFile(this.getFullPath(this.gPathOfNewDir, tKey)) = HashFile(this.getFullPath(this.gPathOfOldDir,tKey)))

						if(this.areFilesByteEqual(this.getFullPath(this.gPathOfNewDir, tFileRelativePath), this.getFullPath(this.gPathOfOldDir,tFileRelativePath)))
						{
							vFileEntries[tKey].gIsToCopy := this.IS_TO_COPY__NO
						}
						else
						{
							vFileEntries[tKey].gIsToCopy := this.IS_TO_COPY__YES
						}
					}

					if(vFileEntries[tKey].gIsToCopy = this.IS_TO_COPY__YES)
					{
						vOutputDirFileListCount := vOutputDirFileListCount + 1

						if((vLengthOfOutputDirector + StrLen(tFileRelativePath)) > 255)
						{
							this.setMessage("Error: A file name is too long for destination. \n File: " . tFileRelativePath)
							this.gIsStarted := false
							this.onProcessEnd()

							return
						}
					}

					vCount := vCount + 1
				}
				else if(MODE__PROCESSING = MODE__PROCESSING__MANY_BY_ONE)
				{
					if(vFileEntries[tKey].gIsToCopy = this.IS_TO_COPY__NULL)
					{
						vFilePaths__forThreads.push(tFileRelativePath)
						vCount2 := vCount2 + 1
					}
					else
					{
						vCount := vCount + 1
					}

					if(vCount2 = SIZE_OF_FILE_BATCHES)
					{
						tNumberOfFilesToCopy := this.process__byteCompareFiles(!vAreDirectoriesOnSameLogicalDrives, vFilePaths__forThreads, vFileEntries, vDirectoryPathToIndex)

						if(tNumberOfFilesToCopy > -1)
						{
							vOutputDirFileListCount := vOutputDirFileListCount + tNumberOfFilesToCopy

							vFilePaths__forThreads := []
							vCount := vCount + vCount2
							vCount2 := 0
						}
						else
						{
							return
						}
					}
				}

				if(vCount >= vProgressInfo.gCountToNextShownPercentage)
				{
					this.setPercentageInfoForReport(vProgressInfo, vCount)
					this.setMessage("Started(3/4): Comparing Files (" . Format("{1:4.2f}", vProgressInfo.gPercentage) . "%)")
				}
			}

			if(!this.gIsStarted)
			{
				return
			}
		}

		if(vCount2 > 0)
		{
			tNumberOfFilesToCopy := this.process__byteCompareFiles(true, vFilePaths__forThreads, vFileEntries, vDirectoryPathToIndex)

			if(tNumberOfFilesToCopy > -1)
			{
				vOutputDirFileListCount := vOutputDirFileListCount + tNumberOfFilesToCopy

				vFilePaths__forThreads := []
				vCount := vCount + vCount2
				vCount2 := 0
			}
			else
			{
				return
			}
		}
		if(vCount >= vProgressInfo.gCountToNextShownPercentage)
		{
			this.setPercentageInfoForReport(vProgressInfo, vCount)
			this.setMessage("Started(3/4): Comparing Files (" . Format("{1:4.2f}", vProgressInfo.gPercentage) . "%)")
		}

		/*vOldDirDeletedEmptyDirList := vOldDirEmptyDirectories.Clone()
		for tKey, tVal in vNewDirEmptyDirectories
		{
			vOldDirDeletedEmptyDirList.Delete(tKey)

			if((vLengthOfOutputDirector + StrLen(tKey)) > 255)
			{
				this.setMessage("Error: A file name is too long for destination. \n Directory: " . tKey)
				this.gIsStarted := false
				this.onProcessEnd()

				return
			}
		}
		*/
		loop % vIndexToDirectoryEntry.length()
		{
			if((vIndexToDirectoryEntry[A_Index].gCondition = this.CONDITION__IN_NEW_DIRECTORY) && vIndexToDirectoryEntry[A_Index].gIsEmpty)
			{
				if(vLengthOfOutputDirector + StrLen(vIndexToDirectoryEntry[A_Index].gPath) > 255)
				{
					this.setMessage("Error: A file name is too long for destination. \n Directory: " . vIndexToDirectoryEntry[A_Index].gPath)
					this.gIsStarted := false
					this.onProcessEnd()

					return
				}
			}
		}

		vCount := 0
		vProgressInfo := this.newProgressInfo(vOutputDirFileListCount)
		;this.setPercentageInfoForReport(vOutputDirFileListCount, vCount, vLastPercentageShown, vCountToNextShownPercentage, vTimeStamp)
		this.setPercentageInfoForReport(vProgressInfo, vCount)
		this.setMessage("Started(4/4): Creating Directory Patch (" . Format("{1:4.2f}", vProgressInfo.gPercentage) . "%)")
		for tKey, tVal in vFileEntries
		{
			if(vFileEntries[tKey].gIsToCopy = this.IS_TO_COPY__YES)
			{
				tFileRelativePath := vIndexToDirectoryEntry[SubStr(tKey, 1, this.stringGetPos2(tKey, "_") - 1)].gPath . SubStr(tKey, this.stringGetPos2(tKey, "_") + 1)
				;MsgBox % "FOUND::  " . this.gPathOfOutputDir . tKey
				;MsgBox % this.gPathOfOutputDir . tKey

				if(this.FileCopy2(this.getFullPath(this.gPathOfNewDir, tFileRelativePath)
						, this.gPathOfOutputDir . tFileRelativePath) = 0) ; && (this.getRandom() > 0.8)
				{
					vCount := vCount + 1

					if(vCount = vProgressInfo.gCountToNextShownPercentage)
					{
						this.setPercentageInfoForReport(vProgressInfo, vCount)
						this.setMessage("Started(4/4): Creating Directory Patch (" . Format("{1:4.2f}", vProgressInfo.gPercentage) . "%)")
					}
				}
				else
				{
					vOutputDirCopyErrorFileList.Push(tKey)
				}
			}

			if(!this.gIsStarted)
			{
				return
			}
		}
		/*for tKey, tVal in vNewDirEmptyDirectories
		{
			this.FileCreateDir2(this.gPathOfOutputDir . tKey)
		}
		*/
		loop % vIndexToDirectoryEntry.length()
		{
			if((vIndexToDirectoryEntry[A_Index].gCondition = this.CONDITION__IN_NEW_DIRECTORY) && vIndexToDirectoryEntry[A_Index].gIsEmpty)
			{
				this.FileCreateDir2(this.gPathOfOutputDir . vIndexToDirectoryEntry[A_Index].gPath)
			}
		}

		if(vOutputDirCopyErrorFileList.length() > 0)
		{
			vOldDirDeletedFilePathListString .= "START: ERROR COPYING THE FOLLOWING FILES `n"

			loop % vOutputDirCopyErrorFileList.Length()
			{
				tFileRelativePath := vIndexToDirectoryEntry[SubStr(vOutputDirCopyErrorFileList[A_Index], 1, this.stringGetPos2(vOutputDirCopyErrorFileList[A_Index], "_") - 1)].gPath . SubStr(vOutputDirCopyErrorFileList[A_Index], this.stringGetPos2(vOutputDirCopyErrorFileList[A_Index], "_") + 1)

				vOldDirDeletedFilePathListString .= this.getFullPath(this.gPathOfNewDir, tFileRelativePath) . " = " . vFileEntries[vOutputDirCopyErrorFileList[A_Index]].gRelevantSize . "`n"
			}

			vOldDirDeletedFilePathListString .= "END : ERROR COPYING THE FOLLOWING FILES `n --------------------- `n"
		}

		/*for tKey, tVal in vOldDirDeletedFileList
		{
			vOldDirDeletedFilePathListString .= tKey . " = " . vOldDirDeletedFileList[tKey].gRelevantSize . "`n"
		}
		*/
		for tKey, tVal in vFileEntries
		{
			if(vFileEntries[tKey].gCondition = this.CONDITION__IN_OLD_DIRECTORY)
			{
				vOldDirDeletedFilePathListString .= vIndexToDirectoryEntry[SubStr(tKey, 1, this.stringGetPos2(tKey, "_") - 1)].gPath . SubStr(tKey, this.stringGetPos2(tKey, "_") + 1) . " = " . vFileEntries[tKey].gRelevantSize . "`n"
			}
		}
		/*for tKey, tVal in vOldDirDeletedEmptyDirList
		{
			vOldDirDeletedFilePathListString .= tKey . "\ = 0"  . "`n"
		}
		*/
		loop % vIndexToDirectoryEntry.length()
		{
			if((vIndexToDirectoryEntry[A_Index].gCondition = this.CONDITION__IN_OLD_DIRECTORY) && vIndexToDirectoryEntry[A_Index].gIsEmpty)
			{
				vOldDirDeletedFilePathListString .= vIndexToDirectoryEntry[A_Index].gPath . " = 0"  . "`n"
			}
		}



		;IniWrite, %vOldDirDeletedFilePathList%, this.gPathOfOutputDir . "\" . "directoryVersioning.info.text", % "DELETED"
		FileEncoding, UTF-8
		FileAppend, %vOldDirDeletedFilePathListString%, % this.gPathOfOutputDir . "\" . "directoryVersioning.info.text"
		d := ErrorLevel

		if(vOutputDirCopyErrorFileList.length() > 0)
		{
			this.setMessage("Finished WITH ERRORS. TIME ELAPSED(ms): " . (A_TickCount - vStartTime))
		}
		else
		{
			this.setMessage("Finished TIME ELAPSED(ms): " . (A_TickCount - vStartTime))
		}

		this.gIsStarted := false
		this.onProcessEnd()
	}
	process__byteCompareFiles(pIsToUseThreads, pFilePaths, pFileEntries, pDirectoryPathToIndex)
	{
		global

		local vFileChecksums := []
		local vFileChecksums2 := []
		local vLengthOfOutputDirectory := StrLen(this.gPathOfOutputDir)
		local vReturn := 0

		if(pIsToUseThreads && (MODE__THREADS = MODE__THREADS__MULTIPLE))
		{
			local tFilePaths := []
			local tFilePaths__oldDirectory := []
			local tStringifiedFilePaths := ""
			local tStringifiedFilePaths__oldDirectory := ""

			if(MODE__READING = MODE__READING__ORDERED)
			{
				tFilePaths := this.process__byteCompareFiles__getSortedFilePaths(this.gPathOfNewDir, pFilePaths)
				tFilePaths__oldDirectory := this.process__byteCompareFiles__getSortedFilePaths(this.gPathOfOldDir, pFilePaths)
				tStringifiedFilePaths := JSON.stringify(tFilePaths)
				tStringifiedFilePaths__oldDirectory := JSON.stringify(tFilePaths__oldDirectory)
			}
			else if(MODE__READING = MODE__READING__NOT_ORDERED)
			{
				tFilePaths := pFilePaths
				tFilePaths__oldDirectory := pFilePaths
				tStringifiedFilePaths := JSON.stringify(tFilePaths)
				tStringifiedFilePaths__oldDirectory := tStringifiedFilePaths
			}


			this.gAhkDllThread.ahkassign("gMainThread_filePaths", tStringifiedFilePaths)
			this.gAhkDllThread.ahkassign("gMainThread_rootPath", this.gPathOfNewDir)
			this.gAhkDllThread__2.ahkassign("gMainThread_filePaths", tStringifiedFilePaths__oldDirectory)
			this.gAhkDllThread__2.ahkassign("gMainThread_rootPath", this.gPathOfOldDir)

			this.gAhkDllThread.ahkLabel("START2")
			this.gAhkDllThread__2.ahkLabel("START2")

			while % (this.areThreadsNotDone())
			{
				Sleep 20

				if(!this.gIsStarted)
				{
					this.gAhkDllThread.ahkTerminate(-500)
					this.gAhkDllThread__2.ahkTerminate(-500)

					return -1
				}
			}

			if(MODE__READING = MODE__READING__ORDERED)
			{
				local tFileCheckSums := JSON.parse(this.gAhkDllThread.ahkgetvar("gReturn2"))
				local tFileCheckSums2 := JSON.parse(this.gAhkDllThread__2.ahkgetvar("gReturn2"))
				local tFilePathsToIndex := {}

				loop % pFilePaths.Length()
				{
					tFilePathsToIndex[pFilePaths[A_Index]] := A_Index
				}
				loop % pFilePaths.Length()
				{
					vFileChecksums[tFilePathsToIndex[tFilePaths[A_Index]]] := tFileCheckSums[A_Index]
					vFileChecksums2[tFilePathsToIndex[tFilePaths__oldDirectory[A_Index]]] := tFileCheckSums2[A_Index]
				}
			}
			else if(MODE__READING = MODE__READING__NOT_ORDERED)
			{
				vFileChecksums := JSON.parse(this.gAhkDllThread.ahkgetvar("gReturn2"))
				vFileChecksums2 := JSON.parse(this.gAhkDllThread__2.ahkgetvar("gReturn2"))
			}
		}
		else
		{
			loop % pFilePaths.Length()
			{
				vFileChecksums.push(HashFile(this.gPathOfNewDir . pFilePaths[A_Index]))

				if(!this.gIsStarted)
				{
					return -1
				}
			}
			loop % pFilePaths.Length()
			{
				vFileChecksums2.push(HashFile(this.gPathOfOldDir . pFilePaths[A_Index]))

				if(!this.gIsStarted)
				{
					return -1
				}
			}
		}

		loop % vFileChecksums.Length()
		{
			tParsedFileSystemPathData := this.parseFileSystemPath(pFilePaths[A_Index])
			tKey := pDirectoryPathToIndex[tParsedFileSystemPathData.gDirectoryPath] . "_" . tParsedFileSystemPathData.gFileNameWithoutExtension . "." . tParsedFileSystemPathData.gFileExtension

			if(vFileChecksums[A_Index] = vFileChecksums2[A_Index])
			{
				pFileEntries[tKey].gIsToCopy := this.IS_TO_COPY__NO
			}
			else
			{
				pFileEntries[tKey].gIsToCopy := this.IS_TO_COPY__YES
			}

			if(vFileEntries[tKey].gIsToCopy = this.IS_TO_COPY__YES)
			{
				vReturn := vReturn + 1

				if((vLengthOfOutputDirectory + StrLen(pFilePaths[A_Index])) > 255)
				{
					this.setMessage("Error: A file name is too long for destination. \n File: " . pFilePaths[A_Index])
					this.gIsStarted := false
					this.onProcessEnd()

					return -1
				}
			}

			if(!this.gIsStarted)
			{
				return -1
			}
		}

		return vReturn
	}
	process__byteCompareFiles__getSortedFilePaths(pRootPath, pFileRelativePaths)
	{
		global

		local vFileClusterLocations := {}
		local vReturn := pFileRelativePaths
		local vLamda := CRX_NULL

		loop % pFileRelativePaths.Length()
		{
			local tFileExtents := this.getFileExtents(pRootPath . pFileRelativePaths[A_Index], true)

			if(tFileExtents.Length() > 0)
			{
				vFileClusterLocations[pFileRelativePaths[A_Index]] := tFileExtents[1][1]
			}
			else
			{
				vFileClusterLocations[pFileRelativePaths[A_Index]] := 0
			}
		}
		vLamda := new Crx_Lambda(this, "process__byteCompareFiles__getSortedFilePaths__getWeight", vFileClusterLocations)

		vReturn := this.sortArray(pFileRelativePaths, vLamda)



		return vReturn
	}
	process__byteCompareFiles__getSortedFilePaths__getWeight(pData, pValue)
	{
		return pData[pValue]
	}
	sortArray(pArray, pFunc_getWeight)
	{
		global

		local vList := {}
		local vReturn := []

		for tKey, tValue in pArray
		{
			local tWeight := pFunc_getWeight.(tValue)

			if(!vList.HasKey(tWeight))
			{
				vList[tWeight] := []
			}

			vList[tWeight].Push(tKey)

			;MsgBox, % pFunc_getWeight.(5)
		}

		for tKey, tValue in vList
		{
			for tKey2, tValue2 in vList[tKey]
			{
				vReturn.Push(pArray[vList[tKey][tKey2]])
			}
		}

		return vReturn
	}

	;VERBATIM COPY FROM THE RELEVANT WORK. DO NOT MODIFY HERE
	getFileExtents(pFilePath, pIsToGetFirstOnly = false)
	{
		global

		local vHFile := DllCall("CreateFile"
				,"str", pFilePath
				,"uint",1 | 0x10000 ;FILE_READ_ACCESS | DELETE
				,"uint",1 | 2 ;FILE_SHARE_READ | FILE_SHARE_WRITE
				,"uint",0,"uint",3 ;OPEN_EXISTING
				,"uint",0,"uint",0)

		if(vHFile = -1)
		{
			MsgBox, % "Failed: " . A_LastError
			return CRX_NULL
		}
		else
		{
			local vNumberOfExts := 0
			;RETRIEVAL_POINTERS_BUFFER
			;REFER TO https://learn.microsoft.com/en-us/windows/win32/api/winioctl/ns-winioctl-retrieval_pointers_buffer
			;VCN := Virtual Cluster Numbers. The offset within file or stream
			;LCN := Logical Cluster Numbers. The offset within the volume
			;
			local tRpb := CRX_NULL
			local tRpb_offset_extentCount := 0
			local tRpb_offset_startingVcn := 8
			local tRpb_offset_extents := 16
			local tRpb_count := 512
			local tRpb_sizeof := tRpb_offset_extents + (tRpb_count * 16)
			local tLcn0 := 0
			local tFileMapTmp := CRX_NULL
			local tFileMap := CRX_NULL
			local tNumberOfExts__temp := 0
			local tReturn := []

			VarSetCapacity(tRpb, tRpb_sizeof)

			loop
			{
				tErrorCode := 0

				DllCall("DeviceIoControl","uint",vHFile
						,"uint", 0x90073 ;FSCTL_GET_RETRIEVAL_POINTERS
						,"uint64 *", tLcn0, "uint", 8
						,"uint", &tRpb, "uint", tRpb_sizeof, "uintp", tNumberOfExts__temp, "uint", 0)

				tErrorCode := A_LastError

				tNumberOfExts__temp := NumGet(tRpb, tRpb_offset_extentCount)

				if(tNumberOfExts__temp)
				{
					tNumberOfExts__temp := (tNumberOfExts__temp > tRpb_count ? tRpb_count : tNumberOfExts__temp)

					VarSetCapacity(tFileMapTmp, (vNumberOfExts + tNumberOfExts__temp) * 16)

					if(vNumberOfExts > 0)
					{
						DllCall("RtlMoveMemory", "uint", &tFileMapTmp, "uint", &tFileMap, "uint", vNumberOfExts * 16)
					}

					DllCall("RtlMoveMemory", "uint", &tFileMapTmp + (vNumberOfExts * 16), "uint", &tRpb + tRpb_offset_extents, "uint", tNumberOfExts__temp * 16)
					vNumberOfExts += tNumberOfExts__temp
					VarSetCapacity(tFileMap, vNumberOfExts*16)
					DllCall("RtlMoveMemory", "uint", &tFileMap, "uint", &tFileMapTmp, "uint", vNumberOfExts * 16)
				}

				tLcn0 := NumGet(tRpb, tRpb_offset_extents + ((tRpb_count - 1) * 16), "uint64")

				If(tErrorCode != 234) ;ERROR_MORE_DATA
				{
					break
				}
				if(tNumberOfExts__temp < tRpb_count)
				{
					break
				}
				if(pIsToGetFirstOnly)
				{
					break
				}
			}

			DllCall("CloseHandle","uint",vHFile)



			/*
			local vOutput := ""

			if(vNumberOfExts > 0)
			{
				vOutput := vOutput . "STARTING VCN: " . NumGet(tRpb, tRpb_offset_startingVcn) . "`n"

				loop, % vNumberOfExts - 1
				{
					tLcn := NumGet(tFileMap, 8 + (A_Index * 16), "uInt64")
					tNextVcn := NumGet(tFileMap, 0  + (A_Index * 16), "uInt64")

					vOutput := vOutput . "(" . tLcn . ", " . tNextVcn . ")`n"
				}
			}

			MsgBox, %vOutput%
			*/


			loop, % vNumberOfExts
			{
				tReturn.push([NumGet(tFileMap, 8 + ((A_Index - 1) * 16), "uInt64"), NumGet(tFileMap, 0  + ((A_Index - 1) * 16), "uInt64")])
			}

			return tReturn
		}
	}



	newFileEntry(pTime, pDirectoryEntryIndex = -1, pRelevantSize = 0, pCondition = 0, pIsToCopy = 0)
	{
		global

		local vReturn := {}

		vReturn["gTime"] := pTime
		vReturn["gDirectoryEntryIndex"] := pDirectoryEntryIndex
		vReturn["gRelevantSize"] := pRelevantSize
		vReturn["gCondition"] := pCondition
		vReturn["gIsToCopy"] := pIsToCopy

		return vReturn
	}
	newDirectoryEntry(pDirectoryPath, pCondition = 0, pIsEmpty = false)
	{
		global

		local vReturn := {}

		vReturn["gPath"] := pDirectoryPath
		vReturn["gCondition"] := pCondition
		vReturn["gIsEmpty"] := pIsEmpty

		return vReturn
	}

	newProgressInfo(pTotal)
	{
		global

		local vReturn := {}

		vReturn["gTotal"] := pTotal
		vReturn["gPercentage"] := 0.0
		vReturn["gCountToNextShownPercentage"] := 0
		vReturn["gTimeStamp"] := 0

		return vReturn
	}

	getRandom(pMin := 0.0, pMax := 1.0)
	{
		global

		local vReturn := 0

		Random vReturn, %pMin%, %pMax%

		return vReturn
	}

	areFilesByteEqual(pFilePath, pFilePath2)
	{
		if(MODE__THREADS = MODE__THREADS__SINGLE)
		{
			return HashFile(pFilePath) = HashFile(pFilePath2)
		}
		else if(MODE__THREADS = MODE__THREADS__MULTIPLE)
		{
			this.gAhkDllThread.ahkassign("gMainThread_filePath", pFilePath)
			this.gAhkDllThread__2.ahkassign("gMainThread_filePath", pFilePath2)

			this.gAhkDllThread.ahkLabel("START")
			this.gAhkDllThread__2.ahkLabel("START")

			;while % ((this.gAhkDllThread.ahkgetvar("gIsFinished", 0) != "true") || (this.gAhkDllThread__2.ahkgetvar("gIsFinished", 0) != "true"))
			while % (!this.areThreadsNotDone())
			{
				Sleep 20
			}

			return (this.gAhkDllThread.ahkgetvar("gReturn") = this.gAhkDllThread__2.ahkgetvar("gReturn"))
		}
	}

	areThreadsNotDone()
	{
		vReturn := true

		Lock(this.gCriticalSection)
		vReturn := ((this.gAhkDllThread.ahkgetvar("gIsFinished", 0) != "true"))
		s := this.gAhkDllThread.ahkgetvar("gIsFinished", 0)
		s := this.gAhkDllThread.ahkReady()
		UnLock(this.gCriticalSection)

		if(!vReturn)
		{
			Lock(this.gCriticalSection__2)
			vReturn := vReturn || ((this.gAhkDllThread__2.ahkgetvar("gIsFinished", 0) != "true"))
			UnLock(this.gCriticalSection__2)
		}

		return vReturn
	}

	setPercentageInfoForReport(pProgressInfo, pCount)
	{
		global

		local vTimeStamp := A_TickCount
		local vNextPercentage := 0

		pProgressInfo.gPercentage := (pCount / pProgressInfo.gTotal) * 100
		vNextPercentage := pProgressInfo.gPercentage + 1

		if(pCount = 0)
		{
			pProgressInfo.gTimeStamp := A_TickCount
		}
		else
		{
			if(vTimeStamp > pProgressInfo.gTimeStamp)
			{
				vNextPercentage := Max(Floor((pProgressInfo.gPercentage / (vTimeStamp - pProgressInfo.gTimeStamp)) * 500), pProgressInfo.gPercentage + 1)
			}
		}

		pProgressInfo.gCountToNextShownPercentage := Min(Max(Floor(((vNextPercentage) / 100) * pProgressInfo.gTotal), pCount + 1), pProgressInfo.gTotal)
	}

	stringGetPos2(pInputVar, pSearchText, pOccurrence = "L1", pOffset = 0)
	{
		global

		local vReturn := -1

		StringGetPos, vReturn, pInputVar, %pSearchText%, %pOccurrence%, %pOffset%

		if(ErrorLevel = 1)
		{
			vReturn := -2
		}

		return (vReturn + 1)
	}

	stringReplace(pInputVar, pSearchText, pReplaceText = "", pReplaceAll = "")
	{
		global

		local vReturn := -1

		StringReplace, vReturn, pInputVar, %pSearchText%, %pReplaceText%, %ReplaceAll%

		return vReturn
	}

	parseFileSystemPath(pPath)
	{
		global

		local vFileName := ""
		local vDirectoryPath := ""
		local vFileExtension := ""
		local vFileNameWithoutExtension := ""
		local vDriveLetter := ""
		local vReturn := {}

		SplitPath pPath, vFileName, vDirectoryPath, vFileExtension, vFileNameWithoutExtension, vDriveLetter

		vReturn["gDriveLetter"] := vDriveLetter
		vReturn["gDirectoryPath"] := vDirectoryPath . "\"
		vReturn["gFileNameWithoutExtension"] := vFileNameWithoutExtension
		vReturn["gFileExtension"] := vFileExtension

		return vReturn
	}

	getRelativePath(pPath, pRootPath)
	{
		;THIS FUNCTION NEEDS FIXING. SEE fileMarking.ahk. AND MAKE SURE TO UPDATE
		;		getNormalizedFileSystemPath() WITH IT
		global

		local vPosition := 0

		StringGetPos, vPosition, pPath, pRootPath

		return SubStr(pPath, vPosition + StrLen(pRootPath) + 3)
	}

	getRelativePathOfDirectoriesInPath(pPath, pRootPath)
	{
		global

		local vReturn := []
		local vTokens := StrSplit(this.getRelativePath(pPath, pRootPath), "\")
		local vCurrentPath := ""

		if(vTokens.Length() = 0)
		{
			vReturn.Push("\")
		}
		else
		{
			loop % vTokens.Length()
			{
				if(vTokens[A_Index] = "")
				{
					if(vTokens.Length() = 1)
					{
						vReturn.Push("\")
					}
				}
				else
				{
					if(vReturn.length() = 0)
					{
						vReturn.Push("\")
					}

					vCurrentPath := vCurrentPath . vTokens[A_Index] . "\"
					vReturn.Push(vCurrentPath)
				}
			}
		}

		return vReturn
	}

	getFullPath(pRootPath, pRelativePath)
	{
		global

		return pRootPath . "\" . pRelativePath
	}

	getDirectoryPathFrom(pFilePath)
	{
		global

		if((pFilePath[pFilePath.Length()] = "/") || pFilePath[pFilePath.Length()] = "\")
		{
			return pFilePath
		}
		else
		{
			tIndex := this.stringGetPos2(pFilePath, "/", "R1")
			tIndex2 := this.stringGetPos2(pFilePath, "\", "R1")

			if(tIndex2 > tIndex)
			{
				tIndex := tIndex2
			}

			if(tIndex = -1)
			{
				return "\"
			}
			else
			{
				return SubStr(pFilePath, 1, tIndex)
			}
		}
	}

	;THIS IS NOT FORMALLY CORRECT, JUST A QUICK THING. REFER TO THE SECURITY PROJECT
	;		IN THE FUTURE. THE NORMALIZATION FUNCTION THERE SHOULD ALSO BE VALID FOR
	;		FILESYSTEM PATH. HOWEVER CARE SHOULD BE GIVEN TO THE EMPTY STRING PATH
	;		IT IS NOT A VALID RELATIVE PATH ACCORDING TO MY WORK ON THE SUBJECT, BUT
	;		I FORGET IF IT IS A VALID FULL PATH.
	getNormalizedFileSystemPath(pPath)
	{
		;THIS FUNCTION NEEDS UPDATING. SEE fileMarking.ahk.
		global

		local vPath := this.stringReplace(pPath, "/", "\", "1")

		return vPath
	}

	enforceAndGetPathAsDirectoryPath(pPath)
	{
		if(pPath.length() = 0)
		{
			return "\"
		}
		else
		{
			if((SubStr(pPath, StrLen(pPath), 1) != "/") && (SubStr(pPath, StrLen(pPath), 1) != "\"))
			{
				return (pPath . "\")
			}
			else
			{
				return pPath
			}
		}
	}

	;s := this.gPathOfNewDir . "gfd"
	;msgbox % getRelativePath(s, this.gPathOfNewDir)
	FileCopy2(SourcePattern, DestPattern, Flag = 0)
	{
		global

		IfNotExist,% SourcePattern
			return -1
		SplitPath, DestPattern, , OutDir
		IfNotExist, OutDir
		{
			FileCreateDir,% OutDir
			if ErrorLevel
				return -2
		}
		FileCopy,% SourcePattern,% DestPattern,% Flag
		return ErrorLevel
	}
	FileCreateDir2(pDirectoryName)
	{
		global

		FileCreateDir, % pDirectoryName

		if ErrorLevel
			return -2
		else
			return ErrorLevel
	}

	crxClass_doAsFunctionObjectCall(pArgs*)
	{
		global

		if(pArgs.length() = 0)
		{
			local tKey := A_ThisHotkey

			this.onHotKey(tKey)
		}
		else
		{
			if(pArgs[1] = this.gHwnd_button_oldDir)
			{
				this.onButtonClick_oldDir()
			}
			else if(pArgs[1] = this.gHwnd_button_newDir)
			{
				this.onButtonClick_newDir()
			}
			else if(pArgs[1] = this.gHwnd_button_outputDir)
			{
				this.onButtonClick_outputDir()
			}
			else if(pArgs[1] = this.gHwnd_button_start)
			{
				this.onButtonClick_start()
			}
			else if(pArgs[1] = this.gHwnd_button_stop)
			{
				this.onButtonClick_stop()
			}
		}
    }

	getTextFromEdit(pHwnd)
	{
		global

		return this.GuiControlGet(, pHwnd)
	}

	debug_setTextToEdit(pHwnd, pMessage)
	{
		global

		this.ControlSetText(, pMessage, "ahk_id " . pHwnd)
	}

	setMessage(pMessage)
	{
		global

		this.ControlSetText(, pMessage, "ahk_id " . this.gHwnd_text_messages)
	}

	onButtonClick_oldDir()
	{
		global

		local vFolder := ""

		FileSelectFolder, vFolder
		vFolder := RegExReplace(vFolder, "\\$")

		this.ControlSetText(, "" . vFolder, "ahk_id " . this.gHwnd_edit_oldDir)
	}
	onButtonClick_newDir()
	{
		global

		local vFolder := ""

		FileSelectFolder, vFolder
		vFolder := RegExReplace(vFolder, "\\$")

		this.ControlSetText(, "" . vFolder, "ahk_id " . this.gHwnd_edit_newDir)
	}
	onButtonClick_outputDir()
	{
		global

		local vFolder := ""

		FileSelectFolder, vFolder
		vFolder := RegExReplace(vFolder, "\\$")

		this.ControlSetText(, "" . vFolder, "ahk_id " . this.gHwnd_edit_outputDir)
	}
	onButtonClick_start()
	{
		if(this.gIsStarted)
			return

		this.gPathOfOldDir := this.enforceAndGetPathAsDirectoryPath(this.getNormalizedFileSystemPath(this.getTextFromEdit(this.gHwnd_edit_oldDir)))
		this.gPathOfNewDir := this.enforceAndGetPathAsDirectoryPath(this.getNormalizedFileSystemPath(this.getTextFromEdit(this.gHwnd_edit_newDir)))
		this.gPathOfOutputDir := this.enforceAndGetPathAsDirectoryPath(this.getNormalizedFileSystemPath(this.getTextFromEdit(this.gHwnd_edit_outputDir)))

		if(FileExist(this.gPathOfOldDir) and FileExist(this.gPathOfNewDir) and FileExist(this.gPathOfOutputDir))
		{
			this.gIsStarted := true
			this.onProcessStart()

			this.setMessage("Started")
			this.process()
		}
	}
	onButtonClick_stop()
	{
		if(!this.gIsStarted)
			return

		this.gIsStarted := false
		this.onProcessEnd()

		this.setMessage("Stopped")
	}

	onProcessStart()
	{
		global

		this.GuiControl("Disable", this.gHwnd_button_start)
		this.GuiControl("Disable", this.gHwnd_edit_oldDir)
		this.GuiControl("Disable", this.gHwnd_button_oldDir)
		this.GuiControl("Disable", this.gHwnd_edit_newDir)
		this.GuiControl("Disable", this.gHwnd_button_newDir)
		this.GuiControl("Disable", this.gHwnd_edit_outputDir)
		this.GuiControl("Disable", this.gHwnd_button_outputDir)
		this.GuiControl("Enable", this.gHwnd_button_stop)
	}
	onProcessEnd()
	{
		global

		this.GuiControl("Enable", this.gHwnd_button_start)
		this.GuiControl("Enable", this.gHwnd_edit_oldDir)
		this.GuiControl("Enable", this.gHwnd_button_oldDir)
		this.GuiControl("Enable", this.gHwnd_edit_newDir)
		this.GuiControl("Enable", this.gHwnd_button_newDir)
		this.GuiControl("Enable", this.gHwnd_edit_outputDir)
		this.GuiControl("Enable", this.gHwnd_button_outputDir)
		this.GuiControl("Disable", this.gHwnd_button_stop)
	}

	onClose()
	{
		ExitApp
	}
}


gMyApp := new MyApp("gMyApp")