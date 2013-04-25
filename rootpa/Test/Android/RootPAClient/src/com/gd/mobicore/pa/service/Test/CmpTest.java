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

import java.util.List;
import java.util.ArrayList;
import android.util.Log;

import com.gd.mobicore.pa.ifc.CommandResult;
import com.gd.mobicore.pa.ifc.CmpMsg;
import com.gd.mobicore.pa.ifc.CmpCommand;
import com.gd.mobicore.pa.ifc.CmpResponse;


/**
This is base class for content management protocol test cases. It has two 
main methods that in many cases should be implemented also in the inherited 
class. One is for creating the cmp command, one is for checking the results.

In addition to that this base class contains some common constants for tests 
in emulators with mcStub and factory method that returns a list of all test 
cases to be executed.
*/
public class CmpTest{
    protected static final String TAG = "RootPA-T";

    protected final static int RETCODE_LENGTH=4;
    protected final static int SHA256_LENGTH=32;
    protected final static int VERSION_ID_LENGTH=4;
    
    protected final static int SIZEFIELD_LENGTH=4;
    protected final static int CMD_HEADER_LENGTH=12;
    protected final static int RSP_HEADER_LENGTH=16;

    protected final static int HOW_MUCH_TLT_CONT_2_1_IS_BIGGER_THAN_OLD=36;    
    
    protected final static int PID=9;
    public final static byte[] TLTUUID={3,3,3,3,4,4,4,4,5,5,5,5,6,6,6,6};
    protected final static int SPID=8;
    protected final static int ROOTID=7;
    
    protected final static byte[] MAC={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};

    protected final static byte[] ENC={1,1,1,1,1,1,1,1,1,1, 
                                       2,2,2,2,2,2,2,2,2,2,
                                       3,3,3,3,3,3,3,3,3,3,
                                       4,4,4,4,4,4,4,4,4,4,
                                       5,5,5,5,5,5,5,5};

    public final static byte[] EXPECTED_SUID={
        0, 0, 0, 0, (byte) 0x44, (byte) 0x55, (byte) 0x66, (byte) 0x77, (byte) 0x88, (byte) 0x99, 
        (byte) 0xAA, (byte) 0xBB, (byte) 0xCC, (byte) 0xDD, (byte) 0xEE, (byte) 0xFF };
    

    protected final static byte[] AUTH_TOKEN={
                                      0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1, // 20
                                      0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2, // 40
                                      0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3, // 60
                                      0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,                                 // 72
                                      0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8, // 92
                                      0x9,0x9,0x9,0x9,0x9,0x9,0x9,0x9,0x9,0x9,0x9,0x9,0x9,0x9,0x9,0x9,0x9,0x9,0x9,0x9, // 112
                                      0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA, // 132 
                                      0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB  // 152
                                      };
        
    protected final static byte[] EXPECTED_ROOT_CONT={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                                      2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
                                                      3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
                                                      4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
                                                      5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
                                                      6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                                                      7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                                                      8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
                                                      9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
                                                      0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA, // 200
                                                      0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB,0xB  // 220
                                                    };


