import sys
sys.path.insert(0, "C:\Programs\python\Projects\crx\lib\\")
sys.path.insert(0, "C:\Programs\python\libraries\\")

import getopt
import os
import crx.utility
import codecs
import six
import pyaml
import sys
import math
import fractions
import json
if(sys.version_info > (2,7)):
    from collections import OrderedDict
else:
    from ordereddict import OrderedDict
#:endif:


gUnicodeCaseFoldingFilePath = unicode(crx.utility.enforcePathAsDirectoryPath(sys.path[2]) + u"data" +
		os.sep + "CaseFolding.txt");
gUnicodeCaseFoldingData = crx.NULL;
gProcessedUnicodeCaseFoldingData = crx.NULL;
gUnicodeFilePath = unicode(crx.utility.enforcePathAsDirectoryPath(sys.path[2]) + u"data" +
		os.sep + "UnicodeData.txt");
gUnicodeData = crx.NULL;
pyaml.warnings({'YAMLLoadWarning': False});

#VERBATIM FROM: generaceCCodeFoUnicodeUnfo.py
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

def getUnicodeCaseFoldingData():
	global gUnicodeCaseFoldingData;

	if(gUnicodeCaseFoldingData == crx.NULL):
		if(not os.path.isfile(gUnicodeCaseFoldingFilePath)):
			print(u"ERROR: Unicode Case Folding file, " + gUnicodeCaseFoldingFilePath + u" not found.\n");
		else:
			tUnicodeData = getUnicodeData();
			tFile = codecs.open(gUnicodeCaseFoldingFilePath, encoding='utf-8');
			tLine = tFile.readline();

			gUnicodeCaseFoldingData = OrderedDict();
			
			while(tLine != ""):
				if((tLine[0] == "#") or (len(tLine) < 2)):
					tLine = tFile.readline();
				else:
					tRawData = tLine.split(u";");
					tInputCodePoint = int(tRawData[0], 16);
					tOutputCodePointStrings = tRawData[2].strip().split(u" ");
					tOutputCodePoints = [];

					for tI in six.moves.range(0, len(tOutputCodePointStrings), 1):
						tOutputCodePoints.append(int(tOutputCodePointStrings[tI], 16));
					#:endif:
					
					if(not(tInputCodePoint in gUnicodeCaseFoldingData)):
						gUnicodeCaseFoldingData[tInputCodePoint] = [];
					#:endif:

					#"CAPITAL" CAN APPEAR WITH "SMALL" IN THE SAME CHARACTER DESCRIPTION,
					#		IN THAT CASE THE CHARACTER IS CAPITAL FROM WHAT I EXAMINED.
					#IF NEITHER "CAPITAL" NOR "SMALL" APPEARS IN THE SAME CHARACTER
					#		DESCRIPTION, THE CHARACTER IS ASSUMED CAPITAL
					gUnicodeCaseFoldingData[tInputCodePoint].append({
							u"inputCodePoint": tInputCodePoint, 
							u"outPutCodePoints": tOutputCodePoints, 
							u"type": tRawData[1].strip(), 
							u"isInputCodePointAnUpper": (not(u"SMALL" in tRawData[3]) or
									(u"CAPITAL" in tRawData[3]))});

					tLine = tFile.readline();
				#:endif:
			#:endwhile:
			
			tFile.close();
		#:endif:
	#:endif:

	return gUnicodeCaseFoldingData;
#:enddef:

