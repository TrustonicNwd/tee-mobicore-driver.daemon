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

public class CmpTltContRegisterActivate extends CmpTest{
    
    protected final static int CMD_SPID_INDEX=CMD_HEADER_LENGTH;
    protected final static int SPID_SIZE=4;
    protected final static int CMD_TLTUUID_INDEX=CMD_SPID_INDEX + SPID_SIZE;
    protected final static int CMD_ED_INDEX=CMD_TLTUUID_INDEX + TLTUUID.length + SHA256_LENGTH + VERSION_ID_LENGTH + SIZEFIELD_LENGTH;
    protected final static int CMD_MAC_INDEX=CMD_ED_INDEX + ENC.length;

    protected final static int RSP_SPCONT_INDEX=RSP_HEADER_LENGTH  + SIZEFIELD_LENGTH;
    protected final static int RSP_TLTCONT_INDEX=RSP_SPCONT_INDEX + EXPECTED_SP_CONT.length  + SIZEFIELD_LENGTH;
    protected final static int RSP_MAC_INDEX=RSP_TLTCONT_INDEX + EXPECTED_TLT_CONT.length+HOW_MUCH_TLT_CONT_2_1_IS_BIGGER_THAN_OLD;

    public CmpTltContRegisterActivate(){
        super(CmpMsg.MC_CMP_CMD_TLT_CONT_REGISTER_ACTIVATE);
    }


    public CmpCommand createCommand(){
        CmpCommand command=new CmpCommand(id_);
        command.setInt(CMD_SPID_INDEX, SPID);
        command.setByteArray(CMD_TLTUUID_INDEX, TLTUUID);
        command.setByteArray(CMD_ED_INDEX, ENC);
        command.setByteArray(CMD_MAC_INDEX, MAC);
        return command;
    }    
    
    public void checkResult(CmpResponse response){
        super.checkResult(response);
        if(false==result_)return;


        byte[] enc=response.getByteArray(RSP_SPCONT_INDEX, EXPECTED_SP_CONT.length);
        if(!Arrays.equals(enc, EXPECTED_SP_CONT)){
            Log.i(TAG, "CmpTltContRegisterActivate: wrong sp cont:"+byteArrayToDisplayableString(enc));
            result_=false;
        }

        enc=response.getByteArray(RSP_TLTCONT_INDEX, EXPECTED_TLT_CONT.length);
        if(!Arrays.equals(enc, EXPECTED_TLT_CONT)){
            Log.i(TAG, "CmpTltContRegisterActivate: wrong tlt cont:"+byteArrayToDisplayableString(enc));
            result_=false;
        }

        byte[] mac=response.getByteArray(RSP_MAC_INDEX, MAC.length);
        if(!Arrays.equals(mac, MAC)){
            Log.i(TAG, "CmpTltContRegisterActivate: wrong MAC:"+byteArrayToDisplayableString(mac));
            result_=false;
        }
        return;
    }
}
