package cn.wch.bleota;

import android.app.Application;

import cn.wch.blelib.ch583.CH583BluetoothManager;
import cn.wch.blelib.ch583.ota.CH583OTAManager;
import cn.wch.blelib.exception.BLELibException;
import cn.wch.blelib.utils.LogUtil;

public class MyApplication extends Application {
    @Override
    public void onCreate() {
        super.onCreate();
        try {
            CH583BluetoothManager.getInstance().init(this);
            CH583OTAManager.getInstance().init(this);

        } catch (BLELibException e) {
            LogUtil.d(e.getMessage());
            e.printStackTrace();
        }

    }
}
