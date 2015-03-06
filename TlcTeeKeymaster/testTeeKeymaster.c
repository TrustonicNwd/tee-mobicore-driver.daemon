/*
 * Copyright (c) 2013-2014 TRUSTONIC LIMITED
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the TRUSTONIC LIMITED nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "MobiCoreDriverApi.h"
#include "tlTeeKeymaster_Api.h"
#include "tlTeeKeymaster_log.h"
#include "tlcTeeKeymaster_if.h"

#include <openssl/dsa.h>
#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>
#include <openssl/rsa.h>

#undef  LOG_TAG
#define LOG_TAG "TlcTeeKeyMasterTest"

#define BYTES_PER_BITS(bits) (((bits-1) >> 3)+1)

/* Exponent number */
#define RSA_DEFAULT_EXPONENT 65537

/* Max SO size for RSA key data*/
#define RSA_KEY_SO_SIZE 4096

/* Max SO size for DSA key data*/
#define DSA_KEY_SO_SIZE 2048

/* Max SO size for ECDSA key data*/
#define ECDSA_KEY_SO_SIZE 512

/* Max EC key data size (66 bytes for P-521) */
#define EC_MAX_CURVE_SIZE 66


/* Curve sizes */
const uint32_t  SECP192R1_BITS = 192;
const uint32_t  SECP224R1_BITS = 224;
const uint32_t  SECP256R1_BITS = 256;
const uint32_t  SECP384R1_BITS = 384;
const uint32_t  SECP521R1_BITS = 521;


uint8_t hash_data[] =
        {
            0xf7, 0xc3, 0xbc, 0x1d, 0x80, 0x8e, 0x04, 0x73,
            0x2a, 0xdf, 0x67, 0x99, 0x65, 0xcc, 0xc3, 0x4c,
            0xa7, 0xae, 0x34, 0x41
        };

uint8_t p[] =
        {
           0x8d, 0xf2, 0xa4, 0x94, 0x49, 0x22, 0x76, 0xaa,
           0x3d, 0x25, 0x75, 0x9b, 0xb0, 0x68, 0x69, 0xcb,
           0xea, 0xc0, 0xd8, 0x3a, 0xfb, 0x8d, 0x0c, 0xf7,
           0xcb, 0xb8, 0x32, 0x4f, 0x0d, 0x78, 0x82, 0xe5,
           0xd0, 0x76, 0x2f, 0xc5, 0xb7, 0x21, 0x0e, 0xaf,
           0xc2, 0xe9, 0xad, 0xac, 0x32, 0xab, 0x7a, 0xac,
           0x49, 0x69, 0x3d, 0xfb, 0xf8, 0x37, 0x24, 0xc2,
           0xec, 0x07, 0x36, 0xee, 0x31, 0xc8, 0x02, 0x91
        };

uint8_t q[] =
        {
           0xc7, 0x73, 0x21, 0x8c, 0x73, 0x7e, 0xc8, 0xee,
           0x99, 0x3b, 0x4f, 0x2d, 0xed, 0x30, 0xf4, 0x8e,
           0xda, 0xce, 0x91, 0x5f
        };

uint8_t g[] =
        {
           0x62, 0x6d, 0x02, 0x78, 0x39, 0xea, 0x0a, 0x13,
           0x41, 0x31, 0x63, 0xa5, 0x5b, 0x4c, 0xb5, 0x00,
           0x29, 0x9d, 0x55, 0x22, 0x95, 0x6c, 0xef, 0xcb,
           0x3b, 0xff, 0x10, 0xf3, 0x99, 0xce, 0x2c, 0x2e,
           0x71, 0xcb, 0x9d, 0xe5, 0xfa, 0x24, 0xba, 0xbf,
           0x58, 0xe5, 0xb7, 0x95, 0x21, 0x92, 0x5c, 0x9c,
           0xc4, 0x2e, 0x9f, 0x6f, 0x46, 0x4b, 0x08, 0x8c,
           0xc5, 0x72, 0xaf, 0x53, 0xe6, 0xd7, 0x88, 0x02
        };

uint8_t x[] =
        {
           0x20, 0x70, 0xb3, 0x22, 0x3d, 0xba, 0x37, 0x2f,
           0xde, 0x1c, 0x0f, 0xfc, 0x7b, 0x2e, 0x3b, 0x49,
           0x8b, 0x26, 0x06, 0x14
        };

uint8_t y[] =
        {
           0x19, 0x13, 0x18, 0x71, 0xd7, 0x5b, 0x16, 0x12,
           0xa8, 0x19, 0xf2, 0x9d, 0x78, 0xd1, 0xb0, 0xd7,
           0x34, 0x6f, 0x7a, 0xa7, 0x7b, 0xb6, 0x2a, 0x85,
           0x9b, 0xfd, 0x6c, 0x56, 0x75, 0xda, 0x9d, 0x21,
           0x2d, 0x3a, 0x36, 0xef, 0x16, 0x72, 0xef, 0x66,
           0x0b, 0x8c, 0x7c, 0x25, 0x5c, 0xc0, 0xec, 0x74,
           0x85, 0x8f, 0xba, 0x33, 0xf4, 0x4c, 0x06, 0x69,
           0x96, 0x30, 0xa7, 0x6b, 0x03, 0x0e, 0xe3, 0x33
        };

uint8_t dsa_digest[] =
        {
            0xa9, 0x99, 0x3e, 0x36, 0x47, 0x06, 0x81, 0x6a,
            0xba, 0x3e, 0x25, 0x71, 0x78, 0x50, 0xc2, 0x6c,
            0x9c, 0xd0, 0xd8, 0x9d
        };


uint8_t dsa_signature_g[] =
        {
           0x8b, 0xac, 0x1a, 0xb6, 0x64, 0x10, 0x43, 0x5c,
           0xb7, 0x18, 0x1f, 0x95, 0xb1, 0x6a, 0xb9, 0x7c,
           0x92, 0xb3, 0x41, 0xc0, 0x41, 0xe2, 0x34, 0x5f,
           0x1f, 0x56, 0xdf, 0x24, 0x58, 0xf4, 0x26, 0xd1,
           0x55, 0xb4, 0xba, 0x2d, 0xb6, 0xdc, 0xd8, 0xc8
        };


