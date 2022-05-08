package cn.wch.otaupdate.ui;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import java.io.File;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

import cn.wch.blelib.chip.ChipType;
import cn.wch.otaupdate.R;

public class ChipTypeAdapter extends RecyclerView.Adapter<ChipTypeAdapter.MyViewHolder>{

    private Context context;
    private List<ChipType> chipTypes=new ArrayList<>();
    private OnSelectListener listener;
    public ChipTypeAdapter(Context context,OnSelectListener listener) {
        this.context = context;
        this.listener=listener;

        ChipType[] values = ChipType.values();
        for (ChipType value : values) {
            if(value!=ChipType.UNKNOWN){
                chipTypes.add(value);
            }
        }
    }

    @NonNull
    @Override
    public MyViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        return new MyViewHolder(LayoutInflater.from(context).inflate(R.layout.dialog_chiptype_item, parent, false));
    }

    @Override
    public void onBindViewHolder(@NonNull MyViewHolder holder, int position) {
        final ChipType chipType = chipTypes.get(position);
        holder.chip.setText(chipType.getDescription());
        holder.itemView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(listener!=null){
                    listener.onSelect(chipType);
                }
            }
        });
    }

    @Override
    public int getItemCount() {
        return chipTypes.size();
    }

    public static class MyViewHolder extends RecyclerView.ViewHolder{
        TextView chip;
        public MyViewHolder(@NonNull View itemView) {
            super(itemView);
            chip=itemView.findViewById(R.id.type);
        }
    }

    public interface OnSelectListener{
        void onSelect(ChipType chipType);
    }
}
