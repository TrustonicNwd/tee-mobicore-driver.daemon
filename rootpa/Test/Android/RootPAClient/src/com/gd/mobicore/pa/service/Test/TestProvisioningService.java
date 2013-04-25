/*
Copyright  © Trustonic Limited 2013

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice, this 
     list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright notice, 
     this list of conditions and the following disclaimer in the documentation 
     and/or other materials provided with the distribution.

  3. Neither the name of the Trustonic Limited nor the names of its contributors 
     may be used to endorse or promote products derived from this software 
     without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
OF THE POSSIBILITY OF SUCH DAMAGE.
*/

package com.gd.mobicore.pa.service.Test;

import android.app.Activity;
import android.os.Bundle;
import android.os.IBinder;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.ComponentName;
import android.content.Context;
import android.util.Log;

import android.os.AsyncTask;
import android.os.IBinder;
import android.content.IntentFilter;
import android.content.BroadcastReceiver;

import java.util.List;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.UUID;

import com.gd.mobicore.pa.ifc.RootPAProvisioningIntents;
import com.gd.mobicore.pa.ifc.RootPAServiceIfc;
import com.gd.mobicore.pa.ifc.CmpMsg;
import com.gd.mobicore.pa.ifc.CmpCommand;
import com.gd.mobicore.pa.ifc.CmpResponse;
import com.gd.mobicore.pa.ifc.CommandResult;
import com.gd.mobicore.pa.ifc.BooleanResult;
import com.gd.mobicore.pa.ifc.SPID;
import com.gd.mobicore.pa.ifc.Version;
import com.gd.mobicore.pa.ifc.SUID;
import com.gd.mobicore.pa.ifc.SPContainerStructure;
import com.gd.mobicore.pa.ifc.SPContainerStateParcel;
import com.gd.mobicore.pa.ifc.SPContainerState;
import com.gd.mobicore.pa.ifc.TrustletContainer;
import com.gd.mobicore.pa.ifc.TrustletContainerState;


class TestProvisioningService extends AsyncTask<IBinder, Void, Void> {
    private RootPAServiceIfc provisioningServiceIfc_=null; 
    private static final String TESTTAG = "RootPA-Test";
    private static final String TAG = "RootPA-T";
    private static final int TEST_SPID_VALUE=8;
    private static final int TEST_WRONG_SPID_VALUE=12;
    private static final int TEST_NO_TLT_SPID_VALUE=16;    
    
    RootPAClient parent_=null;
    TestProvisioningService(RootPAClient parent){
        super();
        parent_=parent;
    }
    
    protected Void doInBackground(IBinder... service) {
        provisioningServiceIfc_= RootPAServiceIfc.Stub.asInterface(service[0]);
        if(provisioningServiceIfc_ == null){
            Log.e(TAG,"FAILURE: no provisioningServiceIfc_, why?");            
        }

        String s=null;
        boolean res=true;

        try{
            Log.d(TAG, "testCmpCommands");
            res=testCmpCommands(parent_.stringList_);
        }catch(Throwable e){
            Log.e(TAG, "Executing testCmpCommands failed due to exception "+e);
            res=false;
        }
        parent_.allTestsPassed_=(parent_.allTestsPassed_==false)?parent_.allTestsPassed_:res;
        Log.i(TESTTAG,"=========================================================================="); 
        Log.i(TESTTAG,((res==true)?"SUCCESS":"FAILURE")+": Test executeCmpCommands");

        try{
            Log.d(TAG, "testRegistrationStatus");
            res=testRegistrationStatus(parent_.stringList_);
        }catch(Throwable e){
            Log.e(TAG, "Executing testRegistrationStatus failed due to exception "+e);
            res=false;
        }
        parent_.allTestsPassed_=(parent_.allTestsPassed_==false)?parent_.allTestsPassed_:res;
        Log.i(TESTTAG,((res==true)?"SUCCESS":"FAILURE")+": Test isRootContainerRegistered and isSPContainerRegistered");

        try{
            Log.d(TAG, "testGetters");
            res=testGetters(parent_.stringList_);
        }catch(Throwable e){
            Log.e(TAG, "Executing testGetters failed due to exception "+e);
            res=false;
        }
        parent_.allTestsPassed_=(parent_.allTestsPassed_==false)?parent_.allTestsPassed_:res;
        Log.i(TESTTAG,((res==true)?"SUCCESS":"FAILURE")+": Test getVersion and getDeviceId");

        try{
            Log.d(TAG, "testLock");
            res=testLock(parent_.stringList_);
        }catch(Throwable e){
            Log.e(TAG, "Executing testLock failed due to exception "+e);
            res=false;
        }
        parent_.allTestsPassed_=(parent_.allTestsPassed_==false)?parent_.allTestsPassed_:res;
        Log.i(TESTTAG,((res==true)?"SUCCESS":"FAILURE")+": Test acquireLock and releaseLock");

        try{
            Log.d(TAG, "testDoProvisioning");
            TEST_SPID = new SPID(TEST_SPID_VALUE);
            res=testDoProvisioning(parent_.stringList_, TEST_SPID);
        }catch(Throwable e){
            Log.e(TAG, "Executing testDoProvisioning failed due to exception "+e);
            res=false;
        }
        parent_.allTestsPassed_=(parent_.allTestsPassed_==false)?parent_.allTestsPassed_:res;
        Log.i(TESTTAG,((res==true)?"SUCCESS":"FAILURE")+": Test doProvisioning");


        try{
            Log.d(TAG, "testDoProvisioning, errors from SE");
            TEST_WRONG_SPID = new SPID(12); // not really wrong spid but a spid that causes errors to be returned
            res=testDoProvisioning(parent_.stringList_, TEST_WRONG_SPID);
        }catch(Throwable e){
            Log.e(TAG, "Executing testDoProvisioning, errors from SE, failed due to exception "+e);
            res=false;
        }
        parent_.allTestsPassed_=(parent_.allTestsPassed_==false)?parent_.allTestsPassed_:res;
        Log.i(TESTTAG,((res==true)?"SUCCESS":"FAILURE")+": Test doProvisioning, errors from SE");

        
        try{
            Log.d(TAG, "testContainerStructureAndState");
            res=testContainerStructureAndState(parent_.stringList_);
        }catch(Throwable e){
            Log.e(TAG, "Executing testContainerStructureAndState failed due to exception "+e);
            res=false;
        }
        parent_.allTestsPassed_=(parent_.allTestsPassed_==false)?parent_.allTestsPassed_:res;
        Log.i(TESTTAG,((res==true)?"SUCCESS":"FAILURE")+": Test getSPContainerStructure and getSPContainerState");

        try{
            Log.d(TAG, "testSessionHandling");
            res=testSessionHandling(parent_.stringList_);
        }catch(Throwable e){
            Log.e(TAG, "Executing testSessionHandling failed due to exception "+e);
            res=false;
        }
        parent_.allTestsPassed_=(parent_.allTestsPassed_==false)?parent_.allTestsPassed_:res;
        Log.i(TESTTAG,((res==true)?"SUCCESS":"FAILURE")+": Test session handling");

        try{
            Log.d(TAG, "test CMP error cases");
            res=testCmpErrorCases(parent_.stringList_);
        }catch(Throwable e){
            Log.e(TAG, "Executing test CMP error cases failed due to exception "+e);
            res=false;
        }
        parent_.allTestsPassed_=(parent_.allTestsPassed_==false)?parent_.allTestsPassed_:res;
        Log.i(TESTTAG,((res==true)?"SUCCESS":"FAILURE")+": Test CMP error cases");
        
        parent_.printFinalResults();
        return null;
    }

    public void onServiceDisconnected(ComponentName className){
        disconnect();
    }

    /**
    Test content management protocol commands
    */
    private boolean testCmpCommands(List<String> stringList){
        String logi=new String();
        CommandResult  res=new CommandResult(0x0FFF0000);
        int uid=129;
        List<CmpCommand> commands=null;
        List<CmpResponse> responses=null;
        
        List<CmpTest> cmpTestCases = CmpTest.generateAll();
        try{
            commands=new ArrayList<CmpCommand>();
            responses=new ArrayList<CmpResponse>();
            for(int i=0; i<cmpTestCases.size(); i++){
                commands.add(cmpTestCases.get(i).createCommand());
                commands.get(i).setIgnoreError(true);
            }
        }catch(java.lang.Exception e){
            Log.e(TAG,"FAILURE: broken test case, initializing data failed: ",e);
        }           

        try{
            provisioningServiceIfc_.acquireLock(uid); // no checking of return value since we do not test lock here (might help in debugging though)
            res=provisioningServiceIfc_.executeCmpCommands(uid, commands, responses);
            provisioningServiceIfc_.releaseLock(uid); // no checking of return value since we do not test lock here (might help in debugging though)
        }catch(Throwable e){
            res = new CommandResult(0x0FFFFFF);
            logi = logi.concat("FAILURE: call to executeCmpCommands failed: " + e + "\n");
        }           
        boolean success=true;
        boolean overallsuccess=success;
        

        for(int i=0; i<cmpTestCases.size(); i++){
            try{
                cmpTestCases.get(i).checkResult(responses.get(i));
            } catch(java.lang.Exception e){
                Log.e(TAG,"FAILURE: checking results from test "+ cmpTestCases.get(i).id() +" failed: ",e);
            }                   
        }

        overallsuccess=res.isOk();
        logi = logi.concat("================= Results of testing executeCmpCommands\n");
        logi = logi.concat(((overallsuccess==true)?"SUCCESS":"FAILURE")+": executeCmpCommands returned: "+res+"\n");
        for(int i=0; i<cmpTestCases.size(); i++){
            logi = logi.concat(((cmpTestCases.get(i).result_==true)?"SUCCESS":"FAILURE")+": Testing "+cmpTestCases.get(i).id()+"\n");
            overallsuccess=(overallsuccess==true)?cmpTestCases.get(i).result_:overallsuccess;
        }
        logi = logi.concat("==========================================================================\n");
        stringList.add(logi);
        return overallsuccess;
    }

