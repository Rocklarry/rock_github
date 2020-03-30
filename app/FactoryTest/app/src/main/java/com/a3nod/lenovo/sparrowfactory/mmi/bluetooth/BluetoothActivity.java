package com.a3nod.lenovo.sparrowfactory.mmi.bluetooth;

import android.Manifest;
import android.app.Activity;
import android.app.ProgressDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothServerSocket;
import android.bluetooth.BluetoothSocket;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.os.AsyncTask;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.CrashHandler;
import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.interact.ConnectManager;
import com.a3nod.lenovo.sparrowfactory.interact.Protocol;
import com.a3nod.lenovo.sparrowfactory.mmi.MMITestProcessManager;
import com.a3nod.lenovo.sparrowfactory.mmi.result.ASSYEntity;
import com.a3nod.lenovo.sparrowfactory.pcba.PCBAHandler;
import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;
import com.a3nod.lenovo.sparrowfactory.tool.SystemInfoTools;

import java.io.IOException;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

/**
 * Created by Lenovo on 2017/11/10.
 */

public class BluetoothActivity extends Activity
{
    public static final String TAG = "MMI_BLUETOOTH_TEST";
    private BluetoothAdapter bluetoothAdapter;
    private final String My_UUID = "00001101-0000-1000-8000-00805F9B34FB";
    private List<BTDevice> mList = new ArrayList<>();
    private BltoothAdapter adtDevices;
    private ListView btlist;
    private TextView tvStatus;
    private Button btn_pass;
    private PCBAHandler handler;
    private TextView tv_local_address;
    private boolean isListening = false;
    private ProgressDialog dialog;
    int testModel = Constant.INTENT_VALUE_TEST_MODEL_PCBA;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.bt_activity);
        testModel = getIntent().getIntExtra(Constant.INTENT_KEY_TEST_MODEL, Constant.INTENT_VALUE_TEST_MODEL_PCBA);
        if (testModel == Constant.INTENT_VALUE_TEST_MODEL_PCBA)
        {
            CrashHandler.getInstance().setCurrentTag(TAG, CrashHandler.TYPE_PCBA);

        } else
        {
            CrashHandler.getInstance().setCurrentTag(TAG, CrashHandler.TYPE_MMI);
        }
        handler = new PCBAHandler(this);
        initTitle();
        init();
        initConfirmButton();

        bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (bluetoothAdapter == null)
        {
            Toast.makeText(this, "Bluetooth Device disable", Toast.LENGTH_SHORT).show();
            tv_local_address.setText(" Bluetooth Device disable");
            finish();
        }
        tv_local_address.setText("local device " + "\n name : " + bluetoothAdapter.getName() + "\n address : " + SystemInfoTools.getBtAddressViaReflection());

        if (!bluetoothAdapter.isEnabled())
        {
            LogUtil.i("aaaaaaaaaaaaaaa111111111");
            bluetoothAdapter.enable();
        } else
        {
            LogUtil.i("aaaaaaaaaaaaaaa2222222222");
            permissionForM();
        }

        adtDevices = new BltoothAdapter(mList, this);
        btlist.setAdapter(adtDevices);
        btlist.setOnItemClickListener(new AdapterView.OnItemClickListener()
        {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l)
            {
                bluetoothAdapter.cancelDiscovery();
                BTDevice device = mList.get(i);
                LogUtil.i("device isBound" + device.isBound);
                if (!device.isBound)
                {
                    LogUtil.i("1111111111111111111");
                    new PairTask(device).start();
                }
            }
        });
        btlist.setOnItemLongClickListener(new AdapterView.OnItemLongClickListener()
        {
            @Override
            public boolean onItemLongClick(AdapterView<?> parent, View view, int position, long id)
            {
                BTDevice device = mList.get(position);
                BluetoothDevice btDev = bluetoothAdapter.getRemoteDevice(device.address);
                if (device.isBound)
                {
                    unpairDevice(btDev);
                    device.isBound = false;
                    adtDevices.notifyDataSetChanged();
                    Toast.makeText(BluetoothActivity.this, "cancel pair", Toast.LENGTH_SHORT).show();
                }
                return true;
            }
        });

        IntentFilter mFilter = new IntentFilter(BluetoothDevice.ACTION_FOUND);
        mFilter.addAction(BluetoothAdapter.ACTION_DISCOVERY_FINISHED);
        mFilter.addAction(BluetoothDevice.ACTION_BOND_STATE_CHANGED);
        mFilter.addAction(BluetoothAdapter.ACTION_STATE_CHANGED);
        registerReceiver(mReceiver, mFilter);
    }

    public void initTitle()
    {
        TextView tv_title = findViewById(R.id.tv_menu_title);
        tv_title.setText(R.string.bluetooth);
        ImageView iv_back = findViewById(R.id.iv_menu_back);
        iv_back.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                finish();
            }
        });
        iv_back.setVisibility(View.GONE);
    }

    public void unpairDevice(BluetoothDevice device)
    {
        try
        {
            Method m = device.getClass().getMethod("removeBond", (Class[]) null);
            m.invoke(device, (Object[]) null);
        } catch (Exception e)
        {
            e.printStackTrace();
        }
    }

    private void init()
    {
        tv_local_address = findViewById(R.id.tv_local_address);
        btlist = findViewById(R.id.btlist);
        tvStatus = findViewById(R.id.tv_status);

        //        new CreateBluetoothServer().execute();
    }

    public void initConfirmButton()
    {
        btn_pass = findViewById(R.id.btn_result_pass);
        btn_pass.setEnabled(false);
        btn_pass.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                handler = null;
                if (bluetoothAdapter != null && bluetoothAdapter.isDiscovering())
                {
                    bluetoothAdapter.cancelDiscovery();
                }
                if (testModel == Constant.INTENT_VALUE_TEST_MODEL_PCBA)
                {
                    ConnectManager.getInstance().setPCBAResult(Protocol.PCBA_TEST_ITEM_BLUETOOTH, true, "007");
                } else if (testModel == Constant.INTENT_VALUE_TEST_MODEL_MMI)
                {
                    ASSYEntity.getInstants().setBTTestResult(true);
                    MMITestProcessManager.getInstance().toNextTest();
                }
                finish();

            }
        });
        Button btn_reset = findViewById(R.id.btn_result_reset);
        btn_reset.setVisibility(View.GONE);
        Button btn_fail = findViewById(R.id.btn_result_fail);
        btn_fail.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                if (bluetoothAdapter != null && bluetoothAdapter.isDiscovering())
                {
                    bluetoothAdapter.cancelDiscovery();
                }

                if (testModel == Constant.INTENT_VALUE_TEST_MODEL_PCBA)
                {
                    ConnectManager.getInstance().setPCBAResult(Protocol.PCBA_TEST_ITEM_BLUETOOTH, false, "007");

                } else if (testModel == Constant.INTENT_VALUE_TEST_MODEL_MMI)
                {
                    ASSYEntity.getInstants().setBTTestResult(false);
                    MMITestProcessManager.getInstance().testFail();
                }
                finish();
            }
        });

    }

    public class PairTask extends Thread
    {
        BTDevice device;

        public PairTask(BTDevice device)
        {
            LogUtil.i("2222222222222222");
            this.device = device;
            dialog = ProgressDialog.show(BluetoothActivity.this, "bluetooth", "start pairing...", false, true);
            LogUtil.i("3333333333333333");
        }

        @Override
        public void run()
        {
            //利用反射方法调用BluetoothDevice.createBond(BluetoothDevice remoteDevice);

            //            Method createBondMethod = null;
            //            try
            //            {
            //                createBondMethod = BluetoothDevice.class.getMethod("createBond");
            //                Log.d("BlueToothTestActivity", "开始配对");
            //                returnValue = (Boolean) createBondMethod.invoke(btDev);
            //
            //
            //
            //            } catch (NoSuchMethodException e)
            //            {
            //                e.printStackTrace();
            //            } catch (IllegalAccessException e)
            //            {
            //                e.printStackTrace();
            //            } catch (InvocationTargetException e)
            //            {
            //                e.printStackTrace();
            //            }

            LogUtil.i("444444444444444444");
            BluetoothDevice bluetoothDevice = bluetoothAdapter.getRemoteDevice(device.address);
            bluetoothDevice.createBond();
            LogUtil.i("55555555555555555555");

        }

    }


    private class ConnectTask extends AsyncTask<Object, Object, Boolean>
    {
        private BluetoothSocket socket = null;
        BTDevice device;

        private ConnectTask(BTDevice device)
        {
            this.device = device;
            dialog = ProgressDialog.show(BluetoothActivity.this, "bluetooth", "start connecting...", false, false);
        }

        @Override
        protected Boolean doInBackground(Object... objects)
        {
            BluetoothDevice btDev = bluetoothAdapter.getRemoteDevice(device.address);
            try
            {
                socket = btDev.createRfcommSocketToServiceRecord(UUID.fromString(My_UUID));
            } catch (Exception e)
            {
                Log.e("", "Error creating socket");
            }

            try
            {
                socket.connect();
                Log.e("", "Connected");
            } catch (IOException e)
            {
                Log.e("", e.getMessage());
                try
                {
                    Log.e("", "trying fallback...");

                    socket = (BluetoothSocket) device.getClass().getMethod("createRfcommSocket", new Class[]{
                            int.class
                    }).invoke(device, 1);
                    socket.connect();
                    Log.e("", "Connected");
                    return true;
                } catch (Exception e2)
                {
                    Log.e("", "Couldn't establish Bluetooth connection!");
                }
            }

            return false;
        }


        @Override
        protected void onPostExecute(Boolean isConnect)
        {
            super.onPostExecute(isConnect);
            dialog.dismiss();

            tvStatus.setText("connect " + (isConnect ? "success" : "fail"));
        }
    }

    private class CreateBluetoothServer extends AsyncTask<Object, Object, Boolean>
    {
        BluetoothServerSocket bluetoothServerSocket = null;

        public CreateBluetoothServer()
        {
            isListening = true;
            try
            {
                bluetoothServerSocket = bluetoothAdapter.listenUsingRfcommWithServiceRecord("com.bluetooth.demo", UUID.fromString(My_UUID));
            } catch (IOException e)
            {
                e.printStackTrace();
            }
        }

        @Override
        protected Boolean doInBackground(Object... objects)
        {
            //服务器端的bltsocket需要传入uuid和一个独立存在的字符串，以便验证，通常使用包名的形式

            while (isListening && bluetoothServerSocket != null)
            {
                try
                {
                    //注意，当accept()返回BluetoothSocket时，socket已经连接了，因此不应该调用connect方法。
                    //这里会线程阻塞，直到有蓝牙设备链接进来才会往下走
                    BluetoothSocket socket = bluetoothServerSocket.accept();
                    if (socket != null)
                    {
                        //                        bluetoothSocket = socket;
                        //回调结果通知
                        //如果你的蓝牙设备只是一对一的连接，则执行以下代码
                        bluetoothServerSocket.close();
                        return true;
                        //如果你的蓝牙设备是一对多的，则应该调用break；跳出循环
                        //break;
                    }
                } catch (IOException e)
                {
                    try
                    {
                        bluetoothServerSocket.close();
                    } catch (IOException e1)
                    {
                        e1.printStackTrace();
                    }
                    break;
                }
            }
            return false;
        }

        public void closeServer()
        {
            isListening = false;
            try
            {
                bluetoothServerSocket.close();
                bluetoothServerSocket = null;
            } catch (IOException e1)
            {
                e1.printStackTrace();
            }
        }

        @Override
        protected void onPostExecute(Boolean aBoolean)
        {
            super.onPostExecute(aBoolean);
        }
    }

    @Override
    protected void onResume()
    {
        super.onResume();
    }

    @Override
    protected void onPause()
    {
        super.onPause();
    }

    @Override
    protected void onDestroy()
    {
        super.onDestroy();
        unregisterReceiver(mReceiver);
        if (bluetoothAdapter != null && bluetoothAdapter.isDiscovering())
        {
            bluetoothAdapter.cancelDiscovery();
        }
        /*if (bluetoothAdapter != null)
        {
            bluetoothAdapter.disable();
        }*/
        handler = null;
    }

    private void StartDiscover()
    {
        if (bluetoothAdapter != null)
        {
            mList.clear();
            bluetoothAdapter.startDiscovery();
        }
    }


    //    private Button Bn_Pass,Bn_fail;

    BroadcastReceiver mReceiver = new BroadcastReceiver()
    {
        @Override
        public void onReceive(Context context, Intent intent)
        {
            String action = intent.getAction();
            Log.v("btmsg", action);
            if (BluetoothDevice.ACTION_FOUND.equals(action))
            {
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                //信号强度。
                short rssi = intent.getExtras().getShort(BluetoothDevice.EXTRA_RSSI);
                BTDevice btDevice = new BTDevice();
                btDevice.address = device.getAddress();
                btDevice.name = device.getName();
                btDevice.isBound = (device.getBondState() == BluetoothDevice.BOND_BONDED);
                btDevice.rssi = rssi;
                if (!TextUtils.isEmpty(btDevice.name))
                {
                    mList.add(btDevice);
                    if (Math.abs(rssi)<50&&!Constant.TEST_TYPE_MMI_AUDO){
                        btn_pass.setEnabled(true);
                    }
                }
                adtDevices.notifyDataSetChanged();
                if (mList.size() >= 3)
                {
//                    btn_pass.setEnabled(true);
                    if (testModel == Constant.INTENT_VALUE_TEST_MODEL_MMI && !MMITestProcessManager.getInstance().isTesting())
                    {
                        return;
                    }
                    if (handler != null)
                    {
                        handler.sendEmptyMessageDelayed(PCBAHandler.MSG_TEST_SUCCESS, PCBAHandler.DELAY);
                    }
                }

            } else if (action.equals(BluetoothAdapter.ACTION_DISCOVERY_FINISHED))
            {
                //                setProgressBarIndeterminateVisibility(false);
                tvStatus.setText("BlueTooth search finish");
                if (handler == null)
                {
                    return;
                }
                if (testModel == Constant.INTENT_VALUE_TEST_MODEL_MMI && !MMITestProcessManager.getInstance().isTesting())
                {
                    return;
                }

                if (mList.size() == 0)
                {
                    handler.sendEmptyMessageDelayed(PCBAHandler.MSG_TEST_FAIL, PCBAHandler.DELAY);
                } else
                {
                    handler.sendEmptyMessageDelayed(PCBAHandler.MSG_TEST_SUCCESS, PCBAHandler.DELAY);
                }

            } else if (action.equals(BluetoothDevice.ACTION_BOND_STATE_CHANGED))
            {
                if (dialog != null)
                {
                    dialog.dismiss();
                }
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                String name = device.getName();
                Log.d("aaa", "device name: " + name);
                int state = intent.getIntExtra(BluetoothDevice.EXTRA_BOND_STATE, -1);
                switch (state)
                {
                    case BluetoothDevice.BOND_NONE:
                        Log.d("aaa", "BOND_NONE 删除配对");
                        Toast.makeText(BluetoothActivity.this, "unbound : " + name, Toast.LENGTH_SHORT).show();
                        break;
                    case BluetoothDevice.BOND_BONDING:
                        Toast.makeText(BluetoothActivity.this, "pairing : " + name, Toast.LENGTH_SHORT).show();
                        Log.d("aaa", "BOND_BONDING 正在配对");
                        break;
                    case BluetoothDevice.BOND_BONDED:
                        Toast.makeText(BluetoothActivity.this, "paired success : " + name, Toast.LENGTH_SHORT).show();
                        Log.d("aaa", "BOND_BONDED 配对成功");
                        break;
                }
                updateList(device);
            } else if (action.equals(BluetoothAdapter.ACTION_STATE_CHANGED))
            {
                int blueState = intent.getIntExtra(BluetoothAdapter.EXTRA_STATE, 0);
                Log.d("qiugang", "blueState = " + (blueState == BluetoothAdapter.STATE_ON));
                if (blueState == BluetoothAdapter.STATE_ON)
                {
                    StartDiscover();
                    if (tv_local_address != null)
                    {
                        tv_local_address.setText("local device " + "\n name : " + bluetoothAdapter.getName() + "\n address : " + SystemInfoTools.getBtAddressViaReflection());
                    }
                }
            }
        }
    };

    public void updateList(BluetoothDevice device)
    {
        for (int i = 0; i < mList.size(); i++)
        {
            if (mList.get(i).address.equals(device.getAddress()))
            {
                mList.get(i).isBound = (device.getBondState() == BluetoothDevice.BOND_BONDED);
                break;
            }
        }

        adtDevices.notifyDataSetChanged();
    }


    private void permissionForM()
    {
        if (checkSelfPermission(Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED || checkSelfPermission(Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED)
        {
            LogUtil.i("aaaaaaaaaaaaaaa33333333333");
            requestPermissions(new String[]{
                    Manifest.permission.ACCESS_FINE_LOCATION,
                    Manifest.permission.ACCESS_COARSE_LOCATION
            }, 1);
        } else
        {
            LogUtil.i("aaaaaaaaaaaaaaa444444444444");
            Intent discoverableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_DISCOVERABLE);
            discoverableIntent.putExtra(BluetoothAdapter.EXTRA_DISCOVERABLE_DURATION, 300);
            startActivity(discoverableIntent);

            StartDiscover();
            Log.e("TAG", "权限已经授予");
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data)
    {
/*        IntentFilter mFilter = new IntentFilter(BluetoothDevice.ACTION_FOUND);
        registerReceiver(mReceiver, mFilter);
        // 注册搜索完时的receiver
        mFilter = new IntentFilter(BluetoothAdapter.ACTION_DISCOVERY_FINISHED);
        registerReceiver(mReceiver, mFilter);*/
        LogUtil.i("aaaaaaaaaaaaaaa6666666");
        permissionForM();
        Log.v("btmsg", "OnCreate");
        super.onActivityResult(requestCode, resultCode, data);
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults)
    {
        LogUtil.i("aaaaaaaaaaaaaaa7777777");
        if (requestCode == 1)
        {
            LogUtil.i("aaaaaaaaaaaaaaa88888888888");
            if (grantResults[0] == PackageManager.PERMISSION_GRANTED)
            {
                LogUtil.i("aaaaaaaaaaaaaaa999999999999");
                StartDiscover();
                Log.e("TAG", "授予权限");
            }
        }
        // super.onRequestPermissionsResult(requestCode, permissions, grantResults);
    }
}
