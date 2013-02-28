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

import java.util.Arrays;
import android.util.Log;
import com.gd.mobicore.pa.ifc.CommandResult;
import com.gd.mobicore.pa.ifc.CmpMsg;
import com.gd.mobicore.pa.ifc.CmpCommand;
import com.gd.mobicore.pa.ifc.CmpResponse;

public class CmpGenerateAuthTokenOffsetPlusLengthOut extends CmpTest{

    public final static byte[] OFFSET_PLUS_LEN_OUT_SUID={
        0, 0, 0, 0, (byte) 0x04, (byte) 0x05, (byte) 0x66, (byte) 0x77, (byte) 0x88, (byte) 0x99, 
        (byte) 0xAA, (byte) 0xBB, (byte) 0x01, (byte) 0x01, (byte) 0x01, (byte) 0x01 };

    public final static int CMD_SUID_INDEX=4;

        
    public CmpGenerateAuthTokenOffsetPlusLengthOut(){
        super(CmpMsg.MC_CMP_CMD_GENERATE_AUTH_TOKEN);
    }
    
    public CmpCommand createCommand(){
        CmpCommand command=new CmpCommand(id_);
        byte[] ksocauth= new byte[32];
        int kid=0; 
        byte[] psssig = new byte[256]; 
                
        command.setByteArray(CMD_SUID_INDEX, OFFSET_PLUS_LEN_OUT_SUID);
        command.setByteArray(28, ksocauth);
        command.setInt(60,kid);
        command.setByteArray(64, psssig);
        return command;
    }    


   public void checkResult(CmpResponse response){
       result_=true;
        
        if(response==null){
            Log.i(TAG,"response for "+id_+" is null");        
            result_=false;
            return;
        }
        
        responseSize_=response.size();
        Log.i(TAG,"size of "+id_+" response: "+responseSize_);

        if (responseSize_!=0){ 
            Log.e(TAG,"****NOTE: response size!=0");            
            result_=false;
            return;
        }
        return;
    }

}