    private SPID TEST_SPID;
    private SPID TEST_WRONG_SPID;
    private static final int TEST_UID=0x0FFF0000;
    private static final int TEST_UID_LOCKED=4321;
    
    private boolean testRegistrationStatus(List<String> stringList){

        TEST_SPID = new SPID(TEST_SPID_VALUE);
        TEST_WRONG_SPID=new SPID(TEST_WRONG_SPID_VALUE);
        
        String logi=new String();
        boolean overallsuccess=true;

        BooleanResult rcResult=new BooleanResult(false);
        BooleanResult scResult=new BooleanResult(false);
        BooleanResult scErrResult=new BooleanResult(false);        

        CommandResult rcRet = new CommandResult(0x0FFF0000);
        CommandResult scRet = new CommandResult(0x0FFF0000);
        CommandResult scErrRet = new CommandResult(0x0FFF0000);
        CommandResult scErr2Ret = new CommandResult(0x0FFF0000);
        CommandResult scErr3Ret = new CommandResult(0x0FFF0000);
        CommandResult rcErrRet = new CommandResult(0x0FFF0000);
        
// success cases
        
        try{
            rcRet = provisioningServiceIfc_.isRootContainerRegistered(rcResult);
        }catch(Throwable e){
            logi = logi.concat("FAILURE: call to isRootContainerRegistered failed: " + e + "\n");
            rcRet = new CommandResult(0x0FFFFFFF);
            overallsuccess=false;
        }

        try{
            scRet = provisioningServiceIfc_.isSPContainerRegistered(TEST_SPID, scResult);
        }catch(Throwable e){
            logi = logi.concat("FAILURE: call to isSPContainerRegistered failed: " + e + "\n");
            scRet = new CommandResult(0x0FFFFFFF);
            overallsuccess=false;
        }

// "error" cases
       
        try{
            scErrRet = provisioningServiceIfc_.isSPContainerRegistered(TEST_WRONG_SPID, scErrResult);
        }catch(Throwable e){
            logi = logi.concat("FAILURE: call to isSPContainerRegistered with wrong SPID failed: " + e + "\n");
            scErrRet = new CommandResult(0x0FFFFFFF);
            overallsuccess=false;
        }
        
        try{
            scErr2Ret = provisioningServiceIfc_.isSPContainerRegistered(null, scErrResult);
        }catch(Throwable e){
            logi = logi.concat("FAILURE: call to isSPContainerRegistered with null SPID failed: " + e + "\n");
            scErr2Ret = new CommandResult(0x0FFFFFFF);
            overallsuccess=false;
        }

        try{
            scErr3Ret = provisioningServiceIfc_.isSPContainerRegistered(TEST_SPID, null);
        }catch(java.lang.NullPointerException s){
            Log.d(TAG,"call to isSPContainerRegistered with result resulted to null pointer exception as expected\n");
            scErr3Ret = new CommandResult(0x0FFFFFFF);
        }catch(Throwable e){
            logi = logi.concat("FAILURE: call to isSPContainerRegistered null data failed: " + e + "\n");
            scErr3Ret = new CommandResult(0x0FFFFFF0);
            overallsuccess=false;
        }

        try{
            rcErrRet = provisioningServiceIfc_.isRootContainerRegistered(null);
        }catch(java.lang.NullPointerException s){
            Log.d(TAG,"call to isRootContainerRegistered with result resulted to null pointer exception as expected\n");
            rcErrRet = new CommandResult(0x0FFFFFFF);            
        }catch(Throwable e){
            logi = logi.concat("FAILURE: call to isRootContainerRegistered null data failed: " + e + "\n");
            rcErrRet = new CommandResult(0x0FFFFFF0);
            overallsuccess=false;
        }

// result handling
        
        overallsuccess=!overallsuccess?overallsuccess:(rcRet.isOk());
        overallsuccess=!overallsuccess?overallsuccess:(rcResult.result()==true);
        overallsuccess=!overallsuccess?overallsuccess:(scRet.isOk());
        overallsuccess=!overallsuccess?overallsuccess:(scResult.result()==true);

        overallsuccess=!overallsuccess?overallsuccess:(scErrRet.isOk());
        overallsuccess=!overallsuccess?overallsuccess:(scErrResult.result()==false);
        
        overallsuccess=!overallsuccess?overallsuccess:(scErr2Ret.result()==CommandResult.ROOTPA_ERROR_ILLEGAL_ARGUMENT);
        overallsuccess=!overallsuccess?overallsuccess:(scErr3Ret.result()==0x0FFFFFFF);
        overallsuccess=!overallsuccess?overallsuccess:(rcErrRet.result()==0x0FFFFFFF);
        
        logi = logi.concat("================= Results of testing registration status: ").concat(((overallsuccess==true)?"SUCCESS":"FAILURE")+"\n");
        logi = logi.concat(((rcRet.isOk())?"SUCCESS":"FAILURE")+": Testing isRootContainerRegistered "+((rcRet.isOk())?"\n":("returned: " +rcRet+" \n")));
        logi = logi.concat(((rcResult.result()==true)?"SUCCESS":"FAILURE")+": Testing isRootContainerRegistered boolean result "+((rcResult.result()==true)?"\n":("is not registered even though expected\n")));

        logi = logi.concat(((scRet.isOk())?"SUCCESS":"FAILURE")+": Testing isSPContainerRegistered "+((scRet.isOk())?"\n":("returned: " +scRet+" \n")));
        logi = logi.concat(((scResult.result()==true)?"SUCCESS":"FAILURE")+": Testing isSPContainerRegistered boolean result "+((scResult.result()==true)?"\n":("is not registered even though expected\n"))); 

        logi = logi.concat(((scErrRet.isOk())?"SUCCESS":"FAILURE")+": Testing isSPContainerRegistered "+((scErrRet.isOk())?"\n":("returned: " +scErrRet+" \n")));
        logi = logi.concat(((scErrResult.result()==false)?"SUCCESS":"FAILURE")+": Testing isSPContainerRegistered boolean result "+((scErrResult.result()==false)?"\n":("is registered even though not expected\n"))); 


        logi = logi.concat(((scErr2Ret.result()==CommandResult.ROOTPA_ERROR_ILLEGAL_ARGUMENT)?"SUCCESS":"FAILURE")+": Testing isSPContainerRegistered null spid"+
                ((scErr2Ret.result()==CommandResult.ROOTPA_ERROR_ILLEGAL_ARGUMENT)?"\n":(" returned: " +scErr2Ret+" \n")));
        logi = logi.concat(((scErr3Ret.result()==0x0FFFFFFF)?"SUCCESS":"FAILURE")+": Testing isSPContainerRegistered null data"+
                ((scErr3Ret.result()==0x0FFFFFFF)?"\n":(" returned: " +scErr3Ret+" \n")));
        logi = logi.concat(((rcErrRet.result()==0x0FFFFFFF)?"SUCCESS":"FAILURE")+": Testing isRootContainerRegistered null data"+
                ((rcErrRet.result()==0x0FFFFFFF)?"\n":(" returned: " +rcErrRet+" \n")));
        
        logi = logi.concat("==========================================================================\n");

        stringList.add(logi);            
        return overallsuccess;
    }
    
