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

#include <iostream>
#include <string.h>
#include <stdlib.h>

#include <TlCm/3.0/cmpMap.h>
#include <TlCm/tlCmApiCommon.h>

#include "cmpHandler.h"

using namespace std;

static const mcSpid_t SPID = 8;
static const mcUuid_t TLTUUID={{3,3,3,3,4,4,4,4,5,5,5,5,6,6,6,6}};

static const mcSoDataCont_t DATACONT={{0x11111111,0x11111111,(mcSoContext_t)0x11111111,(mcSoLifeTime_t)0x11111111,{0x11111111,
                                      {{0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21}}},0x21212121,
                                      0x31313131},{(contType_t)0x31313131,0x31313131,{{0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,
                                      0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41}},{0x41414141},{{0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,
                                      0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,
                                      0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,
                                      0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71}}}};

/*
void printSizes()
{
    cout << MC_CMP_CMD_AUTHENTICATE << " : "<< sizeof(cmpCmdAuthenticate_t) << " : "<< sizeof(cmpRspAuthenticate_t) <<"\n";
    cout << MC_CMP_CMD_BEGIN_ROOT_AUTHENTICATION << " : "<< sizeof(cmpCmdBeginRootAuthentication_t) << " : "<< sizeof(cmpRspBeginRootAuthentication_t) <<"\n";
    cout << MC_CMP_CMD_BEGIN_SOC_AUTHENTICATION << " : "<<sizeof(cmpCmdBeginSocAuthentication_t) << " : "<< sizeof(cmpRspBeginSocAuthentication_t) <<"\n";
    cout << MC_CMP_CMD_BEGIN_SP_AUTHENTICATION << " : "<< sizeof(cmpCmdBeginSpAuthentication_t) << " : "<< sizeof(cmpRspBeginSpAuthentication_t) <<"\n";
    cout << MC_CMP_CMD_GENERATE_AUTH_TOKEN << " : "<< sizeof(cmpCmdGenAuthToken_t) << " : "<< sizeof(cmpRspGenAuthToken_t) <<"\n";    
    cout << MC_CMP_CMD_GET_VERSION << " : "<< sizeof(cmpCmdGetVersion_t) << " : "<< sizeof(cmpRspGetVersion_t) <<"\n";
    cout << MC_CMP_CMD_ROOT_CONT_LOCK_BY_ROOT << " : "<< sizeof(cmpCmdRootContLockByRoot_t) << " : "<< sizeof(cmpRspRootContLockByRoot_t) <<"\n";
    cout << MC_CMP_CMD_ROOT_CONT_REGISTER_ACTIVATE << " : "<< sizeof(cmpCmdRootContRegisterActivate_t) << " : "<< sizeof(cmpRspRootContRegisterActivate_t) <<"\n";
    cout << MC_CMP_CMD_ROOT_CONT_UNLOCK_BY_ROOT << " : "<< sizeof(cmpCmdRootContUnlockByRoot_t) << " : "<< sizeof(cmpRspRootContUnlockByRoot_t) <<"\n";
    cout << MC_CMP_CMD_ROOT_CONT_UNREGISTER << " : "<< sizeof(cmpCmdRootContUnregister_t) << " : "<< sizeof(cmpRspRootContUnregister_t) <<"\n";
    cout << MC_CMP_CMD_SP_CONT_ACTIVATE << " : "<< sizeof(cmpCmdSpContActivate_t) << " : "<< sizeof(cmpRspSpContActivate_t) <<"\n";    
    cout << MC_CMP_CMD_SP_CONT_LOCK_BY_ROOT << " : "<< sizeof(cmpCmdSpContLockByRoot_t) << " : "<< sizeof(cmpRspSpContLockByRoot_t) <<"\n";
    cout << MC_CMP_CMD_SP_CONT_LOCK_BY_SP << " : "<< sizeof(cmpCmdSpContLockBySp_t) << " : "<< sizeof(cmpRspSpContLockBySp_t) <<"\n";
    cout << MC_CMP_CMD_SP_CONT_REGISTER << " : "<< sizeof(cmpCmdSpContRegister_t) << " : "<< sizeof(cmpRspSpContRegister_t) <<"\n";
    cout << MC_CMP_CMD_SP_CONT_REGISTER_ACTIVATE << " : "<< sizeof(cmpCmdSpContRegisterActivate_t) << " : "<< sizeof(cmpRspSpContRegisterActivate_t) <<"\n";
    cout << MC_CMP_CMD_SP_CONT_UNLOCK_BY_ROOT << " : "<< sizeof(cmpCmdSpContUnlockByRoot_t) << " : "<< sizeof(cmpRspSpContUnlockByRoot_t) <<"\n";
    cout << MC_CMP_CMD_SP_CONT_UNLOCK_BY_SP << " : "<< sizeof(cmpCmdSpContUnlockBySp_t) << " : "<< sizeof(cmpRspSpContUnlockBySp_t) <<"\n";    
    cout << MC_CMP_CMD_SP_CONT_UNREGISTER << " : "<< sizeof(cmpCmdSpContUnregister_t) << " : "<< sizeof(cmpRspSpContUnregister_t) <<"\n";
    cout << MC_CMP_CMD_TLT_CONT_ACTIVATE << " : "<< sizeof(cmpCmdTltContActivate_t) << " : "<< sizeof(cmpRspTltContActivate_t) <<"\n";
    cout << MC_CMP_CMD_TLT_CONT_LOCK_BY_SP << " : "<< sizeof(cmpCmdTltContLockBySp_t) << " : "<< sizeof(cmpRspTltContLockBySp_t) <<"\n";
    cout << MC_CMP_CMD_TLT_CONT_PERSONALIZE << " : "<< sizeof(cmpCmdTltContPersonalize_t) << " : "<< sizeof(cmpRspTltContPersonalize_t) <<"\n";
    cout << MC_CMP_CMD_TLT_CONT_REGISTER << " : "<< sizeof(cmpCmdTltContRegister_t) << " : "<< sizeof(cmpRspTltContRegister_t) <<"\n";
    cout << MC_CMP_CMD_TLT_CONT_REGISTER_ACTIVATE << " : "<< sizeof(cmpCmdTltContRegisterActivate_t) << " : "<< sizeof(cmpRspTltContRegisterActivate_t) <<"\n";    
    cout << MC_CMP_CMD_TLT_CONT_UNLOCK_BY_SP << " : "<< sizeof(cmpCmdTltContUnlockBySp_t) << " : "<< sizeof(cmpRspTltContUnlockBySp_t) <<"\n";
    cout << MC_CMP_CMD_TLT_CONT_UNREGISTER << " : "<< sizeof(cmpCmdTltContUnregister_t) << " : "<< sizeof(cmpRspTltContUnregister_t) <<"\n";
    cout << MC_CMP_CMD_GET_SUID << " : "<< sizeof(cmpCmdGetSuid_t) << " : "<< sizeof(cmpRspGetSuid_t) <<"\n";
    cout << MC_CMP_CMD_AUTHENTICATE_TERMINATE << " : "<< sizeof(cmpCmdAuthenticateTerminate_t) << " : "<< sizeof(cmpRspAuthenticateTerminate_t) <<"\n";
}


*/
/**
Since the test is run with the same mcStub as tests run by RootPAClient.java, the order of the commands is the same, so should be the content, as much as it is valid

*/
uint8_t* getCommand(int commandIdx, uint32_t* commandSize)
{
    void* commandP=NULL;
    switch(commandIdx)
    {
        case 0:
            *commandSize=sizeof(cmpCmdGetSuid_t);
            commandP=malloc(*commandSize);
            if(commandP)
            { 
                memset(commandP,0,*commandSize);
                *((uint32_t*)commandP)=MC_CMP_CMD_GET_SUID;
            }
            break;
        case 1:
            *commandSize=sizeof(cmpCmdGetVersion_t);
            commandP=malloc(*commandSize);            
            if(commandP)
            { 
                memset(commandP,0,*commandSize);
                *((uint32_t*)commandP)=MC_CMP_CMD_GET_VERSION;
            }
            break;
        case 2:
            *commandSize=sizeof(cmpCmdBeginSocAuthentication_t);
            commandP=malloc(*commandSize);            
            if(commandP)
            { 
                memset(commandP,0,*commandSize);
                ((cmpCommandHeaderMap_t*)commandP)->commandId=MC_CMP_CMD_BEGIN_SOC_AUTHENTICATION;
            }
            break;
        case 3:
            *commandSize=sizeof(cmpCmdAuthenticate_t);
            commandP=malloc(*commandSize);            
            if(commandP)
            { 
                memset(commandP,0,*commandSize);
                ((cmpCommandHeaderMap_t*)commandP)->commandId=MC_CMP_CMD_AUTHENTICATE;
            }
            break;
        case 4:
            *commandSize=sizeof(cmpCmdGenAuthToken_t);
            commandP=malloc(*commandSize);            
            if(commandP)
            { 
                memset(commandP,0,*commandSize);
                ((cmpCommandHeaderMap_t*)commandP)->commandId=MC_CMP_CMD_GENERATE_AUTH_TOKEN;
            }
            break;
        case 5:
            *commandSize=sizeof(cmpCmdRootContRegisterActivate_t);
            commandP=malloc(*commandSize);
            if(commandP)
            { 
                memset(commandP,0,*commandSize);
                ((cmpCommandHeaderMap_t*)commandP)->commandId=MC_CMP_CMD_ROOT_CONT_REGISTER_ACTIVATE;
            }
            break;
        case 6:
            *commandSize=sizeof(cmpCmdBeginRootAuthentication_t);
            commandP=malloc(*commandSize);
            if(commandP)
            { 
                memset(commandP,0,*commandSize);
                ((cmpCommandHeaderMap_t*)commandP)->commandId=MC_CMP_CMD_BEGIN_ROOT_AUTHENTICATION;
            }
            break;
        case 7:
            *commandSize=sizeof(cmpCmdAuthenticate_t);
            commandP=malloc(*commandSize);
            if(commandP)
            { 
                memset(commandP,0,*commandSize);
                ((cmpCommandHeaderMap_t*)commandP)->commandId=MC_CMP_CMD_AUTHENTICATE;
            }
            break;
        case 8:
            *commandSize=sizeof(cmpCmdRootContLockByRoot_t);
            commandP=malloc(*commandSize);
            if(commandP)
            { 
                memset(commandP,0,*commandSize);
                ((cmpCommandHeaderMap_t*)commandP)->commandId=MC_CMP_CMD_ROOT_CONT_LOCK_BY_ROOT;
            }
            break;
        case 9:
            *commandSize=sizeof(cmpCmdRootContUnlockByRoot_t);
            commandP=malloc(*commandSize);
            if(commandP)
            { 
                memset(commandP,0,*commandSize);
                ((cmpCommandHeaderMap_t*)commandP)->commandId=MC_CMP_CMD_ROOT_CONT_UNLOCK_BY_ROOT;
            }
            break;
        case 10:
            *commandSize=sizeof(cmpCmdRootContUnregister_t);
            commandP=malloc(*commandSize);
            if(commandP)
            { 
                memset(commandP,0,*commandSize);
                ((cmpCommandHeaderMap_t*)commandP)->commandId=MC_CMP_CMD_ROOT_CONT_UNREGISTER;
            }
            break;
        case 11:
            *commandSize=sizeof(cmpCmdSpContLockByRoot_t);
            commandP=malloc(*commandSize);
            if(commandP)
            { 
                memset(commandP,0,*commandSize);
                ((cmpCommandHeaderMap_t*)commandP)->commandId=MC_CMP_CMD_SP_CONT_LOCK_BY_ROOT;
                ((cmpCmdSpContLockByRoot_t*)commandP)->cmd.sdata.spid=SPID;
            }
            break;
        case 12:
            *commandSize=sizeof(cmpCmdSpContUnlockByRoot_t);
            commandP=malloc(*commandSize);
            if(commandP)
            { 
                memset(commandP,0,*commandSize);
                ((cmpCommandHeaderMap_t*)commandP)->commandId=MC_CMP_CMD_SP_CONT_UNLOCK_BY_ROOT;
                ((cmpCmdSpContUnlockByRoot_t*)commandP)->cmd.sdata.spid=SPID;                
            }
            break;
        case 13:
            *commandSize=sizeof(cmpCmdSpContUnregister_t);
            commandP=malloc(*commandSize);
            if(commandP)
            { 
                memset(commandP,0,*commandSize);
                ((cmpCommandHeaderMap_t*)commandP)->commandId=MC_CMP_CMD_SP_CONT_UNREGISTER;
                ((cmpCmdSpContUnregister_t*)commandP)->cmd.sdata.spid=SPID;                
            }
            break;
        case 14:
            *commandSize=sizeof(cmpCmdBeginSpAuthentication_t);
            commandP=malloc(*commandSize);
            if(commandP)
            { 
                memset(commandP,0,*commandSize);
                ((cmpCommandHeaderMap_t*)commandP)->commandId=MC_CMP_CMD_BEGIN_SP_AUTHENTICATION;
                ((cmpCmdBeginSpAuthentication_t*)commandP)->cmd.spid=SPID;
            }
            break;
        case 15:
            *commandSize=sizeof(cmpCmdAuthenticate_t);
            commandP=malloc(*commandSize);
            if(commandP)
            { 
                memset(commandP,0,*commandSize);
                ((cmpCommandHeaderMap_t*)commandP)->commandId=MC_CMP_CMD_AUTHENTICATE;
            }
            break;
        case 16:
            *commandSize=sizeof(cmpCmdSpContRegister_t);
            commandP=malloc(*commandSize);
            if(commandP)
            { 
                memset(commandP,0,*commandSize);
                ((cmpCommandHeaderMap_t*)commandP)->commandId=MC_CMP_CMD_SP_CONT_REGISTER;
                ((cmpCmdSpContRegister_t*)commandP)->cmd.sdata.spid=SPID;
            }
            break;
        case 17:
            *commandSize=sizeof(cmpCmdSpContRegisterActivate_t);
            commandP=malloc(*commandSize);
            if(commandP)
            { 
                memset(commandP,0,*commandSize);
                ((cmpCommandHeaderMap_t*)commandP)->commandId=MC_CMP_CMD_SP_CONT_REGISTER_ACTIVATE;
                ((cmpCmdSpContRegisterActivate_t*)commandP)->cmd.sdata.spid=SPID;
            }
            break;
        case 18:
            *commandSize=sizeof(cmpCmdSpContActivate_t);
            commandP=malloc(*commandSize);
            if(commandP)
            { 
                memset(commandP,0,*commandSize);
                ((cmpCommandHeaderMap_t*)commandP)->commandId=MC_CMP_CMD_SP_CONT_ACTIVATE;
                ((cmpCmdSpContActivate_t*)commandP)->cmd.sdata.spid=SPID;
            }
            break;
        case 19:
            *commandSize=sizeof(cmpCmdSpContLockBySp_t);
            commandP=malloc(*commandSize);
            if(commandP)
            { 
                memset(commandP,0,*commandSize);
                ((cmpCommandHeaderMap_t*)commandP)->commandId=MC_CMP_CMD_SP_CONT_LOCK_BY_SP;
                ((cmpCmdSpContLockBySp_t*)commandP)->cmd.sdata.spid=SPID;
            }
            break;
        case 20:
            *commandSize=sizeof(cmpCmdSpContUnlockBySp_t);
            commandP=malloc(*commandSize);
            if(commandP)
            { 
                memset(commandP,0,*commandSize);
                ((cmpCommandHeaderMap_t*)commandP)->commandId=MC_CMP_CMD_SP_CONT_UNLOCK_BY_SP;
                ((cmpCmdSpContUnlockBySp_t*)commandP)->cmd.sdata.spid=SPID;                
            }
            break;
        case 21:
            *commandSize=sizeof(cmpCmdTltContPersonalize_t);
            commandP=malloc(*commandSize);
            if(commandP)
            { 
                memset(commandP,0,*commandSize);
                ((cmpCommandHeaderMap_t*)commandP)->commandId=MC_CMP_CMD_TLT_CONT_PERSONALIZE;
                ((cmpCmdTltContPersonalize_t*)commandP)->cmd.sdata.spid=SPID;
                memcpy(&((cmpCmdTltContPersonalize_t*)commandP)->cmd.sdata.uuid,&TLTUUID, sizeof(mcUuid_t));
                memcpy(&((cmpCmdTltContPersonalize_t*)(commandP))->cmd.sdata.ed.edata.dataCont, &DATACONT, 140);
            }
            break;
        case 22:
            *commandSize=sizeof(cmpCmdTltContActivate_t);
            commandP=malloc(*commandSize);
            if(commandP)
            { 
                memset(commandP,0,*commandSize);
                ((cmpCommandHeaderMap_t*)commandP)->commandId=MC_CMP_CMD_TLT_CONT_ACTIVATE;
                ((cmpCmdTltContActivate_t*)commandP)->cmd.sdata.spid=SPID;
                memcpy(&((cmpCmdTltContActivate_t*)commandP)->cmd.sdata.uuid,&TLTUUID, sizeof(mcUuid_t)); 
            }
            break;
        case 23:
            *commandSize=sizeof(cmpCmdTltContLockBySp_t);
            commandP=malloc(*commandSize);
            if(commandP)
            { 
                memset(commandP,0,*commandSize);
                ((cmpCommandHeaderMap_t*)commandP)->commandId=MC_CMP_CMD_TLT_CONT_LOCK_BY_SP;
                ((cmpCmdTltContLockBySp_t*)commandP)->cmd.sdata.spid=SPID;  
                memcpy(&((cmpCmdTltContLockBySp_t*)commandP)->cmd.sdata.uuid,&TLTUUID, sizeof(mcUuid_t));                                
            }
            break;
        case 24:
            *commandSize=sizeof(cmpCmdTltContRegister_t);
            commandP=malloc(*commandSize);
            if(commandP)
            { 
                memset(commandP,0,*commandSize);
                ((cmpCommandHeaderMap_t*)commandP)->commandId=MC_CMP_CMD_TLT_CONT_REGISTER;
                ((cmpCmdTltContRegister_t*)commandP)->cmd.sdata.spid=SPID;
                memcpy(&((cmpCmdTltContRegister_t*)commandP)->cmd.sdata.uuid,&TLTUUID, sizeof(mcUuid_t));                                                
            }
            break;
        case 25:
            *commandSize=sizeof(cmpCmdTltContRegisterActivate_t);
            commandP=malloc(*commandSize);
            if(commandP)
            { 
                memset(commandP,0,*commandSize);
                ((cmpCommandHeaderMap_t*)commandP)->commandId=MC_CMP_CMD_TLT_CONT_REGISTER_ACTIVATE;
                ((cmpCmdTltContRegisterActivate_t*)commandP)->cmd.sdata.spid=SPID;
                memcpy(&((cmpCmdTltContRegisterActivate_t*)commandP)->cmd.sdata.uuid,&TLTUUID, sizeof(mcUuid_t));
            }
            break;
        case 26:
            *commandSize=sizeof(cmpCmdTltContUnlockBySp_t);
            commandP=malloc(*commandSize);
            if(commandP)
            { 
                memset(commandP,0,*commandSize);
                ((cmpCommandHeaderMap_t*)commandP)->commandId=MC_CMP_CMD_TLT_CONT_UNLOCK_BY_SP;
                ((cmpCmdTltContUnlockBySp_t*)commandP)->cmd.sdata.spid=SPID;
                memcpy(&((cmpCmdTltContUnlockBySp_t*)commandP)->cmd.sdata.uuid,&TLTUUID, sizeof(mcUuid_t));
            }
            break;
        case 27:
            *commandSize=sizeof(cmpCmdTltContUnregister_t);
            commandP=malloc(*commandSize);
            if(commandP)
            { 
                memset(commandP,0,*commandSize);
                ((cmpCommandHeaderMap_t*)commandP)->commandId=MC_CMP_CMD_TLT_CONT_UNREGISTER;
                ((cmpCmdTltContUnregister_t*)commandP)->cmd.sdata.spid=SPID;
                memcpy(&((cmpCmdTltContUnregister_t*)commandP)->cmd.sdata.uuid,&TLTUUID, sizeof(mcUuid_t));
            }
            break;
        case 28:
            *commandSize=sizeof(cmpCmdAuthenticateTerminate_t);
            commandP=malloc(*commandSize);
            if(commandP)
            { 
                memset(commandP,0,*commandSize);
                ((cmpCommandHeaderMap_t*)commandP)->commandId=MC_CMP_CMD_AUTHENTICATE_TERMINATE;
            }
            break;
        default:
            break;
    }

    return (uint8_t*) commandP;
}

