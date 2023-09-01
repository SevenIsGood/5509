/**************************************************************************************************
 Filename:       BleScannerScreen.java

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

    import android.Manifest;
    import android.app.Dialog;
    import android.bluetooth.BluetoothAdapter;
    import android.bluetooth.BluetoothDevice;
    import android.bluetooth.BluetoothManager;
    import android.content.BroadcastReceiver;
    import android.content.Context;
    import android.content.Intent;
    import android.content.IntentFilter;
    import android.content.pm.PackageManager;
    import android.content.res.Resources;
    import android.net.Uri;
    import android.os.Bundle;
    import android.projects.com.c5545ble.R;
    import android.support.v4.app.ActivityCompat;
    import android.support.v4.content.ContextCompat;
    import android.util.Log;
    import android.view.Menu;
    import android.view.MenuInflater;
    import android.view.MenuItem;
    import android.view.View;
    import android.view.Window;
    import android.widget.Toast;

    import java.io.File;
    import java.io.IOException;
    import java.util.ArrayList;
    import java.util.List;

    public class BleScannerScreen extends ViewPagerActivity {

        // Requests to other activities
        private static final int REQ_ENABLE_BT = 0;
        private static final int MY_PERMISSOIN_REQUEST_ACCESS_FINE_LOCATION = 1;
        private static final Uri URL_FORUM = Uri
                .parse("https://e2e.ti.com/support/dsp/c5000/");
        private static final Uri URL_STHOME = Uri
                .parse("http://www.ti.com/ww/en/wireless_connectivity/sensortag/index.shtml?INTC=SensorTagGatt&HQS=sensortag");
        private static BluetoothManager mBluetoothManager;
        private ScanView mScanView;
        private BluetoothAdapter mBluetoothAdapter;
        // BLE management
        private boolean mBtAdapterEnabled = false;
        private boolean mScanning = false;
        private List<BleDeviceInfo> mDeviceInfoList;
        private BluetoothLeService mBluetoothLeService = null;
        private IntentFilter mFilter;
        private String[] mDeviceFilter = null;
        private boolean mInitialised = false;
        private C5545AppClass appObj;
        private BluetoothAdapter.LeScanCallback mLeScanCallback = new BluetoothAdapter.LeScanCallback() {

            public void onLeScan(final BluetoothDevice device, final int rssi,
                                 byte[] scanRecord) {
                runOnUiThread(new Runnable() {
                    public void run() {
                        if (null != device && checkDeviceFilter(device.getName())) {
                            if (!deviceInfoExists(device.getAddress())) {
                                // New device
                                BleDeviceInfo deviceInfo = createDeviceInfo(device, rssi);
                                addDevice(deviceInfo);
                            }
                        }
                    }

                });
            }
        };
        private BroadcastReceiver mReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                final String action = intent.getAction();
                if (BluetoothAdapter.ACTION_STATE_CHANGED.equals(action)) {
                    // Bluetooth adapter state change
                    switch (mBluetoothAdapter.getState()) {
                        case BluetoothAdapter.STATE_ON:
                            updateMenu();
                            mScanView.setDisplayStatus(true, mScanning);
                            break;
                        case BluetoothAdapter.STATE_OFF:
                            updateMenu();
                            mScanView.setDisplayStatus(false, mScanning);
                            break;
                        default:
                            break;
                    }
                    updateGuiState();
                } else {
                    Log.w(C5545AppClass.TAG, "Unknown action: " + action);
                }
            }
        };

        public BleScannerScreen() {
            mResourceFragmentPager = R.layout.fragment_pager;
            mResourceIdPager = R.id.pager;
        }

        @Override
        public void onCreate(Bundle savedInstanceState) {
            // Start the application
            requestWindowFeature(Window.FEATURE_INDETERMINATE_PROGRESS);
            super.onCreate(savedInstanceState);
            // check whether the Bluetooth is supported or not ,if Bluetooth not
            // supported then end the activity.

            if (ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION) !=
                    PackageManager.PERMISSION_GRANTED) {
                if (ActivityCompat.shouldShowRequestPermissionRationale(this, Manifest.permission.ACCESS_FINE_LOCATION)) {
                    Toast.makeText(this, "This permission is necessary for application to work", Toast.LENGTH_LONG);
                } else {
                    ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.ACCESS_FINE_LOCATION}, MY_PERMISSOIN_REQUEST_ACCESS_FINE_LOCATION);
                }
            }


            mBluetoothManager = (BluetoothManager) getSystemService(BLUETOOTH_SERVICE);
            mBluetoothAdapter = mBluetoothManager.getAdapter();
            if (mBluetoothAdapter == null) {
                Toast.makeText(this, R.string.ble_not_supported,
                        Toast.LENGTH_SHORT).show();
                finish();
                return;
            }

            appObj = (C5545AppClass) getApplicationContext();
            mBluetoothLeService = appObj.getBLEService();
            if (mBluetoothLeService == null) {
                Toast.makeText(this, R.string.ble_connection_fail,
                        Toast.LENGTH_SHORT).show();
                finish();
            }

            // Initialize device list container and device filter
            mDeviceInfoList = new ArrayList<BleDeviceInfo>();
            Resources res = getResources();
            mDeviceFilter = res.getStringArray(R.array.device_filter);

            // Create the fragments and add them to the view pager and tabs
            mScanView = new ScanView();
            mSectionsPagerAdapter.addSection(mScanView, "BLE Device List");

            HelpView hw = new HelpView();
            hw.setParameters("help_scan.html", R.layout.fragment_help, R.id.webpage);
            mSectionsPagerAdapter.addSection(hw, "Help");

            // Register the BroadcastReceiver
            mFilter = new IntentFilter(BluetoothAdapter.ACTION_STATE_CHANGED);
            mFilter.addAction(BluetoothLeService.ACTION_GATT_CONNECTED);
            mFilter.addAction(BluetoothLeService.ACTION_GATT_DISCONNECTED);

        }

        @Override
        protected void onStart() {
            super.onStart();
            updateMenu();

        }

        @Override
        public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
            super.onRequestPermissionsResult(requestCode, permissions, grantResults);

            switch (requestCode) {
                case MY_PERMISSOIN_REQUEST_ACCESS_FINE_LOCATION: {
                    if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    } else {
                        finish();
                    }
                }
            }
        }

        @Override
        protected void onResume() {
            super.onResume();
            registerReceiver(mReceiver, mFilter);
        }

        @Override
        protected void onPause() {
            super.onPause();

        }

        @Override
        protected void onStop() {
            super.onStop();
            unregisterReceiver(mReceiver);
            mScanView.stopTimers();
            stopScan();
        }

        @Override
        public boolean onPrepareOptionsMenu(Menu menu) {
            MenuItem mItem = menu.findItem(R.id.opt_bt);
            if (mBluetoothAdapter.isEnabled()) {
                mItem.setIcon(R.drawable.ic_action_bluetooth_enabled);
                mItem.setEnabled(false);
                mScanView.setDisplayStatus(true, mScanning);

            } else {
                mItem.setIcon(R.drawable.ic_action_bluetooth);
                mItem.setEnabled(true);
                mScanView.setDisplayStatus(false, mScanning);
            }
            return super.onPrepareOptionsMenu(menu);
        }

        private void updateMenu() {
            invalidateOptionsMenu();
        }

        @Override
        public void onDestroy() {
            super.onDestroy();
            mBluetoothAdapter = null;
            File cache = getCacheDir();
            String path = cache.getPath();
            try {
                Runtime.getRuntime().exec(String.format("rm -rf %s", path));
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        @Override
        protected void onRestart() {
            super.onRestart();
        }

        @Override
        public boolean onOptionsItemSelected(MenuItem item) {
            // Handle presses on the action bar items
            switch (item.getItemId()) {
                case android.R.id.home:
                    onBackPressed();
                    return true;
                case R.id.opt_bt:
                    onBluetooth();
                    break;
                case R.id.opt_e2e:
                    onUrl(URL_FORUM);
                    break;
                case R.id.opt_sthome:
                    onUrl(URL_STHOME);
                    break;
                case R.id.opt_about:
                    onAbout();
                    break;
                case R.id.opt_exit:
                    Toast.makeText(this, "Exit...", Toast.LENGTH_SHORT).show();
                    finish();
                    break;
                default:
                    return super.onOptionsItemSelected(item);
            }
            return true;
        }

        @Override
        public boolean onCreateOptionsMenu(Menu menu) {
            this.optionsMenu = menu;
            // Inflate the menu items for use in the action bar
            MenuInflater inflater = getMenuInflater();
            inflater.inflate(R.menu.main_activity_actions, menu);
            return super.onCreateOptionsMenu(menu);
        }

        List<BleDeviceInfo> getDeviceInfoList() {
            return mDeviceInfoList;
        }

        public void updateGuiState() {

            if (mBluetoothAdapter.isEnabled() & mScanView != null) {
                mScanView.setScanBtnStatus(true);
                mScanView.setDisplayStatus(true, mScanning);
            } else {
                mScanView.setScanBtnStatus(false);
                mDeviceInfoList.clear();
                mScanView.notifyDataSetChanged();
                mScanView.setDisplayStatus(false, mScanning);
            }
        }

        public void onDeviceClick(final int pos) {
            if (mScanning)
                stopScan();

            final BluetoothDevice device = mDeviceInfoList.get(pos).getBluetoothDevice();
            if (device == null) {
                Log.e(C5545AppClass.TAG, "Retruning as Device is NULL from onDeviceClick");
                return;
            }
            final Intent intent = new Intent(this, C5545Equalizer.class);
            intent.putExtra(C5545Equalizer.EXTRAS_DEVICE_NAME, device.getName());
            intent.putExtra(C5545Equalizer.EXTRAS_DEVICE_ADDRESS,
                    device.getAddress());
            startActivity(intent);
        }

        public void onScanTimeout() {
            runOnUiThread(new Runnable() {
                public void run() {
                    stopScan();
                }
            });
        }

        void onScanViewReady() {

            if (!mInitialised) {
                mBtAdapterEnabled = mBluetoothAdapter.isEnabled();
                if (!mBtAdapterEnabled) {
                    // Request BT adapter to be turned on
                    Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                    startActivityForResult(enableIntent, REQ_ENABLE_BT);
                }
                mInitialised = true;
            } else {
                mScanView.notifyDataSetChanged();
            }
            // Initial state of widgets
            updateGuiState();
        }

        private void onBluetooth() {
            boolean isBTEnabled = mBluetoothAdapter.isEnabled();
            if (!isBTEnabled) {
                mBluetoothAdapter.enable();
            }
        }

        private void onAbout() {
            final Dialog dialog = new AboutDialog(this);
            dialog.show();
        }

        public void onBtnScan(View view) {
            if (mScanning) {
                stopScan();
            } else {
                startScan();
            }
        }

        private void startScan() {
            // Start device discovery
            if (mBluetoothAdapter.isEnabled()) {
                mDeviceInfoList.clear();
                mScanView.notifyDataSetChanged();
                scanLeDevice(true);
                mScanView.updateGui(mScanning);
                if (!mScanning) {
                    setError();
                }
            } else {
                setError();
            }

        }

        private void stopScan() {
            mScanning = false;
            scanLeDevice(false);
            mScanView.updateGui(mScanning);
        }

        void setError() {
            mScanView.setError();
        }

        private BleDeviceInfo createDeviceInfo(BluetoothDevice device, int rssi) {
            BleDeviceInfo deviceInfo = new BleDeviceInfo(device, rssi);
            return deviceInfo;
        }

        private boolean scanLeDevice(boolean enable) {

            if (mBluetoothAdapter != null & mLeScanCallback != null) {
                if (enable) {
                    mScanning = mBluetoothAdapter.startLeScan(mLeScanCallback);
                } else {
                    mScanning = false;
                    mBluetoothAdapter.stopLeScan(mLeScanCallback);
                }
            }
            return mScanning;
        }

        private void addDevice(BleDeviceInfo device) {
            mDeviceInfoList.add(device);
            mScanView.notifyDataSetChanged();
        }

        boolean checkDeviceFilter(String deviceName) {
            if (deviceName == null)
                return false;

            int n = mDeviceFilter.length;
            if (n > 0) {
                boolean found = false;
                for (int i = 0; i < n && !found; i++) {
                    found = deviceName.equals(mDeviceFilter[i]);
                }
                return found;
            } else
                // Allow all devices if the device filter is empty
                return true;
        }

        private boolean deviceInfoExists(String address) {
            for (int i = 0; i < mDeviceInfoList.size(); i++) {
                if (mDeviceInfoList.get(i).getBluetoothDevice().getAddress()
                        .equals(address)) {
                    return true;
                }
            }
            return false;
        }

        private void onUrl(final Uri uri) {
            Intent web = new Intent(Intent.ACTION_VIEW, uri);
            startActivity(web);
        }
    }
