/**************************************************************************************************
 * Filename:       BleScannerScreen.java
 * Revised:        $Date: Wed Apr 22 13:01:34 2015 +0200$
 * Revision:       $Revision: 599e5650a33a4a142d060c959561f9e9b0d88146$
 * <p/>
 * Copyright (c) 2013 - 2014 Texas Instruments Incorporated
 * <p/>
 * All rights reserved not granted herein.
 * Limited License.
 * <p/>
 * Texas Instruments Incorporated grants a world-wide, royalty-free,
 * non-exclusive license under copyrights and patents it now or hereafter
 * owns or controls to make, have made, use, import, offer to sell and sell ("Utilize")
 * this software subject to the terms herein.  With respect to the foregoing patent
 * license, such license is granted  solely to the extent that any such patent is necessary
 * to Utilize the software alone.  The patent license shall not apply to any combinations which
 * include this software, other than combinations with devices manufactured by or for TI ('TI Devices').
 * No hardware patent is licensed hereunder.
 * <p/>
 * Redistributions must preserve existing copyright notices and reproduce this license (including the
 * above copyright notice and the disclaimer and (if applicable) source code license limitations below)
 * in the documentation and/or other materials provided with the distribution
 * <p/>
 * Redistribution and use in binary form, without modification, are permitted provided that the following
 * conditions are met:
 * <p/>
 * No reverse engineering, decompilation, or disassembly of this software is permitted with respect to any
 * software provided in binary form.
 * any redistribution and use are licensed by TI for use only with TI Devices.
 * Nothing shall obligate TI to provide you with source code for the software licensed and provided to you in object code.
 * <p/>
 * If software source code is provided to you, modification and redistribution of the source code are permitted
 * provided that the following conditions are met:
 * <p/>
 * any redistribution and use of the source code, including any resulting derivative works, are licensed by
 * TI for use only with TI Devices.
 * any redistribution and use of any object code compiled from the source code and any resulting derivative
 * works, are licensed by TI for use only with TI Devices.
 * <p/>
 * Neither the name of Texas Instruments Incorporated nor the names of its suppliers may be used to endorse or
 * promote products derived from this software without specific prior written permission.
 * <p/>
 * DISCLAIMER.
 * <p/>
 * THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 **************************************************************************************************/
package android.projects.com.c5545bpdemo;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.os.Bundle;
import android.projects.com.c5545ble.R;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import java.util.List;

public class ScanView extends Fragment {
    private final int SCAN_TIMEOUT = 15; // Seconds
    private BleScannerScreen mActivity = null;

    private DeviceListAdapter mDeviceAdapter = null;
    private TextView mEmptyMsg;
    private Button mBtnScan = null;
    private ListView mDeviceListView = null;
    private boolean mBusy;

    private CustomTimer mScanTimer = null;
    @SuppressWarnings("unused")
    private CustomTimer mStatusTimer;

    @Override
    public void onAttach(Activity activity) {
        super.onAttach(activity);
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {

        // The last two arguments ensure LayoutParams are inflated properly.
        View view = inflater.inflate(R.layout.fragment_scan, container, false);
        mActivity = (BleScannerScreen) getActivity();

        // Initialize widgets
        mBtnScan = (Button) view.findViewById(R.id.btn_scan);
        mDeviceListView = (ListView) view.findViewById(R.id.device_list);
        mDeviceListView.setClickable(true);
        mDeviceListView.setOnItemClickListener(mDeviceClickListener);
        mEmptyMsg = (TextView) view.findViewById(R.id.no_device);
        mBusy = false;

        // Alert parent activity
        mActivity.onScanViewReady();
        return view;
    }

    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
    }

    @Override
    public void onStart() {
        super.onStart();
    }

    @Override
    public void onResume() {
        super.onResume();
    }

    @Override
    public void onPause() {
        super.onPause();
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
    }

    @Override
    public void onDetach() {
        super.onDetach();
    }

    void setScanBtnStatus(boolean status) {
        if (mBtnScan != null)
            mBtnScan.setEnabled(status);
    }

    void setDisplayStatus(boolean context, boolean isScanning) {

        if (context) {
            if (isScanning) {
                mEmptyMsg.setText(R.string.bt_ble_on_scan);
            } else {
                mEmptyMsg.setText(R.string.bt_ble_on);
            }
        } else {
            mEmptyMsg.setText(R.string.bt_ble_off);
        }

    }

    void setError() {
        setBusy(false);
        stopTimers();
    }

