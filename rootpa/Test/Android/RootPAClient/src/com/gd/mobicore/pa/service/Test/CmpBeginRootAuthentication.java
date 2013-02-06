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

public class CmpBeginRootAuthentication extends CmpTest{

    protected final static byte[] EXPECTED_RND={3,3,3,3,3,3,3,3};

    // using this instead of CmpTest.MAC, since the stub copies it from the container, it ensures the container is read
    protected final static byte[] EXPECTED_MAC={9,9,9,9,9,9,9,9,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xA,0xB,0xB,0xB,0xB};
    
    public CmpBeginRootAuthentication(){
        super(CmpMsg.MC_CMP_CMD_BEGIN_ROOT_AUTHENTICATION);
    }

    protected final static int RSP_SUID_INDEX=RSP_HEADER_LENGTH;
    protected final static int RSP_RND_INDEX=RSP_SUID_INDEX+EXPECTED_SUID.length;
    protected final static int RSP_MAC_INDEX=RSP_RND_INDEX+EXPECTED_RND.length;


    // at this phase the command only contains the command id, RootPA adds the container to the command
    // we can do that with base class method, no need to implement createCommand
    
    public void checkResult(CmpResponse response){
        super.checkResult(response);
        if(false==result_)return;

        byte[] suid=response.getByteArray(RSP_SUID_INDEX, EXPECTED_SUID.length);
        if(!Arrays.equals(suid, EXPECTED_SUID)){
            Log.i(TAG, "CmpBeginRootAuthentication: wrong SUID:"+byteArrayToDisplayableString(suid));
            result_=false;
        }

        byte[] rnd=response.getByteArray(RSP_RND_INDEX, EXPECTED_RND.length);
        if(!Arrays.equals(rnd, EXPECTED_RND)){
            Log.i(TAG, "CmpBeginRootAuthentication: wrong RND:"+byteArrayToDisplayableString(rnd));
            result_=false;
        }

        byte[] mac=response.getByteArray(RSP_MAC_INDEX, EXPECTED_MAC.length);
        if(!Arrays.equals(mac, EXPECTED_MAC)){
            Log.i(TAG, "CmpBeginRootAuthentication: wrong MAC:"+byteArrayToDisplayableString(mac));
            result_=false;
        }

        return;
    }
}
