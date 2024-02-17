package ru.shtrih_m.project14;

import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;

public class AndroidNative
{
    public static int nativeMethod(Context context, int param)
    {
        try
        {
            switch(param)
            {
                case 1031: // SettingCode_WiFi
                {
                    Intent intent = new Intent(android.provider.Settings.ACTION_WIFI_SETTINGS);
                    if (intent == null) return -1;
                    context.startActivity(intent);
                    return 0;
                }
                case 1046: // SettingCode_SystemSettings
                {
                    Intent intent = new Intent(android.provider.Settings.ACTION_SETTINGS);
                    if (intent == null) return -1;
                    context.startActivity(intent);
                    return 0;
                }
                case 1045: // SettingCode_Equipment
                {
                    PackageManager pm = context.getPackageManager();
                    Intent intent = pm.getLaunchIntentForPackage("ru.shtrih_m.SettingsMng");
                    if (intent == null) return -1;
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
        return 1;
    }
}
