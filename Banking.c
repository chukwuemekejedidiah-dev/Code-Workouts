#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define FILENAME "dataBase.txt"

// Define Account structure
struct Account {
    char name[50];
    char phone[20];
    char password[20];
    char pin[5];
    char account_number[20]; // Changed to string to store without leading zero
    float balance;
};




// Create account number from phone number
int createAccountNumber(const char *phone, char *account, const char *password, const char *pin) {
    int len = strlen(phone);
    if (len != 11) return 0; // Invalid length
    for (int i = 0; i < len; i++) {
        if (!isdigit((unsigned char)phone[i])) return 0;
    }
    if (phone[0] != '0') return 0; // Must start with 0

    strcpy(account, phone + 1); // Remove leading zero
    return 1;
}




// Validate phone number format
int isValidPhoneNumber(const char *phone) {
    int len = strlen(phone);
    if (len != 11) return 0;
    for (int i = 0; i < len; i++) {
        if (!isdigit((unsigned char)phone[i])) return 0;
    }
    return phone[0] == '0';
}




// Validate name format
int isValidName(const char *name) {
    int len = strlen(name);
    if (len == 0) return 0;
    for (int i = 0; i < len; i++) {
        if (!isalpha((unsigned char)name[i]) && name[i] != ' ') return 0;
    }
    return 1;
}




// Clear input buffer
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}





// Check balance of an account
void check_balance(const char *acc_num) {
    FILE *file = fopen(FILENAME, "rb");
    if (!file) {
        printf("Error opening file.\n");
        return;
    }

    // Read account details
    struct Account acc;
    while (fread(&acc, sizeof(struct Account), 1, file) == 1) {
        if (strcmp(acc.account_number, acc_num) == 0) {
            printf("Current Balance: ₦%.2f\n", acc.balance);
            fclose(file);
            return;
        
        }
    }
    printf("Account not found.\n");
    fclose(file);
}




// Create a new account
void create_account() {
    struct Account acc;
    char phone[20];
    int pin;
    char password[20];
    char acc_name[20];

    // Get full name
    printf("Enter full name: ");
    clearInputBuffer();
    fgets(acc.name, sizeof(acc.name), stdin);
    acc.name[strcspn(acc.name, "\n")] = '\0'; // Remove newline




   // Validate name
    if (!isValidName(acc.name)) {
        printf("Invalid name. Only letters and spaces allowed.\n");
        return;
    }

    // Get phone number
    printf("Enter phone number (e.g., 08012345678): ");
    scanf("%19s", phone);

    // Validate phone number
    if (!isValidPhoneNumber(phone)) {
        printf("Invalid phone number.\n");
        return;
    }

    // Set password
    printf("Set a password: ");
    clearInputBuffer();
    fgets(acc.password, sizeof(acc.password), stdin);
    acc.password[strcspn(acc.password, "\n")] = '\0'; // Remove newline
    if (strlen(acc.password) < 6) {
        printf("Password must be at least 6 characters long.\n");
        return;
    }


    // Set PIN and validate
   printf("Set a 4-digit PIN: ");
    if (scanf("%d", &pin) != 1 || pin < 1000 || pin > 9999) {
        printf("Invalid PIN format.\n");
        return;
    }

    // Create account number
    if (!createAccountNumber(phone, acc.account_number, acc.password, acc.pin)) {
        printf("Failed to create account number.\n");
        return;
    }

    // Initialize account details
    strcpy(acc.phone, phone);
    acc.balance = 0.0;


    // Save account to file
    FILE *file = fopen(FILENAME, "ab");
    if (!file) {
        printf("Error opening file.\n");
        return;
    }
    fwrite(&acc, sizeof(struct Account), 1, file);
    fclose(file);

    printf("Account created successfully!\n");
    printf("Account Number: %s\n", acc.account_number);
}




