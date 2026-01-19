#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <errno.h>
#include <limits.h>
#include "cjson/cJSON.h" // Include cJSON library for JSON handling


// Class Struct
typedef struct {
    int healthPerRank;
    int armorPerRank;
    double meleeDamagePerRank; // +percent
    double rangedDamagePerRank; // +percent
    double generalDamagePerRank; // +percent
    double damageResistancePerRank; // +percent
} Class;

typedef struct {
    char *name; // Character name, must be lowercase and underscores only. Also seen as the id by the game
    char *textColor; // Hex color code for text color
    char *secondaryColor; // Hex color code for secondary color (used in subtext under rank-up messages)
    int ranks; // Number of ranks; this is because some characters may have 6 ranks instead of 5
    Class class;
} Character;

// Forward declaration of createEvoJSON
void createEvoJSON(cJSON *jsonObj, Character character, int evoStage);

void createNoSoulstoneJSON(cJSON *jsonObj, Character character, int evoStage);

// Create directories recursively like `mkdir -p`
int mkdir_p(const char *path, mode_t mode) {
    if (path == NULL || *path == '\0') {
        errno = EINVAL;
        return -1;
    }

    char tmp[PATH_MAX];
    size_t len = strlen(path);
    if (len >= sizeof(tmp)) {
        errno = ENAMETOOLONG;
        return -1;
    }

    // Copy path and strip trailing '/'
    strcpy(tmp, path);
    if (len > 1 && tmp[len - 1] == '/') {
        tmp[len - 1] = '\0';
    }

    // Iterate and create each component
    for (char *p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            if (mkdir(tmp, mode) != 0) {
                if (errno != EEXIST) {
                    *p = '/';
                    return -1;
                }
            }
            *p = '/';
        }
    }

    // Create final path
    if (mkdir(tmp, mode) != 0) {
        if (errno != EEXIST) {
            return -1;
        }
    }
    return 0;
}

