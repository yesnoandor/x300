package com.fleety.android.r600util.service;

import android.app.IntentService;
import android.content.Intent;
import android.content.pm.IPackageInstallObserver;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.net.Uri;
import android.os.Bundle;

import com.fleety.android.action.R600UtilStrings;

public class PackageInstaller extends IntentService
{
    private Uri            mPackageURI;
    private PackageManager mPm;
    private String         apkPath;
    private String         pkgName;

    public PackageInstaller()
    {
        super("PackageInstallerService");
    }

    @Override
    protected void onHandleIntent(Intent intent)
    {
        System.out.println(getClass().getSimpleName() + "-> "
                + "start installing apk.");
        apkPath = intent.getExtras().getString(
                R600UtilStrings.R600UTIL_DATA_APK_PATH);
        if (apkPath == null || "".equals(apkPath))
        {
            System.out.println(getClass().getSimpleName() + "-> "
                    + "Apk path error ,error=" + apkPath);
            return;
        }
        mPm = getPackageManager();
        mPackageURI = Uri.parse(apkPath);
        showInfo();
        int installFlags = 0;
        try
        {
            PackageInfo pi = mPm.getPackageInfo(pkgName,
                    PackageManager.GET_UNINSTALLED_PACKAGES);
            if (pi != null)
            {
                installFlags |= PackageManager.INSTALL_REPLACE_EXISTING;
            }
        }
        catch (NameNotFoundException e)
        {
        }
        PackageInstallObserver observer = new PackageInstallObserver();
        mPm.installPackage(mPackageURI, observer, installFlags, pkgName);
        System.out.println(getClass().getSimpleName() + "-> "
                + "PackageInstallObserver =" + observer);

    }

    private void showInfo()
    {
        mPackageURI = Uri.parse(apkPath);
        PackageInfo pi = mPm.getPackageArchiveInfo(apkPath,
                PackageManager.GET_ACTIVITIES);
        if (pi != null)
        {
            System.out.println(getClass().getSimpleName() + "-> "
                    + "PackageInfo.packageName =" + pi.packageName);
            System.out.println(getClass().getSimpleName() + "-> "
                    + "PackageInfo.versionName =" + pi.versionName);
            pkgName = pi.packageName;
        }
        else
        {
            System.err.println(getClass().getSimpleName() + "-> "
                    + "Read the apk " + apkPath
                    + " error. The package could not be successfully parsed.");
        }

    }

    class PackageInstallObserver extends IPackageInstallObserver.Stub
    {
        public void packageInstalled(String packageName, int returnCode)
        {
            System.out.println(getClass().getSimpleName() + "-> "
                    + "PackageInstallObserver packageInstalled");
            System.out.println(getClass().getSimpleName() + "-> " + "Pkg="
                    + packageName + " returnCode=" + returnCode);
            Intent intent = new Intent();
            intent.setAction(R600UtilStrings.R600UTIL_SEND_INSTALL_ACTION);
            Bundle data = new Bundle();
            data.putString(R600UtilStrings.R600UTIL_DATA_PACKAGE_NAME,
                    packageName);
            data.putInt(R600UtilStrings.R600UTIL_DATA_INSTALL_APK_RESULT,
                    returnCode);
            intent.putExtras(data);
            sendBroadcast(intent);
        }

    };

}
