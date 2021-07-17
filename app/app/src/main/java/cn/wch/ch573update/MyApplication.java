package cn.wch.ch573update;

import android.app.Application;

import java.io.IOException;

import cn.wch.blelib.ch573.CH573BluetoothManager;
import cn.wch.blelib.ch573.ota.CH573OTAManager;
import cn.wch.blelib.exception.BLELibException;
import cn.wch.blelib.utils.LogUtil;

public class MyApplication extends Application {
    @Override
    public void onCreate() {
        super.onCreate();
        try {
            CH573BluetoothManager.getInstance().init(this);
            CH573OTAManager.getInstance().init(this);

        } catch (BLELibException e) {
            LogUtil.d(e.getMessage());
            e.printStackTrace();
        }

    }
}
