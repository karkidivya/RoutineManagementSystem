#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>

struct user
{
    char username[30], password[20], fileName[30];
};
struct user def = {"default","startyoufr","routine/default.txt"};   //default user
struct user current;												// current user
struct time
{
    int hh, mm;
};
struct time from, to;

char day[5];
int logoff;
COORD coord={0,0};


void gotoxy(int x,int y)
{
    coord.X=x; coord.Y=y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),coord);
}



void initial();
void initial2();
void initial3();
void initial4();
void title(); 
void backToMenu();
void deleting();
void logout();
void getUser ();                    			// check for entered username
void getPass();                     			// check the entered password
int availableUser(char username[]);				// to check if the username is available to register or not
void getFilename();								// to get the filename from username
void createUser();                  			// create new user
void encrypt(char* pass);           			// to encrypt password before writing to file
void decrypt(char* pass);           			// to decrypt password after reading from file
int defMenu();                                 // showing menu and options for admin
int studentMenu();                             // showing menu and options for student
void getDay(char * day);            			// get valid day input
void displayRoutine(FILE* ptr, char* day);      // display routine for given day from file
void createRoutine();                           // to create new personal routine
void updateRoutine();                           // to change the routine of a day
void deleteRoutine();                           // to delete the routine of a day
void createFileCopy();                          // to create a copy file for deleting and updating records
void getTime();                                 // to get time of the classes

void main()
{
    int option;
    initial(); 
    START:
    logoff = 0;
    title(); 
    printf("What would you like to do?");
    printf("\n\t[1] Log in \n\t[2] Sign up \n Enter:");
    while(1)
    {
        scanf("%d",&option);
        if (option == 1 || option ==2)
            break;
        printf("That isn't a valid option.\n");
    }
    if(option == 1)
        getUser();
    if (option == 2)
        createUser();
    system("color A");
    initial2();
    
    while(1)
    {
        title();
        Sleep(500);
        system("color A");
        printf("Welcome '%s' !\n\n", current.username);
        if (strcmp(current.username, def.username)==0)
            defMenu();
        else
            studentMenu();
        if (logoff == 1)
            goto START;
    }
    getch();

}


void getUser ()
{
    // userNo = variable to check for the user's password in other file
    // count = variable to prompt the user to register if username isn't registered
    int count=0, flag=2, userNo=0; 
    FILE* userDetails = fopen("userDetails.txt", "rb");
    char username[30],  checkUser[30];
    printf("Enter your username: ");
    while (1)
    {
        scanf(" %s",username);
        if (strcmp(def.username, username)==0)         // compares input to default user
        {
        	flag =0;
        	break;
		}
        while(userDetails != NULL)          		// compares input to usernames in the file 
        {
            fread(checkUser,30,1,userDetails);
            userNo++;
            if (strcmp(username, checkUser)==0)
            {
            	flag =0;
            	break;
			}
            if (feof(userDetails))
            {   
                flag =1;
                break;
            }
        }
        if (flag==0||count ==3)
            break;
     
        printf("Uhh that isn't a registered user. Try again.\n");
        count++;
    }

    fclose(userDetails);
    if(count == 3)          // prompts user to register
    {
        printf("It seems you haven't been registered. Register here!\n");
        createUser();
    }
    if (flag == 0)          // asks the user for password
    {
        strcpy(current.username, username);
        getPass(userNo);
    }
}


void getPass(int userNo)
{
    int i,count=0;
    char password[20], checkPass[20];
    FILE* pw = fopen("nothinghere.txt","rb");
    if (pw!= NULL)                  // gets corresponding password from file
    {
    	for (i = 0; i < userNo; i++)
	    {
	        fread(checkPass,20,1,pw);
	    }
        decrypt(checkPass);             // decrypts the gotten password
	}
    fclose(pw);
    printf("Enter your password: ");
    while (1)
    {
        scanf(" %s",password);
        if(pw!=NULL)
        {
        	if (strcmp(checkPass,password)==0)          // checks input with password from file
            	break;
		}
		if (strcmp(def.password, password)==0 || count ==3)     // checks input with default password
        {	
		    break;
		}
        printf("Uhh that password doesn't match the user. \nTry again: ");
        count++;
    }
    if(count==3)      // tells the user to register again if password is forgotten
    {
        printf("If you have forgotten your password, you will need to create a new user.");
        printf("\nBecause I haven't programmed that part out yet. (-w-)");
        getch();
        exit(0);
    }
    strcpy(current.password, password);
    system("cls");      // clears the screen
    getFilename();      // gets filename for user
}