def verifyAndGetProcessedUnicodeCodeFoldingData():
	global gProcessedUnicodeCaseFoldingData;
	IS_ENABLING_COMPOSITES = True;
	vUnicodeCaseFoldingData = getUnicodeCaseFoldingData();
	#vExceptionCharacters = 0412, 0432, 1C80, 1C81, 0414, 0434, 1C81, 041E, 043E, 1C82
	vRemovedCharacters = {}; 	#THIS IS FOR CHARACTERS THAT MUST NEVER APPEAR REVERSIBLE. SOME
								#		CHARACTERS GET REMOVED IN THE ALGORITHM BUT A VALID
								#		REVERSIBLE SITUATION IS FOUND. THESE CHARACTER ARE NOT IT.

	if(gProcessedUnicodeCaseFoldingData == crx.NULL):
		tUnicodeData = getUnicodeData();
		tLowerToUpper = OrderedDict();
		tUpperToLower = OrderedDict();
		tCodePointsOfOtherFoldings = {};

		gProcessedUnicodeCaseFoldingData = OrderedDict();
		
		# NOTES:
		# - INPUT IS THE CHARACTERS ON THE LEFT IN THE FILE "CaseFolding.txt", AND OUTPUT
		#		ARE THOSE ON THE RIGHT.
		# ASSUMPTIONS:
		# - GIVEN THAT WE ARE ONLY DEALING WITH CHARACTERS THAT DO NOT HAVE A CANONICAL 
		#		DECOMPOSITION, THE FOLLOWING IS ASSUMED TO HOLD:
		# 	- IF THE FOLDING TYPE IS 'C', THEN THE OUTPUT HAS LENGTH 1 CODEPOINT, OTHERWISE LARGER 
		#			THAN 1 CODEPOINT.
		# 	- IF THE FOLDING TYPE IS NEITHER 'C' NOR 'F', THERE EXISTS EITHER A 'C' OR 'F'
		#			FOLDING FOR THE SAME INPUT.
		# 	- IF THE OUTPUT IS A SINGLE CODE POINT, IT NEVER APPEARS AS AN INPUT.
		# - IF THE OUTPUT IS A SINGLE CODE POINT, AND APPEARS AS A SINGLE CODE POINT
		#		FOR MORE THAN ONE INPUT, THEN ALL THESE INPUTS ARE EITHER NOT FOR 'C' NOR
		#		FOR 'F', OR ARE FOR EITHER 'C' OR 'F' BUT ALL OF THEM HAVE A COMPATIBILITY
		#		OR A CANONICAL DECOMPOSITION EXCEPT FOR ONE INPUT.
		#		<<THIS ASSUMPTION DOES NOT HOLD TRUE.>>
		#				THE FOLLOWING OUTPUT CHARACTERS VIOLATE THIS ASSUMPTION:
		#				01C6(454), 01C9(457), 01CC(460), 01F3(499)
		#				WE CAN FORBID THE FOLLOWING INPUT CHARACTERS ALL TOGETHER ALONG WITH
		#				THE ABOVE OUTPUT CHARACTERS
		#				01C4(452), 01C5(453)		(THESE YIELD 01C6(454))
		#				01C7(455), 01C8(456)		(THESE YIELD 01C9(457))
		#				01CA(458), 01CB(459)		(THESE YIELD 01CC(460))
		#				01F1(497), 01F2(498)		(THESE YIELD 01F3(499))
		#				USING fastunicodecompare() FROM APPLE, CONFIRMED THAT HE ABOVE
		#				WOULD BE RECOGNIZED IN HFS. FOR EXAMPLE, 01C4(452), 01C5(453) AND 01C6(454) 
		#				WOULD RECOGNIZED AS THE SAME CHARACTER. HENCE THESE CHARACTERS WOULD CONTINUE
		#				BEING ALLOWED, BUT WOULD NOT BE CONSIDERED CASE REVERSIBLE.
		#				
		#				THE CHARACTER 03B9(953) VIOLATES THE ASSUMPTION AND IS AN OUTPUT FOR 
		#				0345(837) AND 0399(921). HOWEVER, THE CHARACTER 0345(837) IS NOT RECOGNIZED 
		#				AS EQUIVILANT TO OTHER TWO, NEITHER UNDER WINDOWS, NOR UNDER 
		#				fastunicodecompare().
		#				
		#				THE INPUT CHARACTERS, &#x1C80; TO &#x1C87; ARE NOT RECOGNIZED TO BE THE SAME
		#				UNDER WINDOWS AND "fastunicodecompare.c", TO THE OUTPUT CHARACTERS. FOR 
		#				EXAMPLE, &#x1C80; IS NOT RECOGNIZED THE SAME AS &#x0432; NOR AS &#x0412;, 
		#				BUT &#x0432; IS RECOGNIZED THE SAME AS &#x0432;. THESE CHARACTERS DO NOT 
		#				HAVE A CANONICAL DECOMPOSITION NOR A COMPATIBILITY DECOMPOSITION, WHILE 
		#				THEIR OUTPUT CHARACTER WHICH WAS SOMETIMES UPPER CASE, AND SOMETIMES LOWER 
		#				CASE.		
		# - IF THE OUTPUT IS A SINGLE CODE POINT, AND APPEARS AS A SINGLE CODE POINT
		#		FOR MORE THAN ONE INPUT, THEN FOR ALL THESE INPUTS THE OUTPUT IS FOR AN
		#		UPPER CASE, OR FOR ALL THESE INPUTS THE OUTPUT IS FOR A LOWER CASE.
		#		<<THIS ASSUMPTION DOES NOT HOLD TRUE.>>
		#				THE CHARACTER 03C3 VILATES THIS ASSUMPTION BY BEING A LOWER CASE TO
		#				03A3 AND AN UPPER CASE TO 03C2.
		#				HOWEVER NEITHER WINDOWS, NOR "fastunicodecompare.c", RECOGNIZE
		#				03C2 TO BE EQUIVILANT TO THE OTHER TWO.
		# - IF A CHARACTER IS LOWER CASE, IT IS LOWER CASE UNDER ALL CIRCUMSTANCES, AND IF IT
		#		IS UPPER CASE, IT IS UPPER CASE UNDER ALL CIRCUMSTANCES.
		#		<<THIS ASSUMPTION DOES NOT HOLD TRUE.>>
		#				THE CHARACTER U+0073 IS LOWER CASE FOR U+0053, BUT IS UPPER CASE FOR
		#				U+017F
		#
		# UPDATE:
		# - DECIDEDED TO ALLOW COMPOSITE CHARACTERS IN THE DATA COLLECTED, BUT IT WILL STILL MEANT
		#		FOR THE FILE SYSTEM WORK
		#		WITH THIS NOTICE THE FOLLOWING ISSUES:
		#	- U+00E5 APPEARS AS THE LOWER CASE TO U+00C5 AND U+212B. WINDOWS
		#			DOES NOT RECOGNIZE U+212B TO BE THE SAME AS THE OTHER TWO.
		#	- U+1E61 APPEARS AS THE LOWER CASE TO U+1E60 AND UPPER CASE TO U+1E9B(7835). WINDOWS
		#			DOES NOT RECOGNIZE U+1E9B(7835) TO BE THE SAME AS THE OTHER TWO.
		# - CONFIRMED THAT ALLOWING COMPOSITE CHARACTERS IN THE DATA COLLECTED ONLY ADDED NEW
		#		ENTRIES, AND DID NOT REMOVE EXISTING ONES.
		
		for tKey in vUnicodeCaseFoldingData.keys():
			for tI in six.moves.range(0, len(vUnicodeCaseFoldingData[tKey]), 1):
				tIsValid = True;

				if((IS_ENABLING_COMPOSITES or ((tUnicodeData[tKey][5][0] != "<>") and 
								((tKey < 44032) or (tKey > 55203)))) and
						((vUnicodeCaseFoldingData[tKey][tI][u"type"] == u"C") or 
						(vUnicodeCaseFoldingData[tKey][tI][u"type"] == u"F"))): #and
						#(tKey != 837) and 
						#(not((tKey >= 7296) and (tKey <= 7303))) and
						#(tKey != 962)
						# and ((tKey != 7304) and (tKey != 42570) and (tKey != 42571))
						#(tKey != 7304)): 
					if(len(vUnicodeCaseFoldingData[tKey][tI][u"outPutCodePoints"]) == 1):
						if(vUnicodeCaseFoldingData[tKey][tI][u"type"] == u"C"):
							if(vUnicodeCaseFoldingData[tKey][tI][u"isInputCodePointAnUpper"]):
								if(tKey == 383):
									print('ESS');
								#:endif:
								#WARNING: THIS COMMENT WRITTEN SOME TIME LATER
								#VIOLATION, CODE POINT IS AN UPPER TO MORE THAN ONE THING
								if(tKey in tUpperToLower):
									sys.exit(u"UNEXPECTED SITUATION (1): CODE ASSUMPTIONS ARE WRONG");
									tIsValid = False;
								#WARNING: THIS COMMENT WRITTEN SOME TIME LATER
								#VIOLATION, CODE POINT WAS SEEN BEFORE AS A LOWER
								elif(tKey in tLowerToUpper):
									sys.exit(u"UNEXPECTED SITUATION (2): CODE ASSUMPTIONS ARE WRONG");
									tIsValid = False;
								#WARNING: THIS COMMENT WRITTEN SOME TIME LATER
								#VIOLATION, THE OUTPUT IS NOT COMPLETELY FOLDED (I THINK)
								elif(vUnicodeCaseFoldingData[tKey][tI][u"outPutCodePoints"][0] in 
										gProcessedUnicodeCaseFoldingData):
									sys.exit(u"UNEXPECTED SITUATION (3): CODE ASSUMPTIONS ARE WRONG");
									tIsValid = False;
								else: 
									tIsReversible = True;

									if(vUnicodeCaseFoldingData[tKey][tI][u"outPutCodePoints"][0] in 
											tLowerToUpper):
										tOtherUpperCodePoint = tLowerToUpper[
												vUnicodeCaseFoldingData[tKey][tI][
												u"outPutCodePoints"][0]];

										if(tUnicodeData[tOtherUpperCodePoint][5][0] == ""):
											if(tUnicodeData[tKey][5][0] == ""):
												#WARNING: THIS COMMENT WRITTEN SOME TIME LATER
												#AT THIS POINT, NEITHER THIS CODE POINT, NOR THE
												#		THE OTHER UPPER CODE POINT ARE COMPOSITE
												#		CHARACTERS. WE HAVE TO REMOVE SUCH
												#		CHARACTERS AS REVERSIBLE CASE CONVERSIONS. 
												#		BUT THEY REMAIN VALID AS PART OF CASE
												#		FOLDING FOR STRING COMPARISON, MEANING
												#		tIsValid IS STILL TRUE. REMEMBER THAT 
												#		WHATEVER WE DO WE MUST NOT AFFECT THE 
												#		INTEGRITY OF THE COMPARISON FUNCTION THAT 
												#		WILL BE USED TO IMPLEMENT THE CHECKING 
												#		FUNCTION IN C, AS THE CHECKING FUNCTION IS 
												#		DEFINED IN MY FILE SYSTEM WORK.
												#		THE FOLLOWING IS SIMPLY MAKING SURE THAT WE 
												#		KNOW EXACTLY WHAT WE ARE REMOVING. MEANING
												#		WE CHECKED THAT WE WOULD NOT BE VIOLATING 
												#		THE INTGERITY OF THE CHECKING FUNCTION THAT
												#		IS TO BE IMPLEMENTED IN C.
												if((tKey == 837) or (tKey == 921) or 
														(tKey == 7304) or (tKey == 42570)):
													#WARNING: ONLY INCLUDE INPUT CHARACTERS
													#		IN THE IF STATEMENT ABOVE. THIS IS TO
													#		MAKE SURE THAT IF YOU EVER UPDATE THE
													#		UNICODE DATA, YOU ARE NOT CAUGHT BY
													#		SUPRISE BY CHARACTERS YOU DID NOT
													#		CAREFULY EXAMINE
													vRemovedCharacters[tKey] = True;
													vRemovedCharacters[tOtherUpperCodePoint] = True;
													vRemovedCharacters[vUnicodeCaseFoldingData[tKey][
															tI][u"outPutCodePoints"][0]] = True;
															
													#if(tKey == 921):
													#	sys.exit("FOUND");

													del tLowerToUpper[vUnicodeCaseFoldingData[tKey][
															tI][u"outPutCodePoints"][0]];
													del tUpperToLower[tOtherUpperCodePoint];
													tIsReversible = False;
												else:
													#WARNING: THIS COMMENT WRITTEN SOME TIME LATER
													#A NEW CODE POINT THAT WE DID NOT KNOW ABOUT
													#		BEFORE CAUSED THE VIOLATION ABOVE.
													#		UNTIL WE INVESTIGATE, THE CODE POINT IS
													#		DEEMED INVALID. AND WE SHOULD 
													#		INVESTIGATE BECAUSE OTHERWISE WE
													#		AFFECT THE INTEGRITY OF THE CHECKING
													#		FUNCTION THAT SHALL BE IMPLEMENTED IN C.
													sys.exit(u"UNEXPECTED SITUATION (4): CODE " + 
															"ASSUMPTIONS ARE WRONG " + 
															u"#{0:x}".format(tKey) + u" => " + 
															u"#{0:x}".format(vUnicodeCaseFoldingData[
															tKey][tI][u"outPutCodePoints"][0]));
													tIsValid = False;
												#:endif:
											else:
												#WARNING: THIS COMMENT WRITTEN SOME TIME LATER
												#THE CODE POINT WE ARE PROCESSING CAN BE IGNORED.
												#		THE OTHER 'UPPER' WE FOUND BEFORE IS NOT A
												#		COMPOSED CHARACTER, BUT THIS ONE IS.
												print(u"IGNORING ENTRY FOR {" + 
														u"#{0:x}".format(tKey) + u" => " + 
														u"#{0:x}".format(vUnicodeCaseFoldingData[
														tKey][tI][u"outPutCodePoints"][0]) + u"} " +
														u"BECAUSE OF ENTRY FOR " + 
														u"#{0:x}".format(tOtherUpperCodePoint));
												tIsValid = False;
											#:endif
											
											tIsReversible = False;
										else:
											if(tUnicodeData[tKey][5][0] != ""):
												#WARNING: THIS COMMENT WRITTEN SOME TIME LATER
												#AT THIS POINT, BOTH THIS CODE POINT, AND THE
												#		THE OTHER UPPER CODE POINT ARE COMPOSITE
												#		CHARACTERS. WE HAVE TO REMOVE SUCH
												#		CHARACTERS AS REVERSIBLE CASE CONVERSIONS. 
												#		BUT THEY REMAIN VALID AS PART OF CASE
												#		FOLDING FOR STRING COMPARISON, MEANING
												#		tIsValid IS STILL TRUE. REMEMBER THAT 
												#		WHATEVER WE DO WE MUST NOT AFFECT THE 
												#		INTEGRITY OF THE COMPARISON FUNCTION THAT 
												#		WILL BE USED TO IMPLEMENT THE CHECKING 
												#		FUNCTION IN C, AS THE CHECKING FUNCTION IS 
												#		DEFINED IN MY FILE SYSTEM WORK.
												#		THE FOLLOWING IS SIMPLY MAKING SURE THAT WE 
												#		KNOW EXACTLY WHAT WE ARE REMOVING. MEANING
												#		WE CHECKED THAT WE WOULD NOT BE VIOLATING 
												#		THE INTGERITY OF THE CHECKING FUNCTION THAT
												#		IS TO BE IMPLEMENTED IN C.
												if((tKey == 452) or (tKey == 453) or 
														(tKey == 456) or (tKey == 457) or
														(tKey == 459) or (tKey == 460) or
														(tKey == 498) or
														#COMPOSITE CHARACTERS NEXT
														#212B
														(IS_ENABLING_COMPOSITES and
														((tKey == 8491)))):
													#WARNING: ONLY INCLUDE INPUT CHARACTERS
													#		IN THE IF STATEMENT ABOVE. THIS IS TO
													#		MAKE SURE THAT IF YOU EVER UPDATE THE
													#		UNICODE DATA, YOU ARE NOT CAUGHT BY
													#		SUPRISE BY CHARACTERS YOU DID NOT
													#		CAREFULY EXAMINE
													vRemovedCharacters[tKey] = True;
													vRemovedCharacters[tOtherUpperCodePoint] = True;
													vRemovedCharacters[vUnicodeCaseFoldingData[tKey][
															tI][u"outPutCodePoints"][0]] = True;

													del tLowerToUpper[vUnicodeCaseFoldingData[tKey][
															tI][u"outPutCodePoints"][0]];
													del tUpperToLower[tOtherUpperCodePoint];
													tIsReversible = False;
												else:
													#WARNING: THIS COMMENT WRITTEN SOME TIME LATER
													#A NEW CODE POINT THAT WE DID NOT KNOW ABOUT
													#		BEFORE CAUSED THE VIOLATION ABOVE.
													#		UNTIL WE INVESTIGATE, THE CODE POINT IS
													#		DEEMED INVALID. AND WE SHOULD 
													#		INVESTIGATE BECAUSE OTHERWISE WE
													#		AFFECT THE INTEGRITY OF THE CHECKING
													#		FUNCTION THAT SHALL BE IMPLEMENTED IN C.
													sys.exit(u"UNEXPECTED SITUATION (5): CODE " + 
															"ASSUMPTIONS ARE WRONG " + 
															u"#{0:x}".format(tKey) + u" => " + 
															u"#{0:x}".format(vUnicodeCaseFoldingData[
															tKey][tI][u"outPutCodePoints"][0]));
													tIsValid = False;
												#:endif:
											else:
												#WARNING: THIS COMMENT WRITTEN SOME TIME LATER
												#THE CODE POINT WE ARE PROCESSING IS NOT A COMPOSED.
												#		CODE POINT, UNLIKE THE OTHER 'UPPER' WE 
												#		FOUND BEFORE. WE REMOVE THE OTHER UPPER,
												#		AND KEEP THIS ONE.
												print(u"OVERIDING ENTRY FOR " + 
														u"#{0:x}".format(tOtherUpperCodePoint) + 
														u" WITH ENTRY FOR {" + 
														u"#{0:x}".format(tKey) + u" => " + 
														u"#{0:x}".format(vUnicodeCaseFoldingData[
														tKey][tI][u"outPutCodePoints"][0]) + u"}");
												del gProcessedUnicodeCaseFoldingData[
														tOtherUpperCodePoint];
												del tLowerToUpper[vUnicodeCaseFoldingData[tKey][
														tI][u"outPutCodePoints"][0]];
												del tUpperToLower[tOtherUpperCodePoint];
											#:endif
										#:endif:
									elif(vUnicodeCaseFoldingData[tKey][tI][u"outPutCodePoints"][0] in 
											tUpperToLower):
										tLowerCodePoint = tUpperToLower[vUnicodeCaseFoldingData[
												tKey][tI][u"outPutCodePoints"][0]];

										print(u"ENCOUNTERED CHARACTER THAT IS BOTH LOWER AND " + 
												u"UPPER CASE " + 
												u"#{0:x}".format(vUnicodeCaseFoldingData[tKey][tI][
												u"outPutCodePoints"][0]));

										#WARNING: THIS COMMENT WRITTEN SOME TIME LATER
										#AT THIS POINT OUR CURRENT CODE POINT IS AN UPPER TO A
										#		CODE POINT, 'X', SUCH AS ANOTHER CODE POINT IS A 
										#		LOWER TO 'X'.
										#		IN THIS CASE WE TRY TO SEE IF ONE OF THE CONVERSIONS
										#		CAN BE IGNORED FOR THE PROBLOMATIC CODE POINT 'X'. 
										#		THIS HAPPENS IF EITHER THE CURRENT CODE POINT, OR
										#		THE CODE POINT THAT IS A LOWER TO 'X' IS A NON 
										#		COMPOSED CODE POINT, BUT NOT THE OTHER.
										if(tUnicodeData[tLowerCodePoint][5][0] == ""):
											if(tUnicodeData[tKey][5][0] == ""):
												#WARNING: THIS COMMENT WRITTEN SOME TIME LATER
												#CURRENTLY WE DO NOT KNOW CHARACTERS VIOLATING
												#		THIS SITUATION, BUT IF WE DID WE MUST
												#		EXPLICITLY DEAL WITH THEM LIKE IN THE CASES
												#		ABOVE. MEANING YOU NEED TO ADD A NEW IF ELSE
												#		BLOCK HERE.
												sys.exit(u"UNEXPECTED SITUATION (6): CODE " + 
														"ASSUMPTIONS ARE WRONG");
												tIsValid = False;
											else:
												#WARNING: THIS COMMENT WRITTEN SOME TIME LATER
												#THE CODE POINT WE ARE PROCESSING IS A COMPOSED CODE
												#		POINT, UNLIKE THE 'LOWER' WE
												#		FOUND BEFORE. WE IGNORE THE CURRENT 'UPPER' 
												#		CODE POINT.
												print(u"IGNORING ENTRY FOR {" + 
														u"#{0:x}".format(tKey) + u" => " + 
														u"#{0:x}".format(vUnicodeCaseFoldingData[
														tKey][tI][u"outPutCodePoints"][0]) + u"} " +
														u"BECAUSE OF ENTRY FOR " + 
														u"#{0:x}".format(tLowerCodePoint));
												tIsValid = False;
											#:endif
											
											tIsReversible = False;
										else:
											if(tUnicodeData[tKey][5][0] != ""):
												#WARNING: THIS COMMENT WRITTEN SOME TIME LATER
												#CURRENTLY WE DO NOT KNOW CHARACTERS VIOLATING
												#		THIS SITUATION, BUT IF WE DID WE MUST
												#		EXPLICITLY DEAL WITH THEM LIKE IN THE CASES
												#		ABOVE. MEANING YOU NEED TO ADD A NEW IF ELSE
												#		BLOCK HERE.
												sys.exit(u"UNEXPECTED SITUATION (7): CODE " + 
														"ASSUMPTIONS ARE WRONG " + 
														u"#{0:x}".format(tKey));
												tIsReversible = False;
											else:
												#WARNING: THIS COMMENT WRITTEN SOME TIME LATER
												#THE CODE POINT WE ARE PROCESSING IS NOT A COMPOSED 
												#		CODE POINT, UNLIKE THE 'LOWER' WE
												#		FOUND BEFORE. WE REMOVE THE OTHER 'LOWER' 
												#		CODE POINT, AND KEEP THE CURRENT 'UPPER'
												#		CODE POINT.
												print(u"OVERIDING ENTRY FOR " + 
														u"#{0:x}".format(tLowerCodePoint) + 
														u" WITH ENTRY FOR {" + 
														u"#{0:x}".format(tKey) + u" => " + 
														u"#{0:x}".format(vUnicodeCaseFoldingData[
														tKey][tI][u"outPutCodePoints"][0]) + u"}");
												del gProcessedUnicodeCaseFoldingData[
														tLowerCodePoint];
												del tUpperToLower[vUnicodeCaseFoldingData[
														tKey][tI][u"outPutCodePoints"][0]];
												del tLowerToUpper[tLowerCodePoint];
											#:endif
										#:endif:
									#:endif
									
									if(tIsReversible and 
											(vUnicodeCaseFoldingData[tKey][tI][
											u"outPutCodePoints"][0] in vRemovedCharacters)):
										tIsReversible = False;
									#:endif:

									if(tIsReversible):
										tUpperToLower[tKey] = vUnicodeCaseFoldingData[
												tKey][tI][u"outPutCodePoints"][0];
										tLowerToUpper[vUnicodeCaseFoldingData[tKey][tI][
												u"outPutCodePoints"][0]] = tKey;
									#:endif:
								#:endif:
							else:
								if(tKey == 383):
									print('ess');
								#:endif:
								#WARNING: THIS COMMENT WRITTEN SOME TIME LATER
								#VIOLATION, CODE POINT IS A LOWER TO MORE THAN ONE THING
								if(tKey in tLowerToUpper):
									sys.exit(u"UNEXPECTED SITUATION (8): CODE ASSUMPTIONS ARE WRONG");
									tIsValid = False;
								#WARNING: THIS COMMENT WRITTEN SOME TIME LATER
								#VIOLATION, CODE POINT WAS SEEN BEFORE AS AN UPPER
								elif(tKey in tUpperToLower):
									sys.exit(u"UNEXPECTED SITUATION (9): CODE ASSUMPTIONS ARE WRONG");
									tIsValid = False;
								#WARNING: THIS COMMENT WRITTEN SOME TIME LATER
								#VIOLATION, THE OUTPUT IS NOT COMPLETELY FOLDED (I THINK)
								elif(vUnicodeCaseFoldingData[tKey][tI][u"outPutCodePoints"][0] in 
										gProcessedUnicodeCaseFoldingData):
									sys.exit(u"UNEXPECTED SITUATION (10): CODE ASSUMPTIONS ARE WRONG");
									tIsValid = False;
								else: 
									tIsReversible = True;

									if(vUnicodeCaseFoldingData[tKey][tI][u"outPutCodePoints"][0] in 
											tUpperToLower):
										tOtherLowerCodePoint = tUpperToLower[
												vUnicodeCaseFoldingData[tKey][tI][
												u"outPutCodePoints"][0]];

										if(tUnicodeData[tOtherLowerCodePoint][5][0] == ""):
											if(tUnicodeData[tKey][5][0] == ""):
												#WARNING: THIS COMMENT WRITTEN SOME TIME LATER
												#CURRENTLY WE DO NOT KNOW CHARACTERS VIOLATING
												#		THIS SITUATION, BUT IF WE DID WE MUST
												#		EXPLICITLY DEAL WITH THEM LIKE IN THE CASES
												#		ABOVE. MEANING YOU NEED TO ADD A NEW IF ELSE
												#		BLOCK HERE.
												sys.exit(u"UNEXPECTED SITUATION (11): CODE " + 
														"ASSUMPTIONS ARE WRONG");
												tIsValid = False;
											else:
												#WARNING: THIS COMMENT WRITTEN SOME TIME LATER
												#THE CODE POINT WE ARE PROCESSING CAN BE IGNORED.
												#		THE OTHER 'LOWER' WE FOUND BEFORE IS NOT A
												#		COMPOSED CHARACTER, BUT THIS ONE IS.
												print(u"IGNORING ENTRY FOR {" + 
														u"#{0:x}".format(tKey) + u" => " + 
														u"#{0:x}".format(vUnicodeCaseFoldingData[
														tKey][tI][u"outPutCodePoints"][0]) + u"} " +
														u"BECAUSE OF ENTRY FOR " + 
														u"#{0:x}".format(tOtherLowerCodePoint));
												tIsValid = False;
											#:endif
											
											tIsReversible = False;
										else:
											if(tUnicodeData[tKey][5][0] != ""):
												#WARNING: THIS COMMENT WRITTEN SOME TIME LATER
												#CURRENTLY WE DO NOT KNOW CHARACTERS VIOLATING
												#		THIS SITUATION, BUT IF WE DID WE MUST
												#		EXPLICITLY DEAL WITH THEM LIKE IN THE CASES
												#		ABOVE. MEANING YOU NEED TO ADD A NEW IF ELSE
												#		BLOCK HERE.
												sys.exit(u"UNEXPECTED SITUATION (12): CODE " + 
														"ASSUMPTIONS ARE WRONG");
												tIsValid = False;
												tIsReversible = False;
											else:
												#WARNING: THIS COMMENT WRITTEN SOME TIME LATER
												#THE CODE POINT WE ARE PROCESSING IS NOT A COMPOSED.
												#		CODE POINT, UNLIKE THE OTHER 'LOWER' WE 
												#		FOUND BEFORE. WE REMOVE THE OTHER LOWER,
												#		AND KEEP THIS ONE.
												print(u"OVERIDING ENTRY FOR " + 
														u"#{0:x}".format(tOtherLowerCodePoint) + 
														u" WITH ENTRY FOR {" + 
														u"#{0:x}".format(tKey) + u" => " + 
														u"#{0:x}".format(vUnicodeCaseFoldingData[
														tKey][tI][u"outPutCodePoints"][0]) + u"}");
												del gProcessedUnicodeCaseFoldingData[
														tOtherLowerCodePoint];
												del tUpperToLower[vUnicodeCaseFoldingData[tKey][
														tI][u"outPutCodePoints"][0]];
												del tLowerToUpper[tOtherLowerCodePoint];
											#:endif
										#:endif:
									elif(vUnicodeCaseFoldingData[tKey][tI][u"outPutCodePoints"][0] in 
											tLowerToUpper):
										tUpperCodePoint = tLowerToUpper[vUnicodeCaseFoldingData[
												tKey][tI][u"outPutCodePoints"][0]];

										print(u"ENCOUNTERED CHARACTER THAT IS BOTH UPPER AND " + 
												u"LOWER CASE " + 
												u"#{0:x}".format(vUnicodeCaseFoldingData[tKey][tI][
												u"outPutCodePoints"][0]));

										#WARNING: THIS COMMENT WRITTEN SOME TIME LATER
										#AT THIS POINT OUR CURRENT CODE POINT IS A LOWER TO A
										#		CODE POINT, 'X', SUCH AS ANOTHER CODE POINT IS AN 
										#		UPPER TO 'X'.
										#		IN THIS CASE WE TRY TO SEE IF ONE OF THE CONVERSIONS
										#		CAN BE IGNORED FOR THE PROBLOMATIC CODE POINT 'X'. 
										#		THIS HAPPENS IF EITHER THE CURRENT CODE POINT, OR
										#		THE CODE POINT THAT IS A UPPER TO 'X' IS A NON 
										#		COMPOSED CODE POINT, BUT NOT THE OTHER.
										if(tUnicodeData[tUpperCodePoint][5][0] == ""):
											if(tUnicodeData[tKey][5][0] == ""):
												#WARNING: THIS COMMENT WRITTEN SOME TIME LATER
												#AT THIS POINT, BOTH THIS CODE POINT, AND THE
												#		THE OTHER UPPER CODE POINT ARE NOT COMPOSITE
												#		CHARACTERS. WE HAVE TO REMOVE SUCH
												#		CHARACTERS. THE FOLLOWING IS SIMPLY MAKING
												#		SURE THAT WE KNOW EXACTLY WHAT WE ARE 
												#		REMOVING.
												if((tKey == 1042) or (tKey == 1044) or 
														(tKey == 1054) or (tKey == 1057) or
														(tKey == 1058) or (tKey == 1066) or
														(tKey == 1122) or
														((tKey >= 7296) and (tKey <= 7303)) or
														(tKey == 931) or (tKey == 962)):
													#WARNING: ONLY INCLUDE INPUT CHARACTERS
													#		IN THE IF STATEMENT ABOVE. THIS IS TO
													#		MAKE SURE THAT IF YOU EVER UPDATE THE
													#		UNICODE DATA, YOU ARE NOT CAUGHT BY
													#		SUPRISE BY CHARACTERS YOU DID NOT
													#		CAREFULY EXAMINE
													vRemovedCharacters[tKey] = True;
													vRemovedCharacters[tUpperCodePoint] = True;
													vRemovedCharacters[vUnicodeCaseFoldingData[tKey][
															tI][u"outPutCodePoints"][0]] = True;

													del tLowerToUpper[vUnicodeCaseFoldingData[tKey][
															tI][u"outPutCodePoints"][0]];
													del tUpperToLower[tUpperCodePoint];
													tIsReversible = False;
												else:
													#WARNING: THIS COMMENT WRITTEN SOME TIME LATER
													#A NEW CODE POINT THAT WE DID NOT KNOW ABOUT
													#		BEFORE CAUSED THE VIOLATION ABOVE.
													sys.exit(u"UNEXPECTED SITUATION (13): CODE " + 
															"ASSUMPTIONS ARE WRONG " + 
															u"#{0:x}".format(tKey) + u" => " + 
															u"#{0:x}".format(vUnicodeCaseFoldingData[
															tKey][tI][u"outPutCodePoints"][0]));
													tIsValid = False;
												#:endif:
											else:
												#WARNING: THIS COMMENT WRITTEN SOME TIME LATER
												#THE CODE POINT WE ARE PROCESSING IS A COMPOSED CODE
												#		POINT, UNLIKE THE 'UPPER' WE
												#		FOUND BEFORE. WE IGNORE THE CURRENT 'LOWER' 
												#		CODE POINT.
												print(u"IGNORING ENTRY FOR {" + 
														u"#{0:x}".format(tKey) + u" => " + 
														u"#{0:x}".format(vUnicodeCaseFoldingData[
														tKey][tI][u"outPutCodePoints"][0]) + u"} " +
														u"BECAUSE OF ENTRY FOR " + 
														u"#{0:x}".format(tUpperCodePoint));
												tIsValid = False;
											#:endif
											
											tIsReversible = False;
										else:
											if(tUnicodeData[tKey][5][0] != ""):
												#WARNING: THIS COMMENT WRITTEN SOME TIME LATER
												#AT THIS POINT, BOTH THIS CODE POINT, AND THE
												#		THE OTHER UPPER CODE POINT ARE COMPOSITE
												#		CHARACTERS. WE HAVE TO REMOVE SUCH
												#		CHARACTERS AS REVERSIBLE CASE CONVERSIONS. 
												#		BUT THEY REMAIN VALID AS PART OF CASE
												#		FOLDING FOR STRING COMPARISON, MEANING
												#		tIsValid IS STILL TRUE. REMEMBER THAT 
												#		WHATEVER WE DO WE MUST NOT AFFECT THE 
												#		INTEGRITY OF THE COMPARISON FUNCTION THAT 
												#		WILL BE USED TO IMPLEMENT THE CHECKING 
												#		FUNCTION IN C, AS THE CHECKING FUNCTION IS 
												#		DEFINED IN MY FILE SYSTEM WORK.
												#		THE FOLLOWING IS SIMPLY MAKING SURE THAT WE 
												#		KNOW EXACTLY WHAT WE ARE REMOVING. MEANING
												#		WE CHECKED THAT WE WOULD NOT BE VIOLATING 
												#		THE INTGERITY OF THE CHECKING FUNCTION THAT
												#		IS TO BE IMPLEMENTED IN C.
												if(IS_ENABLING_COMPOSITES and (tKey == 7835)): #1E9B
													#WARNING: ONLY INCLUDE INPUT CHARACTERS
													#		IN THE IF STATEMENT ABOVE. THIS IS TO
													#		MAKE SURE THAT IF YOU EVER UPDATE THE
													#		UNICODE DATA, YOU ARE NOT CAUGHT BY
													#		SUPRISE BY CHARACTERS YOU DID NOT
													#		CAREFULY EXAMINE
													vRemovedCharacters[tKey] = True;
													vRemovedCharacters[tUpperCodePoint] = True;
													vRemovedCharacters[vUnicodeCaseFoldingData[tKey][
															tI][u"outPutCodePoints"][0]] = True;

													del tLowerToUpper[vUnicodeCaseFoldingData[tKey][
															tI][u"outPutCodePoints"][0]];
													del tUpperToLower[tUpperCodePoint];
													tIsReversible = False;
												else:
													#WARNING: THIS COMMENT WRITTEN SOME TIME LATER
													#A NEW CODE POINT THAT WE DID NOT KNOW ABOUT
													#		BEFORE CAUSED THE VIOLATION ABOVE.
													#		UNTIL WE INVESTIGATE, THE CODE POINT IS
													#		DEEMED INVALID. AND WE SHOULD 
													#		INVESTIGATE BECAUSE OTHERWISE WE
													#		AFFECT THE INTEGRITY OF THE CHECKING
													#		FUNCTION THAT SHALL BE IMPLEMENTED IN C.
													#
													#		NOTE THAT I ADDED SETTING tIsValid TO
													#		False TO KEEP BEHAVIOR AS ABOVE. 
													#		HOWEVER, AS OF THIS WRITING	THE SCRIPT 
													#		HAS NOT BEEN RE RUN TO REFELCT
													#		THE CHANGE BECAUSE, SPEAKING FROM
													#		MEMORY, THE SCRIPT DOES NOT ENCOUNTER
													#		THIS SITUATION WITH THE CURRENT UNICODE
													#		DATA.
													sys.exit(u"UNEXPECTED SITUATION (14): CODE " + 
															"ASSUMPTIONS ARE WRONG " + 
															u"#{0:x}".format(tKey));
													tIsReversible = False;
													tIsValid = False;
												#:endif:
											else:
												#WARNING: THIS COMMENT WRITTEN SOME TIME LATER
												#THE CODE POINT WE ARE PROCESSING IS NOT A COMPOSED 
												#		CODE POINT, UNLIKE THE 'UPPER' WE
												#		FOUND BEFORE. WE REMOVE THE OTHER 'UPPER' 
												#		CODE POINT, AND KEEP THE CURRENT 'LOWER'
												#		CODE POINT.
												print(u"OVERIDING ENTRY FOR " + 
														u"#{0:x}".format(tUpperCodePoint) + 
														u" WITH ENTRY FOR {" + 
														u"#{0:x}".format(tKey) + u" => " + 
														u"#{0:x}".format(vUnicodeCaseFoldingData[
														tKey][tI][u"outPutCodePoints"][0]) + u"}");
												del gProcessedUnicodeCaseFoldingData[
														tUpperCodePoint];
												del tLowerToUpper[vUnicodeCaseFoldingData[
														tKey][tI][u"outPutCodePoints"][0]];
												del tUpperToLower[tUpperCodePoint];
											#:endif:
										#:endif:
									#:endif
									
									if(tIsReversible and 
											(vUnicodeCaseFoldingData[tKey][tI][
											u"outPutCodePoints"][0] in vRemovedCharacters)):
										tIsReversible = False;
									#:endif:

									if(tIsReversible):
										tUpperToLower[tKey] = vUnicodeCaseFoldingData[
												tKey][tI][u"outPutCodePoints"][0];
										tLowerToUpper[vUnicodeCaseFoldingData[
												tKey][tI][u"outPutCodePoints"][0]] = tKey;
									#:endif:
								#:endif:
							#:endif:
						else:
							#WARNING: THIS COMMENT WRITTEN SOME TIME LATER
							#VIOLATION, WE HAVE A CODE POINT WITH A NON 'C' FOLD, WHOSE OUTPUT IS
							#		ONE CODE POINT LONG.
							sys.exit(u"UNEXPECTED SITUATION (15): CODE ASSUMPTIONS ARE WRONG");
							tIsValid = False;
						#:endif:
					else:
						if(vUnicodeCaseFoldingData[tKey][tI][u"type"] == u"F"):
							if(tUnicodeData[tKey][5][0] != ""):
								print("CHARACTER WITH COMPATIBILITY DECOMPOSITION ENCOUNTERED:  " + 
										u"#{0:x}".format(tKey) + u" " + tUnicodeData[tKey][5][0]);
							#:endif:
						else:
							#WARNING: THIS COMMENT WRITTEN SOME TIME LATER
							#VIOLATION, WE HAVE A CODE POINT WITH A 'C' FOLD, WHOSE OUTPUT IS
							#		MORE THAN ONE CODE POINT LONG.
							sys.exit(u"UNEXPECTED SITUATION (16): CODE ASSUMPTIONS ARE WRONG " + 
									unicode(tKey));
							tIsValid = False;
						#:endif:
					#:endif:
				else:
					if(tKey == 73):
						print(u"YES1 " + vUnicodeCaseFoldingData[tKey][tI][u"type"]);
						print(tUnicodeData[tKey]);
					#:endif:
					#REMEMBER: I ORIGINALLY TRIED TO FIX THINGS TO ALLOW THE LARGEST SET OF
					#		REVERSIBLE CASE CONVERSION BUT NOT RECOGNIZING CASE FOLDINGS OF 
					#		CHARACTERS NOT RECOGNIZED BY HFS+, USING "fastunicodecompare.c" AS
					#		THE INDIRECT TEST, AND WINDOWS. HOWEVER, BECAUSE THERE IS NOTHING
					#		I COULD USE TO TEST FOR APFS, I DECIDED TO PLAY IT SAFE AND TO NO LONGER 
					#		DO THIS, AND INSTEAD RECOGNIZE THESE OTHER CASE FOLDS AND LOSE SOME 
					#		REVERSIBLE CASE CONVERSION. REMEMBER, THIS IS ON THE ASSUMPTION THAT
					#		THE 'CHECKING' FUNCTION, SEE MY NOTES ELSEWHERE, IS TO BE USED ON ALL
					#		SYSTEMS, OR NOT AT ALL.
					if((tUnicodeData[tKey][5][0] == "<>") or ((tKey >= 44032) and (tKey <= 55203))):
						_gfdgfd = False; #TO ALLOW COMMENTING OUT PRINT WITHOUT ERROR

						#print(u"IGNORING CANONICALLY COMPOSED CHARACTER " + 
						#		unicode(tKey));
						tIsValid = False;
					#elif(tKey == 837):
						#REMEMBER: THIS CHARACTER IS STILL VALID. WE ARE ONLY NOT RECOGNIZING
						#			ITS CASE FOLD
						#_gfdgfd = False;
					#elif((tKey >= 7296) and (tKey <= 7303)):
						#REMEMBER: THESE CHARACTERS ARE STILL VALID. WE ARE ONLY NOT RECOGNIZING
						#			THEIR CASE FOLD
						
						#UPDATE: YOU MIGHT NEED TO USE A DIFFERENT APPROACH BECAUSE THESE CHARACTERS
						#		SIMPLY DID NOT HAVE CASE FOLDING IN UNICODE 3.2, WHICH IS WHAT IS
						#		USED BY HFS+ WHICH IS MIMICKED BY "fastunicodecompare.c", BUT 
						#		UNICODE 9.0, WHICH IS USED BY APFS, DOES HAVE THESE FOLDS.
						#_gfdgfd = False;
					#elif(tKey == 962):
						#REMEMBER: THIS CHARACTER IS STILL VALID. WE ARE ONLY NOT RECOGNIZING
						#			ITS CASE FOLD
						#_gfdgfd = False;						
					#elif((tKey == 7304) or (tKey == 42570) or (tKey == 42571)):
						#THESE CHARACTERS, 1C88(7304) AND A64A(42570) AND A64B(42571), INTRODUCE 
						#		CONTRADICTION TO CODE ASSUMPTIONS, AND WHILE WINDOWS ONLY RECOGNIZES
						#		A64A(42570) AND A64B TO BE EQUIVILANT, WHICH WOULD SOLVE THE PROBLEM,
						#		"fastunicodecompare.c" RECOGNIZES NON OF THEM TO BE EQUIVILANT,
						#		TURNING THIS TO INVALID CHARACTERS PROBLEM.
						#_gfdgfd = False;
						
						#UPDATE: BECAUSE THE 'CHECKING' PROCESS WILL NEED TO ALLOW CHECKING FOR ALL
						#		CHARACTERS, AND MUST BE USED UNDER ALL SYSTEMS IF AT ALL, CHARACTERS
						#		THAT ARE RECOGNIZED UNDER AT LEAST ONE SYSTEM TO BE EQUAL CASE
						#		INSENSITIVELY, MUST CONTINUE TO BE SEEN AS HAVING CASE FOLDING.
					#elif(tKey == 7304):
						#SEE NOTE ABOVE.
						#_gfdgfd = False;
					else:					
						tCodePointsOfOtherFoldings[tKey] = True;
					#:endif:
					
					tIsValid = False;
				#:endif:
				
				if(tIsValid):
					if(tKey in gProcessedUnicodeCaseFoldingData):
						#WARNING: THIS COMMENT WRITTEN SOME TIME LATER
						#VIOLATION, CURRENT CODE POINT WAS PROCESSED BEFORE.
						sys.exit(u"UNEXPECTED SITUATION (17): CODE ASSUMPTIONS ARE WRONG. SEE " +
								u"#{0:x}".format(tKey));
						tIsValid = False;
					elif(0 in vUnicodeCaseFoldingData[tKey][tI][u"outPutCodePoints"]):
						#WARNING: THIS COMMENT WRITTEN SOME TIME LATER
						#VIOLATION, THE NULL CODE POINT IS IN THE OUTPUT.
						sys.exit(u"UNEXPECTED SITUATION (18): CODE ASSUMPTIONS ARE WRONG. SEE " +
								u"#{0:x}".format(tKey));
						tIsValid = False;
					else:
						#BASED ON FastUnicodeCompare() REFERENCE CODE FOR MAC'S HFS, ONLY THE FOLLOWING
						#		SILENT CHARACTERS ARE RECOGNIZED. 'SILENT' IS AS DEFINED IN MY WORK.
						#				0x200C-0x200F		(8204-8207)
						#				0x202A-0x202E		(8234-8238)
						#				0x206A-0x206F		(8298-8303)
						#				0xFEFF				(65279)
						#		IN THE FUTURE, ALL CHARACTERS MARKED AS "Default_Ignorable_Code_Points" IN THE 
						#		UNICODE FILE, "DerivedCoreProperties.txt", MIGHT BE TREATED AS SILENT.
						for tI2 in six.moves.range(0, 
								len(vUnicodeCaseFoldingData[tKey][tI][u"outPutCodePoints"]), 1):
							if(vUnicodeCaseFoldingData[tKey][tI][u"outPutCodePoints"] in 
									[8204, 8205, 8206, 8207, 8234, 8235, 8236, 8237, 8238,
									8298, 8299, 8300, 8301, 8302, 8303, 65279]):
								#WARNING: THIS COMMENT WRITTEN SOME TIME LATER
								#VIOLATION, SILENT CODE POINT FOUND IN THE OUTPUT.
								sys.exit(u"UNEXPECTED SITUATION (19): CODE ASSUMPTIONS ARE WRONG. SEE " +
										u"#{0:x}".format(tKey));
							#:endif:
						#:endfor:

						gProcessedUnicodeCaseFoldingData[tKey] = {u"inputCodePoint": tKey, 
								u"outPutCodePoints": vUnicodeCaseFoldingData[tKey][tI][
										u"outPutCodePoints"], 
								u"type": vUnicodeCaseFoldingData[tKey][tI][u"type"], 
								u"isReversible": False,
								u"isInputCodePointAnUpper": vUnicodeCaseFoldingData[tKey][tI][
										u"isInputCodePointAnUpper"]};
					#:endif:
				#:endif:
			#:endfor:
		#:endfor:
		
		#START: REMOVE CHARACTERS WHERE OUTPUT CHARACTER HAS AN ODD NUMBER OF OCCURANCE
		#if(tLowerToUpper[1090] in {1058, 7300, 7301}):
		#	del tUpperToLower[tLowerToUpper[1090]];
		#	del tLowerToUpper[1090];
		#:endif:
		#if(IS_ENABLING_COMPOSITES and (tLowerToUpper[953] in {837, 921, 8126})):
		#	del tUpperToLower[tLowerToUpper[953]];
		#	del tLowerToUpper[953];
		#:endif:
		#:END
		
		
		for tKey in tCodePointsOfOtherFoldings.keys():
			if(not(tKey in gProcessedUnicodeCaseFoldingData)):
				sys.exit(u"UNEXPECTED SITUATION (20): CODE ASSUMPTIONS ARE WRONG. SEE " + 
						u"#{0:x}".format(tKey));
			#:endif:
		#:endfor:
		
		for tKey in gProcessedUnicodeCaseFoldingData.keys():
			for tI in six.moves.range(0, len(vUnicodeCaseFoldingData[tKey]), 1):
				if(vUnicodeCaseFoldingData[tKey][tI][u"type"] == u"F"):
					#print(unicode(tKey));
					for tI2 in six.moves.range(0, 
							len(vUnicodeCaseFoldingData[tKey][tI][u"outPutCodePoints"]), 1):
						#print(u"=>" + unicode(vUnicodeCaseFoldingData[tKey][tI][u"outPutCodePoints"][tI2]));
						if((vUnicodeCaseFoldingData[tKey][tI][u"outPutCodePoints"][tI2] in 
								gProcessedUnicodeCaseFoldingData)):
							print(u"OUTPUT CHARACTER FOR A COMPATIBILITY DECOMPOSITION, AND IS " + 
									u"ALSO AN INPUT, ENCOUNTERED:  " + u"#{0:x}".format(
									vUnicodeCaseFoldingData[tKey][tI][u"outPutCodePoints"][tI2]));
						elif((vUnicodeCaseFoldingData[tKey][tI][u"outPutCodePoints"][tI2] in 
											tUpperToLower) or
									(vUnicodeCaseFoldingData[tKey][tI][u"outPutCodePoints"][tI2] in 
											tLowerToUpper)):
								print(u"OUTPUT CHARACTER FOR A COMPATIBILITY DECOMPOSITION, AND " + 
										u"IS ALSO PART OF A REVERSIBLE CASE CONVERSION, " +
										u"ENCOUNTERED:  " + u"#{0:x}".format(
										vUnicodeCaseFoldingData[tKey][tI][u"outPutCodePoints"][
										tI2]) + "  SEE " + u"#{0:x}".format(tKey));
							#:endif:
						#:endif:
					#:endif:
				#:endif:
			#:endfor:
		#:endfor:
		
		for tKey in tUpperToLower.keys():
			if(not(tUpperToLower[tKey]) in tLowerToUpper):
				sys.exit(u"CODE ERROR. REVERSIBLE CONVERSIONS ARE NOT BALANCED.");
			#:endif:
		#:endfor:
		
		for tKey in vRemovedCharacters.keys():
			if((tKey in tLowerToUpper) or (tKey in tUpperToLower)):
				tText = u"UpperToLower";

				if(tKey in tLowerToUpper):
					tText = u"tLowerToUpper";
				#:endif:

				#WARNING: THIS COMMENT WRITTEN SOME TIME LATER
				#VIOLATION, WE DID NOT CORRECTLY REMOVE CHARACTERS THAT CAUSED VIOLATIONS BEFORE
				sys.exit(u"UNEXPECTED SITUATION (21): CODE ASSUMPTIONS ARE WRONG " + 
						u"#{0:x}".format(tKey) + u" FOUND IN " + tText);
			#:endif:
		#:endfor:

		for tKey in tLowerToUpper.keys():
			if(not(tLowerToUpper[tKey]) in tUpperToLower):
				sys.exit(u"CODE ERROR. REVERSIBLE CONVERSIONS ARE NOT BALANCED.");
			else:			
				if(tKey in gProcessedUnicodeCaseFoldingData):
					gProcessedUnicodeCaseFoldingData[tKey][u"isReversible"] = True;
				elif(tLowerToUpper[tKey] in gProcessedUnicodeCaseFoldingData):
					gProcessedUnicodeCaseFoldingData[tLowerToUpper[tKey]][u"isReversible"] = True;
				#:endif:
			#:endif:
		#:endfor:
		
		
	#:endif:

	return gProcessedUnicodeCaseFoldingData;
