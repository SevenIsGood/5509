/**************************************************************************************************
 Filename:       C5545Equalizer.java

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

import android.app.Dialog;
import android.app.ProgressDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.PowerManager;
import android.projects.com.c5545ble.R;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

public class C5545Equalizer extends ViewPagerActivity {

    public static final String EXTRAS_DEVICE_NAME = "DEVICE_NAME";
    public static final String EXTRAS_DEVICE_ADDRESS = "DEVICE_ADDRESS";

    private final int CONNECT_TIMEOUT = 15; // Seconds
    private final String LIST_NAME = "NAME";
    private final String LIST_UUID = "UUID";
    BluetoothGattCharacteristic writeCharacteristic;
    BluetoothGattCharacteristic readCharacteristic;
    BluetoothGattCharacteristic notifyCharacteristic;
    int[] equilizerData = new int[5];
    CustomTimer mConnectTimer = null;
    int EQUALIZER_SERVICE_POS = 3;
    int EQUALIZER_ZERO_POS = 0;
    int EQUALIZER_ONE_POS = 1;
    int EQUALIZER_TWO_POS = 2;
    int EQUALIZER_THREE_POS = 3;
    int EQUALIZER_FOUR_POS = 4;

    int EQUALIZER_AUDIO_STATUS_POS = 5;
    int audioNotfctnData = 0;
    private EqualizerView equalizerView;
    private HelpView hw;
    private PowerManager.WakeLock wl;
    private C5545AppClass objAppContext;
    private BluetoothLeService mBluetoothLeService;
    private String devAddres;
    private ProgressDialog mProgress;
    private BluetoothManager mBluetoothManager;
    private BluetoothAdapter mBluetoothAdapter;
    private IntentFilter mFilter;
    private ArrayList<ArrayList<BluetoothGattCharacteristic>> mGattCharacteristics = new ArrayList<ArrayList<BluetoothGattCharacteristic>>();
    // Listener for progress timer expiration
    private CustomTimerCallback mConnectCallback = new CustomTimerCallback() {
        public void onTimeout() {
            runOnUiThread(new Runnable() {
                public void run() {
                    Toast.makeText(getApplicationContext(), R.string.ble_connection_fail, Toast.LENGTH_SHORT)
                            .show();
                }
            });
            finish();
        }
        public void onTick(int i) {

        }
    };
    private BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            Log.d(C5545AppClass.TAG, "GATT Action #######" + action.toString());

            if (BluetoothLeService.ACTION_GATT_CONNECTED.equals(action)) {
                // Register for notification update
                if (mConnectTimer != null) {
                    mConnectTimer.stop();
                    mConnectTimer = null;
                }
            } else if (BluetoothLeService.ACTION_GATT_DISCONNECTED.equals(action)) {
                // GATT disconnect
                Toast.makeText(context, R.string.eqlizr_closing, Toast.LENGTH_LONG)
                        .show();
                enableNotification(EQUALIZER_SERVICE_POS, EQUALIZER_AUDIO_STATUS_POS, false);

                finish();
            } else if (BluetoothLeService.ACTION_GATT_SERVICES_DISCOVERED
                    .equals(action)) {
                displayGattServices(mBluetoothLeService
                        .getSupportedGattServices());

                new ReadEqlzrDataTask().execute();

            } else if (BluetoothLeService.ACTION_DATA_AVAILABLE.equals(action)) {
                byte[] availData = intent.getExtras().getByteArray(
                        BluetoothLeService.EXTRA_DATA);
                Byte b = availData[0]; // Boxing conversion converts `byte` to `Byte`
                equalizerView.setPlayStausNotificatiion(b.intValue());
            } else if (BluetoothLeService.ACTION_WRITE_COMPLETE.equals(action)) {

            } else if (BluetoothLeService.ACTION_WRITE_FAILED.equals(action)) {

            }

        }
    };

    public C5545Equalizer() {
        mResourceFragmentPager = R.layout.fragment_pager;
        mResourceIdPager = R.id.pager;
    }

    static public String ByteArrayToHexString(byte[] inarray) {
        int i, j, in;
        String[] hex = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A",
                "B", "C", "D", "E", "F"};
        String out = "";

        for (j = 0; j < inarray.length; ++j) {
            in = (int) inarray[j] & 0xff;
            i = (in >> 4) & 0x0f;
            out += hex[i];
            i = in & 0x0f;
            out += hex[i];
        }
        return out;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        mBluetoothManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
        mBluetoothAdapter = mBluetoothManager.getAdapter();
        if (mBluetoothAdapter == null) {
            Toast.makeText(this, R.string.ble_not_supported,
                    Toast.LENGTH_SHORT).show();
            finish();
            return;
        }

        equalizerView = new EqualizerView();
        mSectionsPagerAdapter.addSection(equalizerView, "Music Equalizer");
        hw = new HelpView();
        hw.setParameters("help_equalizer.html", R.layout.fragment_help, R.id.webpage);
        mSectionsPagerAdapter.addSection(hw, "Help");

        // Get Application ojbect and BLE Service
        objAppContext = (C5545AppClass) getApplicationContext();
        mBluetoothLeService = objAppContext.getBLEService();
        devAddres = getIntent().getExtras().getString(EXTRAS_DEVICE_ADDRESS);
        // if bluetooth service is not connected the initialize the ble device.
        if (mBluetoothLeService.getConnectionStatus() == BluetoothLeService.STATE_DISCONNECTED) {

            mProgress = new ProgressDialog(this);
            mProgress.setMessage(getResources().getString(R.string.progress_init));
            mProgress.setCanceledOnTouchOutside(false);
            mProgress.setCancelable(false);
            mProgress.setProgressStyle(ProgressDialog.STYLE_SPINNER);
            mProgress.show();

            mBluetoothLeService.initialize();
            mProgress.setMessage(getResources().getString(R.string.progress_connect));
            if (mBluetoothLeService.connect(devAddres)) {
                mProgress.setMessage(getResources().getString(R.string.progress_connected));
                mConnectTimer = new CustomTimer(null, CONNECT_TIMEOUT, mConnectCallback);
            } else {
                Toast.makeText(this, R.string.ble_connection_fail + devAddres, Toast.LENGTH_SHORT).show();
                finish();
            }
        }

        // Get Wakeup lock instance
        PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
        wl = pm.newWakeLock(PowerManager.FULL_WAKE_LOCK, "DoNotDimScreen");

        // Register the BroadcastReceiver
        mFilter = new IntentFilter();
        mFilter.addAction(BluetoothLeService.ACTION_GATT_CONNECTED);
        mFilter.addAction(BluetoothLeService.ACTION_GATT_DISCONNECTED);
        mFilter.addAction(BluetoothLeService.ACTION_GATT_SERVICES_DISCOVERED);
        mFilter.addAction(BluetoothLeService.ACTION_DATA_AVAILABLE);
        mFilter.addAction(BluetoothLeService.ACTION_WRITE_COMPLETE);
        mFilter.addAction(BluetoothLeService.ACTION_WRITE_FAILED);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        if (mBluetoothLeService != null) {
            mBluetoothLeService.disconnect();
        }
        if (null != mProgress) {
            mProgress.dismiss();
        }

    }

    @Override
    protected void onStart() {
        super.onStart();
        if (!mBluetoothAdapter.isEnabled() || mBluetoothLeService == null) {
            Toast.makeText(getApplicationContext(), R.string.eqlizr_closing, Toast.LENGTH_LONG)
                    .show();
            finish();
        }
        registerReceiver(mReceiver, mFilter);
    }

    @Override
    protected void onStop() {
        super.onStop();

        unregisterReceiver(mReceiver);
    }

    @Override
    protected void onResume() {
        super.onResume();
        wl.acquire();
    }

    @Override
    protected void onPause() {
        super.onPause();
        wl.release();
    }
    private void onAbout() {
        final Dialog dialog = new AboutDialog(this);
        dialog.show();
    }
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_equalizer, menu);
        return super.onCreateOptionsMenu(menu);
    }
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {

        switch (item.getItemId()) {
            case R.id.opt_about:
                onAbout();
                break;
            case R.id.opt_exit:
                Toast.makeText(this, R.string.exit, Toast.LENGTH_SHORT).show();
                finish();
                break;
            default:
                return super.onOptionsItemSelected(item);
        }
        return true;
    }

    // Iterate through the supported GATT services/characteristics and store it in a datastructure
    private void displayGattServices(List<BluetoothGattService> gattServices) {
        if (gattServices == null)
            return;
        String unknownCharaString = "Start Demo";
        String unknownServiceString = "";
        String uuid = null;
        ArrayList<HashMap<String, String>> gattServiceData = new ArrayList<HashMap<String, String>>();
        ArrayList<ArrayList<HashMap<String, String>>> gattCharacteristicData = new ArrayList<ArrayList<HashMap<String, String>>>();
        mGattCharacteristics = new ArrayList<ArrayList<BluetoothGattCharacteristic>>();

        int serPos = 0, charPos = 0;
        // Loops through available GATT Services.
        for (BluetoothGattService gattService : gattServices) {

            HashMap<String, String> currentServiceData = new HashMap<String, String>();
            uuid = gattService.getUuid().toString();
            currentServiceData.put(LIST_NAME,
                    SampleGattAttributes.lookup(uuid, unknownServiceString));
            currentServiceData.put(LIST_UUID, uuid);
            gattServiceData.add(currentServiceData);

            ArrayList<HashMap<String, String>> gattCharacteristicGroupData = new ArrayList<HashMap<String, String>>();
            List<BluetoothGattCharacteristic> gattCharacteristics = gattService
                    .getCharacteristics();
            ArrayList<BluetoothGattCharacteristic> charas = new ArrayList<BluetoothGattCharacteristic>();

            // Loops through available Characteristics.
            for (BluetoothGattCharacteristic gattCharacteristic : gattCharacteristics) {
                charas.add(gattCharacteristic);
                HashMap<String, String> currentCharaData = new HashMap<String, String>();
                uuid = gattCharacteristic.getUuid().toString();
                currentCharaData.put(LIST_NAME,
                        SampleGattAttributes.lookup(uuid, unknownCharaString));
                currentCharaData.put(LIST_UUID, uuid);
                gattCharacteristicGroupData.add(currentCharaData);
                charPos++;
            }
            serPos++;
            charPos = 0;
            mGattCharacteristics.add(charas);
            gattCharacteristicData.add(gattCharacteristicGroupData);

        }
    }

    boolean setEqualizer(int charecteristicPos, int value) {
        int charPos = 0;
        byte[] packetData = new byte[1];
        if (value > 15 || value < -15) {
            return false;
        }
        packetData[0] = (byte) value;

        switch (charecteristicPos) {
            case 0:
                charPos = EQUALIZER_ZERO_POS;
                break;
            case 1:
                charPos = EQUALIZER_ONE_POS;
                break;
            case 2:
                charPos = EQUALIZER_TWO_POS;
                break;
            case 3:
                charPos = EQUALIZER_THREE_POS;
                break;
            case 4:
                charPos = EQUALIZER_FOUR_POS;
                break;
            default:
                return false;
        }

        if (write(EQUALIZER_SERVICE_POS, charPos, packetData))
            return true;
        else
            return false;
    }

    boolean readEqualizerData(int equilizerData[]) {
        int charPos = 0;
        byte[] rcvdData = new byte[1];
        rcvdData[0] = 0;
        for (int i = 0; i <= 5; i++) {
            rcvdData[0] = 0;
            switch (i) {
                case 0:
                    charPos = EQUALIZER_ZERO_POS;
                    break;
                case 1:
                    charPos = EQUALIZER_ONE_POS;
                    break;
                case 2:
                    charPos = EQUALIZER_TWO_POS;
                    break;
                case 3:
                    charPos = EQUALIZER_THREE_POS;
                    break;
                case 4:
                    charPos = EQUALIZER_FOUR_POS;
                    break;
                case 5:
                    charPos = EQUALIZER_AUDIO_STATUS_POS;
                    break;
            }
            if (read(EQUALIZER_SERVICE_POS, charPos, rcvdData)) {
                if (charPos == 5) {
                    audioNotfctnData = rcvdData[0];
                    runOnUiThread(new Runnable() {
                        public void run() {
                            equalizerView.setPlayStausNotificatiion(audioNotfctnData);
                        }
                    });

                } else {
                    equilizerData[i] = rcvdData[0];
                }
            } else {
                return false;
            }
        }
        return true;
    }

    public boolean write(int ServicePos, int CharecteristicPos, byte data[]) {
        // Invoke the write of BLE / UART.
        if (mBluetoothLeService != null
                && mBluetoothLeService.getConnectionStatus() == BluetoothLeService.STATE_CONNECTED) {

            if (mGattCharacteristics != null) {

                writeCharacteristic = mGattCharacteristics.get(ServicePos)
                        .get(CharecteristicPos);
                writeCharacteristic.setValue(data);
                final int charaProp = writeCharacteristic.getProperties();

                if ((charaProp | BluetoothGattCharacteristic.PROPERTY_WRITE) > 0) {

                    mBluetoothLeService
                            .writeCharacteristic(writeCharacteristic);
                    try {
                        synchronized (writeCharacteristic) {
                            writeCharacteristic.wait(10);
                        }
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                        return false;
                    }
                    return true;
                }
            }
        }
        return false;
    }

    public boolean enableNotification(int servicePos, int charecteristicPos, boolean status) {
        if (mGattCharacteristics != null
                && mBluetoothLeService.getConnectionStatus() == BluetoothLeService.STATE_CONNECTED) {
            if (servicePos <= mGattCharacteristics.size() && charecteristicPos <= mGattCharacteristics.get(servicePos).size()) {

                notifyCharacteristic = mGattCharacteristics.get(servicePos)
                        .get(charecteristicPos);

                final int charaProp = notifyCharacteristic.getProperties();
                if ((charaProp & BluetoothGattCharacteristic.PROPERTY_NOTIFY) > 0) {
                    mBluetoothLeService.setCharacteristicNotification(
                            notifyCharacteristic, status);
                    return true;
                } else {
                    Log.e(C5545AppClass.TAG, "characteristic " + notifyCharacteristic.getUuid() + " doesn't have notify properties");
                    return false;
                }

            } else {
                return false;
            }
        }
        return false;
    }


    public boolean read(int ServicePos, int CharecteristicPos, byte[] data) {
        // Invoke the write of BLE and then read of BLE to wait for the data.
        if (mBluetoothLeService != null
                && mBluetoothLeService.getConnectionStatus() == BluetoothLeService.STATE_CONNECTED) {

            if (mGattCharacteristics != null) {

                readCharacteristic = mGattCharacteristics.get(ServicePos)
                        .get(CharecteristicPos);

                if (readCharacteristic == null) {
                    return false;
                }

                final int charaProp = readCharacteristic.getProperties();

                if ((charaProp | BluetoothGattCharacteristic.PROPERTY_READ) > 0) {

                    try {
                        synchronized (readCharacteristic) {
                            mBluetoothLeService.readCharacteristic(readCharacteristic);

                            readCharacteristic.wait(1000);
                            byte[] dataRcvd = readCharacteristic.getValue();

                            if (dataRcvd != null && dataRcvd.length > 0) {
                                data[0] = dataRcvd[0];
                            } else {
                                return false;
                            }
                        }

                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }

        } else
            Log.i(C5545AppClass.TAG,
                    "");
        return true;
    }

    class ReadEqlzrDataTask extends AsyncTask<Void, Void, Boolean> {
        @Override
        protected void onPreExecute() {
            super.onPreExecute();
            if (mProgress != null && mProgress.isShowing()) {
                mProgress.setMessage(getResources().getString(R.string.progress_read_eqlzr));
            }
        }

        @Override
        protected Boolean doInBackground(Void... params) {
            boolean readSuccess = readEqualizerData(equilizerData);
            return readSuccess;
        }

        @Override
        protected void onPostExecute(Boolean result) {
            if (result) {
                equalizerView.updateSliders(equilizerData);
            } else {
                Toast.makeText(getApplicationContext(), R.string.progress_read_eqlizer_fail, Toast.LENGTH_LONG).show();
                finish();
            }
            if (mProgress != null && mProgress.isShowing()) {
                mProgress.hide();
            }
            if (!enableNotification(EQUALIZER_SERVICE_POS, EQUALIZER_AUDIO_STATUS_POS, true)) {
                Toast.makeText(getApplicationContext(), R.string.equalizer_error_status, Toast.LENGTH_LONG).show();
                finish();
            }
        }
    }
}