    private boolean testGetters(List<String> stringList){

        String logi=new String();
        boolean overallsuccess=true;
        CommandResult gvRet=new CommandResult(0x0FFF0000);
        CommandResult gdiRet=new CommandResult(0x0FFF0000);
        SUID test_suid = new SUID();
        Version test_version = new Version();
        final String EXPECTED_PRODUCT_ID="xxxxx";
        final int EXPECTED_TAG=2;
        final int EXPECTED_MCI=1;
        final int EXPECTED_SO=2;
        final int EXPECTED_MCLF=3;
        final int EXPECTED_CONT=4;            
        final int EXPECTED_MCCONF=5;
        final int EXPECTED_TLAPI=6;
        final int EXPECTED_DRAPI=7;
        final int EXPECTED_CMP=8;

        try{
            gvRet=provisioningServiceIfc_.getVersion(test_version);
        }catch(Throwable e){
            logi = logi.concat("FAILURE: call to getVersion failed: " + e + "\n");
            gvRet = new CommandResult(0x0FFFFFFF);
            overallsuccess=false;
        }

        try{
            gdiRet=provisioningServiceIfc_.getDeviceId(test_suid);        
        }catch(Throwable e){
            logi = logi.concat("FAILURE: call to getDeviceId failed: " + e + "\n");
            gdiRet = new CommandResult(0x0FFFFFFF);
            overallsuccess=false;
        }

        int tag=test_version.version().getInt("TAG");
        int mci=test_version.version().getInt("MCI");
        int so=test_version.version().getInt("SO");
        int mclf=test_version.version().getInt("MCLF");
        int cont=test_version.version().getInt("CONT");
        int mcconf=test_version.version().getInt("MCCONF");
        int tlapi=test_version.version().getInt("TLAPI");
        int drapi=test_version.version().getInt("DRAPI");
        int cmp=test_version.version().getInt("CMP");

        overallsuccess=!overallsuccess?overallsuccess:(gvRet.isOk());
        overallsuccess=!overallsuccess?overallsuccess:(test_version.productId().compareTo(EXPECTED_PRODUCT_ID)==0);
        overallsuccess=!overallsuccess?overallsuccess:(tag==EXPECTED_TAG);
        overallsuccess=!overallsuccess?overallsuccess:(mci==EXPECTED_MCI);
        overallsuccess=!overallsuccess?overallsuccess:(so==EXPECTED_SO);
        overallsuccess=!overallsuccess?overallsuccess:(mclf==EXPECTED_MCLF);
        overallsuccess=!overallsuccess?overallsuccess:(cont==EXPECTED_CONT);            
        overallsuccess=!overallsuccess?overallsuccess:(mcconf==EXPECTED_MCCONF);
        overallsuccess=!overallsuccess?overallsuccess:(tlapi==EXPECTED_TLAPI);
        overallsuccess=!overallsuccess?overallsuccess:(drapi==EXPECTED_DRAPI);                        
        overallsuccess=!overallsuccess?overallsuccess:(cmp==EXPECTED_CMP);

        overallsuccess=!overallsuccess?overallsuccess:(gdiRet.isOk());
        overallsuccess=!overallsuccess?overallsuccess:(Arrays.equals(test_suid.suid(),CmpTest.EXPECTED_SUID));

        logi = logi.concat("================= Results of testing getters: ").concat(((overallsuccess==true)?"SUCCESS":"FAILURE")+"\n");          
        logi = logi.concat(((gvRet.isOk())?"SUCCESS":"FAILURE")+": Testing getVersion "+((gvRet.isOk())?" \n":("returned: " +gvRet+" \n")));
        logi = logi.concat(((test_version.productId().compareTo(EXPECTED_PRODUCT_ID)==0)?"SUCCESS":"FAILURE")+": Testing getVersion, productId "+((
                test_version.productId().compareTo(EXPECTED_PRODUCT_ID)==0)?" \n":("productId: " +test_version.productId()+" \n")));
        logi = logi.concat(((tag==EXPECTED_TAG)?"SUCCESS":"FAILURE")+": Testing getVersion, tag"+((tag==EXPECTED_TAG)?" \n":(": " +tag+" \n")));
        logi = logi.concat(((mci==EXPECTED_MCI)?"SUCCESS":"FAILURE")+": Testing getVersion, mci"+((mci==EXPECTED_MCI)?" \n":(": " +mci+" \n")));
        logi = logi.concat(((so==EXPECTED_SO)?"SUCCESS":"FAILURE")+": Testing getVersion, so"+((so==EXPECTED_SO)?" \n":(": " +so+" \n")));
        logi = logi.concat(((mclf==EXPECTED_MCLF)?"SUCCESS":"FAILURE")+": Testing getVersion, mclf"+((mclf==EXPECTED_MCLF)?" \n":(": " +mclf+" \n")));
        logi = logi.concat(((cont==EXPECTED_CONT)?"SUCCESS":"FAILURE")+": Testing getVersion, cont"+((cont==EXPECTED_CONT)?" \n":(": " +cont+" \n")));
        logi = logi.concat(((mcconf==EXPECTED_MCCONF)?"SUCCESS":"FAILURE")+": Testing getVersion, mcconf"+((mcconf==EXPECTED_MCCONF)?" \n":(": " +mcconf+" \n")));
        logi = logi.concat(((tlapi==EXPECTED_TLAPI)?"SUCCESS":"FAILURE")+": Testing getVersion, tlapi"+((tlapi==EXPECTED_TLAPI)?" \n":(": " +tlapi+" \n")));
        logi = logi.concat(((drapi==EXPECTED_DRAPI)?"SUCCESS":"FAILURE")+": Testing getVersion, drapi"+((drapi==EXPECTED_DRAPI)?" \n":(": " +drapi+" \n")));
        logi = logi.concat(((cmp==EXPECTED_CMP)?"SUCCESS":"FAILURE")+": Testing getVersion, cmp"+((cmp==EXPECTED_CMP)?" \n":(": " +cmp+" \n")));


        logi = logi.concat(((gdiRet.isOk())?"SUCCESS":"FAILURE")+": Testing getDeviceId "+((gdiRet.isOk())?" \n":("returned: " +gdiRet+" \n")));
        logi = logi.concat(((Arrays.equals(test_suid.suid(),CmpTest.EXPECTED_SUID))?"SUCCESS":"FAILURE")+": Testing getDeviceId, suid "+((Arrays.equals(test_suid.suid(),CmpTest.EXPECTED_SUID))?" \n":("returned: " +CmpTest.byteArrayToDisplayable(test_suid.suid())+" \n")));
        logi = logi.concat("==========================================================================\n");

        stringList.add(logi);
        return overallsuccess;
    }

    private boolean testLock(List<String> stringList){

        String logi=new String();
        boolean overallsuccess=true;
        CommandResult alRet=new CommandResult(0x0FFF0000);
        CommandResult alLockedRet=new CommandResult(0x0FFF0000);
        CommandResult rlLockedRet=new CommandResult(0x0FFF0000);
        CommandResult rlRet=new CommandResult(0x0FFF0000);
        CommandResult rlOpenRet=new CommandResult(0x0FFF0000);

    // first test locking when the lock is open            
        try{
            alRet=provisioningServiceIfc_.acquireLock(TEST_UID);
        }catch(Throwable e){
            logi = logi.concat("FAILURE: call to acquireLock failed: " + e + "\n");
            alRet = new CommandResult(0x0FFFFFFF);
            overallsuccess=false;
        }

    // then test locking when the lock is aquired 
        try{
            alLockedRet=provisioningServiceIfc_.acquireLock(TEST_UID_LOCKED);
        }catch(Throwable e){
            logi = logi.concat("FAILURE: call to acquireLock failed: " + e + "\n");
            alLockedRet = new CommandResult(0x0FFFFFFF);
            overallsuccess=false;
        }
    
    // then test opening the lock with wrong UID when the lock is aquired 
        try{
            rlLockedRet=provisioningServiceIfc_.releaseLock(TEST_UID_LOCKED);        
        }catch(Throwable e){
            logi = logi.concat("FAILURE: call to releaseLock failed: " + e + "\n");
            rlLockedRet = new CommandResult(0x0FFFFFFF);
            overallsuccess=false;
        }

    // then test opening the lock when the lock is aquired 
        try{
            rlRet=provisioningServiceIfc_.releaseLock(TEST_UID);        
        }catch(Throwable e){
            logi = logi.concat("FAILURE: call to releaseLock failed: " + e + "\n");
            rlRet = new CommandResult(0x0FFFFFFF);
            overallsuccess=false;
        }

    // then test opening the lock when already open
        try{
            rlOpenRet=provisioningServiceIfc_.releaseLock(TEST_UID);        
        }catch(Throwable e){
            logi = logi.concat("FAILURE: call to releaseLock failed: " + e + "\n");
            rlOpenRet = new CommandResult(0x0FFFFFFF);
            overallsuccess=false;
        }

        overallsuccess=!overallsuccess?overallsuccess:(alRet.isOk());
        overallsuccess=!overallsuccess?overallsuccess:(alLockedRet.result()==CommandResult.ROOTPA_ERROR_LOCK);
        overallsuccess=!overallsuccess?overallsuccess:(rlLockedRet.result()==CommandResult.ROOTPA_ERROR_LOCK);
        overallsuccess=!overallsuccess?overallsuccess:(rlRet.isOk());
        overallsuccess=!overallsuccess?overallsuccess:(rlOpenRet.isOk());

        logi = logi.concat("================= Results of testing lock: ").concat(((overallsuccess==true)?"SUCCESS":"FAILURE")+"\n");
        logi = logi.concat(((alRet.isOk())?"SUCCESS":"FAILURE")+": Testing acquireLock "+((alRet.isOk())?" \n":("returned: " +alRet+" \n")));
        logi = logi.concat(((alLockedRet.result()==CommandResult.ROOTPA_ERROR_LOCK)?"SUCCESS":"FAILURE")+": Testing acquireLock locked"+
               ((alLockedRet.result()==CommandResult.ROOTPA_ERROR_LOCK)?" \n":("returned: " +alLockedRet+" \n")));

        logi = logi.concat(((rlLockedRet.result()==CommandResult.ROOTPA_ERROR_LOCK)?"SUCCESS":"FAILURE")+": Testing releaseLock locked with different uid "+
               ((rlLockedRet.result()==CommandResult.ROOTPA_ERROR_LOCK)?" \n":("returned: " +rlLockedRet+" \n")));

        logi = logi.concat(((rlRet.isOk())?"SUCCESS":"FAILURE")+": Testing releaseLock "+((rlRet.isOk())?" \n":("returned: " +rlRet+" \n")));            
        logi = logi.concat(((rlOpenRet.isOk())?"SUCCESS":"FAILURE")+": Testing releaseLock already open "+((rlOpenRet.isOk())?" \n":("returned: " +rlOpenRet+" \n")));            
        logi = logi.concat("==========================================================================\n");

        stringList.add(logi);
        return overallsuccess;
    }