void createUser()
{
    FILE* userDetails = fopen("userDetails.txt", "ab");
    FILE* pw = fopen("nothinghere.txt","ab");
    char confirm, username[30], password[20];
    while (1)
    {
        printf("Enter the username you would like: ");
        while(1)
        {
        	scanf(" %s",username);									// takes input for username
        	if (availableUser(username)==1)							// checks if username is available
        		break;
        	else
        		printf("Oops that username is already taken.\n\nTry another one: ");
		}
        printf("Enter the password you would like: ");
        scanf(" %s",password);
        printf("Are you sure? (y to confirm/ any other to discard)\n");
        scanf(" %c",&confirm);
        if (confirm == 'y' || confirm == 'Y')
            break;
    }
    strcpy(current.username, username);
    fwrite(username,30,1, userDetails);         // writing username to the file
    
    strcpy(current.password, password);
    encrypt(password);                          // encrypting the password
    fwrite(password,20,1,pw);                   // writing the password to file
    printf("Your user details have been stored. Remember your password! It can't be changed later.");
    getch();
    system("cls");                              // clears screen
    fclose(userDetails);
    fclose(pw);
	getFilename();                          	// gets filename for user                           	// creates file for new user
}



int availableUser(char username[])
{
	int available=1;									// username available = 1, taken = 0 
	FILE* userDetails = fopen("userDetails.txt","r");
	char checkUser[30];
	if (strcmp(username, def.username)==0)				// compares username (to register) and default username
	{
		available =0;
		goto Here;
	}
	if(userDetails != NULL)
	{
		while(1)										// compares username (to register) and usernames in the files
		{
			if (feof(userDetails))
				break;
			fread(checkUser,30,1,userDetails);
			if(strcmp(checkUser, username)==0)
			{
				available =0;
				break;
			}
		}
	}
	Here:
	fclose(userDetails);
	return(available);
}




void encrypt(char* pass)
{
	int i=0;
	while (pass[i] != '\0')
	{
		pass[i] = pass[i] + i + 1;
		i++;
	}
}



void decrypt(char* pass)
{
	int i=0;
	while (pass[i]!='\0')
	{
		pass[i] = pass[i] - i-1;
		i++;
	}
}


int defMenu()
{
    int option;
    printf("What would you like to do?\n");
    printf("\t[1] View routine \n\t[2] Add new routine \n\t[3] Update routine \n\t[4] Delete routine \n\t[5] Log out \n\t[6] Exit");
    printf("\nEnter: ");
    while(1)
    {
        scanf("%d",&option);
        if (option >0 && option <=6)
            break;
        printf("Uhh that isn't a valid option.");
    }
    system("cls");
    printf("\t\t\t:::::::::::::::::ROUTINE MANAGEMENT SYSTEM:::::::::::::::::\n\n\n\t\t\t  :::::::::::::::::Welcome To Our System::::::::::::::::::\n\n\n");
    switch (option)
    {
    case 1:
    {
        if (fopen(current.fileName, "r")!=NULL)
        {
            printf("Which routine would you like to view?\n");
            printf("\t[1] Class routine \n\t[2] Personal routine \nEnter:");
            while (1)
            {
                scanf("%d", &option);
                if (option == 1 || option == 2)
                    break;
                printf("Uhh that isn't a valid option.\n");
            }
        }
        printf("Which day would you like to view the routine of?\n");
        printf("Enter the first three letters of the day: ");
        getDay(day);
        if (option == 2)
            displayRoutine(fopen(current.fileName,"r"), day);
        else 
            displayRoutine(fopen(def.fileName,"r"),day);
        break;
    }
    
    case 2:
    {
        printf("Note that you can only create one extra routine other than your class routine.\n");
        Sleep(250);
        createRoutine();
        break;
    }
    case 3:
    {
        updateRoutine();
        break;
    }
    case 4:
    {
        deleteRoutine();
        break;
    }
    case 5:
    {
        logout();
        logoff = 1;
        return(0);
    }
    case 6:
    {
        initial4();
        exit(0);
    }
    }
    backToMenu();
}

