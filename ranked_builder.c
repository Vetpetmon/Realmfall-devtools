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
    

    cJSON *evo0JSON = cJSON_CreateObject();

    createEvoJSON(evo0JSON, newCharacter, 0);

    char *evo0String = cJSON_PrintUnformatted(evo0JSON);
    // cat strings to form filepath
    char evo0Filepath[200];
    strcat(strcpy(evo0Filepath, powerFilepath), newCharacter.name);
    strcat(evo0Filepath, "/0star/evo.json");

    // Print contents of filepath for debugging
    printf("Generated JSON content:\n%s\n", evo0Filepath);
    
    FILE *evo0File = fopen(evo0Filepath, "w");
    if (evo0File == NULL) {
        printf("Error creating evo.json file.\n");
        // Free allocated memory
        free(newCharacter.name);
        free(newCharacter.textColor);
        free(newCharacter.secondaryColor);
        cJSON_Delete(evo0JSON);
        free(evo0String);
        return 1; // indicate error
    }
    // We want to prettify the JSON output for easier reading
    char *prettyEvo0String = cJSON_Print(evo0JSON);

    printf("%s\n", prettyEvo0String);
    fputs(prettyEvo0String, evo0File);
    fclose(evo0File);
    cJSON_free(prettyEvo0String);
    free(evo0String);
    cJSON_Delete(evo0JSON);

    return 0; // normal exit
}

void createEvoJSON(cJSON *jsonObj, Character character, int evoStage) {
    cJSON_AddStringToObject(jsonObj, "name", "Soulstone Stuffs");
    cJSON_AddStringToObject(jsonObj, "description", "Handles resource bar and evolving.");
    cJSON_AddBoolToObject(jsonObj, "hidden", cJSON_True);
    cJSON_AddStringToObject(jsonObj, "type", "origins:multiple");
    // cJSON_AddItemToObject(jsonObj, "soulcount",);

}