int main() {
    // Initialize the main 4 classes
    Class meleeClass = {2, 1, 0.08, 0.0, 0.0, 0.0}; // Melee class
    Class rangedClass = {1, 1, 0.0, 0.08, 0.0, 0.0}; // Ranged class
    Class defenseClass = {2, 2, 0.0, 0.0, 0.0, 0.05}; // Defense class
    Class mageClass = {1, 1, 0.0, 0.0, 0.08, 0.0}; // Mage class

    // Strings for File paths
    char powerFilepath[100] = "powers/flavors/";
    char originFilepath[100] = "origins/";
    char rankedFilepath[100] = "origins/ranks/";

    // Veriables used to store user selections
    Class selectedClass;
    Character newCharacter;
    char nameBuffer[50];
    char colorBuffer[20];
    // Get character name from user with verification
    while(1) {
        printf("Enter a name for your character (max 49 characters): ");
        fgets(nameBuffer, sizeof(nameBuffer), stdin);
        // Remove newline character if present
        nameBuffer[strcspn(nameBuffer, "\n")] = 0;
        if (strlen(nameBuffer) == 0) {
            printf("Name cannot be empty. Please try again.\n");
        } else {

            // Check if all characters are:
            // letters or underscores
            // underscores are not at start or end
            // if a letter, must be undercased

            // This is because for datapacks, names must be lowercase and underscores only
            // We do not allow numbers according to naming conventions
            int valid = 1;
            int len = strlen(nameBuffer);
            for (int i = 0; i < len; i++) {
                char c = nameBuffer[i];
                if (!((c >= 'a' && c <= 'z') || c == '_')) {
                    valid = 0;
                    break;
                }
                if (c == '_' && (i == 0 || i == len - 1)) {
                    valid = 0;
                    break;
                }
            }
            if (!valid) {
                printf("Invalid name. Use only lowercase letters and underscores (not at start or end).\n");
                continue; // prompt again
            }
            break; // exit the loop if valid input
        }
    }
    newCharacter.name = strdup(nameBuffer); // Allocate and copy name

    // Get text color from user
    while (1) {
        printf("Enter a hex color code for your character's text color (e.g., #7fffd4 for a aquamarine color): ");
        fgets(colorBuffer, sizeof(colorBuffer), stdin);
        // Remove newline character if present
        colorBuffer[strcspn(colorBuffer, "\n")] = 0;
        // Validate hex color code
        // Check first character is '#' and give feedback if not
        if (colorBuffer[0] != '#') {
            printf("Invalid color code format: Missing '#' at the beginning. Please try again.\n");
            continue; // prompt again
        }
        // Check length and characters
        if (strlen(colorBuffer) != 7) {
            // Uniquely warn about length being 9 (alpha channel included)
            if (strlen(colorBuffer) == 9) {
                printf("Invalid color code format: It seems you've included an alpha channel. Please provide a 6-digit hex code prefixed with '#'.\n");
            } // Because text colors do not have an alpha channel
            else {
                printf("Invalid color code format: Incorrect length. Please ensure it's 7 characters long, including '#'.\n");
            }
            continue; // prompt again
        }
        int valid = 1;
        for (int i = 1; i < 7; i++) {
            char c = colorBuffer[i];
            if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) {
                valid = 0;
                break;
            }
        }
        if (!valid) {
            printf("Invalid color code format. Please try again.\n");
            continue; // prompt again
        }
        break; // exit the loop if valid input
    }
    newCharacter.textColor = strdup(colorBuffer); // Allocate and copy text color

    // Secondary color
    while (1) {
        printf("Enter a hex color code for your character's secondary color (e.g., #7fffd4 for a aquamarine color): ");
        fgets(colorBuffer, sizeof(colorBuffer), stdin);
        // Remove newline character if present
        colorBuffer[strcspn(colorBuffer, "\n")] = 0;
        // Validate hex color code
        // Check first character is '#' and give feedback if not
        if (colorBuffer[0] != '#') {
            printf("Invalid color code format: Missing '#' at the beginning. Please try again.\n");
            continue; // prompt again
        }
        // Check length and characters
        if (strlen(colorBuffer) != 7) {
            // Uniquely warn about length being 9 (alpha channel included)
            if (strlen(colorBuffer) == 9) {
                printf("Invalid color code format: It seems you've included an alpha channel. Please provide a 6-digit hex code prefixed with '#'.\n");
            } // Because text colors do not have an alpha channel
            else {
                printf("Invalid color code format: Incorrect length. Please ensure it's 7 characters long, including '#'.\n");
            }
            continue; // prompt again
        }
        int valid = 1;
        for (int i = 1; i < 7; i++) {
            char c = colorBuffer[i];
            if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) {
                valid = 0;
                break;
            }
        }
        if (!valid) {
            printf("Invalid color code format. Please try again.\n");
            continue; // prompt again
        }
        break; // exit the loop if valid input
    }
    newCharacter.secondaryColor = strdup(colorBuffer); // Allocate and copy secondary color


    // for-loop to retry if invalid input
    while(1) {
        // Give options to user to select class
        printf("Select a class for your character:\n");
        printf("1. Melee\n");
        printf("2. Ranged\n");
        printf("3. Defense\n");
        printf("4. Mage\n");
        int classChoice;
        scanf("%d", &classChoice);
        switch (classChoice) {
            case 1:
                selectedClass = meleeClass;
                break;
            case 2:
                selectedClass = rangedClass;
                break;
            case 3:
                selectedClass = defenseClass;
                break;
            case 4:
                selectedClass = mageClass;
                break;
            default:
                printf("Invalid choice.\n");
        }
        if (classChoice >= 1 && classChoice <= 4) {
            break; // exit the loop if valid input
        }
    }
    newCharacter.class = selectedClass;

    // Get number of ranks from user
    while (1) {
        printf("Enter the number of ranks for your character (5 or 6): ");
        int ranks;
        scanf("%d", &ranks);
        if (ranks == 5 || ranks == 6) {
            newCharacter.ranks = ranks;
            break; // exit the loop if valid input
        } else {
            printf("Invalid number of ranks. Please enter 5 or 6.\n");
        }
    }
    // Print character summary
    printf("\nCharacter Summary:\n");
    printf("Name: %s\n", newCharacter.name);
    printf("Text Color: %s\n", newCharacter.textColor);
    printf("Secondary Text Color: %s\n", newCharacter.secondaryColor);
    printf("Stats per Rank:\n");
    printf("\tHealth: +%d, \n", newCharacter.class.healthPerRank);
    printf("\tArmor: +%d, \n", newCharacter.class.armorPerRank);
    printf("\tMelee Damage: +%.2f%%, \n", newCharacter.class.meleeDamagePerRank * 100);
    printf("\tRanged Damage: +%.2f%%, \n", newCharacter.class.rangedDamagePerRank * 100);
    printf("\tGeneral Damage: +%.2f%%, \n", newCharacter.class.generalDamagePerRank * 100);
    printf("\tDamage Resistance: +%.2f%%\n", newCharacter.class.damageResistancePerRank * 100);
    printf("Number of Ranks: %d\n", newCharacter.ranks);

    // Ask user if the details are correct
    char confirm;
    printf("Are these details correct? (y/n): ");
    scanf(" %c", &confirm);
    // Convert to lowercase
    confirm = tolower(confirm);
    if (confirm != 'y') {
        printf("Character creation cancelled. Please run the program again to create a new character.\n");
        // Free allocated memory
        free(newCharacter.name);
        free(newCharacter.textColor);
        free(newCharacter.secondaryColor);
        return 0; // Still a normal exit.
    }

    // Ensure base directories exist (create parents as needed)
    if (mkdir_p(powerFilepath, 0755) != 0) {
        perror("Error creating power directory");
    }
    if (mkdir_p(originFilepath, 0755) != 0) {
        perror("Error creating origins directory");
    }
    if (mkdir_p(rankedFilepath, 0755) != 0) {
        perror("Error creating ranks directory");
    }

    // Create character-specific directories under powers/flavors
    char characterDir[256];
    strcpy(characterDir, powerFilepath);
    strcat(characterDir, newCharacter.name);
    if (mkdir_p(characterDir, 0755) != 0) {
        perror("Error creating character directory");
    }
    char evo0Dir[256];
    strcpy(evo0Dir, characterDir);
    strcat(evo0Dir, "/0star");
    if (mkdir_p(evo0Dir, 0755) != 0) {
        perror("Error creating evo0 directory");
    }
    
    // Create other rank directories as needed based on newCharacter.ranks
    for (int i = 0; i < newCharacter.ranks; i++) {
        char rankDir[256];
        sprintf(rankDir, "%s/%dstar", characterDir, i);
        if (mkdir_p(rankDir, 0755) != 0) {
            perror("Error creating rank directory");
        }
        // create evo.json files in each directory
        cJSON *evoJSON = cJSON_CreateObject();

        createEvoJSON(evoJSON, newCharacter, i);
        // cat strings to form filepath
        char evoFilepath[200];
        strcat(strcpy(evoFilepath, powerFilepath), newCharacter.name);
        char rankStr[100];
        sprintf(rankStr, "/%dstar/evo.json", i);
        strcat(evoFilepath, rankStr);
        FILE *evoFile = fopen(evoFilepath, "w");
        if (evoFile == NULL) {
            printf("Error creating evo.json file for rank %d.\n", i);
            cJSON_Delete(evoJSON);
            continue; // skip to next rank
        }
        // We want to prettify the JSON output for easier reading
        char *prettyEvoString = cJSON_Print(evoJSON);
        printf("%s\n", prettyEvoString);
        fputs(prettyEvoString, evoFile);
        cJSON_free(prettyEvoString);
        printf("evo.json file created successfully at %s\n", evoFilepath);
        fclose(evoFile);
        cJSON_Delete(evoJSON);

    }
    // Make final rank directory for preventsouls.json
    char finalRankDir[256];
    sprintf(finalRankDir, "%s/%s/%dstar", powerFilepath, newCharacter.name, newCharacter.ranks);
    if (mkdir_p(finalRankDir, 0755) != 0) {
        perror("Error creating final rank directory");
    }

    // Create the preventsouls.json in the final rank directory
    cJSON *noSoulstoneJSON = cJSON_CreateObject();
    createNoSoulstoneJSON(noSoulstoneJSON, newCharacter, newCharacter.ranks);
    // cat strings to form filepath
    char noSoulstoneFilepath[200];
    strcat(strcpy(noSoulstoneFilepath, powerFilepath), newCharacter.name);
    char noSoulstoneRankStr[100];
    sprintf(noSoulstoneRankStr, "/%dstar/preventsouls.json", newCharacter.ranks);
    strcat(noSoulstoneFilepath, noSoulstoneRankStr);
    printf("Creating preventsouls.json at %s\n", noSoulstoneFilepath);
    FILE *noSoulstoneFile = fopen(noSoulstoneFilepath, "w");
    if (noSoulstoneFile == NULL) {
        printf("Error creating preventsouls.json file.\n");
    } else {
        // We want to prettify the JSON output for easier reading
        char *prettyNoSoulstoneString = cJSON_Print(noSoulstoneJSON);
        printf("%s\n", prettyNoSoulstoneString);
        fputs(prettyNoSoulstoneString, noSoulstoneFile);
        cJSON_free(prettyNoSoulstoneString);
        printf("preventsouls.json file created successfully at %s\n", noSoulstoneFilepath);
        fclose(noSoulstoneFile);
    }
    cJSON_Delete(noSoulstoneJSON);

    printf("Character creation completed successfully!\n");

    return 0; // normal exit
}