int studentMenu()
{
    int option;
    printf("What would you like to do?\n");
    printf("\t[1] View routine \n\t[2] Add a personal routine \n\t[3] Log out \n\t[4] Exit");
    printf("\nEnter: ");
    while(1)
    {
        scanf("%d",&option);
        if (option >0 && option <=4)
            break;
        printf("Uhh that isn't a valid option.");
    }
    system("cls");
    printf("\t\t\t:::::::::::::::::ROUTINE MANAGEMENT SYSTEM:::::::::::::::::\n\n\n\t\t\t  :::::::::::::::::Welcome To Our System::::::::::::::::::\n\n\n");
    switch (option)
    {
    case 1:
    {
        if (fopen(current.fileName, "r")!=NULL)
        {
            printf("Which routine would you like to view?\n");
            printf("\t[1] Class routine \n\t[2] Personal routine \nEnter:");
            while (1)
            {
                scanf("%d", &option);
                if (option == 1 || option == 2)
                    break;
                printf("Uhh that isn't a valid option.\n");
            }
        }
        printf("Which day would you like to view the routine of?\n");
        printf("Enter the first three letters of the day: ");
        getDay(day);
        if (option == 2)
            displayRoutine(fopen(current.fileName,"r"), day);
        else 
            displayRoutine(fopen(def.fileName,"r"),day);
        break;
    }
    
    case 2:
    {
        printf("Note that you can only create one extra routine other than your class routine.\n");
        Sleep(250);
        createRoutine();
        break;
    }
    case 3:
    {
        logout();
        logoff = 1;
        return(0);
    }
    case 4:
    {
        initial4();
        exit(0);
    }
    }
    backToMenu();
}

void getDay(char * day)
{
    int i,flag=0; 
    char days[7][4] = {"sun","mon","tue","wed","thu","fri","sat"};
    while (1)
    {
        scanf(" %s",day);
        for (i = 0; i < 3; i++)             // changing the input to lowercase for comparison
        {
            day[i] = tolower(day[i]);
        }
        for (i=0;i<7;i++)              // checking the input by comparing with valid days
        {
            flag =0;
            if (strcmp(day,days[i])!=0)
            {
                flag=1;
            }
            if (flag == 0)
                break;
        }
        if (flag == 0)
            break;
        printf("Please enter a valid input.\n");
    }
    day[3]='\n';                // since the string in the file ends with \n
    day[4]='\0';                
}


void displayRoutine(FILE* ptr, char* day)
{
    initial3();
    Sleep(200); 
    int flag =0;
    char temp[50];
    while (1)           //skipping to the part in the file with the required day
    {
        fgets(temp, 50, ptr);
        if (strcmp(day, temp)==0)
            break;
        if (feof(ptr))
        {
            printf("That day doesn't have a routine.");
            goto HERE;
        }
    }
    while (1)           // printing the routine from the file until \n is encountered
    {
        fgets(temp, 50, ptr);
        puts(temp);
        if (strcmp(temp, "\n")==0 || feof(ptr))                // to stop printing on getting a \n string line
            break;
        flag=1;
    }
    fclose(ptr);
    HERE:
    printf("\n\nPress any key to continue!\n");
    getch();
}


void getFilename()
{
    char filename[40] = "routine/";
    int i=0;
    while (current.username[i]!='\0')              		// gets filename from username
    {
        current.fileName[i]= current.username[i] -1;
        i++;
    }
    current.fileName[i] = '\0';
    strcat(current.fileName, ".txt");
    strcat(filename, current.fileName);
    strcpy(current.fileName, filename);					// current filename as routine/____.txt
}



void createFileCopy()
{
    char ch;
    FILE* routine;
    FILE* defRoutine = fopen("routine/default.txt","r");
    if (defRoutine != NULL)
    {
        routine = fopen("routine/default copy.txt","w");			// creates a file corresponding to user
        while(ch!=EOF)                  				// copies the contents of the default file
        {
            ch = getc(defRoutine);
            fprintf(routine, "%c",ch);
        }
        fclose(routine);
    }
    else
    {
        printf("A file couldn't be created. Make sure the default file is present.");
    }
    fclose(defRoutine);
}

void createRoutine()
{
    FILE * perRoutine;
    char day[4], activity[30], ans;
    getFilename();
    perRoutine = fopen(current.fileName, "w");
    printf("To create a routine, enter\n");
    while (1)
    {
        printf("Day: (eg. sun, mon)\n");
        getDay(day);
        fprintf(perRoutine, "%s",day);
        while(1)
        {
            printf("\nTime: (HH:MM to HH:MM)\n");
            getTime();
            printf("\nAction: ");
            scanf(" %s",activity);
            fprintf(perRoutine,"%-30s%d:%d - %d:%d\n",activity, from.hh, from.mm, to.hh, to.mm);
            printf("Add other entries? (y/ any other to stop)");
            scanf(" %c", &ans);
            system("cls");
            printf("\t\t\t:::::::::::::::::ROUTINE MANAGEMENT SYSTEM:::::::::::::::::\n\n\n\t\t\t  :::::::::::::::::Welcome To Our System::::::::::::::::::\n\n\n");
            if (ans != 'y' && ans != 'Y')
                break;
        }
        fprintf(perRoutine,"\n\n\n");
        printf("\nAdd another day? (y/n)");
        scanf(" %c",&ans);
        system("cls");
        printf("\t\t\t:::::::::::::::::ROUTINE MANAGEMENT SYSTEM:::::::::::::::::\n\n\n\t\t\t  :::::::::::::::::Welcome To Our System::::::::::::::::::\n\n\n");
        if (ans == 'N' || ans == 'n')
            break;
    }
    fclose(perRoutine);
}


