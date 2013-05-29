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
#include <unistd.h>
#include <stdlib.h>

#include <provisioningagent.h>

#include "cmpHandler.h"

using namespace std;

//
// NOTE! in case you are running this in your machine with apache2 server emulating SE
// you can either modify the apache scripts to contain localhost address or 
// you can run command 
// iptables -t nat -A OUTPUT -d 10.0.2.2 -j DNAT --to-destination 127.0.0.1
// to direct the traffic to localhost. In that cases also the SEADRESS below could
// but does not need to be 10.0.2.2
//
#define SEADDRESS "http://127.0.0.1/"

static const mcSuid_t EXPECTED_SUID = {0,{{
    0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
}}};
static const int EXPECTED_TAG=2;
static const char* EXPECTED_PRODUCT_ID="xxxxx";
static const uint32_t EXPECTED_MCI = 1;
static const uint32_t EXPECTED_SO = 2;
static const uint32_t EXPECTED_MCLF = 3;
static const uint32_t EXPECTED_CONTAINER = 4;
static const uint32_t EXPECTED_CONFIG = 5;
static const uint32_t EXPECTED_TLAPI = 6;
static const uint32_t EXPECTED_DRAPI = 7;
static const uint32_t EXPECTED_CMP = 8;
static const int EXPECTED_SP_CONT_STATE =  MC_CONT_STATE_SP_LOCKED;  // 4
static const int EXPECTED_TLT_CONT_STATE =  MC_CONT_STATE_SP_LOCKED;  // 4
static const mcUuid_t EXPECTED_TLT_ID={{0x03, 0x03, 0x03, 0x03, 0x04, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05, 0x06, 0x06, 0x06, 0x06}};
static const int WAIT_TIMEOUT=15;
static bool testStatus_=true;

#define STATES_DONE_IDX 0
#define STATES_INSTALL_IDX (UNREGISTERING_ROOT_CONTAINER+1)
#define NUMBER_OF_STATES (STATES_INSTALL_IDX+1)

static bool states_[NUMBER_OF_STATES];
void stateCallback(ProvisioningState state, rootpaerror_t error, tltInfo_t* tltInfoP)
{
    cout << "rootpaclient stateCallback: " << state << " : " << error << "\n";
    if(state<=UNREGISTERING_ROOT_CONTAINER)
    {
        states_[state]=true;
    }
    else if (PROVISIONING_STATE_INSTALL_TRUSTLET==state && tltInfoP != NULL)
    {
        states_[STATES_INSTALL_IDX]=true;
    }
    else if (PROVISIONING_STATE_THREAD_EXITING==state)
    {
        states_[STATES_DONE_IDX]=true;
    }
}

bool statesOkProvisioning()
{
    for(int i=0; i<= FINISHED_PROVISIONING; i++)
    {
        if(false==states_[i])
        {
            cout << "==== FAILURE: provisioning state "<<i<<"\n";
            return false;
        }
        
    }
    cout << "==== SUCCESS: provisioning states ok\n";
    return true;
}

bool statesOkInstall()
{
    if(false==states_[STATES_INSTALL_IDX]||false==states_[CONNECTING_SERVICE_ENABLER]||false==states_[STATES_DONE_IDX])
    {
        cout << "==== FAILURE: install states\n";
        return false;
    }
    cout << "==== SUCCESS: install states ok\n";
    return true;
}

bool statesOkDelete()
{
    if(false==states_[UNREGISTERING_ROOT_CONTAINER]||false==states_[CONNECTING_SERVICE_ENABLER]||false==states_[STATES_DONE_IDX])
    {
        cout << "==== FAILURE: delete states\n";
        return false;
    }
    cout << "==== SUCCESS: delete states ok\n";
    return true;    
}

bool waitUntilDone(uint32_t waitTimeSeconds)
{
    while(states_[STATES_DONE_IDX]==false)
    {
        if(waitTimeSeconds==0) break;
        sleep(waitTimeSeconds--);
    }
    return states_[STATES_DONE_IDX];
}

