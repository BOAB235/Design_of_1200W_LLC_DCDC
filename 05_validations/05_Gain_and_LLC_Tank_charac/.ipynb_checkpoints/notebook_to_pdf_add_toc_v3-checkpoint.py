import subprocess
from playwright.sync_api import sync_playwright
import os
from datetime import date
from bs4 import BeautifulSoup
import glob

today = date.today().strftime("%Y-%m-%d")

################# INPUTS######################
# ADD ## Table of contents
notebook = "01_LLC_48v_LLC_Magnetic_Cores_V3.ipynb"
title="" # empty = default title : first h1
##############################################


#################### GET NAME OF NOTEBOOK AUTOMATICALLY
ipynb_files = glob.glob("**/*.ipynb", recursive=True)
print(ipynb_files)

notebook= ipynb_files[0]
print(notebook )
###################################################


notebook = notebook.replace(".ipynb","")

# Convert notebook to HTML
subprocess.run(['jupyter', 'nbconvert', f'{notebook}.ipynb', '--to', 'html', '--no-input', '--EmbedImagesPreprocessor.enabled=True'])

# Generate TOC in HTML
with open(f'{notebook}.html', 'r', encoding='utf-8') as f:
    soup = BeautifulSoup(f.read(), 'html.parser')

# Find "Table of contents" heading
# Use get_text() instead of string because the heading may contain nested tags (like anchor links)
toc_heading = None
for heading in soup.find_all(['h1', 'h2', 'h3']):
    if 'Table of contents' in heading.get_text():
        toc_heading = heading
        break

if not toc_heading:
    print("ERROR: No '## Table of contents' heading found in notebook!")
    print("Add '## Table of contents' to your notebook where you want the TOC.")
    exit()

toc_heading['id'] = 'Table-of-contents'

# Find the cell container that holds the TOC heading
toc_cell = toc_heading.find_parent('div', class_='jp-Cell')
if not toc_cell:
    # Fallback for older notebook HTML structure
    toc_cell = toc_heading.find_parent('div', class_='cell')

# Remove old TOC cell if it exists (next cell after TOC heading)
if toc_cell:
    next_cell = toc_cell.find_next_sibling('div', class_='jp-Cell')
    if next_cell and next_cell.find('ul', class_='toc-list'):
        next_cell.decompose()

# Get all headings except TOC heading and title
all_headings = soup.find_all(['h1', 'h2', 'h3'])
headings = [h for h in all_headings if h != toc_heading][1:]  # Exclude TOC and skip title

# Generate TOC
toc_lines = []
prev_level = 2  # Start at level 2 (h2)

for i, h in enumerate(headings, 1):
    level = int(h.name[1])
    h['id'] = f'title_{i}'
    text = h.get_text().replace("¶", "").strip()
    
    if level > prev_level:
        toc_lines.append('<ul>')
    elif level < prev_level:
        for _ in range(prev_level - level):
            toc_lines.append('</ul></li>')
    
    toc_lines.append(f'<li><a href="#title_{i}">{text}</a>')
    prev_level = level

for _ in range(prev_level - 2):
    toc_lines.append('</ul></li>')

# Create TOC HTML with proper structure
toc_list_html = f'<ul class="toc-list">{"".join(toc_lines)}</ul>'

# Add CSS for TOC styling
toc_style = '''
<style>
.toc-list {
    list-style-type: none;
    padding-left: 0;
    line-height: 1.8;
}
.toc-list li {
    margin: 5px 0;
}
.toc-list ul {
    list-style-type: none;
    padding-left: 20px;
}
.toc-list a {
    text-decoration: none;
    color: #1976d2;
}
.toc-list a:hover {
    text-decoration: underline;
}
</style>
'''

# Create a new cell div to hold the TOC (matching Jupyter's structure)
toc_container_html = f'''
<div class="jp-Cell jp-MarkdownCell jp-Notebook-cell">
<div class="jp-Cell-inputWrapper" tabindex="0">
<div class="jp-Collapser jp-InputCollapser jp-Cell-inputCollapser"></div>
<div class="jp-InputArea jp-Cell-inputArea">
<div class="jp-RenderedHTMLCommon jp-RenderedMarkdown jp-MarkdownOutput" data-mime-type="text/markdown">
{toc_style}
{toc_list_html}
</div>
</div>
</div>
</div>
'''

# Insert the TOC container after the TOC heading cell
if toc_cell:
    toc_cell.insert_after(BeautifulSoup(toc_container_html, 'html.parser'))
else:
    # Fallback: insert directly after heading
    toc_heading.insert_after(BeautifulSoup(toc_list_html, 'html.parser'))

with open(f'{notebook}.html', 'w', encoding='utf-8') as f:
    f.write(str(soup))

print(f"✓ {notebook}.html with TOC created!")

# Convert HTML to PDF
print("Generating PDF (this may take a while for large notebooks)...")
with sync_playwright() as p:
    browser = p.chromium.launch()
    page = browser.new_page()
    
    # Set longer timeout for large files
    page.set_default_timeout(120000)  # 2 minutes
    
    # Load the HTML file
    page.goto(f'file:///{os.path.abspath(notebook + ".html").replace(os.sep, "/")}', wait_until='networkidle')
    
    # Add page break before TOC
    page.add_style_tag(content='#Table-of-contents { page-break-before: always !important; }')
    
    if title =="":
        title = page.locator('h1').first.inner_text()
    
    page.pdf(
        path=f'{notebook}.pdf',
        format='A4',
        margin={'top': '25mm', 'bottom': '25mm', 'left': '10mm', 'right': '10mm'},
        display_header_footer=True,
        header_template=f'<div style="font-size:9px; text-align:center; width:100%;"><a href="https://github.com/BOAB235/Design_of_1200W_LLC_DCDC" style="color:#054270; text-decoration:none;">1200W 400V/48V LLC DC-DC Converter Design</a> - {title}</div>',
        footer_template=f'<div style="font-size:9px; width:100%; display:flex; justify-content:space-between; padding:0 10mm;"><span>Abdelaziz BOUZIANI         {today}</span><span class="pageNumber"></span></div>'
    )
    browser.close()

print(f"✓ {notebook}.pdf created!")
