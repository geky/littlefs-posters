/*
 * YAFFS2 config, for littlefs benchmarking
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#define CONFIG_YAFFS_DIRECT 1
#define CONFIG_YAFFS_YAFFS2 1
// TODO docs say not supported? TODO test this?
#define CONFIG_YAFFS_NO_YAFFS1 1
// well yes use less RAM
#define CONFIG_YAFFS_SMALL_RAM 1
// TODO caches short names in RAM, test without this?
#define CONFIG_YAFFS_SHORT_NAMES_IN_RAM 1
// TODO enable trace for non-codemaps?
// disable trace for code size
#define CONFIG_YAFFS_NO_TRACE
// TODO should we include system-related headers instead of
// redefining these?
#define CONFIG_YAFFS_PROVIDE_DEFS 1
#define CONFIG_YAFFSFS_PROVIDE_VALUES 1
#define CONFIG_YAFFS_DEFINES_TYPES 1
#define CONFIG_YAFFS_USE_32_BIT_TIME_T 1
// TODO what does this do?
// #define NO_Y_INLINE 1
#define loff_t off_t

