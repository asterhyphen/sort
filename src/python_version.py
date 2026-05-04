import os
import sys

def process_directory(dirname):
    print(f"\n[INFO] Processing: {dirname}")

    if not os.path.isdir(dirname):
        print("[ERROR] Not a directory")
        return

    files = []

    for f in os.listdir(dirname):
        if f.startswith("."):
            continue

        fullpath = os.path.join(dirname, f)

        if not os.path.isfile(fullpath):
            continue

        stat = os.stat(fullpath)
        created = getattr(stat, "st_birthtime", stat.st_mtime)

        files.append((fullpath, created))

    if not files:
        print(f"[INFO] No files found in {dirname}")
        return

    files.sort(key=lambda x: x[1])

    renamed = 0

    for i, (path, _) in enumerate(files, start=1):
        base, ext = os.path.splitext(path)
        newname = os.path.join(dirname, f"{i}{ext}")

        if path != newname:
            try:
                os.rename(path, newname)
                print(f"[OK] {path} -> {newname}")
                renamed += 1
            except Exception as e:
                print(f"[ERROR] rename failed: {e}")

    print(f"[DONE] {renamed}/{len(files)} files renamed in {dirname}")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(f"[USAGE] {sys.argv[0]} <folder1> <folder2> ...")
        sys.exit(1)

    for folder in sys.argv[1:]:
        process_directory(folder)

    print("\nAll donee!.")
