package com.learn.app_test.test;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

import com.learn.app_test.R;

import java.net.DatagramPacket;
import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.MulticastSocket;
import java.net.NetworkInterface;
import java.util.Enumeration;

public class Multi_Activity extends AppCompatActivity {
    EditText textView_msg;
    Button button_send;

    private static final int MULTI_PORT = 7838;
    private static final String MULTI_IP = "230.0.0.1";

    private MulticastSocket socket = null;
    private InetAddress inetAddress = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate ( savedInstanceState );
        setContentView ( R.layout.activity_multi_ );
        textView_msg = findViewById ( R.id.multi_msg );
        button_send = findViewById ( R.id.multi_send );


        try {
            socket = new MulticastSocket (MULTI_PORT);
            inetAddress = InetAddress.getByName (MULTI_IP);
            socket.setTimeToLive(1);
            socket.joinGroup ( inetAddress );


        } catch (Exception e) {
            e.printStackTrace ( );
        }
        button_send.setOnClickListener ( new View.OnClickListener ( ) {
            @Override
            public void onClick(View view) {
                sendMultiMessage ( );
            }
        } );

    }


    private void sendMultiMessage() {
        new Thread ( new Runnable ( ) {
            public void run() {

                try {
                    String sendData = textView_msg.getText ( ).toString ( )+" IP:"+getIpAddressString()+" PORT:"+MULTI_PORT;
                    Log.d ( "mulit send test", sendData + inetAddress + socket+getIpAddressString() );
                    DatagramPacket packet = new DatagramPacket ( new byte[0], 0, inetAddress, MULTI_PORT );
                    byte data[] = sendData.getBytes( );
                    packet.setData ( data );
                    socket.send (packet );
                    Thread.sleep(3000);
                } catch (Exception e) {
                    e.printStackTrace ( );
                }
            }
        } ).start ( );
    }


    public static String getIpAddressString() {
        try {
            for (Enumeration<NetworkInterface> enNetI = NetworkInterface.getNetworkInterfaces(); enNetI.hasMoreElements(); ) {
                NetworkInterface netI = enNetI.nextElement();
                for (Enumeration<InetAddress> enumIpAddr = netI
                        .getInetAddresses(); enumIpAddr.hasMoreElements(); ) {
                    InetAddress inetAddress = enumIpAddr.nextElement();
                    if (inetAddress instanceof Inet4Address && !inetAddress.isLoopbackAddress()) {
                        return inetAddress.getHostAddress();
                    }
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return "0.0.0.0";
    }


    protected void onDestroy()
    {
        super.onDestroy ();
        socket.close ();
        finish ();
    }

}