rootpaerror_t sysInfoCallback(osInfo_t* osSpecificInfoP)
{
    osSpecificInfoP->imeiEsnP=(char*)malloc(5);
    strcpy(osSpecificInfoP->imeiEsnP,"imei");

    osSpecificInfoP->mnoP=(char*)malloc(5);
    strcpy(osSpecificInfoP->mnoP,"mno");

    osSpecificInfoP->brandP=(char*)malloc(5);
    strcpy(osSpecificInfoP->brandP,"trus");        

    osSpecificInfoP->manufacturerP=(char*)malloc(5);
    strcpy(osSpecificInfoP->manufacturerP,"toni");        

    osSpecificInfoP->hardwareP=(char*)malloc(5);
    strcpy(osSpecificInfoP->hardwareP,"c");        

    osSpecificInfoP->modelP=(char*)malloc(5);
    strcpy(osSpecificInfoP->modelP,"1234");        

    osSpecificInfoP->versionP=(char*)malloc(5);
    strcpy(osSpecificInfoP->versionP,"0300");  
    return ROOTPA_OK;
}

bool checkResult(bool result, const char* testCase)
{
    if(false==result)
    {
        cout << "==== FAILURE: "<< testCase << "\n";
        testStatus_ = false;
    }
    else
    {
        cout << "==== SUCCESS: "<< testCase << "\n";
    }
    return result;
}

bool checkResult(rootpaerror_t retCode, const char* testCase)
{
    return checkResult((ROOTPA_OK==retCode), testCase);
}