    final boolean[] intentsReceived_=new boolean[7]; // 7 == number of intents to be received
    final CommandResult intentError_=new CommandResult(CommandResult.ROOTPA_OK);
    private synchronized boolean allIntentsReceived(){
        return (intentsReceived_[0] && intentsReceived_[1] && intentsReceived_[2] && intentsReceived_[3] && intentsReceived_[4] && intentsReceived_[5] && intentsReceived_[6]);
    }

    private boolean someIntentsReceived(){
        return (intentsReceived_[0] ^ intentsReceived_[1] ^ intentsReceived_[2] ^ intentsReceived_[3] ^ intentsReceived_[4] ^ intentsReceived_[5] ^ intentsReceived_[6]);
    }
    
    private boolean seErrorIntentsReceived(){
        Log.d(TAG, "seErrorIntentsReceived "+intentsReceived_[0] +" "+intentsReceived_[1] +" "+ !intentsReceived_[2] +" "+ intentsReceived_[3] +" "+ !intentsReceived_[4] +" "+ !intentsReceived_[5] +" "+ intentsReceived_[6]);
        return (intentsReceived_[0] && intentsReceived_[1] && !intentsReceived_[2] && intentsReceived_[3] && !intentsReceived_[4] && !intentsReceived_[5] && intentsReceived_[6]);
    }
    
    private synchronized void markIntentReceived(int i){
        intentsReceived_[i]=true;
    }

    private int statusToIndex(int status){
        switch (status){
            case RootPAProvisioningIntents.CONNECTING_SERVICE_ENABLER:
                return 1; //C_CONNECTING_SERVICE_ENABLER;
            case RootPAProvisioningIntents.AUTHENTICATING_SOC:
                return 2; //C_AUTHENTICATING_SOC;
            case RootPAProvisioningIntents.AUTHENTICATING_ROOT:
                return 3; //C_AUTHENTICATING_ROOT;
            case RootPAProvisioningIntents.CREATING_ROOT_CONTAINER:
                return 4; //C_CREATING_ROOT_CONTAINER;
            case RootPAProvisioningIntents.CREATING_SP_CONTAINER:
                return 5; //C_CREATING_SP_CONTAINER;
            case RootPAProvisioningIntents.FINISHED_PROVISIONING:
                return 6; //C_FINISHED_PROVISIONING;
            default:
                return 0;
        }
    }
    
    private boolean testDoProvisioning(List<String> stringList, SPID spid){
        return testDoProvisioning(stringList, false, spid);
    }
    
    private boolean testDoProvisioning(List<String> stringList, boolean embeddedInOtherTest, SPID spid){
        String logi=new String();
        boolean overallsuccess=true;
        CommandResult ret=new CommandResult(0x0FFF0000);
        for(int i=0; i<intentsReceived_.length; i++) intentsReceived_[i]=false;
        intentError_.setValue(CommandResult.ROOTPA_OK);
    // set receiver for intents
    
        android.os.HandlerThread handlerThread = new android.os.HandlerThread("intentReceiverThread");
        handlerThread.start();
        android.os.Looper looper = handlerThread.getLooper();
        android.os.Handler handler= new android.os.Handler(looper);

        BroadcastReceiver receiver=new BroadcastReceiver(){
            public void onReceive(Context ctx, Intent intent){
                int status = intent.getIntExtra(RootPAProvisioningIntents.STATE, 0);
                Log.d(TAG, "Received status intent: "+status);
                markIntentReceived( statusToIndex(status));
            }
        };
        IntentFilter filter=new IntentFilter(RootPAProvisioningIntents.PROVISIONING_PROGRESS_UPDATE);
        parent_.registerReceiver(receiver, filter, null, handler);

        BroadcastReceiver endReceiver=new BroadcastReceiver(){
            public void onReceive(Context ctx, Intent intent){
                Log.d(TAG, "Received finished intent");
                markIntentReceived(0);
            }            
        };
        IntentFilter endFilter=new IntentFilter(RootPAProvisioningIntents.FINISHED_ROOT_PROVISIONING);
        parent_.registerReceiver(endReceiver, endFilter, null, handler);

        BroadcastReceiver errorReceiver=new BroadcastReceiver(){
            public void onReceive(Context ctx, Intent intent){
                intentError_.setValue(intent.getIntExtra(RootPAProvisioningIntents.ERROR, 9999));
                Log.d(TAG, "Provisioning: Received error intent: "+intentError_);
                markIntentReceived(0);
            }            
        };
        IntentFilter errorFilter=new IntentFilter(RootPAProvisioningIntents.PROVISIONING_ERROR);
        parent_.registerReceiver(errorReceiver, errorFilter, null, handler);

        try{
            ret=provisioningServiceIfc_.doProvisioning(TEST_UID, spid);
        }catch(Throwable e){
            logi = logi.concat("FAILURE: call to doProvisioning failed: " + e + "\n");
            Log.d(TAG, ": call to doProvisioning failed: "+e);
            ret = new CommandResult(0x0FFFFFFF);
            overallsuccess=false;
        }

    // we need to wait until all the intents are received, otherwise the interface 
    // is still locked when trying to execute the following tests
        if(ret.isOk()){
            try {
                int totalTime=0;
                while(!allIntentsReceived()){
                    Thread.sleep(500);
                    totalTime+=500;
                    if(totalTime>6000){
                        break;
                    }
                }
                Log.d(TAG, "totalTime: "+totalTime);
                Thread.sleep(1000); // waiting for one more second to ensure the locks are opened for the next tests    
            } catch (Exception e) {
                Log.d(TAG, "sleep failed "+e);
            }
        } else {
            Log.d(TAG, "error when calling doProvisioning "+ret);
        }
        
        parent_.unregisterReceiver(receiver);
        parent_.unregisterReceiver(endReceiver);
        parent_.unregisterReceiver(errorReceiver);
        handlerThread.quit();
        

        overallsuccess=!overallsuccess?overallsuccess:(ret.isOk());
        if(TEST_SPID!=null && spid==TEST_SPID){
            overallsuccess=!overallsuccess?overallsuccess:(intentError_.isOk());
            overallsuccess=!overallsuccess?overallsuccess:allIntentsReceived();            
        }else{
            overallsuccess=!overallsuccess?overallsuccess:(intentError_.result()==CommandResult.ROOTPA_COMMAND_NOT_SUPPORTED); // only the last error is sent as intent
            overallsuccess=!overallsuccess?overallsuccess:seErrorIntentsReceived();
        }       
        
        if(!embeddedInOtherTest)
        {
            if(TEST_SPID!=null && spid==TEST_SPID)
            {
                logi = logi.concat("================= Results of testing doProvisioning: ").concat(((overallsuccess==true)?"SUCCESS":"FAILURE")+"\n");
            }
            else
            {
                logi = logi.concat("================= Results of testing doProvisioning, errors from SE: ").concat(((overallsuccess==true)?"SUCCESS":"FAILURE")+"\n");
            }
        }
        if(TEST_SPID!=null && spid==TEST_SPID){
            for(int i=0; i < 7; i++){ // 7 == number of intents to be received
                logi = logi.concat(((intentsReceived_[i]==true)?"SUCCESS":"FAILURE")+": Testing doProvisioning, receiving intent["+i+"]\n");
            }
            logi = logi.concat(((intentError_.isOk())?"SUCCESS":"FAILURE")+": Testing doProvisioning, intentError "+((intentError_.isOk())?"\n":("returned: " +intentError_+" \n")));            
        }else{
            for(int i=0; i < 7; i++){ 
                if(0 == i || 1 == i || 3 == i ){ // in the error case not all intents are received
                    logi = logi.concat(((intentsReceived_[i]==true)?"SUCCESS":"FAILURE")+": Testing doProvisioning, receiving intent["+i+"]\n");
                }
            }
            // only the last error is sent as intent
            logi = logi.concat(((intentError_.result()==CommandResult.ROOTPA_COMMAND_NOT_SUPPORTED)?"SUCCESS":"FAILURE")+": Testing doProvisioning, intentError "+
                        ((intentError_.result()==CommandResult.ROOTPA_COMMAND_NOT_SUPPORTED)?"\n":("returned: " +intentError_+" \n")));
        }
        logi = logi.concat(((ret.isOk())?"SUCCESS":"FAILURE")+": Testing doProvisioning "+((ret.isOk())?"\n":("returned: " +ret+" \n")));
        if(!embeddedInOtherTest)
        {                             
            logi = logi.concat("==========================================================================\n");
        }

        if(!embeddedInOtherTest || someIntentsReceived())
        {
            stringList.add(logi);
        }
        return overallsuccess;
    }
   
