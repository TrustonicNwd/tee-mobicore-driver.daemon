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

public class CmpGetSuid extends CmpTest{
    private final static int SUID_LENGTH_OLD=12;
    private final static int SUID_LENGTH_NEW=16;
    private final static int ID_LENGTH=4;
    private final static int GET_SUID_RSP_LENGTH_OLD=ID_LENGTH+RETCODE_LENGTH+SUID_LENGTH_OLD;
    private final static int GET_SUID_RSP_LENGTH_NEW=ID_LENGTH+RETCODE_LENGTH+SUID_LENGTH_NEW;
        
    public CmpGetSuid(){
        super(CmpMsg.MC_CMP_CMD_GET_SUID);
    }

    public void checkResult(CmpResponse response){
        super.checkResult(response);
        if(false==result_)return;

        int suidLength=0;
        if(responseSize_==GET_SUID_RSP_LENGTH_OLD){
            suidLength=SUID_LENGTH_OLD;
        }else if(responseSize_==GET_SUID_RSP_LENGTH_NEW){
            suidLength=SUID_LENGTH_NEW;
        }else{
            Log.e(TAG,"****NOTE, Unknown response size");
            result_=false;
        }

        byte[] suid=response.getByteArray(8, suidLength);

        if(!Arrays.equals(suid, EXPECTED_SUID)){
            Log.i(TAG, "SUID code from "+id_+" not as expected:"+byteArrayToDisplayableString(suid));
            result_=false;
        }

        return;
    }

}
