var gThread = null;
var gThread2 = null;
var gStartTime = -1;

function getPromiseCallback(pPromise)
{
	return function(pFunction)
	{
		return pPromise.then(pFunction);
	}
}
function doGetTime()
{
	if(!Date.now)
		{return new Date();}
	return Date.now();
}
function doLog(pText)
{
	/*if(window.console && window.console.log)
		{window.console.log(pText);}*/
	if(doLog.e == null)
		{doLog.e = document.getElementById('MSG');}

	doLog.e.innerHTML += "<br/>" + pText;
}
doLog.e = null;
function doLog2(pText)
{
	if(doLog2.e == null)
		{doLog2.e = document.getElementById('MSG');}

	doLog2.e.innerHTML += "<br/>" + pText;
}
doLog2.e = null;

function doTestLog(pMessage, pTest, pPrepare)
{
	if(pPrepare !== undefined)
	{
		pPrepare();
		if(!pTest()){alert('Test failed [' + pMessage + ']');}
		else{doTestLog__log('Test works [' + pMessage + ']');}
	}
	else
	{
		if(!pTest){alert('Test failed [' + pMessage + ']');}
		else{doTestLog__log('Test works [' + pMessage + ']');}
	}
}
function doTestLog__log(pMessage)
{
	if(window['console'] && window.console['log'])
		{window.console.log(pMessage);}
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
gThread = new crx_crxThread.Thread();

function threadedFunction(T, pFunction_callBack)
{
	console.log('hh ' + this.vS);
	//T.THROW("Exception thrown in threaded function(2)");
	T.BLK(function()
	{
		console.log('doing something');
		pFunction_callBack("f");
	});
	//throw("Exception thrown in threaded function(1)");
	//throw(new Error("Exception thrown in threaded function(1)"));
}
function threadedFunction2(T, pFunction_callBack)
{
	T.BLK(function()
	{
		console.log('threadedFunction2: I SHOULD NOT APPPEAR');
		//pFunction_callBack("f");
	});
	//pFunction_callBack("threadedFunction2 done");
	T.BLK(function()
	{
		console.log('threadedFunction2: I SHOULD NOT APPPEAR');
		//pFunction_callBack("f");
	});
	T.CALL(threadedFunction, null);
	T.BLK(function()
	{
	pFunction_callBack("threadedFunction2 done");
	});
	//throw "threadedFunction2: I throw early";
}

gThread.DEFINE(function(T, pName, pTop, pSleepTime)
{
	this.VAR('pName', pName);
	this.VAR('pTop', pTop);
	//this.VAR('pSleepTime', pSleepTime);
	this.VAR('vStartTime', doGetTime());

	T.TRY(function()
	{
		T.APPLY([[threadedFunction, null]]);
	});
	T.CATCH(function(pE)
	{
		console.log("CAUGHT " + pE);
	});
	
	T.TRY(function()
	{
		T.APPLY(threadedFunction2);
		T.BLK(function(){
		console.log(this._RETURN);
		});
	});
	T.CATCH(function(pE)
	{
		console.log("CAUGHT " + pE);
	});
	
	
	//T.TRY(function()
	/*{*/
		//T.FOR(function(){this.VAR('tI', 0)}, function(){return this.tI < this.pTop;}, function(){this.tI++;}, function()
		this.VAR('tI', pTop - 1);
		T.WHILE(function(){return(this.tI > 0);}, function()
		{
			//T.SLEEP(this.pSleepTime);
			T.IF(function(){return (this.tI === (this.pTop - 1));}, function()
			{
				doLog(this.pName + ' LAST' + ":" + this.tI);
			});
			//T.ELSEIFe(function(){return (this.tI === (this.pTop - 2));}, function()
			/*{*/
//				doLog(this.pName + ' BEFORE LAST' + ":" + this.tI);
				//T.THROW('some exception');
				//throw 'basic javascript exception';
			/*});*/
			T.ELSE(function()
			{
				doLog(this.pName + ":" + this.tI);
			});

			T.BLK(function(){this.tI--;});
		});
	/*});*/
	//T.CATCH(function(tE)
	/*{*/
//		doLog('CAUGHT: ' + tE);
	/*});*/

	

	/*for(var tI = 0; tI < this.pTop; tI++)
	{
		//T.SLEEP(this.pSleepTime);
		T.BLK(function(pI){
		if(pI === (this.pTop - 1))
			{doLog('THREAD LAST:' + pI);}
		else
			{doLog("THREADS:" + pI);}
		}, tI);
	}*/
	
	/*var vThis = this;
	for(var tI = 0; tI < this.pTop - 1; tI++)
	{
		T.SLEEP(this.pSleepTime);
		T.BLK(function(pI){
		return function()
		{
			if(pI === (vThis.pTop - 1))
				{doLog('THREAD LAST:' + pI);}
			else
				{doLog("THREADS:" + pI);}
		};
		}(tI));
	}*/

	T.BLK(function(){
	doLog(this.pName + ' TOTAL TIME: ' + (doGetTime() - this.vStartTime));
	});
});


//gThread.RUN('T1', 100, 0);
//gThread.RUN('T1', 12, 0);
//gThread.RUN('T2', 12, 0);
//gThread.RUN('T3', 13, 0);
//gThread.RUN('T4', 13, 0);
//gThread.RUN('T5', 20, 100);
//gThread.RUN('T6', 10, 100);
//gThread.RUN('T7', 10, 100);
//gThread.RUN('T8', 10, 100);
//gThread.RUN('T9', 10, 100);
//gThread.RUN('T10', 10, 100);

// */

/*for(var tI = 0; tI < 100; tI++)
	{gThread.RUN('T' + tI, 1, 1);} // */

//*/

//-----------------------

gThread2 = new crx_crxThread.Thread();

gThread2.DEFINE(function(T)
{
	//T.TRY(function()
	//{
		T.BLK(function()
		{
		console.log('1');
		//throw new Error("ssss");
		T.APPLY([[threadedFunction2, null]]);
		});
		T.BLK(function()
		{
		console.log('2');
		});
	//});
	//T.CATCH(function(e)
	//{
	//	console.log(e);
	//});
});

//gThread2.RUN()

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

if(true) //PERFORMANCE TESTS
{
	/* IN FIREFOX BASED BROWSERS, ASIDE FROM THE NON THREAD IMPLEMENTATION, 
			CRX THREADS ARE THE FASTEST, THEN RAW PROMISES, THEN CRX PROMISES.
			IN VIVALDI, HOWEVER, A CHROME BASED BROWSER, PROMISES ARE THE FASTEST
			THEN CRX PROMISES, THEN THREADS.
		FIREFOX:
			THREAD TOTAL TIME: 159
			THREADLESS TOTAL TIME: 1
			PROMISE TOTAL TIME: 295
			CRX PROMISE TOTAL TIME: 1849
		PALEMOON:
			THREAD TOTAL TIME: 190
			THREADLESS TOTAL TIME: 2
			PROMISE TOTAL TIME: 330
			CRX PROMISE TOTAL TIME: 2096
		VIVALDI
			THREAD TOTAL TIME: 129
			THREADLESS TOTAL TIME: 1
			PROMISE TOTAL TIME: 27
			CRX PROMISE TOTAL TIME: 79
		IE11
			THREAD TOTAL TIME: 361
			THREADLESS TOTAL TIME: 1
		OPERA
			THREAD TOTAL TIME: 387
			THREADLESS TOTAL TIME: 0
			
		CRXEU WAS USING "RUN_MODE__ONE_PER_THREAD".
	*/
	var tIS_TO_PRINT_PROGRESS = false;
	var tNUMBER_OF_ITERATIONS = 10000;
	var tThreadTotalTime = -1;
	var tThreadLessTotalTime = -1;
	var tPromiseTotalTime = -1;
	var tCrxPromiseTotalTime = -1;
	
	gThread = new crx_crxThread.Thread();

	gThread.DEFINE(function(T, gFunc_onDone)
	{
		this.VAR('vStartTime', doGetTime());

		T.FOR(function(){this.VAR('tI', 0)}, function(){return this.tI < tNUMBER_OF_ITERATIONS;}, 
				function(){this.tI++;}, function()
		{
			T.IF(function(){return (this.tI === (tNUMBER_OF_ITERATIONS - 1));}, function()
			{
				doLog("LAST THREAD: " + this.tI);
			});
			T.ELSEIF(function(){return (this.tI === 0);}, function()
			{
				doLog("FIRST THREAD: " + this.tI);
			});
			T.ELSE(function()
			{
				if(tIS_TO_PRINT_PROGRESS)
					{doLog("THREAD: " + this.tI);}
			});
		});

		T.BLK(function(){
		tThreadTotalTime = (doGetTime() - this.vStartTime);
		setTimeout(gFunc_onDone);
		});
	});
	
	gThread.RUN(function()
	{
		gStartTime = doGetTime();
		for(var tI = 0; tI < tNUMBER_OF_ITERATIONS; tI++)
		{
			if(tI === (tNUMBER_OF_ITERATIONS - 1))
				{doLog('LAST THREADLESS: ' + tI);}
			else if(tI === 0)
				{doLog('FIRST THREADLESS: ' + tI);}
			else
			{
				if(tIS_TO_PRINT_PROGRESS)
					{doLog('THREADLESS: ' + tI);}
			}
		}
		tThreadLessTotalTime = (doGetTime() - gStartTime);


		if(window["Promise"])
		{
			setTimeout(function()
			{
				//var tPromise = null;
				var tPromise = Promise.resolve(true);
				var tIndices = [];
				var tI = 0;
			
			//RAW PROMISES-----------------------------

				for(var tI = 0; tI < tNUMBER_OF_ITERATIONS; tI++)
					{tIndices[tI] = tI;}

				gStartTime = doGetTime();
				
				//THE FOLLOWING IS NOT EXACTLY EQUIVILANT BECAUSE IT RUNS THE LOOPS OF THE
				//		VIRTUAL WHILE LOOP IN "PARALLEL".
				/*tPromise = Promise.all(tIndices.map(function(pI)
				{
					return new Promise((function(pI2)
					{
						return function(pFunc_onDone, pFunc_onError)
						{
							var tPromise = new Promise(function(pFunc_onDone, pFunc_onError)
							{
								if(pI2 === (tNUMBER_OF_ITERATIONS - 1))
									{doLog('LAST PROMISE: ' + pI2);}
								
								pFunc_onDone();
							});
							tPromise = tPromise.then(function(pValue)
							{
								if(pI2 === 0)
									{doLog('FIRST PROMISE: ' + pI2);}
							});
							tPromise = tPromise.then(function(pValue)
							{
								if((pI2 !== 0) && (pI2 !== (tNUMBER_OF_ITERATIONS - 1)))
								{
									if(tIS_TO_PRINT_PROGRESS)
										{doLog('PROMISE: ' + pI2);}
								}

								pFunc_onDone();
							});
						};
					})(pI));
				}));*/
				
				for(var tI = 0; tI < tNUMBER_OF_ITERATIONS; tI++)
				{
					tPromise = tPromise.then((function(pI)
					{
						return function()
						{
							var tPromise = Promise.resolve(true).then(function()
							{
								if(pI === (tNUMBER_OF_ITERATIONS - 1))
									{doLog('LAST PROMISE: ' + pI); return true;}
								else
									{return false;}
							});
							
							tPromise = tPromise.then(function(pIsIfChainBlockResolved)
							{
								if(pIsIfChainBlockResolved)
									{return true;}

								if(pI === 0)
									{doLog('FIRST PROMISE: ' + pI); return true;}
								else
									{return false;}
							});
							tPromise = tPromise.then(function(pIsIfChainBlockResolved)
							{
								//if((pI !== 0) && (pI !== (tNUMBER_OF_ITERATIONS - 1)))
								if(!pIsIfChainBlockResolved)
								{
									if(tIS_TO_PRINT_PROGRESS)
										{doLog('PROMISE: ' + pI);}
								}
							});
							
							return tPromise;
						}
					})(tI));
				}

				tPromise.then(function(pValue)
				{
					tPromiseTotalTime = (doGetTime() - gStartTime);
				});
				

			//CRX PROMISE------------------------------		
				tPromise = CRXPLL__FOR(tPromise, function(){tI = 0;}, 
						function(){return (tI < tNUMBER_OF_ITERATIONS);}, function(){tI++},
						function()
				{
					var vPromise = Promise.resolve(true);

					vPromise = CRXPLL__IF(vPromise, 
							function(){return(tI === (tNUMBER_OF_ITERATIONS - 1));},
							function()
					{
						doLog('LAST CRX PROMISE: ' + tI);
					});
					vPromise = CRXPLL__ELSEIF(vPromise, 
							function(){return(tI === 0);},
							function()
					{
						doLog('FIRST CRX PROMISE: ' + tI);
					});
					vPromise = CRXPLL__ELSE(vPromise, function()
					{
						if(tIS_TO_PRINT_PROGRESS)
							{doLog('CRX PROMISE: ' + tI);}
					});
					
					return vPromise;
				});
				tPromise = CRXPLL__BLK(tPromise, function()
				{
					tCrxPromiseTotalTime = (doGetTime() - gStartTime);
				});
				tPromise = CRXPLL__BLK(tPromise, function()
				{
					doLog("THREAD TOTAL TIME: " + tThreadTotalTime);
					doLog('THREADLESS TOTAL TIME: ' + tThreadLessTotalTime);
					doLog('PROMISE TOTAL TIME: ' + tPromiseTotalTime);
					doLog('CRX PROMISE TOTAL TIME: ' + tCrxPromiseTotalTime);
				});
			});
		}
		else
		{
			doLog("THREAD TOTAL TIME: " + tThreadTotalTime);
			doLog('THREADLESS TOTAL TIME: ' + tThreadLessTotalTime);
			doLog("COULD NOT DO PROMISE TEST. PROMISE API NOT FOUND");
		}
	});
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

if(false) //CONCURENCY TESTS
{
	//RUN_MODE__TIME_PER_THREAD MODE HAS SHOWN TO BE FASTER BY TAKING ROUGHLY 4% LESS TIME
	//N_MODE__FIXED WITH VERY LARGE N GIVES BEST PERFORMANCE, BUT OF COURSE SHOULD NOT BE RELIED
	//		UPON.
	//HENCE RUN_MODE__TIME_PER_THREAD WITH N_MODE__FIXED AND LARGE N HAS GIVEN BEST TIMES.
	//		WHEN I DID THE TEST, THEY GAVE ROUGHLY 8100 AS OPPOSED TO 9000.

	var tIS_TO_PRINT_CHECKING_END_LOG = true;
	var tNUMBER_OF_THREADS = 3;
	var gCount = 0;
	var gEndTime = -1;

	gThread = new crx_crxThread.Thread();
	gThread2 = new crx_crxThread.Thread();
	gStartTime = doGetTime();
	
	gThread.DEFINE(function(T, pName)
	{
		this.VAR("gCount", 0);

		T.FOR(function(){this.VAR('tI', 0)}, function(){return this.tI < 100000;}, 
				function(){this.tI++;}, function()
		{
			this.gCount = this.gCount + 1;

			T.IF(function(){return (this.gCount === 50000);}, function()
			{
				this.gCount = 0;
				doLog("Thread " + pName + " did: " + (this.tI + 1));
			});
		});
		
		T.BLK(function()
		{
			gEndTime = doGetTime();
			gCount = gCount + 1;
		});
	});
	
	gThread2.DEFINE(function(T)
	{
		T.WHILE(function(){return true;}, function()
		{
			T.SLEEP(1000);
			T.IF(function(){return(gCount === tNUMBER_OF_THREADS);}, function()
			{
				doLog("TOTAL TIME: " + (gEndTime - gStartTime));
				T.BREAK();
			});
			T.ELSE(function()
			{
				if(tIS_TO_PRINT_CHECKING_END_LOG)
					{doLog("CHECKING END");}
			});
		});
	});
	
	for(var tI = 1; tI <= tNUMBER_OF_THREADS; tI++)
	{
		gThread.RUN("T" + tI);
	}
	gThread2.RUN();
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

gThread = new crx_crxThread.Thread();

function threadedFunction01(T, pFunction_callBack, pX)
{
	pFunction_callBack(pX - 1);
}
function threadedFunction02(T, pFunction_callBack, pX)
{
	T.BLK(function(){
	pFunction_callBack(pX - 1);
	});
}
function threadedFunction03(T, pFunction_callBack, pX)
{
	T.IF(function(){return (pX > 0);}, function()
	{
		T.CALL(threadedFunction03, pX - 1);
		T.BLK(function(){
		pFunction_callBack(pX * this._RETURN[0]);
		});
	});
	T.ELSEIF(function(){return (pX === 0);}, function()
	{
		pFunction_callBack(1);
	});
	T.ELSEIF(function(){return pX === -4;}, function()
	{
		throw 'threadedFunction03: ' + pX;
	});
	T.ELSE(function()
	{
		T.TRY(function()
		{
			T.CALL(threadedFunction03, pX - 1);
			T.BLK(function(){
			pFunction_callBack(pX + this._RETURN[0]);
			});
		});
		T.CATCH(function(tE)
		{
			throw tE + "||" + pX;
		});
	});
}
function threadedFunction04_a(T, pFunction_callBack, pX)
{
	T.CALL(threadedFunction04_b);
}
function threadedFunction04_b(T, pFunction_callBack, pX)
{
	throw Error('HJHG');
}
function threadedFunction05_a(T, pFunction_callBack)
{
	T.CALL(threadedFunction05_b);
	T.BLK(function()
	{
		pFunction_callBack();
	});
}
function threadedFunction05_b(T, pFunction_callBack)
{
	T.CALL(threadedFunction05_c);
	T.BLK(function(){
	pFunction_callBack();
	});
}
function threadedFunction05_c() //REMEMBER, THIS IS NOT A THREADED FUNCTION
{
	threadedFunction05_d();
}
function threadedFunction05_d() //REMEMBER, THIS IS NOT A THREADED FUNCTION
{
	threadedFunction05_e();
}
function threadedFunction05_e() //REMEMBER, THIS IS NOT A THREADED FUNCTION
{
	throw new Error('eee');
}

if(false) //THREAD TESTS
{
	gThread.DEFINE(function(T)
	{
		this.VAR('vX', 5);
		this.VAR('vY', 2);
		this.VAR('vZ', 0);
		this.VAR('vS', "");

		T.IF(function(pA){return this.vY === pA}, function(pA)
		{
			doTestLog('IF01', true);
			doTestLog('IF02', this.vX === 5);
			
			this.VAR('vZ', 10);
		}, 2);
		T.ELSE(function()
		{
			doTestLog('IF01', false);
		});
		
		T.IF(function(pA){return false}, function(pA)
		{
		}, 2);
		T.ELSE(function()
		{
			this.vX++;
		});

		T.BLK(function(){
		doTestLog('IF03', this.n === undefined);
		doTestLog('IF04', this.vX === 6);
		
		this.vX = 0;
		this.vY = 0;
		this.vZ = 0;
		this.vS = "";
		T.FOR(function(a){this.VAR('tI', 0);this.VAR('tA', a);this.vS+='a1';},
				function(a){this.VAR('tB', a + 1); this.vS+='b1';return (this.tI < 5);}, 
				function(a){this.VAR('tC', a + 2); this.vS+='c1';this.tI++;}, function(a)
		{
			this.vS+='d1';
			if(this.tI === 2)
			{
				//alert(this.tA);
				this.vY = this.tA + this.tB + this.tC;
			}
			T.FOR(function(){this.VAR('tI2', 0);this.vS+='a2';}, function(){this.vS+='b2'; return (this.tI2 < 10);},
					function(){this.vS+='c2'; this.tI2++;}, function()
			{
				this.vS+='d2';
				if(this.tI2 === 2)
					{T.BREAK();}

				T.BLK(function(){
				T.IF(function(){return (this.tI === 3);}, function()
				{
					T.BREAK('loop1');
				});
				});
				T.BLK(function(){
				this.vS+='e2';
				this.vX++;
				});
				
			}, 'loop2');

			T.BLK(function(){
			this.vS+='e1';
			this.vZ++;
			});
		}, 'loop1', 8);

		T.BLK(function(){
		doTestLog('FOR01', this.vX === 6);
		doTestLog('FOR02', this.vY === 27);
		doTestLog('FOR03', this.vZ === 3);
		doTestLog('FOR04', this.vS === "a1b1d1a2b2d2e2c2b2d2e2c2b2d2e1c1b1d1a2b2d2e2c2b2d2e2c2b2d2e1c1b1d1a2b2d2e2c2b2d2e2c2b2d2e1c1b1d1a2b2d2");
		});
		
		T.CALL(threadedFunction01, 5);
		T.BLK(function(){
		doTestLog('FUNC01', this._RETURN[0] === 4);
		});

		T.CALL(threadedFunction02, 8);
		T.BLK(function(){
		doTestLog('FUNC02', this._RETURN[0] === 7);
		});

		T.CALL(threadedFunction03, 4);
		T.BLK(function(){
		doTestLog('FUNC03', this._RETURN[0] === 24);
		});
		});
		
		T.TRY(function()
		{
			T.CALL(threadedFunction03, -1);
		});
		T.CATCH(function(tE)
		{
			doTestLog('FUNC04', tE === "threadedFunction03: -4||-3||-2||-1");
		});
		
		T.IF(function(){return true;}, function()
		{
			doTestLog('IF05', true);
		});
		T.ELSEIF(function(){return true;}, function()
		{
			doTestLog('IF06: You should NOT see me', false);
		});
	});
	gThread.RUN();
}

if(false) //STACK TRACE TESTS
{
	gThread = new crx_crxThread.Thread();

	gThread.DEFINE(function(T)
	{
		T.IF(function(){return true;}, function()
		{
			//T.CALL(threadedFunction04_a);
			T.CALL(threadedFunction05_a);
		});
	});
	gThread.RUN();		//YOU SHOULD SEE THE STACK TRACE IN THE CONSOLE.
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


if(false) /*TIMER TESTS: SCHEDULED*/
{
	var tThread = new crx_crxThread.Thread();
	var tThread2 = new crx_crxThread.Thread();
	var gIsFinished = false;

	tThread.DEFINE(function(T, pInterval, pNumberOfLoops)
	{
		this.VAR('time', doGetTime());

		T.FOR(function(){this.VAR('tI', 0);}, function(){return this.tI < pNumberOfLoops;}, function(){this.tI++}, function()
		{
			T.SLEEP(pInterval);
		});
		T.BLK(function(){
		gIsFinished = true;
		console.log(doGetTime() - this.time);
		});
	});

	tThread2.DEFINE(function(T)
	{
		T.WHILE(function(){return !gIsFinished}, function()
			{});
	});

	tThread.RUN(1, 1000);
	//tThread.RUN(20, 50);
	//tThread2.RUN(); /*WITHOUT THIS tThread WILL BE MUCH LESS ACCURATE FOR SMALLER INTERVALS AS EXPECTED.*/
}
else if(false) /*TIMER TEST: PERIODIC*/
{
	var tPERIOD = 1;
	var tNUMBER_OF_RUNS = 1000;
	var tThread2 = new crx_crxThread.Thread();
	var tIsFinished = false;
	var tCount = 0;
	var tTime = doGetTime();
	

	crx_crxEu.schedulePeriodicCall(tPERIOD, function()
	{
		tCount++;
		
		if(tCount === tNUMBER_OF_RUNS)
		{
			tIsFinished = true;
			console.log(doGetTime() - tTime);

			return;
		}
	}, true);

	tThread2.DEFINE(function(T)
	{
		T.WHILE(function(){return !tIsFinished}, function()
			{});
	});

	tThread2.RUN(); /*WITHOUT THIS schedulePeriodicCall WILL BE MUCH LESS ACCURATE FOR SMALLER INTERVALS (PERIODS) AS EXPECTED.*/
}