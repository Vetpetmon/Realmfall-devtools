#include "cjson/cJSON.h"
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
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
    char *textColor; // Hex color code for text color
    char *secondaryColor; // Hex color code for secondary color (used in subtext under rank-up messages)
    int ranks; // Number of ranks; this is because some characters may have 6 ranks instead of 5
    CharacterClass charClass;
} Character;

// Creates a power rank JSON object
void createEvoJSON(cJSON *jsonObj, Character character, int evoStage);

// Creates a no soulstone power JSON object for the maximum rank
void createNoSoulstoneJSON(cJSON *jsonObj, Character character, int evoStage);

// Creates an origin rank JSON object
void createRankOriginJSON(cJSON *jsonObj, Character character, int evoStage);

// Helper: create a play_sound action object
cJSON *create_play_sound_action(const char *sound, double volume, double pitch);

// Helper: create an execute_command action object
cJSON *create_execute_command_action(const char *command);

// Helper: create a change_resource action object
cJSON *create_change_resource_action(const char *resource, double change, const char *operation);

// Helper: create a spawn_particles action object
cJSON *create_spawn_particles_action(const char *particle, int count, double speed, cJSON *spread, int duplicate_spread);
