#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>

typedef struct {
    char path[PATH_MAX];
    time_t created;
} FileEntry;

int compare(const void *a, const void *b) {
    FileEntry *fa = (FileEntry *)a;
    FileEntry *fb = (FileEntry *)b;
    return (fa->created > fb->created) - (fa->created < fb->created);
}

int main() {
    DIR *dir = opendir(".");
    if (!dir) {
        perror("opendir");
        return 1;
    }

    struct dirent *entry;
    FileEntry *files = NULL;
    int count = 0, capacity = 100;

    files = malloc(capacity * sizeof(FileEntry));
    if (!files) {
        perror("malloc");
        closedir(dir);
        return 1;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type != DT_REG) continue;
        if (entry->d_name[0] == '.') continue; // Hidden files will be ignored, remove this line if u want to rename hidden files as well

        struct stat sb;
        if (stat(entry->d_name, &sb) == -1) continue;

        if (count >= capacity) {
            capacity *= 2;
            files = realloc(files, capacity * sizeof(FileEntry));
            if (!files) {
                perror("realloc");
                closedir(dir);
                return 1;
            }
        }

        strncpy(files[count].path, entry->d_name, PATH_MAX);
        files[count].created = sb.st_birthtimespec.tv_sec;
        count++;
    }

    closedir(dir);
    qsort(files, count, sizeof(FileEntry), compare);

    for (int i = 0; i < count; i++) {
        char *ext = strrchr(files[i].path, '.');
        char newname[PATH_MAX];
        if (ext)
            snprintf(newname, sizeof(newname), "%d%s", i + 1, ext);
        else
            snprintf(newname, sizeof(newname), "%d", i + 1);

        if (strcmp(files[i].path, newname) != 0)
            rename(files[i].path, newname);
    }

    free(files);
    return 0;
}
