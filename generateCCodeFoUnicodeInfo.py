import sys
sys.path.insert(0, "C:\Programs\python\Projects\crx\lib\\")
sys.path.insert(0, "C:\Programs\python\libraries\\")

import getopt
import os
import crx.utility
import codecs
import six
import sys
import math
import fractions


#NOTE: I NEEDED TO USE INDEX 2 NOT 0, BECAUSE OF MY MODIFICATION OF THE PATH PER MY STANDARD IN THE 
#		STATEMENTS ABOVE
#		HENCE, THIS IS CURRENTLY ILL DEFINED.
gUnicodeFilePath = unicode(crx.utility.enforcePathAsDirectoryPath(sys.path[2]) + u"data" +
		os.sep + "UnicodeData.txt");
gUnicodeData = crx.NULL;
gLargestCcc = 0;


def getUnicodeData():
	global gUnicodeData;

	if(gUnicodeData == crx.NULL):
		if(not os.path.isfile(gUnicodeFilePath)):
			print(u"ERROR: Unicode file, " + gUnicodeFilePath + u" not found.\n");
			return;
		#:endif:
		
		
		tFile = codecs.open(gUnicodeFilePath, encoding='utf-8');
		tLine = tFile.readline();
		
		gUnicodeData = {};

		while(tLine != ""):
			tRawData = tLine.split(u";");
			tIndex = int(tRawData[0], 16);
			tData = [None] * 15;
			
			gUnicodeData[tIndex] = crx.NULL;
			
			#print(tIndex);

			tData[0] = tIndex;
			tData[1] = tRawData[1];
			#Lu 	Letter, Uppercase
			#Ll 	Letter, Lowercase
			#Lt 	Letter, Titlecase
			#Mn 	Mark, Non-Spacing
			#Mc 	Mark, Spacing Combining
			#Me 	Mark, Enclosing
			#Nd 	Number, Decimal Digit
			#Nl 	Number, Letter
			#No 	Number, Other
			#Zs 	Separator, Space
			#Zl 	Separator, Line
			#Zp 	Separator, Paragraph
			#Cc 	Other, Control
			#Cf 	Other, Format
			#Cs 	Other, Surrogate
			#Co 	Other, Private Use
			#Cn 	Other, Not Assigned (no characters in the file have this property)
			tData[2] = tRawData[2];
			tData[3] = int(tRawData[3]);
			# L 	Left-to-Right
			# R 	Right-to-Left
			# AL 	Right-to-Left Arabic
			# ES 	European Number Separator
			# ET 	European Number Terminator
			# AN 	Arabic Number
			# CS 	Common Number Separator
			# NSM 	Nonspacing Mark
			# BN 	Boundary Neutral
			# B 	Paragraph Separator
			# S 	Segment Separator 	Tab
			# WS 	Whitespace
			# ON 	Other Neutrals
			# LRE 	Left-to-Right Embedding
			# LRO 	Left-to-Right Override
			# RLE 	Right-to-Left Embedding
			# RLO 	Right-to-Left Override
			# PDF 	Pop Directional Format
			# LRI 	Left-to-Right Isolate
			# RLI 	Right-to-Left Isolate
			# FSI 	First Strong Isolate
			# PDI 	Pop Directional Isolate
			tData[4] = tRawData[4];
			# ""		Canonical
			#		THE REST ARE COMPATIBILITY
			# <font>   	A font variant (e.g. a blackletter form).
			# <noBreak>   	A no-break version of a space or hyphen.
			# <initial>   	An initial presentation form (Arabic).
			# <medial>   	A medial presentation form (Arabic).
			# <final>   	A final presentation form (Arabic).
			# <isolated>   	An isolated presentation form (Arabic).
			# <circle>   	An encircled form.
			# <super>   	A superscript form.
			# <sub>   	A subscript form.
			# <vertical>   	A vertical layout presentation form.
			# <wide>   	A wide (or zenkaku) compatibility character.
			# <narrow>   	A narrow (or hankaku) compatibility character.
			# <small>   	A small variant form (CNS compatibility).
			# <square>   	A CJK squared font variant.
			# <fraction>   	A vulgar fraction form.
			# <compat>   	Otherwise unspecified compatibility character.
			tData[5] = ["", []];

			if(tRawData[5] != u""):
				tStrings = tRawData[5].split();
				tStartIndex = 0;

				if(tStrings[0][0] == "<"):
					tStartIndex = 1;
					tData[5][0] = tStrings[0];
				else:
					tData[5][0] = "<>";
				#:endif:

				for tI in six.moves.range(tStartIndex, len(tStrings), 1):
					tData[5][1].append(int(tStrings[tI], 16));
				#:endfor:
			#:endif:

			if(tRawData[6] == u""):
				tData[6] = -1.0;
			else:
				tData[6] = float(tRawData[6]);
			#:endif:
			
			if(tRawData[7] == u""):
				tData[7] = -1;
			else:
				tData[7] = int(tRawData[7]);
			#:endif:

			tData[8] = fractions.Fraction(0, 1);

			if(tRawData[8] != u""):
				tFractionParts = tRawData[8].split();
				tIsNegative = False;

				for tI in six.moves.range(0, len(tFractionParts), 1):
					tFraction = fractions.Fraction(tFractionParts[tI]);

					if(tFraction < 0):
						tIsNegative = True;
						tFraction = -tFraction;
					#:endif:

					tData[8] += tFraction;
				#:endfor:

				if(tIsNegative):
					tData[8] = -tData[8];
				#:endif:
			#:endif:

			if(tRawData[9] == u"Y"):
				tData[9] = True;
			else:
				tData[9] = False;
			#:endif:

			tData[10] = tRawData[10];
			tData[11] = tRawData[11];
			
			if(tRawData[12] == u""):
				tData[12] = -1;
			else:
				tData[12] = int(tRawData[12], 16);
			#:endif:
			
			if(tRawData[13] == u""):
				tData[13] = -1;
			else:
				tData[13] = int(tRawData[13], 16);
			#:endif:
			
			if(tRawData[14].strip() == u""):
				tData[14] = -1;
			else:
				tData[14] = int(tRawData[14], 16);
			#:endif:

			gUnicodeData[tIndex] = tData;

			tLine = tFile.readline();
		#:endwhile:

		tFile.close();
	#:endif:
	
	return gUnicodeData;
