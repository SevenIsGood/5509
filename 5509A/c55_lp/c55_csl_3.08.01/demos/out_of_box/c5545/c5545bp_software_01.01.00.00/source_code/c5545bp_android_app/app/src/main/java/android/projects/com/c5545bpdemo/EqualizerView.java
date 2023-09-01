/**************************************************************************************************
 Filename:       EqualizerView.java

 Copyright (c) 2016 Texas Instruments Incorporated

 All rights reserved not granted herein.
 Limited License.

 Texas Instruments Incorporated grants a world-wide, royalty-free,
 non-exclusive license under copyrights and patents it now or hereafter
 owns or controls to make, have made, use, import, offer to sell and sell ("Utilize")
 this software subject to the terms herein.  With respect to the foregoing patent
 license, such license is granted  solely to the extent that any such patent is necessary
 to Utilize the software alone.  The patent license shall not apply to any combinations which
 include this software, other than combinations with devices manufactured by or for TI ('TI Devices').
 No hardware patent is licensed hereunder.

 Redistributions must preserve existing copyright notices and reproduce this license (including the
 above copyright notice and the disclaimer and (if applicable) source code license limitations below)
 in the documentation and/or other materials provided with the distribution

 Redistribution and use in binary form, without modification, are permitted provided that the following
 conditions are met:

 * No reverse engineering, decompilation, or disassembly of this software is permitted with respect to any
 software provided in binary form.
 * any redistribution and use are licensed by TI for use only with TI Devices.
 * Nothing shall obligate TI to provide you with source code for the software licensed and provided to you in object code.

 If software source code is provided to you, modification and redistribution of the source code are permitted
 provided that the following conditions are met:

 * any redistribution and use of the source code, including any resulting derivative works, are licensed by
 TI for use only with TI Devices.
 * any redistribution and use of any object code compiled from the source code and any resulting derivative
 works, are licensed by TI for use only with TI Devices.

 Neither the name of Texas Instruments Incorporated nor the names of its suppliers may be used to endorse or
 promote products derived from this software without specific prior written permission.

 DISCLAIMER.

 THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.


 **************************************************************************************************/

package android.projects.com.c5545bpdemo;

import android.graphics.Rect;
import android.graphics.drawable.ColorDrawable;
import android.media.MediaPlayer;
import android.media.audiofx.Equalizer;
import android.os.Bundle;
import android.projects.com.c5545ble.R;
import android.support.v4.app.Fragment;
import android.support.v4.content.ContextCompat;
import android.support.v4.content.res.ResourcesCompat;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.SeekBar;
import android.widget.TextView;

/**
 * Created by mistral on 20/1/16.
 */

/**
 * A placeholder fragment containing a simple view.
 */
public class EqualizerView extends Fragment implements SeekBar.OnSeekBarChangeListener {

