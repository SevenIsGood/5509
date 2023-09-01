/**************************************************************************************************
 Filename:       VerticalSeekBar.java

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

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.drawable.Drawable;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.widget.SeekBar;

public class VerticalSeekBar extends SeekBar {
    /**
     * The angle by which the SeekBar view should be rotated.
     */
    private static final int ROTATION_ANGLE = -90;

    /**
     * A change listener registrating start and stop of tracking. Need an own listener because the listener in SeekBar
     * is private.
     */
    private OnSeekBarChangeListener mOnSeekBarChangeListener;

    /**
     * Standard constructor to be implemented for all views.
     *
     * @param context The Context the view is running in, through which it can access the current theme, resources, etc.
     * @see android.view.View#View(Context)
     */
    public VerticalSeekBar(final Context context) {
        super(context);
    }

    /**
     * Standard constructor to be implemented for all views.
     *
     * @param context The Context the view is running in, through which it can access the current theme, resources, etc.
     * @param attrs   The attributes of the XML tag that is inflating the view.
     * @see android.view.View#View(Context, AttributeSet)
     */
    public VerticalSeekBar(final Context context, final AttributeSet attrs) {
        super(context, attrs);
    }

    /**
     * Standard constructor to be implemented for all views.
     *
     * @param context  The Context the view is running in, through which it can access the current theme, resources, etc.
     * @param attrs    The attributes of the XML tag that is inflating the view.
     * @param defStyle An attribute in the current theme that contains a reference to a style resource that supplies default
     *                 values for the view. Can be 0 to not look for defaults.
     * @see android.view.View#View(Context, AttributeSet, int)
     */
    public VerticalSeekBar(final Context context, final AttributeSet attrs, final int defStyle) {
        super(context, attrs, defStyle);
    }

    /*
     * (non-Javadoc) ${see_to_overridden}
     */
    @Override
    protected final void onSizeChanged(final int width, final int height, final int oldWidth, final int oldHeight) {
        super.onSizeChanged(height, width, oldHeight, oldWidth);
    }

    /*
     * (non-Javadoc) ${see_to_overridden}
     */
    @Override
    protected final synchronized void onMeasure(final int widthMeasureSpec, final int heightMeasureSpec) {
        super.onMeasure(heightMeasureSpec, widthMeasureSpec);
        setMeasuredDimension(getMeasuredHeight(), getMeasuredWidth());
    }

    /*
     * (non-Javadoc) ${see_to_overridden}
     */
    @Override
    protected final void onDraw(final Canvas c) {
        c.rotate(ROTATION_ANGLE);
        c.translate(-getHeight(), 0);

        super.onDraw(c);
    }

    /*
     * (non-Javadoc) ${see_to_overridden}
     */
    @Override
    public final void setOnSeekBarChangeListener(final OnSeekBarChangeListener listener) {
        // Do not use super for the listener, as this would not set the fromUser flag properly
        mOnSeekBarChangeListener = listener;
    }

    /*
     * (non-Javadoc) ${see_to_overridden}
     */
    @Override
    public final boolean onTouchEvent(final MotionEvent event) {
        if (!isEnabled()) {
            return false;
        }

        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                setProgressInternally(getMax() - (int) (getMax() * event.getY() / getHeight()), true);
                if (mOnSeekBarChangeListener != null) {
                    mOnSeekBarChangeListener.onStartTrackingTouch(this);
                }
                break;

            case MotionEvent.ACTION_MOVE:
                setProgressInternally(getMax() - (int) (getMax() * event.getY() / getHeight()), true);
                break;

            case MotionEvent.ACTION_UP:
                setProgressInternally(getMax() - (int) (getMax() * event.getY() / getHeight()), true);
                if (mOnSeekBarChangeListener != null) {
                    mOnSeekBarChangeListener.onStopTrackingTouch(this);
                }
                break;

            case MotionEvent.ACTION_CANCEL:
                if (mOnSeekBarChangeListener != null) {
                    mOnSeekBarChangeListener.onStopTrackingTouch(this);
                }
                break;

            default:
                break;
        }

        return true;
    }

    /**
     * Set the progress by the user. (Unfortunately, Seekbar.setProgressInternally(int, boolean) is not accessible.)
     *
     * @param progress the progress.
     * @param fromUser flag indicating if the change was done by the user.
     */
    public final void setProgressInternally(final int progress, final boolean fromUser) {
        if (progress != getProgress() && mOnSeekBarChangeListener != null) {
            mOnSeekBarChangeListener.onProgressChanged(this, progress, fromUser);
        }
        super.setProgress(progress);
        onSizeChanged(getWidth(), getHeight(), 0, 0);
    }

    @Override
    public void setBackgroundDrawable(Drawable background) {
        super.setBackgroundDrawable(background);
    }

    /*
         * (non-Javadoc) ${see_to_overridden}
         */
    @Override
    public final void setProgress(final int progress) {
        setProgressInternally(progress, false);
    }
}