#:enddef:

def decomposeCanonicallyComposedCharacter(pKey):
	vUnicodeData = getUnicodeData();
	vReturn = [];

	if((not(pKey in vUnicodeData)) or (vUnicodeData[pKey][5][0] != "<>")):
		vReturn.append(pKey);
	else:
		for tI in six.moves.range(0, len(vUnicodeData[pKey][5][1]), 1):
			vReturn += decomposeCanonicallyComposedCharacter(vUnicodeData[pKey][5][1][tI]);
			# if(pKey == 7853):
				# print vUnicodeData[pKey][5][1];
			# #:endif:
		#:endfor:
	#:endif:

	return vReturn;
#:enddef:

def getLargestCcc():
	global gLargestCcc;

	if(gLargestCcc == 0):
		vUnicodeData = getUnicodeData();

		for tKey in vUnicodeData.keys():
			if(vUnicodeData[tKey][3] > gLargestCcc):
				gLargestCcc = vUnicodeData[tKey][3];
			#:endif:
		#:endif:
	#:endif:

	return gLargestCcc;
#:enddef:

def checkCcc(pCodePoints):
	vUnicodeData = getUnicodeData();
	vPreviousUnicodeCharacterCanonicalCombiningClass = 0;
	vIsValid = True;

	for tI in six.moves.range(0, len(pCodePoints), 1):
		tUnicodeCharacterCanonicalCombiningClass = 0;

		if(pCodePoints[tI] in vUnicodeData):
			tUnicodeCharacterCanonicalCombiningClass = vUnicodeData[pCodePoints[tI]][3];
		#:endif:

		if((vPreviousUnicodeCharacterCanonicalCombiningClass == 0) or
				(tUnicodeCharacterCanonicalCombiningClass == 0) or
				(vPreviousUnicodeCharacterCanonicalCombiningClass <= 
						tUnicodeCharacterCanonicalCombiningClass)):
			vPreviousUnicodeCharacterCanonicalCombiningClass = (
					tUnicodeCharacterCanonicalCombiningClass);
		else:
			vIsValid = False;

			break;
		#:endif:
	#:endif:

	return vIsValid;
