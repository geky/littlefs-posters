// littlefs bench runner defines


// common includes
#ifdef BENCH_INCLUDE
    #ifndef BENCH_DEFINES_H
    #define BENCH_DEFINES_H

    // include a filesystem?
    #if defined(LFS3)
    #include "lfs3.h"
    #elif defined(LFS2)
    #include "lfs2.h"
    #include "runners/bench_lfs2.h"
    #elif defined(SPIFFS)
    #include "spiffs.h"
    #include "spiffs_nucleus.h"
    #include "runners/bench_spiffs.h"
    #elif defined(YAFFS2)
    #include "yaffs_yaffs2.h"
    #include "yaffsfs.h"
    #include "runners/bench_yaffs2.h"
    #else
    #error "No filesystem defined?"
    #endif

    // ifdef macros for filesystem version
    #ifdef LFS3
    #define BENCH_IFDEF_LFS3(a, b) (a)
    #else
    #define BENCH_IFDEF_LFS3(a, b) (b)
    #endif
    #ifdef LFS2
    #define BENCH_IFDEF_LFS2(a, b) (a)
    #else
    #define BENCH_IFDEF_LFS2(a, b) (b)
    #endif
    #ifdef SPIFFS
    #define BENCH_IFDEF_SPIFFS(a, b) (a)
    #else
    #define BENCH_IFDEF_SPIFFS(a, b) (b)
    #endif
    #ifdef YAFFS2
    #define BENCH_IFDEF_YAFFS2(a, b) (a)
    #else
    #define BENCH_IFDEF_YAFFS2(a, b) (b)
    #endif

    // needed for common lfs3_cfg struct
    #include "lfs3.h"
    // needed for offsetof
    #include <stddef.h>

    // common bench_cfg struct
    //
    // this gets a bit messy due to littlefs's bds expecting a littlefs
    // lfs3_cfg struct
    //
    // to make this work with other filesystems, we always wrap an lfs3_cf
    // and do some funky casting to get to the filesystem-specific config
    //
    // this is a big hack and should _never_ be included in stack/ctx
    // measurements
    struct bench_cfg {
        struct lfs3_cfg cfg;
        #if defined(LFS2)
        struct lfs2_config cfg_lfs2;
        #elif defined(SPIFFS)
        spiffs_config cfg_spiffs;
        #elif defined(YAFFS2)
        struct {
            struct yaffs_param param;
            struct yaffs_driver drv;
        } cfg_yaffs2;
        #endif
    };

    // bench.py generates CFG as a lfs3_cfg pointers, these are conveniences
    // to access filesystem-specific cfg
    #define CFG_LFS3 CFG
    #define CFG_LFS2 (&((const struct bench_cfg*)CFG)->cfg_lfs2)
    #define CFG_SPIFFS (&((const struct bench_cfg*)CFG)->cfg_spiffs)
    #define CFG_YAFFS2 (&((const struct bench_cfg*)CFG)->cfg_yaffs2)

    // hacky macro to get to the bench config from a specific filesystem cfg
    #define BENCH_CFG_FROM(field, p) \
            (const struct bench_cfg*)( \
                (uint8_t*)(p) - offsetof(const struct bench_cfg, field))
    #endif
#endif


