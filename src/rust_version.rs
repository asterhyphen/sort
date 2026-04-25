use std::cmp::Ordering;
use std::fs;
use std::io;
use std::path::Path;
use std::time::SystemTime;

#[derive(Debug)]
struct FileEntry {
    path: String,
    created: SystemTime,
}

fn main() -> io::Result<()> {
    let mut files: Vec<FileEntry> = Vec::new();

    for entry in fs::read_dir(".")? {
        let entry = entry?;
        let path = entry.path();

        // Skip non-files
        if !path.is_file() {
            continue;
        }

        let name = match path.file_name().and_then(|n| n.to_str()) {
            Some(n) => n,
            None => continue,
        };

        // Skip hidden files
        if name.starts_with('.') {
            continue;
        }

        let metadata = fs::metadata(&path)?;

        // Use created time if available, fallback to modified
        let created = metadata
            .created()
            .or_else(|_| metadata.modified())
            .unwrap_or(SystemTime::UNIX_EPOCH);

        files.push(FileEntry {
            path: name.to_string(),
            created,
        });
    }

    // Sort by creation time
    files.sort_by(|a, b| match a.created.cmp(&b.created) {
        Ordering::Less => Ordering::Less,
        Ordering::Greater => Ordering::Greater,
        Ordering::Equal => Ordering::Equal,
    });

    for (i, file) in files.iter().enumerate() {
        let path = Path::new(&file.path);

        let new_name = match path.extension().and_then(|e| e.to_str()) {
            Some(ext) => format!("{}.{}", i + 1, ext),
            None => format!("{}", i + 1),
        };

        if file.path != new_name {
            fs::rename(&file.path, &new_name)?;
        }
    }

    Ok(())
}