    private boolean testContainerStructureAndState(List<String> stringList){

        final SPContainerState EXPECTED_SP_CONT_STATE=SPContainerState.SP_LOCKED;
        final SPContainerState EXPECTED_SP_CONT_STATE_ERR=SPContainerState.DOES_NOT_EXIST;
        final int EXPECTED_NUMBER_OF_TRUSTLETS=2;
        final TrustletContainerState EXPECTED_TLT_CONT_STATE=TrustletContainerState.SP_LOCKED;
        long FOURS=0x0303030304040404L;
        long FIVES=0x0505050506060606L;
        final UUID EXPECTED_TLT_ID=new UUID(FOURS,FIVES); //CmpTest.TLTUUID;

        TEST_SPID = new SPID(TEST_SPID_VALUE);
        TEST_WRONG_SPID=new SPID(TEST_WRONG_SPID_VALUE);
        SPID TEST_SPID_UNKNOWN_TLT_CONT_INDICES=new SPID(TEST_NO_TLT_SPID_VALUE);
        String logi=new String();
        boolean overallsuccess=true;
        CommandResult strRet=new CommandResult(0x0FFF0000);
        CommandResult stRet=new CommandResult(0x0FFF0000);
        CommandResult strErrRet=new CommandResult(0x0FFF0000);
        CommandResult stErrRet=new CommandResult(0x0FFF0000);
        CommandResult strErr2Ret=new CommandResult(0x0FFF0000);
        CommandResult stErr2Ret=new CommandResult(0x0FFF0000);
        CommandResult strErr3Ret=new CommandResult(0x0FFF0000);
        CommandResult stErr3Ret=new CommandResult(0x0FFF0000);
        CommandResult strErr4Ret=new CommandResult(0x0FFF0000);

        SPContainerStructure test_structure = new SPContainerStructure();
        SPContainerStateParcel test_state = new SPContainerStateParcel();           
        SPContainerStructure test_structure_2 = new SPContainerStructure();
        SPContainerStateParcel test_state_2 = new SPContainerStateParcel();           
        SPContainerStructure test_structure_3 = new SPContainerStructure();
        SPContainerStateParcel test_state_3 = new SPContainerStateParcel();           
        SPContainerStructure test_structure_4 = new SPContainerStructure();        

// success cases
        try{
            strRet=provisioningServiceIfc_.getSPContainerStructure(TEST_SPID, test_structure);
        }catch(Exception e){
            logi = logi.concat("FAILURE: call to getSPContainerStructure failed: " + e + "\n");
            strRet = new CommandResult(0x0FFFFFFF);
            overallsuccess=false;
        }

        try{
            stRet=provisioningServiceIfc_.getSPContainerState(TEST_SPID, test_state);
        }catch(Exception e){
            logi = logi.concat("FAILURE: call to getSPContainerState failed: " + e + "\n");
            stRet = new CommandResult(0x0FFFFFFF);
            overallsuccess=false;
        }

// "error" cases

        try{
            strErrRet=provisioningServiceIfc_.getSPContainerStructure(TEST_WRONG_SPID, test_structure_2);
        }catch(Exception e){
            logi = logi.concat("FAILURE: call to getSPContainerStructure with wrong SPID failed: " + e + "\n");
            strErrRet = new CommandResult(0x0FFFFFFF);
            overallsuccess=false;
        }

        try{
            stErrRet=provisioningServiceIfc_.getSPContainerState(TEST_WRONG_SPID, test_state_2);
        }catch(Exception e){
            logi = logi.concat("FAILURE: call to getSPContainerState with wrong SPID failed: " + e + "\n");
            stErrRet = new CommandResult(0x0FFFFFFF);
            overallsuccess=false;
        }

        try{
            strErr2Ret=provisioningServiceIfc_.getSPContainerStructure(TEST_SPID, null);
        }catch(java.lang.NullPointerException s){
            Log.d(TAG,"call to getSPContainerStructure with null state resulted to null pointer exception as expected\n");
            strErr2Ret = new CommandResult(0x0FFFFFFF);            
        }catch(Exception e){
            logi = logi.concat("FAILURE: call to getSPContainerStructure with null structure failed: " + e + "\n");
            strErr2Ret = new CommandResult(0x0FFFFFF0);
            overallsuccess=false;
        }

        try{
            stErr2Ret=provisioningServiceIfc_.getSPContainerState(TEST_SPID, null);
        }catch(java.lang.NullPointerException s){
            Log.d(TAG,"call to getSPContainerState with null state resulted to null pointer exception as expected\n");
            stErr2Ret = new CommandResult(0x0FFFFFFF);            
        }catch(Exception e){
            logi = logi.concat("FAILURE: call to getSPContainerState with null state failed: " + e + "\n");
            stErr2Ret = new CommandResult(0x0FFFFFF0);
            overallsuccess=false;
        }

        try{
            strErr3Ret=provisioningServiceIfc_.getSPContainerStructure(null, test_structure_3);
        }catch(Exception e){
            logi = logi.concat("FAILURE: call to getSPContainerStructure with null SPID failed: " + e + "\n");
            strErr3Ret = new CommandResult(0x0FFFFFFF);
            overallsuccess=false;
        }

        try{
            stErr3Ret=provisioningServiceIfc_.getSPContainerState(null, test_state_3);
        }catch(Exception e){
            logi = logi.concat("FAILURE: call to getSPContainerState with null SPID failed: " + e + "\n");
            stErr3Ret = new CommandResult(0x0FFFFFFF);
            overallsuccess=false;
        }


        try{
            strErr4Ret=provisioningServiceIfc_.getSPContainerStructure(TEST_SPID_UNKNOWN_TLT_CONT_INDICES, test_structure_4);
        }catch(Exception e){
            logi = logi.concat("FAILURE: call to getSPContainerStructure where sp container points to missing tlt containers failed: " + e + "\n");
            strErr4Ret = new CommandResult(0x0FFFFFFF);
            overallsuccess=false;
        }
        
    
        
        List<TrustletContainer>	trustlets = test_structure.tcList();
        
        overallsuccess=!overallsuccess?overallsuccess:(strRet.isOk());
        overallsuccess=!overallsuccess?overallsuccess:(test_structure.state()==EXPECTED_SP_CONT_STATE);
        overallsuccess=!overallsuccess?overallsuccess:(trustlets.size()==EXPECTED_NUMBER_OF_TRUSTLETS);

        
        if(overallsuccess==true){
        
            overallsuccess=!overallsuccess?overallsuccess:(trustlets.get(0).state().getEnumeratedValue()==EXPECTED_TLT_CONT_STATE);
            overallsuccess=!overallsuccess?overallsuccess:(trustlets.get(0).trustletId().compareTo(EXPECTED_TLT_ID)==0);
            overallsuccess=!overallsuccess?overallsuccess:(trustlets.get(1).state().getEnumeratedValue()==EXPECTED_TLT_CONT_STATE);
            overallsuccess=!overallsuccess?overallsuccess:(trustlets.get(1).trustletId().compareTo(EXPECTED_TLT_ID)==0);
        }
        
        overallsuccess=!overallsuccess?overallsuccess:(stRet.isOk());
        overallsuccess=!overallsuccess?overallsuccess:(test_state.getEnumeratedValue()==EXPECTED_SP_CONT_STATE);
        
        overallsuccess=!overallsuccess?overallsuccess:(strErrRet.isOk());
        overallsuccess=!overallsuccess?overallsuccess:(stErrRet.isOk());
        overallsuccess=!overallsuccess?overallsuccess:(test_structure_2.state()==EXPECTED_SP_CONT_STATE_ERR);
        overallsuccess=!overallsuccess?overallsuccess:(test_state_2.getEnumeratedValue()==EXPECTED_SP_CONT_STATE_ERR);               
        overallsuccess=!overallsuccess?overallsuccess:(strErr2Ret.result()==0x0FFFFFFF); // null pointer exception received
        overallsuccess=!overallsuccess?overallsuccess:(stErr2Ret.result()==0x0FFFFFFF); // null pointer exception received
        overallsuccess=!overallsuccess?overallsuccess:(strErr3Ret.result()==CommandResult.ROOTPA_ERROR_ILLEGAL_ARGUMENT);
        overallsuccess=!overallsuccess?overallsuccess:(stErr3Ret.result()==CommandResult.ROOTPA_ERROR_ILLEGAL_ARGUMENT);
        overallsuccess=!overallsuccess?overallsuccess:(strErr4Ret.result()==CommandResult.ROOTPA_ERROR_REGISTRY);
        
        logi = logi.concat("================= Results of testing container structure and state: ").concat(((overallsuccess==true)?"SUCCESS":"FAILURE")+"\n");            
        logi = logi.concat(((strRet.isOk())?"SUCCESS":"FAILURE")+": Testing getSPContainerStructure "+((strRet.isOk())?"\n":("returned: " +strRet+" \n")));
        logi = logi.concat(((test_structure.state()==EXPECTED_SP_CONT_STATE)?"SUCCESS":"FAILURE")+": Testing getSPContainerStructure state "+
               ((test_structure.state()==EXPECTED_SP_CONT_STATE)?"\n":("returned: " +test_structure.state()+" \n")));
        logi = logi.concat(((trustlets.size()==EXPECTED_NUMBER_OF_TRUSTLETS)?"SUCCESS":"FAILURE")+": Testing getSPContainerStructure nr of trustlets "+
               ((trustlets.size()==EXPECTED_NUMBER_OF_TRUSTLETS)?"\n":("returned: " +trustlets.size()+" \n")));

        if(trustlets.size()==EXPECTED_NUMBER_OF_TRUSTLETS){

            logi = logi.concat(((trustlets.get(0).trustletId().compareTo(EXPECTED_TLT_ID)==0)?"SUCCESS":"FAILURE")+": Testing getSPContainerStructure trustlet id "+
                   ((trustlets.get(0).trustletId().compareTo(EXPECTED_TLT_ID)==0)?"\n":("returned: " +trustlets.get(0).trustletId().toString()+" \n")));

            logi = logi.concat(((trustlets.get(0).state().getEnumeratedValue()==EXPECTED_TLT_CONT_STATE)?"SUCCESS":"FAILURE")+": Testing getSPContainerStructure trustlet state "+
                   ((trustlets.get(0).state().getEnumeratedValue()==EXPECTED_TLT_CONT_STATE)?"\n":("returned: " +trustlets.get(0).state().getEnumeratedValue()+" \n")));
        
            logi = logi.concat(((trustlets.get(1).trustletId().compareTo(EXPECTED_TLT_ID)==0)?"SUCCESS":"FAILURE")+": Testing getSPContainerStructure trustlet id "+
                   ((trustlets.get(1).trustletId().compareTo(EXPECTED_TLT_ID)==0)?"\n":("returned: " +trustlets.get(1).trustletId().toString()+" \n")));

            logi = logi.concat(((trustlets.get(1).state().getEnumeratedValue()==EXPECTED_TLT_CONT_STATE)?"SUCCESS":"FAILURE")+": Testing getSPContainerStructure trustlet state "+
                   ((trustlets.get(1).state().getEnumeratedValue()==EXPECTED_TLT_CONT_STATE)?"\n":("returned: " +trustlets.get(1).state().getEnumeratedValue()+" \n")));
        }

        logi = logi.concat(((stRet.isOk())?"SUCCESS":"FAILURE")+": Testing getSPContainerState "+((stRet.isOk())?"\n":("returned: " +stRet+" \n")));
        logi = logi.concat(((test_state.getEnumeratedValue()==EXPECTED_SP_CONT_STATE)?"SUCCESS":"FAILURE")+": Testing getSPContainerState state "+
               ((test_state.getEnumeratedValue()==EXPECTED_SP_CONT_STATE)?"\n":("returned: " +test_state.getEnumeratedValue()+" \n")));

        
        logi = logi.concat(((strErrRet.isOk())?"SUCCESS":"FAILURE")+": Testing getSPContainerStructure wrong SPID"+
                ((strErrRet.isOk())?"\n":(" returned: " +strErrRet+" \n")));
        logi = logi.concat(((stErrRet.isOk())?"SUCCESS":"FAILURE")+": Testing getSPContainerState wrong SPID"+
                ((stErrRet.isOk())?"\n":(" returned: " +stErrRet+" \n")));
                
        logi = logi.concat(((test_structure.state()==EXPECTED_SP_CONT_STATE)?"SUCCESS":"FAILURE")+": Testing getSPContainerStructure state, nonexitent "+
               ((test_structure_2.state()==EXPECTED_SP_CONT_STATE_ERR)?"\n":("returned: " +test_structure_2.state()+" \n")));

        logi = logi.concat(((test_state_2.getEnumeratedValue()==EXPECTED_SP_CONT_STATE_ERR)?"SUCCESS":"FAILURE")+": Testing getSPContainerState state, nonexistent "+
               ((test_state_2.getEnumeratedValue()==EXPECTED_SP_CONT_STATE_ERR)?"\n":("returned: " +test_state_2.getEnumeratedValue()+" \n")));

        
        logi = logi.concat(((strErr2Ret.result()==0x0FFFFFFF)?"SUCCESS":"FAILURE")+": Testing getSPContainerStructure null data"+
                ((strErr2Ret.result()==0x0FFFFFFF)?"\n":(" returned: " +strErr2Ret+" \n")));
        logi = logi.concat(((stErr2Ret.result()==0x0FFFFFFF)?"SUCCESS":"FAILURE")+": Testing getSPContainerState null data"+
                ((stErr2Ret.result()==0x0FFFFFFF)?"\n":(" returned: " +stErr2Ret+" \n")));  
        logi = logi.concat(((strErr3Ret.result()==CommandResult.ROOTPA_ERROR_ILLEGAL_ARGUMENT)?"SUCCESS":"FAILURE")+": Testing getSPContainerStructure null SPID "+
                ((strErr3Ret.result()==CommandResult.ROOTPA_ERROR_ILLEGAL_ARGUMENT)?"\n":(" returned: " +strErr3Ret+" \n")));
        logi = logi.concat(((stErr3Ret.result()==CommandResult.ROOTPA_ERROR_ILLEGAL_ARGUMENT)?"SUCCESS":"FAILURE")+": Testing getSPContainerState null SPID"+
                ((stErr3Ret.result()==CommandResult.ROOTPA_ERROR_ILLEGAL_ARGUMENT)?"\n":(" returned: " +stErr3Ret+" \n")));

        logi = logi.concat(((strErr4Ret.result()==CommandResult.ROOTPA_ERROR_REGISTRY)?"SUCCESS":"FAILURE")+": Testing getSPContainerStructure, sp cont points to missing tlt cont's "+
                ((strErr4Ret.result()==CommandResult.ROOTPA_ERROR_REGISTRY)?"\n":(" returned: " +strErr4Ret+" \n")));


        logi = logi.concat("==========================================================================\n");

        stringList.add(logi);
        return overallsuccess;
    }