uint8_t test_digest_sha512[] =
        {
			0xee, 0x26, 0xb0, 0xdd, 0x4a, 0xf7, 0xe7, 0x49,
			0xaa, 0x1a, 0x8e, 0xe3, 0xc1, 0x0a, 0xe9, 0x92,
			0x3f, 0x61, 0x89, 0x80, 0x77, 0x2e, 0x47, 0x3f,
			0x88, 0x19, 0xa5, 0xd4, 0x94, 0x0e, 0x0d, 0xb2,
			0x7a, 0xc1, 0x85, 0xf8, 0xa0, 0xe1, 0xd5, 0xf8,
			0x4f, 0x88, 0xbc, 0x88, 0x7f, 0xd6, 0x7b, 0x14,
			0x37, 0x32, 0xc3, 0x04, 0xcc, 0x5f, 0xa9, 0xad,
			0x8e, 0x6f, 0x57, 0xf5, 0x00, 0x28, 0xa8, 0xff
        };


teeResult_t testRsa(
        uint32_t length,
        bool usecrt)
{
    teeResult_t  ret     = TEE_ERR_FAIL;
    BIGNUM       *e      = NULL;
    RSA          *rsa    = NULL;
    uint8_t      *buffer = NULL;
    uint8_t      *cipher = NULL;
    uint8_t      *mod    = NULL;
    uint8_t      *msg    = NULL;
    uint8_t      *output = NULL;
    uint8_t      *pub_exp = NULL;
    uint8_t      *pri_exp = NULL;
    uint8_t      *rsa_key    = NULL;
    uint8_t      *rsa_key_so = NULL;
    teeKeyMeta_t keymeta     = {0};
    uint32_t     rsa_key_len    = 0;
    uint32_t     rsa_key_so_len = 0;
    uint32_t     msg_len     = 0;
    uint32_t     cipher_len  = 0;
    uint32_t     rsa_size    = 0;
    uint32_t     modlen      = 0;
    uint32_t     pub_exp_len = 0;
    uint32_t     pri_exp_len = 0;
    uint32_t     offset      = 0;
    bool         validity    = false;
    /* CRT parameters */
    uint8_t      *p      = NULL;
    uint8_t      *q      = NULL;
    uint8_t      *dp     = NULL;
    uint8_t      *dq     = NULL;
    uint8_t      *qinv   = NULL;
    uint32_t     plen    = 0;
    uint32_t     qlen    = 0;
    uint32_t     dplen   = 0;
    uint32_t     dqlen   = 0;
    uint32_t     qinvlen = 0;

    LOG_I("RSA test - key size=%d crt=%d", length, usecrt);

    do
    {
        rsa = RSA_new();
        if (rsa == NULL)
        {
            break;
        }

        e = BN_new();
        if (e== NULL)
        {
            break;
        }

        /* Set public exponent*/
        BN_set_word(e, 65537);

        /* Generate RSA key */
        if(!RSA_generate_key_ex(
                        rsa,
                        length,
                        e,
                        NULL))
        {
            break;
        }

        rsa_size = RSA_size(rsa);

        /* Allocate enough memory for the buffers */
        buffer = malloc(rsa_size*12);
        if (buffer == NULL)
        {
            break;
        }

        mod     = buffer;
        pub_exp = buffer + rsa_size;
        pri_exp = buffer + rsa_size*2;
        cipher  = buffer + rsa_size*3;
        msg     = buffer + rsa_size*4;
        output  = buffer + rsa_size*5;
        p       = buffer + rsa_size*6;
        q       = buffer + rsa_size*7;
        dp      = buffer + rsa_size*8;
        dq      = buffer + rsa_size*9;
        qinv    = buffer + rsa_size*10;

        cipher_len = msg_len = rsa_size;

        rsa_key = malloc(RSA_KEY_SO_SIZE);
        if (rsa_key == NULL)
        {
            break;
        }

        rsa_key_so = malloc(RSA_KEY_SO_SIZE);
        if (rsa_key_so == NULL)
        {
            break;
        }

        modlen = BN_bn2bin(rsa->n, mod);
        pub_exp_len = BN_bn2bin(rsa->e, pub_exp);

        memset(&keymeta, 0x0, sizeof(teeKeyMeta_t));
        keymeta.keytype = TEE_KEYTYPE_RSA;
        keymeta.rsakey.keysize = length;
        keymeta.rsakey.lenpubmod = modlen;
        keymeta.rsakey.lenpubexp = pub_exp_len;
        if(usecrt)
        {
            plen = BN_bn2bin(rsa->p, p);
            qlen = BN_bn2bin(rsa->q, q);
            dplen   = BN_bn2bin(rsa->dmp1, dp);
            dqlen   = BN_bn2bin(rsa->dmq1, dq);
            qinvlen = BN_bn2bin(rsa->iqmp, qinv);
            keymeta.rsakey.type = TEE_KEYPAIR_RSACRT;
            keymeta.rsakey.rsacrtpriv.lenp      = plen;
            keymeta.rsakey.rsacrtpriv.lenq      = qlen;
            keymeta.rsakey.rsacrtpriv.lendp     = dplen;
            keymeta.rsakey.rsacrtpriv.lendq     = dqlen;
            keymeta.rsakey.rsacrtpriv.lenqinv   = qinvlen;
        }
        else
        {
            pri_exp_len = BN_bn2bin(rsa->d, pri_exp);
            keymeta.rsakey.type = TEE_KEYPAIR_RSA;
            keymeta.rsakey.rsapriv.lenpriexp = pri_exp_len;
        }

        /*

         * RSA key data:
         * |--key metadata--|--public modulus--|--public exponent--|--private exponent--|
         *
         * RSA CRT key data:
         * |--key metadata--|--public modulus--|--public exponent--|--P--|--Q--|--DP--|--DQ--|--Qinv--|
         */
        memcpy(rsa_key, &keymeta, sizeof(teeKeyMeta_t));
        offset += sizeof(teeKeyMeta_t);
        memcpy(rsa_key+offset, mod, modlen);
        offset += modlen;
        memcpy(rsa_key+offset, pub_exp, pub_exp_len);
        offset += pub_exp_len;
        if(usecrt)
        {
            memcpy(rsa_key+offset, p, plen);
            offset += plen;
            memcpy(rsa_key+offset, q, qlen);
            offset += qlen;
            memcpy(rsa_key+offset, dp, dplen);
            offset += dplen;
            memcpy(rsa_key+offset, dq, dqlen);
            offset += dqlen;
            memcpy(rsa_key+offset, qinv, qinvlen);
            rsa_key_len = sizeof(teeKeyMeta_t) +
                            modlen +
                            pub_exp_len +
                            plen + qlen + dplen + dqlen + qinvlen;
        }
        else
        {
            memcpy(rsa_key+offset, pri_exp, pri_exp_len);
            rsa_key_len = sizeof(teeKeyMeta_t) +
                            modlen +
                            pub_exp_len +
                            pri_exp_len;
        }

        rsa_key_so_len = RSA_KEY_SO_SIZE;

        ret = TEE_KeyImport(
                rsa_key,
                rsa_key_len,
                rsa_key_so,
                &rsa_key_so_len);
        if (TEE_ERR_NONE != ret)
        {
            LOG_E("TEE_KeyImport failed: %d\n", ret);
            break;
        }

        memset(msg, 0x12, msg_len);
        if (RSA_private_decrypt(
                rsa_size,
                msg,
                cipher,
                rsa,
                RSA_NO_PADDING) != rsa_size)
        {
            LOG_E("RSA_private_decrypt failed\n");
            break;
        }

        /* Verify data */
        ret = TEE_RSAVerify(
                rsa_key_so,
                rsa_key_so_len,
                msg,
                msg_len,
                cipher,
                cipher_len,
                TEE_RSA_NODIGEST_NOPADDING,
                &validity);
        if ((TEE_ERR_NONE != ret) || (!validity))
        {
            LOG_E("TEE_RSAVerify failed. ret=%x  validity=%d\n", ret, validity);
            break;
        }

        /* Now do the opposite */
        memset(cipher, 0x0, cipher_len);

        /* Sign data */
        ret = TEE_RSASign(
                rsa_key_so,
                rsa_key_so_len,
                msg,
                msg_len,
                cipher,
                &cipher_len,
                TEE_RSA_NODIGEST_NOPADDING);
        if (TEE_ERR_NONE != ret)
        {
            LOG_E("TEE_RSASign failed. ret=%x\n", ret);
            break;
        }

        if (cipher_len != rsa_size)
        {
            LOG_E("Invalid signature length=%d\n", cipher_len);
            break;
        }

        if (rsa_size != RSA_public_encrypt(
                rsa_size,
                cipher,
                output,
                rsa,
                RSA_NO_PADDING))
        {
            LOG_E("RSA_public_encrypt failed\n");
            break;
        }

        if (memcmp(msg, output, cipher_len) != 0)
        {
            LOG_E("Messages mismatch\n");
            ret = TEE_ERR_FAIL;
            break;
        }

        ret = TEE_ERR_NONE;

    } while(false);

    if (rsa_key_so) free(rsa_key_so);
    if (rsa_key)    free(rsa_key);
    if (buffer)     free(buffer);
    BN_free(e);
    RSA_free(rsa);

    return ret;
}


