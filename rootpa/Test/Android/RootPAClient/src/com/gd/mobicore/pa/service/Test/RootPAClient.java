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


// import android.os.RemoteException;

import java.util.List;
import java.util.ArrayList;
import java.lang.Integer;
     
import com.gd.mobicore.pa.ifc.RootPAProvisioningIntents;

public class RootPAClient extends Activity {

    boolean provisioningServiceIsBound_;
    boolean developerServiceIsBound_;    
    boolean oemServiceIsBound_;

    List<String> stringList_=new ArrayList<String>();
    boolean allTestsPassed_ = true;
    
    private static final String TESTTAG = "RootPA-Test";
    private static final String TAG = "RootPA-T";

    private int callCounter_=0;
    void printFinalResults()
    {
        Log.i(TAG,"printFinalResults "+callCounter_);        
        if(--callCounter_==0){ // number of services

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

    private TestDeveloperService testDeveloperService_=null;
    private ServiceConnection developerServiceConnection_ = new ServiceConnection(){
        public void onServiceDisconnected(ComponentName className){
            testDeveloperService_.disconnect();
        }
        public void onServiceConnected(ComponentName className, IBinder service){
            testDeveloperService_=new TestDeveloperService(RootPAClient.this);
            testDeveloperService_.execute(service);
        }
    };

    private TestOemService testOemService_=null;   
    private ServiceConnection oemServiceConnection_ = new ServiceConnection(){
        public void onServiceDisconnected(ComponentName className){
            testOemService_.disconnect();
        }
        public void onServiceConnected(ComponentName className, IBinder service){
            testOemService_=new TestOemService(RootPAClient.this);
            testOemService_.execute(service);
        }
    };

    private TestProvisioningService testProvisioningService_=null;   
    private ServiceConnection provisioningServiceConnection_ = new ServiceConnection(){
        public void onServiceDisconnected(ComponentName className){
            testProvisioningService_.disconnect();
        }

        public void onServiceConnected(ComponentName className, IBinder service){
            testProvisioningService_=new TestProvisioningService(RootPAClient.this);
            testProvisioningService_.execute(service);
        }
    }; 
    
    void doBindService(){       
        
        
//        byte[] address="https://195.81.216.155:44378/service-enabler/enrollment/".getBytes(); 
        byte[] address="https://10.0.2.2:443/".getBytes(); // using local apache2 server
        int loggingLevel=android.util.Log.DEBUG;
        try{
            Intent psintent=new Intent(RootPAProvisioningIntents.PROVISIONING_SERVICE);
            psintent.putExtra("SE", address);
            psintent.putExtra("LOG", loggingLevel);        
            bindService(psintent, provisioningServiceConnection_, Context.BIND_AUTO_CREATE);
            provisioningServiceIsBound_=true;
            callCounter_++;            
        
            Intent dsintent=new Intent(RootPAProvisioningIntents.DEVELOPER_SERVICE);
            dsintent.putExtra("SE", address);
            dsintent.putExtra("LOG", loggingLevel);
            bindService(dsintent, developerServiceConnection_, Context.BIND_AUTO_CREATE);       
            developerServiceIsBound_=true;
            callCounter_++;

            Intent osintent=new Intent(RootPAProvisioningIntents.OEM_SERVICE);
            osintent.putExtra("SE", address);
            osintent.putExtra("LOG", loggingLevel);        
            bindService(osintent, oemServiceConnection_, Context.BIND_AUTO_CREATE);       
            oemServiceIsBound_=true;
            callCounter_++;
            
        }catch(Exception e){
            Log.e(TAG,"an exception received in bingind "+e);
        }

        Log.d(TAG,"===Binding done");
    }

    
    void bindServiceWrongIp(ServiceConnection oemServiceWrongIpConnection){
        byte[] address="http://10.255.255.8:9/".getBytes(); // this is supposed to be nonexistent address,
        int loggingLevel=android.util.Log.DEBUG;

        Intent osintent=new Intent(RootPAProvisioningIntents.OEM_SERVICE);
        osintent.putExtra("SE", address);
        osintent.putExtra("LOG", loggingLevel);        
        bindService(osintent, oemServiceWrongIpConnection, Context.BIND_AUTO_CREATE);       
    }

    void unbindServiceWrongIp(ServiceConnection oemServiceWrongIpConnection){
        unbindService(oemServiceWrongIpConnection);
    }
    
    void doUnbindService(){
        Log.d(TAG,"===Unbinding");
        if(provisioningServiceIsBound_){
            unbindService(provisioningServiceConnection_);
            provisioningServiceIsBound_=false;
        }

        if(developerServiceIsBound_){
            unbindService(developerServiceConnection_);
            developerServiceIsBound_=false;
        }

        if(oemServiceIsBound_){
            unbindService(oemServiceConnection_);
            oemServiceIsBound_=false;
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
}
