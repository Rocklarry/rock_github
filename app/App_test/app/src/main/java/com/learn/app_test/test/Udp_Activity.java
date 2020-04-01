package com.learn.app_test.test;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.learn.app_test.R;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;

public class Udp_Activity extends AppCompatActivity {

    EditText editText_ip,editText_port,textView_msg;
    Button button_send;
    private DatagramSocket socket=null;
    private InetAddress inetAddress=null;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate ( savedInstanceState );
        setContentView ( R.layout.activity_udp_ );

        editText_ip = findViewById ( R.id.editText_ip );
        editText_port=findViewById ( R.id.editText_port );
        textView_msg = findViewById ( R.id.textView_msg );
        button_send = findViewById ( R.id.button_send);


        try {
            socket = new DatagramSocket (Integer.parseInt ( editText_port.getText ().toString ()));//把port转换成int整形
            inetAddress = InetAddress.getByName ( editText_ip.getText ().toString () );
        } catch (Exception e) {
            e.printStackTrace ( );
        }
        button_send.setOnClickListener ( new View.OnClickListener ( ) {
            @Override
            public void onClick(View view) {
                sendMessage ();
            }
        } );

    }


    private void sendMessage() {
        new Thread (new Runnable (){
            public void run(){

                try {
                    String sendData = textView_msg.getText ().toString ();
                    byte data[]=sendData.getBytes ();
                    Log.d("udp send test",sendData+inetAddress+socket);
                    DatagramPacket packet = new DatagramPacket ( data,data.length,inetAddress,8880 );
                    socket.send ( packet );
                } catch (Exception e) {
                    e.printStackTrace ( );
                }
            }
        }).start ();
    }
}
