import json
import re

# Get notebook path
notebook_path = "01_LLC_48v_LLC_tank_design_note_v2.ipynb"

# Read notebook
with open(notebook_path, 'r', encoding='utf-8') as f:
    notebook = json.load(f)

# Process markdown cells
for cell in notebook['cells']:
    if cell['cell_type'] == 'markdown':
        for i, line in enumerate(cell['source']):
            # Check if line is a heading (starts with #)
            if line.lstrip().startswith('#'):
                # Remove <a> tags from heading lines only
                new_line = re.sub(r'\s*<a[^>]*>.*?</a>\s*', '', line).rstrip()
                # Keep newline if original had it
                cell['source'][i] = new_line + ('\n' if line.endswith('\n') else '')

# Save notebook
with open(notebook_path, 'w', encoding='utf-8') as f:
    json.dump(notebook, f, indent=1, ensure_ascii=False)

print(f"Done! Removed anchor tags from titles in: {notebook_path}")
