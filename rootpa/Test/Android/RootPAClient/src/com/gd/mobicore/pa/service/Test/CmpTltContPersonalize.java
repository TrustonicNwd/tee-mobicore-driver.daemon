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

public class CmpTltContPersonalize extends CmpTest{
    
    protected final static byte[] PERSONALIZATION_DATA={0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,
                                                        0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,
                                                        0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,
                                                        0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,
                                                        0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,
                                                        0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,
                                                        0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71};
    

    protected final static int CMD_SPID_INDEX=CMD_HEADER_LENGTH;                   
    protected final static int SPID_SIZE=4;
    
    protected final static int CMD_TLTUUID_INDEX=CMD_SPID_INDEX + SPID_SIZE;
    
    protected final static int CMD_PID_INDEX=CMD_TLTUUID_INDEX + TLTUUID.length;
    protected final static int PID_SIZE=4;

    protected final static int CMD_LEN_INDEX=CMD_PID_INDEX + PID_SIZE;

    protected final static int CMD_PERSONALIZATION_DATA_INDEX=CMD_LEN_INDEX + SIZEFIELD_LENGTH;
    protected final static int CMD_PADDING_SIZE=1924;
    protected final static int CMD_MAC_INDEX=CMD_PERSONALIZATION_DATA_INDEX + PERSONALIZATION_DATA.length + CMD_PADDING_SIZE;

    protected final static int RSP_LEN_INDEX=RSP_HEADER_LENGTH;
    protected final static int RSP_PERSONALIZATION_DATA_INDEX=RSP_LEN_INDEX+SIZEFIELD_LENGTH;
    protected final static int RSP_PADDING_SIZE=2028;
    protected final static int RSP_MAC_INDEX=RSP_PERSONALIZATION_DATA_INDEX + PERSONALIZATION_DATA.length + RSP_PADDING_SIZE;

    public CmpTltContPersonalize(){
        super(CmpMsg.MC_CMP_CMD_TLT_CONT_PERSONALIZE);
    }


    public CmpCommand createCommand(){
        CmpCommand command=new CmpCommand(id_);
        command.setInt(CMD_SPID_INDEX, SPID);
        command.setByteArray(CMD_TLTUUID_INDEX, TLTUUID);
        command.setInt(CMD_PID_INDEX, PID);
        command.setInt(CMD_LEN_INDEX, PERSONALIZATION_DATA.length);
        Log.i(TAG, "CmpTltContPersonalize: length: "+command.getInt(CMD_LEN_INDEX));
        command.setByteArray(CMD_PERSONALIZATION_DATA_INDEX, PERSONALIZATION_DATA);
        command.setByteArray(CMD_MAC_INDEX, MAC);
        return command;
    }    
    
    public void checkResult(CmpResponse response){
        super.checkResult(response);
        if(false==result_)return;

        int len=response.getInt(RSP_LEN_INDEX);
        if(len != PERSONALIZATION_DATA.length){
            Log.i(TAG, "CmpTltContPersonalize: wrong length: "+len);
            result_=false;
        }
        
        byte[] enc=response.getByteArray(RSP_PERSONALIZATION_DATA_INDEX, PERSONALIZATION_DATA.length);
        if(!Arrays.equals(enc, PERSONALIZATION_DATA)){
            Log.i(TAG, "CmpTltContPersonalize: wrong personalization data cont: "+byteArrayToDisplayableString(enc));
            result_=false;
        }

        byte[] mac=response.getByteArray(RSP_MAC_INDEX, MAC.length);
        if(!Arrays.equals(mac, MAC)){
            Log.i(TAG, "CmpTltContPersonalize: wrong MAC:"+byteArrayToDisplayableString(mac));
            result_=false;
        }
        return;
    }
}
