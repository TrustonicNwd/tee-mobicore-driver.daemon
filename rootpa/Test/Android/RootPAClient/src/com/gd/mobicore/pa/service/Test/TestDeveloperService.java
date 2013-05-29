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
import android.content.Intent;
import android.os.IBinder;
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

import com.gd.mobicore.pa.ifc.RootPAProvisioningIntents;
import com.gd.mobicore.pa.ifc.RootPADeveloperIfc;
import com.gd.mobicore.pa.ifc.CommandResult;

class TestDeveloperService extends AsyncTask<IBinder, Void, Void> {
    RootPADeveloperIfc developerServiceIfc_=null;
    private static final String TESTTAG = "RootPA-Test";
    private static final String TAG = "RootPA-T";
    private byte[] pukHash_={1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2};
    
    RootPAClient parent_=null;
    TestDeveloperService(RootPAClient parent){
        super();
        parent_=parent;
    }

    protected Void doInBackground(IBinder... s) {
        IBinder service=s[0];
        boolean res=true;
        developerServiceIfc_= RootPADeveloperIfc.Stub.asInterface(service);
        if(developerServiceIfc_ == null){
            Log.e(TAG,"FAILURE: no developerServiceIfc_, why?");
        }

        try{
            Log.d(TAG, "testInstallTrustlet");
            res=testInstallTrustlet(parent_.stringList_);
        }catch(Throwable e){
            Log.e(TAG, "Executing testInstallTrustlet failed due to exception "+e);
            res=false;
        }
        parent_.allTestsPassed_=(parent_.allTestsPassed_==false)?parent_.allTestsPassed_:res;
        Log.i(TESTTAG,((res==true)?"SUCCESS":"FAILURE")+": Test installTrustlet");
        
        parent_.printFinalResults();
        return null;
    }
    private final static int TLT_INSTALL_TEST_SPID=10;
    private final byte[] TEST_TRUSTLET={
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  // 200
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  // 400
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  // 600
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  // 800
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  // 1000
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  // 1200
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  // 1400
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  // 1600
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  // 1800
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  // 2000
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1  // 2200
        };
        
    private final byte[] TEST_KEY={
        1,1,1,1,1,1,1,1,1,1,            
        2,2,2,2,2,2,2,2,2,2,
        3,3,3,3,3,3,3,3,3,3,
        4,4
        };

    byte[] trustlet_;
    boolean done_=false;
        
