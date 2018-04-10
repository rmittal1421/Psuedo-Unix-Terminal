#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <zconf.h>
#include <stdlib.h>

#define PERMISSION_SIZE sizeof("drwxrwxrwx")

/*
 * Global flags.
 */
bool flag_i;
bool flag_R;
bool flag_l;

/*
 * To complete the -l command, we need to print out the permissions that are allowed on any
 * file or directory. This code does the same.
 * Code taken from stakeOverflow.
 */
char *permission(mode_t permission) {
    static char toReturn[PERMISSION_SIZE];
    snprintf(toReturn, PERMISSION_SIZE, "%c%c%c%c%c%c%c%c%c%c",
             (S_ISDIR(permission)) ? 'd' : '-',
             (permission & S_IRUSR) ? 'r' : '-', (permission & S_IWUSR) ? 'w' : '-',
             (permission & S_IXUSR) ? 'x' : '-',
             (permission & S_IRGRP) ? 'r' : '-', (permission & S_IWGRP) ? 'w' : '-',
             (permission & S_IXGRP) ? 'x' : '-',
             (permission & S_IROTH) ? 'r' : '-', (permission & S_IWOTH) ? 'w' : '-',
             (permission & S_IXOTH) ? 'x' : '-');

    return toReturn;
}

/*
 * PrintInformation is printing the information about every file/directory read in a directory
 * by a pointer of type dirent and prints out the information according to the flags or we can say,
 * it prints out the asked properties.
 * It takes care of the flag_i which means that has i been asked or not.
 * Similarly, it takes care of l.
 * If they were called together, they both are coded in such a way that it prints out results of both the
 * commands together.
 *
 * To take care of the recursive R function, it just uses this function to print out bunch of information
 * but for every directory.
 */
void printInformation(char *directoryName, char *fileName) {
    char dir_name[2 * 1024];
    struct stat buffer;

    //Code for group and owner name taken from the code provided by professor.
    struct passwd *pw = NULL;
    struct group *grp;

    //Concatenation code taken from https://stackoverflow.com/questions/5901181/c-string-append
    sprintf(dir_name, "%s/%s", directoryName, fileName);
    lstat(dir_name, &buffer);

    if (flag_i) {
        printf("%ld ", (long) buffer.st_ino);
    }

    if (flag_l) {
        printf("%11s ", permission(buffer.st_mode));

        pw = getpwuid(buffer.st_uid);
        grp = getgrgid(buffer.st_gid);

        char timeString[1024];
        time_t buffer_t = buffer.st_mtime;
        struct tm *time = localtime(&buffer_t);
        strftime(timeString, sizeof(timeString), "%b %d %Y %H:%M", time);

        printf("%3ld ", (long) buffer.st_nlink);
        (pw != NULL) ? printf("%10s ", pw->pw_name) : printf("%10s ", "");
        (grp != NULL) ? printf("%10s ", grp->gr_name) : printf("%10s ", "");
        printf("%10ld ", (long) buffer.st_size);
        printf("%s ", timeString);
    }

    printf("%s\n", fileName);
}

/*
 * This function takes in a directory name to which we have to traverse recursively ad uses printInformation
 * function to print out the details.
 */
void recursivePrintInfo(char *directoryName) {
    DIR *dir;
    struct dirent *read;

    if ((dir = opendir(directoryName)) == NULL) {
        printf ("./UnixLs %s: No such file or directory.\n", directoryName);
        exit(-1);
    }

    printf("%s\n", directoryName);

    while ((read = readdir(dir)) != NULL) {
        if (read->d_name[0] != '.') {
            printInformation(directoryName, read->d_name);
        }
    }
    printf("\n");

    closedir(dir);

    dir = opendir(directoryName);
    while ((read = readdir(dir)) != NULL) {
        if (read->d_name[0] != '.') {
            if (read->d_type == DT_DIR) {
                char dir_together [1024];
                sprintf (dir_together, "%s/%s", directoryName, read->d_name);
                recursivePrintInfo(dir_together);
            }
        }
    }
    closedir(dir);
}

/*
 * commandLS takes in the agruments given in by the main which are already verified that they are correct and
 * are in correct order. It just reads all the entries in the char array and complete the commands accordingly.
 *
 * There is an edge case when no command is given, it has been handles too.
 */
void commandLS(int argc, char **argv) {

    bool oneDir = true;
    DIR *directory;

    struct dirent *read;
    int count = 0;
    char directoryName[1024];

    for (int i = 1; i < argc; ++i) {
        if ((argv[i][0] != '-')) {
            if (i != argc - 1) oneDir = false;
            count++;

            if (flag_R) {
                sprintf (directoryName, "%s", argv[i]);
                recursivePrintInfo(directoryName);
            }
            else {
                if ((directory = opendir(argv[i])) == NULL) {
                    printf ("./UnixLs %s: No such file or directory.\n", argv[i]);
                    exit(-1);
                }


                if (!oneDir) printf("%s\n", argv[i]);
                while ((read = readdir(directory)) != NULL) {
                    if (read->d_name[0] != '.') {
                        printInformation(argv[i], read->d_name);
                    }
                }

                printf("\n");
                closedir(directory);
            }
        }
    }

    if (count == 0) {

        if (flag_R) {
            getcwd(directoryName, 1024);
            recursivePrintInfo(directoryName);
        } else {
            directory = opendir(".");
            while ((read = readdir(directory)) != NULL) {
                if (read->d_name[0] != '.') {
                    printInformation(".", read->d_name);
                }
            }
        }
    }
}

/*
 * We need to check every string given in as an input, does contains all the valid commands and
 * it turns all the flags which are asked to true and then passed into commandLS to complete those commands.
 */
bool checkString (const char* string) {

    for (int i = 1; ;++i) {
        if (string[i] == '\0') {
            if (i == 1) return false;
            break;
        }

        char flag = string[i];

        if (flag == 'i') flag_i = true;
        if (flag == 'l') flag_l = true;
        if (flag == 'R') flag_R = true;

        if (flag != 'i' && flag != 'l' && flag != 'R') {
            return false;
        }
    }
    return true;
}

/*
 * Main function checks whether any of the commands is not given in between any of the directories names.
 * If it is, then the function returns and nothing is printed and error is shown to the user.
 *
 * It also calls to check the string given in by user and if it returns, the commands are completed.
 */
int main(int argc, char **argv) {

    bool given_dir = false;

    for (int i = 1; i < argc; ++i) {
        if (argv [i][0] == '-') {
            
            if (given_dir) {
                printf ("./UnixLs: Commands have to come before directories names.\n");
                return -1;
            }

            if (!checkString (argv[i])) {
                printf ("./UnixLs: Commands can only contain i, l and R (in any combination).\n");
                return -1;
            }
        }
        else {
            given_dir = true;
        }
    }

    commandLS(argc, argv);
    printf ("\n");
}




