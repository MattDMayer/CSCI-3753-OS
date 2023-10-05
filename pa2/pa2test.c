#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <sys/stat.h>		
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#define DEVICENAME "/dev/simple_character_device"
#define BUFFERSIZE 756

int main()
{
    char input;
    int length;
    char *buffer;
    int dScan;
    int sizeWrite;
    int offset;
    int whence;
    int file = open(DEVICENAME, O_RDWR  | O_APPEND);
    bool run = true;
    while(run == true)
    {
        printf("'r' Device Read\n");
        printf("'w' Device Write\n");
        printf("'s' Device Seek\n");
        printf("press 'ctrl + d' to abort at any time\n");
        dScan = scanf(" %c", &input);
        if(dScan == EOF)
        {
            close(file);
            return 0;
        }
        switch (input)
        {
            case 'r':
                buffer = (char *)malloc(BUFFERSIZE * sizeof(char));
				printf("Enter the number of bytes you want to read: ");
				dScan = scanf(" %d", &length);
                if(dScan == EOF)
                {
                    close(file);
                    return 0;
                }
				read(file, buffer, length);
				printf("%s\n", buffer); //buffer print
				//while(getchar() != '\n');
				free(buffer);
				break;
            
            case 'w':
				buffer = (char *)malloc(BUFFERSIZE * sizeof(char));
				printf("Enter the string you want to write: ");
				dScan = scanf(" %s", buffer);
                if(dScan == EOF)
                {
                    close(file);
                    return 0;
                }
                    
                printf("\n");
				sizeWrite = strlen(buffer);
				write(file, buffer, sizeWrite); //buffer write.
				free(buffer);
				break;

            case 's':
                offset = 0;
                whence = 0;
                printf("Enter an offset value: ");
                dScan = scanf(" %d", &offset);
                if(dScan == EOF)
                {
                    close(file);
                    return 0;
                }
                printf("Enter a value for whence (0 for SEEK_SET, 1 for SEEK_CUR, 2 for SEEK_END): ");
                dScan = scanf(" %d", &whence);
                if(dScan == EOF)
                {
                    close(file);
                    return 0;
                }
                switch (whence)
                {
                    case 0:
                        lseek(file, offset, whence);
                        break;

                    case 1:
                        lseek(file, offset, whence);
                        break;

                    case 2:
                        lseek(file, offset, whence);
                        break;

                    default:
                        break;
                } 
        }
    }
}