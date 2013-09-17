/*
 * Copyright (c) 2013 TRUSTONIC LIMITED
 * All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef FASTCALL_H_
#define FASTCALL_H_

/** Firmware Driver and Fastcall Handlers
 *
 * Mobicore can have one firmware driver. Such driver is loaded
 * typically at boot, and cannot be unloaded. Firmware driver is
 * intended to act as system integration means, and is not intended
 * to act as typical peripheral driver.
 *
 * Fastcall handlers
 * Firmware driver can register fastcall handler in some platforms.
 * Fastcall handler is called for fastcalls unknown to Mobicore.
 *
 * Fastcall handlers get driver memory mappings in range of 0-2MB
 * at the time handler is installed. Driver must not unmap any of
 * these mappings. If new mappings are made, they cannot be relied
 * to be visible in fastcall hook function.
 *
 * Fastcall handler may map sections (of size 1M). These mappings are not
 * visible to driver.
 *
 * Fastcall handler gets access to number of registers at the time of
 * fastcall was made in NWd. Actual number of registers depends on
 * platform, but is always at least 4 (r0-r3).
 *
 * Notes on use of Fastcall handlers
 * Fastcall handlers
 * - cannot call any TlApi or DrApi functions.
 * - cannot have synchronization with firmware driver.
 * - do not have large stack. If stack is needed, switch to your own.
 * - may be executed concurrently in all CPUs.
 * - must not cause exceptions. There is no means to recover in case
 *   of exception.
 *
 */

#define FCH_L1_MAX  12

typedef word_t *fastcall_registers_t;

/** Context for FCs hook call
 *
 * This is (currently) shared between all processors
 */
struct fcContext {
    word_t size; // Size of context
    // Callback to modify L1 entry.
    // Return the virtual address corresponding to modified entry
    void *(*setL1Entry)(word_t idx, word_t entry);
    word_t registers; // Number of registers available in fastcall
};

/* Fastcall handler initialization
 *
 * Called once before any fastcall.
 * Can map memories for fastcall context.
 * This function must never cause any exceptions.
 * Call is executed in SVC mode.
 *
 * Return value
 *      0 Successfull initialization.
 *      Other cancel hook installation
 */
typedef word_t (*fcInitHook)(
        struct fcContext *context
);


/* Fastcall handler
 *
 * Executed possibly concurrently in all CPUs.
 * This function must never cause any exceptions.
 * Call is executed in IRQ mode.
 * On Entry
 *   fastcall_regs_t[0]...fastcall_regs_t[15] contain input registers.
 * On exit
 *   fastcall_regs_t[0]...fastcall_regs_t[3] contain r0...r3
 *   Registers beyond r3 must not be modified. Result of any
 *   modification is unpredictable.
 *
 */
typedef void (*fcEntryHook)(
        fastcall_registers_t *regs_t,
        struct fcContext *context
);


#endif /* FASTCALL_H_ */
