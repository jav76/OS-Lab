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
        printf("%8s.%c %d bytes used\n", fileName, ext, usedBytes);
    }
    printf("\n");
    printf("Total space used: %d sectors, %d bytes\n", usedSectors, usedBytes);
    printf("Free space: %d sectors, %d bytes\n", 512 - usedSectors, 512*512 - usedBytes);

}

void readFile(FILE* floppy, char searchFile[])
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

    int i, j, size, noSecs, startPos;


    /*

    //load the disk map from sector 256
    char map[512];
    fseek(floppy,512*256,SEEK_SET);
    for(i=0; i<512; i++)
        map[i]=fgetc(floppy);

    //load the directory from sector 257
    char dir[512];
    fseek(floppy,512*257,SEEK_SET);
    for (i=0; i<512; i++)
        dir[i]=fgetc(floppy);

    //print disk map
    printf("Disk usage map:\n");
    printf("      0 1 2 3 4 5 6 7 8 9 A B C D E F\n");
    printf("     --------------------------------\n");
    for (i=0; i<16; i++) {
        switch(i) {
            case 15: printf("0xF_ "); break;
            case 14: printf("0xE_ "); break;
            case 13: printf("0xD_ "); break;
            case 12: printf("0xC_ "); break;
            case 11: printf("0xB_ "); break;
            case 10: printf("0xA_ "); break;
            default: printf("0x%d_ ", i); break;
        }
        for (j=0; j<16; j++) {
            if (map[16*i+j]==-1) printf(" X"); else printf(" .");
        }
        printf("\n");
    }

    // print directory
    printf("\nDisk directory:\n");
    printf("Name    Type Start Length\n");
    for (i=0; i<512; i=i+16) {
        if (dir[i]==0) break;
        for (j=0; j<8; j++) {
            if (dir[i+j]==0) printf(" "); else printf("%c",dir[i+j]);
        }
        if ((dir[i+8]=='t') || (dir[i+8]=='T')) printf("text"); else printf("exec");
        printf(" %5d %6d bytes\n", dir[i+9], 512*dir[i+10]);
    }

*/
/*
	//write the map and directory back to the floppy image
    fseek(floppy,512*256,SEEK_SET);
    for (i=0; i<512; i++) fputc(map[i],floppy);

    fseek(floppy,512*257,SEEK_SET);
    for (i=0; i<512; i++) fputc(dir[i],floppy);
*/
    fclose(floppy);
}