#:enddef:
#WARNING: THIS COMMENT WRITTEN SOME TIME LATER
#THIS ASSUMES checkCcc() WAS CALLED FIRST.
def checkCcc2(pCodePoints):
	vUnicodeData = getUnicodeData();
	vLargestCcc = getLargestCcc();

	if(not(pCodePoints[0] in vUnicodeData) or 
			(vUnicodeData[pCodePoints[0]][3] == 0) or (len(pCodePoints) == 1) or 
			(vUnicodeData[pCodePoints[0]][3] == vLargestCcc)):
		return True;
	else:
		tUnicodeCharacterCanonicalCombiningClass = vUnicodeData[pCodePoints[0]][3];
		tReturn = True;

		for tI in six.moves.range(1, len(pCodePoints), 1):
			if(vUnicodeData[pCodePoints[tI]][3] != tUnicodeCharacterCanonicalCombiningClass):
				tReturn = False;

				break;
			#:endif:
		#:endfor:

		return tReturn;
	#:endif:
#:enddef:

#WARNING: THIS COMMENT WRITTEN SOME TIME LATER
#THE FOLLOWING GENERATED THE crx_c_os_osString_getCompositeCodePoints() AND THE
#		crx_c_os_osString_getUnicodeCharactersCanonicalCombiningClasses() FUNCTIONS.
def doMain(pSavePath):
	if(os.path.isfile(pSavePath)):
		print(u"ERROR: File \"" + pSavePath + "\" already exists.\n");
		return;
	#:endif:

	MAXIMUM_LENGH_OF_CANONICAL_DECOMPOSITION = 4;
	vUnicodeData = getUnicodeData();
	vNumberOfcharactersDisallowed = 0;
	vNumberOfCharactersInvolvedInOrdering = 0;
	vNumberOfCharactersInvolved = 0;
	vCharactersInvolvedInOrdering = {};
	vMaximumLength = 0;
	vKeys = crx.NULL;
	vString = u"";
	vString2 = u"";
	vIsFirst = True;
	vCount = 0;

	for tKey in vUnicodeData:
		if(vUnicodeData[tKey][5][0] == "<>"):
			vNumberOfcharactersDisallowed = vNumberOfcharactersDisallowed + 1;
			vNumberOfCharactersInvolved = vNumberOfCharactersInvolved + 1;
			
			if(vUnicodeData[tKey][3] > 0):
				vNumberOfCharactersInvolved = vNumberOfCharactersInvolved - 1;
			#:endif:
			
			if(len(vUnicodeData[tKey][5][1]) > vMaximumLength):
				vMaximumLength = len(vUnicodeData[tKey][5][1]);
			#:endif:
		#:endif:
	#:endif:
	
	for tKey in vUnicodeData:
		if(vUnicodeData[tKey][3] > 0):
			vNumberOfCharactersInvolvedInOrdering = vNumberOfCharactersInvolvedInOrdering + 1;
			vNumberOfCharactersInvolved = vNumberOfCharactersInvolved + 1;
			vCharactersInvolvedInOrdering[tKey] = vUnicodeData[tKey][3];
		#:endif:
	#:endif:
	
	print(u"Number of Characters Allowed: " + unicode(vNumberOfcharactersDisallowed));
	print(u"Number Of Characters Involved In Ordering: " + 
			unicode(vNumberOfCharactersInvolvedInOrdering));
	print(u"Number Of Characters Involved: " + unicode(vNumberOfCharactersInvolved));	#ROUGHLY THE 
																						#	SUM OF 
																						#	THE 
																						#	PREVIOUS 
																						#	TWO
	print(u"Maximum Partial Canonical Decmposition Length: " + unicode(vMaximumLength));	#2
	
	vString += (
	u"CompositeCodePoint\r\n" +
	u"{\r\n" + 
	u"	uint32_t gCodePoint;\r\n" + 
	#u"	uint32_t gCodePoints[" + unicode(vMaximumLength) + u"];\r\n" + 
	u"	uint32_t gCodePoints[" + unicode(MAXIMUM_LENGH_OF_CANONICAL_DECOMPOSITION) + u"];\r\n" + 
	u"}\r\n" +
	u"\r\n" +
	u"CompositeCodePoint * getCodePointsThatAreComposites(void)\r\n" +
	u"{\r\n" +
	u"	static CompositeCodePoint vReturn[" + unicode(vNumberOfcharactersDisallowed) + u"] = \r\n" + 
	u"	{\r\n" +
	u"		");
	
	vKeys = sorted(vUnicodeData.keys());

	for tI in six.moves.range(0, len(vKeys), 1):
		if(vUnicodeData[vKeys[tI]][5][0] != "<>"):
			continue;
		#:endif:
		
		tIsFirst = True;
		tFullExpansion = [];

		if(not(vIsFirst)):
			if(vCount > 0):
				vString2 += u",";
			else:
				vString2 += (u",\r\n" +
	u"		");
			#:endif:
		else:
			vIsFirst = False;
		#:endif:

		vString2 += u"{";
		vString2 += u"{:>6}".format(unicode(vKeys[tI]));
		vString2 += u", {";
		
		tFullExpansion = decomposeCanonicallyComposedCharacter(vKeys[tI]);
		
		#WARNING: THIS COMMENT WRITTEN SOME TIME LATER
		#DOES EXPANSION FOLLOW NFD
		if(not(checkCcc(tFullExpansion))):
			sys.exit(u"CODE ASSUMPTION WRONG (1): " + unicode(vKeys[tI]) + u" : " + 
					unicode(tFullExpansion));
		#:endif:
		#WARNING: THIS COMMENT WRITTEN SOME TIME LATER
		#IS EXAPNSION ONE OF THE FOLLOWING:
		#		- STARTS WITH A 'PROPER' CODE POINT, MEANING EITHER ONE WITH CCC 0, OR ONE NOT
		#				IN "UnicodeData.txt". IN THIS CASE WHETHER EXPANSION HAD AN EFFECT OR NOT,
		#				THE NET EFFECT IS A 'PROPER' STARTING CODE POINT SUCH e INSTEAD OF
		#				AN APOSTROPHE.
		#		- DOES NOT START WITH A PROPER CODE POINT, BUT IS ONLY A SINGLE CODE POINT.
		#				IN THIS CASE THE EXPANSION DID NOTHING, OR POSSIBLY DID SOMETHING, BUT THE 
		#				CODE POINT IS ONE THAT 'NEEDS' TO BE COMBINED WITH OTHERS, SUCH AS AN 
		#				APOSTROPHE.
		#		- DOES NOT START WITH A PROPER CODE POINT, IS MORE THAN ONE CODE POINT, BUT ALL
		#				THE CODE POINTS HAVE THE SAME CCC. IN THIS CASE THE EXPANSION DID SOMETHING,
		#				AND IS NOT PROPER, BUT IS ONE OF THOSE SPECIAL CASES WHERE THE CODE POINTS
		#				ARE ALL THE SAME. 
		#				(I FORGET WHAT CODE POINTS THIS COVERS, IF ANY AT ALL, BUT AT LEAST IT 
		#				GUARANTEES AN ALGORITHM THAT ONLY NEEDS TO EXAMINE THE FIRST CODE POINT FOR 
		#				EITHER 0 CCC OR NOT 0 CCC AFTER AN EXPANSION IS DONE, OR WITHOUT DOING
		#				AN EXPANSION IF CANONICALLY COMPOSED CHARACTERS ARE FORBIDDEN).
		#
		#THIS FAILS FOR:
		#	- 3955 DECOMPOSED TO [3953, 3954].
		#	- 3957 DECOMPOSED TO [3953, 3956].
		#	- 3969 DECOMPOSED TO [3953, 3968].
		if(not(checkCcc2(tFullExpansion))):
			print(u"CODE ASSUMPTION WRONG (2): " + unicode(vKeys[tI]) + u" : " + 
					unicode(tFullExpansion));
		#WARNING: THIS COMMENT WRITTEN SOME TIME LATER
		#DOES EXPANSION FIT IN WHAT WE THINK IS THE MAXIMUM LENGTH AFTER CANONICAL DECOMPOSITION.
		elif(len(tFullExpansion) > MAXIMUM_LENGH_OF_CANONICAL_DECOMPOSITION):
			sys.exit(u"CODE ASSUMPTION WRONG (3): " + unicode(vKeys[tI]) + u" : " + 
					unicode(tFullExpansion));
		#:endif:
		
		if(False):
			for tI2 in six.moves.range(0, len(vUnicodeData[vKeys[tI]][5][1]), 1):
				if(not(tIsFirst)):
					vString2 += u", ";
				else:
					tIsFirst = False;
				#:endif:

				vString2 += u"{:>6}".format(unicode(vUnicodeData[vKeys[tI]][5][1][tI2]));
			#:endfor:

			for tI2 in six.moves.range(len(vUnicodeData[vKeys[tI]][5][1]), vMaximumLength, 1):
				if(not(tIsFirst)):
					vString2 += u", ";
				else:
					tIsFirst = False;
				#:endif:

				vString2 += u"{:>6}".format(u"0");
			#:endfor:
		else:
			for tI2 in six.moves.range(0, len(tFullExpansion), 1):
				if(not(tIsFirst)):
					vString2 += u", ";
				else:
					tIsFirst = False;
				#:endif:

				vString2 += u"{:>6}".format(unicode(tFullExpansion[tI2]));
			#:endfor:

			for tI2 in six.moves.range(len(tFullExpansion), MAXIMUM_LENGH_OF_CANONICAL_DECOMPOSITION, 1):
				if(not(tIsFirst)):
					vString2 += u", ";
				else:
					tIsFirst = False;
				#:endif:

				vString2 += u"{:>6}".format(u"0");
			#:endfor:
		#:endif:

		#vString2 += u", 0}}";
		vString2 += u"}}";

		vCount += 1;

		if(vCount == 2):
			vString += vString2;
			vString2 = u"";
			vCount = 0;
		#:endif:
	#:endif:
	
	if(vString2 != ""):
		vString += (
			vString2 + "\r\n");
		vString2 = u"";
	#:endif:

	vString += (u"\r\n" +
	u"	};\r\n"+
	u"\r\n" + 
	u"	return vReturn;\r\n" +
	u"}\r\n" + 
	u"\r\n");
	
	
	#vGeneratedCodeInfo = generateCTableLookUpFunction("getUnicodeCharacterCcc", 
	#		vCharactersInvolvedInOrdering, 0);
			
	#vString += vGeneratedCodeInfo["code"];
	
	#print(vGeneratedCodeInfo["countOfCharactersInLoopkupTables"]);
	#print(len(vCharactersInvolvedInOrdering));
	
	
	#print(vCharactersInvolvedInOrdering);
	
	
	vString += (
	u"uint32_t * getUnicodeCharacterCanonicalCombiningClass(void)\r\n" +
	u"{\r\n" +
	u"	static uint8_t vReturn[" + unicode(len(vCharactersInvolvedInOrdering)) + u"] = \r\n" + 
	u"	{\r\n" +
	u"		");
	
	vIsFirst = True;
	vCount = 0;

	vKeys = sorted(vCharactersInvolvedInOrdering.keys());

	for tI in six.moves.range(0, len(vKeys), 1):
		if(not(vIsFirst)):
			if(vCount > 0):
				vString2 += u",		";
			else:
				vString2 += (u",\r\n" +
	u"		");
			#:endif:
		else:
			vIsFirst = False;
		#:endif:

		vString2 += (u"{:>7}".format(unicode(vKeys[tI])) + ", " + 
				u"{:>4}".format(unicode(vCharactersInvolvedInOrdering[vKeys[tI]])));

		vCount += 1;

		if(vCount == 4):
			vString += vString2;
			vString2 = u"";
			vCount = 0;
		#:endif:
	#:endif:
	
	if(vString2 != ""):
		vString += vString2;
		vString2 = u"";
	#:endif:

	vString += (u"\r\n" +
	u"	};\r\n"+
	u"\r\n" + 
	u"	return vReturn;\r\n" +
	u"}\r\n" + 
	u"\r\n");
	
	#vString += unicode(vCharactersInvolvedInOrdering);
	

	if(not(os.path.isfile(pSavePath))):
		tFile = codecs.open(pSavePath, "w", "utf-8");
		tFile.write(vString);
		tFile.close();
		d = 3;
	else:
		vReturn["text"] += u"ERROR: File \"" + pSavePath + "\" already exists.\n";
	#:endif:
