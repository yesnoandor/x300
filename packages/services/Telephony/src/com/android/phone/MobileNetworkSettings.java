/*
 * Copyright (C) 2006 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.phone;

import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.TelephonyIntents;
import com.android.internal.telephony.TelephonyProperties;
import com.android.internal.telephony.uicc.UiccController;

import android.app.ActionBar;
import android.app.AlertDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager.NameNotFoundException;
import android.net.ConnectivityManager;
import android.net.Uri;
import android.os.AsyncResult;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.SystemProperties;
import android.preference.CheckBoxPreference;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceScreen;
import android.provider.Settings.SettingNotFoundException;
import android.provider.Settings;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.util.Log;
import android.view.MenuItem;

import com.android.internal.telephony.RILConstants;
import com.android.internal.telephony.uicc.IccCardApplicationStatus.AppType;

import static com.android.internal.telephony.MSimConstants.SUB1;

import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;

import android.util.CommonFunction;


/**
 * "Mobile network settings" screen.  This preference screen lets you
 * enable/disable mobile data, and control data roaming and other
 * network-specific mobile data features.  It's used on non-voice-capable
 * tablets as well as regular phone devices.
 *
 * Note that this PreferenceActivity is part of the phone app, even though
 * you reach it from the "Wireless & Networks" section of the main
 * Settings app.  It's not part of the "Call settings" hierarchy that's
 * available from the Phone app (see CallFeaturesSetting for that.)
 */
