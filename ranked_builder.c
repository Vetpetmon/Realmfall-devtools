#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <errno.h>
#include <limits.h>
#include "rfcharacters.h"
#include "cjson/cJSON.h" // Include cJSON library for JSON handling

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
            if (ag_mkdir(tmp, mode) != 0) {
                if (errno != EEXIST) {
                    *p = '/';
                    return -1;
                }
            }
            *p = '/';
        }
    }

    // Create final path
    if (ag_mkdir(tmp, mode) != 0) {
        if (errno != EEXIST) {
            return -1;
        }
    }
    return 0;
}


// Helper implementation: create a play_sound action object
cJSON *create_play_sound_action(const char *sound, double volume, double pitch) {
    cJSON *action = cJSON_CreateObject();
    cJSON_AddStringToObject(action, "type", "origins:play_sound");
    cJSON_AddStringToObject(action, "sound", sound);
    cJSON_AddNumberToObject(action, "volume", volume);
    cJSON_AddNumberToObject(action, "pitch", pitch);
    return action;
}


// Helper implementation: create an execute_command action object
cJSON *create_execute_command_action(const char *command) {
    cJSON *action = cJSON_CreateObject();
    cJSON_AddStringToObject(action, "type", "origins:execute_command");
    cJSON_AddStringToObject(action, "command", command);
    return action;
}


// Helper implementation: create a change_resource action object
cJSON *create_change_resource_action(const char *resource, double change, const char *operation) {
    cJSON *action = cJSON_CreateObject();
    cJSON_AddStringToObject(action, "type", "origins:change_resource");
    cJSON_AddStringToObject(action, "resource", resource);
    cJSON_AddNumberToObject(action, "change", change);
    cJSON_AddStringToObject(action, "operation", operation);
    return action;
}


// Helper implementation: create a spawn_particles action object
cJSON *create_spawn_particles_action(const char *particle, int count, double speed, cJSON *spread, int duplicate_spread) {
    cJSON *action = cJSON_CreateObject();
    cJSON_AddStringToObject(action, "type", "origins:spawn_particles");
    cJSON_AddStringToObject(action, "particle", particle);
    cJSON_AddNumberToObject(action, "count", count);
    cJSON_AddNumberToObject(action, "speed", speed);
    if (spread != NULL) {
        if (duplicate_spread) {
            cJSON_AddItemToObject(action, "spread", cJSON_Duplicate(spread, 1));
        } else {
            cJSON_AddItemToObject(action, "spread", spread);
        }
    }
    return action;
}

