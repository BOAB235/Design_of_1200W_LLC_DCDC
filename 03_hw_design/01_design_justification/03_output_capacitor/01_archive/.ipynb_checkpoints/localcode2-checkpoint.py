################## Thsi will be not explorted in PDF juste for internal pre-processing

from IPython.display import display, HTML


def show_figure_pair_with_captions(fig_counter, img1, caption1, img2=None, caption2=None, first_part_path=None):
    
    img1 = first_part_path+img1
    #global fig_counter
    if not img2 is None: 
        img2 = first_part_path+img2
        html = f"""
        <div style="display: flex; justify-content: center; width: 100%; gap: 1cm;">
          <div style="flex: 1; text-align: center; max-width: 9cm;">
            <img src="{img1}" style="width: 100%; height: auto;">
            <div style="word-wrap: break-word;">
                <b>Figure {fig_counter}:</b> {caption1}
            </div>
          </div>
          <div style="flex: 1; text-align: center; max-width: 9cm;">
            <img src="{img2}" style="width: 100%; height: auto;">
            <div style="word-wrap: break-word;">
                <b>Figure {fig_counter + 1}:</b> {caption2}
            </div>
          </div>
        </div>
        <br>
        """
        fig_counter += 2
    else: 
        html = f"""
        <div style="text-align: center; width: 100%;">
          <img src="{img1}" style="max-width: 19cm; width: 100%; height: auto;">
          <div style="word-wrap: break-word;">
              <b>Figure {fig_counter}:</b> {caption1}
          </div>
        </div>
        <br>
        """
        fig_counter += 1
    display(HTML(html))
    return fig_counter

def One_figure_with_captions(fig_counter, img1, caption1, img2=None, caption2=None, first_part_path=None, width=19):
    
    img1 = first_part_path+img1

    html = f"""
    <div style="width: 100%; text-align: center;">
        <img src="{img1}" style="max-width: 100%; width: {width}cm; height: auto;">
        <div style="max-width: 100%; word-wrap: break-word;">
            <b>Figure {fig_counter}:</b> {caption1}
        </div>
    </div>
    <br>
    """

    fig_counter += 1
    display(HTML(html))
    return fig_counter




# example of use 
#fig_counter = 1  # Global figure counter
_=""""
first_part_path = "./01_PWM_ESP32/01_Scope_screenshots/07_MCPWM_PTZ_all_regis/"
for i in range(0,len (figs),2):

    show_figure_pair_with_captions(
        figs[i],
        captions_list[i],
        figs[i+1],
        captions_list[i+1], 
        first_part_path= first_part_path, 
    )
"""