void createNoSoulstoneJSON(cJSON *jsonObj, Character character, int evoStage) {
    cJSON_AddStringToObject(jsonObj, "name", "No Soulstone Stuffs");
    cJSON_AddStringToObject(jsonObj, "description", "Prevents using soulstone when at max evolution stage.");
    cJSON_AddBoolToObject(jsonObj, "hidden", cJSON_True);
    cJSON_AddStringToObject(jsonObj, "type", "origins:multiple");

    // preventsoul
    cJSON *itemUsePreventObj = cJSON_CreateObject();
    cJSON_AddStringToObject(itemUsePreventObj, "type", "origins:prevent_item_use");
    // item_condition
    cJSON *itemConditionObj = cJSON_CreateObject();
    cJSON_AddStringToObject(itemConditionObj, "type", "origins:ingredient");
    cJSON *itemConditionIngredientObj = cJSON_CreateObject();
    cJSON_AddStringToObject(itemConditionIngredientObj, "item", "bisccel:soulstone");

    cJSON_AddItemToObject(itemConditionObj, "ingredient", itemConditionIngredientObj);
    cJSON_AddItemToObject(itemUsePreventObj, "item_condition", itemConditionObj);
    // Now add to main jsonObj
    cJSON_AddItemToObject(jsonObj, "preventsoul", itemUsePreventObj);
    // Soulcount action_on_callback
    cJSON *soulcountCallbackObj = cJSON_CreateObject();
    cJSON_AddStringToObject(soulcountCallbackObj, "type", "origins:action_on_callback");
    // entity_action_chosen
    cJSON *entityActionChosenObj = cJSON_CreateObject();
    cJSON_AddStringToObject(entityActionChosenObj, "type", "origins:execute_command");
    // We need to cat!
    char commandStr[200];
    sprintf(commandStr, "scoreboard players set @s bisccel.soulcount %d", evoStage); // Set to max
    cJSON_AddStringToObject(entityActionChosenObj, "command", commandStr);
    cJSON_AddItemToObject(soulcountCallbackObj, "entity_action_chosen", entityActionChosenObj);
    cJSON_AddBoolToObject(soulcountCallbackObj, "execute_chosen_when_orb", cJSON_True);

    cJSON_AddItemToObject(jsonObj, "soulcount", soulcountCallbackObj); // Add to main jsonObj
}