#:enddef:

#VERBATIM FROM "generateCCodeForCodePages.py"
def generateCTableLookUpFunction(pFunctionName, pData, pValueOfUndefined):
	THRESHOLD = 10;
	IS_TO_USE_TYPE_COMPRESSION = True;
	vString = "";
	vCountOfCharactersInLoopkupTables = 0;
	tLargestKey = max(pData);
	tTypeName = "uint32_t";
	tValueOfUndefinedCharacter = pValueOfUndefined;
	tCount = 0;
	tCount2 = 0;
	tString__top = u"";
	tString__bottom = u"";
	tIsFirstIf = True;
	tCurrentStart = -1;
	tLastDifference = -1;
	tCurrentDifference = -1;
	tRanges = [];
	vReturn = {};
	
	if(IS_TO_USE_TYPE_COMPRESSION):
		tLargestValue = max(pData.values());
		tReverseHashTable = {};

		for tKey in pData:
			tReverseHashTable[pData[tKey]] = True;
		#:endfor:

		if(tLargestValue < 256):
			tWasSuitableValueFound = False;

			tTypeName = "uint8_t";

			for tI2 in six.moves.range(255, 0, -1):
				if(not(tI2 in tReverseHashTable)):
					tValueOfUndefinedCharacter = tI2;
					tWasSuitableValueFound = True;

					break;
				#:endif:
			#:endfor:

			if(not(tWasSuitableValueFound)):
				tTypeName = "uint16_t";
				tValueOfUndefinedCharacter = 11111;
			#:endif:
		#:endif:
		elif(tLargestValue < 65536):
			tWasSuitableValueFound = False;

			tTypeName = "uint16_t";

			for tI2 in six.moves.range(65535, 0, -1):
				if(not(tI2 in tReverseHashTable)):
					tValueOfUndefinedCharacter = tI2;
					tWasSuitableValueFound = True;

					break;
				#:endif:
			#:endfor:

			if(not(tWasSuitableValueFound)):
				tTypeName = "uint32_t";
				tValueOfUndefinedCharacter = 1111111;
			#:endif:
		#:endif:
	#:endif:

	#six.print_(u"Trying" + unicode(tI2), flush=True);

	vString += ( 
	u"uint32_t " + pFunctionName + u"(uint32_t pValue)\r\n" +
	u"{\r\n");

	for tI2 in six.moves.range(0, tLargestKey + 1, 1):
		#six.print_(unicode(tI2) + ", " + unicode(tCount) + ", " + unicode(tCount2));

		if(not(tI2 in pData)):
			tCurrentDifference = pValueOfUndefined;
		else:
			tCurrentDifference = pData[tI2] - tI2;
		#:endif:

		if(tCurrentStart != -1):
			if(tCurrentDifference != tLastDifference):
				if(tCount2 < THRESHOLD):
					#six.print_(unicode(tI2) + ", " + unicode(tCount) + ", " +  unicode(tCount2) + ", " + 
					#		unicode(tLastDifference) + ", " + unicode(tCurrentDifference), flush=True);
					#six.moves.input("Press Enter To Continue..")
					tCount = tCount + tCount2;
					tCount2 = 1;
				else:
					tString = u"";

					if(tCount > 0):
						tTopAndBottom = (
								generateCTableLookUpFunction__generateTopAndBottomForNonHighlyRepeatingValues(
								pData, tTypeName, tValueOfUndefinedCharacter, tIsFirstIf, 
								tCurrentStart, tCurrentStart + tCount));
								
						vCountOfCharactersInLoopkupTables += tCount;
								
						tString__top += tTopAndBottom["top"];
						#tString__bottom += tTopAndBottom["bottom"];
						
						tRanges.append(generateCTableLookUpFunction__makeRangeEntry(tCurrentStart + tCount,
								tTopAndBottom["bottom"]));
						
						#six.print_(tTopAndBottom["bottom"]);
						#six.moves.input("Press Enter To Continue..");

						tIsFirstIf = False;
						tCount = 0;
					#:endif:

					tString = generateCTableLookUpFunction__generateBottomForHighlyRepeatingValues(pData,
							tIsFirstIf, tLastDifference, tI2 - tCount2, tI2, pValueOfUndefined);
					
					#tString__bottom += tString;

					tRanges.append(generateCTableLookUpFunction__makeRangeEntry(tI2, tString));
					
					#six.print_(tString__bottom);
					#six.moves.input("Press Enter To Continue..");

					tIsFirstIf = False;								
					
					tCurrentStart = tI2;
					tCount2 = 1;
				#:endif:

				tLastDifference = tCurrentDifference;
			else:
				tCount2 = tCount2 + 1;
			#:endif
		else:
			tCurrentStart = tI2;
			tCount = 0;
			tCount2 = 1;
			tLastDifference = tCurrentDifference;
		#:endif:
	#:endfor:

	if(tCount2 < THRESHOLD):
		tTopAndBottom = crx.NULL;
		tCount = tCount + tCount2;
		
		tTopAndBottom = generateCTableLookUpFunction__generateTopAndBottomForNonHighlyRepeatingValues(
					pData, tTypeName, tValueOfUndefinedCharacter, tIsFirstIf, tCurrentStart, 
					tCurrentStart + tCount);
		
		vCountOfCharactersInLoopkupTables += tCount;

		tString__top += tTopAndBottom["top"];
		#tString__bottom += tTopAndBottom["bottom"];

		tRanges.append(generateCTableLookUpFunction__makeRangeEntry(tCurrentStart + tCount,
				tTopAndBottom["bottom"]));

		tIsFirstIf = False;
	else:
		tString = u"";

		if(tCount > 0):
			tTopAndBottom = generateCTableLookUpFunction__generateTopAndBottomForNonHighlyRepeatingValues(
					pData, tTypeName, tValueOfUndefinedCharacter, tIsFirstIf, tCurrentStart, 
					tCurrentStart + tCount);

			vCountOfCharactersInLoopkupTables += tCount;

			tString__top += tTopAndBottom["top"];
			#tString__bottom += tTopAndBottom["bottom"];

			tRanges.append(generateCTableLookUpFunction__makeRangeEntry(tCurrentStart + tCount,
				tTopAndBottom["bottom"]));

			tIsFirstIf = False;
		#:endif:

		tString = generateCTableLookUpFunction__generateBottomForHighlyRepeatingValues(pData,
				tIsFirstIf, tLastDifference, tLargestKey + 1 - tCount2, tLargestKey + 1,
				pValueOfUndefined);

		#tString__bottom = tString;
				
		tRanges.append(generateCTableLookUpFunction__makeRangeEntry(tCurrentStart + tCount, tString));

		tIsFirstIf = False;
	#:endif:
	
	if(IS_TO_USE_TYPE_COMPRESSION):
		tString__bottom += ( 
	u"	uint32_t vReturn /*= ?*/;\r\n");
	#:endif:
		

	if(not(tIsFirstIf)):
		tString__bottom += (u"\r\n" +
			generateCTableLookUpFunction__resolveRanges(IS_TO_USE_TYPE_COMPRESSION, 
			pValueOfUndefined, tRanges, 4, 1));
	else:
		#SHOULD NOT HAPPEN
		tString__bottom += (u"\r\n" +
	u"	return " + unicode(pValueOfUndefined) + u";");
	#:endif:

	vString += tString__top + tString__bottom;

	if(IS_TO_USE_TYPE_COMPRESSION):
		vString += (
	u"\r\n" +
	u"	return ((vReturn != " + unicode(tValueOfUndefinedCharacter) + 
			u") ? vReturn : " + unicode(pValueOfUndefined) + u");\r\n");
	#:endif:

	vString += (
	u"}\r\n\r\n");

	vReturn["code"] = vString;
	vReturn["countOfCharactersInLoopkupTables"] = vCountOfCharactersInLoopkupTables;

	return vReturn;
