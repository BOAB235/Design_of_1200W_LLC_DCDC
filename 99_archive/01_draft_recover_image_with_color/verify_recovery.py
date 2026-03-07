import os
import glob
from PIL import Image

# Paths
source_dir = r'C:\Users\a037702\Downloads\recouver_images\old_images_with_metadata_5'
project_dir = r'C:\Users\a037702\NotSynchronized\git\Design_of_1200W_LLC_DCDC'

print("="*100)
print("VERIFICATION: Compare backup vs current project images")
print("="*100)

# Find all oscilloscope screenshots in backup
patterns = ['2025-*.png', '2026-*.png']
backup_images = []
for pattern in patterns:
    backup_images.extend(glob.glob(os.path.join(source_dir, '**', pattern), recursive=True))

print(f"\n1. BACKUP folder has {len(backup_images)} oscilloscope screenshots")

# Check each one in the project
missing = []
grayscale_still = []
color_ok = []

for backup_path in backup_images:
    rel_path = os.path.relpath(backup_path, source_dir)
    project_path = os.path.join(project_dir, rel_path)
    
    # Check if file exists in project
    if not os.path.exists(project_path):
        missing.append(rel_path)
        continue
    
    # Check if it's color or grayscale
    try:
        img = Image.open(project_path)
        backup_img = Image.open(backup_path)
        
        # Mode 'L' = grayscale, 'RGB'/'RGBA'/'P' = color
        if img.mode == 'L':
            grayscale_still.append((rel_path, img.mode, backup_img.mode))
        else:
            color_ok.append(rel_path)
    except Exception as e:
        print(f"ERROR checking {rel_path}: {e}")

print(f"\n2. PROJECT CHECK:")
print(f"   ✓ Color restored correctly: {len(color_ok)}")
print(f"   ✗ Still grayscale: {len(grayscale_still)}")
print(f"   ✗ Missing from project: {len(missing)}")

if missing:
    print("\n" + "="*100)
    print("MISSING FILES:")
    for f in missing[:10]:
        print(f"  - {f}")
    if len(missing) > 10:
        print(f"  ... and {len(missing)-10} more")

if grayscale_still:
    print("\n" + "="*100)
    print("STILL GRAYSCALE (need re-restoration):")
    for f, mode, backup_mode in grayscale_still[:10]:
        print(f"  - {f} (current: {mode}, backup: {backup_mode})")
    if len(grayscale_still) > 10:
        print(f"  ... and {len(grayscale_still)-10} more")

print("\n" + "="*100)
if len(color_ok) == len(backup_images):
    print("✓✓✓ PERFECT! All images recovered successfully in COLOR ✓✓✓")
else:
    print(f"⚠ WARNING: {len(backup_images) - len(color_ok)} images need attention")
print("="*100)