    static final int MAX_SLIDERS = 5;
    VerticalSeekBar sliders[] = new VerticalSeekBar[MAX_SLIDERS];
    TextView slider_labels[] = new TextView[MAX_SLIDERS];
    int num_sliders = 0;
    int min_level = 0;
    int max_level = 30;
    Equalizer mEqualizer = null;
    ImageView imgPlayStatsLogo;
    TextView imgPlayStatsText;
    private MediaPlayer mMediaPlayer;
    private C5545Equalizer mActivity = null;
    RelativeLayout layout;
    public EqualizerView() {

    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {

        View view = inflater.inflate(R.layout.fragment_equalizer, container, false);
        layout = (RelativeLayout)view.findViewById(R.id.main_layout);
        mActivity = (C5545Equalizer) getActivity();

        sliders[0] = (VerticalSeekBar) view.findViewById(R.id.seekBar1);
        slider_labels[0] = (TextView) view.findViewById(R.id.slider_label_1);
        sliders[1] = (VerticalSeekBar) view.findViewById(R.id.seekBar2);
        slider_labels[1] = (TextView) view.findViewById(R.id.slider_label_2);
        sliders[2] = (VerticalSeekBar) view.findViewById(R.id.seekBar3);
        slider_labels[2] = (TextView) view.findViewById(R.id.slider_label_3);
        sliders[3] = (VerticalSeekBar) view.findViewById(R.id.seekBar4);
        slider_labels[3] = (TextView) view.findViewById(R.id.slider_label_4);
        sliders[4] = (VerticalSeekBar) view.findViewById(R.id.seekBar5);
        slider_labels[4] = (TextView) view.findViewById(R.id.slider_label_5);
        imgPlayStatsLogo = (ImageView) view.findViewById(R.id.img_logo_text);
        imgPlayStatsText = (TextView) view.findViewById(R.id.musicStatusLabel);

        mMediaPlayer = MediaPlayer.create(getActivity().getApplicationContext(), R.raw.test_cbr);
        mEqualizer = new Equalizer(0, mMediaPlayer.getAudioSessionId());

        if (mEqualizer != null) {

            mEqualizer.setEnabled(true);
            int num_bands = mEqualizer.getNumberOfBands();
            num_sliders = num_bands;
            short r[] = mEqualizer.getBandLevelRange();
            min_level = r[0];
            max_level = r[1];

            for (int i = 0; i < num_sliders && i < MAX_SLIDERS; i++) {
                sliders[i].setOnSeekBarChangeListener(this);
            }

        }
        return view;
    }

    private void disableEnableControls(boolean enable, ViewGroup vg){
        if(null == vg)
        {
            return;
        }
        for (int i = 0; i < vg.getChildCount(); i++){
            View child = vg.getChildAt(i);

            child.setEnabled(enable);
            if(enable)
            {
                for(int slider = 0;slider<sliders.length;slider++) {
                    Rect bounds = sliders[slider].getProgressDrawable().getBounds();
                    sliders[slider].setProgressDrawable(ContextCompat.getDrawable(getActivity(), R.drawable.progressbar));
                    sliders[slider].getProgressDrawable().setBounds(bounds);
                }
            }
            else
            {
                for(int slider = 0;slider<sliders.length;slider++) {
                    Rect bounds = sliders[slider].getProgressDrawable().getBounds();
                    sliders[slider].setProgressDrawable(ContextCompat.getDrawable(getActivity(), R.drawable.progressbar_disable));
                    sliders[slider].getProgressDrawable().setBounds(bounds);
                }
            }
            if (child instanceof ViewGroup){
                disableEnableControls(enable, (ViewGroup)child);
            }
        }
    }
    void setPlayStausNotificatiion(int statusID) {
        switch (statusID) {
            case 0:
                imgPlayStatsLogo.setImageResource(R.drawable.stop);
                imgPlayStatsText.setText(R.string.stop_music);
                disableEnableControls(false,layout);
                break;
            case 1:
                imgPlayStatsLogo.setImageResource(R.drawable.play);
                imgPlayStatsText.setText(R.string.play_music);
                disableEnableControls(true,layout);
                break;
            case 2:
                imgPlayStatsLogo.setImageResource(R.drawable.pause);
                imgPlayStatsText.setText(R.string.pause_music);
                disableEnableControls(false,layout);
                break;
            default:
                break;
        }

    }

    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);

    }

    @Override
    public void onProgressChanged(SeekBar seekBar, int level, boolean fromUser) {

    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {
        Log.d(C5545AppClass.TAG, "Equalizer Name onStart " + seekBar.getProgress());
    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {

        int setValue = seekBar.getProgress() - 15;
        int seekBarId = -1;
        if (sliders[0].getId() == seekBar.getId()) {
            seekBarId = 0;
        }
        if (sliders[1].getId() == seekBar.getId()) {
            seekBarId = 1;
        }
        if (sliders[2].getId() == seekBar.getId()) {
            seekBarId = 2;
        }
        if (sliders[3].getId() == seekBar.getId()) {
            seekBarId = 3;
        }
        if (sliders[4].getId() == seekBar.getId()) {
            seekBarId = 4;
        }
        mActivity.setEqualizer(seekBarId, setValue);
    }

    public void updateSliders(int[] array) {
        for (int j = 0; j < array.length; j++) {
            int newIntValue = 15 + array[j];
            sliders[j].setProgress(newIntValue);
        }
    }

}