void createEvoJSON(cJSON *jsonObj, Character character, int evoStage) {
    cJSON_AddStringToObject(jsonObj, "name", "Soulstone Stuffs");
    cJSON_AddStringToObject(jsonObj, "description", "Handles resource bar and evolving.");
    cJSON_AddBoolToObject(jsonObj, "hidden", cJSON_True);
    cJSON_AddStringToObject(jsonObj, "type", "origins:multiple");

    cJSON *soulcountObj = cJSON_CreateObject();
    cJSON_AddStringToObject(soulcountObj, "type", "origins:resource");
    // MIN/MAX
    cJSON_AddNumberToObject(soulcountObj, "min", 0);
    cJSON_AddNumberToObject(soulcountObj, "max", 20 + (evoStage * 20)); // 20 base + 20 per rank
    // STARTING VALUE
    cJSON_AddNumberToObject(soulcountObj, "start_value", 0);

    // HUD RENDER
    cJSON *hudRenderObj = cJSON_CreateObject();
    cJSON_AddBoolToObject(hudRenderObj, "should_render", cJSON_True);
    cJSON_AddStringToObject(hudRenderObj, "sprite_location", "bisccel:textures/gui/soh_resources.png");
    cJSON_AddItemToObject(hudRenderObj, "bar_index", cJSON_CreateNumber(6)); 
    // HUD RENDER CONDITIONS
    cJSON *hudConditionsObj = cJSON_CreateObject();
    cJSON_AddStringToObject(hudConditionsObj, "type", "origins:inventory");
    cJSON_AddStringToObject(hudConditionsObj, "process_mode", "items");
    cJSON *itemConditionsObj = cJSON_CreateObject();
    cJSON_AddStringToObject(itemConditionsObj, "type", "origins:ingredient");
    cJSON *itemConditionsIngredientObj = cJSON_CreateObject();
    cJSON_AddStringToObject(itemConditionsIngredientObj, "item", "bisccel:soulstone");

    cJSON_AddItemToObject(itemConditionsObj, "ingredient", itemConditionsIngredientObj);
    cJSON_AddItemToObject(hudConditionsObj, "item_condition", itemConditionsObj);

    cJSON_AddItemToObject(hudRenderObj, "condition", hudConditionsObj);
    cJSON_AddArrayToObject(hudRenderObj, "slots"); // Empty array for slots
    cJSON_AddStringToObject(hudRenderObj, "slot", "weapon.mainhand");
    cJSON_AddStringToObject(hudRenderObj, "comparison", "!=");
    cJSON_AddItemToObject(hudRenderObj, "compare_to", cJSON_CreateNumber(0)); 
    // Now we nest
    cJSON_AddItemToObject(soulcountObj, "hud_render", hudRenderObj);
    cJSON_AddItemToObject(jsonObj, "soulcount", soulcountObj);

    // max_action
    cJSON *maxActionObj = cJSON_CreateObject();
    cJSON_AddStringToObject(maxActionObj, "type", "origins:and");
    // Actions array
    cJSON *actionsArray = cJSON_CreateArray();
    cJSON *action1 = cJSON_CreateObject();
    cJSON_AddStringToObject(action1, "type", "origins:execute_command");
    // We need to cat!
    char commandStr[200];
    sprintf(commandStr, "function bisccel:ranks/%s/%dstar", character.name, evoStage + 1);
    cJSON_AddStringToObject(action1, "command", commandStr);
    cJSON_AddItemToArray(actionsArray, action1); // Add first action

    // Second action: run command, tellraw
    cJSON *action2 = cJSON_CreateObject();
    cJSON_AddStringToObject(action2, "type", "origins:execute_command");
    char tellrawCommand[400];
    sprintf(tellrawCommand, "tellraw @s [{\"text\":\"<\"},{\"selector\":\"@s\",\"bold\":true,\"color\":\"%s\"},{\"text\":\"> PLACEHOLDER \"},{\"text\":\"\\n\"},{\"selector\":\"@s\",\"italic\":true,\"color\":\"%s\"},{\"text\":\" has upgraded to %d star!\",\"italic\":true,\"color\":\"%s\"}]", character.textColor, character.secondaryColor, evoStage + 1, character.secondaryColor);
    cJSON_AddStringToObject(action2, "command", tellrawCommand);
    cJSON_AddItemToArray(actionsArray, action2); // Add second action

    // Third action; play sound
    cJSON *action3 = cJSON_CreateObject();
    cJSON_AddStringToObject(action3, "type", "origins:play_sound");
    cJSON_AddStringToObject(action3, "sound", "minecraft:block.respawn_anchor.charge");
    cJSON_AddNumberToObject(action3, "volume", 1.0);
    cJSON_AddNumberToObject(action3, "pitch", 0.5);
    cJSON_AddItemToArray(actionsArray, action3); // Add third action

    // Fourth action: play sound
    cJSON *action4 = cJSON_CreateObject();
    cJSON_AddStringToObject(action4, "type", "origins:play_sound");
    cJSON_AddStringToObject(action4, "sound", "minecraft:minecraft:item.trident.thunder");
    cJSON_AddNumberToObject(action4, "volume", 1.0);
    cJSON_AddNumberToObject(action4, "pitch", 1.5);
    cJSON_AddItemToArray(actionsArray, action4); // Add fourth action

    // fifth action: sound effect
    cJSON *action5 = cJSON_CreateObject();
    cJSON_AddStringToObject(action5, "type", "origins:play_sound");
    cJSON_AddStringToObject(action5, "sound", "minecraft:entity.evoker.cast_spell");
    cJSON_AddNumberToObject(action5, "volume", 1.0);
    cJSON_AddNumberToObject(action5, "pitch", 0.75);
    cJSON_AddItemToArray(actionsArray, action5); // Add fifth action


    // sixth action: sound effect
    cJSON *action6 = cJSON_CreateObject();
    cJSON_AddStringToObject(action6, "type", "origins:play_sound");
    cJSON_AddStringToObject(action6, "sound", "minecraft:block.anvil.fall");
    cJSON_AddNumberToObject(action6, "volume", 1.0);
    cJSON_AddNumberToObject(action6, "pitch", 0.5);
    cJSON_AddItemToArray(actionsArray, action6); // Add sixth action

    // seventh action: spawn particle
    cJSON *action7 = cJSON_CreateObject();
    cJSON_AddStringToObject(action7, "type", "origins:spawn_particle");
    cJSON_AddStringToObject(action7, "particle", "minecraft:flame");
    cJSON_AddNumberToObject(action7, "count", 50);
    cJSON_AddNumberToObject(action7, "speed", 0.2);
    // Position object
    // We will reuse this 3 more times, so create once
    cJSON *positionObj = cJSON_CreateObject();
    cJSON_AddStringToObject(positionObj, "x", "0");
    cJSON_AddStringToObject(positionObj, "y", "0.5");
    cJSON_AddStringToObject(positionObj, "z", "0");
    cJSON_AddItemToObject(action7, "spread", positionObj);

    cJSON_AddItemToArray(actionsArray, action7); // Add seventh action

    // Eigth action: spawn particle
    cJSON *action8 = cJSON_CreateObject();
    cJSON_AddStringToObject(action8, "type", "origins:spawn_particle");
    cJSON_AddStringToObject(action8, "particle", "minecraft:end_rod");
    cJSON_AddNumberToObject(action8, "count", 20);
    cJSON_AddNumberToObject(action8, "speed", 0.2);
    cJSON_AddItemToObject(action8, "spread", cJSON_Duplicate(positionObj, 1));
    cJSON_AddItemToArray(actionsArray, action8); // Add eighth action

    // Ninth action: spawn particle
    cJSON *action9 = cJSON_CreateObject();
    cJSON_AddStringToObject(action9, "type", "origins:spawn_particle");
    cJSON_AddStringToObject(action9, "particle", "minecraft:wax_off");
    cJSON_AddNumberToObject(action9, "count", 20);
    cJSON_AddNumberToObject(action9, "speed", 10);
    cJSON_AddItemToObject(action9, "spread", cJSON_Duplicate(positionObj, 1));
    cJSON_AddItemToArray(actionsArray, action9); // Add ninth action

    cJSON_AddItemToObject(maxActionObj, "actions", actionsArray);
    cJSON_AddItemToObject(jsonObj, "max_action", maxActionObj);

    // soulincrease item
    cJSON *soulIncreaseObj = cJSON_CreateObject();
    cJSON_AddStringToObject(soulIncreaseObj, "type", "origins:action_on_item_use");
    //entity_action
    cJSON *entityActionObj = cJSON_CreateObject();
    cJSON_AddStringToObject(entityActionObj, "type", "origins:change_resource");
    // This has to be cat
    char resourceStr[100];
    sprintf(resourceStr, "bisccel:flavors/%s/%dstar/evo_soulcount", character.name, evoStage);
    cJSON_AddStringToObject(entityActionObj, "resource", resourceStr);
    cJSON_AddNumberToObject(entityActionObj, "change", 1);
    cJSON_AddStringToObject(entityActionObj, "operation", "add");

    cJSON_AddItemToObject(soulIncreaseObj, "entity_action", entityActionObj);
    // item_condition
    cJSON *itemConditionObj = cJSON_CreateObject();
    cJSON_AddStringToObject(itemConditionObj, "type", "origins:ingredient");
    cJSON *itemConditionIngredientObj2 = cJSON_CreateObject();
    cJSON_AddStringToObject(itemConditionIngredientObj2, "item", "bisccel:soulstone");

    cJSON_AddItemToObject(itemConditionObj, "ingredient", itemConditionIngredientObj2);
    cJSON_AddItemToObject(soulIncreaseObj, "item_condition", itemConditionObj);
    // trigger
    cJSON_AddStringToObject(soulIncreaseObj, "trigger", "instant");
    // priority
    cJSON_AddItemToObject(soulIncreaseObj, "priority", cJSON_CreateNumber(0));
    // Now add to main jsonObj
    cJSON_AddItemToObject(jsonObj, "soul_increase", soulIncreaseObj);

    //resetsoul
    cJSON *resetSoulObj = cJSON_CreateObject();
    cJSON_AddStringToObject(resetSoulObj, "type", "origins:action_on_callback");
    // entity_action_chosen 
    cJSON *entityActionChosenObj = cJSON_CreateObject();
    cJSON_AddStringToObject(entityActionChosenObj, "type", "origins:and");
    // Actions array
    cJSON *resetActionsArray = cJSON_CreateArray();
    cJSON *resetAction1 = cJSON_CreateObject();
    cJSON_AddStringToObject(resetAction1, "type", "origins:change_resource");
    // This has to be cat
    char resetResourceStr[200];
    sprintf(resetResourceStr, "bisccel:flavors/%s/%dstar/evo_soulcount", character.name, evoStage);
    cJSON_AddStringToObject(resetAction1, "resource", resetResourceStr);
    cJSON_AddNumberToObject(resetAction1, "change", 0);
    cJSON_AddStringToObject(resetAction1, "operation", "set");
    cJSON_AddItemToArray(resetActionsArray, resetAction1); // Add first action
    // second action: execute command
    cJSON *resetAction2 = cJSON_CreateObject();
    cJSON_AddStringToObject(resetAction2, "type", "origins:execute_command");
    // nned to assemble string
    char resetCommandStr[200];
    sprintf(resetCommandStr, "scoreboard players set @s bisccel.starcount %d", evoStage);
    cJSON_AddStringToObject(resetAction2, "command", resetCommandStr);
    cJSON_AddItemToArray(resetActionsArray, resetAction2); // Add second action

    cJSON_AddItemToObject(entityActionChosenObj, "actions", resetActionsArray);
    cJSON_AddItemToObject(resetSoulObj, "entity_action_chosen", entityActionChosenObj);

    cJSON_AddBoolToObject(resetSoulObj, "execute_chosen_when_orb", cJSON_True);

    cJSON_AddItemToObject(jsonObj, "reset_soul", resetSoulObj);

}