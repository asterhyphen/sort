use std::cmp::Ordering;
use std::env;
use std::fs;
use std::io;
use std::path::{Path, PathBuf};
use std::time::SystemTime;

#[derive(Debug)]
struct FileEntry {
    path: PathBuf,
    created: SystemTime,
}

fn process_directory(dirname: &str) -> io::Result<()> {
    println!("\n[INFO] Processing: {}", dirname);

    let dir_path = Path::new(dirname);

    if !dir_path.is_dir() {
        println!("[ERROR] Not a directory");
        return Ok(());
    }

    let mut files: Vec<FileEntry> = Vec::new();

    for entry in fs::read_dir(dir_path)? {
        let entry = entry?;
        let path = entry.path();

        if !path.is_file() {
            continue;
        }

        let name = match path.file_name().and_then(|n| n.to_str()) {
            Some(n) => n,
            None => continue,
        };

        if name.starts_with('.') {
            continue;
        }

        let metadata = fs::metadata(&path)?;

        let created = metadata
            .created()
            .or_else(|_| metadata.modified())
            .unwrap_or(SystemTime::UNIX_EPOCH);

        files.push(FileEntry { path, created });
    }

    if files.is_empty() {
        println!("[INFO] No files found in {}", dirname);
        return Ok(());
    }

    files.sort_by(|a, b| match a.created.cmp(&b.created) {
        Ordering::Less => Ordering::Less,
        Ordering::Greater => Ordering::Greater,
        Ordering::Equal => Ordering::Equal,
    });

    let mut renamed = 0;

    for (i, file) in files.iter().enumerate() {
        let ext = file.path.extension().and_then(|e| e.to_str());

        let new_name = match ext {
            Some(ext) => format!("{}.{}", i + 1, ext),
            None => format!("{}", i + 1),
        };

        let new_path = dir_path.join(new_name);

        if file.path != new_path {
            match fs::rename(&file.path, &new_path) {
                Ok(_) => {
                    println!("[OK] {:?} -> {:?}", file.path, new_path);
                    renamed += 1;
                }
                Err(e) => {
                    println!("[ERROR] rename failed: {}", e);
                }
            }
        }
    }

    println!(
        "[DONE] {}/{} files renamed in {}",
        renamed,
        files.len(),
        dirname
    );

    Ok(())
}

fn main() -> io::Result<()> {
    let args: Vec<String> = env::args().collect();

    if args.len() < 2 {
        println!("[USAGE] {} <folder1> <folder2> ...", args[0]);
        return Ok(());
    }

    for dir in &args[1..] {
        process_directory(dir)?;
    }

    println!("\nAll donee!.");
    Ok(())
}