teeResult_t testDsa(
        uint32_t bits)
{
    teeResult_t     ret  = TEE_ERR_FAIL;
    DSA             *dsa = NULL;
    int             counter = 0;
    unsigned long   h;
    uint8_t         pub_key[DSA_KEY_SO_SIZE] = {0};
    uint8_t         dsa_sig[128] = {0};
    uint8_t         p[384] = {0};
    uint8_t         q[64]  = {0};
    uint8_t         g[384] = {0};
    uint8_t         x[64]  = {0};
    uint8_t         y[384] = {0};
    uint32_t        p_len;
    uint32_t        q_len;
    uint32_t        g_len;
    uint32_t        x_len;
    uint32_t        y_len;
    uint32_t        r_len;
    uint32_t        s_len;
    teeKeyMeta_t    keymeta    = {0};
    teeDsaParams_t  dsa_params = {0};
    uint8_t         *dsa_key    = NULL;
    uint8_t         *dsa_key_so = NULL;
    uint32_t        dsa_key_len = 0;
    uint32_t        dsa_key_so_len = 0;
    uint32_t        dsa_sig_len = sizeof(dsa_sig);
    uint32_t        pub_key_len = sizeof(pub_key);
    teePubKeyMeta_t  *pub_meta  = NULL;
    uint8_t         *ptr_pub_key      = NULL;
    DSA_SIG         *dsa_signature_01 = NULL;
    DSA_SIG         *dsa_signature_02 = NULL;
    size_t          digest_len  = 0;
    uint8_t         *digest     = NULL;
    bool            validity    = false;
    uint32_t        offset      = 0;
    const uint8_t   digest_sha1[] = {
                    0xaa, 0xf4, 0xc6, 0x1d, 0xdc, 0xc5, 0xe8, 0xa2,
                    0xda, 0xbe, 0xde, 0x0f, 0x3b, 0x48, 0x2c, 0xd9,
                    0xae, 0xa9, 0x43, 0x4d
                    };

    const uint8_t   digest_sha256[] = {
                    0x2c, 0xf2, 0x4d, 0xba, 0x5f, 0xb0, 0xa3, 0x0e,
                    0x26, 0xe8, 0x3b, 0x2a, 0xc5, 0xb9, 0xe2, 0x9e,
                    0x1b, 0x16, 0x1e, 0x5c, 0x1f, 0xa7, 0x42, 0x5e,
                    0x73, 0x04, 0x33, 0x62, 0x93, 0x8b, 0x98, 0x24
                    };

    do
    {
        LOG_I("Generating DSA key pair. Bit size %d..", bits);

        dsa = DSA_new();
        if (dsa == NULL)
        {
            LOG_E("Cannot allocate DSA key structure\n");
            break;
        }

        if (!DSA_generate_parameters_ex(
                dsa,
                bits,
                NULL,
                0,
                &counter,
                &h,
                NULL))
        {
            LOG_E("DSA_generate_parameters_ex failed\n");
            break;
        }

        p_len = BN_bn2bin(dsa->p, p);
        q_len = BN_bn2bin(dsa->q, q);
        g_len = BN_bn2bin(dsa->g, g);

        dsa_key = (uint8_t*)malloc(DSA_KEY_SO_SIZE);
        if (!dsa_key)
        {
            break;
        }

        dsa_key_so = (uint8_t*)malloc(DSA_KEY_SO_SIZE);
        if (!dsa_key_so)
        {
            break;
        }

        /* Initialize buffer */
        memset(dsa_key, 0, DSA_KEY_SO_SIZE);

        /* Set parameters */
        dsa_params.p = (uint8_t*)&p[0];
        dsa_params.q = (uint8_t*)&q[0];
        dsa_params.g = (uint8_t*)&g[0];
        dsa_params.pLen = p_len;
        dsa_params.qLen = q_len;
        dsa_params.gLen = g_len;
        dsa_params.xLen = q_len; /* Same as q length*/
        dsa_params.yLen = p_len; /* Same as p length*/
        ret = TEE_DSAGenerateKeyPair(
                dsa_key,
                DSA_KEY_SO_SIZE,
                &dsa_params,
                &dsa_key_len);
        if (TEE_ERR_NONE != ret)
        {
            LOG_E("TEE_DSAGenerateKeyPair failed: %d\n", ret);
            break;
        }

        ret = TEE_GetKeyInfo(
                dsa_key,
                dsa_key_len,
                &keymeta);
        if (TEE_ERR_NONE != ret)
        {
            LOG_E("TEE_GetKeyInfo failed: %d\n", ret);
            break;
        }

        if (keymeta.keytype != TEE_KEYTYPE_DSA)
        {
            LOG_E("Invalid key type = %d\n", keymeta.keytype);
            break;
        }

        ret = TEE_GetPubKey(
                dsa_key,
                dsa_key_len,
                pub_key,
                &pub_key_len);
        if (TEE_ERR_NONE != ret)
        {
            LOG_E("TEE_GetPubKey failed: %d\n", ret);
            break;
        }

        pub_meta = (teePubKeyMeta_t *)pub_key;

        if (bits == 1024)
        {
            digest = (uint8_t *)&digest_sha1[0];
            digest_len = sizeof(digest_sha1);
        }
        else if (bits == 2048)
        {
            digest = (uint8_t *)&digest_sha256[0];
            digest_len = sizeof(digest_sha256);
        }
        else
        {
            /* For 3072 bits. 64 bytes digest will be reduced to 32 bytes */
            digest = (uint8_t *)&test_digest_sha512[0];
            digest_len = sizeof(test_digest_sha512);
        }

        LOG_I("Signing data with TEE_DSASign()");
        ret = TEE_DSASign(
                dsa_key,
                dsa_key_len,
                digest,
                digest_len,
                dsa_sig,
                &dsa_sig_len);
        if (TEE_ERR_NONE != ret)
        {
            LOG_E("TEE_DSASign failed: %d\n", ret);
            break;
        }


        LOG_I("Verifying DSA signature with TEE_DSAVerify()");
        ret = TEE_DSAVerify(
                dsa_key,
                dsa_key_len,
                digest,
                digest_len,
                dsa_sig,
                dsa_sig_len,
                &validity);
        if (TEE_ERR_NONE != ret)
        {
            LOG_E("TEE_DSAVerify failed: %d\n", ret);
            break;
        }

        LOG_I("DSA signature validity: %d\n", validity);

        ptr_pub_key = pub_key +
                      sizeof(teePubKeyMeta_t) +
                      pub_meta->dsakey.pLen +
                      pub_meta->dsakey.qLen +
                      pub_meta->dsakey.gLen;

        dsa_signature_01 = DSA_SIG_new();
        if (dsa_signature_01 == NULL)
        {
            LOG_E("Cannot allocate DSA signature structure\n");
            break;
        }

        size_t len = dsa_sig_len/2;
        dsa->pub_key     = BN_bin2bn(ptr_pub_key, pub_meta->dsakey.yLen, NULL);
        dsa_signature_01->r = BN_bin2bn(dsa_sig,len, NULL);
        dsa_signature_01->s = BN_bin2bn(dsa_sig+len, len, NULL);

        if ((dsa->pub_key == NULL) ||
            (dsa_signature_01->r == NULL) ||
            (dsa_signature_01->s == NULL))
        {
            LOG_E("Cannot allocate bignum for signature data\n");
            break;
        }

        LOG_I("Verifying DSA signature with OpenSSL APIs");
        if (DSA_do_verify(digest, digest_len, dsa_signature_01, dsa) != 1)
        {
            LOG_E("DSA signature not valid\n");
            break;
        }

        LOG_I("Generating DSA key with OpenSSL APIs");
        if (DSA_generate_key(dsa) != 1)
        {
            LOG_E("DSA key generation failed\n");
            break;
        }

        LOG_I("Generating DSA signature with OpenSSL APIs");
        if ((dsa_signature_02 = DSA_do_sign(digest, digest_len, dsa)) == NULL)
        {
            LOG_E("Cannot generate DSA signature\n");
            break;
        }

        LOG_I("Verifying DSA signature with OpenSSL APIs");
        if (DSA_do_verify(digest, digest_len, dsa_signature_02, dsa) != 1)
        {
            LOG_E("DSA signature not valid\n");
            break;
        }

        LOG_I("DSA signature is valid");

        /* Extract x and y form DSA key data */
        y_len = BN_bn2bin(dsa->pub_key, y);
        x_len = BN_bn2bin(dsa->priv_key, x);

        /* Initialize key and signature buffer */
        memset(dsa_sig,    0, sizeof(dsa_sig));
        memset(dsa_key,    0, DSA_KEY_SO_SIZE);
        memset(dsa_key_so, 0, DSA_KEY_SO_SIZE);
        dsa_key_so_len  = DSA_KEY_SO_SIZE;


        memset(&keymeta, 0x0, sizeof(teeKeyMeta_t));
        keymeta.keytype = TEE_KEYTYPE_DSA;
        keymeta.dsakey.pLen = p_len;
        keymeta.dsakey.qLen = q_len;
        keymeta.dsakey.gLen = g_len;
        keymeta.dsakey.xLen = x_len;
        keymeta.dsakey.yLen = y_len;

        dsa_key_len = sizeof(teeKeyMeta_t) +
                    keymeta.dsakey.pLen +
                    keymeta.dsakey.qLen +
                    keymeta.dsakey.gLen +
                    keymeta.dsakey.xLen +
                    keymeta.dsakey.yLen;

        offset = 0;
        memcpy(dsa_key, &keymeta, sizeof(teeKeyMeta_t));
        offset += sizeof(teeKeyMeta_t);
        memcpy(dsa_key + offset, &p[0], p_len);
        offset += p_len;
        memcpy(dsa_key + offset, &q[0], q_len);
        offset += q_len;
        memcpy(dsa_key + offset, &g[0], g_len);
        offset += g_len;
        memcpy(dsa_key + offset, &y[0], y_len);
        offset += y_len;
        memcpy(dsa_key + offset, &x[0], y_len);

        /* Import key */
        ret = TEE_KeyImport(
                dsa_key,
                dsa_key_len,
                dsa_key_so,
                &dsa_key_so_len);
        if (TEE_ERR_NONE != ret)
        {
            LOG_E("TEE_KeyImport failed: %d\n", ret);
            break;
        }

        LOG_I("Imported DSA key data length=%d", dsa_key_so_len);

        /* Populate signature buffer */

        r_len = BN_bn2bin(dsa_signature_02->r, dsa_sig);
        s_len = BN_bn2bin(dsa_signature_02->s, dsa_sig+r_len);
        dsa_sig_len = r_len + s_len;

        validity = false;

        LOG_I("Verifying DSA signature (generated by OpenSSL) with TEE_DSAVerify()");
        ret = TEE_DSAVerify(
                dsa_key_so,
                dsa_key_so_len,
                digest,
                digest_len,
                dsa_sig,
                dsa_sig_len,
                &validity);
        if ((TEE_ERR_NONE != ret) || (!validity))
        {
            LOG_E("TEE_DSAVerify failed: %d\n", ret);
            break;
        }

        LOG_I("DSA signature is valid\n");

        ret = TEE_ERR_NONE;

    } while(false);

    if (dsa_key)           free(dsa_key);
    if (dsa_key_so)        free(dsa_key_so);
    if (dsa_signature_01)  DSA_SIG_free(dsa_signature_01);
    if (dsa_signature_02)  DSA_SIG_free(dsa_signature_02);
    if (dsa)               DSA_free(dsa);

    return ret;
}


