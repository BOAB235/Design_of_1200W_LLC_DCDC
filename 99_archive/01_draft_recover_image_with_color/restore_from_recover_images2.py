import os
import shutil
import glob
from PIL import Image

# === CONFIGURATION ===
new_backup_folders = [
    r'C:\Users\a037702\Downloads\recover_images2\old_images_with_metadata',
    r'C:\Users\a037702\Downloads\recover_images2\old_images_with_metadata__',
]
project_dir = r'C:\Users\a037702\NotSynchronized\git\Design_of_1200W_LLC_DCDC'

print("="*100)
print("RECOVERY FROM NEW FOLDER: recover_images2")
print("="*100)

# === STEP 1: Scan new backup folders ===
print("\nSTEP 1: Scanning new backup folders...")
total_images_found = {}

for backup_dir in new_backup_folders:
    if not os.path.exists(backup_dir):
        print(f"  ✗ Not found: {os.path.basename(backup_dir)}")
        continue
    
    backup_name = os.path.basename(backup_dir)
    print(f"\n  Scanning {backup_name}...")
    
    # Find all PNGs/JPGs in backup
    patterns = ['**/*.png', '**/*.jpg', '**/*.jpeg']
    backup_images = []
    for pattern in patterns:
        backup_images.extend(glob.glob(os.path.join(backup_dir, pattern), recursive=True))
    
    total_images_found[backup_name] = len(backup_images)
    color_images = 0
    
    for img_path in backup_images:
        try:
            img = Image.open(img_path)
            if img.mode != 'L':  # Not grayscale
                color_images += 1
        except:
            pass
    
    print(f"    Total images: {len(backup_images)}")
    print(f"    Color images: {color_images}")
    print(f"    Grayscale: {len(backup_images) - color_images}")

# === STEP 2: Restore color images ===
print("\n" + "="*100)
print("STEP 2: Restoring color images from new backups...")
print("="*100)

restored_from_backup = {}
skipped_already_color = 0

for backup_dir in new_backup_folders:
    if not os.path.exists(backup_dir):
        continue
    
    backup_name = os.path.basename(backup_dir)
    print(f"\n  Processing {backup_name}...")
    
    # Find all PNGs/JPGs in backup
    patterns = ['**/*.png', '**/*.jpg', '**/*.jpeg']
    backup_images = []
    for pattern in patterns:
        backup_images.extend(glob.glob(os.path.join(backup_dir, pattern), recursive=True))
    
    count = 0
    for source_path in backup_images:
        try:
            # Check if it's color
            img = Image.open(source_path)
            if img.mode == 'L':  # Skip grayscale
                continue
            
            # Get destination path
            rel_path = os.path.relpath(source_path, backup_dir)
            dest_path = os.path.join(project_dir, rel_path)
            
            # Only restore if destination is grayscale or missing
            restore = False
            if not os.path.exists(dest_path):
                restore = True
            else:
                dest_img = Image.open(dest_path)
                if dest_img.mode == 'L':  # Destination is grayscale
                    restore = True
                else:
                    skipped_already_color += 1
            
            if restore:
                os.makedirs(os.path.dirname(dest_path), exist_ok=True)
                shutil.copy2(source_path, dest_path)
                restored_from_backup[rel_path] = backup_name
                count += 1
                if count % 50 == 0:
                    print(f"    Restored {count}...")
                
        except Exception as e:
            pass
    
    print(f"    ✓ Restored {count} color images from {backup_name}")

print(f"\n  Total restored from new backups: {len(restored_from_backup)}")
print(f"  Skipped (already color): {skipped_already_color}")

# === STEP 3: Final verification ===
print("\n" + "="*100)
print("FINAL VERIFICATION")
print("="*100)

os.chdir(project_dir)
all_images = (
    glob.glob("**/*.jpg", recursive=True)
    + glob.glob("**/*.jpeg", recursive=True)
    + glob.glob("**/*.png", recursive=True)
)

color_count = 0
gray_count = 0
for img_path in all_images:
    try:
        img = Image.open(img_path)
        if img.mode == 'L':
            gray_count += 1
        else:
            color_count += 1
    except:
        pass

print(f"\nFinal status:")
print(f"  ✓ Color images: {color_count}")
print(f"  ✗ Grayscale images: {gray_count}")
print(f"\n  Restored in this run: {len(restored_from_backup)}")

if restored_from_backup:
    print(f"\nFirst 10 restored files:")
    for i, (path, source) in enumerate(list(restored_from_backup.items())[:10]):
        print(f"  {i+1}. {path}")
        print(f"     (from {source})")

if gray_count == 0:
    print("\n✓✓✓ ALL IMAGES IN COLOR! ✓✓✓")
else:
    print(f"\n⚠ {gray_count} images still grayscale")

print("="*100)