    /**
    This method does not test any additional commands but it tests the ones that were already tested, this time the emphasis being in testing
    MobiCore session.
    */
    private CommandResult sessionTestCommands(List<CmpTest> testCases, boolean ignoreError)
    {
        List<CmpCommand> commands=null;
        List<CmpResponse> responses=null;

        CommandResult res=new CommandResult(0x0FFF0000);
        try{
            commands=new ArrayList<CmpCommand>();
            responses=new ArrayList<CmpResponse>();
            for(int i=0; i<testCases.size(); i++){
                commands.add(testCases.get(i).createCommand());
                commands.get(i).setIgnoreError(ignoreError);
            }
        }catch(java.lang.Exception e){
            Log.e(TAG,"FAILURE: broken test case, initializing data failed: ",e);
        }


        try{
            res=provisioningServiceIfc_.executeCmpCommands(TEST_UID, commands, responses);
        }catch(Throwable e){
            res = new CommandResult(0x0FFFFFFF);
            Log.e(TAG,"FAILURE: sessionTestCommands executeCmpCommands failed: " + e + "\n");
        }           

        for(int i=0; i<testCases.size(); i++){
            try{
                testCases.get(i).checkResult(responses.get(i));
            } catch(java.lang.Exception e){
                testCases.get(i).fail();
                Log.e(TAG,"sessionTestCommands checking results from test "+ testCases.get(i).id() +" failed: "+e);
            }                   
        }
        return res;
    }