#:enddef:
def generateCTableLookUpFunction__generateTopAndBottomForNonHighlyRepeatingValues(pYml, pTypeName, 
		pValueOfUndefinedCharacter, pIsFirstIf, pStart, pEndExclusive):
	vString = u"";
	vString__bottom = u"";
	vIfString = u"else if";
	vCount = 0;
	tString = u"";
	vIsFirst = True;
	vVariableName = ("vValues" + unicode(pStart) + 
					u"To" + unicode(pEndExclusive - 1));

	if(pIsFirstIf):
		vIfString = u"if";
	#:endif

	vString += (
	u"	static " + pTypeName + u" " + vVariableName + "[" + 
			unicode(pEndExclusive - pStart) + u"] = \r\n" +			
	u"	{\r\n" + 
	u"		");

	for tI in six.moves.range(pStart, pEndExclusive, 1):
		if(not(vIsFirst)):
			if(vCount > 0):
				tString += u",";
			else:
				tString += (u",\r\n" +
	u"		");
			#:endif:
		else:
			vIsFirst = False;
		#:endif:

		if(not(tI in pYml)):
			tString += u"{:>8}".format(pValueOfUndefinedCharacter);	#private use unicode character U+10F447
		else:
			tString += u"{:>8}".format(pYml[tI]);
		#:endif:

		vCount += 1;

		if(vCount == 10):
			vString += tString;
			tString = u"";
			vCount = 0;
		#:endif:
	#:endfor:

	if(tString != ""):
		vString += (
			tString + "\r\n");
		tString = u"";
	#:endif:

	vString += (
	u"	};\r\n");
		
	#vString__bottom += (
	#u"	" + vIfString + u"(pValue < " + unicode(pEndExclusive) + u")\r\n"
	#u"		{return " + vVariableName + u"[pValue - " + unicode(pStart) + "];}\r\n");

	vString__bottom += vVariableName + u"[pValue - " + unicode(pStart) + u"]";
	
		
	return {"top": vString, "bottom": vString__bottom};
