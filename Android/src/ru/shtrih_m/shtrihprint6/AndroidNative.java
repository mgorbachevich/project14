package ru.shtrih_m.shtrihprint6;

import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.bluetooth.BluetoothAdapter;
import android.os.Environment;

public class AndroidNative
{
    public static int startNativeActivity(Context context, int param)
    {
        try
        {
            switch(param)
            {
                case 1030: // SettingCode_Ethernet
                {
                    Intent intent = new Intent("android.settings.ETHERNET_SETTINGS");
                    if (intent == null) return -1;
                    intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                    context.startActivity(intent);
                    return 0;
                }
                case 1031: // SettingCode_WiFi
                {
                    Intent intent = new Intent(android.provider.Settings.ACTION_WIFI_SETTINGS);
                    if (intent == null) return -1;
                    intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                    context.startActivity(intent);
                    return 0;
                }
                case 1046: // SettingCode_SystemSettings
                {
                    Intent intent = new Intent(android.provider.Settings.ACTION_SETTINGS);
                    if (intent == null) return -1;
                    intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                    context.startActivity(intent);
                    return 0;
                }
                case 1045: // SettingCode_Equipment
                {
                    PackageManager pm = context.getPackageManager();
                    Intent intent = pm.getLaunchIntentForPackage("ru.shtrih_m.SettingsMng");
                    if (intent == null) return -1;
                    intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                    intent.addCategory(Intent.CATEGORY_LAUNCHER);
                    context.startActivity(intent);
                    return 0;
                }
            }
        }
        catch (Exception e)
        {
            return -2;
        }
        return -1;
    }

    public static int isNativeEnvironment(int interfaceType)
    {
        try
        {
            switch(interfaceType)
            {
                case 1: // Bluetooth
                {
                    // https://stackoverflow.com/questions/25684953/android-check-if-bluetooth-is-on-off
                    BluetoothAdapter ba = BluetoothAdapter.getDefaultAdapter();
                    if (ba != null && ba.isEnabled()) return 1;
                    return 0;
                }
                case 3: // SDCard
                {
                    // https://stackoverflow.com/questions/902089/how-to-tell-if-the-sdcard-is-mounted-in-android
                    // Checks if external storage is available to at least read
                    String state = Environment.getExternalStorageState();
                    if (Environment.MEDIA_MOUNTED.equals(state) ||
                        Environment.MEDIA_MOUNTED_READ_ONLY.equals(state)) return 1;
                    return 0;
                }
            }
        }
        catch (Exception e)
        {
            return -2;
        }
        return -1;
    }

    public static int getMemory(int type) // 0 - maxHeap, 1 - availableHeap, 2 - used
    {
        // https://stackoverflow.com/questions/3170691/how-to-get-current-memory-usage-in-android
        try
        {
            Runtime r = Runtime.getRuntime();
            switch(type)
            {
                case 0: return (int)(r.maxMemory() / 1024); // Kb
                case 1: return (int)((r.maxMemory() - r.totalMemory() + r.freeMemory()) / 1024);
                case 2: return (int)((r.totalMemory() - r.freeMemory())  / 1024);
            }
        }
        catch (Exception e)
        {
            return -1;
        }
        return -2;
    }
}
