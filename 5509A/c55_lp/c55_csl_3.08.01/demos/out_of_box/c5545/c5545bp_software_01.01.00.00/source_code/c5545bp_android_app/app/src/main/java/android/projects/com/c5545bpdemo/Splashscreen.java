/**************************************************************************************************
 Filename:       Splashscreen.java

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

import android.app.ActionBar;
import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.projects.com.c5545ble.R;
import android.support.v7.app.ActionBarActivity;
import android.support.v7.app.AppCompatActivity;
import android.view.MotionEvent;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.ImageView;

public class Splashscreen extends Activity {
    Thread splashThread;
    ImageView logo, logo_text;
    ImageView img1, img2, img3, img4, img5, img6, img7, img8, img9, img10, img11, img12, img13, img14, img15,
            img16, img17, img17a, img18, img19, img20, img21, img22, img23, img24, img25, img26, img27, img28, img29,
            img30, img31, img32, img33, img34, img35, img36, img37, img38;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_splashscreen);
        logo_text = (ImageView) findViewById(R.id.img_logo_text);
        final Animation runAnimation = AnimationUtils.loadAnimation(this,
                R.anim.text_anim);

        logo_text.startAnimation(runAnimation);
        logo_text.setVisibility(View.VISIBLE);

        img1 = (ImageView) findViewById(R.id.imge1);
        img2 = (ImageView) findViewById(R.id.imge2);
        img3 = (ImageView) findViewById(R.id.imge3);
        img4 = (ImageView) findViewById(R.id.imge4);
        img5 = (ImageView) findViewById(R.id.imge5);
        img6 = (ImageView) findViewById(R.id.imge6);
        img7 = (ImageView) findViewById(R.id.imge7);
        img8 = (ImageView) findViewById(R.id.imge8);
        img9 = (ImageView) findViewById(R.id.imge9);
        img10 = (ImageView) findViewById(R.id.imge10);
        img11 = (ImageView) findViewById(R.id.imge11);
        img12 = (ImageView) findViewById(R.id.imge12);
        img13 = (ImageView) findViewById(R.id.imge13);
        img14 = (ImageView) findViewById(R.id.imge14);
        img15 = (ImageView) findViewById(R.id.imge15);
        img16 = (ImageView) findViewById(R.id.imge16);
        img17 = (ImageView) findViewById(R.id.imge17);
        img17a = (ImageView) findViewById(R.id.imge17a);
        img18 = (ImageView) findViewById(R.id.imge18);
        img19 = (ImageView) findViewById(R.id.imge19);
        img20 = (ImageView) findViewById(R.id.imge20);
        img21 = (ImageView) findViewById(R.id.imge21);
        img22 = (ImageView) findViewById(R.id.imge22);
        img23 = (ImageView) findViewById(R.id.imge23);
        img24 = (ImageView) findViewById(R.id.imge24);
        img25 = (ImageView) findViewById(R.id.imge25);
        img26 = (ImageView) findViewById(R.id.imge26);
        img27 = (ImageView) findViewById(R.id.imge27);
        img28 = (ImageView) findViewById(R.id.imge28);
        img29 = (ImageView) findViewById(R.id.imge29);
        img30 = (ImageView) findViewById(R.id.imge30);
        img31 = (ImageView) findViewById(R.id.imge31);
        img32 = (ImageView) findViewById(R.id.imge32);
        img33 = (ImageView) findViewById(R.id.imge33);
        img34 = (ImageView) findViewById(R.id.imge34);
        img35 = (ImageView) findViewById(R.id.imge35);
        img36 = (ImageView) findViewById(R.id.imge36);
        img37 = (ImageView) findViewById(R.id.imge37);
        img38 = (ImageView) findViewById(R.id.imge38);


        final Animation img1A = AnimationUtils.loadAnimation(this,
                R.anim.fade_in);
        final Animation img2A = AnimationUtils.loadAnimation(this,
                R.anim.fade_in);
        final Animation img3A = AnimationUtils.loadAnimation(this,
                R.anim.fade_in);
        final Animation img4A = AnimationUtils.loadAnimation(this,
                R.anim.fade_in);
        final Animation img5A = AnimationUtils.loadAnimation(this,
                R.anim.fade_in);
        final Animation img6A = AnimationUtils.loadAnimation(this,
                R.anim.fade_in);
        final Animation img7A = AnimationUtils.loadAnimation(this,
                R.anim.fade_in);
        final Animation img8A = AnimationUtils.loadAnimation(this,
                R.anim.fade_in);
        final Animation img9A = AnimationUtils.loadAnimation(this,
                R.anim.fade_in);
        final Animation img10A = AnimationUtils.loadAnimation(this,
                R.anim.fade_in);
        final Animation img11A = AnimationUtils.loadAnimation(this,
                R.anim.fade_in);
        final Animation img12A = AnimationUtils.loadAnimation(this,
                R.anim.fade_in);
        final Animation img13A = AnimationUtils.loadAnimation(this,
                R.anim.fade_in);
        final Animation img14A = AnimationUtils.loadAnimation(this,
                R.anim.fade_in);
        final Animation img15A = AnimationUtils.loadAnimation(this,
                R.anim.fade_in);
        final Animation img16A = AnimationUtils.loadAnimation(this,
                R.anim.fade_in);
        final Animation img17A = AnimationUtils.loadAnimation(this,
                R.anim.fade_in);
        final Animation img17AA = AnimationUtils.loadAnimation(this,
                R.anim.fade_in);
        final Animation img18A = AnimationUtils.loadAnimation(this,
                R.anim.fade_in);
        final Animation img19A = AnimationUtils.loadAnimation(this,
                R.anim.fade_in);
        final Animation img20A = AnimationUtils.loadAnimation(this,
                R.anim.fade_in);
        final Animation img21A = AnimationUtils.loadAnimation(this,
                R.anim.fade_in);
        final Animation img22A = AnimationUtils.loadAnimation(this,
                R.anim.fade_in);
        final Animation img23A = AnimationUtils.loadAnimation(this,
                R.anim.fade_in);
        final Animation img24A = AnimationUtils.loadAnimation(this,
                R.anim.fade_in);
        final Animation img25A = AnimationUtils.loadAnimation(this,
                R.anim.fade_in);
        final Animation img26A = AnimationUtils.loadAnimation(this,
                R.anim.fade_in);
        final Animation img27A = AnimationUtils.loadAnimation(this,
                R.anim.fade_in);
        final Animation img28A = AnimationUtils.loadAnimation(this,
                R.anim.fade_in);
        final Animation img29A = AnimationUtils.loadAnimation(this,
                R.anim.fade_in);
        final Animation img30A = AnimationUtils.loadAnimation(this,
                R.anim.fade_in);
        final Animation img31A = AnimationUtils.loadAnimation(this,
                R.anim.fade_in);
        final Animation img32A = AnimationUtils.loadAnimation(this,
                R.anim.fade_in);
        final Animation img33A = AnimationUtils.loadAnimation(this,
                R.anim.fade_in);
        final Animation img34A = AnimationUtils.loadAnimation(this,
                R.anim.fade_in);
        final Animation img35A = AnimationUtils.loadAnimation(this,
                R.anim.fade_in);
        final Animation img36A = AnimationUtils.loadAnimation(this,
                R.anim.fade_in);
        final Animation img37A = AnimationUtils.loadAnimation(this,
                R.anim.fade_in);
        final Animation img38A = AnimationUtils.loadAnimation(this,
                R.anim.fade_in);


        runAnimation.setAnimationListener(new Animation.AnimationListener() {

            @Override
            public void onAnimationStart(Animation animation) {
                img1.startAnimation(img1A);
                img1.setVisibility(View.VISIBLE);
                img11.startAnimation(img11A);
                img11.setVisibility(View.VISIBLE);
            }

            @Override
            public void onAnimationEnd(Animation animation) {

            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }

        });
        img11A.setAnimationListener(new Animation.AnimationListener() {

            @Override
            public void onAnimationStart(Animation animation) {
            }

            @Override
            public void onAnimationEnd(Animation animation) {
                img10.startAnimation(img10A);
                img10.setVisibility(View.VISIBLE);

            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }

        });
        img10A.setAnimationListener(new Animation.AnimationListener() {

            @Override
            public void onAnimationStart(Animation animation) {
            }

            @Override
            public void onAnimationEnd(Animation animation) {
                img9.startAnimation(img9A);
                img9.setVisibility(View.VISIBLE);

            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }

        });
        img9A.setAnimationListener(new Animation.AnimationListener() {

            @Override
            public void onAnimationStart(Animation animation) {
            }

            @Override
            public void onAnimationEnd(Animation animation) {
                img8.startAnimation(img8A);
                img8.setVisibility(View.VISIBLE);

            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }

        });
        img8A.setAnimationListener(new Animation.AnimationListener() {

            @Override
            public void onAnimationStart(Animation animation) {
            }

            @Override
            public void onAnimationEnd(Animation animation) {
                img7.startAnimation(img7A);
                img7.setVisibility(View.VISIBLE);

            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }

        });

        img7A.setAnimationListener(new Animation.AnimationListener() {

            @Override
            public void onAnimationStart(Animation animation) {
            }

            @Override
            public void onAnimationEnd(Animation animation) {
                img30.startAnimation(img30A);
                img30.setVisibility(View.VISIBLE);

            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }

        });


        img1A.setAnimationListener(new Animation.AnimationListener() {

            @Override
            public void onAnimationStart(Animation animation) {
            }

            @Override
            public void onAnimationEnd(Animation animation) {
                img2.startAnimation(img2A);
                img2.setVisibility(View.VISIBLE);

            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }

        });
        img2A.setAnimationListener(new Animation.AnimationListener() {

            @Override
            public void onAnimationStart(Animation animation) {

            }

            @Override
            public void onAnimationEnd(Animation animation) {
                img3.startAnimation(img3A);
                img3.setVisibility(View.VISIBLE);
            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }

        });
        img3A.setAnimationListener(new Animation.AnimationListener() {

            @Override
            public void onAnimationStart(Animation animation) {

            }

            @Override
            public void onAnimationEnd(Animation animation) {
                img4.startAnimation(img4A);
                img4.setVisibility(View.VISIBLE);
            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }

        });
        img4A.setAnimationListener(new Animation.AnimationListener() {

            @Override
            public void onAnimationStart(Animation animation) {

            }

            @Override
            public void onAnimationEnd(Animation animation) {
                img5.startAnimation(img5A);
                img5.setVisibility(View.VISIBLE);
            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }

        });
        img5A.setAnimationListener(new Animation.AnimationListener() {

            @Override
            public void onAnimationStart(Animation animation) {

            }

            @Override
            public void onAnimationEnd(Animation animation) {
                img6.startAnimation(img6A);
                img6.setVisibility(View.VISIBLE);
            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }

        });
        img6A.setAnimationListener(new Animation.AnimationListener() {

            @Override
            public void onAnimationStart(Animation animation) {

            }

            @Override
            public void onAnimationEnd(Animation animation) {
                img18.startAnimation(img18A);
                img18.setVisibility(View.VISIBLE);
            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }

        });
        img18A.setAnimationListener(new Animation.AnimationListener() {

            @Override
            public void onAnimationStart(Animation animation) {

            }

            @Override
            public void onAnimationEnd(Animation animation) {
                img17.startAnimation(img17A);
                img17.setVisibility(View.VISIBLE);
            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }

        });
        img17A.setAnimationListener(new Animation.AnimationListener() {

            @Override
            public void onAnimationStart(Animation animation) {

            }

            @Override
            public void onAnimationEnd(Animation animation) {
                img17a.startAnimation(img17AA);
                img17a.setVisibility(View.VISIBLE);
            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }

        });
        img17AA.setAnimationListener(new Animation.AnimationListener() {

            @Override
            public void onAnimationStart(Animation animation) {

            }

            @Override
            public void onAnimationEnd(Animation animation) {
                img16.startAnimation(img16A);
                img16.setVisibility(View.VISIBLE);
            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }

        });
        img16A.setAnimationListener(new Animation.AnimationListener() {

            @Override
            public void onAnimationStart(Animation animation) {

            }

            @Override
            public void onAnimationEnd(Animation animation) {
                img15.startAnimation(img15A);
                img15.setVisibility(View.VISIBLE);
            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }

        });
        img15A.setAnimationListener(new Animation.AnimationListener() {

            @Override
            public void onAnimationStart(Animation animation) {

            }

            @Override
            public void onAnimationEnd(Animation animation) {
                img14.startAnimation(img14A);
                img14.setVisibility(View.VISIBLE);
            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }

        });
        img14A.setAnimationListener(new Animation.AnimationListener() {

            @Override
            public void onAnimationStart(Animation animation) {

            }

            @Override
            public void onAnimationEnd(Animation animation) {
                img13.startAnimation(img13A);
                img13.setVisibility(View.VISIBLE);
            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }

        });
        img13A.setAnimationListener(new Animation.AnimationListener() {

            @Override
            public void onAnimationStart(Animation animation) {

            }

            @Override
            public void onAnimationEnd(Animation animation) {
                img12.startAnimation(img12A);
                img12.setVisibility(View.VISIBLE);
            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }

        });
        img12A.setAnimationListener(new Animation.AnimationListener() {

            @Override
            public void onAnimationStart(Animation animation) {

            }

            @Override
            public void onAnimationEnd(Animation animation) {
                img21.startAnimation(img21A);
                img21.setVisibility(View.VISIBLE);
            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }

        });

        img21A.setAnimationListener(new Animation.AnimationListener() {

            @Override
            public void onAnimationStart(Animation animation) {
            }

            @Override
            public void onAnimationEnd(Animation animation) {
                img20.startAnimation(img20A);
                img20.setVisibility(View.VISIBLE);

            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }

        });
        img20A.setAnimationListener(new Animation.AnimationListener() {

            @Override
            public void onAnimationStart(Animation animation) {
            }

            @Override
            public void onAnimationEnd(Animation animation) {
                img19.startAnimation(img19A);
                img19.setVisibility(View.VISIBLE);

            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }

        });
        img19A.setAnimationListener(new Animation.AnimationListener() {

            @Override
            public void onAnimationStart(Animation animation) {
            }

            @Override
            public void onAnimationEnd(Animation animation) {
                img22.startAnimation(img22A);
                img22.setVisibility(View.VISIBLE);

            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }

        });
        img22A.setAnimationListener(new Animation.AnimationListener() {

            @Override
            public void onAnimationStart(Animation animation) {
            }

            @Override
            public void onAnimationEnd(Animation animation) {

//                img30.startAnimation(img30A);
//                img30.setVisibility(View.VISIBLE);
            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }

        });
        img30A.setAnimationListener(new Animation.AnimationListener() {

            @Override
            public void onAnimationStart(Animation animation) {
            }

            @Override
            public void onAnimationEnd(Animation animation) {
                img29.startAnimation(img29A);
                img29.setVisibility(View.VISIBLE);

            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }

        });
        img29A.setAnimationListener(new Animation.AnimationListener() {

            @Override
            public void onAnimationStart(Animation animation) {
            }

            @Override
            public void onAnimationEnd(Animation animation) {
                img28.startAnimation(img28A);
                img28.setVisibility(View.VISIBLE);

            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }

        });
        img28A.setAnimationListener(new Animation.AnimationListener() {

            @Override
            public void onAnimationStart(Animation animation) {
            }

            @Override
            public void onAnimationEnd(Animation animation) {
                img27.startAnimation(img27A);
                img27.setVisibility(View.VISIBLE);

            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }

        });
        img27A.setAnimationListener(new Animation.AnimationListener() {

            @Override
            public void onAnimationStart(Animation animation) {
            }

            @Override
            public void onAnimationEnd(Animation animation) {
                img26.startAnimation(img26A);
                img26.setVisibility(View.VISIBLE);

            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }

        });
        img26A.setAnimationListener(new Animation.AnimationListener() {

            @Override
            public void onAnimationStart(Animation animation) {
            }

            @Override
            public void onAnimationEnd(Animation animation) {
                img25.startAnimation(img25A);
                img25.setVisibility(View.VISIBLE);

            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }

        });
        img25A.setAnimationListener(new Animation.AnimationListener() {

            @Override
            public void onAnimationStart(Animation animation) {
            }

            @Override
            public void onAnimationEnd(Animation animation) {
                img24.startAnimation(img24A);
                img24.setVisibility(View.VISIBLE);

            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }

        });
        img24A.setAnimationListener(new Animation.AnimationListener() {

            @Override
            public void onAnimationStart(Animation animation) {
            }

            @Override
            public void onAnimationEnd(Animation animation) {
                img23.startAnimation(img23A);
                img23.setVisibility(View.VISIBLE);

            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }

        });

        img23A.setAnimationListener(new Animation.AnimationListener() {

            @Override
            public void onAnimationStart(Animation animation) {
            }

            @Override
            public void onAnimationEnd(Animation animation) {
                img31.startAnimation(img31A);
                img31.setVisibility(View.VISIBLE);
                img32.setVisibility(View.VISIBLE);
                img33.setVisibility(View.VISIBLE);
                img34.setVisibility(View.VISIBLE);
                img35.setVisibility(View.VISIBLE);
                img36.setVisibility(View.VISIBLE);
                img37.setVisibility(View.VISIBLE);
                img38.setVisibility(View.VISIBLE);

            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }

        });

        splashThread = new Thread() {
            @Override
            public void run() {
                try {
                    synchronized (this) {
                        wait(2000);
                    }
                } catch (InterruptedException ex) {
                }
                finish();

                Intent intent = new Intent(Splashscreen.this,
                        BleScannerScreen.class);
                startActivity(intent);


            }
        };
        splashThread.start();
    }


    public boolean onTouchEvent(MotionEvent evt) {
        if (evt.getAction() == MotionEvent.ACTION_DOWN) {
            synchronized (splashThread) {
                splashThread.notifyAll();
            }
        }
        return true;
    }

    @Override
    protected void onResume() {
        super.onResume();

    }
}
