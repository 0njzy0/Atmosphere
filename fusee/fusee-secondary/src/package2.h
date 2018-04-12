#ifndef FUSEE_PACKAGE2_H
#define FUSEE_PACKAGE2_H

/* This is a library for patching Package2 prior to handoff to Exosphere. */

#define MAGIC_PK21 (0x31324B50)
#define PACKAGE2_SIZE_MAX 0x7FC000

#define PACKAGE2_SECTION_KERNEL 0x0
#define PACKAGE2_SECTION_INI1   0x1
#define PACKAGE2_SECTION_UNUSED 0x2
#define PACKAGE2_SECTION_MAX 0x3

#define PACKAGE2_MINVER_THEORETICAL 0x0
#define PACKAGE2_MAXVER_100 0x2
#define PACKAGE2_MAXVER_200 0x3
#define PACKAGE2_MAXVER_300 0x4
#define PACKAGE2_MAXVER_302 0x5
#define PACKAGE2_MAXVER_400_410 0x6
#define PACKAGE2_MAXVER_500_CURRENT 0x7

#define PACKAGE2_MINVER_100 0x3
#define PACKAGE2_MINVER_200 0x4
#define PACKAGE2_MINVER_300 0x5
#define PACKAGE2_MINVER_302 0x6
#define PACKAGE2_MINVER_400_410 0x7
#define PACKAGE2_MINVER_500_CURRENT 0x8

#define NX_BOOTLOADER_PACKAGE2_LOAD_ADDRESS ((void *)(0xA9800000ull))

typedef struct {
    union {
        uint8_t ctr[0x10];
        uint32_t ctr_dwords[0x4];
    };
    uint8_t section_ctrs[4][0x10];
    uint32_t magic;
    uint32_t entrypoint;
    uint32_t _0x58;
    uint8_t version_max; /* Must be > TZ value. */
    uint8_t version_min; /* Must be < TZ value. */
    uint16_t _0x5E;
    uint32_t section_sizes[4];
    uint32_t section_offsets[4];
    uint8_t section_hashes[4][0x20];
} package2_meta_t;

typedef struct {
    uint8_t signature[0x100];
    union {
        package2_meta_t metadata;
        uint8_t encrypted_header[0x100];
    };
} package2_header_t;

void package2_patch(void *package2_address);

#endif