#:enddef:
def generateCTableLookUpFunction__generateBottomForHighlyRepeatingValues(pYml, pIsFirstIf, pValue, pStart, 
		pEndExclusive, pValueOfUndefined):
	vIfString = u"else if";
	vReturnString = u"";

	if(pIsFirstIf):
		vIfString = u"if";
	#:endif
	
	if(pValue == 0):
		vReturnString = u"pValue";
	elif(pValue == pValueOfUndefined):
		vReturnString = unicode(pValueOfUndefined);
	else:
		vReturnString = u"pValue + " + unicode(pValue);
	#:endif:

	#return (
	#u"	" + vIfString + "(pValue < " + unicode(pEndExclusive) + ")\r\n" +
	#u"		{return " + vReturnString + u";}\r\n");
	
	return vReturnString;
#:enddef:
def generateCTableLookUpFunction__makeRangeEntry(pLimit, pReturnString):
	return {"limit": pLimit, "returnString": pReturnString};
#:enddef:
def generateCTableLookUpFunction__resolveRanges(pIsToUseTypeCompression, pValueOfUndefined, pRanges, pCostLimit, 
		pIndentationDepth):
	return generateCTableLookUpFunction__resolveRanges__do(pIsToUseTypeCompression, 
			pValueOfUndefined, pRanges, pCostLimit, 0, pIndentationDepth, 0, len(pRanges) - 1, 
			False);
