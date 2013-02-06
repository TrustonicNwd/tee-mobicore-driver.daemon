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

import android.util.Log;
import com.gd.mobicore.pa.ifc.CommandResult;
import com.gd.mobicore.pa.ifc.CmpMsg;
import com.gd.mobicore.pa.ifc.CmpCommand;
import com.gd.mobicore.pa.ifc.CmpResponse;

public class CmpGetVersion extends CmpTest{

    public CmpGetVersion(){
        super(CmpMsg.MC_CMP_CMD_GET_VERSION);
    }

   public void checkResult(CmpResponse response){
        super.checkResult(response);
        if(false==result_) return;

        int versionTag=response.getInt(8);

        if( versionTag != 2 || 
            !response.getString(12, 64).trim().equals("xxxxx") ||
            response.getInt(76) != 1 || 
            response.getInt(80) != 2 || 
            response.getInt(84) != 3 ||
            response.getInt(88) != 4 || 
            response.getInt(92) != 5 || 
            response.getInt(96) != 6 ||
            response.getInt(100) != 7 || 
            response.getInt(104) != 8){
            result_=false;    
        }

        if(versionTag==2 && responseSize_>=108){
            if(result_==false){
                Log.i(TAG,"Version tag: "+versionTag);
                Log.i(TAG,"Version productID: "+response.getString(12, 64).trim());
                Log.i(TAG,"Version MCI      : "+Integer.toHexString(response.getInt(76)));
                Log.i(TAG,"Version SO       : "+Integer.toHexString(response.getInt(80)));
                Log.i(TAG,"Version MCLF     : "+Integer.toHexString(response.getInt(84)));
                Log.i(TAG,"Version Container: "+Integer.toHexString(response.getInt(88)));
                Log.i(TAG,"Version MC config: "+Integer.toHexString(response.getInt(92)));
                Log.i(TAG,"Version TLAPI    : "+Integer.toHexString(response.getInt(96)));                                
                Log.i(TAG,"Version DRAPI    : "+Integer.toHexString(response.getInt(100)));
                Log.i(TAG,"Version CMP      : "+Integer.toHexString(response.getInt(104)));
            }
        }else if(versionTag ==1 && responseSize_>=16){
            Log.i(TAG,"Version tag: "+versionTag);
            Log.i(TAG,"**** NOTE: Old version tag");
            Log.i(TAG,"Version: "+response.getInt(12));
        }else{    
            result_=false;
        }

        return;
    }
}