    private boolean testSessionHandling(List<String> stringList){
        String logi=new String();
        boolean overallsuccess=true;
        TEST_SPID = new SPID(TEST_SPID_VALUE);

        try{
            provisioningServiceIfc_.acquireLock(TEST_UID); // no checking of return value since we do not test lock here (might help in debugging though)
        }catch(Exception e){
            Log.d(TAG, "acquiring lock failed "+e);
        }

// the authentication is supposed to succeed
    
        List<CmpTest> spAuthTestCases = CmpTest.generateSpAuth();
        CommandResult authRes=new CommandResult(0x0FFF0000);
        authRes=sessionTestCommands(spAuthTestCases, false);

// the following cases are supposed to succeed even when running in different "batch" from the actual authentication          
        Log.d(TAG,"session tests: cmp authenticated");
        List<CmpTest> spTestCases = CmpTest.generateSpCommandsAndAuthTerminate();
        CommandResult spRes=new CommandResult(0x0FFF0000);
        spRes=sessionTestCommands(spTestCases, false);

// the following cases are supposed to fail since we are not authenticated
        
        Log.d(TAG,"session tests: cmp not authenticated");  // this also test running multiple commands when ignoreError is false and one of the tests fail
        List<CmpTest> nonAuthCases = CmpTest.generateSpCommandsAndAuthTerminate();
        CommandResult nonAuthRes=new CommandResult(0x0FFF0000);
        nonAuthRes=sessionTestCommands(nonAuthCases, false);

// releasing lock and acquiring it again (for the case that we may open and close session in relation to the lock)

        try{
            provisioningServiceIfc_.releaseLock(TEST_UID); // no checking of return value since we do not test lock here (might help in debugging though)
        }catch(Exception e){
            Log.d(TAG, "releasing lock failed "+e);
        }

        try{
            provisioningServiceIfc_.acquireLock(TEST_UID); // no checking of return value since we do not test lock here (might help in debugging though)
        }catch(Exception e){
            Log.d(TAG, "acquiring lock failed "+e);
        }

// authenticate again

        List<CmpTest> spAuth2TestCases = CmpTest.generateSpAuth();
        CommandResult auth2Res=new CommandResult(0x0FFF0000);
        auth2Res=sessionTestCommands(spAuth2TestCases, false);

// call do provisioning (should return an error since locked)

        Log.d(TAG,"session tests: doProvisioning should fail");
        boolean failedProvisioningOk = !testDoProvisioning(stringList, true, TEST_SPID);

        try{
            provisioningServiceIfc_.releaseLock(TEST_UID); // no checking of return value since we do not test lock here (might help in debugging though)
        }catch(Exception e){
            Log.d(TAG, "releasing lock failed "+e);
        }

// this is supposed to succeed since own session is created for SE
       Log.d(TAG,"session tests: doProvisioning should succeed");
       List<String> stringListProvisioning = new ArrayList<String>();
       boolean successfullProvisioningOk=testDoProvisioning(stringListProvisioning, true, TEST_SPID);

// the following cases are supposed to fail since we are not authenticated (successful doProvisioning closed the session), 
// unfortunately this will pass also if the execution fails seriously

        try{
            provisioningServiceIfc_.acquireLock(TEST_UID); // no checking of return value since we do not test lock here (might help in debugging though)
        }catch(Exception e){
            Log.d(TAG, "acquiring lock failed "+e);
        }

        Log.d(TAG,"session tests: cmp not auth after doProvisioning");  // this also test running multiple commands when ignoreError is true and one of the tests fail
        List<CmpTest> nonAuth2Cases = CmpTest.generateSpCommandsAndAuthTerminate();
        CommandResult nonAuth2Res=new CommandResult(0x0FFF0000);
        nonAuth2Res=sessionTestCommands(nonAuthCases, true);

        try{
            provisioningServiceIfc_.releaseLock(TEST_UID); // no checking of return value since we do not test lock here (might help in debugging though)
        }catch(Exception e){
            Log.d(TAG, "releasing lock failed "+e);
        }
        
// checking results
        overallsuccess=(overallsuccess==true)?(authRes.isOk()):overallsuccess;
        overallsuccess=(overallsuccess==true)?(spRes.isOk()):overallsuccess;
        overallsuccess=(overallsuccess==true)?(!nonAuthRes.isOk()):overallsuccess;
        overallsuccess=(overallsuccess==true)?(auth2Res.isOk()):overallsuccess;
        overallsuccess=(overallsuccess==true)?failedProvisioningOk:overallsuccess;
        overallsuccess=(overallsuccess==true)?successfullProvisioningOk:overallsuccess;
        overallsuccess=(overallsuccess==true)?(!nonAuth2Res.isOk()):overallsuccess;
        
        logi = logi.concat("================= Results of testing session handling\n");
        logi = logi.concat(((authRes.isOk())?"SUCCESS":"FAILURE")+": sp auth executeCmpCommands returned: "+authRes+"\n");
        logi = logi.concat(((spRes.isOk())?"SUCCESS":"FAILURE")+": sp executeCmpCommands returned: "+spRes+"\n");
        logi = logi.concat(((!nonAuthRes.isOk())?"SUCCESS":"FAILURE")+": sp nonauth executeCmpCommands returned: "+nonAuthRes+"\n");
        logi = logi.concat(((auth2Res.isOk())?"SUCCESS":"FAILURE")+": sp auth 2 executeCmpCommands returned: "+auth2Res+"\n");
        logi = logi.concat((failedProvisioningOk?"SUCCESS":"FAILURE")+": provisioning should fail\n");
        logi = logi.concat((successfullProvisioningOk?"SUCCESS":"FAILURE")+": provisioning should succeed\n");
        String logi2=new String();
        
        logi2 = logi2.concat(((!nonAuth2Res.isOk())?"SUCCESS":"FAILURE")+": sp nonauth 2 executeCmpCommands returned: "+nonAuth2Res+"\n");

        for(int i=0; i<spAuthTestCases.size(); i++){
            logi2 = logi2.concat(((spAuthTestCases.get(i).result_==true)?"SUCCESS":"FAILURE")+": Testing "+spAuthTestCases.get(i).id()+"\n");
            overallsuccess=(overallsuccess==true)?spAuthTestCases.get(i).result_:overallsuccess;
        }

        for(int i=0; i<spTestCases.size(); i++){
            logi2 = logi2.concat(((spTestCases.get(i).result_==true)?"SUCCESS":"FAILURE")+": Testing "+spTestCases.get(i).id()+"\n");
            overallsuccess=(overallsuccess==true)?spTestCases.get(i).result_:overallsuccess;
        }

        // expecting to fail, checking accordingly
        for(int i=0; i<nonAuthCases.size(); i++){
            logi2 = logi2.concat(((nonAuthCases.get(i).result_==false)?"SUCCESS":"FAILURE")+": Testing "+nonAuthCases.get(i).id()+"\n");
            overallsuccess=(overallsuccess==true)?(nonAuthCases.get(i).result_==false):overallsuccess;
        }

        for(int i=0; i<spAuth2TestCases.size(); i++){
            logi2 = logi2.concat(((spAuth2TestCases.get(i).result_==true)?"SUCCESS":"FAILURE")+": Testing "+spAuth2TestCases.get(i).id()+"\n");
            overallsuccess=(overallsuccess==true)?spAuth2TestCases.get(i).result_:overallsuccess;
        }

        // expecting to fail, checking accordingly
        for(int i=0; i<nonAuth2Cases.size(); i++){
            logi2 = logi2.concat(((nonAuth2Cases.get(i).result_==false)?"SUCCESS":"FAILURE")+": Testing "+nonAuth2Cases.get(i).id()+"\n");
            overallsuccess=(overallsuccess==true)?(nonAuth2Cases.get(i).result_==false):overallsuccess;
        }

        logi2 = logi2.concat("==========================================================================\n");

        stringList.add(logi);
        stringList.addAll(stringListProvisioning);
        stringList.add(logi2);        
        return overallsuccess;            
    }
    