void getTime()
{
    printf("From:\n");
    while(1)
    {
        printf("\tHH: ");
        scanf("%d", &from.hh);
        if(from.hh >= 0 && from.hh <24)
            break;
        printf("That isn't a valid hour.");
    }
    while(1)
    {
        printf("\tMM: ");
        scanf("%d", &from.mm);
        if(from.mm >= 0 && from.mm <60)
            break;
        printf("That isn't a valid minute.");
    }
    printf("To:\n");
    while(1)
    {
        printf("\tHH: ");
        scanf("%d", &to.hh);
        if(to.hh >= 0 && to.hh <24)
            break;
        printf("That isn't a valid hour.");
    }
    while(1)
    {
        printf("\tMM: ");
        scanf("%d", &to.mm);
        if(to.mm >= 0 && to.mm <60)
            break;
        printf("That isn't a valid minute.");
    }
}

void updateRoutine()
{
    createFileCopy();
    char temp[50], class[30], ans;
    FILE * routine;
    FILE * routineCopy = fopen("routine/default copy.txt", "r");
    printf("Which day would you like to update?\n");
    getDay(day);
    printf("The previous routine is:\n\n");
    displayRoutine(fopen(def.fileName, "r"), day);
    printf("\nEnter the new routine:\n");
    routine = fopen(def.fileName, "w");
    while (1)
    {
        fgets(temp, 50, routineCopy);
        fputs(temp, routine);
        if (strcmp(temp, day)==0)
            break;
    }
    while(1)
    {
        printf("\nClass: ");
        scanf(" %s",class);
        getTime();
        fprintf(routine, "%-30s%d:%d - %d:%d\n",class, from.hh, from.mm, to.hh, to.mm);
        fgets(temp, 50, routineCopy);
        printf("Add other classes? (y/n)");
        scanf(" %c", &ans);
        if (ans == 'n' || ans == 'N')
            break;
        system("cls");
    }
    fputs("\n",routine);
    while (1)
    {
        fputs("\n", routine);
        fgets(temp, 50, routineCopy);
        if (strcmp(temp, "\n")==0)
            break;
    }
    while(1)
    {
        fgets(temp, 50, routineCopy);
        fputs(temp, routine);
        if (feof(routineCopy))
            break;
    }       
    printf("Press any key to continue!");
    getch();
    fclose(routineCopy); 
    fclose(routine);

}
void deleteRoutine()
{
    createFileCopy();
    char temp[50];
    FILE * routine;
    FILE * routineCopy = fopen("routine/default copy.txt", "r");
    printf("Which day would you like to delete the routine of?");
    getDay(day);
    displayRoutine(fopen(def.fileName,"r"),day);
    routine = fopen(def.fileName, "w");
    while (1)
    {
        fgets(temp, 50, routineCopy);
        fputs(temp, routine);
        if (strcmp(temp, day)==0)
            break;
    }
    while (1)
    {
        fgets(temp, 50, routineCopy);
        fputs("\n", routine);
        if (strcmp(temp, "\n")==0)
            break;
    }
    while (1)
    {
        fgets(temp, 50, routineCopy);
        fputs(temp, routine);
        if (feof(routineCopy))
            break;
    }
    fclose(routineCopy);
    fclose(routine);
    deleting();
}