// preconfigured defines that control how benches run
#ifdef BENCH_DEFINE
    // include an id for the current fs to simplify above scripts
    BENCH_DEFINE(FS,                    BENCH_IFDEF_LFS3(
                                                LFS3_IFDEF_GBMAP(3, 30),
                                            BENCH_IFDEF_LFS2(2,
                                            BENCH_IFDEF_SPIFFS(4,
                                            BENCH_IFDEF_YAFFS2(5, 0))))     )
    //          name                    value (overridable)
    BENCH_DEFINE(READ_SIZE,             (PAGE_SIZE) ? PAGE_SIZE : 1         )
    BENCH_DEFINE(PROG_SIZE,             (PAGE_SIZE) ? PAGE_SIZE : 1         )
    BENCH_DEFINE(BLOCK_SIZE,            4096                                )
    // optional, overrides both READ_SIZE and PROG_SIZE
    BENCH_DEFINE(PAGE_SIZE,             0                                   )
    // default cache size, this doesn't necessarily need to be limited by
    // read/prog, but doing so levels the playing field
    BENCH_DEFINE(CACHE_SIZE,            LFS3_MAX(
                                            256,
                                            LFS3_MAX(READ_SIZE, PROG_SIZE)) )
    // total disk size
    BENCH_DEFINE(DISK_SIZE,             8*1024*1024                         )
    BENCH_DEFINE(BLOCK_COUNT,           DISK_SIZE/BLOCK_SIZE                )
    // ERASE_SIZE is just informative
    BENCH_DEFINE(ERASE_SIZE,            4096                                )
    // littlefs3 specific defines
    #if defined(LFS3)
    BENCH_DEFINE(BLOCK_RECYCLES,        100                                 )
    // NOTE this was expanded to match littlefs2
    BENCH_DEFINE(RCACHE_SIZE,           LFS3_MAX(256, READ_SIZE)            )
    BENCH_DEFINE(PCACHE_SIZE,           LFS3_MAX(256, PROG_SIZE)            )
    // NOTE this was expanded to match littlefs2
    BENCH_DEFINE(FCACHE_SIZE,           CACHE_SIZE                          )
    BENCH_DEFINE(LOOKAHEAD_SIZE,        16                                  )
    BENCH_DEFINE(GC_FLAGS,              LFS3_GC_GC                          )
    BENCH_DEFINE(GC_STEPS,              0                                   )
    BENCH_DEFINE(GC_LOOKAHEAD_THRESH,   -1                                  )
    BENCH_DEFINE(GC_LOOKGBMAP_THRESH,   -1                                  )
    BENCH_DEFINE(GC_PREERASE_COUNT,     -1                                  )
    BENCH_DEFINE(GC_COMPACT_THRESH,     0                                   )
    BENCH_DEFINE(SHRUB_SIZE,            BLOCK_SIZE/4                        )
    // TODO crystal/fragment_thresh 1/16 or 1/8?
    BENCH_DEFINE(FRAGMENT_SIZE,         LFS3_MIN(BLOCK_SIZE/16, 512)        )
    // TODO should max-prog_size be enforced in lfs3_init?
    BENCH_DEFINE(CRYSTAL_THRESH,        LFS3_MAX(BLOCK_SIZE/16, PROG_SIZE)  )
    BENCH_DEFINE(LOOKGBMAP_THRESH,      BLOCK_COUNT/4                       )
    // littlefs2 specific defines
    #elif defined(LFS2)
    BENCH_DEFINE(BLOCK_CYCLES,          100                                 )
    BENCH_DEFINE(LCACHE_SIZE,           LFS3_MIN(
                                            LFS3_MAX(
                                                CACHE_SIZE,
                                                LFS3_MAX(
                                                    READ_SIZE,
                                                    PROG_SIZE)),
                                            BLOCK_SIZE)                     )
    BENCH_DEFINE(LOOKAHEAD_SIZE,        16                                  )
    BENCH_DEFINE(COMPACT_THRESH,        0                                   )
    BENCH_DEFINE(METADATA_MAX,          0                                   )
    BENCH_DEFINE(INLINE_MAX,            0                                   )
    // spiffs specific defines
    #elif defined(SPIFFS)
    // things break below 64 byte pages, but while spiffs technically
    // works with <256 byte pages, it performs very poorly
    BENCH_DEFINE(SPAGE_TIGHT,           false                               )
    BENCH_DEFINE(SPAGE_SIZE,            LFS3_MAX(
                                            PROG_SIZE,
                                            (SPAGE_TIGHT) ? 64 : 256)       )
    BENCH_DEFINE(FD_COUNT,              1                                   )
    BENCH_DEFINE(FD_SIZE,               FD_COUNT*sizeof(spiffs_fd)          )
    // spiffs's page cache is different from littlefs's cache, let's
    // default to max(3, 3*cache) pages to roughly match littlefs
    BENCH_DEFINE(SCACHE_COUNT,          LFS3_MAX(
                                            2,
                                            (2*CACHE_SIZE)/SPAGE_SIZE)      )
    BENCH_DEFINE(SCACHE_SIZE,           sizeof(spiffs_cache)
                                            + SCACHE_COUNT
                                                * (sizeof(spiffs_cache_page)
                                                    + SPAGE_SIZE)           )
    // yaffs2 specific defines
    #elif defined(YAFFS2)
    // this is limited to 512B by struct yaffs_obj_hdr
    BENCH_DEFINE(YPAGE_TIGHT,           false                               )
    BENCH_DEFINE(YPAGE_SIZE,            LFS3_MAX(
                                            PROG_SIZE,
                                            (YPAGE_TIGHT) ? 512 : 512)      )
    BENCH_DEFINE(RESERVED_BLOCKS,       2                                   )
    // yaffs2's page cache is different from littlefs's cache, let's
    // default to max(3, 3*cache) pages to roughly match littlefs
    BENCH_DEFINE(YCACHE_COUNT,          LFS3_MAX(
                                            2,
                                            (2*CACHE_SIZE)/YPAGE_SIZE)      )
    BENCH_DEFINE(REFRESH_PERIOD,        1000                                )
    #endif
    // bd defines
    BENCH_DEFINE(ERASE_VALUE,           BENCH_IFDEF_SPIFFS(-2,
                                            BENCH_IFDEF_YAFFS2(0xff, -1))   )
    #ifndef BENCH_NAND
    // default timings for NOR flash, based on w25q64jv:
    // https://www.winbond.com/resource-files/
    // W25Q256JV%20SPI%20RevQ%2002072025%20Plus.pdf
    //
    // note one thing unique to NOR flash is the extreme erase cost
    //
    // FR=104 MHz, quad prog (9.6 ns * 8/4)
    // => +~19 ns for bus (not read!)
    //
    // readed=40ns/B fR=50 MHz, quad read (20 ns * 8/4)
    // progged=1582ns/B tPP=0.4 ms, page=256 (0.4 ms / 256 + bus)
    // erased=10986ns/B tSE=45 ms, sector=4096 (45 ms / 4096)
    //
    // reads=0ns (no transaction cost)
    // progs=400000ns tPP=0.4 ms, page=256
    // erases=45000000ns tSE=45 ms, sector=4096
    // readed=40ns/B fR=50 MHz, quad read (20 ns * 8/4)
    // progged=1484ns/B tPP=0.4 ms (((4096/256)*0.4ms - 0.4ms)/4096 + bus)
    // erased=0ns/B (no per-byte cost)
    //
    #ifdef BENCH_SIMPLE
    BENCH_DEFINE(READS_TIMING,          0                                   )
    BENCH_DEFINE(PROGS_TIMING,          0                                   )
    BENCH_DEFINE(ERASES_TIMING,         0                                   )
    BENCH_DEFINE(READED_TIMING,         40                                  )
    BENCH_DEFINE(PROGGED_TIMING,        1582                                )
    BENCH_DEFINE(ERASED_TIMING,         10986                               )
    #else
    BENCH_DEFINE(READS_TIMING,          0                                   )
    BENCH_DEFINE(PROGS_TIMING,          400000                              )
    BENCH_DEFINE(ERASES_TIMING,         45000000                            )
    BENCH_DEFINE(READED_TIMING,         40                                  )
    BENCH_DEFINE(PROGGED_TIMING,        1484                                )
    BENCH_DEFINE(ERASED_TIMING,         0                                   )
    #endif
    #else
    // default timings for NAND flash, based on w25n01gv:
    // https://www.winbond.com/resource-files/W25N01GV%20Rev%20R%20070323.pdf
    //
    // FR=104 MHz, quad read/prog (9.6 ns * 8/4)
    // => +~19 ns for bus
    //
    // readed=31ns/B tRD1=25 us, p=2048, s=512 (25 us / 2048 + bus)
    // progged=141ns/B tPP=250 us, p=2048, s=512 (250 us / 2048 + bus)
    // erased=15ns/B tBE=2 ms, block=131072 (2 ms / 131072)
    //
    // reads=25000ns tRD1=25 us, p=2048, s=512
    // progs=250000ns tPP=250 us, p=2048, s=512
    // erases=2000000ns tBE=2 ms, block=131072
    // readed=31ns/B tRD1=25 us (((131072/2048)*25us - 25us)/131072 + bus)
    // progged=139ns/B tPP=250 us (((131072/2048)*250us - 250us)/131072 + bus)
    // erased=0ns/B (no per-byte cost)
    //
    #ifdef BENCH_SIMPLE
    BENCH_DEFINE(READS_TIMING,          0                                   )
    BENCH_DEFINE(PROGS_TIMING,          0                                   )
    BENCH_DEFINE(ERASES_TIMING,         0                                   )
    BENCH_DEFINE(READED_TIMING,         31                                  )
    BENCH_DEFINE(PROGGED_TIMING,        141                                 )
    BENCH_DEFINE(ERASED_TIMING,         15                                  )
    #else
    BENCH_DEFINE(READS_TIMING,          25000                               )
    BENCH_DEFINE(PROGS_TIMING,          250000                              )
    BENCH_DEFINE(ERASES_TIMING,         2000000                             )
    BENCH_DEFINE(READED_TIMING,         31                                  )
    BENCH_DEFINE(PROGGED_TIMING,        139                                 )
    BENCH_DEFINE(ERASED_TIMING,         0                                   )
    #endif
    #endif
    #ifndef BENCH_KIWIBD
    BENCH_DEFINE(ERASE_CYCLES,          0                                   )
    BENCH_DEFINE(BADBLOCK_BEHAVIOR,     LFS3_EMUBD_BADBLOCK_PROGERROR       )
    BENCH_DEFINE(POWERLOSS_BEHAVIOR,    LFS3_EMUBD_POWERLOSS_ATOMIC         )
    BENCH_DEFINE(BD_SEED,               0                                   )
    #endif
