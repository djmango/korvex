#include "main.h"
#include "JINX.h"

#define MSG "Hello JINX!"

//*************This space reserved for user-defined functions***************
    //Example of user defined JINX helper function.
    //Since it is at the top of this file, it can be called from anywhere else in this file.
    //Good practice is to put its prototype in JINX.h, though.
    void handleGet(JINX *inStr) {
        //Get the first token from the sent command
        getToken(inStr, 1);

        //Host outgoing messages
        char *message = (char*)malloc(sizeof(char) * (strlen(inStr->token) + 30));
        if (strcmp(inStr->token, "DEBUG_JINX") == 0) {
            writeJINXMessage("Asked for Debug");
            sprintf(message, "%s, %d", inStr->token, DEBUG_JINX);
        } else {
            sprintf(message, "%s %s", inStr->token, " was unable to be gotten.");
        }

        //Free malloc'd string
        writeJINXMessage(message);
        free(message);
        message = NULL;
    }

    //Returns positive integer parsed from character buffer
    int parseInt(const char *intString) {
        char digit;

        //Limit to 32 digit integer. Please don't send 32 digit integers
        char tempStr[33] = "";

        int len = strlen(intString);

        //Catch empty string
        if (len == 0) {
            char errorMessage[100];
            sprintf(errorMessage, "Error, unable to parse integer: %s", intString);
            writeJINXData("Error ", errorMessage);
        }

        for (int i = 0; i < len; i++) {
            digit = intString[i];
            if ((digit < '0') || (digit > '9')) {
                char errorMessage[100];
                sprintf(errorMessage, "Error, unable to parse integer: %s", intString);
                writeJINXData("Error", errorMessage);
                return -1;
            }

            tempStr[i] = digit;
        }

        return atoi(tempStr);
    }
//**************************************************************************

//Example parse. User can and should replace with own body.
void parseMessage(JINX *inStr) {
    //Echo entire recieved message
    writeJINXMessage(inStr->command);
    //Set inStr->token to first token (space-delimated word)
    getToken(inStr, 0);

    if (strcmp(inStr->token, "Option_1") == 0) {
        //Do option 1
        writeJINXMessage("Option 1 chosen.");
    } else if(strcmp(inStr->token, "get") == 0) {
        //Call another function to handle "get"
        handleGet(inStr);
    } else {
        //Do default
        writeJINXMessage("No comparison found");
    }
}
