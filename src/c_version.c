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

void process_directory(const char *dirname) {
    printf("\n[INFO] Processing: %s\n", dirname);

    DIR *dir = opendir(dirname);
    if (!dir) {
        perror("[ERROR] opendir failed");
        return;
    }

    struct dirent *entry;
    FileEntry *files = NULL;
    int count = 0, capacity = 100;

    files = malloc(capacity * sizeof(FileEntry));
    if (!files) {
        perror("[ERROR] malloc failed");
        closedir(dir);
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type != DT_REG) continue;
        if (entry->d_name[0] == '.') continue;

        char fullpath[PATH_MAX];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", dirname, entry->d_name);

        struct stat sb;
        if (stat(fullpath, &sb) == -1) {
            perror("[WARN] stat failed");
            continue;
        }

        if (count >= capacity) {
            capacity *= 2;
            FileEntry *tmp = realloc(files, capacity * sizeof(FileEntry));
            if (!tmp) {
                perror("[ERROR] realloc failed");
                free(files);
                closedir(dir);
                return;
            }
            files = tmp;
        }

        strncpy(files[count].path, fullpath, PATH_MAX);
        files[count].created = sb.st_mtime;
        count++;
    }

    closedir(dir);

    if (count == 0) {
        printf("[INFO] No files found in %s\n", dirname);
        free(files);
        return;
    }

    qsort(files, count, sizeof(FileEntry), compare);

    int renamed = 0;
    for (int i = 0; i < count; i++) {
        char *ext = strrchr(files[i].path, '.');

        char newname[PATH_MAX];
        if (ext)
            snprintf(newname, sizeof(newname), "%s/%d%s", dirname, i + 1, ext);
        else
            snprintf(newname, sizeof(newname), "%s/%d", dirname, i + 1);

        if (strcmp(files[i].path, newname) != 0) {
            if (rename(files[i].path, newname) == 0) {
                printf("[OK] %s -> %s\n", files[i].path, newname);
                renamed++;
            } else {
                perror("[ERROR] rename failed");
            }
        }
    }

    printf("[DONE] %d/%d files renamed in %s\n", renamed, count, dirname);

    free(files);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("[USAGE] %s <folder1> <folder2> ...\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        process_directory(argv[i]);
    }

    printf("\nAll donee!.\n");
    return 0;
}