#endif


// struct lfs3_cfg definition
#ifdef BENCH_CFG
    struct bench_cfg _cfg = {
        // we always create an lfs3_cfg struct, this weirdness is
        // necessary to make littlefs's bd API work
        .cfg = {
            #ifdef BENCH_CFG_CFG
            BENCH_CFG_CFG
            #endif
            // common cfg fields
            .read_size                  = READ_SIZE,
            .prog_size                  = PROG_SIZE,
            .block_size                 = BLOCK_SIZE,
            .block_count                = BLOCK_COUNT,
            // littlefs3 specific cfg fields
            #if defined(LFS3)
            .block_recycles             = BLOCK_RECYCLES,
            .rcache_size                = RCACHE_SIZE,
            .pcache_size                = PCACHE_SIZE,
            .fcache_size                = FCACHE_SIZE,
            .lookahead_size             = LOOKAHEAD_SIZE,
            #ifdef LFS3_GBMAP
            .gc_lookgbmap_thresh        = GC_LOOKGBMAP_THRESH,
            .lookgbmap_thresh           = LOOKGBMAP_THRESH,
            #endif
            #ifdef LFS3_PREERASE
            .gc_preerase_count          = GC_PREERASE_COUNT,
            #endif
            #ifdef LFS3_GC
            .gc_flags                   = GC_FLAGS,
            .gc_steps                   = GC_STEPS,
            #endif
            .gc_lookahead_thresh        = GC_LOOKAHEAD_THRESH,
            .gc_compact_thresh          = GC_COMPACT_THRESH,
            .shrub_size                 = SHRUB_SIZE,
            .fragment_size              = FRAGMENT_SIZE,
            .crystal_thresh             = CRYSTAL_THRESH,
            #endif
        },
        // littlefs2 specific cfg fields
        #if defined(LFS2)
        .cfg_lfs2 = {
            .read                       = bench_lfs2_bd_read,
            .prog                       = bench_lfs2_bd_prog,
            .erase                      = bench_lfs2_bd_erase,
            .sync                       = bench_lfs2_bd_sync,
            .read_size                  = READ_SIZE,
            .prog_size                  = PROG_SIZE,
            .block_size                 = BLOCK_SIZE,
            .block_count                = BLOCK_COUNT,
            .block_cycles               = BLOCK_CYCLES,
            .cache_size                 = LCACHE_SIZE,
            .lookahead_size             = LOOKAHEAD_SIZE,
            .compact_thresh             = COMPACT_THRESH,
            .metadata_max               = METADATA_MAX,
            .inline_max                 = INLINE_MAX,
        },
        #elif defined(SPIFFS)
        .cfg_spiffs = {
            .hal_read_f                 = bench_spiffs_bd_read,
            .hal_write_f                = bench_spiffs_bd_write,
            .hal_erase_f                = bench_spiffs_bd_erase,
            .phys_size                  = BLOCK_SIZE * BLOCK_COUNT,
            .phys_addr                  = 0,
            .phys_erase_block           = BLOCK_SIZE,
            .log_block_size             = BLOCK_SIZE,
            .log_page_size              = SPAGE_SIZE,
        },
        #elif defined(YAFFS2)
        .cfg_yaffs2 = {
            .drv = {
                .drv_read_chunk_fn      = bench_yaffs2_bd_readchunk,
                .drv_write_chunk_fn     = bench_yaffs2_bd_writechunk,
                .drv_erase_fn           = bench_yaffs2_bd_erase,
                .drv_mark_bad_fn        = bench_yaffs2_bd_markbad,
                .drv_check_bad_fn       = bench_yaffs2_bd_checkbad,
            },
            .param = {
                .name                   = "/",
                .inband_tags            = true,
                .total_bytes_per_chunk  = YPAGE_SIZE,
                .chunks_per_block       = BLOCK_SIZE / YPAGE_SIZE,
                .spare_bytes_per_chunk  = 0,
                .start_block            = 0,
                .end_block              = BLOCK_COUNT-1,
                .n_reserved_blocks      = RESERVED_BLOCKS,
                .n_caches               = YCACHE_COUNT,
                .cache_bypass_aligned   = false,
                .use_nand_ecc           = true, // fake ecc
                .tags_9bytes            = false,
                .no_tags_ecc            = false,
                .is_yaffs2              = true,
                .empty_lost_n_found     = false,
                .refresh_period         = REFRESH_PERIOD,
                .skip_checkpt_rd        = false,
                .skip_checkpt_wr        = false,
                .enable_xattr           = false,
                .max_objects            = 0, // unbounded
                .hide_lost_n_found      = false,
                .stored_endian          = 1, // le
            },
        },
        #endif
    };
    struct lfs3_cfg *BENCH_CFG = &_cfg.cfg;

    // a bit of a hack, but force reset yaffs2 global state
    #ifdef YAFFS2
    extern struct list_head yaffsfs_deviceList;
    INIT_LIST_HEAD(&yaffsfs_deviceList);
    extern int yaffsfs_handlesInitialised;
    yaffsfs_handlesInitialised = false;
    #endif
