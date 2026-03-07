import os
import shutil
import glob

# Source: recovered folder with original COLOR images
source_dir = r'C:\Users\a037702\Downloads\recouver_images\old_images_with_metadata_5'

# Destination: your project directory
dest_dir = r'C:\Users\a037702\NotSynchronized\git\Design_of_1200W_LLC_DCDC'

# Find all oscilloscope screenshots (2025-* and 2026-* patterns)
patterns = ['2025-*.png', '2026-*.png']

restored_count = 0
failed = []

for pattern in patterns:
    search_pattern = os.path.join(source_dir, '**', pattern)
    source_images = glob.glob(search_pattern, recursive=True)
    
    print(f"\nFound {len(source_images)} images matching pattern: {pattern}")
    
    for source_path in source_images:
        # Get relative path from source_dir
        rel_path = os.path.relpath(source_path, source_dir)
        
        # Build destination path
        dest_path = os.path.join(dest_dir, rel_path)
        
        try:
            # Ensure destination directory exists
            os.makedirs(os.path.dirname(dest_path), exist_ok=True)
            
            # Copy file (overwrite grayscale with color version)
            shutil.copy2(source_path, dest_path)
            restored_count += 1
            print(f"✓ Restored: {rel_path}")
            
        except Exception as e:
            failed.append((rel_path, str(e)))
            print(f"✗ FAILED: {rel_path} - {e}")

print("\n" + "="*100)
print(f"RESTORATION COMPLETE")
print(f"Total restored: {restored_count}")
print(f"Failed: {len(failed)}")
print("="*100)

if failed:
    print("\nFailed files:")
    for f, err in failed:
        print(f"  - {f}: {err}")