    protected final static byte[] EXPECTED_SP_CONT={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                                    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
                                                    3,3,3,3,3,3,3,3,3,3,3,3,4,0,0,0,3,3,3,3,
                                                    3,3,3,3,4,4,4,4,5,5,5,5,6,6,6,6,3,3,3,3,                            // 2 TLT CONTAINERS WITH SAME ID FOR TESTING, rest are FF
                                                    4,4,4,4,5,5,5,5,6,6,6,6,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,
                                                    (byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,
                                                    (byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,
                                                    (byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,
                                                    (byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,
                                                    (byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,
                                                    (byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,
                                                    (byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,
                                                    (byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,
                                                    (byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,
                                                    (byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,
                                                    (byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,0x10,0x10,0x10,0x10, // 320
                                                    0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11, // 340
                                                    0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12, // 360
                                                    0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13, // 380
                                                    0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14                      // 396
                                                    };


    protected final static byte[] EXPECTED_TLT_CONT={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                                    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
                                                    3,3,3,3,3,3,3,3,3,3,3,3,4,0,0,0,3,3,3,3, // state = 4
                                                    4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
                                                    5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
                                                    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                                                    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                                                    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8};        // 156


/**
Add new test cases here. Not that any test case that requires registry access after 
receiving response from the cmtl can not be the last one (mcStub can only check if 
registry access was performed in the next test case execution). GetVersion is good to 
keep as the last test cases since it does not require registry access.

Since mcStub requires correct authentication for executing the commands, we have 
CmpAuthenticate() here more than once. On the other hand, differently from MobiCore 
mcStub allows multiple different authentication, thats why we have CmpAuthenticateTerminate
only once.
*/
    public static final List<CmpTest> generateAll(){   

        ArrayList<CmpTest> cases=new ArrayList<CmpTest>();

        cases.add(new CmpGetSuid());
        cases.add(new CmpGetVersion());
        cases.add(new CmpBeginSocAuthentication());
        cases.add(new CmpAuthenticate());
        cases.add(new CmpGenerateAuthToken());

        cases.add(new CmpRootContRegisterActivate());
        cases.add(new CmpBeginRootAuthentication());
        cases.add(new CmpAuthenticate());
        cases.add(new CmpRootContLockByRoot());
        cases.add(new CmpRootContUnlockByRoot());

        cases.add(new CmpRootContUnregister());        
        cases.add(new CmpSpContLockByRoot());
        cases.add(new CmpSpContUnlockByRoot());
        cases.add(new CmpSpContUnregister());                       
        cases.add(new CmpBeginSpAuthentication());

        cases.add(new CmpAuthenticate());
        cases.add(new CmpSpContRegister());
        cases.add(new CmpSpContRegisterActivate());
        cases.add(new CmpSpContActivate());
        cases.add(new CmpSpContLockBySp());

        cases.add(new CmpSpContUnlockBySp());
        cases.add(new CmpTltContPersonalize());
        cases.add(new CmpTltContActivate());
        cases.add(new CmpTltContLockBySp());
        cases.add(new CmpTltContRegister());

        cases.add(new CmpTltContRegisterActivate());
        cases.add(new CmpTltContUnlockBySp());
        cases.add(new CmpTltContUnregister());        
        cases.add(new CmpAuthenticateTerminate());

        return cases;
    }


    public static final List<CmpTest> generateSpAuth(){   

        ArrayList<CmpTest> cases=new ArrayList<CmpTest>();
        cases.add(new CmpBeginSpAuthentication());
        cases.add(new CmpAuthenticate());
        return cases;
    }

    public static final List<CmpTest> generateSpCommandsAndAuthTerminate(){

        ArrayList<CmpTest> cases=new ArrayList<CmpTest>();

        cases.add(new CmpTltContRegisterActivate());
        cases.add(new CmpTltContLockBySp());
        cases.add(new CmpTltContUnlockBySp());
        cases.add(new CmpTltContUnregister());        
        cases.add(new CmpAuthenticateTerminate());

        return cases;
    }

    public static final List<CmpTest> generateErrorCases(){
        ArrayList<CmpTest> cases=new ArrayList<CmpTest>();
        cases.add(new CmpUnknown());
        cases.add(new CmpTooLong());
        cases.add(new CmpTooShort());
        return cases;
    }

    public static final List<CmpTest> generateNoContainerCases(){
        ArrayList<CmpTest> cases=new ArrayList<CmpTest>();

        cases.add(new CmpBeginSpAuthentication());
        cases.add(new CmpAuthenticate());
        cases.add(new CmpTltContLockBySpNoContainer());
        cases.add(new CmpAuthenticateTerminate());

        return cases;
    }

    public static final List<CmpTest> generateNoStoreContainerCases(){
        ArrayList<CmpTest> cases=new ArrayList<CmpTest>();
        
        cases.add(new CmpGenerateAuthTokenNoStore());       

        return cases;
    }

    public static final List<CmpTest> generateResponseErrorCases(){
        ArrayList<CmpTest> cases=new ArrayList<CmpTest>();

        cases.add(new CmpGenerateAuthTokenResponseIdWrong());
        cases.add(new CmpGenerateAuthTokenOffsetPlusLengthOut());
        cases.add(new CmpGenerateAuthTokenOffsetOut());

        return cases;
    }
    
    public CmpCommand createCommand(){
        return new CmpCommand(id_);    
    }    


    CmpTest(int id){
        result_=false;
        id_=id;
    }
    
    
/**
    check
    response size > 0
    response id corresponds to command id
    return code == ROOTPA_OK

*/
   public void checkResult(CmpResponse response){
        result_=true;
        
        if(response==null){
            Log.i(TAG,"response for "+id_+" is null");        
            result_=false;
            return;
        }
        
        responseSize_=response.size();
        Log.i(TAG,"size of "+id_+" response: "+responseSize_);

        if (responseSize_==0){ 
            Log.e(TAG,"****NOTE: response size==0");            
            result_=false;
            return;
        }

        int maskedId=CmpMsg.responseIdToCommandId(response.responseId());
        if(maskedId!=id_){
            Log.e(TAG,"****NOTE: Masked ID code differs from the original "+id_+" != "+maskedId);
            result_=false;
        }
            
        int returnCode=response.returnCode();
        if (returnCode!=0){
            Log.e(TAG,"****NOTE: Return code from "+id_+" is different from 0: 0x"+ Integer.toHexString(returnCode));
            result_=false;
        }

        Log.i(TAG,"basic checks for "+id_+" done, returning "+result_);
        return;
    }

    protected String byteArrayToDisplayableString(byte[] bytes){
        if(bytes==null)return "null";
        StringBuffer s=new StringBuffer(bytes.length*4);
        for(int i=0;i<bytes.length;i++)
        {
            s.append(((bytes[i]&0xFF)>15)?" 0x":" 0x0");
            s.append(Integer.toHexString(bytes[i]&0xFF));
        }
        return s.toString();
    }
    
    public static String byteArrayToDisplayable(byte[] bytes){
        return (new CmpTest(0).byteArrayToDisplayableString(bytes));
    }

    public int id(){
        return id_;
    }
    
    public void fail(){
        result_=false;
    }


    protected int responseSize_;
    protected int id_; 
    public boolean result_;
}
