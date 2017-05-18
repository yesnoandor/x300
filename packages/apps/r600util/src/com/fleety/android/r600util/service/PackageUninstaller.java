package com.fleety.android.r600util.service;

import com.fleety.android.action.R600UtilStrings;

import android.app.IntentService;
import android.content.Intent;
import android.content.pm.IPackageDeleteObserver;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.os.Bundle;
import android.os.RemoteException;

public class PackageUninstaller extends IntentService
{
    private PackageManager mPm;
    private String         pkgName;

    public PackageUninstaller()
    {
        super("PackageUninstallerService");
    }

    @Override
    protected void onHandleIntent(Intent intent)
    {
        System.out.println(getClass().getSimpleName() + "-> "
                + "start uninstalling package.");
        pkgName = intent.getExtras().getString(
                R600UtilStrings.R600UTIL_DATA_PACKAGE_NAME);
        if (pkgName == null || "".equals(pkgName))
        {
            System.out.println(getClass().getSimpleName() + "-> "
                    + "Package name error ,error=" + pkgName);
            return;
        }
        mPm = getPackageManager();
        PackageInfo pi = null;
        try
        {
            pi = mPm.getPackageInfo(pkgName, PackageManager.GET_ACTIVITIES);
        }
        catch (NameNotFoundException e)
        {
            System.out.println(getClass().getSimpleName() + "-> "
                    + "pkg not installed. Not need to uninstall." + pkgName);
        }
        PackageDeleteObserver observer_delete = new PackageDeleteObserver();
        int uninstallFlags = 0;
        mPm.deletePackage(pkgName, observer_delete, uninstallFlags);
        System.out.println(getClass().getSimpleName() + "-> "
                + "PackageDeleteObserver delete " + pkgName + " ="
                + observer_delete);

    }

    class PackageDeleteObserver extends IPackageDeleteObserver.Stub
    {

        @Override
        // modify by fleety jie.liu 20161019 start
        //public void packageDeleted(boolean returnCode) throws RemoteException
        public void packageDeleted(String packageName, int returnCode) throws RemoteException
        // modify by fleety jie.liu 20161019 end 
        {
            System.out.println(getClass().getSimpleName() + "-> "
                    + "PackageDeleteObserver packageDeleted pkg=" + pkgName);
            System.out.println(getClass().getSimpleName() + "-> "
                    + " returnCode=" + returnCode);
            Intent intent = new Intent();
            intent.setAction(R600UtilStrings.R600UTIL_SEND_UNINSTALL_ACTION);
            Bundle data = new Bundle();
            // modify by fleety jie.liu 20161019 start
            //data.putBoolean(R600UtilStrings.R600UTIL_DATA_UNINSTALL_PACKAGE_RESULT, returnCode);
            if(returnCode == 1){
               data.putBoolean(R600UtilStrings.R600UTIL_DATA_UNINSTALL_PACKAGE_RESULT, true);
            }
            else
            {
               data.putBoolean(R600UtilStrings.R600UTIL_DATA_UNINSTALL_PACKAGE_RESULT, false);
            } 
            // modify by fleety jie.liu 20161019 end 
            intent.putExtras(data);
            sendBroadcast(intent);
        }

    }

}