//Find account by username
int find_account_by_username(const char *username, struct Account *acc_out) {
    FILE *file = fopen(FILENAME, "rb");
    if (!file) return 0;

    struct Account acc;
    while (fread(&acc, sizeof(struct Account), 1, file) == 1) {
        if (strcmp(acc.name, username) == 0) {
            *acc_out = acc;
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}



// Update balance of an account
int update_balance(const char *acc_num, float amount, int add) {
    FILE *file = fopen(FILENAME, "rb+");
    if (!file) return 0;

    struct Account acc;
    while (fread(&acc, sizeof(struct Account), 1, file) == 1) {
        if (strcmp(acc.account_number, acc_num) == 0) {
            if (!add && acc.balance < amount) {
                fclose(file);
                return 0;
            }
            fseek(file, sizeof(struct Account), SEEK_CUR);
            acc.balance += add ? amount : -amount;
            fwrite(&acc, sizeof(struct Account), 1, file);
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}





// Transfer amount between accounts
int transfer(const char *from_acc_num, const char *to_acc_num, float amount) {
    struct Account from_acc;
    if (!find_account_by_username(from_acc_num, &from_acc)) return 0;

    struct Account to_acc;
    if (!find_account_by_username(to_acc_num, &to_acc)) return 0;

    if (from_acc.balance < amount) return 0;

 
 
    // Update balances
    update_balance(from_acc_num, amount, 0); // Withdraw from sender
    update_balance(to_acc_num, amount, 1);   // Deposit to recipient

    return 1;
}


//


// Main function
    int main() {
        int choice;
        char acc_num[20];
        int to_acc;
        int pin;
        float amount;
    struct Account acc;
    char password[20];
    

    while (1) {
        printf("\nXcash Banking Services: \n");
        printf("1. Create account\n");
        printf("2. Sign in\n");
        printf("3. Exit\n");
        printf("Choice: ");
        if (scanf("%d", &choice) != 1) { clearInputBuffer(); continue; }
        if (choice == 1) {
            create_account();
            continue;
        } else if (choice == 2) {
            char acc_name[20];
            struct Account user;
            printf("Enter your username: ");
            scanf(" %19s", acc_name);

    
            char password[20];
            printf("Enter your password: ");
            scanf(" %19s", password);  


            // Find account by username
            if (!find_account_by_username(acc_name, &user)) {
                printf("Account not found.\n");
                continue;
            }
            printf("Welcome, %s!\n", user.name);
            int action;
           



            // Verify password
           if(strcmp(user.password, password) != 0) {
                printf("Account not found.\n");
                continue;
            }

           //compare password and user name to login
           if(strcmp(user.name, acc_name) == 0 && strcmp(user.password,user.password) == 0) {
                printf("Login successful!\n");
              } else {
                printf("Invalid username or password.\n");
                continue;
            }
           


            // User menu
            do {

                printf("\n User Menu: \n");
                printf("1. Check Balance\n");
                printf("2. Deposit\n");
                printf("3. Withdraw\n");
                printf("4. Transfer\n");
                printf("5. Logout\n");
                printf("Choice: ");
                if (scanf("%d", &action) != 1) { clearInputBuffer(); action = 0; continue; }
                float amount;
                char to_acc[20];
                switch (action) {
                    case 1:
                        check_balance(user.account_number);
                        break;
                    case 2:
                        printf("Enter deposit amount: "); scanf("%f", &amount);
                        if (update_balance(user.account_number, amount, 1)) printf("Deposited ₦%.2f\n", amount);
                        else printf("Deposit failed.\n");
                        break;
                    case 3:
                        printf("Enter withdrawal amount: "); scanf("%f", &amount);
                        if (update_balance(user.account_number, amount, 0)) printf("Withdrew ₦%.2f\n", amount);
                        else printf("Insufficient funds.\n");
                        break;
                    case 4:
                        printf("Enter recipient account number: "); scanf(" %19s", to_acc);
                        printf("Enter amount to transfer: "); scanf("%f", &amount);
                        printf("Enter PIN to authorize transfer: ");
                        scanf("%d", &pin);
                        if (transfer(user.account_number, to_acc, amount)) {
                            printf("Transferred N%.2f to %s\n", amount, to_acc);
                        } else {
                            printf("Transfer failed. Check recipient or balance.\n");
                        }
                        break;
                    case 5:
                        printf("Logging out...\n");
                        break;
                    default:
                        printf("Invalid choice.\n");
                }
            } while (action != 5);
        } else if (choice == 3) {
            printf("Exiting...\n");
            break;
        } else {
            printf("Invalid choice.\n");
        }
    }
    
    
        }



















