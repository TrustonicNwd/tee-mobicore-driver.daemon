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
import android.content.IntentFilter;
import android.content.BroadcastReceiver;
import android.os.IBinder;
import android.content.ServiceConnection;
import android.content.ComponentName;
import android.content.Context;
import android.util.Log;
import android.os.AsyncTask;

// import android.os.RemoteException;

import java.util.List;
import java.util.ArrayList;
import java.util.Arrays;
import java.lang.Integer;
import java.util.UUID;
       
import com.gd.mobicore.pa.ifc.RootPAProvisioningIntents;
import com.gd.mobicore.pa.ifc.RootPAServiceIfc;
import com.gd.mobicore.pa.ifc.RootPADeveloperIfc;
import com.gd.mobicore.pa.ifc.RootPAOemIfc;
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

public class RootPAClient extends Activity {

    RootPAServiceIfc provisioningServiceIfc_=null; 
    boolean provisioningServiceIsBound;

    RootPADeveloperIfc developerServiceIfc_=null;
    boolean developerServiceIsBound;    

    RootPAOemIfc oemServiceIfc_=null;
    boolean oemServiceIsBound;    

    List<String> stringList_=new ArrayList<String>();
    boolean allTestsPassed_ = true;
    
    private static final String TESTTAG = "RootPA-Test";
    private static final String TAG = "RootPA-J";

    private int callCounter_=0;
    private void printFinalResults()
    {
        if(++callCounter_==3){ // number of services

            Log.i(TESTTAG,"==========================================================================");            
            Log.i(TESTTAG,"================= Execution of tests "+(allTestsPassed_?"PASSED":"FAILED"));

            if(!allTestsPassed_){
                Log.i(TESTTAG,"================= More detailed results:");            
                Log.i(TESTTAG,"==========================================================================");
                for(int i=0; i<stringList_.size();i++){
                    Log.i(TESTTAG,stringList_.get(i));
                }
            }else{
                Log.i(TESTTAG,"==========================================================================");
            }        
        }
    }
        
    private ServiceConnection developerServiceConnection = new ServiceConnection(){
        public void onServiceDisconnected(ComponentName className){
            developerServiceIfc_=null;
        }
        public void onServiceConnected(ComponentName className, IBinder service){
            new TestDeveloperService().execute(service);
        }
    };

   
    private ServiceConnection oemServiceConnection = new ServiceConnection(){
        public void onServiceDisconnected(ComponentName className){
            oemServiceIfc_=null; 
        }
        public void onServiceConnected(ComponentName className, IBinder service){
            new TestOemService().execute(service);
        }
    };


    private ServiceConnection provisioningServiceConnection = new ServiceConnection(){
        public void onServiceDisconnected(ComponentName className){
            provisioningServiceIfc_=null; 
        }

        public void onServiceConnected(ComponentName className, IBinder service){
            new TestProvisioningService().execute(service);
        }
    };

    void doBindService(){       
        
        byte[] address="http://10.0.2.2:80/".getBytes(); // using local apache2 server
        Intent psintent=new Intent("com.gd.mobicore.pa.service.PROVISIONING_SERVICE");
        psintent.putExtra("SE", address);
        bindService(psintent, provisioningServiceConnection, Context.BIND_AUTO_CREATE);
        provisioningServiceIsBound=true;
        
        Intent dsintent=new Intent("com.gd.mobicore.pa.service.DEVELOPER_SERVICE");
        dsintent.putExtra("SE", address);
        bindService(dsintent, developerServiceConnection, Context.BIND_AUTO_CREATE);       
        developerServiceIsBound=true;

        Intent osintent=new Intent("com.gd.mobicore.pa.service.OEM_SERVICE");
        osintent.putExtra("SE", address);
        bindService(osintent, oemServiceConnection, Context.BIND_AUTO_CREATE);       
        oemServiceIsBound=true;

        Log.d(TAG,"===Binding done");
    }