    /**
    Test content management protocol commands with error cases
    */
    private boolean testCmpErrorCases(List<String> stringList){
        String logi=new String();
        CommandResult  res=new CommandResult(0x0FFF0000);
        CommandResult  nocontainerres=new CommandResult(0x0FFF0000);
        CommandResult  nostorecontainerres=new CommandResult(0x0FFF0000);
        CommandResult  responseerrorres=new CommandResult(0x0FFF0000);
        CommandResult  nullcres=new CommandResult(0x0FFF0000);
        CommandResult  nullrres=new CommandResult(0x0FFF0000);

        int uid=129;
        List<CmpCommand> commands=null;
        List<CmpResponse> responses=null;

// error commands
        Log.i(TAG,"q: error commands ");
        
        List<CmpTest> cmpTestCases = CmpTest.generateErrorCases();
        try{
            commands=new ArrayList<CmpCommand>();
            responses=new ArrayList<CmpResponse>();
            for(int i=0; i<cmpTestCases.size(); i++){
                commands.add(cmpTestCases.get(i).createCommand());
                commands.get(i).setIgnoreError(true);
            }
        }catch(java.lang.Exception e){
            Log.e(TAG,"FAILURE: broken test case, initializing data failed: ",e);
        }           

        try{
            provisioningServiceIfc_.acquireLock(uid); // no checking of return value since we do not test lock here (might help in debugging though)
            res=provisioningServiceIfc_.executeCmpCommands(uid, commands, responses);
            provisioningServiceIfc_.releaseLock(uid); // no checking of return value since we do not test lock here (might help in debugging though)
        }catch(Throwable e){
            res = new CommandResult(0x0FFFFFFF);
            logi = logi.concat("FAILURE: call to executeCmpCommands failed: " + e + "\n");
        }           

        for(int i=0; i<cmpTestCases.size(); i++){
            try{
                cmpTestCases.get(i).checkResult(responses.get(i));
            } catch(java.lang.Exception e){
                Log.e(TAG,"FAILURE: checking results from test "+ cmpTestCases.get(i).id() +" failed: ",e);
            }                   
        }

// no container 
        Log.i(TAG,"testCmpErrorCases: no container ");        

        List<CmpTest> cmpNoContainerCases = CmpTest.generateNoContainerCases();
        try{
            commands=new ArrayList<CmpCommand>();
            responses=new ArrayList<CmpResponse>();
            for(int i=0; i<cmpNoContainerCases.size(); i++){
                commands.add(cmpNoContainerCases.get(i).createCommand());
                commands.get(i).setIgnoreError(true);
            }
        }catch(java.lang.Exception e){
            Log.e(TAG,"FAILURE: broken no container test case, initializing data failed: ",e);
        }           

        try{
            provisioningServiceIfc_.acquireLock(uid); // no checking of return value since we do not test lock here (might help in debugging though)
            nocontainerres=provisioningServiceIfc_.executeCmpCommands(uid, commands, responses);
            provisioningServiceIfc_.releaseLock(uid); // no checking of return value since we do not test lock here (might help in debugging though)
        }catch(Throwable e){
            nocontainerres = new CommandResult(0x0FFFFFFF);
            logi = logi.concat("FAILURE: call to executeCmpCommands failed: " + e + "\n");
        }           


        for(int i=0; i<cmpNoContainerCases.size(); i++){
            try{
                cmpNoContainerCases.get(i).checkResult(responses.get(i));
            } catch(java.lang.Exception e){
                Log.e(TAG,"FAILURE: checking results from test "+ cmpNoContainerCases.get(i).id() +" failed: ",e);
            }                   
        }
// can not store container generateNoStoreContainerCases        

        Log.i(TAG,"testCmpErrorCases: can not store container ");        

        List<CmpTest> cmpNoStoreContainerCases = CmpTest.generateNoStoreContainerCases();
        try{
            commands=new ArrayList<CmpCommand>();
            responses=new ArrayList<CmpResponse>();
            for(int i=0; i<cmpNoStoreContainerCases.size(); i++){
                commands.add(cmpNoStoreContainerCases.get(i).createCommand());
                commands.get(i).setIgnoreError(true);
            }
        }catch(java.lang.Exception e){
            Log.e(TAG,"FAILURE: broken no store container test case, initializing data failed: ",e);
        }           

        try{
            provisioningServiceIfc_.acquireLock(uid); // no checking of return value since we do not test lock here (might help in debugging though)
            nostorecontainerres=provisioningServiceIfc_.executeCmpCommands(uid, commands, responses);
            provisioningServiceIfc_.releaseLock(uid); // no checking of return value since we do not test lock here (might help in debugging though)
        }catch(Throwable e){
            nostorecontainerres = new CommandResult(0x0FFFFFFF);
            logi = logi.concat("FAILURE: call to executeCmpCommands failed: " + e + "\n");
        }           


        for(int i=0; i<cmpNoStoreContainerCases.size(); i++){
            try{
                cmpNoStoreContainerCases.get(i).checkResult(responses.get(i));
            } catch(java.lang.Exception e){
                Log.e(TAG,"FAILURE: checking results from test "+ cmpNoStoreContainerCases.get(i).id() +" failed: ",e);
            }                   
        }

// unknown response, offsets wrong
        
        
        Log.i(TAG,"testCmpErrorCases: problems in response id's or offsets ");        

        List<CmpTest> cmpResponseErrorCases = CmpTest.generateResponseErrorCases();
        try{
            commands=new ArrayList<CmpCommand>();
            responses=new ArrayList<CmpResponse>();
            for(int i=0; i<cmpResponseErrorCases.size(); i++){
                commands.add(cmpResponseErrorCases.get(i).createCommand());
                commands.get(i).setIgnoreError(true);
            }
        }catch(java.lang.Exception e){
            Log.e(TAG,"FAILURE: broken response test case, initializing data failed: ",e);
        }           

        try{
            provisioningServiceIfc_.acquireLock(uid); // no checking of return value since we do not test lock here (might help in debugging though)
            responseerrorres=provisioningServiceIfc_.executeCmpCommands(uid, commands, responses);
            provisioningServiceIfc_.releaseLock(uid); // no checking of return value since we do not test lock here (might help in debugging though)
        }catch(Throwable e){
            responseerrorres = new CommandResult(0x0FFFFFFF);
            logi = logi.concat("FAILURE: call to executeCmpCommands failed: " + e + "\n");
        }           

        for(int i=0; i<cmpResponseErrorCases.size(); i++){
            try{
                cmpResponseErrorCases.get(i).checkResult(responses.get(i));
            } catch(java.lang.Exception e){
                Log.e(TAG,"FAILURE: checking results from test "+ cmpResponseErrorCases.get(i).id() +" failed: ",e);
            }                   
        }
        
        
// null parameters
        Log.i(TAG,"testCmpErrorCases: null params ");        

        try{
            provisioningServiceIfc_.acquireLock(uid); // no checking of return value since we do not test lock here (might help in debugging though)
            nullcres=provisioningServiceIfc_.executeCmpCommands(uid, null, responses);
            provisioningServiceIfc_.releaseLock(uid); // no checking of return value since we do not test lock here (might help in debugging though)
        }catch(Throwable e){
            nullcres = new CommandResult(0x0FFFFFFF);
            logi = logi.concat("FAILURE: call to executeCmpCommands with null commands failed: " + e + "\n");
        }           


        try{
            provisioningServiceIfc_.acquireLock(uid); // no checking of return value since we do not test lock here (might help in debugging though)
            nullrres=provisioningServiceIfc_.executeCmpCommands(uid, commands, null);
        }catch(java.lang.NullPointerException s){
            Log.d(TAG,"call call to executeCmpCommands with null responses resulted to null pointer exception as expected\n");
            nullrres = new CommandResult(0x0FFFFFF0);
            try{
                provisioningServiceIfc_.releaseLock(uid); // no checking of return value since we do not test lock here (might help in debugging though)
            }catch (Throwable le){
                Log.e(TAG, "releasing lock failed "+le);
            }
        }catch(Throwable e){
            nullrres = new CommandResult(0x0FFFFFFF);
            logi = logi.concat("FAILURE: call to executeCmpCommands with null responses failed: " + e + "\n");
        }           

// check results
        
        boolean success=true;
        boolean overallsuccess=success;
        
        overallsuccess=!overallsuccess?overallsuccess:(res.result()==CommandResult.ROOTPA_COMMAND_NOT_SUPPORTED);
        overallsuccess=!overallsuccess?overallsuccess:(nocontainerres.result()==CommandResult.ROOTPA_ERROR_REGISTRY_OBJECT_NOT_AVAILABLE);
        overallsuccess=!overallsuccess?overallsuccess:(nostorecontainerres.result()==CommandResult.ROOTPA_ERROR_REGISTRY);
        overallsuccess=!overallsuccess?overallsuccess:(responseerrorres.result()==CommandResult.ROOTPA_ERROR_INTERNAL);
        overallsuccess=!overallsuccess?overallsuccess:(nullcres.result()==CommandResult.ROOTPA_ERROR_ILLEGAL_ARGUMENT);
        overallsuccess=!overallsuccess?overallsuccess:(nullrres.result()==0x0FFFFFF0);
        
        logi = logi.concat("================= Results of testing CMP command error cases \n");
        logi = logi.concat(((res.result()==CommandResult.ROOTPA_COMMAND_NOT_SUPPORTED)?"SUCCESS":"FAILURE")+": executeCmpCommands (error cases) returned: "+res+"\n");
        for(int i=0; i<cmpTestCases.size(); i++){
            logi = logi.concat(((cmpTestCases.get(i).result_==true)?"SUCCESS":"FAILURE")+": Testing unknown command and wrong command length, command "+cmpTestCases.get(i).id()+"\n");
            overallsuccess=(overallsuccess==true)?cmpTestCases.get(i).result_:overallsuccess;
        }
        
        logi = logi.concat(((nostorecontainerres.result()==CommandResult.ROOTPA_ERROR_REGISTRY)?"SUCCESS":"FAILURE")+": executeCmpCommands (cat not  store container) returned: "+nostorecontainerres+"\n");
        for(int i=0; i<cmpNoStoreContainerCases.size(); i++){
            logi = logi.concat(((cmpNoStoreContainerCases.get(i).result_==true)?"SUCCESS":"FAILURE")+": Testing can not store container handling, command "+cmpNoStoreContainerCases.get(i).id()+"\n");
            overallsuccess=(overallsuccess==true)?cmpNoStoreContainerCases.get(i).result_:overallsuccess;
        }

        logi = logi.concat(((nocontainerres.result()==CommandResult.ROOTPA_ERROR_REGISTRY_OBJECT_NOT_AVAILABLE)?"SUCCESS":"FAILURE")+": executeCmpCommands (no container) returned: "+nocontainerres+"\n");
        for(int i=0; i<cmpNoContainerCases.size(); i++){
            logi = logi.concat(((cmpNoContainerCases.get(i).result_==true)?"SUCCESS":"FAILURE")+": Testing no container handling, command "+cmpNoContainerCases.get(i).id()+"\n");
            overallsuccess=(overallsuccess==true)?cmpNoContainerCases.get(i).result_:overallsuccess;
        }

        logi = logi.concat(((responseerrorres.result()==CommandResult.ROOTPA_ERROR_INTERNAL)?"SUCCESS":"FAILURE")+": executeCmpCommands (response errors) returned: "+responseerrorres+"\n");
        for(int i=0; i<cmpResponseErrorCases.size(); i++){
            logi = logi.concat(((cmpResponseErrorCases.get(i).result_==true)?"SUCCESS":"FAILURE")+": Testing response errors, command "+cmpResponseErrorCases.get(i).id()+"\n");
            overallsuccess=(overallsuccess==true)?cmpResponseErrorCases.get(i).result_:overallsuccess;
        }
        
        logi = logi.concat(((nullcres.result()==CommandResult.ROOTPA_ERROR_ILLEGAL_ARGUMENT)?"SUCCESS":"FAILURE")+": Testing executeCmpCommands with null command "+
                ((nullcres.result()==CommandResult.ROOTPA_ERROR_ILLEGAL_ARGUMENT)?"\n":("returned "+nullcres+"\n")));
        logi = logi.concat(((nullrres.result()==0x0FFFFFF0)?"SUCCESS":"FAILURE")+": Testing executeCmpCommands with null response "+
                ((nullrres.result()==0x0FFFFFF0)?"\n":("returned "+nullrres+"\n")));
        logi = logi.concat("==========================================================================\n");
        stringList.add(logi);
        return overallsuccess;
    }
    
    void disconnect(){
        provisioningServiceIfc_=null; 
    }
}