#:enddef:
def generateCTableLookUpFunction__resolveRanges__do(pIsToUseTypeCompression, pValueOfUndefined,
		pRanges, pCostLimit, pCurrentCost, pIndentationDepth, pStart, pEnd, pIsExtraElseRquired):
	vReturn = u"";

	if(((pCurrentCost + (pEnd - pStart + 1)) < pCostLimit) or ((pEnd - pStart + 1) < 4)):
		tIfElse = u"if";
		tReturnPre = u"return";

		if(pIsToUseTypeCompression):
			tReturnPre = u"vReturn =";
		#:endif:

		for tI in six.moves.range(pStart, pEnd + 1, 1):
			tScopeHeader = u"";
		
			if(pIsExtraElseRquired or (tI < pEnd)):
				tScopeHeader = tIfElse + u"(pValue < " + unicode(pRanges[tI]["limit"]) + u")";
			else:
				tScopeHeader = u"else";
			#:endif:
			
			vReturn += (
					(u"	" * pIndentationDepth) + tScopeHeader + u"\r\n" + 
					(u"	" * (pIndentationDepth + 1)) + u"{" + tReturnPre + u" " + 
							pRanges[tI]["returnString"] + u";}\r\n");
			tIfElse = u"else if";
		#:endfor:

		if(pIsExtraElseRquired):
			vReturn += (
					(u"	" * pIndentationDepth) + u"else\r\n" + 
					(u"	" * (pIndentationDepth + 1)) + u"{" + tReturnPre + u" " + 
							unicode(pValueOfUndefined) + u";}\r\n");
		#:endif:
	else:
		tIndex = int(math.floor((pEnd - pStart + 1) / 2)) + pStart - 1;
		
		vReturn += (
		(u"	" * pIndentationDepth) + u"if(pValue < " + unicode(pRanges[tIndex]["limit"]) + u")\r\n" +
		(u"	" * pIndentationDepth) + u"{\r\n" +
											generateCTableLookUpFunction__resolveRanges__do(
											pIsToUseTypeCompression, pValueOfUndefined,
											pRanges, pCostLimit, pCurrentCost + 1, 
											pIndentationDepth + 1, pStart, 
											tIndex, False) +
		(u"	" * pIndentationDepth) + u"}\r\n" +
		(u"	" * pIndentationDepth) + u"else\r\n" +
		(u"	" * pIndentationDepth) + u"{\r\n" +
											generateCTableLookUpFunction__resolveRanges__do(
											pIsToUseTypeCompression, pValueOfUndefined,
											pRanges, pCostLimit, pCurrentCost + 1, 
											pIndentationDepth + 1, tIndex + 1, 
											pEnd, True) +
		(u"	" * pIndentationDepth) + u"}\r\n");
	#:endif:
	
	return vReturn;
#:enddef:


if __name__ == '__main__':
	doMain(u"C:\\Programs\\Python\\Projects\\Tests\\e.h");

	six.moves.input("Press Enter To Continue..")
#:endif: