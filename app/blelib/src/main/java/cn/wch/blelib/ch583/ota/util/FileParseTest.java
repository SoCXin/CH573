package cn.wch.blelib.ch583.ota.util;

import android.content.Context;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

public class FileParseTest {

    public static Context mContext;

    public static void init(Context context){
        mContext=context;
    }

    public static void saveAsBinFile(byte[] data)throws IOException {
        File dir = mContext.getExternalFilesDir("PSD");
        File file=new File(dir,"1.bin");

        FileOutputStream fileOutputStream=new FileOutputStream(file);
        fileOutputStream.write(data);
        fileOutputStream.flush();
        fileOutputStream.close();
    }
}