public class MobileNetworkSettings extends PreferenceActivity
        implements DialogInterface.OnClickListener,
        DialogInterface.OnDismissListener, Preference.OnPreferenceChangeListener{

    // debug data
    private static final String LOG_TAG = "NetworkSettings";
    private static final boolean DBG = true;
    public static final int REQUEST_CODE_EXIT_ECM = 17;

    //String keys for preference lookup
    private static final String BUTTON_DATA_ENABLED_KEY = "button_data_enabled_key";
    private static final String BUTTON_PREFERED_NETWORK_MODE = "preferred_network_mode_key";
    private static final String BUTTON_ROAMING_KEY = "button_roaming_key";
    private static final String BUTTON_CDMA_LTE_DATA_SERVICE_KEY = "cdma_lte_data_service_key";
    private static final String BUTTON_UPLMN_KEY = "button_uplmn_key";
    private static final String BUTTON_ENABLED_NETWORKS_KEY = "enabled_networks_key";
    private static final String BUTTON_CARRIER_SETTINGS_KEY = "carrier_settings_key";
    private static final String KEY_PREFERRED_LTE = "toggle_preferred_lte";

    static final int preferredNetworkMode = Phone.PREFERRED_NT_MODE;

    //Information about logical "up" Activity
    private static final String UP_ACTIVITY_PACKAGE = "com.android.settings";
    private static final String UP_ACTIVITY_CLASS =
            "com.android.settings.Settings$WirelessSettingsActivity";
            
    /* zhangzhaolei add for modemConfig 20150717 start */     
    //ModemConfig settings
    private static final String MODEM_CONFIG_PACKAGE = "com.erobbing.modemconfig";
    private static final String KEY_PREF_NETWORK_CHOOSE = "network_choose";
    /* zhangzhaolei add for modemConfig 20150717 end */

    //UI objects
    private ListPreference mButtonPreferredNetworkMode;
    private ListPreference mButtonEnabledNetworks;
    private CheckBoxPreference mButtonDataRoam;
    private CheckBoxPreference mButtonDataEnabled;
    private Preference mLteDataServicePref;
    private CheckBoxPreference mButtonPreferredLte;

    private static final String iface = "rmnet0"; //TODO: this will go away

    private Phone mPhone;
    private MyHandler mHandler;
    private boolean mOkClicked;

    //GsmUmts options and Cdma options
    GsmUmtsOptions mGsmUmtsOptions;
    CdmaOptions mCdmaOptions;

    private Preference mClickedPreference;
    private boolean mShow4GForLTE;
    private boolean mIsGlobalCdma;

    private static final String NETWORK_MODE_SEPARATOR = "-";
    //This is a method implemented for DialogInterface.OnClickListener.
    //  Used to dismiss the dialogs when they come up.
    public void onClick(DialogInterface dialog, int which) {
        if (which == DialogInterface.BUTTON_POSITIVE) {
            mPhone.setDataRoamingEnabled(true);
            mOkClicked = true;
        } else {
            // Reset the toggle
            mButtonDataRoam.setChecked(false);
        }
    }

    public void onDismiss(DialogInterface dialog) {
        // Assuming that onClick gets called first
        if (!mOkClicked) {
            mButtonDataRoam.setChecked(false);
        }
    }

    /**
     * Invoked on each preference click in this hierarchy, overrides
     * PreferenceActivity's implementation.  Used to make sure we track the
     * preference click events.
     */
    @Override
    public boolean onPreferenceTreeClick(PreferenceScreen preferenceScreen, Preference preference) {
    	log(CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::+++++++++++++++");

        /** TODO: Refactor and get rid of the if's using subclasses */
        if (mGsmUmtsOptions != null &&
                mGsmUmtsOptions.preferenceTreeClick(preference) == true) {
            return true;
        } else if (mCdmaOptions != null &&
                   mCdmaOptions.preferenceTreeClick(preference) == true) {
            if (Boolean.parseBoolean(
                    SystemProperties.get(TelephonyProperties.PROPERTY_INECM_MODE))) {

                mClickedPreference = preference;

                // In ECM mode launch ECM app dialog
                startActivityForResult(
                    new Intent(TelephonyIntents.ACTION_SHOW_NOTICE_ECM_BLOCK_OTHERS, null),
                    REQUEST_CODE_EXIT_ECM);
            }
            return true;
        } else if (preference == mButtonPreferredNetworkMode) {
            //displays the value taken from the Settings.System
            int settingsNetworkMode = android.provider.Settings.Global.getInt(mPhone.getContext().
                    getContentResolver(), android.provider.Settings.Global.PREFERRED_NETWORK_MODE,
                    preferredNetworkMode);
            setPreferredNetworkModeValue(settingsNetworkMode);
            return true;
        } else if (preference == mButtonDataRoam) {
            if (DBG) log("onPreferenceTreeClick: preference == mButtonDataRoam.");

            //normally called on the toggle click
            if (mButtonDataRoam.isChecked()) {
                // First confirm with a warning dialog about charges
                mOkClicked = false;
                new AlertDialog.Builder(this).setMessage(
                        getResources().getString(R.string.roaming_warning))
                        .setTitle(android.R.string.dialog_alert_title)
                        .setIconAttribute(android.R.attr.alertDialogIcon)
                        .setPositiveButton(android.R.string.yes, this)
                        .setNegativeButton(android.R.string.no, this)
                        .show()
                        .setOnDismissListener(this);
            } else {
                mPhone.setDataRoamingEnabled(false);
            }
            return true;
        } else if (preference == mButtonDataEnabled) {
            if (DBG) log("onPreferenceTreeClick: preference == mButtonDataEnabled.");
            ConnectivityManager cm =
                    (ConnectivityManager)getSystemService(Context.CONNECTIVITY_SERVICE);

            cm.setMobileDataEnabled(mButtonDataEnabled.isChecked());
            return true;
        } else if (preference == mLteDataServicePref) {
            String tmpl = android.provider.Settings.Global.getString(getContentResolver(),
                        android.provider.Settings.Global.SETUP_PREPAID_DATA_SERVICE_URL);
            if (!TextUtils.isEmpty(tmpl)) {
                TelephonyManager tm = (TelephonyManager) getSystemService(
                        Context.TELEPHONY_SERVICE);
                String imsi = tm.getSubscriberId();
                if (imsi == null) {
                    imsi = "";
                }
                final String url = TextUtils.isEmpty(tmpl) ? null
                        : TextUtils.expandTemplate(tmpl, imsi).toString();
                Intent intent = new Intent(Intent.ACTION_VIEW, Uri.parse(url));
                startActivity(intent);
            } else {
                android.util.Log.e(LOG_TAG, "Missing SETUP_PREPAID_DATA_SERVICE_URL");
            }
            return true;
        }  else if (preference == mButtonEnabledNetworks) {
            int settingsNetworkMode = android.provider.Settings.Global.getInt(mPhone.getContext().
                    getContentResolver(), android.provider.Settings.Global.PREFERRED_NETWORK_MODE,
                    preferredNetworkMode);
            mButtonEnabledNetworks.setValue(Integer.toString(settingsNetworkMode));
            return true;
        } else if (preference == mButtonPreferredLte) {
            final Message msg = mHandler.obtainMessage(MyHandler.MESSAGE_SET_PREFERRED_LTE);

            if (PhoneGlobals.getInstance().mPhoneServiceClient != null) {
                PhoneGlobals.getInstance().setTDDDataOnly(0, mButtonPreferredLte.isChecked(),
                        msg);
            }
            return true;
        } else {
            // if the button is anything but the simple toggle preference,
            // we'll need to disable all preferences to reject all click
            // events until the sub-activity's UI comes up.
            preferenceScreen.setEnabled(false);
            // Let the intents be launched by the Preference manager
            return false;
        }
		
		//log(CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::---------------");
    }

    @Override
    protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);

        addPreferencesFromResource(R.xml.network_setting);

        mPhone = PhoneGlobals.getPhone();
        mHandler = new MyHandler();

        try {
            Context con = createPackageContext("com.android.systemui", 0);
            int id = con.getResources().getIdentifier("config_show4GForLTE",
                    "bool", "com.android.systemui");
            mShow4GForLTE = con.getResources().getBoolean(id);
        } catch (NameNotFoundException e) {
            loge("NameNotFoundException for show4GFotLTE");
            mShow4GForLTE = false;
        }

        //Register for intent broadcasts
        IntentFilter intentFilter = new IntentFilter(Intent.ACTION_AIRPLANE_MODE_CHANGED);
        intentFilter.addAction(TelephonyIntents.ACTION_SIM_STATE_CHANGED);

        registerReceiver(mReceiver, intentFilter);

        //get UI object references
        PreferenceScreen prefSet = getPreferenceScreen();

        mButtonDataEnabled = (CheckBoxPreference) prefSet.findPreference(BUTTON_DATA_ENABLED_KEY);
        //Move data enable button to setting root screen
        prefSet.removePreference(mButtonDataEnabled);
        mButtonDataRoam = (CheckBoxPreference) prefSet.findPreference(BUTTON_ROAMING_KEY);
        mButtonPreferredNetworkMode = (ListPreference) prefSet.findPreference(
                BUTTON_PREFERED_NETWORK_MODE);
        mButtonEnabledNetworks = (ListPreference) prefSet.findPreference(
                BUTTON_ENABLED_NETWORKS_KEY);

        mLteDataServicePref = prefSet.findPreference(BUTTON_CDMA_LTE_DATA_SERVICE_KEY);

        mButtonPreferredLte = (CheckBoxPreference) prefSet.findPreference(KEY_PREFERRED_LTE);
        if (!getResources().getBoolean(R.bool.config_tdd_data_only)) {
            prefSet.removePreference(mButtonPreferredLte);
            mButtonPreferredLte = null;
        }

        /* zhangzhaolei add for modemConfig 20150717 start */
        if(!checkApkExist(mPhone.getContext(), MODEM_CONFIG_PACKAGE)) {
            prefSet.removePreference(prefSet.findPreference(KEY_PREF_NETWORK_CHOOSE));
        }
        /* zhangzhaolei add for modemConfig 20150717 end */
        int networkFeature = SystemProperties.getInt("persist.radio.network_feature",
                Constants.NETWORK_MODE_DEFAULT);
        switch (networkFeature) {
            case Constants.NETWORK_MODE_HIDE:
                prefSet.removePreference(mButtonPreferredNetworkMode);
                break;
            case Constants.NETWORK_MODE_CMCC:
                mButtonPreferredNetworkMode
                        .setDialogTitle(R.string.preferred_network_mode_dialogtitle_cmcc);
                if (getResources().getBoolean(R.bool.config_network_cmcc_feature)) {
                    if (UiccController.getInstance().getUiccCard() != null &&
                            UiccController.getInstance().getUiccCard()
                                    .isApplicationOnIcc(AppType.APPTYPE_USIM)) {
                        mButtonPreferredNetworkMode.setEntries(
                                R.array.preferred_network_mode_options_cmcc);
                        mButtonPreferredNetworkMode.setEntryValues(
                                R.array.preferred_network_mode_options_values_cmcc);
                    } else {
                        prefSet.removePreference(mButtonPreferredNetworkMode);
                    }
                } else {
                    mButtonPreferredNetworkMode.setEntries(
                            R.array.preferred_network_mode_choices_cmcc);
                    mButtonPreferredNetworkMode.setEntryValues(
                            R.array.preferred_network_mode_values_cmcc);
                }
                break;
            case Constants.NETWORK_MODE_TDCDMA:
                mButtonPreferredNetworkMode
                        .setEntries(R.array.preferred_network_mode_choices_tdscdma);
                mButtonPreferredNetworkMode
                        .setEntryValues(R.array.preferred_network_mode_values_tdscdma);
                break;
            case Constants.NETWORK_MODE_LTE:
                mButtonPreferredNetworkMode.setEntries(R.array.preferred_network_mode_choices_lte);
                mButtonPreferredNetworkMode
                        .setEntryValues(R.array.preferred_network_mode_values_lte);
                break;
            case Constants.NETWORK_MODE_DEFAULT:
            default:
                break;
        }

        if (!getResources().getBoolean(R.bool.config_uplmn_for_cta_test)) {
            Preference mUPLMNPref = prefSet.findPreference(BUTTON_UPLMN_KEY);
            prefSet.removePreference(mUPLMNPref);
            mUPLMNPref = null;
        }

        boolean isLteOnCdma = mPhone.getLteOnCdmaMode() == PhoneConstants.LTE_ON_CDMA_TRUE;
        mIsGlobalCdma = isLteOnCdma && getResources().getBoolean(R.bool.config_show_cdma);
        //mButtonEnabledNetworks is not needed anymore
        prefSet.removePreference(mButtonEnabledNetworks);
        if (getResources().getBoolean(R.bool.world_phone) == true) {
            // set the listener for the mButtonPreferredNetworkMode list preference so we can issue
            // change Preferred Network Mode.
            mButtonPreferredNetworkMode.setOnPreferenceChangeListener(this);

            //Get the networkMode from Settings.System and displays it
            int settingsNetworkMode = android.provider.Settings.Global.getInt(mPhone.getContext().
                    getContentResolver(),android.provider.Settings.Global.PREFERRED_NETWORK_MODE,
                    preferredNetworkMode);
            setPreferredNetworkModeValue(settingsNetworkMode);
            mCdmaOptions = new CdmaOptions(this, prefSet, mPhone);
            mGsmUmtsOptions = new GsmUmtsOptions(this, prefSet);
        } else {
            if (!isLteOnCdma) {
                prefSet.removePreference(mButtonPreferredNetworkMode);
            } else {
                mButtonPreferredNetworkMode.setOnPreferenceChangeListener(this);
                    int settingsNetworkMode = android.provider.Settings.Global.getInt(
                            mPhone.getContext().getContentResolver(),
                            android.provider.Settings.Global.PREFERRED_NETWORK_MODE,
                            preferredNetworkMode);
                    setPreferredNetworkModeValue(settingsNetworkMode);
            }
            int phoneType = mPhone.getPhoneType();
            if (phoneType == PhoneConstants.PHONE_TYPE_CDMA) {
                if (isLteOnCdma) {
                    mButtonEnabledNetworks.setEntries(
                            R.array.enabled_networks_cdma_choices);
                    mButtonEnabledNetworks.setEntryValues(
                            R.array.enabled_networks_cdma_values);
                }
                mCdmaOptions = new CdmaOptions(this, prefSet, mPhone);
            } else if (phoneType == PhoneConstants.PHONE_TYPE_GSM) {
                if (!getResources().getBoolean(R.bool.config_prefer_2g)
                        && !getResources().getBoolean(R.bool.config_enabled_lte)) {
                    mButtonEnabledNetworks.setEntries(
                            R.array.enabled_networks_except_gsm_lte_choices);
                    mButtonEnabledNetworks.setEntryValues(
                            R.array.enabled_networks_except_gsm_lte_values);
                } else if (!getResources().getBoolean(R.bool.config_prefer_2g)) {
                    int select = (mShow4GForLTE == true) ?
                        R.array.enabled_networks_except_gsm_4g_choices
                        : R.array.enabled_networks_except_gsm_choices;
                    mButtonEnabledNetworks.setEntries(select);
                    mButtonEnabledNetworks.setEntryValues(
                            R.array.enabled_networks_except_gsm_values);
                } else if (!getResources().getBoolean(R.bool.config_enabled_lte)) {
                    mButtonEnabledNetworks.setEntries(
                            R.array.enabled_networks_except_lte_choices);
                    mButtonEnabledNetworks.setEntryValues(
                            R.array.enabled_networks_except_lte_values);
                } else if (mIsGlobalCdma) {
                    mButtonEnabledNetworks.setEntries(
                            R.array.enabled_networks_cdma_choices);
                    mButtonEnabledNetworks.setEntryValues(
                            R.array.enabled_networks_cdma_values);
                } else {
                    int select = (mShow4GForLTE == true) ? R.array.enabled_networks_4g_choices
                        : R.array.enabled_networks_choices;
                    mButtonEnabledNetworks.setEntries(select);
                    mButtonEnabledNetworks.setEntryValues(
                            R.array.enabled_networks_values);
                }
                mGsmUmtsOptions = new GsmUmtsOptions(this, prefSet);
            } else {
                throw new IllegalStateException("Unexpected phone type: " + phoneType);
            }
            mButtonEnabledNetworks.setOnPreferenceChangeListener(this);
            int settingsNetworkMode = android.provider.Settings.Global.getInt(
                    mPhone.getContext().getContentResolver(),
                    android.provider.Settings.Global.PREFERRED_NETWORK_MODE,
                    preferredNetworkMode);
            if (DBG) log("settingsNetworkMode: " + settingsNetworkMode);
            mButtonEnabledNetworks.setValue(Integer.toString(settingsNetworkMode));
        }

        final boolean missingDataServiceUrl = TextUtils.isEmpty(
                android.provider.Settings.Global.getString(getContentResolver(),
                        android.provider.Settings.Global.SETUP_PREPAID_DATA_SERVICE_URL));
        if (!isLteOnCdma || missingDataServiceUrl) {
            prefSet.removePreference(mLteDataServicePref);
        } else {
            android.util.Log.d(LOG_TAG, "keep ltePref");
        }

        // Read platform settings for carrier settings
        final boolean isCarrierSettingsEnabled = getResources().getBoolean(
                R.bool.config_carrier_settings_enable);
        if (!isCarrierSettingsEnabled) {
            Preference pref = prefSet.findPreference(BUTTON_CARRIER_SETTINGS_KEY);
            if (pref != null) {
                prefSet.removePreference(pref);
                // Some times carrier settings added multiple times(ex: for world mode)
                // so, remove carrier settings if there a second one exists.
                pref = prefSet.findPreference(BUTTON_CARRIER_SETTINGS_KEY);
                if (pref != null) {
                    prefSet.removePreference(pref);
                }
            }
        }

        ActionBar actionBar = getActionBar();
        if (actionBar != null) {
            // android.R.id.home will be triggered in onOptionsItemSelected()
            actionBar.setDisplayHomeAsUpEnabled(true);
        }

        if (this.getResources().getBoolean(R.bool.hide_roaming)) {
            prefSet.removePreference(mButtonDataRoam);
        }
		
		/*
        //lidongdong remove Network Choose 20160914 start
        if (mButtonPreferredNetworkMode != null) {
            prefSet.removePreference(mButtonPreferredNetworkMode);
        }
        //lidongdong remove Network Choose 20160914 end
		*/
    }

    private void updateButtonPreferredLte() {
        if (mButtonPreferredLte == null) {
            return;
        }
        try {
            mButtonPreferredLte.setEnabled(PhoneUtils.isLTE(Settings.Global.getInt(getContentResolver(),
                    Settings.Global.PREFERRED_NETWORK_MODE)));
            mButtonPreferredLte.setChecked(Settings.Global.getInt(getContentResolver(),
                    Constants.SETTING_TDD_DATA_ONLY_USER_REF) == Constants.SETTING_ON);
        } catch (SettingNotFoundException e) {
            Log.d(LOG_TAG, "failed to update lte button", e);
        }
    }
    
    /* zhangzhaolei add for modemConfig 20150717 start */
    public boolean checkApkExist(Context context, String packageName) {
        if (packageName == null || "".equals(packageName)) {
            return false;
        }
        try {
            ApplicationInfo info = context.getPackageManager()
                                        .getApplicationInfo(packageName,
                                         PackageManager.GET_UNINSTALLED_PACKAGES);
            return true;
        } catch (Exception e) {
            return false;
        }
    }
    /* zhangzhaolei add for modemConfig 20150717 end */

    private int getAcqValue() {
        int acq = 0;
		
		log(CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::+++++++++++++++");

        try {
            acq = Settings.Global.getInt(getContentResolver(), Constants.SETTINGS_ACQ);
        } catch (SettingNotFoundException e) {
            Log.d(LOG_TAG, "failed to get acq", e);
        }
		
		log(CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::---------------");

        return acq;
    }

    private void setPreferredNetworkModeValue(int settingsNetworkMode) {
        int networkFeature = SystemProperties.getInt("persist.radio.network_feature",
                Constants.NETWORK_MODE_DEFAULT);

		
		log(CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::+++++++++++++++");

		
		log("settingsNetworkMode = " + settingsNetworkMode);
		log("networkFeature = " + networkFeature);

        //only these cases need set acq
        if ((networkFeature == Constants.NETWORK_MODE_CMCC
                || networkFeature == Constants.NETWORK_MODE_LTE)
                && (settingsNetworkMode == RILConstants.NETWORK_MODE_TD_SCDMA_GSM_WCDMA_LTE)) {
            if (networkFeature == Constants.NETWORK_MODE_CMCC
                    && getResources().getBoolean(R.bool.config_network_cmcc_feature)) {
                mButtonPreferredNetworkMode.setValue(Integer.toString(settingsNetworkMode));
            } else {
                // default is 4G preferred mode
                int acq = getAcqValue();
                String acqString = (0 == acq) ? "1" : Integer.toString(acq);
                String networkmodeString = Integer.toString(settingsNetworkMode)
                        + NETWORK_MODE_SEPARATOR + acqString;
                Log.d(LOG_TAG, networkmodeString);
				log("networkmodeString = " + networkmodeString);
                mButtonPreferredNetworkMode.setValue(networkmodeString);
            }
        } else {
            mButtonPreferredNetworkMode.setValue(Integer.toString(settingsNetworkMode));
        }

		
		log(CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::---------------");
    }

    @Override
    protected void onResume() {
        super.onResume();

        // upon resumption from the sub-activity, make sure we re-enable the
        // preferences.
        setScreenState();

        // Set UI state in onResume because a user could go home, launch some
        // app to change this setting's backend, and re-launch this settings app
        // and the UI state would be inconsistent with actual state
        mButtonDataRoam.setChecked(mPhone.getDataRoamingEnabled());

        updateButtonPreferredLte();

        if (getPreferenceScreen().findPreference(BUTTON_PREFERED_NETWORK_MODE) != null)  {
            mPhone.getPreferredNetworkType(mHandler.obtainMessage(
                    MyHandler.MESSAGE_GET_PREFERRED_NETWORK_TYPE));
        }

        if (getPreferenceScreen().findPreference(BUTTON_ENABLED_NETWORKS_KEY) != null)  {
            mPhone.getPreferredNetworkType(mHandler.obtainMessage(
                    MyHandler.MESSAGE_GET_PREFERRED_NETWORK_TYPE));
        }
    }

    private void setScreenState() {
        int simState = TelephonyManager.getDefault().getSimState();
        getPreferenceScreen().setEnabled(simState != TelephonyManager.SIM_STATE_ABSENT);
    }

    /**
     * Receiver for ACTION_AIRPLANE_MODE_CHANGED and ACTION_SIM_STATE_CHANGED.
     */
    private BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
			log(CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::+++++++++++++++");
		
            String action = intent.getAction();

            if (action.equals(Intent.ACTION_AIRPLANE_MODE_CHANGED) ||
                    action.equals(TelephonyIntents.ACTION_SIM_STATE_CHANGED)) {
                setScreenState();
            }

			if(action.equals(Intent.ACTION_AIRPLANE_MODE_CHANGED)){
				int settingsNetworkMode = android.provider.Settings.Global.getInt(mPhone.getContext().
                    getContentResolver(), android.provider.Settings.Global.PREFERRED_NETWORK_MODE,
                    preferredNetworkMode);

				log("settingsNetworkMode = " + settingsNetworkMode);

				//Set the modem network mode
                mPhone.setPreferredNetworkType(settingsNetworkMode, mHandler
                        .obtainMessage(MyHandler.MESSAGE_SET_PREFERRED_NETWORK_TYPE));
			}

			
			
			log(CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::---------------");
        }
    };


    @Override
    protected void onPause() {
        super.onPause();
    }

    /**
     * Implemented to support onPreferenceChangeListener to look for preference
     * changes specifically on CLIR.
     *
     * @param preference is the preference to be changed, should be mButtonCLIR.
     * @param objValue should be the value of the selection, NOT its localized
     * display value.
     */
    public boolean onPreferenceChange(Preference preference, Object objValue) {    	
		log(CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::+++++++++++++++");

        if (preference == mButtonPreferredNetworkMode) {
            String strMode = (String) objValue;
            String strAcq =  "0";
            boolean isContainAcq = strMode.contains(NETWORK_MODE_SEPARATOR);
            if (isContainAcq) {
                String[] values = strMode.split(NETWORK_MODE_SEPARATOR);
                strMode = values[0];
                strAcq = values[1];
            }
            //NOTE onPreferenceChange seems to be called even if there is no change
            //Check if the button value is changed from the System.Setting
            mButtonPreferredNetworkMode.setValue((String) objValue);
            int buttonNetworkMode;
            buttonNetworkMode = Integer.valueOf((String) strMode).intValue();
            int settingsNetworkMode = android.provider.Settings.Global.getInt(
                    mPhone.getContext().getContentResolver(),
                    android.provider.Settings.Global.PREFERRED_NETWORK_MODE, preferredNetworkMode);
            int buttonAcq = Integer.valueOf((String) strAcq).intValue();
            int settingsAcq = getAcqValue();

			log("buttonNetworkMode = " + buttonNetworkMode);
			log("settingsNetworkMode = " + settingsNetworkMode);
			log("buttonAcq = " + buttonAcq);
			log("settingsAcq = " + settingsAcq);
			
            if (buttonNetworkMode != settingsNetworkMode || buttonAcq != settingsAcq) {
                int modemNetworkMode;
                // if new mode is invalid ignore it
                switch (buttonNetworkMode) {
                    case Phone.NT_MODE_WCDMA_PREF:
                    case Phone.NT_MODE_GSM_ONLY:
                    case Phone.NT_MODE_WCDMA_ONLY:
                    case Phone.NT_MODE_GSM_UMTS:
                    case Phone.NT_MODE_CDMA:
                    case Phone.NT_MODE_CDMA_NO_EVDO:
                    case Phone.NT_MODE_EVDO_NO_CDMA:
                    case Phone.NT_MODE_GLOBAL:
                    case Phone.NT_MODE_LTE_CDMA_AND_EVDO:
                    case Phone.NT_MODE_LTE_GSM_WCDMA:
                    case Phone.NT_MODE_LTE_CMDA_EVDO_GSM_WCDMA:
                    case Phone.NT_MODE_LTE_ONLY:
                    case Phone.NT_MODE_LTE_WCDMA:
                    case Phone.NT_MODE_TD_SCDMA_ONLY:
                    case Phone.NT_MODE_TD_SCDMA_WCDMA:
                    case Phone.NT_MODE_TD_SCDMA_LTE:
                    case Phone.NT_MODE_TD_SCDMA_GSM:
                    case Phone.NT_MODE_TD_SCDMA_GSM_LTE:
                    case Phone.NT_MODE_TD_SCDMA_GSM_WCDMA:
                    case Phone.NT_MODE_TD_SCDMA_WCDMA_LTE:
                    case Phone.NT_MODE_TD_SCDMA_GSM_WCDMA_LTE:
                    case Phone.NT_MODE_TD_SCDMA_CDMA_EVDO_GSM_WCDMA:
                    case Phone.NT_MODE_TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA:
                    case Phone.NT_MODE_LTE_CDMA_EVDO_GSM:
                        // This is one of the modes we recognize
                        modemNetworkMode = buttonNetworkMode;
                        break;
                    default:
                        loge("Invalid Network Mode (" + buttonNetworkMode + ") chosen. Ignore.");
                        return true;
                }

                UpdatePreferredNetworkModeSummary(buttonNetworkMode, buttonAcq);

                android.provider.Settings.Global.putInt(mPhone.getContext().getContentResolver(),
                        android.provider.Settings.Global.PREFERRED_NETWORK_MODE,
                        buttonNetworkMode );
                //Set the modem network mode
                // mPhone.setPreferredNetworkType(modemNetworkMode, mHandler
                //        .obtainMessage(MyHandler.MESSAGE_SET_PREFERRED_NETWORK_TYPE));
                setPreferredNetworkType(isContainAcq, modemNetworkMode, strAcq);
            }
        } else if (preference == mButtonEnabledNetworks) {
            mButtonEnabledNetworks.setValue((String) objValue);
            int buttonNetworkMode;
            buttonNetworkMode = Integer.valueOf((String) objValue).intValue();
            if (DBG) log("buttonNetworkMode: " + buttonNetworkMode);

			log("buttonNetworkMode" + buttonNetworkMode);
			
            int settingsNetworkMode = android.provider.Settings.Global.getInt(
                    mPhone.getContext().getContentResolver(),
                    android.provider.Settings.Global.PREFERRED_NETWORK_MODE, preferredNetworkMode);

			
			log("settingsNetworkMode" + settingsNetworkMode);

            if (buttonNetworkMode != settingsNetworkMode) {
                int modemNetworkMode;
                // if new mode is invalid ignore it
                switch (buttonNetworkMode) {
                    case Phone.NT_MODE_WCDMA_PREF:
                    case Phone.NT_MODE_GSM_ONLY:
                    case Phone.NT_MODE_LTE_GSM_WCDMA:
                    case Phone.NT_MODE_LTE_CMDA_EVDO_GSM_WCDMA:
                    case Phone.NT_MODE_CDMA:
                    case Phone.NT_MODE_CDMA_NO_EVDO:
                    case Phone.NT_MODE_LTE_CDMA_AND_EVDO:
                    case Phone.NT_MODE_LTE_CDMA_EVDO_GSM:
                        // This is one of the modes we recognize
                        modemNetworkMode = buttonNetworkMode;
                        break;
                    default:
                        loge("Invalid Network Mode (" + buttonNetworkMode + ") chosen. Ignore.");
                        return true;
                }

                UpdateEnabledNetworksValueAndSummary(buttonNetworkMode);

                android.provider.Settings.Global.putInt(mPhone.getContext().getContentResolver(),
                        android.provider.Settings.Global.PREFERRED_NETWORK_MODE,
                        buttonNetworkMode );
                //Set the modem network mode
                mPhone.setPreferredNetworkType(modemNetworkMode, mHandler
                        .obtainMessage(MyHandler.MESSAGE_SET_PREFERRED_NETWORK_TYPE));
            }
        }

		log(CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::---------------");

        // always let the preference setting proceed.
        return true;
    }

    // now use phone feature service to set network mode
    private void setPreferredNetworkType(boolean containAcq, int networkMode, String strAcq) {
        // now use phone feature service to set network mode
        final int modemNetworkMode = networkMode;
        final Message msg;
		
		log(CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::+++++++++++++++");

		log("containAcq = " + containAcq);
		log("networkMode = " + modemNetworkMode);
		log("strAcq = " + strAcq);

        if (PhoneGlobals.getInstance().mPhoneServiceClient != null) {
            msg = mHandler.obtainMessage(
                MyHandler.MESSAGE_SET_PREFERRED_NETWORK_TYPE_WITH_ACQ);
        } else {
            msg = mHandler.obtainMessage(
                MyHandler.MESSAGE_SET_PREFERRED_NETWORK_TYPE);
        }
        if (containAcq) {
            final int acq = Integer.valueOf(strAcq).intValue();
            if (PhoneGlobals.getInstance().mPhoneServiceClient != null) {
                PhoneGlobals.getInstance()
                        .setPrefNetworWithAcq(0, modemNetworkMode, acq, msg);
            } else {
                // Set the modem network mode
                mPhone.setPreferredNetworkType(modemNetworkMode,
                        mHandler.obtainMessage(
                                MyHandler.MESSAGE_SET_PREFERRED_NETWORK_TYPE));
            }
        } else {
            if (PhoneGlobals.getInstance().mPhoneServiceClient != null) {
                PhoneGlobals.getInstance().setPrefNetwork(0, modemNetworkMode, msg);
            } else {
                // Set the modem network mode
                mPhone.setPreferredNetworkType(modemNetworkMode,
                        mHandler.obtainMessage(MyHandler.MESSAGE_SET_PREFERRED_NETWORK_TYPE));
            }
        }
		
		log(CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::----------");
    }

    private class MyHandler extends Handler {

        static final int MESSAGE_GET_PREFERRED_NETWORK_TYPE = 0;
        static final int MESSAGE_SET_PREFERRED_NETWORK_TYPE = 1;
        static final int MESSAGE_SET_PREFERRED_NETWORK_TYPE_WITH_ACQ = 2;
        static final int MESSAGE_SET_PREFERRED_LTE = 3;

        @Override
        public void handleMessage(Message msg) {
        	
			log(CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::+++++++++++++++");
			
			log("msg.what = " + msg.what);
			
			switch (msg.what) {
                case MESSAGE_GET_PREFERRED_NETWORK_TYPE:
                    handleGetPreferredNetworkTypeResponse(msg);
                    break;

                case MESSAGE_SET_PREFERRED_NETWORK_TYPE:
                    handleSetPreferredNetworkTypeResponse(msg);
                    break;
                case MESSAGE_SET_PREFERRED_NETWORK_TYPE_WITH_ACQ:
                    handleSetPreferredNetworkTypeWithAcqResponse();
                    break;
                case MESSAGE_SET_PREFERRED_LTE:
                    handleSetPreferredLTEResponse();
            }

			
			log(CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::---------------");
        }

        private void handleSetPreferredLTEResponse() {
            updateButtonPreferredLte();
        }

        private void handleSetPreferredNetworkTypeWithAcqResponse() {
            if (mButtonPreferredNetworkMode != null) {
                UpdatePreferredNetworkModeSummary(android.provider.Settings.Global
                        .getInt(mPhone.getContext().getContentResolver(),
                        android.provider.Settings.Global.PREFERRED_NETWORK_MODE,
                        preferredNetworkMode), getAcqValue());
            }
            updateButtonPreferredLte();
        }

        private void handleGetPreferredNetworkTypeResponse(Message msg) {
            AsyncResult ar = (AsyncResult) msg.obj;

            if (ar.exception == null) {
                int modemNetworkMode = ((int[])ar.result)[0];

                if (DBG) {
                    log ("handleGetPreferredNetworkTypeResponse: modemNetworkMode = " +
                            modemNetworkMode);
                }

                int settingsNetworkMode = android.provider.Settings.Global.getInt(
                        mPhone.getContext().getContentResolver(),
                        android.provider.Settings.Global.PREFERRED_NETWORK_MODE,
                        preferredNetworkMode);

                if (DBG) {
                    log("handleGetPreferredNetworkTypeReponse: settingsNetworkMode = " +
                            settingsNetworkMode);
                }

                //check that modemNetworkMode is from an accepted value
                if (modemNetworkMode == Phone.NT_MODE_WCDMA_PREF ||
                        modemNetworkMode == Phone.NT_MODE_GSM_ONLY ||
                        modemNetworkMode == Phone.NT_MODE_WCDMA_ONLY ||
                        modemNetworkMode == Phone.NT_MODE_GSM_UMTS ||
                        modemNetworkMode == Phone.NT_MODE_CDMA ||
                        modemNetworkMode == Phone.NT_MODE_CDMA_NO_EVDO ||
                        modemNetworkMode == Phone.NT_MODE_EVDO_NO_CDMA ||
                        modemNetworkMode == Phone.NT_MODE_GLOBAL ||
                        modemNetworkMode == Phone.NT_MODE_LTE_CDMA_AND_EVDO ||
                        modemNetworkMode == Phone.NT_MODE_LTE_GSM_WCDMA ||
                        modemNetworkMode == Phone.NT_MODE_LTE_CMDA_EVDO_GSM_WCDMA ||
                        modemNetworkMode == Phone.NT_MODE_LTE_ONLY ||
                        modemNetworkMode == Phone.NT_MODE_LTE_WCDMA ||
                        modemNetworkMode == Phone.NT_MODE_TD_SCDMA_ONLY ||
                        modemNetworkMode == Phone.NT_MODE_TD_SCDMA_WCDMA ||
                        modemNetworkMode == Phone.NT_MODE_TD_SCDMA_LTE ||
                        modemNetworkMode == Phone.NT_MODE_TD_SCDMA_GSM ||
                        modemNetworkMode == Phone.NT_MODE_TD_SCDMA_GSM_LTE ||
                        modemNetworkMode == Phone.NT_MODE_TD_SCDMA_GSM_WCDMA ||
                        modemNetworkMode == Phone.NT_MODE_TD_SCDMA_WCDMA_LTE ||
                        modemNetworkMode == Phone.NT_MODE_TD_SCDMA_GSM_WCDMA_LTE ||
                        modemNetworkMode == Phone.NT_MODE_TD_SCDMA_CDMA_EVDO_GSM_WCDMA ||
                        modemNetworkMode == Phone.NT_MODE_TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA ||
                        modemNetworkMode == Phone.NT_MODE_LTE_CDMA_EVDO_GSM) {
                    if (DBG) {
                        log("handleGetPreferredNetworkTypeResponse: if 1: modemNetworkMode = " +
                                modemNetworkMode);
                    }

                    //check changes in modemNetworkMode and updates settingsNetworkMode
                    if (modemNetworkMode != settingsNetworkMode) {
                        if (DBG) {
                            log("handleGetPreferredNetworkTypeResponse: if 2: " +
                                    "modemNetworkMode != settingsNetworkMode");
                        }

                        settingsNetworkMode = modemNetworkMode;

                        if (DBG) { log("handleGetPreferredNetworkTypeResponse: if 2: " +
                                "settingsNetworkMode = " + settingsNetworkMode);
                        }

                        //changes the Settings.System accordingly to modemNetworkMode
                        android.provider.Settings.Global.putInt(
                                mPhone.getContext().getContentResolver(),
                                android.provider.Settings.Global.PREFERRED_NETWORK_MODE,
                                settingsNetworkMode );
                    }
                    int acq = getAcqValue();
                    if (mButtonPreferredNetworkMode != null) {
                        UpdatePreferredNetworkModeSummary(modemNetworkMode, acq);
                        // changes the mButtonPreferredNetworkMode accordingly to modemNetworkMode
                        setPreferredNetworkModeValue(modemNetworkMode);
                    } else if (mButtonEnabledNetworks != null) {
                        UpdateEnabledNetworksValueAndSummary(modemNetworkMode);
                        // changes the mButtonEnabledNetworks accordingly to modemNetworkMode
                        mButtonEnabledNetworks.setValue(Integer.toString(modemNetworkMode));
                    }
                } else {
                    if (DBG) log("handleGetPreferredNetworkTypeResponse: else: reset to default");
                    resetNetworkModeToDefault();
                }
                updateButtonPreferredLte();
            }
        }

        private void handleSetPreferredNetworkTypeResponse(Message msg) {
            AsyncResult ar = (AsyncResult) msg.obj;
            int networkMode;

            if (ar.exception == null) {
                if (mButtonPreferredNetworkMode != null) {
                    networkMode = Integer.valueOf(
                            mButtonPreferredNetworkMode.getValue()).intValue();
                    android.provider.Settings.Global.putInt(mPhone.getContext()
                            .getContentResolver(),
                            android.provider.Settings.Global.PREFERRED_NETWORK_MODE, networkMode);
                } else if (mButtonEnabledNetworks != null) {
                    networkMode = Integer.valueOf(
                            mButtonEnabledNetworks.getValue()).intValue();
                    android.provider.Settings.Global.putInt(mPhone.getContext()
                            .getContentResolver(),
                            android.provider.Settings.Global.PREFERRED_NETWORK_MODE, networkMode);
                }
                updateButtonPreferredLte();
            } else {
                mPhone.getPreferredNetworkType(obtainMessage(MESSAGE_GET_PREFERRED_NETWORK_TYPE));
            }
        }

        private void resetNetworkModeToDefault() {			
			log(CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::+++++++++++++++");
			
            //set the mButtonPreferredNetworkMode
            setPreferredNetworkModeValue(preferredNetworkMode);
            mButtonEnabledNetworks.setValue(Integer.toString(preferredNetworkMode));
            //set the Settings.System
            android.provider.Settings.Global.putInt(mPhone.getContext().getContentResolver(),
                        android.provider.Settings.Global.PREFERRED_NETWORK_MODE,
                        preferredNetworkMode );
            //Set the Modem
            mPhone.setPreferredNetworkType(preferredNetworkMode,
                    this.obtainMessage(MyHandler.MESSAGE_SET_PREFERRED_NETWORK_TYPE));

			
			log(CommonFunction._FILE_() + ":::" + CommonFunction._FUNC_() + "::---------------");
        }
    }

    private void UpdatePreferredNetworkModeSummary(int NetworkMode, int acq) {
        int networkFeature = SystemProperties.getInt("persist.radio.network_feature",
                Constants.NETWORK_MODE_DEFAULT);
        switch(NetworkMode) {
            case Phone.NT_MODE_WCDMA_PREF:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_wcdma_perf_summary);
                break;
            case Phone.NT_MODE_GSM_ONLY:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_gsm_only_summary);
                break;
            case Phone.NT_MODE_WCDMA_ONLY:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_wcdma_only_summary);
                break;
            case Phone.NT_MODE_GSM_UMTS:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_gsm_wcdma_summary);
                break;
            case Phone.NT_MODE_CDMA:
                switch (mPhone.getLteOnCdmaMode()) {
                    case PhoneConstants.LTE_ON_CDMA_TRUE:
                        mButtonPreferredNetworkMode.setSummary(
                            R.string.preferred_network_mode_cdma_summary);
                    break;
                    case PhoneConstants.LTE_ON_CDMA_FALSE:
                    default:
                        mButtonPreferredNetworkMode.setSummary(
                            R.string.preferred_network_mode_cdma_evdo_summary);
                        break;
                }
                break;
            case Phone.NT_MODE_CDMA_NO_EVDO:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_cdma_only_summary);
                break;
            case Phone.NT_MODE_EVDO_NO_CDMA:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_evdo_only_summary);
                break;
            case Phone.NT_MODE_LTE_ONLY:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_lte_summary);
                break;
            case Phone.NT_MODE_LTE_GSM_WCDMA:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_lte_gsm_wcdma_summary);
                break;
            case Phone.NT_MODE_LTE_CDMA_AND_EVDO:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_lte_cdma_evdo_summary);
                break;
            case Phone.NT_MODE_LTE_CMDA_EVDO_GSM_WCDMA:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_global_summary);
                break;
            case Phone.NT_MODE_GLOBAL:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_cdma_evdo_gsm_wcdma_summary);
                break;
            case Phone.NT_MODE_LTE_WCDMA:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_lte_wcdma_summary);
                break;
            case Phone.NT_MODE_TD_SCDMA_ONLY:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_td_scdma_only_summary);
                break;
            case Phone.NT_MODE_TD_SCDMA_WCDMA:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_td_scdma_wcdma_summary);
                break;
            case Phone.NT_MODE_TD_SCDMA_LTE:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_td_scdma_lte_summary);
                break;
            case Phone.NT_MODE_TD_SCDMA_GSM:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_td_scdma_gsm_summary);
                break;
            case Phone.NT_MODE_TD_SCDMA_GSM_LTE:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_td_scdma_gsm_lte_summary);
                break;
            case Phone.NT_MODE_TD_SCDMA_GSM_WCDMA:
                if (networkFeature == Constants.NETWORK_MODE_CMCC) {
                    mButtonPreferredNetworkMode.setSummary(
                            R.string.preferred_network_mode_3g_2g_auto_summary);
                } else if (networkFeature == Constants.NETWORK_MODE_LTE) {
                    mButtonPreferredNetworkMode.setSummary(
                            R.string.preferred_network_mode_4g_3g_2g_td);
                } else {
                    mButtonPreferredNetworkMode.setSummary(
                            R.string.preferred_network_mode_td_scdma_gsm_wcdma_summary);
                }
                break;
            case Phone.NT_MODE_TD_SCDMA_WCDMA_LTE:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_td_scdma_wcdma_lte_summary);
                break;
            case Phone.NT_MODE_TD_SCDMA_GSM_WCDMA_LTE:
                if (networkFeature == Constants.NETWORK_MODE_CMCC) {
                    if (getResources().getBoolean(R.bool.config_network_cmcc_feature)) {
                         mButtonPreferredNetworkMode.setSummary(
                                 R.string.preferred_network_mode_4g_3g_2g_auto_summary);
                    } else {
                        if (acq == 1) {
                            mButtonPreferredNetworkMode
                                    .setSummary(R.string.preferred_network_mode_4g_3g_2g_4g);
                        } else if (acq == 2) {
                            mButtonPreferredNetworkMode
                                    .setSummary(R.string.preferred_network_mode_4g_3g_2g_3g);
                        } else {
                            mButtonPreferredNetworkMode.setSummary(
                                    R.string.preferred_network_mode_4g_3g_2g_auto_summary);
                        }
                    }
                } else if (networkFeature == Constants.NETWORK_MODE_LTE){
                    if (acq == 1) {
                        mButtonPreferredNetworkMode
                                .setSummary(R.string.preferred_network_mode_4g_3g_2g_lte);
                    } else {
                        mButtonPreferredNetworkMode.setSummary(
                                R.string.preferred_network_mode_td_scdma_gsm_wcdma_lte_summary);
                    }
                } else {
                    mButtonPreferredNetworkMode.setSummary(
                            R.string.preferred_network_mode_td_scdma_gsm_wcdma_lte_summary);
                }
                break;
            case Phone.NT_MODE_TD_SCDMA_CDMA_EVDO_GSM_WCDMA:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_td_scdma_cdma_evdo_gsm_wcdma_summary);
                break;
            case Phone.NT_MODE_TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_td_scdma_lte_cdma_evdo_gsm_wcdma_summary);
                break;
            case Phone.NT_MODE_LTE_CDMA_EVDO_GSM:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_lte_cdma_evdo_gsm_summary);
                break;
            default:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_global_summary);
        }
    }

    private void UpdateEnabledNetworksValueAndSummary(int NetworkMode) {
        switch (NetworkMode) {
            case Phone.NT_MODE_WCDMA_ONLY:
            case Phone.NT_MODE_GSM_UMTS:
            case Phone.NT_MODE_WCDMA_PREF:
                if (!mIsGlobalCdma) {
                    mButtonEnabledNetworks.setValue(
                            Integer.toString(Phone.NT_MODE_WCDMA_PREF));
                    mButtonEnabledNetworks.setSummary(R.string.network_3G);
                } else {
                    mButtonEnabledNetworks.setValue(
                            Integer.toString(Phone.NT_MODE_LTE_CMDA_EVDO_GSM_WCDMA));
                    mButtonEnabledNetworks.setSummary(R.string.network_global);
                }
                break;
            case Phone.NT_MODE_GSM_ONLY:
                if (!mIsGlobalCdma) {
                    mButtonEnabledNetworks.setValue(
                            Integer.toString(Phone.NT_MODE_GSM_ONLY));
                    mButtonEnabledNetworks.setSummary(R.string.network_2G);
                } else {
                    mButtonEnabledNetworks.setValue(
                            Integer.toString(Phone.NT_MODE_LTE_CMDA_EVDO_GSM_WCDMA));
                    mButtonEnabledNetworks.setSummary(R.string.network_global);
                }
                break;
            case Phone.NT_MODE_LTE_GSM_WCDMA:
            case Phone.NT_MODE_LTE_ONLY:
            case Phone.NT_MODE_LTE_WCDMA:
                if (!mIsGlobalCdma) {
                    mButtonEnabledNetworks.setValue(
                            Integer.toString(Phone.NT_MODE_LTE_GSM_WCDMA));
                    mButtonEnabledNetworks.setSummary((mShow4GForLTE == true)
                            ? R.string.network_4G : R.string.network_lte);
                } else {
                    mButtonEnabledNetworks.setValue(
                            Integer.toString(Phone.NT_MODE_LTE_CMDA_EVDO_GSM_WCDMA));
                    mButtonEnabledNetworks.setSummary(R.string.network_global);
                }
                break;
            case Phone.NT_MODE_LTE_CDMA_AND_EVDO:
                mButtonEnabledNetworks.setValue(
                        Integer.toString(Phone.NT_MODE_LTE_CDMA_AND_EVDO));
                mButtonEnabledNetworks.setSummary(R.string.network_lte);
                break;
            case Phone.NT_MODE_CDMA:
            case Phone.NT_MODE_EVDO_NO_CDMA:
            case Phone.NT_MODE_GLOBAL:
                mButtonEnabledNetworks.setValue(
                        Integer.toString(Phone.NT_MODE_CDMA));
                mButtonEnabledNetworks.setSummary(R.string.network_3G);
                break;
            case Phone.NT_MODE_CDMA_NO_EVDO:
                mButtonEnabledNetworks.setValue(
                        Integer.toString(Phone.NT_MODE_CDMA_NO_EVDO));
                mButtonEnabledNetworks.setSummary(R.string.network_1x);
                break;
            case Phone.NT_MODE_LTE_CMDA_EVDO_GSM_WCDMA:
                mButtonEnabledNetworks.setValue(
                        Integer.toString(Phone.NT_MODE_LTE_CMDA_EVDO_GSM_WCDMA));
                mButtonEnabledNetworks.setSummary(R.string.network_global);
                break;
            case Phone.NT_MODE_LTE_CDMA_EVDO_GSM:
                mButtonEnabledNetworks.setValue(
                            Integer.toString(Phone.NT_MODE_LTE_CDMA_EVDO_GSM));
                mButtonEnabledNetworks.setSummary(R.string.network_lte);
                break;
            default:
                String errMsg = "Invalid Network Mode (" + NetworkMode + "). Ignore.";
                loge(errMsg);
                mButtonEnabledNetworks.setSummary(errMsg);
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        switch(requestCode) {
        case REQUEST_CODE_EXIT_ECM:
            Boolean isChoiceYes =
                data.getBooleanExtra(EmergencyCallbackModeExitDialog.EXTRA_EXIT_ECM_RESULT, false);
            if (isChoiceYes) {
                // If the phone exits from ECM mode, show the CDMA Options
                mCdmaOptions.showDialog(mClickedPreference);
            } else {
                // do nothing
            }
            break;

        default:
            break;
        }
    }

    private static void log(String msg) {
        Log.d(LOG_TAG, msg);
    }

    private static void loge(String msg) {
        Log.e(LOG_TAG, msg);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        final int itemId = item.getItemId();
        if (itemId == android.R.id.home) {  // See ActionBar#setDisplayHomeAsUpEnabled()
            // Commenting out "logical up" capability. This is a workaround for issue 5278083.
            //
            // Settings app may not launch this activity via UP_ACTIVITY_CLASS but the other
            // Activity that looks exactly same as UP_ACTIVITY_CLASS ("SubSettings" Activity).
            // At that moment, this Activity launches UP_ACTIVITY_CLASS on top of the Activity.
            // which confuses users.
            // TODO: introduce better mechanism for "up" capability here.
            /*Intent intent = new Intent(Intent.ACTION_MAIN);
            intent.setClassName(UP_ACTIVITY_PACKAGE, UP_ACTIVITY_CLASS);
            intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
            startActivity(intent);*/
            finish();
            return true;
        }
        return super.onOptionsItemSelected(item);
    }
}
