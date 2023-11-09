#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

char isSignIn[100];
struct User
{
    char username[100];
    char password[100];
    int status;
    // 1: active, 0: blocked, 2: idle
    struct User *next;
};

struct User *readUserInfoFromFile()
{
    FILE *file = fopen("nguoidung.txt", "r");

    if (!file)
    {
        printf("Error opening file.\n");
        exit(EXIT_FAILURE);
    }

    struct User *head = NULL;
    struct User *tail = NULL;
    char username[100];
    char password[100];
    int status;

    while (fscanf(file, "%s %s %d", username, password, &status) == 3)
    {
        struct User *newUser = (struct User *)malloc(sizeof(struct User));
        if (!newUser)
        {
            printf("Memory allocation failed.\n");
            exit(EXIT_FAILURE);
        }
        strcpy(newUser->username, username);
        strcpy(newUser->password, password);
        newUser->status = status;
        newUser->next = NULL;

        if (head == NULL)
        {
            head = newUser;
            tail = newUser;
        }
        else
        {
            tail->next = newUser;
            tail = newUser;
        }
    }
    fclose(file);
    return head;
}

void updateFile(struct User *head)
{
    FILE *file = fopen("nguoidung.txt", "w");

    if (!file)
    {
        printf("Error opening file.\n");
        exit(EXIT_FAILURE);
    }

    struct User *current = head;
    while (current != NULL)
    {
        fprintf(file, "%s %s %d\n", current->username, current->password, current->status);
        current = current->next;
    }

    fclose(file);
}

void registerUser(struct User **head)
{
    char username[100], password[100];

    printf("Enter username: ");
    scanf("%s", username);

    // Check if username already exists
    struct User *current = *head;
    while (current != NULL)
    {
        if (strcmp(current->username, username) == 0)
        {
            printf("Account existed.\n");
            return;
        }
        current = current->next;
    }

    printf("Enter password: ");
    scanf("%s", password);

    // Create a new user node
    struct User *newUser = (struct User *)malloc(sizeof(struct User));
    if (!newUser)
    {
        printf("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    strcpy(newUser->username, username);
    strcpy(newUser->password, password);
    newUser->status = 2; // Set status to idle
    newUser->next = *head;
    *head = newUser;

    // Update file with new user information
    FILE *file = fopen("nguoidung.txt", "a");
    if (file == NULL)
    {
        printf("Error opening file.\n");
        return;
    }
    fprintf(file, "%s %s %d\n", username, password, 2); // 2 for idle status
    fclose(file);

    printf("Successful registration. Activation required.\n");
}

void activateUser(struct User *head)
{
    char username[100], password[100], activationCode[100];
    int maxAttempts = 0;
    // Prompt for username, password, and activation code
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);

    // Find the user
    struct User *current = head;
    while (current != NULL)
    {
        if (strcmp(current->username, username) == 0)
        {
            if (strcmp(current->password, password) == 0)
            {
                while (maxAttempts < 4)
                {
                    printf("Enter activation code: ");
                    scanf("%s", &activationCode);
                    if (strcmp(activationCode, "20204994") == 0)
                    {
                        current->status = 1; // Set status to active
                        printf("Account is activated\n");
                        updateFile(head);
                        maxAttempts = 0;
                        return;
                    }
                    else
                    {
                        printf("Activation code is incorrect.\n");
                    }
                    maxAttempts++;
                }
                if (maxAttempts == 4)
                {
                    printf("Activation code is incorrect. \nAccount is blocked\n");
                    current->status = 0;
                    updateFile(head);
                    return;
                }
                maxAttempts = 0;
            }
        }
        current = current->next;
    }
    printf("Username or password is wrong");
}

void signIn(struct User *head)
{
    char username[100], password[100];
    int maxAttempts = 3;

    printf("Enter username: ");
    scanf("%s", username);
    struct User *current = head;
    while (current != NULL)
    {
        if (strcmp(current->username, username) == 0)
        {
            if (current->status == 0)
            {
                printf("Account is blocked.\n");
                return;
            }
            int attempts = 0;
            while (attempts < 3)
            {
                printf("Enter password: ");
                scanf("%s", password);
                if (strcmp(current->password, password) == 0)
                {
                    if (current->status == 1)
                    {
                        strcpy(isSignIn, username);
                        printf("Hello %s\n", &username);
                    }
                    else
                    {
                        printf("User signed in successfully!\n");
                    }
                    return;
                }
                else
                {
                    attempts++;
                    printf("Password is incorrect\n");
                }
            }
            if (attempts >= 3)
            {
                current->status = 0; // Set status to blocked
                updateFile(head);
                printf("Too many incorrect attempts. Account blocked.\n");
                return;
            }
        }
        current = current->next;
    }

    printf("Cannot find account.\n");
}

void searchUser(struct User *head)
{
    char username[100];

    // Prompt for username
    printf("Enter username to search: ");
    scanf("%s", username);

    // Find the user
    struct User *current = head;
    while (current != NULL)
    {
        if (strcmp(current->username, username) == 0)
        {
            printf("Username: %s\n", current->username);
            if (current->status == 1)
            {
                printf("Status: active\n");
            }
            else if (current->status == 0)
            {
                printf("Status: blocked\n");
            }
            else
            {
                printf("Status: idle\n");
            }
            return;
        }
        current = current->next;
    }

    printf("User not found.\n");
}

void changePassword(struct User *head)
{
    char username[100], currentPassword[100], newPassword[100];

    printf("Enter username: ");
    scanf("%s", username);
    if (strcmp(isSignIn, "") == 0)
    {
        printf("Account is not sign in");
        return;
    }
    if (strcmp(isSignIn, username) != 0)
    {
        printf("Cannot find account");
        return;
    }
    printf("Enter current password: ");
    scanf("%s", currentPassword);
    printf("Enter new password: ");
    scanf("%s", newPassword);

    // Find the user
    struct User *current = head;
    while (current != NULL)
    {
        if (strcmp(current->username, username) == 0)
        {
            if (strcmp(current->password, currentPassword) == 0)
            {
                strcpy(current->password, newPassword);
                updateFile(head);
                printf("Password changed successfully!\n");
                return;
            }
            else
            {
                printf("Current password is incorrect. Please try again\n");
                return;
            }
        }
        current = current->next;
    }

    printf("User not found.\n");
}

void signOut(struct User *head)
{
    char username[100];

    // Prompt for username
    printf("Enter username: ");
    scanf("%s", username);
    if (strcmp(isSignIn, "") == 0)
    {
        printf("Account is not sign in");
        return;
    }
    if (strcmp(isSignIn, username) != 0)
    {
        printf("Cannot find account");
        return;
    }
    strcpy(isSignIn, "");
    printf("Goodbye hust");
}

int Menu()
{
    struct User *userList = readUserInfoFromFile();
    int choice;
    bool loop = true;
    do
    {
        printf("\n\nUSER MANAGEMENT PROGRAM\n-------------------------------\n1. Register\n2. Activate\n3. Sign in\n4. Search\n5. Change password\n6. Sign out\nYour choice (1-6, other to quit): ");
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
        {
            registerUser(&userList);
            break;
        }
        case 2:
        {
            activateUser(userList);
            break;
        }
        case 3:
        {
            signIn(userList);
            break;
        }
        case 4:
        {
            searchUser(userList);
            break;
        }
        case 5:
        {
            changePassword(userList);
            break;
        }
        case 6:
        {
            signOut(userList);
            break;
        }
        default:
            loop = false;
            break;
        }
    } while (loop);
    printf("See you again!!!");
    return 0;
}

void main()
{
    strcpy(isSignIn, "");
    Menu();
}