#:enddef:

def doMain3(pFullFilePath):
	MAX_NUMBER_OF_OUTPUT_CODE_POINTS = 3;
	vProcessedUnicodeCaseFoldingData = verifyAndGetProcessedUnicodeCodeFoldingData();
	vString = (
	u"typedef struct CaseFoldEntry\r\n" + 
	u"{\r\n" +
	u"	uint32_t gInput;\r\n" +
	u"	uint32_t gOutput[" + unicode(MAX_NUMBER_OF_OUTPUT_CODE_POINTS) + u"];\r\n" +
	u"	bool gIsInputAnUpperCase;\r\n" +
	u"	bool gIsReversible;\r\n" +
	u"} CodeFoldEntry\r\n" + 
	u"\r\n" +
	u"\r\n" +
	u"CaseFoldEntry const * getCaseFoldEntries()\r\n" +
	u"{\r\n" + 
	u"	static const CODE_FOLD_ENTRIES[" + unicode(len(vProcessedUnicodeCaseFoldingData)) + u"] = \r\n" +
	u"	{\r\n" + 
	u"		");
	vIsFirst = True;
	vCount = 0;
	tString = u"";
	
	for tKey in vProcessedUnicodeCaseFoldingData:
		tIsFirst = True;

		if(len(vProcessedUnicodeCaseFoldingData[tKey]['outPutCodePoints']) > 
				(MAX_NUMBER_OF_OUTPUT_CODE_POINTS)):
			sys.exit(u"LENGTH OF OUTPUT CODE POINTS IS LONGER THAN EXPECTED.");
		#:endif:

		if(not(vIsFirst)):
			if(vCount > 0):
				tString += u", ";
			else:
				tString += (u",\r\n" +
	u"		");
			#:endif:
		else:
			vIsFirst = False;
		#:endif:

		tString += (u"{" + u"{:>6}".format(
				vProcessedUnicodeCaseFoldingData[tKey]['inputCodePoint']) + ", {");

		for tI in six.moves.range(0, 
				len(vProcessedUnicodeCaseFoldingData[tKey]['outPutCodePoints']), 1):
			if(not(tIsFirst)):
				tString += u", ";
			else:
				tIsFirst = False;
			#:endif:

			tString += u"{:>6}".format(
				vProcessedUnicodeCaseFoldingData[tKey]['outPutCodePoints'][tI]);
		#:endfor:
		
		for tI in six.moves.range(len(vProcessedUnicodeCaseFoldingData[tKey]['outPutCodePoints']),
				MAX_NUMBER_OF_OUTPUT_CODE_POINTS, 1):
			if(not(tIsFirst)):
				tString += u", ";
			else:
				tIsFirst = False;
			#:endif:
			
			tString += u"{:>6}".format(0);
		#:endfor:
		
		#tString += u", 0}, ";
		tString += u"}, ";

		if(vProcessedUnicodeCaseFoldingData[tKey]['isInputCodePointAnUpper']):
			tString += u" true";
		else:
			tString += u"false";
		#:endif:
		
		if(vProcessedUnicodeCaseFoldingData[tKey]['isReversible']):
			tString += u",  true";
		else:
			tString += u", false";
		#:endif:

		tString += u"}";
		
		vCount += 1;

		if(vCount == 1):
			vString += tString;
			tString = u"";
			vCount = 0;
		#:endif:
	#:endfor:

	if(tString != ""):
		vString += (
			tString + "\r\n");
		tString = u"";
	else:
		vString += u"\r\n";
	#:endif:
	
	vString += (
	u"	}\r\n" + 
	u"\r\n" + 
	u"	return &(CODE_FOLD_ENTRIES[0]);\r\n" +
	u"}\r\n");
	
	if(not(os.path.isfile(pFullFilePath))):
		tFile = codecs.open(pFullFilePath, "w", "utf-8");
		tFile.write(vString);
		tFile.close();
	else:
		print(u"ERROR: File \"" + pFullFilePath + "\" already exists.\n");
	#:endif:
#:enddef:

def generateCTableLookUpFunction(pFunctionName, pData, pValueOfUndefined):
	THRESHOLD = 120;
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
	doMain3(u"C:\\Programs\\Python\\Projects\\Tests\\e.h");
	#x = verifyAndGetProcessedUnicodeCodeFoldingData();
	#print(x);
	#tStart = -1;
	#tPrevious = -1;
	
	# for tKey in x:
		# if(tStart == -1):
			# tStart = tKey;
		# else:
			# if(tKey != (tPrevious + 1)):
				# if(tPrevious > tStart):
					# print(u"range " + unicode(tStart) + u" -> " + unicode(tPrevious) + u" (" + 
							# unicode(tPrevious - tStart + 1) + u")");
				# else:
					# print(u"range " + unicode(tStart));
				# #:endif:

				# tStart = tKey;
			# #:endif:
		# #:endif:
		# tPrevious = tKey;		
	# #:endfor:
	
	#tFile = codecs.open(u"C:\\Programs\\Python\\Projects\\Tests\\proceedCodeFolds.txt", "w", "utf-8");
	#tFile.write(json.dumps(x, indent = 4));
	#tFile.close();
	
	

	six.moves.input("Press Enter To Continue..")
#:endif: