**FREE

///
// ILEvator Example : send SMS with link mobility
//
///

ctl-opt dftactgrp(*no) actgrp(*new) debug(*yes) ;
ctl-opt bndDir('ILEVATOR':'JSONXML');
ctl-opt copyright('Sitemule.com  (C), 2022-2023');

Dcl-F Opr00 Usage(*Input) Keyed;

/define RPG_HAS_OVERLOAD
/copy ilevator/QRPGLEREF,ilevator
/copy ilevator/QRPGLEREF,basicauth
/copy noxdb/QRPGLEREF,JSONParser

exec sql set option commit=*NONE;
// test: 
// call IVEX19 ( (122 (*DEC 9 0)) '  ' '1' '0' )

dcl-pi *N;
	Optk           packed (9);
	SYPFX          like(oppfx);
	First          Ind;
	Fail           Ind;
end-pi;

Dcl-S auth         Varchar(128);
Dcl-S xmlPtr       Pointer; //Pointer to the XML tree 
Dcl-S xmlString    Varchar(32767); //Out: Response Body                          
Dcl-S val          Varchar(1024); //temp. value
Dcl-S stscode      Varchar(1024); //temp. value
Dcl-S desc         Varchar(1024); //temp. value
Dcl-S node         Varchar(256); //temp. value
Dcl-S SenderID     Varchar(64);
Dcl-S reqType      Varchar(32);
Dcl-S reqXlate     Varchar(32);
Dcl-S MyServerId   Varchar(32);
Dcl-S Proxy        Ind        INZ(*OFF);
Dcl-S ProxyURL     Varchar(128);
Dcl-S ProxyPort    Varchar(128);
Dcl-S URL          Varchar(128);
Dcl-S ok           Ind;
Dcl-S str          Int(10);
Dcl-S pos          Int(10);
Dcl-S Trc          Ind        INZ(*OFF);
Dcl-S HttpSts      Uns(5);

chain optk Opr00r;
if (not %found(opr00));
	json_joblog(
		'ERROR - Operator for token' 
		+ %char(optk) 
		+ ' not found'
	);
	*inlr = *On;
	return;
endif;

if First = *Off or Fail = *On;
	exec sql
		update 
		msg00 
		set MSSTS = 0
		where MSOPTK = :optk  and MSSTS = 1
		and MSSCTS <= current timestamp;
	First = *On;
	Fail  = *Off;
endif;

sendMessages();
*inlr = *On;
return;
	
// ------------------------------------------------------------------------------------
dcl-proc sendMessages;

	dcl-s pMessages 	pointer;
	dcl-s pResponse     pointer;
	dcl-s pRows         pointer;
	
	///dcl-s error			ind;

	// Always set the JSON delimiters:  
	json_setDelimiters('/\@[] .{}');
	json_SqlConnect();

	pRows = getMessagesRows();  

	dow json_GetLength (pRows) > 0;

		pMessages = buildMessages(pRows);
		pResponse = http_request (pMessages);
		handleResponse ( pResponse : pRows : pMessages);
		
		if %shtdn;
			leave;
		endif;

		// Ny liste
		json_delete (pResponse);
		json_delete (pMessages) ;
		json_delete (pRows) ;
		pRows = getMessagesRows();  

	enddo;
	
	// If any of this goes wrong the "error" is set return *ON.
	// We retrive the error message with "getLastError"
	///if error;
		// json_joblog(GetLastError('*MSGID'));
		// json_joblog(GetLastError('*MSGTXT'));
		// json_joblog(pRspIlob);
	///	return;
	///endif;

on-exit;
	json_delete (pRows) ;
	json_delete (pMessages) ;
	json_SqlDisconnect();

end-proc;

// ------------------------------------------------------------------------------------
// http_request
// ------------------------------------------------------------------------------------
dcl-proc http_request;

	dcl-pi http_request pointer;
		pRequest pointer;
	end-pi;


    dcl-s httpClient     pointer;
    dcl-s rspBuffer varchar(1000000:4) ;
    dcl-s reqBuffer  varchar(1000000:4) ;
	dcl-s inUrl      	varchar(256);
	dcl-s Url        	varchar(256);
	dcl-s proxyUrl   	varchar(256);
	dcl-s proxyPort  	varchar(5);

	unpackUrl ( OPGWUL : url : proxyUrl : proxyPort);

    httpClient = iv_newHttpClient();

    iv_setCertificate(httpClient : '/bluenote/default.kdb' : 'default' );

	// format like: 'http://fwdprx.workmule.dk:3128'
	if proxyUrl > '';
    	iv_setProxyTunnel (httpClient : 
			'http://' + proxyUrl + ':' + %char(proxyPort) 
		);
	endif; 

	iv_addHeader(httpClient : 
		'Content-Type' : 
		'application/json; charset=utf-8'
	);

	if OPSNID > ''; 
	    iv_setAuthProvider(httpClient : 
			iv_basicauth_new(OPSNID : OPSNPN)
		);
	endif;

    iv_setResponseDataBuffer(
        httpClient : 
        %addr(rspBuffer) : 
        %size(rspBuffer) : 
        IV_VARCHAR4 : 
        IV_CCSID_JOB
    );

    iv_setRequestDataBuffer(
        httpClient : 
        %addr(reqBuffer) : 
        %size(reqBuffer) : 
        IV_VARCHAR4 : 
        IV_CCSID_JOB
    );

	rspBuffer = json_AsJsonText16M ( pRequest );

    iv_execute (httpClient : 'POST' : url); 

    if iv_getStatus(httpClient) <> IV_HTTP_OK ; 
        iv_joblog('Invalid status: ' + %char(iv_getStatus(httpClient)));
		return *NULL;
    endif;

	return json_parseString(rspBuffer);

    on-exit;
        iv_free(httpClient);
end-proc;


// ------------------------------------------------------------------------------------
// buildHttpRequest
// ------------------------------------------------------------------------------------

//dcl-proc buildHttpRequest;
//
//	dcl-pi buildHttpRequest pointer;
//
//	end-pi;
//
//	dcl-s pHttp		pointer;
//	dcl-s pAuth  		pointer;
//	dcl-s pRequest 		pointer;
//	dcl-s pHeader		pointer;
//	dcl-s pProxy		pointer;
//	dcl-s Url			varchar(256);
//	dcl-s proxyUrl		varchar(256);
//	dcl-s proxyPort		varchar(5);
//
//
//	pRequest =  json_newObject();
//	json_moveObjectInto(pHttp : 'request' : pRequest); 
//	pAuth    =  json_newObject();
//	json_moveObjectInto(pRequest  : 'authenticate' : pAuth);
//	// json_setStr (pRequest : 'url' : 'https://wsx.sp247.net/sms/sendbatch');
//	json_setStr (pRequest : 'url' : url);
//	json_setStr (pRequest   : 'method' : 'POST');
//	json_setInt (pRequest   :'timeout' : 30);
//	pHeader    =  json_newObject();
//	json_moveObjectInto(pRequest : 'headers' : pHeader); 
//	json_setStr (pHeader   :'Content-Type' : 	
//		'application/json; charset=utf-8');
//	// json_setStr (pHeader   :'Content-Type' : 	
//		// 'application/json; charset=windows-1252');
//	json_setInt (pRequest   :'toCCSID' : 1208);
//	json_setStr (pAuth   :'userid' : OPSNID);
//	json_setStr (pAuth   :'password' : OPSNPN);
//	// json_moveObjectInto(pRequest : 'payload' : json_getStr(pReq)); 
//	
//
//	if ProxyURL > '';
//		pProxy    =  json_newObject();
//		json_setStr(pProxy: 'server' : ProxyURL);
//		json_setStr(pProxy: 'port' : ProxyPort);
//		json_moveObjectInto(pRequest  : 'proxy' : pProxy); 
//	endif;
//
//
//	// Using variables or ILOB or option parameter ( in a mix) as request / response  
//	// Note Ilobs and strings are left out by *NULL where varchars are left out by *OMIT
//	if Trc;
//		json_WriteJsonStmf (phttp : '/tmp/smsreq.json': 1208 : *OFF );
//	endif;
//
//	return pHttp;
//end-proc;

// ------------------------------------------------------------------------------------
// getMessagesRows
// ------------------------------------------------------------------------------------
dcl-proc getMessagesRows;

	dcl-pi getMessagesRows pointer;
	end-pi;
	
	dcl-s sql 			varchar(4096);

	if OPMPS <= 0;
		OPMPS = 100;
	elseif OPMPS > 1000;
		OPMPS = 1000;
	endif;

	sql   = 'Select * from msg00 -
			 where MSOPTK = '+ %char(optk) + '-
			 and MSSTS = 0';
    return json_sqlResultSet(sql :  1 :OPMPS );

end-proc;
// ------------------------------------------------------------------------------------
// buildMessages
// ------------------------------------------------------------------------------------
dcl-proc buildMessages;

	dcl-pi buildMessages pointer;
		pRows pointer value;
	end-pi;

	dcl-DS list         likeds(json_iterator);
	dcl-s Xmsmstk 		int(10);
	dcl-s pReq 			pointer;
	dcl-s pHttp 		pointer;
	dcl-s pMessage 		pointer;
	dcl-s pMessages 	pointer;
	dcl-s pfx  			char(3);

	pReq = json_newObject();
	json_setStr(pReq:'platformId' : 'COMMON_API'); 
	json_setStr(pReq:'platformPartnerId' : OPPADD);
	json_SetBool(pReq:'useDeliveryReport' : *OFF);

	pMessages = json_newArray();
	json_moveObjectInto(pReq:'sendRequestMessages' : pMessages);

	list = json_setIterator(pRows);
	DoW json_ForEach(list);

		// Opdater til status 1
		Xmsmstk = json_getNum(list.this : 'msmstk');
		exec sql
			update 
			msg00 
			set MSSTS = 1
			where msmstk = :Xmsmstk;
		pMessage = json_newObject();
		pfx = json_getStr(list.this : 'msmpfx');
		if pfx = '';
			pfx = SYPFX;
		endif;
		json_setStr(pMessage : 'source' : OPSNNM);
		json_setStr(pMessage : 'destination' : 
			'+' + %trim(pfx) + json_getStr(list.this : 'MSTMPN'));
		// reqData = json_getStr(list.this : 'MSTEXT');
		// reqData = xlateStr(reqData:0:1252);
		// json_setStr(pMessage : 'userData' : reqData);
		json_setStr(pMessage : 'userData' : 
			json_getStr(list.this : 'MSTEXT'));
		json_setStr(pMessage : 'refId' : json_getStr(list.this : 'msmstk'));
		json_arrayPush ( pMessages : pMessage);
	EndDo;
	
	return pReq;

end-proc;

// ------------------------------------------------------------------------------------
// handleResponse
// ------------------------------------------------------------------------------------
dcl-proc handleResponse;

	dcl-pi handleResponse ;
		pResponse 	pointer value;
		pRows 	 	pointer value;
		pMessages   pointer value;
	end-pi;

	dcl-s  ts		timestamp;
	dcl-s  msg		varchar(512);
	dcl-s  ErrFile	varchar(512);
	dcl-s  xmsmstk	int(10);
	Dcl-DS list     likeds(json_iterator);
	dcl-s ResultCode	varchar(16);
	if Trc;
		json_WriteJsonStmf (pResponse : '/tmp/smsresp.json': 1208 : *OFF );
	endif;
	list = json_setIterator(pResponse);
	if LIST.LENGTH > 0;
		DoW json_ForEach(list);
			Xmsmstk = json_getNum(list.this : 'refId');
			if Xmsmstk > 0;
				ResultCode = json_getStr(list.this : 'resultCode');
				if ResultCode = '1005';
					exec sql
						update 
						msg00 
						set MSSTS = 2,
							MSSNTS = now(),
							MSMSID = 'SMS2023'
						where msmstk = :Xmsmstk;
				else;
					Fail = *On;
					Msg = ResultCode + ' ' + json_getStr(list.this : 'message');
					exec sql
						update 
						msg00 set
							mssts = case
							when MSRTYC < 10 then 1
							else 3 end,
						MSSCTS = case
							when MSRTYC < 10 then current timestamp + 10 MINUTE
							else MSSCTS end,
						MSRTYC = MSRTYC + 1,
						MSMSID = 'SMS2129',
						MSMDTA = :Msg
						where msmstk = :Xmsmstk;
				endif;
			endif;
		enddo; 
	else;
		CheckError(pResponse: pRows : pMessages);
	endif;