#endif


// struct lfs3_*bd_cfg definition
#ifdef BENCH_BDCFG
    #ifndef BENCH_KIWIBD
    struct lfs3_emubd_cfg _bdcfg = {
        #ifdef BENCH_BDCFG_CFG
        BENCH_BDCFG_CFG
        #endif
        .erase_value                    = ERASE_VALUE,
        .reads_timing                   = READS_TIMING,
        .progs_timing                   = PROGS_TIMING,
        .erases_timing                  = ERASES_TIMING,
        .readed_timing                  = READED_TIMING,
        .progged_timing                 = PROGGED_TIMING,
        .erased_timing                  = ERASED_TIMING,
        .erase_cycles                   = ERASE_CYCLES,
        .badblock_behavior              = BADBLOCK_BEHAVIOR,
        .powerloss_behavior             = POWERLOSS_BEHAVIOR,
        .seed                           = BD_SEED,
    };
    struct lfs3_emubd_cfg *BENCH_BDCFG = &_bdcfg;
    #else
    struct lfs3_kiwibd_cfg _bdcfg = {
        #ifdef BENCH_BDCFG_CFG
        BENCH_BDCFG_CFG
        #endif
        .erase_value                    = ERASE_VALUE,
        .reads_timing                   = READS_TIMING,
        .progs_timing                   = PROGS_TIMING,
        .erases_timing                  = ERASES_TIMING,
        .readed_timing                  = READED_TIMING,
        .progged_timing                 = PROGGED_TIMING,
        .erased_timing                  = ERASED_TIMING,
    };
    struct lfs3_kiwibd_cfg *BENCH_BDCFG = &_bdcfg;
    #endif
#endif



