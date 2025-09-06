/**
 * @file lazybsd_crypto.c
 * @author mengdemao (mengdemao19951021@163.com)
 * @version 1.0
 * @date 2024-06-29
 *
 * @brief
 *
 * @copyright Copyright (c) 2024  mengdemao
 *
 */
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/endian.h>
#include <sys/libkern.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/types.h>

#include <crypto/siphash/siphash.h>

static void SipRounds(SIPHASH_CTX* ctx, int final);

// void
// SipHash_InitX(SIPHASH_CTX *ctx, int rc, int rf)
// {

// 	ctx->v[0] = 0x736f6d6570736575ull;
// 	ctx->v[1] = 0x646f72616e646f6dull;
// 	ctx->v[2] = 0x6c7967656e657261ull;
// 	ctx->v[3] = 0x7465646279746573ull;
// 	ctx->buf.b64 = 0;
// 	ctx->bytes = 0;
// 	ctx->buflen = 0;
// 	ctx->rounds_compr = rc;
// 	ctx->rounds_final = rf;
// 	ctx->initialized = 1;
// }

void SipHash_SetKey(SIPHASH_CTX* ctx, const uint8_t key[static SIPHASH_KEY_LENGTH])
{
    uint64_t k[2];

    KASSERT(ctx->v[0] == 0x736f6d6570736575ull && ctx->initialized == 1, ("%s: context %p not properly initialized", __func__, ctx));

    k[0] = le64dec(&key[0]);
    k[1] = le64dec(&key[8]);

    ctx->v[0] ^= k[0];
    ctx->v[1] ^= k[1];
    ctx->v[2] ^= k[0];
    ctx->v[3] ^= k[1];

    ctx->initialized = 2;
}

static size_t SipBuf(SIPHASH_CTX* ctx, const uint8_t** src, size_t len, int final)
{
    size_t x = 0;

    KASSERT((!final && len > 0) || (final && len == 0), ("%s: invalid parameters", __func__));

    if (!final) {
        x = MIN(len, sizeof(ctx->buf.b64) - ctx->buflen);
        bcopy(*src, &ctx->buf.b8[ctx->buflen], x);
        ctx->buflen += x;
        *src += x;
    } else
        ctx->buf.b8[7] = (uint8_t)ctx->bytes;

    if (ctx->buflen == 8 || final) {
        ctx->v[3] ^= le64toh(ctx->buf.b64);
        SipRounds(ctx, 0);
        ctx->v[0] ^= le64toh(ctx->buf.b64);
        ctx->buf.b64 = 0;
        ctx->buflen  = 0;
    }
    return (x);
}

uint64_t SipHash_End(SIPHASH_CTX* ctx)
{
    uint64_t r;

    KASSERT(ctx->initialized == 2, ("%s: context %p not properly initialized", __func__, ctx));

    SipBuf(ctx, NULL, 0, 1);
    ctx->v[2] ^= 0xff;
    SipRounds(ctx, 1);
    r = (ctx->v[0] ^ ctx->v[1]) ^ (ctx->v[2] ^ ctx->v[3]);

    bzero(ctx, sizeof(*ctx));
    return (r);
}

uint64_t SipHashX(SIPHASH_CTX* ctx, int rc, int rf, const uint8_t key[static SIPHASH_KEY_LENGTH], const void* src, size_t len)
{

    SipHash_InitX(ctx, rc, rf);
    SipHash_SetKey(ctx, key);
    SipHash_Update(ctx, src, len);

    return (SipHash_End(ctx));
}

#define SIP_ROTL(x, b) (uint64_t)(((x) << (b)) | ((x) >> (64 - (b))))

static void SipRounds(SIPHASH_CTX* ctx, int final)
{
    int rounds;

    if (!final)
        rounds = ctx->rounds_compr;
    else
        rounds = ctx->rounds_final;

    while (rounds--) {
        ctx->v[0] += ctx->v[1];
        ctx->v[2] += ctx->v[3];
        ctx->v[1] = SIP_ROTL(ctx->v[1], 13);
        ctx->v[3] = SIP_ROTL(ctx->v[3], 16);

        ctx->v[1] ^= ctx->v[0];
        ctx->v[3] ^= ctx->v[2];
        ctx->v[0] = SIP_ROTL(ctx->v[0], 32);

        ctx->v[2] += ctx->v[1];
        ctx->v[0] += ctx->v[3];
        ctx->v[1] = SIP_ROTL(ctx->v[1], 17);
        ctx->v[3] = SIP_ROTL(ctx->v[3], 21);

        ctx->v[1] ^= ctx->v[2];
        ctx->v[3] ^= ctx->v[0];
        ctx->v[2] = SIP_ROTL(ctx->v[2], 32);
    }
}
