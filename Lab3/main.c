// ACADEMIC INTEGRITY PLEDGE
//
// - I have not used source code obtained from another student nor
//   any other unauthorized source, either modified or unmodified.
//
// - All source code and documentation used in my program is either
//   my original work or was derived by me from the source code
//   published in the textbook for this course or presented in
//   class.
//
// - I have not discussed coding details about this project with
//   anyone other than my instructor. I understand that I may discuss
//   the concepts of this program with other students and that another
//   student may help me debug my program so long as neither of us
//   writes anything during the discussion or modifies any computer
//   file during the discussion.
//
// - I have violated neither the spirit nor letter of these restrictions.
//
//
//
// Signed: Jaret Varn Date: 11/25/20

//filesys.c
//Based on a program by Michael Black, 2007
//Revised 11.3.2020 O'Neil

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

void listFiles(FILE* floppy)
{

    //load the directory from sector 257
    char dir[512];
    int i, j, usedSectors = 0, usedBytes = 0;
    fseek(floppy,512*257,SEEK_SET);
    for (i=0; i<512; i++)
        dir[i]=fgetc(floppy);

    // print files list
    printf("Files list:\n");
    for (i=0; i<512; i=i+16)
    {
        if (dir[i]==0)
        {
            continue;
        }
        char fileName[8] = "";
        for (j=0; j<8; j++)
        {
            if (dir[i + j] != 0)
            {
                fileName[j] = dir[i + j];
                //printf("%c",dir[i+j]);
            }
        }
        char ext;
        if (toupper(dir[i + 8]) == 'T')
        {
            ext = 't';
        }
        else
        {
            if (toupper(dir[i + 8]) == 'X')
            {
                ext = 'x';
            }
        }
        usedSectors = usedSectors + dir[i + 10];
        usedBytes = usedBytes + dir[i + 10] * 512;
        printf("%8s.%c %d bytes used\n", fileName, ext, dir[i + 10] * 512);
    }
    printf("\n");
    printf("Total space used: %d sectors, %d bytes\n", usedSectors, usedBytes);
    printf("Free space: %d sectors, %d bytes\n", 512 - usedSectors, 512*512 - usedBytes);

}

void readFile(FILE* floppy, char searchFile[8])
{
    //load the directory from sector 257
    char dir[512];
    int i, j;
    int sectorStart, sectorSize;
    bool found = false, readable = false;
    fseek(floppy,512*257,SEEK_SET);
    for (i=0; i<512; i++)
        dir[i]=fgetc(floppy);

    for (i=0; i<512; i=i+16)
    {
        if (dir[i]==0)
        {
            continue;
        }
        char fileName[8] = "";
        for (j=0; j<8; j++)
        {
            if (dir[i + j] != 0)
            {
                fileName[j] = dir[i + j];
            }
        }
        char ext;
        if (toupper(dir[i + 8]) == 'T')
        {
            ext = 't';
        }
        else
        {
            if (toupper(dir[i + 8]) == 'X')
            {
                ext = 'x';
            }
        }
        if (strcmp(fileName, searchFile) == 0)
        {
            found = true;
            sectorStart = dir[i + 9];
            sectorSize = dir[i + 10];
            if (ext == 'x')
            {
                printf("File %s.%c is not readable.\n", fileName, ext);
                break;
            }
            readable = true;
            break;
        }
    }
    if (found)
    {
        if (readable)
        {
            char fileContents[12288];
            //printf("File start: %d File size: %d\n", sectorStart, sectorSize);
            fseek(floppy,512 * sectorStart,SEEK_SET);
            for(i=0; i<12288; i++)
            {
                fileContents[i] = fgetc(floppy);
                if (fileContents[i] == '\0')
                {
                    break;
                }
                printf("%c", fileContents[i]);
            }
        }
    }
    else
    {
        printf("File not found\n");
    }
}

