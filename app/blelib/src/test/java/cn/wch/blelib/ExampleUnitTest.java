package cn.wch.blelib;

import org.junit.Test;

import static org.junit.Assert.*;

import androidx.annotation.NonNull;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;

import cn.wch.blelib.ch583.ota.exception.CH583OTAException;
import cn.wch.blelib.host.core.Connector;

/**
 * Example local unit test, which will execute on the development machine (host).
 *
 * @see <a href="http://d.android.com/tools/testing">Testing documentation</a>
 */
public class ExampleUnitTest {
    @Test
    public void addition_isCorrect() {
        try {
            ByteBuffer byteBuffer = parseBinFile2(new File("D:\\1.BIN"), 0x2000);
            saveAsBinFile(new File("D:\\2.bin"),byteBuffer.array());
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static ByteBuffer parseBinFile2(File file, int startAddr) throws IOException {

        //读取文件
        FileInputStream stream = new FileInputStream(file);
        FileChannel channel = stream.getChannel();
        channel.position(startAddr);

        //缓冲区实际大小为文件大小减去擦除起始地址
        ByteBuffer byteBuffer=ByteBuffer.allocate((int) file.length()-startAddr);
        channel.read(byteBuffer);
        channel.close();
        stream.close();
        byteBuffer.flip();
        return byteBuffer;
    }

    public static void saveAsBinFile(File file,byte[] data)throws IOException {
        if(!file.exists()){
            file.createNewFile();
        }
        FileOutputStream fileOutputStream=new FileOutputStream(file);
        fileOutputStream.write(data);
        fileOutputStream.flush();
        fileOutputStream.close();
    }
}