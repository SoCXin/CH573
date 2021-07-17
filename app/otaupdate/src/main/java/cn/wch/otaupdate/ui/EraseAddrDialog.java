package cn.wch.otaupdate.ui;

import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.DialogFragment;
import cn.wch.otaupdate.R;

public class EraseAddrDialog extends DialogFragment {
    private Button confirm;
    private Button cancel;
    private EditText etAddr;
    private OnClickListener onClickListener;
    private Handler handler;

    public static EraseAddrDialog newInstance() {
        Bundle args = new Bundle();
        EraseAddrDialog fragment = new EraseAddrDialog();
        fragment.setArguments(args);
        return fragment;
    }

    public EraseAddrDialog() {
        handler=new Handler(Looper.getMainLooper());
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        getDialog().getWindow().requestFeature(Window.FEATURE_NO_TITLE);
        View inflate = inflater.inflate(R.layout.dialog_erase_addr, null);
        init(inflate);
        return inflate;
    }

    private void init(View inflate) {
        confirm=inflate.findViewById(R.id.btnConfirm);
        cancel=inflate.findViewById(R.id.btnCancel);
        etAddr=inflate.findViewById(R.id.etAddr);

        confirm.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(checkValid()){
                    dismiss();
                    if(onClickListener!=null){
                        onClickListener.onStartUpgrade(parseValue());
                    }
                }
            }
        });

        cancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                dismiss();
            }
        });
    }

    private boolean checkValid(){
        String s = etAddr.getText().toString();
        if(TextUtils.isEmpty(s)){
            showToast("地址为空");
            return false;
        }
        if(!s.matches("[0-9|a-f|A-F]+")){
            showToast("HEX地址不合法");
            return false;
        }
        return true;
    }

    private int parseValue(){
        String s = etAddr.getText().toString();
        return Integer.parseInt(s,16);
    }

    public void setOnClickListener(OnClickListener listener){
        this.onClickListener=listener;
    }

    public interface OnClickListener{
        void onStartUpgrade(int addr);
    }

    private void showToast(final String message){
        handler.post(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(getContext(),message,Toast.LENGTH_SHORT).show();
            }
        });
    }
}
