/**************************************************************************************************
 Filename:       BluetoothLeService.java

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

import android.app.Service;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.content.Context;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.util.Log;

import java.util.List;

/**
 * Service for managing connection and data communication with a GATT server
 * hosted on a given Bluetooth LE device.
 */
public class BluetoothLeService extends Service {

    public static final int STATE_DISCONNECTED = 0;
    public static final int STATE_CONNECTING = 1;
    public static final int STATE_CONNECTED = 2;
    public final static String ACTION_GATT_CONNECTED = "com.example.bluetooth.le.ACTION_GATT_CONNECTED";
    public final static String ACTION_GATT_DISCONNECTED = "com.example.bluetooth.le.ACTION_GATT_DISCONNECTED";
    public final static String ACTION_GATT_SERVICES_DISCOVERED = "com.example.bluetooth.le.ACTION_GATT_SERVICES_DISCOVERED";
    public final static String ACTION_DATA_AVAILABLE = "com.example.bluetooth.le.ACTION_DATA_AVAILABLE";
    public final static String ACTION_WRITE_COMPLETE = "com.example.bluetooth.le.ACTION_WRITE_COMPLETE";
    public final static String ACTION_WRITE_FAILED = "com.example.bluetooth.le.ACTION_WRITE_COMPLETE";
    public final static String ACTION_READ_COMPLETE = "com.example.bluetooth.le.ACTION_READ_COMPLETE";
    public final static String ACTION_READ_FAILED = "com.example.bluetooth.le.ACTION_READ_FAILED";
    public final static String ACTION_AUDIO_STATUS_CHANGED = "com.example.bluetooth.le.ACTION_AUDIO_STATUS_CHANGED";
    public final static String EXTRA_DATA = "com.example.ti.ble.common.EXTRA_DATA";
    public final static String EXTRA_UUID = "com.example.ti.ble.common.EXTRA_UUID";
    public final static String EXTRA_STATUS = "com.example.ti.ble.common.EXTRA_STATUS";
    public final static String EXTRA_ADDRESS = "com.example.ti.ble.common.EXTRA_ADDRESS";
    public static int data1 = 0, data2 = 0, data3 = 0;
    private final IBinder mBinder = new LocalBinder();
    private BluetoothManager mBluetoothManager;
    private BluetoothAdapter mBluetoothAdapter;
    private String mBluetoothDeviceAddress;
    private BluetoothGatt mBluetoothGatt;
    private int mConnectionState = STATE_DISCONNECTED;
    // Implements callback methods for GATT events that the app cares about. For
    // example,connection change and services discovered.
    private final BluetoothGattCallback mGattCallback = new BluetoothGattCallback() {
        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status,
                                            int newState) {
            String intentAction;
            if (newState == BluetoothProfile.STATE_CONNECTED) {
                intentAction = ACTION_GATT_CONNECTED;
                mConnectionState = STATE_CONNECTED;
                broadcastUpdate(intentAction);
                // Attempts to discover services after successful connection.
                mBluetoothGatt.discoverServices();
            } else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
                // if service connection is not successful then broadcast the
                // broadcast receiver for the action ACTION_GATT_DISCONNECTED.
                intentAction = ACTION_GATT_DISCONNECTED;
                mConnectionState = STATE_DISCONNECTED;
                broadcastUpdate(intentAction);
            }
        }

        // Call back of Blutooth BLE service if service discovered and broad
        // cast the same.
        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                broadcastUpdate(ACTION_GATT_SERVICES_DISCOVERED);
                Log.i(C5545AppClass.TAG, "BLE ServicesDiscovered !!! : " + status);
            } else
                Log.i(C5545AppClass.TAG, "BLE ServicesDiscovered Failure !!! : " + status);
        }

        @Override
        public void onCharacteristicRead(BluetoothGatt gatt,
                                         BluetoothGattCharacteristic characteristic, int status) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                broadcastUpdate(ACTION_READ_COMPLETE, characteristic);
            } else {
                broadcastUpdate(ACTION_READ_FAILED, characteristic);
            }
        }

        public void onCharacteristicWrite(BluetoothGatt gatt,
                                          BluetoothGattCharacteristic characteristic, int status) {
            Log.d(C5545AppClass.TAG, "onCharacteristicWite");
            if (status == BluetoothGatt.GATT_SUCCESS) {
                broadcastUpdate(ACTION_WRITE_COMPLETE, characteristic);
            } else {
                broadcastUpdate(ACTION_WRITE_FAILED, characteristic);
            }
        }

        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {
            Log.d(C5545AppClass.TAG, "onCharacteristicChanged");
            super.onCharacteristicChanged(gatt, characteristic);
            Log.d(C5545AppClass.TAG, "onCharacteristicChanged");
            broadcastUpdate(ACTION_DATA_AVAILABLE, characteristic);
        }


        @Override
        public void onDescriptorWrite(BluetoothGatt gatt, BluetoothGattDescriptor descriptor, int status) {
            super.onDescriptorWrite(gatt, descriptor, status);
            // the characteristic, let's see if it failed or not
            if (status == BluetoothGatt.GATT_SUCCESS) {
                Log.d(C5545AppClass.TAG, "GATTSUCCESS :: UUID:: " + descriptor.getCharacteristic().getUuid().toString() + "Status ::" + status);
            } else {
                Log.d(C5545AppClass.TAG, "GATTFAILED ::UUID:: " + descriptor.getCharacteristic().getUuid().toString() + "Status ::" + status);
            }

        }
    };

    private void broadcastUpdate(final String action) {
        final Intent intent = new Intent(action);
        sendBroadcast(intent);
    }

    private void broadcastUpdate(final String action,
                                 final BluetoothGattCharacteristic characteristic) {
        final Intent intent = new Intent(action);

        final byte[] data = characteristic.getValue();
        if (data != null && data.length > 0) {
            intent.putExtra(EXTRA_DATA, data);
        }
        Log.d(C5545AppClass.TAG, "Broadcast Update:: " + data[0]);
        sendBroadcast(intent);
    }

    @Override
    public IBinder onBind(Intent intent) {
        return mBinder;
    }

    @Override
    public boolean onUnbind(Intent intent) {
        // After using a given device, you should make sure that
        // BluetoothGatt.close() is called
        // such that resources are cleaned up properly. In this particular
        // example, close() is
        // invoked when the UI is disconnected from the Service.
        close();
        return super.onUnbind(intent);
    }

    /**
     * Initializes a reference to the local Bluetooth adapter.
     *
     * @return Return true if the initialization is successful.
     */
    public boolean initialize() {
        // For API level 18 and above, get a reference to BluetoothAdapter
        // through
        // BluetoothManager.
        if (mBluetoothManager == null) {
            mBluetoothManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
            if (mBluetoothManager == null) {
                return false;
            }
        }
        mBluetoothAdapter = mBluetoothManager.getAdapter();
        if (mBluetoothAdapter == null) {
            return false;
        }
        return true;
    }

    /**
     * Connects to the GATT server hosted on the Bluetooth LE device.
     *
     * @param address The device address of the destination device.
     * @return Return true if the connection is initiated successfully. The
     * connection result is reported asynchronously through the
     * {@code BluetoothGattCallback#onConnectionStateChange(android.bluetooth.BluetoothGatt, int, int)}
     * callback.
     */
    public boolean connect(final String address) {
        if (mBluetoothAdapter == null || address == null) {
            return false;
        }
        final BluetoothDevice device = mBluetoothAdapter
                .getRemoteDevice(address);
        int connectionState = mBluetoothManager.getConnectionState(device,
                BluetoothProfile.GATT);
        if (connectionState == BluetoothProfile.STATE_DISCONNECTED) {
            // Previously connected device. Try to reconnect.
            if (mBluetoothDeviceAddress != null
                    && address.equals(mBluetoothDeviceAddress)
                    && mBluetoothGatt != null) {
                if (mBluetoothGatt.connect()) {
                    mConnectionState = STATE_CONNECTING;
                    return true;
                } else {
                    return false;
                }
            }
            if (device == null) {
                return false;
            }
            mBluetoothGatt = device.connectGatt(this, false, mGattCallback);
            mBluetoothDeviceAddress = address;
            mConnectionState = STATE_CONNECTING;
        }
        else
        {
            Log.w(C5545AppClass.TAG, "Attempt to connect in state: " + connectionState);
            return false;
        }
        return true;
    }

    /**
     * Disconnects an existing connection or cancel a pending connection. The
     * disconnection result is reported asynchronously through the
     * {@code BluetoothGattCallback#onConnectionStateChange(android.bluetooth.BluetoothGatt, int, int)}
     * callback.
     */
    public void disconnect() {
        if (mBluetoothAdapter == null || mBluetoothGatt == null) {
            return;
        }
        Log.d(C5545AppClass.TAG, "mBluetoothGatt disconnected");
        mBluetoothGatt.disconnect();
        mConnectionState = STATE_DISCONNECTED;
        if (mBluetoothGatt == null) {
            return;
        }
        mBluetoothGatt.close();
        mBluetoothGatt = null;
    }

    /**
     * After using a given BLE device, the app must call this method to ensure
     * resources are released properly.
     */
    public void close() {
        if (mBluetoothGatt == null) {
            return;
        }
        mBluetoothGatt.close();
        mBluetoothGatt = null;
    }

    /**
     * Request a read on a given {@code BluetoothGattCharacteristic}. The read
     * result is reported asynchronously through the
     * {@code BluetoothGattCallback#onCharacteristicRead(android.bluetooth.BluetoothGatt, android.bluetooth.BluetoothGattCharacteristic, int)}
     * callback.
     *
     * @param characteristic The characteristic to read from.
     */
    public void readCharacteristic(BluetoothGattCharacteristic characteristic) {
        if (mBluetoothAdapter == null || mBluetoothGatt == null) {
            return;
        }
        mBluetoothGatt.readCharacteristic(characteristic);
    }

    /**
     * Request a write on a given {@code BluetoothGattCharacteristic}. The read
     * result is reported asynchronously through the
     * {@code BluetoothGattCallback#onCharacteristicWrite(android.bluetooth.BluetoothGatt, android.bluetooth.BluetoothGattCharacteristic, int)}
     * callback.
     *
     * @param characteristic The characteristic to read from.
     */
    public void writeCharacteristic(BluetoothGattCharacteristic characteristic) {
        if (mBluetoothAdapter == null || mBluetoothGatt == null) {
            return;
        }
        mBluetoothGatt.writeCharacteristic(characteristic);
    }

    public int getConnectionStatus() {
        return mConnectionState;
    }

    /**
     * Enables or disables notification on a give characteristic.
     *
     * @param notifyCharacteristic Characteristic to act on.
     * @param enabled              If true, enable notification. False otherwise.
     */
    public void setCharacteristicNotification(BluetoothGattCharacteristic notifyCharacteristic, boolean enabled) {
        if (mBluetoothAdapter == null || mBluetoothGatt == null) {
            return;
        }
        if (mBluetoothGatt.setCharacteristicNotification(notifyCharacteristic, enabled)) {

            BluetoothGattDescriptor clientConfig = notifyCharacteristic
                    .getDescriptor(SampleGattAttributes.CLIENT_CHARACTERISTIC_CONFIG);
            if (clientConfig != null) {

                if (enabled) {
                    Log.d(C5545AppClass.TAG, "Enable notification: " + notifyCharacteristic.getUuid().toString());
                    clientConfig.setValue(BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE);
                } else {
                    Log.d(C5545AppClass.TAG, "Disable notification: " + notifyCharacteristic.getUuid().toString());
                    clientConfig.setValue(BluetoothGattDescriptor.DISABLE_NOTIFICATION_VALUE);
                }
                mBluetoothGatt.writeDescriptor(clientConfig);
            }

        }
    }

    /**
     * Retrieves a list of supported GATT services on the connected device. This
     * should be invoked only after {@code BluetoothGatt#discoverServices()}
     * completes successfully.
     *
     * @return A {@code List} of supported services.
     */
    public List<BluetoothGattService> getSupportedGattServices() {
        if (mBluetoothGatt == null)
            return null;

        return mBluetoothGatt.getServices();
    }

    public class LocalBinder extends Binder {
        BluetoothLeService getService() {
            return BluetoothLeService.this;
        }
    }
}