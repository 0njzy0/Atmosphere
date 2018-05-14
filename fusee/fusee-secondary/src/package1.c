#include <errno.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "package1.h"
#include "bct.h"
#include "se.h"

int package1_read_and_parse_boot0(void **package1loader, size_t *package1loader_size, nx_keyblob_t *keyblobs, uint32_t *revision, FILE *boot0) {
    static nvboot_config_table bct = {0}; /* Normal firmware BCT, primary. TODO: check? */
    nv_bootloader_info *pk1_info = &bct.bootloader[0]; /* TODO: check? */

    size_t fpos, pk1_offset;

    if (package1loader == NULL || package1loader_size != NULL || keyblobs == NULL || revision == NULL || boot0 == NULL) {
        errno = EINVAL;
        return -1;
    }

    fpos = ftell(boot0);

    /* Read the BCT. */
    if (fread(&bct, sizeof(nvboot_config_table), 1, boot0) == 0) {
        return -1;
    }
    if (bct.bootloader_used < 1) {
        errno = EILSEQ;
        return -1;
    }

    *revision = pk1_info->attribute;
    *package1loader_size = pk1_info->length;

    pk1_offset = 0x4000 * pk1_info->start_blk + 0x200 * pk1_info->start_page;

    (*package1loader) = memalign(16, *package1loader_size);

    if (*package1loader == NULL) {
        errno = ENOMEM;
        return -1;
    }

    /* Read the pk1/pk1l. */
    if (fseek(boot0, fpos + pk1_offset, SEEK_SET) != 0) {
        return -1;
    }
    if (fread(*package1loader, *package1loader_size, 1, boot0) == 0) {
        return -1;
    }

    /* Skip the backup pk1/pk1l. */
    if (fseek(boot0, *package1loader_size, SEEK_CUR) != 0) {
        return -1;
    }

    /* Read the full keyblob area.*/
    for (size_t i = 0; i < 32; i++) {
        if (fread(&keyblobs[i], sizeof(nx_keyblob_t), 1, boot0) == 0) {
            return -1;
        }
        if (fseek(boot0, 0x200 - sizeof(nx_keyblob_t), SEEK_CUR)) {
            return -1;
        }
    }

    return 0;
}

size_t package1_get_tsec_fw(void **tsec_fw, const void *package1loader, size_t package1loader_size) {
    /* The TSEC firmware is always located at a 256-byte aligned address. */
    /* We're looking for its 4 first bytes. We assume its size is always 0xF00 bytes. */
    const uint32_t *pos;
    uintptr_t pk1l = (uintptr_t)package1loader;
    for (pos = (const uint32_t *)pk1l; (uintptr_t)pos < pk1l + package1loader_size && *pos != 0xCF42004D; pos += 0x40);

    (*tsec_fw) = (void *)pos;
    return 0xF00;
}

size_t package1_get_encrypted_package1(package1_header_t **package1, uint8_t *ctr, const void *package1loader, size_t package1loader_size) {
    const uint32_t *pos;
    uintptr_t pk1l = (uintptr_t)package1loader;

    if (package1loader_size < 0x4000) {
        return 0; /* Shouldn't happen, ever. */
    }

    for (pos = (const uint32_t *)pk1l; (uintptr_t)pos < pk1l + 0x3FF8 && (pos[0] != 0x70012000 || pos[2] != 0x40007000); pos++);
    pos = (const uint32_t *)(pk1l + pos[1] - 0x40010000);

    memcpy(ctr, pos + 4, 0x10);
    (*package1) = (package1_header_t *)(pos + 8);
    return *pos;
}

bool package1_decrypt(package1_header_t *package1, size_t package1_size, const uint8_t *ctr) {
    uint8_t __attribute__((aligned(16))) ctrbuf[16];
    memcpy(ctrbuf, ctr, 16);
    se_aes_ctr_crypt(0xB, package1, package1_size, package1, package1_size, ctr, 16);
    return memcmp(package1->magic, "PK11", 4) == 0;
}

void *package1_get_warmboot_fw(const package1_header_t *package1) {
    /*  
        The layout of pk1 changes between versions.

        However, the secmon always starts by this erratum code:
        https://github.com/ARM-software/arm-trusted-firmware/blob/master/plat/nvidia/tegra/common/aarch64/tegra_helpers.S#L312
        and thus by 0xD5034FDF.

        Nx-bootloader seems to always start by 0xE328F0C0 (msr cpsr_f, 0xc0).
    */
    const uint32_t *data = (const uint32_t *)package1->data;

    for (size_t i = 0; i < 3; i++) {
        switch (*data) {
            case 0xD5034FDFu:
                data += package1->secmon_size / 4;
                break;
            case 0xE328F0C0:
                data += package1->nx_bootloader_size / 4;
                break;
            default:
                /* TODO: should we validate its signature? */
                return (void *)data;
        }
    }

    return NULL;
}
