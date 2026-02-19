from playwright.sync_api import sync_playwright

with sync_playwright() as p:
    browser = p.chromium.launch()
    page = browser.new_page()
    page.goto('file:///C:/Users/a037702/NotSynchronized/git/Design_of_1200W_LLC_DCDC/05_validations/02_emc/01_Conducted_Emissions_Test/EMC_Conducted_Emissions_Test_V4.html')
    
    page.add_style_tag(content='#Table-of-contents { page-break-before: always !important; }')
    
    page.pdf(
        path='EMC_Conducted_Emissions_Test_V4.pdf',
        format='A4',
        margin={'top': '25mm', 'bottom': '25mm', 'left': '10mm', 'right': '10mm'},
        display_header_footer=True,
        header_template='<div style="font-size:9px; text-align:center; width:100%;">EMC Conducted Emissions Test - 1200W LLC DC/DC</div>',
        footer_template='<div style="font-size:9px; width:100%; display:flex; justify-content:space-between; padding:0 10mm;"><span>Your Name</span><span class="pageNumber"></span></div>'
    )
    browser.close()
