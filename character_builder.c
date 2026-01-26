#include "ranked_builder.h"
#include "rfcharacters.h"
#include "cjson/cJSON.h" // Include cJSON library for JSON handling
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

// Enum for menu choices
enum MenuChoice {
    CREATE_CHARACTER = 1,
    GENERATE_FILES = 2,
    EXIT = 3
};

// Get user input to create a new Character
Character get_user_input_character();

// Add a Character to the characters array
Character **add_character(Character **characters, size_t *character_count, Character new_character);

// Clear screen function (platform-independent)
void clear_screen();

// Clear screen implementation
void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int main() {
    // Array of characters to build
    Character **characters = NULL;
    size_t character_count = 0;

    // loop and ask user if they want to create a new character, generate files, or exit
    while (1) {
        printf("\nCharacter Builder Menu:\n");
        printf("1. Create a new character\n");
        printf("2. Generate character files\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        int choice;
        scanf("%d", &choice);
        // Clear input buffer
        while (getchar() != '\n');
        if (choice == CREATE_CHARACTER) {
            Character newCharacter = get_user_input_character();
            // If the user cancelled during input, name will be NULL
            if (newCharacter.name == NULL) {
                printf("Character input cancelled; no character was added.\n");
            } else {
                characters = add_character(characters, &character_count, newCharacter);
                // Free temporary character fields in the returned struct (they were duplicated)
                free(newCharacter.name);
                free(newCharacter.displayName);
                free(newCharacter.textColor);
                free(newCharacter.secondaryColor);
            }
            clear_screen();
        } else if (choice == GENERATE_FILES) {
            // Generate character files for all characters in the array
            for (size_t i = 0; i < character_count; i++) {
                // Generate files for each character using the ranked_builder generator
                generate_character_files(*characters[i]);
            }
            printf("Character files generated for %zu characters.\n", character_count);
            // Clear screen after generation
            clear_screen();
        } else if (choice == EXIT) {
            // Free allocated characters
            for (size_t i = 0; i < character_count; i++) {
                free(characters[i]->name);
                free(characters[i]->displayName);
                free(characters[i]->textColor);
                free(characters[i]->secondaryColor);
                free(characters[i]);
            }
            free(characters);
            break;
        } else {
            printf("Invalid choice. Please try again.\n");
        }
    }
    

    return 0;
}

// Get user input to create a new Character
Character get_user_input_character() {
    Character newCharacter;
    // Initialize to NULL
    newCharacter.name = NULL;
    newCharacter.displayName = NULL;
    newCharacter.textColor = NULL;
    newCharacter.secondaryColor = NULL;
    newCharacter.ranks = 0;
    // Get user input for each field
    // Buffers for input
    char nameBuffer[128];
    char colorBuffer[32];
    // Predefined classes copied from ranked_builder defaults
    CharacterClass meleeClass = {2, 1, 0.08, 0.0, 0.0, 0.0};
    CharacterClass rangedClass = {1, 1, 0.0, 0.08, 0.0, 0.0};
    CharacterClass defenseClass = {2, 2, 0.0, 0.0, 0.0, 0.05};
    CharacterClass mageClass = {1, 1, 0.0, 0.0, 0.08, 0.0};
    CharacterClass selectedClass = meleeClass;

    // Name: must be lowercase letters and underscores only, underscores not at start/end
    while (1) {
        printf("Enter a name for your character (max 30 characters, lowercase and underscores only): ");
        if (!fgets(nameBuffer, sizeof(nameBuffer), stdin)) break;
        nameBuffer[strcspn(nameBuffer, "\n")] = '\0';
        if (strlen(nameBuffer) == 0) {
            printf("Name cannot be empty. Please try again.\n");
            continue;
        } else if(strlen(nameBuffer) > 30) {
            printf("Name too long. Maximum 30 characters allowed.\n");
            continue;
        }
        int valid = 1;
        size_t len = strlen(nameBuffer);
        for (size_t i = 0; i < len; i++) {
            char c = nameBuffer[i];
            if (!((c >= 'a' && c <= 'z') || c == '_')) { valid = 0; break; }
            if (c == '_' && (i == 0 || i == len - 1)) { valid = 0; break; }
        }
        if (!valid) {
            printf("Invalid name. Use only lowercase letters and underscores (not at start or end).\n");
            continue;
        }
        break;
    }
    newCharacter.name = strdup(nameBuffer);

    // Display name
    printf("Enter a display name for your character (proper capitalization and spaces allowed): ");
    if (fgets(nameBuffer, sizeof(nameBuffer), stdin)) {
        nameBuffer[strcspn(nameBuffer, "\n")] = '\0';
    } else {
        nameBuffer[0] = '\0';
    }
    newCharacter.displayName = strdup(nameBuffer);

    // Text color
    while (1) {
        printf("Enter a hex color code for your character's text color (e.g., #7fffd4): ");
        if (!fgets(colorBuffer, sizeof(colorBuffer), stdin)) break;
        colorBuffer[strcspn(colorBuffer, "\n")] = '\0';
        if (colorBuffer[0] != '#') { printf("Missing '#' at start.\n"); continue; }
        if (strlen(colorBuffer) != 7) { printf("Color must be 7 characters (including '#').\n"); continue; }
        int valid = 1;
        for (int i = 1; i < 7; i++) {
            char c = colorBuffer[i];
            if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) { valid = 0; break; }
        }
        if (!valid) { printf("Invalid hex digits.\n"); continue; }
        break;
    }
    newCharacter.textColor = strdup(colorBuffer);

    // Secondary color
    while (1) {
        printf("Enter a hex color code for your character's secondary color (e.g., #7fffd4): ");
        if (!fgets(colorBuffer, sizeof(colorBuffer), stdin)) break;
        colorBuffer[strcspn(colorBuffer, "\n")] = '\0';
        if (colorBuffer[0] != '#') { printf("Missing '#' at start.\n"); continue; }
        if (strlen(colorBuffer) != 7) { printf("Color must be 7 characters (including '#').\n"); continue; }
        int valid = 1;
        for (int i = 1; i < 7; i++) {
            char c = colorBuffer[i];
            if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) { valid = 0; break; }
        }
        if (!valid) { printf("Invalid hex digits.\n"); continue; }
        break;
    }
    newCharacter.secondaryColor = strdup(colorBuffer);

    // Class selection
    while (1) {
        printf("Select a class for your character:\n1. Melee\n2. Ranged\n3. Defense\n4. Mage\nEnter choice: ");
        int classChoice = 0;
        if (scanf("%d", &classChoice) != 1) { while (getchar() != '\n'); continue; }
        while (getchar() != '\n');
        switch (classChoice) {
            case 1: selectedClass = meleeClass; break;
            case 2: selectedClass = rangedClass; break;
            case 3: selectedClass = defenseClass; break;
            case 4: selectedClass = mageClass; break;
            default: printf("Invalid choice.\n"); continue;
        }
        break;
    }
    newCharacter.charClass = selectedClass;

    // Number of ranks (5 or 6)
    while (1) {
        printf("Enter the number of ranks for your character (5 or 6): ");
        int ranks = 0;
        if (scanf("%d", &ranks) != 1) { while (getchar() != '\n'); continue; }
        while (getchar() != '\n');
        if (ranks == 5 || ranks == 6) { newCharacter.ranks = ranks; break; }
        printf("Invalid number of ranks. Please enter 5 or 6.\n");
    }

    // Summary and confirmation
    printf("\nCharacter Summary:\n");
    printf("Name: %s\n", newCharacter.name);
    printf("Display Name: %s\nText Color: %s\nSecondary Color: %s\n", newCharacter.displayName, newCharacter.textColor, newCharacter.secondaryColor);
    printf("Number of Ranks: %d\n", newCharacter.ranks);
    printf("Class Stats per Rank:\n");
    printf("\tHealth: +%d\n", newCharacter.charClass.healthPerRank);
    printf("\tArmor: +%d\n", newCharacter.charClass.armorPerRank);
    printf("\tMelee Damage: +%.2f%%\n", newCharacter.charClass.meleeDamagePerRank * 100);
    printf("\tRanged Damage: +%.2f%%\n", newCharacter.charClass.rangedDamagePerRank * 100);
    printf("\tGeneral Damage: +%.2f%%\n", newCharacter.charClass.generalDamagePerRank * 100);
    printf("\tDamage Resistance: +%.2f%%\n", newCharacter.charClass.damageResistancePerRank * 100);
    printf("Are these details correct? (y/n): ");
    char confirm = 'n';
    if (scanf(" %c", &confirm) == 1) { confirm = tolower(confirm); }
    while (getchar() != '\n');
    if (confirm != 'y') {
        printf("Cancelled. Returning empty character.\n");
        // Free any allocated fields
        free(newCharacter.name); free(newCharacter.displayName);
        free(newCharacter.textColor); free(newCharacter.secondaryColor);
        // Return an empty placeholder (caller should ignore if name == NULL)
        Character empty = {0};
        empty.name = NULL;
        return empty;
    }

    return newCharacter;
}

// Add a Character to the characters array
Character **add_character(Character **characters, size_t *character_count, Character new_character) {
    

    // Reallocate the characters array to hold one more character
    Character **temp = realloc(characters, sizeof(Character *) * (*character_count + 1));
    if (!temp) {
        fprintf(stderr, "Memory reallocation failed\n");
        return characters; // Return original array on failure
    }
    characters = temp;
    // Allocate memory for the new character and copy data
    characters[*character_count] = malloc(sizeof(Character));
    if (!characters[*character_count]) {
        fprintf(stderr, "Memory allocation for new character failed\n");
        return characters; // Return original array on failure
    }
    // allocate for name and displayName and colors
    characters[*character_count]->name = strdup(new_character.name);
    characters[*character_count]->displayName = strdup(new_character.displayName);
    characters[*character_count]->textColor = strdup(new_character.textColor);
    characters[*character_count]->secondaryColor = strdup(new_character.secondaryColor);
    characters[*character_count]->ranks = new_character.ranks;
    characters[*character_count]->charClass = new_character.charClass;
    // Increment character count
    (*character_count)++;
    return characters;
}