import os
import shutil
import glob
import subprocess
from PIL import Image

# === CONFIGURATION ===
backup_folders = [
    r'C:\Users\a037702\Downloads\recouver_images\Images_no_Meta_2',  # Most complete
    r'C:\Users\a037702\Downloads\recouver_images\Images_no_Meta_0',
    r'C:\Users\a037702\Downloads\recouver_images\old_images_with_metadata_5',
]
project_dir = r'C:\Users\a037702\NotSynchronized\git\Design_of_1200W_LLC_DCDC'

print("="*100)
print("COMPLETE RECOVERY: Restore ALL color images")
print("="*100)

# === STEP 1: Restore from backup folders ===
print("\nSTEP 1: Restoring from backup folders...")
restored_from_backup = {}

for backup_dir in backup_folders:
    if not os.path.exists(backup_dir):
        continue
    
    backup_name = os.path.basename(backup_dir)
    print(f"\n  Scanning {backup_name}...")
    
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
            
            if restore:
                os.makedirs(os.path.dirname(dest_path), exist_ok=True)
                shutil.copy2(source_path, dest_path)
                restored_from_backup[rel_path] = backup_name
                count += 1
                
        except Exception as e:
            pass
    
    print(f"    ✓ Restored {count} color images from {backup_name}")

print(f"\n  Total restored from backups: {len(restored_from_backup)}")

# === STEP 2: Check what's still grayscale ===
print("\nSTEP 2: Checking for remaining grayscale images...")

os.chdir(project_dir)
all_images = (
    glob.glob("**/*.jpg", recursive=True)
    + glob.glob("**/*.jpeg", recursive=True)
    + glob.glob("**/*.png", recursive=True)
)

still_grayscale = []
for img_path in all_images:
    try:
        img = Image.open(img_path)
        if img.mode == 'L':
            still_grayscale.append(img_path)
    except:
        pass

print(f"  Still grayscale: {len(still_grayscale)}")

# === STEP 3: Restore remaining from git ===
if still_grayscale:
    print("\nSTEP 3: Restoring remaining images from git...")
    
    restored_from_git = 0
    not_in_git = []
    
    for img_path in still_grayscale:
        try:
            # Check if exists in git
            result = subprocess.run(
                ['git', 'ls-tree', 'HEAD', '--', img_path],
                capture_output=True,
                text=True,
                timeout=2
            )
            
            if result.returncode == 0 and result.stdout.strip():
                # Restore from git
                result = subprocess.run(
                    ['git', 'checkout', 'HEAD', '--', img_path],
                    capture_output=True,
                    timeout=5
                )
                if result.returncode == 0:
                    restored_from_git += 1
                    if restored_from_git % 100 == 0:
                        print(f"    Restored {restored_from_git}...")
            else:
                not_in_git.append(img_path)
                
        except:
            pass
    
    print(f"  ✓ Restored from git: {restored_from_git}")
    print(f"  ✗ Not recoverable (not in any backup or git): {len(not_in_git)}")
    
    if not_in_git:
        print(f"\n  First 10 unrecoverable:")
        for f in not_in_git[:10]:
            print(f"    - {f}")
else:
    print("\nSTEP 3: Skipped (no grayscale images remaining)")

# === FINAL VERIFICATION ===
print("\n" + "="*100)
print("FINAL VERIFICATION")
print("="*100)

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
print(f"\n  Total recovered: {len(restored_from_backup) + (restored_from_git if still_grayscale else 0)}")

if gray_count == 0:
    print("\n✓✓✓ ALL IMAGES SUCCESSFULLY RECOVERED IN COLOR! ✓✓✓")
else:
    print(f"\n⚠ {gray_count} images remain grayscale (not recoverable)")

print("="*100)