    void notifyDataSetChanged() {
        List<BleDeviceInfo> deviceList = mActivity.getDeviceInfoList();
        if (mDeviceAdapter == null) {
            mDeviceAdapter = new DeviceListAdapter(mActivity, deviceList);
        }
        mDeviceListView.setAdapter(mDeviceAdapter);
        mDeviceAdapter.notifyDataSetChanged();
        if (deviceList.size() > 0) {
            mEmptyMsg.setVisibility(View.GONE);
        } else {
            mEmptyMsg.setVisibility(View.VISIBLE);
        }
    }

    void setBusy(boolean f) {
        if (f != mBusy) {
            mBusy = f;
            if (!mBusy) {
                stopTimers();
                mBtnScan.setEnabled(true);    // Enable in case of connection timeout
                mDeviceAdapter.notifyDataSetChanged(); // Force enabling of all Connect buttons
            }
            mActivity.showBusyIndicator(f);
        }
    }

    void updateGui(boolean scanning) {
        if (mBtnScan == null || mDeviceAdapter == null)
            return; // UI not ready

        setBusy(scanning);

        if (scanning) {
            // Indicate that scanning has started
            mScanTimer = new CustomTimer(null, SCAN_TIMEOUT, mPgScanCallback);
            mBtnScan.setText("Stop");
            mEmptyMsg.setVisibility(View.VISIBLE);
            mEmptyMsg.setText(R.string.bt_ble_on_scan);
            mActivity.updateGuiState();
        } else {
            mBtnScan.setText("Scan");
            mEmptyMsg.setText(R.string.bt_ble_nodevice);
            mActivity.setProgressBarIndeterminateVisibility(false);
            mDeviceAdapter.notifyDataSetChanged();
        }
    }

    // Listener for device list
    private OnItemClickListener mDeviceClickListener = new OnItemClickListener() {
        public void onItemClick(AdapterView<?> parent, View view, int pos, long id) {
            mActivity.onDeviceClick(pos);
        }
    };

    // Listener for progress timer expiration
    private CustomTimerCallback mPgScanCallback = new CustomTimerCallback() {
        public void onTimeout() {
            mActivity.onScanTimeout();
        }

        public void onTick(int i) {
            mActivity.refreshBusyIndicator();
        }
    };

    // Listener for connect/disconnect expiration
    private CustomTimerCallback mClearStatusCallback = new CustomTimerCallback() {
        public void onTimeout() {
            mActivity.runOnUiThread(new Runnable() {
                public void run() {
                    Toast.makeText(getActivity(), R.string.app_closing, Toast.LENGTH_LONG)
                            .show();

                }
            });
            mStatusTimer = null;
        }

        public void onTick(int i) {
        }
    };

    public void stopTimers() {
        if (mScanTimer != null) {
            mScanTimer.stop();
            mScanTimer = null;
        }
    }

    //
    // CLASS DeviceAdapter: handle device list
    //
    @SuppressLint("InflateParams")
    class DeviceListAdapter extends BaseAdapter {
        private List<BleDeviceInfo> mDevices;
        private LayoutInflater mInflater;

        public DeviceListAdapter(Context context, List<BleDeviceInfo> devices) {
            mInflater = LayoutInflater.from(context);
            mDevices = devices;
        }

        public int getCount() {
            return mDevices.size();
        }

        public Object getItem(int position) {
            return mDevices.get(position);
        }

        public long getItemId(int position) {
            return position;
        }

        public View getView(int position, View convertView, ViewGroup parent) {
            ViewGroup vg;

            if (convertView != null) {
                vg = (ViewGroup) convertView;
            } else {
                vg = (ViewGroup) mInflater.inflate(R.layout.element_device, null);
            }

            BleDeviceInfo deviceInfo = mDevices.get(position);
            BluetoothDevice device = deviceInfo.getBluetoothDevice();
            int rssi = deviceInfo.getRssi();
            String name;
            name = device.getName();
            if (name == null) {
                name = new String("Unknown device");
            }

            String descr = name + "\n" + device.getAddress() + "\nRssi: " + rssi + " dBm";
            ((TextView) vg.findViewById(R.id.descr)).setText(descr);

            ImageView iv = (ImageView) vg.findViewById(R.id.devImage);
            iv.setImageResource(R.drawable.sensortag);

            // Disable connect button when connecting or connected
            Button bv = (Button) vg.findViewById(R.id.btnConnect);

            return vg;
        }
    }
}