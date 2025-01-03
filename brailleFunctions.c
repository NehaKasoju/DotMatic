#include "PC_FileIO.c"

//--------------------------------
//configure sensors
SensorType [S1] = sensorEV3 Touch ;
int statusEStop = SensorValue[S1];

//--------------------------------
void systemStart()
{
   displayTextLine(3, "Press Enter to Start Printing");

   wait1Msec(1000);

   while(!getButtonPress(buttonEnter))
        {}

   while(getButtonPress(buttonAny))
    {}

}

//--------------------------------
char word[11];  // global array storing word (10 characters + null terminator)

int readWordFromFile(string fileName) {
    TFileHandle fin;
    bool fileHandle = openReadPC(fin, fileName);

    if (!fileHandle) {
        displayTextLine(4, "Error opening file");
        return -1;  
    }

    int charCount = 0;
    char c = 0;
    int whiteSpace = 0;

    while (readCharPC(fin, c) && charCount < 10 && whiteSpace == 0) {
        if (c == ' ' || c == '\n' || c == '\t') {
            whiteSpace = 1;
        } else {
            word[charCount] = c;
            charCount++;
        }
    }

    word[charCount] = '\0';  // null-terminate the string
    closeFilePC(fin);

    return charCount;  // return the number of characters in word (excluding null terminator)
}

//--------------------------------

void eStop (int statusEStop){

    if (statusEStop > 0){
        displayTextLine(4, "Emergency Stop Activated!");

        motor[motorC] = 0;
        motor[motorD] = 0;

        const int TIME_UNTIL_ROLL = 15;

        displayTextLine(5, "%d secs until paper roll out.", TIME_UNTIL_ROLL);
        wait1Msec (15000) ;

        motor[motorA] = 10;
        while (abs(nMotorEncoder[motorA]) <= 180){}
        motor[motorA] = 0;
    }
}

//--------------------------------
void systemStop (){

    while (!getButtonPress(buttonAny)){}

    while (getButtonPress(buttonDown)){}

    displayTextLine(4, "System Stopping.");

    motor[motorC] = 0;
    motor[motorD] = 0;
    motor[motorA] = 0;

    wait1Msec (15000) ;

}

//--------------------------------
int alpha[26][3][2] = {
    {
        {0,1},
        {0,0},
        {0,0}
    }, //A

    {
        {0,1},
        {0,1},
        {0,0}
    }, //B

    {
        {1,1},
        {0,0},
        {0,0}
    }, //C

    {
        {1,1},
        {1,0},
        {0,0}
    }, //D

    {
        {0,1},
        {1,0},
        {0,0}
    }, //E

    {
        {1,1},
        {0,1},
        {0,0}
    }, //F

    {
        {1,1},
        {1,1},
        {0,0}
    }, //G

    {
        {0,1},
        {1,1},
        {0,0}
    }, //H

    {
        {1,0},
        {0,1},
        {0,0}
    }, //I

    {
        {1,0},
        {1,1},
        {0,0}
    }, //J

    {
        {0,1},
        {0,0},
        {0,1}
    }, //K

    {
        {0,1},
        {0,1},
        {0,1}
    }, //L

    {
        {1,1},
        {0,0},
        {0,1}
    }, //M

    {
        {1,1},
        {1,0},
        {0,1}
    }, //N

    {
        {0,1},
        {1,0},
        {0,1}
    }, //O

    {
        {1,1},
        {0,1},
        {0,1}
    }, //P

    {
        {1,1},
        {1,1},
        {0,1}
    }, //Q

    {
        {0,1},
        {1,1},
        {0,1}
    }, //R

    {
        {1,0},
        {0,1},
        {0,1}
    }, //S

    {
        {1,0},
        {1,1},
        {0,1}
    }, //T

    {
        {0,1},
        {0,0},
        {1,1}
    }, //U

    {
        {0,1},
        {0,1},
        {1,1}
    }, //V

    {
        {1,0},
        {1,1},
        {1,0}
    }, //W

    {
        {1,1},
        {0,0},
        {1,1}
    }, //X

    {
        {1,1},
        {1,0},
        {1,1}
    }, //Y

    {
        {0,1},
        {1,0},
        {1,1}
    }, //Z
};

char wrd[10] = {'h','e','l','l','o','w','o','r','l','d'};

void printWrd(int wrdLen) {
    int row[wrdLen*2];
    int indices[wrdLen];

    for (int i = 0; i < wrdLen; i++) {
        //determining indices to be used for alpha (using ascii codes)
        //i.e. what letter is in wrd[i]
        indices[i] = wrd[i] - 'a';
    }

    for(int i = 0; i < 3; i++) {
        int k = 0;
        for (int j = 0; j < wrdLen; j++) {
            row[k]=alpha[indices[j]][i][0];
            row[k+1]=alpha[indices[j]][i][1];
            k+=2;
        }
        printRow(&row[0], wrdLen*2);
    }

}

void printRow(int *ptr, int len) {
	for (int i = 0; i<len; i++) {
		if (*ptr) {
			moveCrank(360);
		}
		moveCart(8.25);
		ptr++;
	}
}

void movePaper(int deg){
	nMotorEncoder[motorA] = 0;
	motor[motorA] = -10;
	while (nMotorEncoder[motorA] <= deg) {}
	motor[motorA] = 0;
}

void moveCart(int deg){
	nMotorEncoder[motorD] = 0;
	motor[motorD] = -10;
	while (nMotorEncoder[motorD] <= deg) {}
	motor[motorD] = 0;
}

void moveCrank(int deg){
	nMotorEncoder[motorC] = 0;
	motor[motorC] = -10;
	while (nMotorEncoder[motorC] <= deg) {}
	motor[motorC] = 0;
}


task main() {
    systemStart();

    string fileName = "input.txt";

    int wordLength = readWordFromFile(fileName);
    if (wordLength < 0) {
        displayTextLine(5, "Exiting due to file read error.");
        //return;
    }

    // Display the word read for debugging
    displayTextLine(5, "Word: %s", word);

    while (true) {

        // Check for E-Stop status before printing
        if (SensorValue[S1]) {
            eStop(SensorValue[S1]);
        }

        // Allocate memory for row data
        int rowData[wordLength * 2];
        int indices[wordLength];

        // Determine letter indices
        for (int i = 0; i < wordLength; i++) {
            indices[i] = word[i] - 'a';
        }

        // Loop through each of the three Braille rows
        for (int row = 0; row < 3; row++) {
            // Populate row data for the current row
            for (int i = 0; i < wordLength; i++) {
                rowData[i * 2] = alpha[indices[i]][row][0];
                rowData[i * 2 + 1] = alpha[indices[i]][row][1];
            }

            // Print the current row
            printRows(wordLength, rowData);

            movePaper(50);

            // Check for E-Stop between rows
            if (SensorValue[S1]) {
                eStop(SensorValue[S1]);
            }
        }

        systemStop();
    }
}

