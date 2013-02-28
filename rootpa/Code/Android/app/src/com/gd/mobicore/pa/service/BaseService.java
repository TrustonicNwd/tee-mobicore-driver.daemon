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

package com.gd.mobicore.pa.service;

import android.app.Service;
import android.content.Intent;

import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.atomic.AtomicInteger;

import com.gd.mobicore.pa.jni.CommonPAWrapper;
import com.gd.mobicore.pa.ifc.RootPAProvisioningIntents;
import com.gd.mobicore.pa.ifc.CommandResult;

public abstract class BaseService extends Service {
    protected static final String TAG = "RootPA-J";    

    /*
        being statically linked library, the Common C implementation does not handle locks, 
        they must be handled in the using implementation, in this case here.
    */
    private static final int LOCK_FREE=0;
    private static final AtomicInteger lock_= new AtomicInteger(LOCK_FREE);
    private static final int LOCK_TIMEOUT_MS=60000;
    private TimerTask timerTask_=null;
    private Timer timer_=null;

    protected int doProvisioningLockSuid_=0;
    protected byte[] se_ = null; 
    
    private static final int C_CONNECTING_SERVICE_ENABLER=1;
    private static final int C_AUTHENTICATING_SOC=2;
    private static final int C_CREATING_ROOT_CONTAINER=3;
    private static final int C_AUTHENTICATING_ROOT=4;
    private static final int C_CREATING_SP_CONTAINER=5;
    private static final int C_FINISHED_PROVISIONING=6;
    private static final int C_ERROR=7;
    private static final int C_UNREGISTERING_ROOT_CONTAINER=8;
    private static final int C_PROVISIONING_STATE_THREAD_EXITING=0xDEAD;

    protected final CommonPAWrapper commonPaWrapper_=new CommonPAWrapper(this);
   
    protected CommonPAWrapper commonPAWrapper(){
        return commonPaWrapper_;
    }

    protected synchronized CommandResult acquireLock(int uid, boolean openSession){
        Log.d(TAG,">>BaseService.acquireLock "+uid+" "+lock_.get()+" "+timer_);
        if(uid==LOCK_FREE){
            return new CommandResult(CommandResult.ROOTPA_ERROR_ILLEGAL_ARGUMENT);            
        }
        boolean result=lock_.compareAndSet(LOCK_FREE, uid);
        if(result==true || lock_.get() == uid){

            if(result==true && openSession==true){
                commonPAWrapper().openSession();
            }

            if(timer_!=null){
                timerTask_.cancel();
                timer_.cancel();
            }

            timer_=new Timer();
            timerTask_=new TimerTask(){
                public void run(){
                    lock_.set(LOCK_FREE);
                }
            };
            timer_.schedule(timerTask_,LOCK_TIMEOUT_MS);
            Log.d(TAG,"<<BaseService.acquireLock, successfull return "+timer_);
            return new CommandResult(CommandResult.ROOTPA_OK);
        }
        return new CommandResult(CommandResult.ROOTPA_ERROR_LOCK);
    }
            
    // this is public for the ProvisioningService to call it
    protected synchronized CommandResult releaseLock(int uid, boolean closeSession){
        Log.d(TAG,"BaseService.releaseLock "+uid+" "+lock_.get()+" "+timer_);
                               
        if(uid==LOCK_FREE){
            return new CommandResult(CommandResult.ROOTPA_ERROR_ILLEGAL_ARGUMENT);            
        }

        if((lock_.get()==LOCK_FREE) || (lock_.compareAndSet(uid, LOCK_FREE)==true)){

            if(closeSession){
                commonPAWrapper().closeSession();
            }

            if(timer_!=null){
                timerTask_.cancel();
                timerTask_=null;
                timer_.cancel();
                timer_=null;
            }
            return new CommandResult(CommandResult.ROOTPA_OK);
        }
        return new CommandResult(CommandResult.ROOTPA_ERROR_LOCK);
    }

    protected synchronized boolean locked(int uid){
        return(lock_.get() != uid && uid != LOCK_FREE);
    }
    
    /**
    This method is called from the C code to send the trustlet binary to the client 
    (trustlet connector/"sp.pa" for develope trustlet) that then can store it where desired.
    */
    public void trustletInstallCallback(byte[] trustlet){
        Log.d(TAG,">>DeveloperService.trustletInstallCallback");
        Intent intent=new Intent(RootPAProvisioningIntents.INSTALL_TRUSTLET);
        intent.putExtra(RootPAProvisioningIntents.TRUSTLET, trustlet);
        sendBroadcast(intent);
        Log.d(TAG,"<<DeveloperService.trustletInstallCallback");        
    }
    
    /**
    This method is called from the C code to get the path for files directory
    */
    public String getFilesDirPath(){
        return this.getFilesDir().getAbsolutePath();
    }

    /**
     This method is called from the C code to send the intents while executing doProvisioning
    */
    public void provisioningStateCallback(int state, int ret){
        Log.d(TAG,">>provisioningStateCallback "+state+" "+ret);       
        
        Intent intent=new Intent(RootPAProvisioningIntents.PROVISIONING_PROGRESS_UPDATE);
                
        switch(state){
            case C_CONNECTING_SERVICE_ENABLER:
                intent.putExtra(RootPAProvisioningIntents.STATE, RootPAProvisioningIntents.CONNECTING_SERVICE_ENABLER);
                break;
            case C_AUTHENTICATING_SOC:
                intent.putExtra(RootPAProvisioningIntents.STATE, RootPAProvisioningIntents.AUTHENTICATING_SOC);
                break;
            case C_CREATING_ROOT_CONTAINER:
                intent.putExtra(RootPAProvisioningIntents.STATE, RootPAProvisioningIntents.CREATING_ROOT_CONTAINER);
                break;
            case C_AUTHENTICATING_ROOT:
                intent.putExtra(RootPAProvisioningIntents.STATE, RootPAProvisioningIntents.AUTHENTICATING_ROOT);
                break;
            case C_CREATING_SP_CONTAINER:
                intent.putExtra(RootPAProvisioningIntents.STATE, RootPAProvisioningIntents.CREATING_SP_CONTAINER);
                break;
            case C_FINISHED_PROVISIONING:
                intent.putExtra(RootPAProvisioningIntents.STATE, RootPAProvisioningIntents.FINISHED_PROVISIONING);
                sendBroadcast(new Intent(RootPAProvisioningIntents.FINISHED_ROOT_PROVISIONING));
                break;
            case C_UNREGISTERING_ROOT_CONTAINER:
                intent.putExtra(RootPAProvisioningIntents.STATE, RootPAProvisioningIntents.UNREGISTERING_ROOT_CONTAINER);
                break;
            case C_ERROR:
                intent=new Intent(RootPAProvisioningIntents.PROVISIONING_ERROR);
            
                intent.putExtra(RootPAProvisioningIntents.ERROR, ret);
                break;

            case C_PROVISIONING_STATE_THREAD_EXITING:
                try{
                    CommandResult res=releaseLock(doProvisioningLockSuid_, false);
                    if(!res.isOk()){
                        Log.e(TAG,"provisioningStateCallback releasing lock failed, res: "+res.result());                    
                    }
                doProvisioningLockSuid_=0;                    
                intent=null; // no intent sent in this case
                }catch(Exception e){
                    Log.e(TAG,"provisioningStateCallback releasing lock failed: "+e);
                }            
                break;
            default:
                Log.e(TAG,"unknown state: "+state);
                intent=null;
                break;
        }
        if(intent!=null){
            sendBroadcast(intent);
        }

        Log.d(TAG,"<<provisioningStateCallback ");
    }    
}