    void doUnbindService(){
        Log.d(TAG,"===Unbinding");
        if(provisioningServiceIsBound){
            unbindService(provisioningServiceConnection);
            provisioningServiceIsBound=false;
        }

        if(developerServiceIsBound){
            unbindService(developerServiceConnection);
            developerServiceIsBound=false;
        }

        if(oemServiceIsBound){
            unbindService(oemServiceConnection);
            oemServiceIsBound=false;
        }
    }
   
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.i(TAG,"RootPAClient says hello!");
        doBindService();
    }
    
    public void onDestroy(){       
        Log.i(TAG,"RootPAClient onDestroy!");
        doUnbindService();
        System.exit(0);    
    }
    
    private class TestDeveloperService extends AsyncTask<IBinder, Void, Void> {
        protected Void doInBackground(IBinder... s) {
            IBinder service=s[0];
            boolean res=true;
            developerServiceIfc_= RootPADeveloperIfc.Stub.asInterface(service);
            if(developerServiceIfc_ == null){
                Log.e(TAG,"FAILURE: no developerServiceIfc_, why?");
            }

            try{
                Log.d(TAG, "testInstallTrustlet");
                res=testInstallTrustlet(stringList_);
            }catch(Throwable e){
                Log.e(TAG, "Executing testInstallTrustlet failed due to exception "+e);
                res=false;
            }
            allTestsPassed_=(allTestsPassed_==false)?allTestsPassed_:res;
            Log.i(TESTTAG,((res==true)?"SUCCESS":"FAILURE")+": Test installTrustlet");
            
            printFinalResults();
            return null;
        }

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
            CommandResult tret=new CommandResult(0xFFFF0000);
            CommandResult kret=new CommandResult(0xFFFF0000);
            
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
            registerReceiver(receiver, filter, null, handler);
          
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
            registerReceiver(statusreceiver, statusfilter, null, handler);

            
            try{
                tret=developerServiceIfc_.installTrustlet(TEST_TRUSTLET, null);
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
                        if(totalTime>3000){
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

            done_=false; // another call
            try{
                kret=developerServiceIfc_.installTrustlet(null, TEST_KEY);
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
                        if(totalTime>3000){
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
            
            unregisterReceiver(receiver);
            unregisterReceiver(statusreceiver);
            handlerThread.quit();            
            
            if(trustlet_!=null){
                boolean trustletStatus=Arrays.equals(trustlet_,TEST_TRUSTLET);
                Log.d(TAG,"trustletStatus "+trustletStatus+" "+trustlet_.length+" "+TEST_TRUSTLET.length);
                overallsuccess=!overallsuccess?overallsuccess:trustletStatus;
            }else{
                overallsuccess=false;
            }
            overallsuccess=!overallsuccess?overallsuccess:(tret.isOk());
            overallsuccess=!overallsuccess?overallsuccess:(kret.isOk());

            logi = logi.concat("================= Results of testing installTrustlet: ").concat(((overallsuccess==true)?"SUCCESS":"FAILURE")+"\n");            
            logi = logi.concat(((tret.isOk())?"SUCCESS":"FAILURE")+": Testing installTrustlet, tlt "+((tret.isOk())?"\n":("returned: " +tret+" \n")));
            logi = logi.concat(((trustlet_!=null && Arrays.equals(trustlet_,TEST_TRUSTLET))?"SUCCESS":"FAILURE")+": Testing installTrustlet, tlt content in the intent\n");
            logi = logi.concat(((kret.isOk())?"SUCCESS":"FAILURE")+": Testing installTrustlet, key "+((kret.isOk())?"\n":("returned: " +kret+" \n")));
            logi = logi.concat("==========================================================================\n");

            stringList.add(logi);
            return overallsuccess;            
        }
    }

    private class TestProvisioningService extends AsyncTask<IBinder, Void, Void> {
        protected Void doInBackground(IBinder... service) {
            provisioningServiceIfc_= RootPAServiceIfc.Stub.asInterface(service[0]);
            if(provisioningServiceIfc_ == null){
                Log.e(TAG,"FAILURE: no provisioningServiceIfc_, why?");            
            }

            String s=null;
            boolean res=true;
        
            try{
                Log.d(TAG, "testCmpCommands");
                res=testCmpCommands(stringList_);
            }catch(Throwable e){
                Log.e(TAG, "Executing testCmpCommands failed due to exception "+e);
                res=false;
            }

            allTestsPassed_=(allTestsPassed_==false)?allTestsPassed_:res;
            Log.i(TESTTAG,"=========================================================================="); 
            Log.i(TESTTAG,((res==true)?"SUCCESS":"FAILURE")+": Test executeCmpCommands");

            try{
                Log.d(TAG, "testRegistrationStatus");
                res=testRegistrationStatus(stringList_);
            }catch(Throwable e){
                Log.e(TAG, "Executing testRegistrationStatus failed due to exception "+e);
                res=false;
            }
            allTestsPassed_=(allTestsPassed_==false)?allTestsPassed_:res;
            Log.i(TESTTAG,((res==true)?"SUCCESS":"FAILURE")+": Test isRootContainerRegistered and isSPContainerRegistered");

            try{
                Log.d(TAG, "testGetters");
                res=testGetters(stringList_);
            }catch(Throwable e){
                Log.e(TAG, "Executing testGetters failed due to exception "+e);
                res=false;
            }
            allTestsPassed_=(allTestsPassed_==false)?allTestsPassed_:res;
            Log.i(TESTTAG,((res==true)?"SUCCESS":"FAILURE")+": Test getVersion and getDeviceId");

            try{
                Log.d(TAG, "testLock");
                res=testLock(stringList_);
            }catch(Throwable e){
                Log.e(TAG, "Executing testLock failed due to exception "+e);
                res=false;
            }
            allTestsPassed_=(allTestsPassed_==false)?allTestsPassed_:res;
            Log.i(TESTTAG,((res==true)?"SUCCESS":"FAILURE")+": Test acquireLock and releaseLock");

            try{
                Log.d(TAG, "testDoProvisioning");
                res=testDoProvisioning(stringList_);
            }catch(Throwable e){
                Log.e(TAG, "Executing testDoProvisioning failed due to exception "+e);
                res=false;
            }
            allTestsPassed_=(allTestsPassed_==false)?allTestsPassed_:res;
            Log.i(TESTTAG,((res==true)?"SUCCESS":"FAILURE")+": Test doProvisioning");

            try{
                Log.d(TAG, "testContainerStructureAndState");
                res=testContainerStructureAndState(stringList_);
            }catch(Throwable e){
                Log.e(TAG, "Executing testContainerStructureAndState failed due to exception "+e);
                res=false;
            }
            allTestsPassed_=(allTestsPassed_==false)?allTestsPassed_:res;
            Log.i(TESTTAG,((res==true)?"SUCCESS":"FAILURE")+": Test getSPContainerStructure and getSPContainerState");

            try{
                Log.d(TAG, "testSessionHandling");
                res=testSessionHandling(stringList_);
            }catch(Throwable e){
                Log.e(TAG, "Executing testSessionHandling failed due to exception "+e);
                res=false;
            }
            allTestsPassed_=(allTestsPassed_==false)?allTestsPassed_:res;
            Log.i(TESTTAG,((res==true)?"SUCCESS":"FAILURE")+": Test session handling");
            
            printFinalResults();
            return null;
        }

        public void onServiceDisconnected(ComponentName className){
            provisioningServiceIfc_=null; 
        }

        /**
        Test content management protocol commands
        */
        private boolean testCmpCommands(List<String> stringList){
            String logi=new String();
            CommandResult  res=new CommandResult(0xFFFF0000);
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
                res = new CommandResult(0xFFFFFFFF);
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
        private static final int TEST_UID=0xFFFF0000;
        private static final int TEST_UID_LOCKED=4321;
        
        private boolean testRegistrationStatus(List<String> stringList){

            TEST_SPID = new SPID(8);
            String logi=new String();
            boolean overallsuccess=true;

            BooleanResult rcResult=new BooleanResult(false);
            BooleanResult scResult=new BooleanResult(false);

            CommandResult rcRet = new CommandResult(0xFFFF0000);
            CommandResult scRet = new CommandResult(0xFFFF0000);

            try{
                rcRet = provisioningServiceIfc_.isRootContainerRegistered(rcResult);
            }catch(Throwable e){
                logi = logi.concat("FAILURE: call to isRootContainerRegistered failed: " + e + "\n");
                rcRet = new CommandResult(0xFFFFFFFF);
                overallsuccess=false;
            }

            try{
                scRet = provisioningServiceIfc_.isSPContainerRegistered(TEST_SPID, scResult);
            }catch(Throwable e){
                logi = logi.concat("FAILURE: call to isSPContainerRegistered failed: " + e + "\n");
                scRet = new CommandResult(0xFFFFFFFF);
                overallsuccess=false;
            }

            overallsuccess=!overallsuccess?overallsuccess:(rcRet.isOk());
            overallsuccess=!overallsuccess?overallsuccess:(rcResult.result()==true);
            overallsuccess=!overallsuccess?overallsuccess:(scRet.isOk());
            overallsuccess=!overallsuccess?overallsuccess:(scResult.result()==true);            

            logi = logi.concat("================= Results of testing registration status: ").concat(((overallsuccess==true)?"SUCCESS":"FAILURE")+"\n");
            logi = logi.concat(((rcRet.isOk())?"SUCCESS":"FAILURE")+": Testing isRootContainerRegistered "+((rcRet.isOk())?"\n":("returned: " +rcRet+" \n")));
            logi = logi.concat(((rcResult.result()==true)?"SUCCESS":"FAILURE")+": Testing isRootContainerRegistered boolean result "+((rcResult.result()==true)?"\n":("is not registered even though expected\n")));

            logi = logi.concat(((scRet.isOk())?"SUCCESS":"FAILURE")+": Testing isSPContainerRegistered "+((scRet.isOk())?"\n":("returned: " +scRet+" \n")));
            logi = logi.concat(((scResult.result()==true)?"SUCCESS":"FAILURE")+": Testing isSPContainerRegistered boolean result "+((scResult.result()==true)?"\n":("is not registered even though expected\n"))); 
            logi = logi.concat("==========================================================================\n");

            stringList.add(logi);            
            return overallsuccess;
        }
        
        private boolean testGetters(List<String> stringList){

            String logi=new String();
            boolean overallsuccess=true;
            CommandResult gvRet=new CommandResult(0xFFFF0000);
            CommandResult gdiRet=new CommandResult(0xFFFF0000);
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
                gvRet = new CommandResult(0xFFFFFFFF);
                overallsuccess=false;
            }

            try{
                gdiRet=provisioningServiceIfc_.getDeviceId(test_suid);        
            }catch(Throwable e){
                logi = logi.concat("FAILURE: call to getDeviceId failed: " + e + "\n");
                gdiRet = new CommandResult(0xFFFFFFFF);
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
            CommandResult alRet=new CommandResult(0xFFFF0000);
            CommandResult alLockedRet=new CommandResult(0xFFFF0000);
            CommandResult rlLockedRet=new CommandResult(0xFFFF0000);
            CommandResult rlRet=new CommandResult(0xFFFF0000);
            CommandResult rlOpenRet=new CommandResult(0xFFFF0000);

        // first test locking when the lock is open            
            try{
                alRet=provisioningServiceIfc_.acquireLock(TEST_UID);
            }catch(Throwable e){
                logi = logi.concat("FAILURE: call to acquireLock failed: " + e + "\n");
                alRet = new CommandResult(0xFFFFFFFF);
                overallsuccess=false;
            }

        // then test locking when the lock is aquired 
            try{
                alLockedRet=provisioningServiceIfc_.acquireLock(TEST_UID_LOCKED);
            }catch(Throwable e){
                logi = logi.concat("FAILURE: call to acquireLock failed: " + e + "\n");
                alLockedRet = new CommandResult(0xFFFFFFFF);
                overallsuccess=false;
            }
        
        // then test opening the lock with wrong UID when the lock is aquired 
            try{
                rlLockedRet=provisioningServiceIfc_.releaseLock(TEST_UID_LOCKED);        
            }catch(Throwable e){
                logi = logi.concat("FAILURE: call to releaseLock failed: " + e + "\n");
                rlLockedRet = new CommandResult(0xFFFFFFFF);
                overallsuccess=false;
            }

        // then test opening the lock when the lock is aquired 
            try{
                rlRet=provisioningServiceIfc_.releaseLock(TEST_UID);        
            }catch(Throwable e){
                logi = logi.concat("FAILURE: call to releaseLock failed: " + e + "\n");
                rlRet = new CommandResult(0xFFFFFFFF);
                overallsuccess=false;
            }

        // then test opening the lock when already open
            try{
                rlOpenRet=provisioningServiceIfc_.releaseLock(TEST_UID);        
            }catch(Throwable e){
                logi = logi.concat("FAILURE: call to releaseLock failed: " + e + "\n");
                rlOpenRet = new CommandResult(0xFFFFFFFF);
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
        
        private synchronized boolean allIntentsReceived(){
            return (intentsReceived_[0] && intentsReceived_[1] && intentsReceived_[2] && intentsReceived_[3] && intentsReceived_[4] && intentsReceived_[5] && intentsReceived_[6]);
        }

        private boolean someIntentsReceived(){
            return (intentsReceived_[0] ^ intentsReceived_[1] ^ intentsReceived_[2] ^ intentsReceived_[3] ^ intentsReceived_[4] ^ intentsReceived_[5] ^ intentsReceived_[6]);
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
        
        private boolean testDoProvisioning(List<String> stringList){
            return testDoProvisioning(stringList, false);
        }
        
        private boolean testDoProvisioning(List<String> stringList, boolean embeddedInOtherTest){
            TEST_SPID = new SPID(8);
            String logi=new String();
            boolean overallsuccess=true;
            CommandResult ret=new CommandResult(0xFFFF0000);
            for(int i=0; i<intentsReceived_.length; i++) intentsReceived_[i]=false;

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
            registerReceiver(receiver, filter, null, handler);

            BroadcastReceiver endReceiver=new BroadcastReceiver(){
                public void onReceive(Context ctx, Intent intent){
                    Log.d(TAG, "Received finished intent");
                    markIntentReceived(0);
                }            
            };
            IntentFilter endFilter=new IntentFilter(RootPAProvisioningIntents.FINISHED_ROOT_PROVISIONING);
            registerReceiver(endReceiver, endFilter, null, handler);

            BroadcastReceiver errorReceiver=new BroadcastReceiver(){
                public void onReceive(Context ctx, Intent intent){
                    Log.d(TAG, "Received error intent: "+intent.getIntExtra(RootPAProvisioningIntents.ERROR, 0));
                    markIntentReceived(0);
                }            
            };
            IntentFilter errorFilter=new IntentFilter(RootPAProvisioningIntents.PROVISIONING_ERROR);
            registerReceiver(errorReceiver, errorFilter, null, handler);

            try{
                ret=provisioningServiceIfc_.doProvisioning(TEST_UID, TEST_SPID);
            }catch(Throwable e){
                logi = logi.concat("FAILURE: call to doProvisioning failed: " + e + "\n");
                Log.d(TAG, ": call to doProvisioning failed: "+e);
                ret = new CommandResult(0xFFFFFFFF);
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
                        if(totalTime>3000){
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
            
            unregisterReceiver(receiver);
            unregisterReceiver(endReceiver);
            unregisterReceiver(errorReceiver);
            handlerThread.quit();
            
            overallsuccess=!overallsuccess?overallsuccess:allIntentsReceived();
            overallsuccess=!overallsuccess?overallsuccess:(ret.isOk());

            if(!embeddedInOtherTest)
            {
                logi = logi.concat("================= Results of testing doProvisioning: ").concat(((overallsuccess==true)?"SUCCESS":"FAILURE")+"\n");            
            }
            for(int i=0; i < 7; i++){ // 7 == number of intents to be received
                logi = logi.concat(((intentsReceived_[i]==true)?"SUCCESS":"FAILURE")+": Testing doProvisioning, receiving intent["+i+"]\n");
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
            final int EXPECTED_NUMBER_OF_TRUSTLETS=2;
            final TrustletContainerState EXPECTED_TLT_CONT_STATE=TrustletContainerState.SP_LOCKED;
            long FOURS=0x0303030304040404L;
            long FIVES=0x0505050506060606L;
            final UUID EXPECTED_TLT_ID=new UUID(FOURS,FIVES); //CmpTest.TLTUUID;

            TEST_SPID = new SPID(8);
            String logi=new String();
            boolean overallsuccess=true;
            CommandResult strRet=new CommandResult(0xFFFF0000);
            CommandResult stRet=new CommandResult(0xFFFF0000);
            SPContainerStructure test_structure = new SPContainerStructure();
            SPContainerStateParcel test_state = new SPContainerStateParcel();           
                       
            try{
                strRet=provisioningServiceIfc_.getSPContainerStructure(TEST_SPID, test_structure);
            }catch(Exception e){
                logi = logi.concat("FAILURE: call to getSPContainerStructure failed: " + e + "\n");
                strRet = new CommandResult(0xFFFFFFFF);
                overallsuccess=false;
            }

            try{
                stRet=provisioningServiceIfc_.getSPContainerState(TEST_SPID, test_state);
            }catch(Exception e){
                logi = logi.concat("FAILURE: call to getSPContainerState failed: " + e + "\n");
                stRet = new CommandResult(0xFFFFFFFF);
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

            logi = logi.concat("==========================================================================\n");

            stringList.add(logi);
            return overallsuccess;
        }

        /**
        This method does not test any additional commands but it tests the ones that were already tested, this time the emphasis being in testing
        MobiCore session.
        */
        private CommandResult sessionTestCommands(List<CmpTest> testCases)
        {
            List<CmpCommand> commands=null;
            List<CmpResponse> responses=null;

            CommandResult res=new CommandResult(0xFFFF0000);
            try{
                commands=new ArrayList<CmpCommand>();
                responses=new ArrayList<CmpResponse>();
                for(int i=0; i<testCases.size(); i++){
                    commands.add(testCases.get(i).createCommand());
                    commands.get(i).setIgnoreError(false);
                }
            }catch(java.lang.Exception e){
                Log.e(TAG,"FAILURE: broken test case, initializing data failed: ",e);
            }


            try{
                res=provisioningServiceIfc_.executeCmpCommands(TEST_UID, commands, responses);
            }catch(Throwable e){
                res = new CommandResult(0xFFFFFFFF);
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

            try{
                provisioningServiceIfc_.acquireLock(TEST_UID); // no checking of return value since we do not test lock here (might help in debugging though)
            }catch(Exception e){
                Log.d(TAG, "acquiring lock failed "+e);
            }

    // the authentication is supposed to succeed
        
            List<CmpTest> spAuthTestCases = CmpTest.generateSpAuth();
            CommandResult authRes=new CommandResult(0xFFFF0000);
            authRes=sessionTestCommands(spAuthTestCases);

    // the following cases are supposed to succeed even when running in different "batch" from the actual authentication          
            Log.d(TAG,"session tests: cmp authenticated");
            List<CmpTest> spTestCases = CmpTest.generateSpCommandsAndAuthTerminate();
            CommandResult spRes=new CommandResult(0xFFFF0000);
            spRes=sessionTestCommands(spTestCases);
    
    // the following cases are supposed to fail since we are not authenticated
            
            Log.d(TAG,"session tests: cmp not authenticated");
            List<CmpTest> nonAuthCases = CmpTest.generateSpCommandsAndAuthTerminate();
            CommandResult nonAuthRes=new CommandResult(0xFFFF0000);
            nonAuthRes=sessionTestCommands(nonAuthCases);

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
            CommandResult auth2Res=new CommandResult(0xFFFF0000);
            auth2Res=sessionTestCommands(spAuth2TestCases);

    // call do provisioning (should return an error since locked)

            Log.d(TAG,"session tests: doProvisioning should fail");
            boolean failedProvisioningOk = !testDoProvisioning(stringList, true);

            try{
                provisioningServiceIfc_.releaseLock(TEST_UID); // no checking of return value since we do not test lock here (might help in debugging though)
            }catch(Exception e){
                Log.d(TAG, "releasing lock failed "+e);
            }

    // this is supposed to succeed since own session is created for SE
           Log.d(TAG,"session tests: doProvisioning should succeed");
           boolean successfullProvisioningOk=testDoProvisioning(stringList, true);

    // the following cases are supposed to fail since we are not authenticated (successful doProvisioning closed the session), 
    // unfortunately this will pass also if the execution fails seriously

            try{
                provisioningServiceIfc_.acquireLock(TEST_UID); // no checking of return value since we do not test lock here (might help in debugging though)
            }catch(Exception e){
                Log.d(TAG, "acquiring lock failed "+e);
            }

            Log.d(TAG,"session tests: cmp not auth after doProvisioning");
            List<CmpTest> nonAuth2Cases = CmpTest.generateSpCommandsAndAuthTerminate();
            CommandResult nonAuth2Res=new CommandResult(0xFFFF0000);
            nonAuth2Res=sessionTestCommands(nonAuthCases);

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
            logi = logi.concat(((!nonAuth2Res.isOk())?"SUCCESS":"FAILURE")+": sp nonauth 2 executeCmpCommands returned: "+nonAuth2Res+"\n");

            for(int i=0; i<spAuthTestCases.size(); i++){
                logi = logi.concat(((spAuthTestCases.get(i).result_==true)?"SUCCESS":"FAILURE")+": Testing "+spAuthTestCases.get(i).id()+"\n");
                overallsuccess=(overallsuccess==true)?spAuthTestCases.get(i).result_:overallsuccess;
            }

            for(int i=0; i<spTestCases.size(); i++){
                logi = logi.concat(((spTestCases.get(i).result_==true)?"SUCCESS":"FAILURE")+": Testing "+spTestCases.get(i).id()+"\n");
                overallsuccess=(overallsuccess==true)?spTestCases.get(i).result_:overallsuccess;
            }

            // expecting to fail, checking accordingly
            for(int i=0; i<nonAuthCases.size(); i++){
                logi = logi.concat(((nonAuthCases.get(i).result_==false)?"SUCCESS":"FAILURE")+": Testing "+nonAuthCases.get(i).id()+"\n");
                overallsuccess=(overallsuccess==true)?(nonAuthCases.get(i).result_==false):overallsuccess;
            }

            for(int i=0; i<spAuth2TestCases.size(); i++){
                logi = logi.concat(((spAuth2TestCases.get(i).result_==true)?"SUCCESS":"FAILURE")+": Testing "+spAuth2TestCases.get(i).id()+"\n");
                overallsuccess=(overallsuccess==true)?spAuth2TestCases.get(i).result_:overallsuccess;
            }

            // expecting to fail, checking accordingly
            for(int i=0; i<nonAuth2Cases.size(); i++){
                logi = logi.concat(((nonAuth2Cases.get(i).result_==false)?"SUCCESS":"FAILURE")+": Testing "+nonAuth2Cases.get(i).id()+"\n");
                overallsuccess=(overallsuccess==true)?(nonAuth2Cases.get(i).result_==false):overallsuccess;
            }

            logi = logi.concat("==========================================================================\n");

            stringList.add(logi);
            return overallsuccess;            
        }
    };

    private class TestOemService extends AsyncTask<IBinder, Void, Void> {
        protected Void doInBackground(IBinder... s) {
            IBinder service=s[0];
            boolean res=true;
            oemServiceIfc_= RootPAOemIfc.Stub.asInterface(service);
            if(oemServiceIfc_ == null){
                Log.e(TAG,"FAILURE: no oemServiceIfc_, why?");
            }

            try{
                Log.d(TAG, "testUnregisterRootContainer");
                res=testUnregisterRootContainer(stringList_);
            }catch(Throwable e){
                Log.e(TAG, "Executing testUnregisterRootContainer failed due to exception "+e);
                res=false;
            }
            allTestsPassed_=(allTestsPassed_==false)?allTestsPassed_:res;
            Log.i(TESTTAG,((res==true)?"SUCCESS":"FAILURE")+": Test testUnregisterRootContainer");
            printFinalResults();
            return null;
        }
        
        boolean unregisterReceived_=false;
		int unregisterError_=CommandResult.ROOTPA_OK;
        boolean done_=false;		
        
        boolean testUnregisterRootContainer(List<String> stringList){
            String logi=new String();            
            boolean overallsuccess=true;
            CommandResult ret=new CommandResult(0xFFFF0000);            

        // set receiver for intents
        
            android.os.HandlerThread handlerThread = new android.os.HandlerThread("intentReceiverThread");
            handlerThread.start();
            android.os.Looper looper = handlerThread.getLooper();
            android.os.Handler handler= new android.os.Handler(looper);

            BroadcastReceiver errorReceiver=new BroadcastReceiver(){
                public void onReceive(Context ctx, Intent intent){
					unregisterError_=intent.getIntExtra(RootPAProvisioningIntents.ERROR, 0);
                    Log.d(TAG, "Received error intent: "+intent.getIntExtra(RootPAProvisioningIntents.ERROR, unregisterError_));
                }            
            };
            IntentFilter errorFilter=new IntentFilter(RootPAProvisioningIntents.PROVISIONING_ERROR);
            registerReceiver(errorReceiver, errorFilter, null, handler);
            
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
            registerReceiver(statusreceiver, statusfilter, null, handler);
            
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
                        if(totalTime>3000){
                            break;
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

            unregisterReceiver(errorReceiver);
            unregisterReceiver(statusreceiver);
            handlerThread.quit();            

            overallsuccess=!overallsuccess?overallsuccess:(ret.isOk());
            overallsuccess=!overallsuccess?overallsuccess:(unregisterReceived_==true);
            overallsuccess=!overallsuccess?overallsuccess:(unregisterError_==CommandResult.ROOTPA_OK);

            logi = logi.concat("================= Results of testing unregisterRootContainer: ").concat(((overallsuccess==true)?"SUCCESS":"FAILURE")+"\n");            
            logi = logi.concat(((ret.isOk())?"SUCCESS":"FAILURE")+": Testing unregisterRootContainer, tlt "+((ret.isOk())?"\n":("returned: " +ret+" \n")));
            logi = logi.concat(((unregisterReceived_==true)?"SUCCESS":"FAILURE")+": Testing unregisterRootContainer, finished intent receiving\n");
            logi = logi.concat(((unregisterError_==CommandResult.ROOTPA_OK)?"SUCCESS":"FAILURE")+": Testing unregisterRootContainer error check"+((unregisterError_==CommandResult.ROOTPA_OK)?"\n":(" error intent received: " +unregisterError_+" \n")));
            logi = logi.concat("==========================================================================\n");

            stringList.add(logi);
            return overallsuccess;            
        }
    };
}
