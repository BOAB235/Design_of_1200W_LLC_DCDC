import subprocess
from playwright.sync_api import sync_playwright
import os
from datetime import date
today = date.today().strftime("%Y-%m-%d")

# pip install playwright
# playwright install chromium


################# INPUTS######################
notebook = "EMC_Conducted_Emissions_Test_V4.ipynb"
title="" # empty = default title : first h1

##############################################

notebook = notebook .replace(".ipynb","")

# Convert notebook to HTML
subprocess.run(['jupyter', 'nbconvert', f'{notebook}.ipynb', '--to', 'html', '--no-input', '--EmbedImagesPreprocessor.enabled=True'])


print(f"✓ {notebook}.html created!")

# Convert HTML to PDF
with sync_playwright() as p:
    browser = p.chromium.launch()
    page = browser.new_page()
    page.goto(f'file:///{os.path.abspath(notebook + ".html").replace(os.sep, "/")}')
    page.add_style_tag(content='#Table-of-contents { page-break-before: always !important; }')
    
    if title =="":
        # After page.goto()
        title = page.locator('h1').first.inner_text()
    
    page.pdf(
        path=f'{notebook}.pdf',
        format='A4',
        margin={'top': '25mm', 'bottom': '25mm', 'left': '10mm', 'right': '10mm'},
        display_header_footer=True,
        #header_template=f'<div style="font-size:9px; text-align:center; width:100%;"><a href="https://github.com/BOAB235/Design_of_1200W_LLC_DCDC">1200W 400V/48V LLC DC-DC Converter Design</a> -  {title}</div>',
        header_template=f'<div style="font-size:9px; text-align:center; width:100%;"><a href="https://github.com/BOAB235/Design_of_1200W_LLC_DCDC" style="color:#333; text-decoration:none;">1200W 400V/48V LLC DC-DC Converter Design</a> - {title}</div>',
        footer_template=f'<div style="font-size:9px; width:100%; display:flex; justify-content:space-between; padding:0 10mm;"><span>Abdelaziz BOUZIANI         {today}</span><span class="pageNumber"></span></div>'
    )
    browser.close()

print(f"✓ {notebook}.pdf created!")