int createCommands(CmpMessage** commandsP, CmpMessage** responsesP)
{
    int numberOfCommands=0;
    uint8_t* cP=NULL;
    uint32_t commandSize=0;
    while(NULL!=(cP=getCommand(numberOfCommands, &commandSize)))
    {
        *commandsP=(CmpMessage*)realloc(*commandsP, sizeof(CmpMessage)*(numberOfCommands+1));
        memset(&(*commandsP)[numberOfCommands],0,sizeof(CmpMessage));
        (*commandsP)[numberOfCommands].length=commandSize;
        (*commandsP)[numberOfCommands].contentP=cP;
        (*commandsP)[numberOfCommands].hdr.id=numberOfCommands;
        (*commandsP)[numberOfCommands].hdr.ignoreError=true;
        numberOfCommands++;
    }
    
    *responsesP = (CmpMessage*) malloc(sizeof(CmpMessage)*numberOfCommands);
    memset(*responsesP, 0, sizeof(CmpMessage)*numberOfCommands);
    return numberOfCommands;
}

bool checkCmpResults(int numberOfCommands, CmpMessage* responsesP)
{
    bool result=true;
    for(int i=0;i<numberOfCommands;i++)
    {
        uint32_t retCode=0;
        
        if(0==i || 1==i || 4==i)
        {
            *((uint32_t*)responsesP[i].contentP+OLD_RETCODE_INDEX);
        }
        else
        {
            *((uint32_t*)responsesP[i].contentP+RETCODE_INDEX);        
        }
        
        if(responsesP[i].length <= 0 || retCode != 0)
        {
            cout << "==== FAILURE: cmpResult "<< i << "\n";
            result=false;
        }
        else
        {
            cout << "==== SUCCESS: cmpResult "<< i << "\n";        
        }
    }
    return result;
}

void cleanup(int numberOfCommands, CmpMessage* commandsP, CmpMessage* responsesP)
{
    for(int i=0; i<numberOfCommands; i++)
    {
        free(commandsP[i].contentP);
        free(responsesP[i].contentP);
    }
    free(commandsP);
    free(responsesP);
}
