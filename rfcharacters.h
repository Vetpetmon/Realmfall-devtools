#include "cjson/cJSON.h"
#include <sys/types.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#else
#include <linux/limits.h>
#endif

// Create directories recursively like `mkdir -p`
// Platform-agnostic mkdir wrapper: on Windows use _mkdir(path), on POSIX use mkdir(path, mode)
static int ag_mkdir(const char *path, mode_t mode) {
#ifdef _WIN32
    (void)mode;
    return _mkdir(path);
#else
    return mkdir(path, mode);
#endif
}

int mkdir_p(const char *path, mode_t mode);

// Class Struct
typedef struct {
    int healthPerRank;
    int armorPerRank;
    double meleeDamagePerRank; // +percent
    double rangedDamagePerRank; // +percent
    double generalDamagePerRank; // +percent
    double damageResistancePerRank; // +percent
} CharacterClass;

typedef struct {
    char *name; // Character name, must be lowercase and underscores only. Also seen as the id by the game
    char *displayName; // Display name with proper capitalization and spaces
    char *textColor; // Hex color code for text color
    char *secondaryColor; // Hex color code for secondary color (used in subtext under rank-up messages)
    int ranks; // Number of ranks; this is because some characters may have 6 ranks instead of 5
    CharacterClass charClass;
} Character;