teeResult_t testEcdsa(
        teeEccCurveType_t   curveType,
        uint32_t            opensslCurveType
        )
{
    teeResult_t   ret               = TEE_ERR_FAIL;
    uint8_t       key_data[1024]    = {0};
    uint32_t      key_data_len      = sizeof(key_data);
    uint8_t       ecdsa_sig[1024]   = {0};
    uint32_t      so_data_len       = 0;
    uint32_t      ecdsa_sig_len     = sizeof(ecdsa_sig);
    uint32_t      hash_len          = sizeof(hash_data);
    bool          validity          = false;
    EC_KEY        *eckey_01         = NULL;
    EC_KEY        *eckey_02         = NULL;
    ECDSA_SIG     *ecdsa_signature_01 = NULL;
    ECDSA_SIG     *ecdsa_signature_02  = NULL;
    BIGNUM        *x_bn             = NULL;
    BIGNUM        *y_bn             = NULL;
    uint8_t       x[EC_MAX_CURVE_SIZE]    = {0};
    uint8_t       y[EC_MAX_CURVE_SIZE]    = {0};
    uint8_t       priv[EC_MAX_CURVE_SIZE] = {0};
    uint8_t       *ptr_x            = &x[0];
    uint8_t       *ptr_y            = &y[0];
    uint8_t       *ptr_priv         = &priv[0];
    uint32_t      x_len             = 0;
    uint32_t      y_len             = 0;
    uint32_t      priv_len          = 0;
    teeKeyMeta_t  keymeta           = {0};
    uint8_t       *ec_pub_key       = NULL;
    uint8_t       *ecdsa_key        = NULL;
    uint8_t       *ecdsa_key_so     = NULL;
    uint32_t      ecdsa_key_len     = 0;
    uint32_t      ecdsa_key_so_len  = 0;
    uint32_t      offset            = 0;
    const EC_POINT  *pubkey         = NULL;
    const EC_GROUP  *group_01       = NULL;
    uint32_t      curve_size        = 0;
    uint32_t      r_len 			= 0;
    uint32_t      s_len				= 0;


    do
    {
        LOG_I("Generating ECDSA key pair..");

        /* Generate EC key pair */
        ret = TEE_ECDSAGenerateKeyPair(
                key_data,
                key_data_len,
                curveType,
                &so_data_len);
        if (TEE_ERR_NONE != ret)
        {
            LOG_E("TEE_ECDSAGenerateKeyPair failed: %d\n", ret);
            break;
        }

        ret = TEE_GetKeyInfo(
                key_data,
                so_data_len,
                &keymeta);
        if (TEE_ERR_NONE != ret)
        {
            LOG_E("TEE_GetKeyInfo failed: %d\n", ret);
            break;
        }

        if (keymeta.keytype != TEE_KEYTYPE_ECDSA)
        {
            LOG_E("Invalid key type = %d\n", keymeta.keytype);
            break;
        }

        LOG_I("Signing data with ECDSA..");
        ret = TEE_ECDSASign(
                key_data,
                so_data_len,
                hash_data,
                hash_len,
                ecdsa_sig,
                &ecdsa_sig_len);
        if (TEE_ERR_NONE != ret)
        {
            LOG_E("TEE_ECDSASign failed: %d\n", ret);
            break;
        }

        LOG_I("Verifying ECDSA signature..");
        ret = TEE_ECDSAVerify(
                key_data,
                so_data_len,
                hash_data,
                hash_len,
                ecdsa_sig,
                ecdsa_sig_len,
                &validity);
        if (TEE_ERR_NONE != ret)
        {
            LOG_E("TEE_ECDSAVerify failed: %d\n", ret);
            break;
        }

        if (validity ==  false)
        {
            LOG_E("ECDSA signature not valid\n");
            break;
        }

        uint32_t nid = opensslCurveType;

        /* Generate EC key pair by using OpenSSL API */
        eckey_01 = EC_KEY_new_by_curve_name(nid);
        if (eckey_01 == NULL)
        {
            LOG_E("EC_KEY_new_by_curve_name failed\n");
            break;
        }

        LOG_I("Generating EC keypair (OpenSSL API)..");
        if (!EC_KEY_generate_key(eckey_01))
        {
            LOG_E("EC_KEY_generate_key failed\n");
            break;
        }

        x_bn = BN_new();
        y_bn = BN_new();
        if ((x_bn == NULL) || (y_bn == NULL))
        {
            LOG_E("BN_new failed\n");
            break;
        }

        pubkey = EC_KEY_get0_public_key(eckey_01);
        if (pubkey == NULL)
        {
            LOG_E("EC_KEY_get0_public_key failed\n");
            break;
        }

        group_01 = EC_KEY_get0_group(eckey_01);
        if (group_01 == NULL)
        {
            LOG_E("EC_KEY_get0_group failed\n");
            break;
        }

        if (!EC_POINT_get_affine_coordinates_GFp(group_01, pubkey, x_bn, y_bn, NULL))
        {
            LOG_E("EC_POINT_get_affine_coordinates_GFp failed\n");
            break;
        }

        x_len = BN_bn2bin(x_bn,    ptr_x);
        y_len = BN_bn2bin(y_bn,    ptr_y);

        priv_len = BN_bn2bin(EC_KEY_get0_private_key(eckey_01), ptr_priv);

        ecdsa_key = (uint8_t *)malloc(ECDSA_KEY_SO_SIZE);
        if (!ecdsa_key)
        {
            LOG_E("OOM\n");
            break;
        }
        memset(ecdsa_key, 0x0, ECDSA_KEY_SO_SIZE);

        ecdsa_key_so = (uint8_t *)malloc(ECDSA_KEY_SO_SIZE);
        if (!ecdsa_key_so)
        {
            LOG_E("OOM\n");
            break;
        }
        memset(ecdsa_key_so, 0x0, ECDSA_KEY_SO_SIZE);

        switch(curveType)
        {
            case TEE_ECC_CURVE_NIST_P192:
                curve_size = BYTES_PER_BITS(SECP192R1_BITS);
                break;
            case TEE_ECC_CURVE_NIST_P224:
                curve_size = BYTES_PER_BITS(SECP224R1_BITS);
                break;
            case TEE_ECC_CURVE_NIST_P256:
                curve_size = BYTES_PER_BITS(SECP256R1_BITS);
                break;
            case TEE_ECC_CURVE_NIST_P384:
                curve_size = BYTES_PER_BITS(SECP384R1_BITS);
                break;
            case TEE_ECC_CURVE_NIST_P521:
                curve_size = BYTES_PER_BITS(SECP521R1_BITS);
                break;
            default:
                curve_size = 0;
                break;
        }

        if (curve_size == 0)
        {
            LOG_E("Invalid curve size\n");
            break;
        }
        /*
         * ECDSA key data:
         * |-- Key metadata --|--x--|--y--|--private key data --|
         */
        keymeta.keytype = TEE_KEYTYPE_ECDSA;
        keymeta.ecdsakey.curve = curveType;
        keymeta.ecdsakey.curveLen = curve_size;
        offset = 0;

        /**
         * BN_bn2bin() removes leading 0s and it is possible that
         * we may get 65 bytes long key data while using NID_secp521r1.
         * For that reason, we need to add missing leading 0s while
         * importing key data as 66 bytes key data
         * */
        memcpy(ecdsa_key, &keymeta, sizeof(teeKeyMeta_t));
        offset += sizeof(teeKeyMeta_t);
        memcpy(ecdsa_key+offset+(curve_size-x_len), ptr_x, x_len);
        offset += curve_size;
        memcpy(ecdsa_key+offset+(curve_size-y_len), ptr_y, y_len);
        offset += curve_size;
        memcpy(ecdsa_key+offset+(curve_size-priv_len), ptr_priv, priv_len);

        ecdsa_key_len =  sizeof(teeKeyMeta_t) + (3*curve_size); /* x, y and private key */
        ecdsa_key_so_len = ECDSA_KEY_SO_SIZE;

        ret = TEE_KeyImport(
                ecdsa_key,
                ecdsa_key_len,
                ecdsa_key_so,
                &ecdsa_key_so_len);
        if (TEE_ERR_NONE != ret)
        {
            LOG_E("TEE_KeyImport failed: %d\n", ret);
            break;
        }

        memset(ecdsa_sig, 0x0, sizeof(ecdsa_sig));
        ecdsa_sig_len = sizeof(ecdsa_sig);

        LOG_I("Signing data with ECDSA (keymaster API)..");
        ret = TEE_ECDSASign(
                ecdsa_key_so,
                ecdsa_key_so_len,
                hash_data,
                hash_len,
                ecdsa_sig,
                &ecdsa_sig_len);
        if (TEE_ERR_NONE != ret)
        {
            LOG_E("TEE_ECDSASign failed: %d\n", ret);
            break;
        }

        size_t len = ecdsa_sig_len/2;

        ecdsa_signature_01 = ECDSA_SIG_new();
        if (ecdsa_signature_01 == NULL)
        {
            LOG_E("Cannot allocate ECDSA signature structure\n");
            break;
        }

        validity = false;

        LOG_I("Verifying ECDSA signature (keymaster API)..");
        ret = TEE_ECDSAVerify(
                ecdsa_key_so,
                ecdsa_key_so_len,
                hash_data,
                hash_len,
                ecdsa_sig,
                ecdsa_sig_len,
                &validity);
        if ((TEE_ERR_NONE != ret) || (!validity))
        {
            LOG_E("TEE_ECDSAVerify failed: %d\n", ret);
            break;
        }

        ecdsa_signature_01->r =  BN_bin2bn(ecdsa_sig,     len, NULL);
        ecdsa_signature_01->s =  BN_bin2bn(ecdsa_sig+len, len, NULL);

        LOG_I("Verifying ECDSA signature (OpenSSL API)..");
        if (ECDSA_do_verify(hash_data, hash_len, ecdsa_signature_01, eckey_01) != 1)
        {
            LOG_E("ECDSA_do_verify failed\n");
            break;
        }

        /* Get public key data */
        uint32_t ec_pub_key_len = 1024;
        /* Allocate buffer for key data secure object */
        ec_pub_key = (uint8_t*)malloc(1024);
        if (!ec_pub_key)
        {
            LOG_E("OOM\n");
            break;
        }

        memset(ec_pub_key, 0x0, ec_pub_key_len);

        ret = TEE_GetPubKey(
                ecdsa_key_so,
                ecdsa_key_so_len,
                ec_pub_key,
                &ec_pub_key_len);
        if (TEE_ERR_NONE != ret)
        {
            LOG_E("TEE_GetPubKey failed: %d\n", ret);
            break;
        }

        uint8_t *x_data = ec_pub_key + sizeof(teePubKeyMeta_t);
        uint8_t *y_data = x_data + curve_size;

        /* Convert to bignum */
        x_bn =  BN_bin2bn(x_data, curve_size, NULL);
        y_bn =  BN_bin2bn(y_data, curve_size, NULL);

        /* Generate new key structure */
        eckey_02 = EC_KEY_new_by_curve_name(nid);
        if (eckey_02 == NULL)
        {
            LOG_E("EC_KEY_new_by_curve_name failed\n");
            break;
        }

        if (!EC_KEY_set_public_key_affine_coordinates(eckey_02, x_bn, y_bn))
        {
            LOG_E("EC_KEY_set_public_key_affine_coordinates failed\n");
            break;
        }

        LOG_I("Verifying ECDSA signature with exported key (OpenSSL API)..");
        if (ECDSA_do_verify(hash_data, hash_len, ecdsa_signature_01, eckey_02) != 1)
        {
            LOG_E("ECDSA_do_verify failed\n");
            break;
        }

        LOG_I("ECDSA signature is valid\n");

        /*
         * Sign data with large digest value (64 bytes). Digest will be
         * reduced while using P192, P224, P256 and P384
         */
        LOG_I("Creating ECDSA signature (OpenSSL API)..");
        ecdsa_signature_02 = ECDSA_do_sign(
        		test_digest_sha512,
        		sizeof(test_digest_sha512),
        		eckey_01);
		if(!ecdsa_signature_02) {
            LOG_E("ECDSA_do_sign failed\n");
            break;
		}

        LOG_I("Verifying ECDSA signature (OpenSSL API)..");
        if (ECDSA_do_verify(
        		test_digest_sha512,
        		sizeof(test_digest_sha512),
        		ecdsa_signature_02,
        		eckey_01) != 1)
        {
            LOG_E("ECDSA_do_verify failed\n");
            break;
        }


        memset(ecdsa_sig, 0x0, sizeof(ecdsa_sig));

        r_len = BN_num_bytes(ecdsa_signature_02->r);
        s_len = BN_num_bytes(ecdsa_signature_02->s);

        r_len = BN_bn2bin(ecdsa_signature_02->r, ecdsa_sig + (curve_size - r_len));
        s_len = BN_bn2bin(ecdsa_signature_02->s, ecdsa_sig+curve_size + (curve_size - s_len));
        ecdsa_sig_len = 2*curve_size;

        validity = false;

        LOG_I("Verifying ECDSA signature (keymaster API)..");
        ret = TEE_ECDSAVerify(
                ecdsa_key_so,
                ecdsa_key_so_len,
                test_digest_sha512,
                sizeof(test_digest_sha512),
                ecdsa_sig,
                ecdsa_sig_len,
                &validity);
        if ((TEE_ERR_NONE != ret) || (!validity))
        {
            LOG_E("TEE_ECDSAVerify failed: %d\n", ret);
            break;
        }

        LOG_I("ECDSA signature validity: %d\n", validity);

        ret = TEE_ERR_NONE;

    } while(false);

    if (ecdsa_key)       	free(ecdsa_key);
    if (ecdsa_key_so)    	free(ecdsa_key_so);
    if (ec_pub_key)      	free(ec_pub_key);
    if (ecdsa_signature_01) ECDSA_SIG_free(ecdsa_signature_01);
    if (ecdsa_signature_02) ECDSA_SIG_free(ecdsa_signature_02);
    if (eckey_01)        	EC_KEY_free(eckey_01);
    if (eckey_02)        	EC_KEY_free(eckey_02);
    if (x_bn)            	BN_free(x_bn);
    if (y_bn)            	BN_free(y_bn);

    return ret;
}


