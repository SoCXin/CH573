package cn.wch.blelib.ch583.callback;

import android.bluetooth.BluetoothDevice;

public interface ScanResult {

    void onResult(BluetoothDevice device, int rssi, byte[] broadcastRecord);
}
