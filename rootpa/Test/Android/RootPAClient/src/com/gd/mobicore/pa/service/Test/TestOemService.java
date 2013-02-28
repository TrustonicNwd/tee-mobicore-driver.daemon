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

import com.gd.mobicore.pa.ifc.RootPAProvisioningIntents;
import com.gd.mobicore.pa.ifc.RootPAOemIfc;
import com.gd.mobicore.pa.ifc.CommandResult;

class TestOemService extends AsyncTask<IBinder, Void, Void> {
    private static final String TESTTAG = "RootPA-Test";
    private static final String TAG = "RootPA-T";
    
    RootPAClient parent_=null;
    TestOemService(RootPAClient parent){
        super();
        parent_=parent;
    }

    protected Void doInBackground(IBinder... s) {
        IBinder service=s[0];
        boolean res=true;
        oemServiceIfc_= RootPAOemIfc.Stub.asInterface(service);
        if(oemServiceIfc_ == null){
            Log.e(TAG,"FAILURE: no oemServiceIfc_, why?");
        }

        try{
            Log.d(TAG, "testUnregisterRootContainer");
            res=testUnregisterRootContainer(parent_.stringList_, true);
        }catch(Throwable e){
            Log.e(TAG, "Executing testUnregisterRootContainer failed due to exception "+e);
            res=false;
        }
        parent_.allTestsPassed_=(parent_.allTestsPassed_==false)?parent_.allTestsPassed_:res;
        Log.i(TESTTAG,((res==true)?"SUCCESS":"FAILURE")+": Test testUnregisterRootContainer");

        parent_.doUnbindService();
        parent_.bindServiceWrongIp(oemServiceWrongIpConnection_);
        return null;
    }

    
    private ServiceConnection oemServiceWrongIpConnection_ = new ServiceConnection(){

        public void onServiceDisconnected(ComponentName className){
            oemServiceIfc_=null;
        }

        public void onServiceConnected(ComponentName className, IBinder service){
            boolean res=true;
            oemServiceIfc_= RootPAOemIfc.Stub.asInterface(service);
            try{
                Log.d(TAG, "testUnregisterRootContainer, no SE");
                res=testUnregisterRootContainer(parent_.stringList_, false);
            }catch(Throwable e){
                Log.e(TAG, "Executing testUnregisterRootContainer, no SE failed due to exception "+e);
                res=false;
            }
            parent_.allTestsPassed_=(parent_.allTestsPassed_==false)?parent_.allTestsPassed_:res;
            Log.i(TESTTAG,((res==true)?"SUCCESS":"FAILURE")+": Test testUnregisterRootContainer, no SE");

            parent_.unbindServiceWrongIp(oemServiceWrongIpConnection_);
            parent_.printFinalResults();            
        }    
    };
    
    
    private RootPAOemIfc oemServiceIfc_=null;    
    private boolean unregisterReceived_=false;
    private int unregisterError_=CommandResult.ROOTPA_OK;
    private boolean done_=false;		
    
