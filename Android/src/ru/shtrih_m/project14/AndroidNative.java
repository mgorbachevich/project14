package ru.shtrih_m.project14;

import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;

public class AndroidNative
{
    public static int nativeMethod(Context context, int v)
    {
        try
        {
            PackageManager pm = context.getPackageManager();
            Intent intent = pm.getLaunchIntentForPackage("ru.shtrih_m.SettingsMng");
            if (intent == null) throw new PackageManager.NameNotFoundException();
            intent.addCategory(Intent.CATEGORY_LAUNCHER);
            context.startActivity(intent);
        }
        catch (Exception e)
        {
            return -1;
        }
        return 0;
    }
}