int main(int argc, char* argv[] )
{   
//    printSizes();
    setPaths(".", ".");
    checkResult(setSeAddress(SEADDRESS, sizeof(SEADDRESS)),"setSeAddress");
    
    checkResult(openSessionToCmtl(), "openSessionToCmtl");

    mcSuid_t suid;
    memset(&suid, 0, sizeof(mcSuid_t));
    if(checkResult(getSuid(&suid), "getSuid"))
    {
        checkResult((memcmp(&suid, &EXPECTED_SUID, sizeof(mcSuid_t))==0), "getSuid, check suid");
    }
    
    int tag=0;
    mcVersionInfo_t version;
    memset(&version,0,sizeof(mcVersionInfo_t));
    if(checkResult(getVersion(&tag, &version),"getVersion"))
    {
        checkResult(EXPECTED_TAG==tag, "getVersion, tag");
        checkResult(memcmp(EXPECTED_PRODUCT_ID, &version.productId, strlen(EXPECTED_PRODUCT_ID))==0, "getVersion, product id");
        checkResult(EXPECTED_MCI==version.versionMci, "getVersion, mci");
        checkResult(EXPECTED_SO==version.versionSo, "getVersion, so");
        checkResult(EXPECTED_MCLF==version.versionMclf, "getVersion, mclf");
        checkResult(EXPECTED_CONTAINER==version.versionContainer, "getVersion, container");
        checkResult(EXPECTED_CONFIG==version.versionMcConfig, "getVersion, config");
        checkResult(EXPECTED_TLAPI==version.versionTlApi, "getVersion, tlapi");
        checkResult(EXPECTED_DRAPI==version.versionDrApi, "getVersion, drapi");
        checkResult(EXPECTED_CMP==version.versionCmp, "getVersion, cmp");
    }

    bool isRegistered=false;
    if(checkResult(isRootContainerRegistered(&isRegistered), "isRootContainerRegistered"))
    {
        checkResult(true==isRegistered, "isRootContainerRegistered, isRegistered");
    }

    mcSpid_t spid = 8;
    isRegistered=false;
    if(checkResult(isSpContainerRegistered(spid, &isRegistered),"isSpContainerRegistered"))
    {
        checkResult(true==isRegistered, "isSpContainerRegistered, isRegistered");
    }

    mcContainerState_t state;
    if(checkResult(getSpContainerState(spid, &state),"getSpContainerState"))
    {
        checkResult(EXPECTED_SP_CONT_STATE==state, "getSpContainerState, state");
    }

    SpContainerStructure spContainerStructure;
    memset(&spContainerStructure,0,sizeof(SpContainerStructure));
    if(checkResult(getSpContainerStructure(spid, &spContainerStructure),"getSpContainerStructure"))
    {
        checkResult(EXPECTED_SP_CONT_STATE==spContainerStructure.state,"getSpContainerStructure, sp state ");
        checkResult(memcmp(&EXPECTED_TLT_ID, &spContainerStructure.tltContainers[0].uuid, 16)==0,"getSpContainerStructure, tlt id 0");
        checkResult(EXPECTED_TLT_CONT_STATE==spContainerStructure.tltContainers[0].state,"getSpContainerStructure, tlt state 0");
        checkResult(memcmp(&EXPECTED_TLT_ID, &spContainerStructure.tltContainers[1].uuid, 16)==0,"getSpContainerStructure, tlt id 1");
        checkResult(EXPECTED_TLT_CONT_STATE==spContainerStructure.tltContainers[1].state,"getSpContainerStructure, tlt state 1");
    }

    memset(states_,0,NUMBER_OF_STATES);
    if(checkResult(doProvisioning(spid, stateCallback, sysInfoCallback),"doProvisioning"))
    {
        checkResult(waitUntilDone(WAIT_TIMEOUT), "doProvisioning, waitUntilDone");
        if(!statesOkProvisioning())
        {
            testStatus_=false;
        }
    }

    memset(states_,0,NUMBER_OF_STATES);
    if(checkResult(unregisterRootContainer(stateCallback, sysInfoCallback),"unregisterRootContainer"))
    {
        checkResult(waitUntilDone(WAIT_TIMEOUT), "unregisterRootContainer, waitUntilDone");
        if(!statesOkDelete())
        {
            testStatus_=false;
        }
    }

    trustletInstallationData_t tltData;
    memset(&tltData, 0, sizeof(tltData));
    tltData.dataP=(uint8_t*)"AAAAAAAAAABBBBBBBBBBCCCCCCCCCC";
    tltData.dataLength=30;
    tltData.dataType=REQUEST_DATA_TLT;
    memcpy(&tltData.uuid, &EXPECTED_TLT_ID,sizeof(EXPECTED_TLT_ID));    
    tltData.minTltVersion=1;
    tltData.tltPukHashP=(uint8_t*)"DDDDDDDDDDEEEEEEEEEEFFFFFFFFFF00";
    tltData.tltPukHashLength=32;
    tltData.memoryType=2;
    tltData.numberOfInstances=1;
    tltData.flags=0;

    
    memset(states_,0,NUMBER_OF_STATES);
    if(checkResult(installTrustlet(spid, stateCallback, sysInfoCallback, &tltData),"installTrustlet tlt"))
    {
        checkResult(waitUntilDone(WAIT_TIMEOUT), "installTrustlet tlt, waitUntilDone");
        if(!statesOkInstall())
        {
            testStatus_=false;
        }            
    }

    tltData.dataType=REQUEST_DATA_KEY; // since we are not dealing with real data we use the same for both
    memset(states_,0,NUMBER_OF_STATES);
    if(checkResult(installTrustlet(spid, stateCallback, sysInfoCallback, &tltData),"installTrustlet key"))
    {
        checkResult(waitUntilDone(WAIT_TIMEOUT), "installTrustlet key, waitUntilDone");
        if(!statesOkProvisioning()) // not checking install state since the tlt will not be sent back
        {
            testStatus_=false;
        }            
    }

    CmpMessage* commandsP=NULL;
    CmpMessage* responsesP=NULL;
    uint32_t internalError=0;
    int numberOfCommands=createCommands(&commandsP, &responsesP);
    if(checkResult( executeCmpCommands(numberOfCommands, commandsP, responsesP, &internalError),"executeCmpCommands" ))
    {
        checkResult( 0==internalError,"executeCmpCommands, internalError");
        if(checkCmpResults(numberOfCommands, responsesP)==false)
        {
            testStatus_=false;
        }            
    }
    cleanup(numberOfCommands, commandsP, responsesP);
    
    closeSessionToCmtl();
    cout << "========================================\n";
    if(true == testStatus_)
    {
        cout << "==== SUCCESS: all tests passed\n";
    }
    else
    {
        cout << "==== FAILURE: some of the tests failed\n";
    }
    cout << "========================================\n";
    return 0;
}
