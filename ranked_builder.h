#include "cjson/cJSON.h"
#include "rfcharacters.h"
#include <sys/types.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#else
#include <linux/limits.h>
#endif

// Creates a power rank JSON object
void createEvoJSON(cJSON *jsonObj, Character character, int evoStage);

// Creates a no soulstone power JSON object for the maximum rank
void createNoSoulstoneJSON(cJSON *jsonObj, Character character, int evoStage);

// Creates an origin rank JSON object
void createRankOriginJSON(cJSON *jsonObj, Character character, int evoStage);

// Creates a power that holds stat upgrades based on evo stage and character class
void createStatUpgradePowerJSON(cJSON *jsonObj, Character character, int evoStage);

// Creates stat increase power description based on character class and evo stage
char *createStatUpgradeDescription(Character character, int evoStage);

// Makes stat increases based on character class and evo stage
int calculateStatIncrease(int base, int perRank, int evoStage);
float calculateStatIncreaseFloat(float base, float perRank, int evoStage);
double calculateStatIncreaseDouble(double base, double perRank, int evoStage);

// Helper: create a play_sound action object
cJSON *create_play_sound_action(const char *sound, double volume, double pitch);

// Helper: create an execute_command action object
cJSON *create_execute_command_action(const char *command);

// Helper: create a change_resource action object
cJSON *create_change_resource_action(const char *resource, double change, const char *operation);

// Helper: create a spawn_particles action object
cJSON *create_spawn_particles_action(const char *particle, int count, double speed, cJSON *spread, int duplicate_spread);

// Generate all files and directories for a Character (used by character_builder)
int generate_character_files(Character newCharacter);