int main(int argc, char *args[])
{
    teeResult_t ret = TEE_ERR_NONE;
    uint8_t     *rsa_key     = NULL;
    uint8_t     *dsa_key     = NULL;
    uint8_t     *dsa_key_so  = NULL;
    uint32_t    rsa_key_len    = 0;
    uint32_t    dsa_key_len    = 0;
    uint32_t    dsa_key_so_len = 0;
    teeDsaParams_t  dsa_params = {0};
    teeKeyMeta_t    keymeta    = {0};
    /* hardcoded data for signing */
    uint8_t      msg[256]      = {0};
    uint8_t      sig[1024]     = {0};
    uint8_t      dsa_sig[1024] = {0};
    uint32_t     i = 0;
    uint32_t     offset = 0;
    uint32_t     hashLen     = sizeof(hash_data);
    uint32_t     msg_len     = sizeof(msg);
    uint32_t     sig_len     = sizeof(sig);
    uint32_t     dsa_sig_len = sizeof(dsa_sig);
    uint8_t      *pub_key    = NULL;
    bool         validity    = false;
    uint32_t     key_size    = TEE_RSA_KEY_SIZE_2048;
    uint32_t     rsa_size_in_bits[] = { 1024, 2048, 3072, 4096 };
    uint32_t     dsa_size_in_bits[] = { 1024, 2048, 3072 };
    teeEccCurveType_t   ec_curves[] = {
            TEE_ECC_CURVE_NIST_P192,
            TEE_ECC_CURVE_NIST_P224,
            TEE_ECC_CURVE_NIST_P256,
            TEE_ECC_CURVE_NIST_P384,
            TEE_ECC_CURVE_NIST_P521
    };
    uint32_t     openssl_ec_nid[] = {
            NID_X9_62_prime192v1,
            NID_secp224r1,
            NID_X9_62_prime256v1,
            NID_secp384r1,
            NID_secp521r1
    };
    teeRsaKeyPairType_t  keyPairType = TEE_KEYPAIR_RSACRT;
    teeRsaSigAlg_t       algorithm   = TEE_RSA_NODIGEST_NOPADDING;

    do
    {
        /* Set initial data to 0x12..0x12 */
        memset(&msg, 0x12, sizeof(msg));

        /* Reset values before RSA operations */
        validity     = false;

        /* Allocate buffer for key data secure object */
        rsa_key = (uint8_t*)malloc(RSA_KEY_SO_SIZE);
        if (!rsa_key)
        {
            LOG_E("OOM\n");
            break;
        }

        /* Initialize buffer */
        memset(rsa_key, 0, RSA_KEY_SO_SIZE);

        LOG_I("Generating RSA key pair..");

        /* Generate RSA key pair */
        ret = TEE_RSAGenerateKeyPair(
                keyPairType,
                rsa_key,
                RSA_KEY_SO_SIZE,
                key_size,
                RSA_DEFAULT_EXPONENT,
                &rsa_key_len);
        if (TEE_ERR_NONE != ret)
        {
            LOG_E("TEE_RSAGenerateKeyPair failed: %d\n", ret);
            break;
        }

        LOG_I("(RSA) Signing data..");

        /* Sign data */
        ret = TEE_RSASign(
                rsa_key,
                rsa_key_len,
                msg,
                msg_len,
                sig,
                &sig_len,
                algorithm);
        if (TEE_ERR_NONE != ret)
        {
            LOG_E("TEE_RSASign failed: %d\n", ret);
            break;
        }

        LOG_I("(RSA) Verifying signature..");

        /* Verify data */
        ret = TEE_RSAVerify(
                rsa_key,
                rsa_key_len,
                msg,
                msg_len,
                sig,
                sig_len,
                algorithm,
                &validity);
        if (TEE_ERR_NONE != ret)
        {
            LOG_E("TEE_RSAVerify failed: %d\n", ret);
            break;
        }

        LOG_I("Signature validity: %d\n", validity);


        LOG_I("Generating DSA key pair..");

        /* Allocate buffer for key data secure object */
        dsa_key = (uint8_t*)malloc(DSA_KEY_SO_SIZE);
        if (!dsa_key)
        {
            LOG_E("OOM\n");
            break;
        }

        /* Initialize buffer */
        memset(dsa_key, 0, DSA_KEY_SO_SIZE);

        /* Generate DSA key pair */
        dsa_params.p = (uint8_t*)&p[0];
        dsa_params.q = (uint8_t*)&q[0];
        dsa_params.g = (uint8_t*)&g[0];
        dsa_params.pLen = sizeof(p);
        dsa_params.qLen = sizeof(q);
        dsa_params.gLen = sizeof(g);
        dsa_params.xLen = 20;
        dsa_params.yLen = 64;
        ret = TEE_DSAGenerateKeyPair(
                dsa_key,
                DSA_KEY_SO_SIZE,
                &dsa_params,
                &dsa_key_len);
        if (TEE_ERR_NONE != ret)
        {
            LOG_E("TEE_DSAGenerateKeyPair failed: %d\n", ret);
            break;
        }

        LOG_I("Signing data with DSA..");
        ret = TEE_DSASign(
                dsa_key,
                dsa_key_len,
                hash_data,
                hashLen,
                dsa_sig,
                &dsa_sig_len);
        if (TEE_ERR_NONE != ret)
        {
            LOG_E("TEE_DSASign failed: %d\n", ret);
            break;
        }

        validity = false;

        LOG_I("Verifying DSA signature..");
        ret = TEE_DSAVerify(
                dsa_key,
                dsa_key_len,
                hash_data,
                hashLen,
                dsa_sig,
                dsa_sig_len,
                &validity);
        if (TEE_ERR_NONE != ret)
        {
            LOG_E("TEE_DSAVerify failed: %d\n", ret);
            break;
        }

        LOG_I("DSA signature validity: %d\n", validity);

        free(dsa_key);
        dsa_key = NULL;

        LOG_I("Importing DSA key data..");

        memset(&keymeta, 0x0, sizeof(teeKeyMeta_t));
        keymeta.keytype = TEE_KEYTYPE_DSA;
        keymeta.dsakey.pLen = sizeof(p);
        keymeta.dsakey.qLen = sizeof(q);
        keymeta.dsakey.gLen = sizeof(g);
        keymeta.dsakey.xLen = sizeof(x);
        keymeta.dsakey.yLen = sizeof(y);

        dsa_key_len = sizeof(teeKeyMeta_t) +
                    keymeta.dsakey.pLen +
                    keymeta.dsakey.qLen +
                    keymeta.dsakey.gLen +
                    keymeta.dsakey.xLen +
                    keymeta.dsakey.yLen;

        /* Allocate buffer for key data */
        dsa_key = (uint8_t*)malloc(dsa_key_len);
        if (!dsa_key)
        {
            LOG_E("OOM\n");
            break;
        }


        offset = 0;
        memcpy(dsa_key, &keymeta, sizeof(teeKeyMeta_t));
        offset += sizeof(teeKeyMeta_t);
        memcpy(dsa_key + offset, &p, sizeof(p));
        offset += dsa_params.pLen;
        memcpy(dsa_key + offset, &q, sizeof(q));
        offset += dsa_params.qLen;
        memcpy(dsa_key + offset, &g, sizeof(g));
        offset += dsa_params.gLen;
        memcpy(dsa_key + offset, &y, sizeof(y));
        offset += dsa_params.yLen;
        memcpy(dsa_key + offset, &x, sizeof(x));

        /* Allocate buffer for key data secure object */
        dsa_key_so = (uint8_t*)malloc(DSA_KEY_SO_SIZE);
        if (!dsa_key_so)
        {
            LOG_E("OOM\n");
            break;
        }

        dsa_key_so_len = DSA_KEY_SO_SIZE;

        ret = TEE_KeyImport(
                dsa_key,
                dsa_key_len,
                dsa_key_so,
                &dsa_key_so_len);
        if (TEE_ERR_NONE != ret)
        {
            LOG_E("TEE_KeyImport failed: %d\n", ret);
            break;
        }

        validity = false;

        LOG_I("Verifying DSA signature..");
        ret = TEE_DSAVerify(
                dsa_key_so,
                dsa_key_so_len,
                dsa_digest,
                sizeof(dsa_digest),
                dsa_signature_g,
                sizeof(dsa_signature_g),
                &validity);
        if (TEE_ERR_NONE != ret)
        {
            LOG_E("TEE_DSAVerify failed: %d\n", ret);
            break;
        }

        LOG_I("DSA Signature validity: %d\n", validity);

        LOG_I("Retrieving RSA public key data..");

        uint32_t pub_key_len = 1024;
        /* Allocate buffer for key data secure object */
        pub_key = (uint8_t*)malloc(1024);
        if (!pub_key)
        {
            LOG_E("OOM\n");
            break;
        }

        memset(pub_key, 0x0, pub_key_len);

        ret = TEE_GetPubKey(
                rsa_key,
                rsa_key_len,
                pub_key,
                &pub_key_len);
        if (TEE_ERR_NONE != ret)
        {
            LOG_E("TEE_GetPubKey failed: %d\n", ret);
            break;
        }

        LOG_I("Retrieving DSA public key data..");

        memset(pub_key, 0x0, pub_key_len);
        pub_key_len = 1024;

        ret = TEE_GetPubKey(
                dsa_key_so,
                dsa_key_so_len,
                pub_key,
                &pub_key_len);
        if (TEE_ERR_NONE != ret)
        {
            LOG_E("TEE_GetPubKey failed: %d\n", ret);
            break;
        }

        LOG_I("Running RSA tests..");

        /* The tests cover 1024, 2048, 3072 and 4096 bit RSA key data */
        for (i=0; i<sizeof(rsa_size_in_bits)/sizeof(uint32_t); i++)
        {
            /* Run the tests without CRT */
            if (TEE_ERR_NONE != testRsa(rsa_size_in_bits[i], false))
            {
                LOG_E("RSA test (non CRT) for %d bit key data FAILED", rsa_size_in_bits[i]);
            }

            /* Run the tests with CRT */
            if (TEE_ERR_NONE != testRsa(rsa_size_in_bits[i], true))
            {
                LOG_E("RSA test (CRT) for %d bit key data FAILED", rsa_size_in_bits[i]);
            }
        }

        LOG_I("Running DSA tests..");
        /* The tests cover 1024, 2048 and 3072 bit DSA key data */
        for (i=0; i<sizeof(dsa_size_in_bits)/sizeof(uint32_t); i++)
        {
            if (TEE_ERR_NONE != testDsa(dsa_size_in_bits[i]))
            {
                LOG_E("DSA for %d bit key data FAILED", dsa_size_in_bits[i]);
            }

        }

        LOG_I("Running ECDSA tests..");

        /* The tests cover curves p192, p224, p256, p384 and p521 */
        for (i=0; i<sizeof(ec_curves)/sizeof(uint32_t); i++)
        {
            if (TEE_ERR_NONE != testEcdsa(ec_curves[i], openssl_ec_nid[i]))
            {
                LOG_E("ECDSA for curve type %d FAILED", ec_curves[i]);
            }
        }

        LOG_I("Done..");

    } while(false);

    /* Free the buffers */
    if (rsa_key != NULL)    free(rsa_key);
    if (dsa_key_so != NULL) free(dsa_key_so);
    if (dsa_key != NULL)    free(dsa_key);
    if (pub_key != NULL)    free(pub_key);

    return 0;
}