int main() {
    // Initialize the main 4 classes
    CharacterClass meleeClass = {2, 1, 0.08, 0.0, 0.0, 0.0}; // Melee class
    CharacterClass rangedClass = {1, 1, 0.0, 0.08, 0.0, 0.0}; // Ranged class
    CharacterClass defenseClass = {2, 2, 0.0, 0.0, 0.0, 0.05}; // Defense class
    CharacterClass mageClass = {1, 1, 0.0, 0.0, 0.08, 0.0}; // Mage class

    // Strings for File paths
    char powerFilepath[100] = "powers/flavors/";
    char originFilepath[100] = "origins/";
    char rankedFilepath[100] = "origins/ranks/";

    // Veriables used to store user selections
    CharacterClass selectedClass;
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
    newCharacter.charClass = selectedClass;

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
    printf("\tHealth: +%d, \n", newCharacter.charClass.healthPerRank);
    printf("\tArmor: +%d, \n", newCharacter.charClass.armorPerRank);
    printf("\tMelee Damage: +%.2f%%, \n", newCharacter.charClass.meleeDamagePerRank * 100);
    printf("\tRanged Damage: +%.2f%%, \n", newCharacter.charClass.rangedDamagePerRank * 100);
    printf("\tGeneral Damage: +%.2f%%, \n", newCharacter.charClass.generalDamagePerRank * 100);
    printf("\tDamage Resistance: +%.2f%%\n", newCharacter.charClass.damageResistancePerRank * 100);
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
    for (int i = 0; i < newCharacter.ranks+1; i++) {
        if (i < newCharacter.ranks) {
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
            //printf("%s\n", prettyEvoString);
            fputs(prettyEvoString, evoFile);
            cJSON_free(prettyEvoString);
            printf("evo.json file created successfully at %s\n", evoFilepath);
            fclose(evoFile);
            cJSON_Delete(evoJSON);
        }

        // Repeat for createRankOriginJSON
        char originRankDir[256];
        sprintf(originRankDir, "%s%s/%dstar", rankedFilepath, newCharacter.name, i);
        if (mkdir_p(originRankDir, 0755) != 0) {
            perror("Error creating origin rank directory");
        }
        cJSON *rankOriginJSON = cJSON_CreateObject();
        createRankOriginJSON(rankOriginJSON, newCharacter, i);
        // cat strings to form filepath
        char originFilepathFull[200];
        strcat(strcpy(originFilepathFull, rankedFilepath), newCharacter.name);
        char originRankStr[100];
        sprintf(originRankStr, "/%dstar.json", i);
        strcat(originFilepathFull, originRankStr);
        FILE *originFile = fopen(originFilepathFull, "w");
        if (originFile == NULL) {
            printf("Error creating origin rank JSON file for rank %d.\n", i);
            cJSON_Delete(rankOriginJSON);
            continue; // skip to next rank
        }
        char *prettyOriginString = cJSON_Print(rankOriginJSON);
        fputs(prettyOriginString, originFile);
        cJSON_free(prettyOriginString);
        printf("Origin rank JSON file created successfully at %s\n", originFilepathFull);
        fclose(originFile);
        cJSON_Delete(rankOriginJSON);

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
        char *prettyNoSoulstoneString = cJSON_Print(noSoulstoneJSON);
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
    // entity_action_chosen (execute command)
    char commandStr[200];
    sprintf(commandStr, "scoreboard players set @s bisccel.soulcount %d", evoStage); // Set to max
    cJSON *entityActionChosenObj = create_execute_command_action(commandStr);
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
    
    cJSON_AddArrayToObject(hudConditionsObj, "slots"); // Empty array for slots
    cJSON_AddStringToObject(hudConditionsObj, "slot", "weapon.mainhand");
    cJSON_AddStringToObject(hudConditionsObj, "comparison", "!=");
    cJSON_AddItemToObject(hudConditionsObj, "compare_to", cJSON_CreateNumber(0)); 

    cJSON_AddItemToObject(hudRenderObj, "condition", hudConditionsObj);
    // Now we nest
    cJSON_AddItemToObject(soulcountObj, "hud_render", hudRenderObj);
    cJSON_AddItemToObject(jsonObj, "soulcount", soulcountObj);

    // max_action
    cJSON *maxActionObj = cJSON_CreateObject();
    cJSON_AddStringToObject(maxActionObj, "type", "origins:and");
    // Actions array
    cJSON *actionsArray = cJSON_CreateArray();
    // First action: execute command (set origin rank)
    char commandStr[300];
    sprintf(commandStr, "origin set @s bisccel:rank bisccel:ranks/%s/%dstar", character.name, evoStage + 1);
    cJSON *action1 = create_execute_command_action(commandStr);
    cJSON_AddItemToArray(actionsArray, action1);

    // Second action: run command, tellraw
    // Second action: tellraw (execute command)
    char tellrawCommand[400];
    sprintf(tellrawCommand, "tellraw @s [{\"text\":\"<\"},{\"selector\":\"@s\",\"bold\":true,\"color\":\"%s\"},{\"text\":\"> PLACEHOLDER \"},{\"text\":\"\\n\"},{\"selector\":\"@s\",\"italic\":true,\"color\":\"%s\"},{\"text\":\" has upgraded to %d star!\",\"italic\":true,\"color\":\"%s\"}]", character.textColor, character.secondaryColor, evoStage + 1, character.secondaryColor);
    cJSON *action2 = create_execute_command_action(tellrawCommand);
    cJSON_AddItemToArray(actionsArray, action2);

    // Third action; play sound
    cJSON *action3 = create_play_sound_action("minecraft:block.respawn_anchor.charge", 1.0, 0.5);
    cJSON_AddItemToArray(actionsArray, action3);
    cJSON *action4 = create_play_sound_action("minecraft:item.trident.thunder", 1.0, 1.5);
    cJSON_AddItemToArray(actionsArray, action4);
    cJSON *action5 = create_play_sound_action("minecraft:entity.evoker.cast_spell", 1.0, 0.75);
    cJSON_AddItemToArray(actionsArray, action5);
    cJSON *action6 = create_play_sound_action("minecraft:block.anvil.fall", 1.0, 0.5);
    cJSON_AddItemToArray(actionsArray, action6); 

    // Position object for particles (reused)
    cJSON *positionObj = cJSON_CreateObject();
    cJSON_AddNumberToObject(positionObj, "x", 0);
    cJSON_AddNumberToObject(positionObj, "y", 0.5);
    cJSON_AddNumberToObject(positionObj, "z", 0);

    cJSON *action7 = create_spawn_particles_action("minecraft:flame", 50, 0.2, positionObj, 0);
    cJSON_AddItemToArray(actionsArray, action7);
    cJSON *action8 = create_spawn_particles_action("minecraft:end_rod", 20, 0.2, positionObj, 1);
    cJSON_AddItemToArray(actionsArray, action8);
    cJSON *action9 = create_spawn_particles_action("minecraft:wax_off", 20, 10, positionObj, 1);
    cJSON_AddItemToArray(actionsArray, action9); 

    // cJSON_AddItemToObject(maxActionObj, "actions", actionsArray);
    // attach maxActionObj to soulcountObj
    cJSON_AddItemToObject(maxActionObj, "actions", actionsArray);
    cJSON_AddItemToObject(soulcountObj, "max_action", maxActionObj);
    // Now add soulcountObj to main jsonObj
    cJSON_AddItemToObject(jsonObj, "soulcount", soulcountObj);

    // soulincrease item
    cJSON *soulIncreaseObj = cJSON_CreateObject();
    cJSON_AddStringToObject(soulIncreaseObj, "type", "origins:action_on_item_use");
    //entity_action
    // entity_action: change_resource
    char resourceStr[100];
    sprintf(resourceStr, "bisccel:flavors/%s/%dstar/evo_soulcount", character.name, evoStage);
    cJSON *entityActionObj = create_change_resource_action(resourceStr, 1, "add");
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
    // first action: change_resource (reset)
    char resetResourceStr[200];
    sprintf(resetResourceStr, "bisccel:flavors/%s/%dstar/evo_soulcount", character.name, evoStage);
    cJSON *resetAction1_dup = create_change_resource_action(resetResourceStr, 0, "set");
    cJSON_AddItemToArray(resetActionsArray, resetAction1_dup); // Add first action
    // second action: execute command (set starcount)
    char resetCommandStr[200];
    sprintf(resetCommandStr, "scoreboard players set @s bisccel.starcount %d", evoStage);
    cJSON *resetAction2 = create_execute_command_action(resetCommandStr);
    cJSON_AddItemToArray(resetActionsArray, resetAction2); // Add second action

    cJSON_AddItemToObject(entityActionChosenObj, "actions", resetActionsArray);
    cJSON_AddItemToObject(resetSoulObj, "entity_action_chosen", entityActionChosenObj);

    cJSON_AddBoolToObject(resetSoulObj, "execute_chosen_when_orb", cJSON_True);

    cJSON_AddItemToObject(jsonObj, "reset_soul", resetSoulObj);

}

void createRankOriginJSON(cJSON *jsonObj, Character character, int evoStage) {

    const char *starFilled = "★"; // Unicode for filled star
    const char *starEmpty = "☆"; // Unicode for empty star

    char nameStr[100];
    // Example for 1 star: "[★☆☆☆☆☆]" for 6 ranks
    strcpy(nameStr, "[");
    for (int i = 0; i < character.ranks; i++) {
        const char *star = (i <= evoStage-1) ? starFilled : starEmpty;
        strncat(nameStr, star, sizeof(nameStr) - strlen(nameStr) - 1);
    }
    strcat(nameStr, "]");
    cJSON_AddStringToObject(jsonObj, "name", nameStr);
    char descriptionStr[200];
    sprintf(descriptionStr, "Collect %d Lesser Soulstones to upgrade", 20 + (evoStage * 20));
    cJSON_AddStringToObject(jsonObj, "description", descriptionStr);
    
    // powers array
    cJSON *powersArray = cJSON_CreateArray();
    // Add preventsouls power only if at max rank
    if (evoStage == character.ranks) {
        char noSoulPowerStr[300];
        sprintf(noSoulPowerStr, "bisccel:flavors/%s/%dstar/preventsouls", character.name, evoStage);
        cJSON_AddItemToArray(powersArray, cJSON_CreateString(noSoulPowerStr));
    } // else add evo power 
    else {
        char nextEvoPowerStr[200];
        sprintf(nextEvoPowerStr, "bisccel:flavors/%s/%dstar/evo", character.name, evoStage);
        cJSON_AddItemToArray(powersArray, cJSON_CreateString(nextEvoPowerStr));
    }
    // Add to main jsonObj
    cJSON_AddItemToObject(jsonObj, "powers", powersArray);

    // Icon obj
    cJSON *iconObj = cJSON_CreateObject();
    cJSON_AddStringToObject(iconObj, "item", "bisccel:soulstone");
    cJSON_AddItemToObject(jsonObj, "icon", iconObj);

    cJSON_AddBoolToObject(jsonObj, "unchoosable", cJSON_True);
    cJSON_AddNumberToObject(jsonObj, "impact", 0);

}