/* ------------------------DISPLAY FUNCTIONS----------------------------*/
void title()
{
    int i,j,k,l;
    char title[]=("\t\t\t:::::::::::::::::ROUTINE MANAGEMENT SYSTEM:::::::::::::::::\n\n\n\t\t\t  :::::::::::::::::Welcome To Our System::::::::::::::::::\n\n\n\t\t\t");

    for (i=0;i<strlen(title);i++)
    {
        putchar(title[i]);
        Sleep(10);
    }
    printf("\n\n");

}
void initial()
{
   int i;
   system("color A");
   Sleep(100);
   printf("\n\n\n\n\n\n\n\n\n\n\n\n\t\t\t\t\tLoading:");
   for (i = 0; i <= 7; i++)
   {
      Sleep(200);
      printf(".");

      if (i == 5)
      {
         Sleep(150);
         system("color 4");
         system("color 6");
         system("color 2");
         system("color 04");

         system("color 4F");
         system("color 6");
         system("color 2E");
         system("color A");

      }
   }
   system("cls");
}
void initial2()
{
   int i;
   system("color C");
   Sleep(500);
   printf("\n\n\n\n\n\n\n\n\n\n\n\n\t\t\t\t\tLOGGING YOU INTO THE SYSTEM WAIT A FEW MOMENTS:");
   for (i = 0; i <= 13; i++)
   {
      Sleep(200);
      printf(".");

      if (i == 5)
      {
         Sleep(150);
         system("color A");
         system("color 6");
         system("color 2");
         system("color 0A");

         system("color 4F");
         system("color 6");
         system("color 2E");
         system("color C");

      }
   }
   printf("\n\t\t\t\tFETCHING DATA");
    for (i = 0; i <= 5; i++)
   {
      Sleep(200);
      printf(".");
   }
    printf("\n\t\t\t\tLOGGED IN SUCCESSFULLY");
    for (i = 0; i <= 4; i++)
   {
      Sleep(200);
      printf(".");
   }

   system("cls");
   system("color A");
}
void initial3()
{
    system("cls");
   int i;
   system("color A");
   Sleep(500);
   printf("\n\n\n\n\n\n\n\n\n\n\n\n\t\t\t\tHopping into the server:");
   for (i = 0; i <= 5; i++)
   {
      Sleep(200);
      printf(".");

      if (i == 5)
      {
         Sleep(150);
         system("color A");
         system("color 6");
         system("color 2");
         system("color 0A");

         system("color 4F");
         system("color 6");
         system("color 2E");
         system("color A");

      }
   }
   printf("\n\t\t\t\t\tSEARCHING");
    for (i = 0; i <= 5; i++)
   {
      Sleep(200);
      printf(".");
   }
    printf("\n\t\t\t\t\tRETRIEVING DATA");
    for (i = 0; i <= 3; i++)
   {
      Sleep(200);
      printf(".");
   }

   system("cls");
   printf("\n\n\t\tROUTINE FOR THE DAY\n\n\n");
}

void initial4()
{
    system("cls");
    Sleep(500);
    int i,j,k,l;
    char title[]=("\n\n\t\tThanks for using our system :)\t");

    for (i=0;i<strlen(title);i++)
    {
        putchar(title[i]);
        Sleep(25);
    }
    Sleep(500);
    printf("\n\n");

}

void backToMenu()
{
    int i;
    char message[] = "GOING BACK TO THE MENU......PLEASE WAIT.......";
    system("cls");
    Sleep(200);
    printf("\n\n\n\n\n\n\n\n\n\n\n\n\t\t\t");
    for (i=0;i< strlen(message);i++)
    {
        putchar(message[i]);
        Sleep(25);
        if (i==7)
        {
            system("color A");
            system("color 6");
            system("color 2");
            system("color 0A");
            system("color 4F");
            system("color 6");
            system("color 2E");
            system("color A");
        }
    }
    Sleep(250);
    system("cls");
}

void deleting()
{
    int i;
    char message[] = "DELETING ROUTINE......PLEASE WAIT.......\n\n\t\t\tROUTINE SUCCESSFULLY DELETED";
    system("cls");
    Sleep(200);
    printf("\n\n\n\n\n\n\n\n\n\n\n\n\t\t\t");
    for (i=0;i< strlen(message);i++)
    {
        putchar(message[i]);
        Sleep(25);
        if (i==7 || i==27)
        {
            system("color A");
            system("color 6");
            system("color 2");
            system("color 0A");
            system("color 4F");
            system("color 6");
            system("color 2E");
            system("color A");
        }
    }
    Sleep(500);
    system("cls");
}
void logout()
{
   int i;
   system("cls");
   system("color A");
   Sleep(100);
   printf("\n\n\n\n\n\n\n\n\n\n\n\n\t\t\t\t\tLogging out");
   for (i = 0; i <= 7; i++)
   {
      Sleep(200);
      printf(".");

      if (i == 5)
      {
         Sleep(150);
         system("color 4");
         system("color 6");
         system("color 2");
         system("color 04");

         system("color 4F");
         system("color 6");
         system("color 2E");
         system("color A");

      }
   }
   system("cls");
}