    boolean testUnregisterRootContainer(List<String> stringList, boolean seIpExpectedToBeToBeOk){
        String logi=new String();            
        boolean overallsuccess=true;
        unregisterReceived_=false;
        unregisterError_=CommandResult.ROOTPA_OK;
        done_=false;		
        
        CommandResult ret=new CommandResult(0x0FFF0000);            

    // set receiver for intents
    
        android.os.HandlerThread handlerThread = new android.os.HandlerThread("intentReceiverThread");
        handlerThread.start();
        android.os.Looper looper = handlerThread.getLooper();
        android.os.Handler handler= new android.os.Handler(looper);

        BroadcastReceiver errorReceiver=new BroadcastReceiver(){
            public void onReceive(Context ctx, Intent intent){
                unregisterError_=intent.getIntExtra(RootPAProvisioningIntents.ERROR, 0);
                Log.d(TAG, "OEM: Received error intent: "+unregisterError_);
            }            
        };
        IntentFilter errorFilter=new IntentFilter(RootPAProvisioningIntents.PROVISIONING_ERROR);
        parent_.registerReceiver(errorReceiver, errorFilter, null, handler);
        
        BroadcastReceiver statusreceiver=new BroadcastReceiver(){
            public void onReceive(Context ctx, Intent intent){
                int status = intent.getIntExtra(RootPAProvisioningIntents.STATE, 0);
                Log.d(TAG, "Received status intent for unregister root: "+status);
                if(status==RootPAProvisioningIntents.FINISHED_PROVISIONING){
                    done_=true;
                }else if (status==RootPAProvisioningIntents.UNREGISTERING_ROOT_CONTAINER){
                    unregisterReceived_=true;
                }
            }
        };
        IntentFilter statusfilter=new IntentFilter(RootPAProvisioningIntents.PROVISIONING_PROGRESS_UPDATE);
        parent_.registerReceiver(statusreceiver, statusfilter, null, handler);
        
        try{
            ret=oemServiceIfc_.unregisterRootContainer();
        }catch(Throwable e){
            logi = logi.concat("FAILURE: call to unregisterRootContainer failed: " + e + "\n");
            overallsuccess=false;
        }

        if(ret.isOk()){
            try {
                int totalTime=0;
                while(done_==false){

                    Thread.sleep(500);
                    totalTime+=500;
                    if(seIpExpectedToBeToBeOk){
                        if(totalTime>6000){
                            break;
                        }
                    }else{
                        if(totalTime>7000){ // testing connection to libcurl timeout
                            break;
                        }
                    }
                }
                Log.d(TAG, "totalTime: "+totalTime);
//                    Thread.sleep(1000); // not waiting for one more second since this is the last test, uncomment if more tests added after this
            } catch (Exception e) {
                Log.d(TAG, "sleep failed "+e);
            }
        }
        else
        {
            Log.d(TAG, "error when calling unregisterRootContainer "+ret);                
        }

        parent_.unregisterReceiver(errorReceiver);
        parent_.unregisterReceiver(statusreceiver);
        handlerThread.quit();            

        if(seIpExpectedToBeToBeOk)
        {
            overallsuccess=!overallsuccess?overallsuccess:(ret.isOk());
            overallsuccess=!overallsuccess?overallsuccess:(unregisterReceived_==true);
            overallsuccess=!overallsuccess?overallsuccess:(unregisterError_==CommandResult.ROOTPA_OK);

            logi = logi.concat("================= Results of testing unregisterRootContainer: ").concat(((overallsuccess==true)?"SUCCESS":"FAILURE")+"\n");
            logi = logi.concat(((ret.isOk())?"SUCCESS":"FAILURE")+": Testing unregisterRootContainer, tlt "+((ret.isOk())?"\n":("returned: " +ret+" \n")));
            logi = logi.concat(((unregisterReceived_==true)?"SUCCESS":"FAILURE")+": Testing unregisterRootContainer, finished intent receiving\n");
            logi = logi.concat(((unregisterError_==CommandResult.ROOTPA_OK)?"SUCCESS":"FAILURE")+": Testing unregisterRootContainer error check"+((unregisterError_==CommandResult.ROOTPA_OK)?"\n":(" error intent received: " +new CommandResult(unregisterError_)+" \n")));
            logi = logi.concat("==========================================================================\n");
        }
        else
        {
            overallsuccess=!overallsuccess?overallsuccess:(ret.isOk());
            overallsuccess=!overallsuccess?overallsuccess:(unregisterReceived_==false);
            overallsuccess=!overallsuccess?overallsuccess:(unregisterError_==CommandResult.ROOTPA_ERROR_NETWORK);

            logi = logi.concat("================= Results of testing unregisterRootContainer when SE is not available: ").concat(((overallsuccess==true)?"SUCCESS":"FAILURE")+"\n");
            logi = logi.concat(((ret.isOk())?"SUCCESS":"FAILURE")+": Testing unregisterRootContainer, no SE "+((ret.isOk())?"\n":("returned: " +ret+" \n")));
            logi = logi.concat(((unregisterReceived_==false)?"SUCCESS":"FAILURE")+": Testing unregisterRootContainer, no SE, finished intent receiving\n");
            logi = logi.concat(((unregisterError_==CommandResult.ROOTPA_ERROR_NETWORK)?"SUCCESS":"FAILURE")+": Testing unregisterRootContainer, no SE, error check"+((unregisterError_==CommandResult.ROOTPA_ERROR_NETWORK)?"\n":(" error intent received: " +new CommandResult(unregisterError_)+" \n")));
            logi = logi.concat("==========================================================================\n");            
        }
        stringList.add(logi);
        return overallsuccess;            
    }
    
    void disconnect(){
        oemServiceIfc_=null; 
    }
}