void writeFile(FILE* floppy, char searchFile[8])
{

    //load the directory from sector 257
    char dir[512];
    int i, j, freeDir = -1, freeMap = -1;
    bool found = false, isText = false;
    fseek(floppy,512*257,SEEK_SET);
    for (i=0; i<512; i++)
        dir[i]=fgetc(floppy);

    //load the map
    char map[512];
    fseek(floppy,512*256,SEEK_SET);
    for(i=0; i<512; i++)
        map[i]=fgetc(floppy);

    for (i=0; i<512; i=i+16)
    {
        if (dir[i] == 0 && freeDir == -1)
        {
            // Find first free sector
            freeDir = i;
            continue;
        }
        char fileName[8] = "";
        for (j=0; j<8; j++)
        {
            if (dir[i + j] != 0)
            {
                fileName[j] = dir[i + j];
            }
        }
        char ext;
        if (toupper(dir[i + 8]) == 'T')
        {
            ext = 't';
        }
        else
        {
            if (toupper(dir[i + 8]) == 'X')
            {
                ext = 'x';
            }
        }
        if (strcmp(fileName, searchFile) == 0)
        {
            found = true;
            if (ext == 't')
            {
                isText = true;
                break;
            }
            break;
        }
    }
    if (found)
    {
        if (isText)
        {
            // Duplicate file
            printf("File %s.t already exists\n", searchFile);
        }
    }
    else
    {
        // File does not exist yet

        for (i = 0; i < 16; i++)
        {
            if (freeMap != -1)
            {
                break;
            }
            for (j = 0; j < 16; j++)
            {
                if (freeMap != -1)
                {
                    break;
                }
                if (map[16 * i * j] == 0)
                {
                    // Found a free map spot
                    map[16 * i * j] = 0xff;
                    freeMap = 16 * i * j;
                }
            }
        }
        if (freeMap == -1)
        {
            printf("Insufficient disk space.\n");
        }
        else
        {
            for (i = 0; i < strlen(searchFile); i++)
            {
                dir[freeDir + i] = searchFile[i];
            }
            for (i = strlen(searchFile); i < 8; i++)
            {
                dir[freeDir + i] = 0;
            }
            dir[freeDir + 8] = 't'; // Text file
            dir[freeDir + 9] = freeMap; // Offset
            dir[freeDir + 10] = 1; // One sector sized file
        }
        printf("Enter text for file %s\n", searchFile);
        char text[512];
        fgets(text, sizeof(text), stdin);

        // Write the contents of text to the floppy
        fseek(floppy, 512*freeMap, SEEK_SET);
        for (i = 0; i < 512; i++)
            fputc(text[i], floppy);

        //write the map and directory to the floppy
        fseek(floppy,512*256,SEEK_SET);
        for (i=0; i<512; i++) fputc(map[i],floppy);

        fseek(floppy,512*257,SEEK_SET);
        for (i=0; i<512; i++) fputc(dir[i],floppy);
    }
}


void deleteFile(FILE* floppy, char searchFile[8])
{

    //load the directory from sector 257
    char dir[512];
    int i, j;
    bool found = false;
    int sectorStart, sectorSize;
    fseek(floppy,512*257,SEEK_SET);
    for (i=0; i<512; i++)
        dir[i]=fgetc(floppy);

    //load the map
    char map[512];
    fseek(floppy,512*256,SEEK_SET);
    for(i=0; i<512; i++)
        map[i]=fgetc(floppy);

    for (i=0; i<512; i=i+16)
    {
        if (dir[i] == 0)
        {
            continue;
        }
        char fileName[8] = "";
        for (j=0; j<8; j++)
        {
            if (dir[i + j] != 0)
            {
                fileName[j] = dir[i + j];
            }
        }
        if (strcmp(fileName, searchFile) == 0)
        {
            found = true;
            dir[i] = 0;
            sectorStart = dir[i + 9];
            sectorSize = dir[i + 10];
            break;
        }
    }
    if (found)
    {
        // File found to delete
        for (i = 0; i < sectorSize; i++)
        {
            map[sectorStart] = 0;
            sectorStart++;
        }
        printf("File %s deleted.\n", searchFile);
    }
    else
    {
        // File does not exist
        printf("File %s does not exist.\n", searchFile);

    }
    //write the map and directory back to the floppy image
    fseek(floppy,512*256,SEEK_SET);
    for (i=0; i<512; i++) fputc(map[i],floppy);

    fseek(floppy,512*257,SEEK_SET);
    for (i=0; i<512; i++) fputc(dir[i],floppy);
}

int main(int argc, char* argv[])
{
    char name[8] = {-1};
    if (argc < 3)
    {
        printf("No filename given.\n");
    }
    else
    {
        strcpy(name, argv[2]);
        if (argc != 3)
        {
            printf("Invalid arguments");
            return 0;
        }
    }

    //open the floppy image
    FILE* floppy;
    floppy=fopen("floppya.img","r+");
    if (floppy==0)
    {
        printf("floppya.img not found\n");
        return 0;
    }

    switch (toupper(*argv[1]))
    {
        case 'D':
        {
            if (name[0] != -1)
            {
                printf("Given command D\n");
                deleteFile(floppy, name);
            }
            else
            {
                printf("No filename given. Cannot process command.");
                return 0;
            }

            break;
        }

        case 'L':
        {
            printf("Given command L\n");
            listFiles(floppy);
            break;
        }

        case 'M':
        {
            if (name[0] != -1)
            {
                printf("Given command M\n");
                writeFile(floppy, name);
            }
            else
            {
                printf("No filename given. Cannot process command.");
                return 0;
            }
            break;
        }

        case 'P':
        {
            if (name[0] != -1)
            {
                printf("Given command P\n");
                readFile(floppy, name);
            }
            else
            {
                printf("No filename given. Cannot process command.");
                return 0;
            }
            break;
        }

        default:
        {
            printf("Invalid command\n");
        }

    }

    fclose(floppy);
}