end-proc;

//  	
// "sendRequestMessages": [
// {
// "source": "$senderID",
// "destination": "$RcpNumber",
// "userData": "$message ",
// "refId": "000000001"
// },
// {
// "source": "DHL",
// "destination": "+4551274443",
// "userData": "Det her skal gaa galt!",
// "refId": "000000002"
// }
// ]
// }
//  
// ------------------------------------------------------------------------------------
// CheckError	
// ------------------------------------------------------------------------------------
dcl-proc CheckError	;

	dcl-pi CheckError	 ;
		pResponse 	pointer value;
		pRows 	 	pointer value;
		pMessages   pointer value;
	end-pi;

	dcl-s  ts		timestamp;
	dcl-s  msg		varchar(512);
	dcl-s  ErrFile	varchar(512);
	dcl-s  xmsmstk	int(10);
	Dcl-DS list     likeds(json_iterator);

	Fail = *On;
	ts = %timestamp();

	if (pResponse = *NULL) ;
		json_joblog ('Connection failed !! ');
	endif;


	Msg = json_getStr (pResponse : 'status') +
		' ' + json_getStr (pResponse : 'description');
	if Msg = *blanks;
		Msg = %char(HttpSts);
	endif;
	// Dump resultat
	//ErrFile = '/tmp/smsreq'+%char(ts)+'.json';
	//json_WriteJsonStmf (phttp : ErrFile : 1208 : *OFF );
	ErrFile = '/tmp/smsresp'+%char(ts)+'.json';
	json_WriteJsonStmf (pResponse : ErrFile : 1208 : *OFF );
	list = json_setIterator(pRows); // Oprindelig liste fra udtrækket?
	DoW json_ForEach(list);
		Xmsmstk = json_getNum(list.this : 'msmstk');
		exec sql
			update 
			msg00 
			set MSSTS = 3,
			MSSNTS = :ts,
			MSMSID = 'SMS2129',
			MSMDTA = :Msg,
			MSRTYC = MSRTYC + 1
			where msmstk = :Xmsmstk;
	enddo;


end-proc;
// ------------------------------------------------------------------------------------
// unpackUrl
// ------------------------------------------------------------------------------------
dcl-proc unpackUrl;

	dcl-pi *n ;
		inUrl      varchar(256) const;
		Url        varchar(256);
		proxyUrl   varchar(256);
		proxyPort  varchar(5);
	end-pi;

	dcl-s pos	int(10);
	dcl-s pos2	int(10);

	url = inUrl; 
	proxyUrl  = '';
	proxyPort = '';

	// Tjek om der er proxy - hvis der står < i første karakter
	if %subst(inUrl:1:1) = '<';
		pos2 = %scan('>':inUrl);
		if pos2 = 0; // Fejl i definition!
			url = %subst(inUrl:2); // bedre end ingenting
		else;
			url = %subst(inUrl:pos2+1);
			pos = %scan(':':inUrl);
			if pos = 0 or pos >= pos2;
				ProxyPort = '8080';
				ProxyUrl = %subst(inUrl:2:pos2-1);
			else;
				ProxyPort = %subst(inUrl:pos+1:(pos2-pos)-1);
				ProxyUrl = %subst(inUrl:2:pos-2);
			endif;
		endif;
	endif;

end-proc;	 