    boolean testInstallTrustlet(List<String> stringList){
        String logi=new String();            
        boolean overallsuccess=true;
        CommandResult tret=new CommandResult(0x0FFF0000);
        CommandResult kret=new CommandResult(0x0FFF0000);
        
    // set receiver for intents
    
        android.os.HandlerThread handlerThread = new android.os.HandlerThread("tltIntentReceiverThread");

        handlerThread.start();
        android.os.Looper looper = handlerThread.getLooper();
        android.os.Handler handler= new android.os.Handler(looper);
        BroadcastReceiver receiver=new BroadcastReceiver(){
            public void onReceive(Context ctx, Intent intent){
                trustlet_ = intent.getByteArrayExtra(RootPAProvisioningIntents.TRUSTLET);
                Log.d(TAG, "Received install tlt intent");

            }
        };
        IntentFilter filter=new IntentFilter(RootPAProvisioningIntents.INSTALL_TRUSTLET);
        parent_.registerReceiver(receiver, filter, null, handler);
      
        BroadcastReceiver statusreceiver=new BroadcastReceiver(){
            public void onReceive(Context ctx, Intent intent){
                int status = intent.getIntExtra(RootPAProvisioningIntents.STATE, 0);
                Log.d(TAG, "Received status intent for install trustlet: "+status);
                if(status==RootPAProvisioningIntents.FINISHED_PROVISIONING){
                    done_=true;
                }                        
            }
        };
        IntentFilter statusfilter=new IntentFilter(RootPAProvisioningIntents.PROVISIONING_PROGRESS_UPDATE);
        parent_.registerReceiver(statusreceiver, statusfilter, null, handler);

    // installTrustlet, trustlet binary
        
        try{
            tret=developerServiceIfc_.installTrustlet(TLT_INSTALL_TEST_SPID, CmpTest.TLTUUID, TEST_TRUSTLET, 1, pukHash_, 1, 3, 4);
        }catch(Throwable e){
            logi = logi.concat("FAILURE: call to installTrustlet with trustlet failed: " + e + "\n");
            overallsuccess=false;
        }            

        if(tret.isOk()){
            try {
                int totalTime=0;
                while(done_==false){
                    Thread.sleep(500);
                    totalTime+=500;
                    if(totalTime>6000){
                        break;
                    }
                }
                Log.d(TAG, "totalTime: "+totalTime);
                Thread.sleep(1000); // waiting for one more second to ensure the locks are opened for the following tests    
            } catch (Exception e) {
                Log.d(TAG, "sleep failed "+e);
            }
        }
        else
        {
            Log.d(TAG, "error when calling installTrustlet for tlt"+tret);                
        }

    // installTrustlet, key
        
        done_=false; // another call
        try{
            kret=developerServiceIfc_.installTrustletOrKey(TLT_INSTALL_TEST_SPID, CmpTest.TLTUUID, null, TEST_KEY, 1, pukHash_);
        }catch(Throwable e){
            logi = logi.concat("FAILURE: call to installTrustlet with key failed: " + e + "\n");
            overallsuccess=false;
        }

        if(kret.isOk()){
            try {
                int totalTime=0;
                while(done_==false){
                    Thread.sleep(500);
                    totalTime+=500;
                    if(totalTime>6000){
                        break;
                    }
                }
                Log.d(TAG, "totalTime: "+totalTime);
                Thread.sleep(1000); // waiting for one more second to ensure the locks are opened for the following tests    
            } catch (Exception e) {
                Log.d(TAG, "sleep failed "+e);
            }
        }
        else
        {
            Log.d(TAG, "error when calling installTrustlet for key "+kret);                
        }            
        
        parent_.unregisterReceiver(receiver);
        parent_.unregisterReceiver(statusreceiver);
        handlerThread.quit();            
        
        if(trustlet_!=null){
            boolean trustletStatus=Arrays.equals(trustlet_,TEST_TRUSTLET);
            Log.d(TAG,"trustletStatus "+trustletStatus+" "+trustlet_.length+" "+TEST_TRUSTLET.length);
            overallsuccess=!overallsuccess?overallsuccess:trustletStatus;
        }else{
            overallsuccess=false;
        }
        
    // error cases
        CommandResult err1ret=new CommandResult(0x0FFF0000);
        try{
            err1ret=developerServiceIfc_.installTrustletOrKey(TLT_INSTALL_TEST_SPID, CmpTest.TLTUUID, TEST_TRUSTLET, TEST_KEY, 1, pukHash_);
        }catch(Throwable e){
            logi = logi.concat("FAILURE: call to installTrustlet with trustlet failed: " + e + "\n");
            overallsuccess=false;
        }        

        CommandResult err2ret=new CommandResult(0x0FFF0000);
        
        try{
            err2ret=developerServiceIfc_.installTrustletOrKey(TLT_INSTALL_TEST_SPID, CmpTest.TLTUUID, null, null, 1, pukHash_);
        }catch(Throwable e){
            logi = logi.concat("FAILURE: call to installTrustlet with trustlet failed: " + e + "\n");
            overallsuccess=false;
        }        

// results
        
        overallsuccess=!overallsuccess?overallsuccess:(tret.isOk());
        overallsuccess=!overallsuccess?overallsuccess:(kret.isOk());
        overallsuccess=!overallsuccess?overallsuccess:(err1ret.result()==CommandResult.ROOTPA_ERROR_ILLEGAL_ARGUMENT);
        overallsuccess=!overallsuccess?overallsuccess:(err2ret.result()==CommandResult.ROOTPA_ERROR_ILLEGAL_ARGUMENT);
        
        logi = logi.concat("================= Results of testing installTrustlet: ").concat(((overallsuccess==true)?"SUCCESS":"FAILURE")+"\n");            
        logi = logi.concat(((tret.isOk())?"SUCCESS":"FAILURE")+": Testing installTrustlet, tlt "+((tret.isOk())?"\n":("returned: " +tret+" \n")));
        logi = logi.concat(((trustlet_!=null && Arrays.equals(trustlet_,TEST_TRUSTLET))?"SUCCESS":"FAILURE")+": Testing installTrustlet, tlt content in the intent");
        
        if(trustlet_==null) logi = logi.concat(", trustlet_ is null\n");
        else if(!Arrays.equals(trustlet_,TEST_TRUSTLET)) logi = logi.concat(", received: \n"+CmpTest.byteArrayToDisplayable(trustlet_)+" \n");
        else logi = logi.concat("\n");
        
        logi = logi.concat(((kret.isOk())?"SUCCESS":"FAILURE")+": Testing installTrustlet, key "+((kret.isOk())?"\n":("returned: " +kret+" \n")));
        logi = logi.concat(((err1ret.result()==CommandResult.ROOTPA_ERROR_ILLEGAL_ARGUMENT)?"SUCCESS":"FAILURE")+": Testing installTrustlet, both arguments given"+((err1ret.result()==CommandResult.ROOTPA_ERROR_ILLEGAL_ARGUMENT)?"\n":(", returned: " +err1ret+" \n")));
        logi = logi.concat(((err2ret.result()==CommandResult.ROOTPA_ERROR_ILLEGAL_ARGUMENT)?"SUCCESS":"FAILURE")+": Testing installTrustlet, null arguments given"+((err2ret.result()==CommandResult.ROOTPA_ERROR_ILLEGAL_ARGUMENT)?"\n":(", returned: " +err2ret+" \n")));

        logi = logi.concat("==========================================================================\n");

        stringList.add(logi);
        return overallsuccess;            
    }
    
    void disconnect(){
        developerServiceIfc_=null;    
    }
}