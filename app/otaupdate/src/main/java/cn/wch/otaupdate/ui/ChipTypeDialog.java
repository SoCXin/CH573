package cn.wch.otaupdate.ui;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.widget.Button;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.DialogFragment;
import androidx.recyclerview.widget.DividerItemDecoration;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import cn.wch.blelib.chip.ChipType;
import cn.wch.otaupdate.R;

public class ChipTypeDialog extends DialogFragment {

    private RecyclerView recyclerView;
    private Button cancel;
    private OnSelectListener listener;
    public static ChipTypeDialog newInstance() {

        Bundle args = new Bundle();
        ChipTypeDialog fragment = new ChipTypeDialog();
        fragment.setArguments(args);
        return fragment;
    }

    public ChipTypeDialog() {

    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        getDialog().getWindow().requestFeature(Window.FEATURE_NO_TITLE);
        View inflate = inflater.inflate(R.layout.dialog_chiptype, null);
        init(inflate);
        return inflate;
    }

    private void init(View view){
        recyclerView=view.findViewById(R.id.chipList);
        cancel=view.findViewById(R.id.cancel);
        ChipTypeAdapter adapter=new ChipTypeAdapter(getActivity(), new ChipTypeAdapter.OnSelectListener() {
            @Override
            public void onSelect(ChipType chipType) {
                dismiss();
                if(listener!=null){
                    listener.onSelect(chipType);
                }
            }
        });
        recyclerView.setLayoutManager(new LinearLayoutManager(getActivity(),LinearLayoutManager.VERTICAL,false));
        recyclerView.setAdapter(adapter);
        DividerItemDecoration decoration=new DividerItemDecoration(getActivity(),DividerItemDecoration.VERTICAL);
        recyclerView.addItemDecoration(decoration);

        cancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                dismiss();
            }
        });
    }

    public static interface OnSelectListener{
        void onSelect(ChipType chipType);
    }

    public void setSelectListener(OnSelectListener listener){
        this.listener=listener;
    }
}
